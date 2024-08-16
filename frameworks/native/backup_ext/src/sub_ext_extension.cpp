/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ext_extension.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <map>
#include <regex>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <directory_ex.h>
#include <sys/stat.h>
#include <unistd.h>

#include <directory_ex.h>
#include <unique_fd.h>

#include "accesstoken_kit.h"
#include "bundle_mgr_client.h"
#include "errors.h"
#include "ipc_skeleton.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_filesystem/b_dir.h"
#include "b_filesystem/b_file.h"
#include "b_filesystem/b_file_hash.h"
#include "b_json/b_json_cached_entity.h"
#include "b_jsonutil/b_jsonutil.h"
#include "b_ohos/startup/backup_para.h"
#include "b_tarball/b_tarball_factory.h"
#include "filemgmt_libhilog.h"
#include "hitrace_meter.h"
#include "i_service.h"
#include "sandbox_helper.h"
#include "service_proxy.h"
#include "tar_file.h"
#include "b_anony/b_anony.h"

namespace OHOS::FileManagement::Backup {
const uint32_t MAX_FD_GROUP_USE_TIME = 1000;          // 每组打开最大时间1000ms

void BackupExtExtension::WaitToSendFd(std::chrono::system_clock::time_point &startTime, int &fdSendNum)
{
    HILOGD("WaitToSendFd Begin");
    std::unique_lock<std::mutex> lock(startSendMutex_);
    startSendFdRateCon_.wait(lock, [this] { return sendRate_ > 0; });
    if (fdSendNum >= sendRate_) {
        HILOGI("current time fd num is max rate, bundle name:%{public}s, rate:%{public}d", bundleName_.c_str(),
            sendRate_);
        auto curTime = std::chrono::system_clock::now();
        auto useTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count();
        if (useTimeMs < MAX_FD_GROUP_USE_TIME) {
            int32_t sleepTime = MAX_FD_GROUP_USE_TIME - useTimeMs;
            HILOGI("will wait time:%{public}d ms", sleepTime);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        } else {
            HILOGW("current fd send num exceeds one second");
        }
        fdSendNum = 0;
        startTime = std::chrono::system_clock::now();
    }
    HILOGD("WaitToSendFd End");
}

void BackupExtExtension::RefreshTimeInfo(std::chrono::system_clock::time_point &startTime, int &fdSendNum)
{
    auto currentTime = std::chrono::system_clock::now();
    auto useTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
    if (useTime >= MAX_FD_GROUP_USE_TIME) {
        HILOGI("RefreshTimeInfo Begin, fdSendNum is:%{public}d", fdSendNum);
        startTime = std::chrono::system_clock::now();
        fdSendNum = 0;
    }
}

void BackupExtExtension::SetClearDataFlag(bool isClearData)
{
    isClearData_ = isClearData;
    HILOGI("set clear data flag:%{public}d", isClearData);
    if (extension_ == nullptr) {
        HILOGE("Extension handle have been released");
        return;
    }
    extension_->SetClearDataFlag(isClearData);
    if (!extension_->WasFromSpecialVersion() && !extension_->RestoreDataReady()) {
        DoClear();
    }
}

std::map<std::string, off_t> BackupExtExtension::GetIdxFileInfos(bool isSpecialVersion)
{
    string restoreDir = isSpecialVersion ? "" :
        string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    auto extManageInfo = GetExtManageInfo();
    std::map<std::string, off_t> idxFileInfos;
    for (size_t i = 0; i < extManageInfo.size(); ++i) {
        std::string realPath = restoreDir + extManageInfo[i].hashName;
        idxFileInfos[realPath] = extManageInfo[i].sta.st_size;
    }
    return idxFileInfos;
}

void BackupExtExtension::CheckTmpDirFileInfos(bool isSpecialVersion)
{
    ErrFileInfo errFiles;
    auto idxFileInfos = GetIdxFileInfos(isSpecialVersion);
    struct stat attr;
    for (auto it : idxFileInfos) {
        if (it.first.size() >= PATH_MAX || stat(it.first.data(), &attr) == -1) {
            HILOGE("(Debug) Failed to get stat of %{public}s, errno = %{public}d",
                GetAnonyPath(it.first).c_str(), errno);
            errFiles[it.first].push_back(errno);
        } else if (it.second != attr.st_size) {
            HILOGE("(Debug) RecFile:%{public}s size err, recSize: %{public}" PRId64 ", idxSize: %{public}" PRId64 "",
                GetAnonyPath(it.first).c_str(), attr.st_size, it.second);
            errFiles[it.first] = std::vector<int>();
        }
    }
    HILOGE("(Debug) Temp file check result: Total file: %{public}zu, err file: %{public}zu",
        idxFileInfos.size(), errFiles.size());
    if (!errFiles.empty()) {
        HILOGE("(Debug) The received file and idx is not same");
    } else {
        HILOGI("(Debug) The received file and idx is same");
    }
}

tuple<bool, vector<string>> BackupExtExtension::CheckRestoreFileInfos()
{
    vector<string> errFiles;
    struct stat curFileStat {};
    for (const auto& it : endFileInfos_) {
        if (lstat(it.first.c_str(), &curFileStat) != 0) {
            HILOGE("(Debug) Failed to lstat, err = %{public}d", errno);
            errFiles.emplace_back(it.first);
            errFileInfos_[it.first].push_back(errno);
        } else if (curFileStat.st_size != it.second) {
            HILOGE("(Debug) File size check error, file: %{public}s", GetAnonyPath(it.first).c_str());
            errFiles.emplace_back(it.first);
            errFileInfos_[it.first].push_back(errno);
        }
    }
    for (const auto& it : errFileInfos_) {
        for (const auto &codeIt : it.second) {
            HILOGE("(Debug)  errfileInfos file = %{public}s -> %{public}d", GetAnonyPath(it.first).c_str(), codeIt);
        }
    }
    HILOGE("(Debug) End file check result Total file: %{public}zu, err file: %{public}zu",
        endFileInfos_.size(), errFileInfos_.size());
    if (errFiles.size()) {
        return {false, errFiles};
    }
    return {true, errFiles};
}











































































































































































































































std::function<void(ErrCode, const std::string)> BackupExtExtension::HandleFullBackupCallbackEx(
    wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get HandleFullBackupCallbackEx");
    return [obj](ErrCode errCode, const std::string backupExRetInfo) {
        HILOGI("begin call backup");
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        extensionPtr->extension_->InvokeAppExtMethod(errCode, backupExRetInfo);
        if (backupExRetInfo.size()) {
            HILOGI("Will notify backup result report");
            extensionPtr->appExecFinished_ = true;
            extensionPtr->ShutDownExecOnProcessTask();
            extensionPtr->AsyncTaskBackup(extensionPtr->extension_->GetUsrConfig());
            extensionPtr->AppResultReport(backupExRetInfo, BackupRestoreScenario::FULL_BACKUP);
        }
    };
}


std::function<void(ErrCode, const std::string)> BackupExtExtension::HandleFullBackupCallback(
    wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get HandleFullBackupCallbackEx");
    return [obj](ErrCode errCode, std::string errMsg) {
        HILOGI("begin call backup");
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        extensionPtr->appExecFinished_ = true;
        extensionPtr->ShutDownExecOnProcessTask();
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        extensionPtr->AsyncTaskBackup(extensionPtr->extension_->GetUsrConfig());
    };
}

std::function<void(ErrCode, std::string)> BackupExtExtension::FullRestoreCallbackEx(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get callbackEx");
    return [obj](ErrCode errCode, const std::string restoreRetInfo) {
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        extensionPtr->extension_->InvokeAppExtMethod(errCode, restoreRetInfo);
        if (errCode == ERR_OK) {
            if (restoreRetInfo.size()) {
                HILOGI("Will notify restore result report");
                extensionPtr->AppResultReport(restoreRetInfo, BackupRestoreScenario::FULL_RESTORE);
            }
            return;
        }
        if (restoreRetInfo.empty()) {
            extensionPtr->AppDone(errCode);
            extensionPtr->DoClear();
        } else {
            std::string errInfo;
            BJsonUtil::BuildRestoreErrInfo(errInfo, errCode, restoreRetInfo);
            extensionPtr->AppResultReport(errInfo, BackupRestoreScenario::FULL_RESTORE, errCode);
            extensionPtr->DoClear();
        }
    };
}

std::function<void(ErrCode, std::string)> BackupExtExtension::FullRestoreCallback(wptr<BackupExtExtension> obj)
{
    return [obj](ErrCode errCode, std::string errMsg) {
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        extensionPtr->appExecFinished_ = true;
        extensionPtr->ShutDownExecOnProcessTask();
        HILOGI("Current bundle will execute app done");
        if (errMsg.empty()) {
            extensionPtr->AppDone(errCode);
        } else {
            std::string errInfo;
            BJsonUtil::BuildRestoreErrInfo(errInfo, errCode, errMsg);
            extensionPtr->AppResultReport(errInfo, BackupRestoreScenario::FULL_RESTORE, errCode);
        }
        extensionPtr->DoClear();
    };
}

std::function<void(ErrCode, std::string)> BackupExtExtension::IncRestoreResultCallback(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get callback for onRestore");
    return [obj](ErrCode errCode, std::string errMsg) {
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        extensionPtr->appExecFinished_ = true;
        extensionPtr->ShutDownExecOnProcessTask();
        HILOGI("Current bundle will execute app done");
        if (errMsg.empty()) {
            extensionPtr->AppIncrementalDone(errCode);
        } else {
            std::string errInfo;
            BJsonUtil::BuildRestoreErrInfo(errInfo, errCode, errMsg);
            extensionPtr->AppResultReport(errInfo, BackupRestoreScenario::INCREMENTAL_RESTORE, errCode);
        }
        extensionPtr->DoClear();
    };
}

void BackupExtExtension::ReportAppProcessInfo(const std::string processInfo, BackupRestoreScenario scenario)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Report app process error, proxy is empty");
        return;
    }
    auto ret = proxy->ReportAppProcessInfo(processInfo, scenario);
    if (ret != ERR_OK) {
        HILOGE("Report app process error, ipc failed, ret:%{public}d", ret);
        return;
    }
}

