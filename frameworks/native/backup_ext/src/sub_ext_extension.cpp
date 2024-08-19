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
const uint32_t MAX_FD_GROUP_USE_TIME = 1000; // 每组打开最大时间1000ms

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
            HILOGE("(Debug) Failed to get stat of %{public}s, errno = %{public}d", GetAnonyPath(it.first).c_str(),
                errno);
            errFiles[it.first].push_back(errno);
        } else if (it.second != attr.st_size) {
            HILOGE("(Debug) RecFile:%{public}s size err, recSize: %{public}" PRId64 ", idxSize: %{public}" PRId64 "",
                GetAnonyPath(it.first).c_str(), attr.st_size, it.second);
            errFiles[it.first] = std::vector<int>();
        }
    }
    HILOGE("(Debug) Temp file check result: Total file: %{public}zu, err file: %{public}zu", idxFileInfos.size(),
        errFiles.size());
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
    for (const auto &it : endFileInfos_) {
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
    for (const auto &it : errFileInfos_) {
        for (const auto &codeIt : it.second) {
            HILOGE("(Debug)  errfileInfos file = %{public}s -> %{public}d", GetAnonyPath(it.first).c_str(), codeIt);
        }
    }
    HILOGE("(Debug) End file check result Total file: %{public}zu, err file: %{public}zu", endFileInfos_.size(),
        errFileInfos_.size());
    if (errFiles.size()) {
        return { false, errFiles };
    }
    return { true, errFiles };
}

void BackupExtExtension::AppIncrementalDone(ErrCode errCode)
{
    HILOGI("Begin");
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to obtain the ServiceProxy handle");
        DoClear();
        return;
    }
    auto ret = proxy->AppIncrementalDone(errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed to notify the app done. err = %{public}d", ret);
    }
}

ErrCode BackupExtExtension::GetBackupInfo(std::string &result)
{
    auto obj = wptr<BackupExtExtension>(this);
    auto ptr = obj.promote();
    if (ptr == nullptr) {
        HILOGE("Failed to get ext extension.");
        return BError(BError::Codes::EXT_INVAL_ARG, "extension getBackupInfo exception").GetCode();
    }
    if (ptr->extension_ == nullptr) {
        HILOGE("Failed to get extension.");
        return BError(BError::Codes::EXT_INVAL_ARG, "extension getBackupInfo exception").GetCode();
    }
    auto callBackup = [ptr](ErrCode errCode, const std::string result) {
        if (ptr == nullptr) {
            HILOGE("Failed to get ext extension.");
            return;
        }
        HILOGI("GetBackupInfo callBackup start. result = %{public}s", result.c_str());
        ptr->backupInfo_ = result;
    };
    auto ret = ptr->extension_->GetBackupInfo(callBackup);
    if (ret != ERR_OK) {
        HILOGE("Failed to get backupInfo. err = %{public}d", ret);
        return BError(BError::Codes::EXT_INVAL_ARG, "extension getBackupInfo exception").GetCode();
    }
    HILOGD("backupInfo = %s", backupInfo_.c_str());
    result = backupInfo_;
    backupInfo_.clear();

    return ERR_OK;
}

ErrCode BackupExtExtension::UpdateFdSendRate(std::string &bundleName, int32_t sendRate)
{
    try {
        std::lock_guard<std::mutex> lock(updateSendRateLock_);
        HILOGI("Update SendRate, bundleName:%{public}s, sendRate:%{public}d", bundleName.c_str(), sendRate);
        VerifyCaller();
        bundleName_ = bundleName;
        sendRate_ = sendRate;
        if (sendRate > 0) {
            startSendFdRateCon_.notify_one();
        }
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to UpdateFdSendRate");
        return BError(BError::Codes::EXT_BROKEN_IPC).GetCode();
    }
}