std::function<void(ErrCode, const std::string)> BackupExtExtension::ExecOnProcessCallback(wptr<BackupExtExtension> obj,
        BackupRestoreScenario scenario)
{
    return [obj, scenario](ErrCode errCode, const std::string processInfo) {
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("Execute onProcess callback error, extPtr is empty");
            return;
        }
        extPtr->ShutDownOnProcessTimeoutTask();
        if (extPtr->isSameClock_ && extPtr->execOnProcessTimeoutTimes_ > 0) {
            HILOGE("Current time call onProcess no result, timeOut count:%{public}d",
                extPtr->execOnProcessTimeoutTimes_);
            return;
        }
        extPtr->execOnProcessTimeoutTimes_ = 0; // 正常场景重置超时次数
        if (processInfo.size()) {
            extPtr->ReportAppProcessInfo(processInfo, scenario);
        } else {
            extPtr->ShutDownExecOnProcessTask(); // 认为没实现onProcess,停掉定时器
            return;
        }
    };
}

// std::function<void(ErrCode, const std::string)> BackupExtExtension::ExecOnProcessTimeoutCallback(
//     wptr<BackupExtExtension> obj)
// {

// }

bool BackupExtExtension::CreateExecOnProcessTask(BackupRestoreScenario scenario)
{
    auto taskCallBack = [obj{ wptr<BackupExtExtension>(this) }, scenario]() {
        HILOGI("1111");
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("CreateExecOnProcessTask error, extPtr is empty");
            return;
        }
        extPtr->isSameClock_ = true;
        auto onProcessCallback = extPtr->ExecOnProcessCallback(obj, scenario);
        if (!extPtr->appExecFinished_) { // app没有结束，就继续执行
            extPtr->ExecOnProcessTimeoutTask(obj);
            ErrCode ret = extPtr->extension_->OnProcess(onProcessCallback);
            if (ret != ERR_OK) {
                HILOGE("Execute js method onProcess error, ret:%{public}d", ret);
                return;
            }
        }
        extPtr->isSameClock_ = false;
    };
    execOnProcessTaskTimer_.Setup();
    uint32_t onProcessTimerId = execOnProcessTaskTimer_.Register(taskCallBack,
        BConstants::CALL_APP_ON_PROCESS_TIME_INTERVAL, false);
    execOnProcessTaskTimerId_ = onProcessTimerId;
    return true;
}