std::function<void(ErrCode, std::string)> BackupExtExtension::OnRestoreCallback(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get full restore onRestore callback");
    return [obj](ErrCode errCode, std::string errMsg) {
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        HILOGI("Current bundle will execute app done");
        extensionPtr->FinishOnProcessTask();
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

std::function<void(ErrCode, std::string)> BackupExtExtension::OnRestoreExCallback(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get full restore onRestoreEx callback");
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
        extensionPtr->FinishOnProcessTask();
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

std::function<void(ErrCode, std::string)> BackupExtExtension::AppDoneCallbackEx(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get callback for appDone");
    return [obj](ErrCode errCode, std::string errMsg) {
        HILOGI("begin call callBackupExAppDone");
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        extensionPtr->AppDone(errCode);
        extensionPtr->DoClear();
    };
}

std::function<void(ErrCode, std::string)> BackupExtExtension::IncreOnRestoreExCallback(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get callback for increment onRestoreEx");
    return [obj](ErrCode errCode, const std::string restoreRetInfo) {
        HILOGI("begin call increment restoreEx");
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        extensionPtr->FinishOnProcessTask();
        extensionPtr->extension_->InvokeAppExtMethod(errCode, restoreRetInfo);
        if (errCode == ERR_OK) {
            if (restoreRetInfo.size()) {
                extensionPtr->AppResultReport(restoreRetInfo, BackupRestoreScenario::INCREMENTAL_RESTORE);
            }
            return;
        }
        if (restoreRetInfo.empty()) {
            extensionPtr->AppIncrementalDone(errCode);
            extensionPtr->DoClear();
        } else {
            std::string errInfo;
            BJsonUtil::BuildRestoreErrInfo(errInfo, errCode, restoreRetInfo);
            extensionPtr->AppResultReport(errInfo, BackupRestoreScenario::INCREMENTAL_RESTORE, errCode);
            extensionPtr->DoClear();
        }
    };
}

std::function<void(ErrCode, std::string)> BackupExtExtension::IncreOnRestoreCallback(wptr<BackupExtExtension> obj)
{
    return [obj](ErrCode errCode, std::string errMsg) {
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        HILOGI("Current bundle will execute app done");
        extensionPtr->FinishOnProcessTask();
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

std::function<void(ErrCode, const std::string)> BackupExtExtension::OnBackupCallback(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get HandleFullBackupCallbackEx");
    return [obj](ErrCode errCode, std::string errMsg) {
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
        extensionPtr->FinishOnProcessTask();
        extensionPtr->AsyncTaskBackup(extensionPtr->extension_->GetUsrConfig());
    };
}

std::function<void(ErrCode, const std::string)> BackupExtExtension::OnBackupExCallback(wptr<BackupExtExtension> obj)
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
            extensionPtr->FinishOnProcessTask();
            extensionPtr->AsyncTaskBackup(extensionPtr->extension_->GetUsrConfig());
            extensionPtr->AppResultReport(backupExRetInfo, BackupRestoreScenario::FULL_BACKUP);
        }
    };
}

std::function<void(ErrCode, const std::string)> BackupExtExtension::IncOnBackupCallback(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get IncOnBackupCallback");
    return [obj](ErrCode errCode, std::string errMsg) {
        HILOGI("App onbackup end");
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
        }
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("Current extension execute call backup error, extPtr is empty");
            return;
        }
        HILOGI("Start GetAppLocalListAndDoIncrementalBackup");
        extPtr->FinishOnProcessTask();
        proxy->GetAppLocalListAndDoIncrementalBackup();
    };
}

std::function<void(ErrCode, const std::string)> BackupExtExtension::IncOnBackupExCallback(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get HandleIncBackupEx callback");
    return [obj](ErrCode errCode, const std::string backupExRetInfo) {
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
        }
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
            HILOGI("Start GetAppLocalListAndDoIncrementalBackup");
            extensionPtr->FinishOnProcessTask();
            proxy->GetAppLocalListAndDoIncrementalBackup();
            HILOGI("Will notify backup result report");
            extensionPtr->AppResultReport(backupExRetInfo, BackupRestoreScenario::INCREMENTAL_BACKUP);
        }
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

void BackupExtExtension::StartOnProcessTaskThread(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario)
{
    HILOGI("Begin Create onProcess Task Thread");
    onProcessTimeoutTimer_.Setup();
    StartOnProcessTimeOutTimer(obj);
    SyncCallJsOnProcessTask(obj, scenario);
    callJsOnProcessThread_ = std::thread([obj, scenario]() {
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("Create onProcess Task thread failed, extPtr is empty");
            return;
        }
        extPtr->ExecCallOnProcessTask(obj, scenario);
    });
    HILOGI("End Create onProcess Task End");
}

void BackupExtExtension::ExecCallOnProcessTask(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario)
{
    HILOGI("Begin");
    while (!stopCallJsOnProcess_.load()) {
        std::unique_lock<std::mutex> lock(onProcessLock_);
        execOnProcessCon_.wait_for(lock, std::chrono::seconds(BConstants::CALL_APP_ON_PROCESS_TIME_INTERVAL),
            [this] { return this->stopCallJsOnProcess_.load(); });
        if (stopCallJsOnProcess_.load()) {
            HILOGE("Current extension execute js onProcess method finished");
            return;
        }
        if (onProcessTimeout_.load()) {
            HILOGE("Current extension execute js method onProcess timeout");
            StartOnProcessTimeOutTimer(obj);
            continue;
        }
        HILOGI("Continue call js method onProcess");
        AsyncCallJsOnProcessTask(obj, scenario);
        StartOnProcessTimeOutTimer(obj);
    }
    HILOGI("End");
}

void BackupExtExtension::AsyncCallJsOnProcessTask(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario)
{
    HILOGI("Begin");
    if (stopCallJsOnProcess_.load()) {
        HILOGE("Current extension execute finished");
        return;
    }
    auto task = [obj, scenario]() {
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("Async Call js onProcess error, extPtr is empty");
            return;
        }
        extPtr->SyncCallJsOnProcessTask(obj, scenario);
    };
    threadPool_.AddTask([task]() { task(); });
    HILOGI("End");
}

void BackupExtExtension::SyncCallJsOnProcessTask(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario)
{
    HILOGI("Begin");
    if (stopCallJsOnProcess_.load()) {
        HILOGE("Current extension execute finished");
        return;
    }
    auto callBack = [obj, scenario](ErrCode errCode, const std::string processInfo) {
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("Async call js onPreocess callback failed, exPtr is empty");
            return;
        }
        extPtr->CloseOnProcessTimeOutTimer();
        extPtr->onProcessTimeout_.store(false);
        if (extPtr->onProcessTimeoutCnt_.load() > 0) {
            HILOGI("onProcess execute success, decrease cnt is:%{public}d",
                extPtr->onProcessTimeoutCnt_.load());
            extPtr->onProcessTimeoutCnt_--;
        }
        if (processInfo.size() == 0) {
            HILOGE("Current extension has no js method named onProcess.");
            extPtr->stopCallJsOnProcess_.store(true);
            extPtr->execOnProcessCon_.notify_one();
            return;
        }
        std::string processInfoJsonStr;
        BJsonUtil::BuildOnProcessRetInfo(processInfoJsonStr, processInfo);
        HILOGI("Will notify backup sa process result");
        extPtr->ReportAppProcessInfo(processInfoJsonStr, scenario);
    };
    auto extenionPtr = obj.promote();
    if (extenionPtr == nullptr) {
        HILOGE("Async call js onPreocess failed, extenionPtr is empty");
        return;
    }
    ErrCode ret = extenionPtr->extension_->OnProcess(callBack);
    if (ret != ERR_OK) {
        HILOGE("Call OnProcess Failed, ret:%{public}d", ret);
        return;
    }
    HILOGI("End");
}

void BackupExtExtension::FinishOnProcessTask()
{
    HILOGI("Begin");
    stopCallJsOnProcess_.store(true);
    onProcessTimeoutCnt_ = 0;
    execOnProcessCon_.notify_one();
    if (callJsOnProcessThread_.joinable()) {
        callJsOnProcessThread_.join();
    }
    HILOGI("End");
}

void BackupExtExtension::StartOnProcessTimeOutTimer(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin");
    if (stopCallJsOnProcess_.load()) {
        HILOGE("Current extension execute finished");
        return;
    }
    auto timeoutCallback = [obj]() {
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("Start Create timeout callback failed, extPtr is empty");
            return;
        }
        if (extPtr->onProcessTimeoutCnt_.load() >= BConstants::APP_ON_PROCESS_TIMEOUT_MAX_COUNT) {
            HILOGE("Current extension onProcess timeout more than three times");
            extPtr->stopCallJsOnProcess_.store(true);
            extPtr->onProcessTimeoutCnt_ = 0;
            extPtr->execOnProcessCon_.notify_one();
            extPtr->AppDone(BError(BError::Codes::EXT_ABILITY_TIMEOUT));
            return;
        }
        extPtr->onProcessTimeoutCnt_++;
        extPtr->onProcessTimeout_.store(true);
        HILOGE("Extension onProcess timeout, Increase cnt:%{public}d", extPtr->onProcessTimeoutCnt_.load());
    };
    uint32_t timerId = onProcessTimeoutTimer_.Register(timeoutCallback, BConstants::APP_ON_PROCESS_MAX_TIMEOUT, true);
    onProcessTimeoutTimerId_ = timerId;
    HILOGI("End");
}

void BackupExtExtension::CloseOnProcessTimeOutTimer()
{
    HILOGI("Begin");
    onProcessTimeoutTimer_.Unregister(onProcessTimeoutTimerId_);
    HILOGI("End");
}
} // namespace OHOS::FileManagement::Backup