bool BackupExtExtension::ShutDownExecOnProcessTask()
{
    HILOGI("Start Shutdown onProcess task timer");
    execOnProcessTaskTimer_.Shutdown();
    execOnProcessTaskTimer_.Unregister(execOnProcessTaskTimerId_);
    HILOGI("End Shutdown onProcess task timer, execOnProcessTaskTimerId: %{public}d", execOnProcessTaskTimerId_);
    return true;
}

bool BackupExtExtension::ExecOnProcessTimeoutTask(wptr<BackupExtExtension> obj)
{
    auto timeoutCallback = [obj]() {
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("CreateExecOnProcessTask error, extPtr is empty");
            return;
        }
        if (extPtr->execOnProcessTimeoutTimes_ >= BConstants::APP_ON_PROCESS_TIMEOUT_MAX_COUNT) {
            HILOGE("Current App timeout more than three times, Will Report");
            extPtr->execOnProcessTimeoutTimes_ = 0; // 已经满足三次超时条件
            extPtr->ShutDownOnProcessTimeoutTask();
            extPtr->ShutDownExecOnProcessTask();
            extPtr->AppDone(BError(BError::Codes::EXT_ABILITY_DIED));
        } else {
            extPtr->execOnProcessTimeoutTimes_++;
        }
    };
    execOnProcessTimeoutTimer_.Setup();
    uint32_t timeoutTimerId = execOnProcessTimeoutTimer_.Register(timeoutCallback,
        BConstants::APP_ON_PROCESS_MAX_TIMEOUT, true);
    execOnProcessTimeoutTimerId_ = timeoutTimerId;
    return true;
}

bool BackupExtExtension::ShutDownOnProcessTimeoutTask()
{
    HILOGI("Start Shutdown onProcess Timeout timer");
    execOnProcessTimeoutTimer_.Shutdown();
    execOnProcessTimeoutTimer_.Unregister(execOnProcessTimeoutTimerId_);
    HILOGI("End Shutdown onProcess Timeout timer, timeoutTimerId: %{public}d", execOnProcessTimeoutTimerId_);
    return true;
}
} // namespace OHOS::FileManagement::Backup
