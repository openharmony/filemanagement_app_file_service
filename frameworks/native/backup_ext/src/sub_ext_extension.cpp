/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
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
#include <cinttypes>
#include <fstream>
#include <fnmatch.h>
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
#include "b_radar/b_radar.h"
#include "b_tarball/b_tarball_factory.h"
#include "b_utils/scan_file_singleton.h"
#include "filemgmt_libhilog.h"
#include "hitrace_meter.h"
#include "sandbox_helper.h"
#include "service_client.h"
#include "tar_file.h"
#include "b_anony/b_anony.h"

namespace OHOS::FileManagement::Backup {
const uint32_t MAX_FD_GROUP_USE_TIME = 1000; // 每组打开最大时间1000ms

ErrCode BackupExtExtension::HandleIncrementalBackup(int incrementalFd, int manifestFd)
{
    HILOGI("Start HandleIncrementalBackup. incrementalFd:%{public}d, manifestFd:%{public}d", incrementalFd, manifestFd);
    UniqueFd incrementalFdUnique(dup(incrementalFd));
    UniqueFd manifestFdUnique(dup(manifestFd));
    ErrCode ret = HandleIncrementalBackup(std::move(incrementalFdUnique), std::move(manifestFdUnique));
    close(incrementalFd);
    close(manifestFd);
    return ret;
}

ErrCode BackupExtExtension::HandleIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Start HandleIncrementalBackup");
        if (!IfAllowToBackupRestore()) {
            return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE, "Application does not allow backup or restore")
                .GetCode();
        }
        AsyncTaskDoIncrementalBackup(move(incrementalFd), move(manifestFd));
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to handle incremental backup");
        return BError(BError::Codes::EXT_INVAL_ARG).GetCode();
    }
}

ErrCode BackupExtExtension::IncrementalOnBackup(bool isClearData)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    SetClearDataFlag(isClearData);
    if (!IfAllowToBackupRestore()) {
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE, "Application does not allow backup or restore")
            .GetCode();
    }
    curScenario_ = BackupRestoreScenario::INCREMENTAL_BACKUP;
    AsyncTaskOnIncrementalBackup();
    return ERR_OK;
}

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
    if (!extension_->WasFromSpecialVersion() && !extension_->RestoreDataReady()) {
        DoClear();
    }
}

string BackupExtExtension::GetBundlePath()
{
    if (bundleName_ == BConstants::BUNDLE_FILE_MANAGER) {
        return string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    } else if (bundleName_ == BConstants::BUNDLE_MEDIAL_DATA) {
        return string(BConstants::PATH_MEDIALDATA_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    }
    return string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
}

std::map<std::string, off_t> BackupExtExtension::GetIdxFileInfos(bool isSpecialVersion)
{
    string restoreDir = isSpecialVersion ? "" : GetBundlePath();
    auto extManageInfo = GetExtManageInfo(isSpecialVersion);
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
    for (const auto &it : idxFileInfos) {
        if (it.first.size() >= PATH_MAX || stat(it.first.data(), &attr) == -1) {
            HILOGE("(Debug) Failed to get stat of %{public}s, errno = %{public}d", GetAnonyPath(it.first).c_str(),
                errno);
            errFiles[it.first].emplace_back(errno);
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
        std::stringstream ss;
        ss << R"("totalFile": )" << idxFileInfos.size() << R"(, "restoreFile": )"
            << idxFileInfos.size() - errFiles.size() << R"(, "info": "different received file and idx")";
        AppRadar::Info info (bundleName_, "", ss.str());
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "BackupExtExtension::CheckTmpDirFileInfos",
            AppRadar::GetInstance().GetUserId(), BizStageRestore::BIZ_STAGE_CHECK_DATA_FAIL,
            static_cast<int32_t>(BError::BackupErrorCode::E_UKERR));
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
            HILOGE("Failed to lstat %{public}s, err = %{public}d", GetAnonyPath(it.first).c_str(), errno);
            errFiles.emplace_back(it.first);
            errFileInfos_[it.first].emplace_back(errno);
        } else if (curFileStat.st_size != it.second) {
            HILOGE("File size error, file: %{public}s, idx: %{public}" PRId64 ", act: %{public}" PRId64 "",
                GetAnonyPath(it.first).c_str(), it.second, curFileStat.st_size);
            errFiles.emplace_back(it.first);
            errFileInfos_[it.first].emplace_back(errno);
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
        std::stringstream ss;
        ss << R"("totalFile": )" << endFileInfos_.size() << R"(, "restoreFile": )"
            << endFileInfos_.size() - errFileInfos_.size();
        AppRadar::Info info (bundleName_, "", ss.str());
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "BackupExtExtension::CheckRestoreFileInfos",
            AppRadar::GetInstance().GetUserId(), BizStageRestore::BIZ_STAGE_CHECK_DATA_FAIL,
            static_cast<int32_t>(BError::BackupErrorCode::E_UKERR));
        return { false, errFiles };
    }
    return { true, errFiles };
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
        extensionPtr->OnRestoreFinish();
        if (extensionPtr->isExecAppDone_.load()) {
            HILOGE("Appdone has been executed for the current application");
            return;
        }
        HILOGI("Current bundle will execute app done");
        if (errCode == ERR_OK) {
            auto spendTime = extensionPtr->GetOnStartTimeCost();
            if (spendTime >= BConstants::MAX_TIME_COST) {
                std::stringstream ss;
                ss << R"("spendTime": )"<< spendTime << "ms";
                AppRadar::Info info (extensionPtr->bundleName_, "", ss.str());
                AppRadar::GetInstance().RecordRestoreFuncRes(info, "BackupExtExtension::OnRestoreCallback",
                    AppRadar::GetInstance().GetUserId(), BizStageRestore::BIZ_STAGE_ON_RESTORE, ERR_OK);
            }
        }
        extensionPtr->FinishOnProcessTask();
        if (errMsg.empty()) {
            extensionPtr->AppDone(errCode);
        } else {
            std::string errInfo;
            BJsonUtil::BuildExtensionErrInfo(errInfo, errCode, errMsg);
            extensionPtr->ReportAppStatistic("OnRestoreCallback", errCode);
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
        extensionPtr->OnRestoreExFinish();
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        if (extensionPtr->isExecAppDone_.load()) {
            HILOGE("Appdone has been executed for the current application");
            return;
        }
        extensionPtr->extension_->InvokeAppExtMethod(errCode, restoreRetInfo);
        if (errCode == ERR_OK) {
            if (restoreRetInfo.size()) {
                auto spendTime = extensionPtr->GetOnStartTimeCost();
                if (spendTime >= BConstants::MAX_TIME_COST) {
                    std::stringstream ss;
                    ss << R"("spendTime": )"<< spendTime << "ms";
                    AppRadar::Info info (extensionPtr->bundleName_, "", ss.str());
                    AppRadar::GetInstance().RecordRestoreFuncRes(info, "BackupExtExtension::OnRestoreExCallback",
                        AppRadar::GetInstance().GetUserId(), BizStageRestore::BIZ_STAGE_ON_RESTORE, ERR_OK);
                }
                HILOGI("Will notify restore result report");
                extensionPtr->FinishOnProcessTask();
                extensionPtr->ReportAppStatistic("OnRestoreExCallback1", errCode);
                extensionPtr->AppResultReport(restoreRetInfo, BackupRestoreScenario::FULL_RESTORE);
            }
            return;
        }
        HILOGE("Call extension onRestoreEx failed, errInfo = %{public}s", restoreRetInfo.c_str());
        extensionPtr->FinishOnProcessTask();
        if (restoreRetInfo.empty()) {
            extensionPtr->AppDone(errCode);
            extensionPtr->DoClear();
        } else {
            std::string errInfo;
            BJsonUtil::BuildExtensionErrInfo(errInfo, errCode, restoreRetInfo);
            extensionPtr->ReportAppStatistic("OnRestoreExCallback2", errCode);
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
        extensionPtr->OnRestoreExFinish();
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        if (extensionPtr->isExecAppDone_.load()) {
            HILOGE("Appdone has been executed for the current application");
            return;
        }
        extensionPtr->extension_->InvokeAppExtMethod(errCode, restoreRetInfo);
        if (errCode == ERR_OK) {
            if (restoreRetInfo.size()) {
                auto spendTime = extensionPtr->GetOnStartTimeCost();
                if (spendTime >= BConstants::MAX_TIME_COST) {
                    std::stringstream ss;
                    ss << R"("spendTime": )"<< spendTime << "ms";
                    AppRadar::Info info (extensionPtr->bundleName_, "", ss.str());
                    AppRadar::GetInstance().RecordRestoreFuncRes(info, "BackupExtExtension::IncreOnRestoreExCallback",
                        AppRadar::GetInstance().GetUserId(), BizStageRestore::BIZ_STAGE_ON_RESTORE, ERR_OK);
                }
                extensionPtr->FinishOnProcessTask();
                extensionPtr->ReportAppStatistic("IncreOnRestoreExCallback1", errCode);
                extensionPtr->AppResultReport(restoreRetInfo, BackupRestoreScenario::INCREMENTAL_RESTORE);
            }
            return;
        }
        HILOGE("Call increment onRestoreEx failed, errInfo = %{public}s", restoreRetInfo.c_str());
        extensionPtr->FinishOnProcessTask();
        if (restoreRetInfo.empty()) {
            extensionPtr->AppIncrementalDone(errCode);
            extensionPtr->DoClear();
        } else {
            std::string errInfo;
            BJsonUtil::BuildExtensionErrInfo(errInfo, errCode, restoreRetInfo);
            extensionPtr->ReportAppStatistic("IncreOnRestoreExCallback2", errCode);
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
        extensionPtr->OnRestoreFinish();
        if (extensionPtr->isExecAppDone_.load()) {
            HILOGE("Appdone has been executed for the current application");
            return;
        }
        HILOGI("Current bundle will execute app done");
        if (errCode == ERR_OK) {
            auto spendTime = extensionPtr->GetOnStartTimeCost();
            if (spendTime >= BConstants::MAX_TIME_COST) {
                std::stringstream ss;
                ss << R"("spendTime": )"<< spendTime << "ms";
                AppRadar::Info info (extensionPtr->bundleName_, "", ss.str());
                AppRadar::GetInstance().RecordRestoreFuncRes(info, "BackupExtExtension::IncreOnRestoreCallback",
                    AppRadar::GetInstance().GetUserId(), BizStageRestore::BIZ_STAGE_ON_RESTORE, ERR_OK);
            }
        }
        extensionPtr->FinishOnProcessTask();
        if (errMsg.empty()) {
            extensionPtr->AppIncrementalDone(errCode);
        } else {
            std::string errInfo;
            BJsonUtil::BuildExtensionErrInfo(errInfo, errCode, errMsg);
            extensionPtr->ReportAppStatistic("IncreOnRestoreCallback", errCode);
            extensionPtr->AppResultReport(errInfo, BackupRestoreScenario::INCREMENTAL_RESTORE, errCode);
        }
        extensionPtr->DoClear();
    };
}

int32_t BackupExtExtension::GetOnStartTimeCost()
{
    auto onBackupRestoreEnd = std::chrono::system_clock::now();
    std::lock_guard<std::mutex> lock(onStartTimeLock_);
    auto spendTime = std::chrono::duration_cast<std::chrono::milliseconds>(onBackupRestoreEnd - g_onStart).count();
    return spendTime;
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
        extensionPtr->OnBackupFinish();
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        if (extensionPtr->isExecAppDone_.load()) {
            HILOGE("Appdone has been executed for the current application");
            return;
        }
        extensionPtr->FinishOnProcessTask();
        if (errCode == ERR_OK) {
            auto spendTime = extensionPtr->GetOnStartTimeCost();
            if (spendTime >= BConstants::MAX_TIME_COST) {
                AppRadar::Info info(extensionPtr->bundleName_, "", string("\"spend_time\":\" ").
                    append(to_string(spendTime)).append(string("ms\"")));
                AppRadar::GetInstance().RecordBackupFuncRes(info, "BackupExtExtension::OnBackupCallback",
                    AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_ON_BACKUP,
                    static_cast<int32_t>(ERR_OK));
            }
            extensionPtr->AsyncTaskBackup(extensionPtr->extension_->GetUsrConfig());
            return;
        }
        HILOGE("Call extension onBackup failed, errInfo = %{public}s", errMsg.c_str());
        if (!errMsg.empty()) {
            std::string errInfo;
            BJsonUtil::BuildExtensionErrInfo(errInfo, errCode, errMsg);
            extensionPtr->AppResultReport(errInfo, BackupRestoreScenario::FULL_BACKUP, errCode);
        }
        extensionPtr->AppDone(errCode);
        extensionPtr->DoClear();
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
        extensionPtr->OnBackupExFinish();
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        if (extensionPtr->isExecAppDone_.load()) {
            HILOGE("Appdone has been executed for the current application");
            return;
        }
        extensionPtr->extension_->InvokeAppExtMethod(errCode, backupExRetInfo);
        if (errCode == ERR_OK) {
            if (backupExRetInfo.size()) {
                auto spendTime = extensionPtr->GetOnStartTimeCost();
                if (spendTime >= BConstants::MAX_TIME_COST) {
                    AppRadar::Info info(extensionPtr->bundleName_, "", string("\"spend_time\":\" ").
                        append(to_string(spendTime)).append(string("ms\"")));
                    AppRadar::GetInstance().RecordBackupFuncRes(info, "BackupExtExtension::OnBackupExCallback",
                        AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_ON_BACKUP,
                        static_cast<int32_t>(ERR_OK));
                }
                HILOGI("Will notify backup result report");
                extensionPtr->FinishOnProcessTask();
                extensionPtr->AsyncTaskBackup(extensionPtr->extension_->GetUsrConfig());
                extensionPtr->AppResultReport(backupExRetInfo, BackupRestoreScenario::FULL_BACKUP);
            }
            return;
        }
        HILOGE("Call extension onBackupEx failed, errInfo = %{public}s", backupExRetInfo.c_str());
        extensionPtr->FinishOnProcessTask();
        if (!backupExRetInfo.empty()) {
            std::string errInfo;
            BJsonUtil::BuildExtensionErrInfo(errInfo, errCode, backupExRetInfo);
            extensionPtr->AppResultReport(errInfo, BackupRestoreScenario::FULL_BACKUP, errCode);
        }
        extensionPtr->AppDone(errCode);
        extensionPtr->DoClear();
    };
}

std::function<void(ErrCode, const std::string)> BackupExtExtension::IncOnBackupCallback(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get IncOnBackupCallback");
    return [obj](ErrCode errCode, std::string errMsg) {
        HILOGI("App onbackup end");
        auto proxy = ServiceClient::GetInstance();
        if (proxy == nullptr) {
            throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
        }
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("Current extension execute call backup error, extPtr is empty");
            return;
        }
        extPtr->OnBackupFinish();
        if (extPtr->isExecAppDone_.load()) {
            HILOGE("Appdone has been executed for the current application");
            return;
        }
        HILOGI("Start GetAppLocalListAndDoIncrementalBackup");
        extPtr->FinishOnProcessTask();
        if (errCode == ERR_OK) {
            auto spendTime = extPtr->GetOnStartTimeCost();
            if (spendTime >= BConstants::MAX_TIME_COST) {
                AppRadar::Info info(extPtr->bundleName_, "", string("\"spend_time\":\" ").
                    append(to_string(spendTime)).append(string("ms\"")));
                AppRadar::GetInstance().RecordBackupFuncRes(info, "BackupExtExtension::IncOnBackupCallback",
                    AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_ON_BACKUP,
                    static_cast<int32_t>(ERR_OK));
            }
            proxy->GetAppLocalListAndDoIncrementalBackup();
            return;
        }
        HILOGE("Call extension IncOnBackup failed, errInfo = %{public}s", errMsg.c_str());
        if (!errMsg.empty()) {
            std::string errInfo;
            BJsonUtil::BuildExtensionErrInfo(errInfo, errCode, errMsg);
            extPtr->AppResultReport(errInfo, BackupRestoreScenario::INCREMENTAL_BACKUP, errCode);
        }
        extPtr->AppIncrementalDone(errCode);
        extPtr->DoClear();
    };
}

std::function<void(ErrCode, const std::string)> BackupExtExtension::IncOnBackupExCallback(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get HandleIncBackupEx callback");
    return [obj](ErrCode errCode, const std::string backupExRetInfo) {
        auto proxy = ServiceClient::GetInstance();
        if (proxy == nullptr) {
            throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
        }
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        extensionPtr->OnBackupExFinish();
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        if (extensionPtr->isExecAppDone_.load()) {
            HILOGE("Appdone has been executed for the current application");
            return;
        }
        extensionPtr->extension_->InvokeAppExtMethod(errCode, backupExRetInfo);
        if (errCode == ERR_OK) {
            if (backupExRetInfo.size()) {
                auto spendTime = extensionPtr->GetOnStartTimeCost();
                if (spendTime >= BConstants::MAX_TIME_COST) {
                    AppRadar::Info info(extensionPtr->bundleName_, "", string("\"spend_time\":\" ").
                        append(to_string(spendTime)).append(string("ms\"")));
                    AppRadar::GetInstance().RecordBackupFuncRes(info, "BackupExtExtension::IncOnBackupExCallback",
                        AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_ON_BACKUP,
                        static_cast<int32_t>(ERR_OK));
                }
                HILOGI("Start GetAppLocalListAndDoIncrementalBackup");
                extensionPtr->FinishOnProcessTask();
                proxy->GetAppLocalListAndDoIncrementalBackup();
                HILOGI("Will notify backup result report");
                extensionPtr->AppResultReport(backupExRetInfo, BackupRestoreScenario::INCREMENTAL_BACKUP);
            }
            return;
        }
        HILOGE("Call extension IncOnBackupEx failed, errInfo = %{public}s", backupExRetInfo.c_str());
        extensionPtr->FinishOnProcessTask();
        if (!backupExRetInfo.empty()) {
            std::string errInfo;
            BJsonUtil::BuildExtensionErrInfo(errInfo, errCode, backupExRetInfo);
            extensionPtr->AppResultReport(errInfo, BackupRestoreScenario::INCREMENTAL_BACKUP, errCode);
        }
        extensionPtr->AppIncrementalDone(errCode);
        extensionPtr->DoClear();
    };
}

void BackupExtExtension::ReportAppProcessInfo(const std::string processInfo, BackupRestoreScenario scenario)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Report app process error, proxy is empty");
        return;
    }
    HILOGI("Will notify backup sa process result");
    auto ret = proxy->ReportAppProcessInfo(processInfo, scenario);
    if (ret != ERR_OK) {
        HILOGE("Report app process error, ipc failed, ret:%{public}d", ret);
        return;
    }
}

ErrCode BackupExtExtension::StartOnProcessTaskThread(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario)
{
    HILOGI("Begin Create onProcess Task Thread");
    onProcessTimeoutTimer_.Setup();
    isFirstCallOnProcess_.store(true);
    StartOnProcessTimeOutTimer(obj, scenario);
    SyncCallJsOnProcessTask(obj, scenario);
    if (isExecAppDone_.load()) {
        HILOGE("Call onProcess timeout, Current extension finished");
        return BError(BError::Codes::EXT_ABILITY_TIMEOUT);
    }
    callJsOnProcessThread_ = std::thread([obj, scenario]() {
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("Create onProcess Task thread failed, extPtr is empty");
            return;
        }
        extPtr->ExecCallOnProcessTask(obj, scenario);
    });
    HILOGI("End Create onProcess Task End");
    return BError(BError::Codes::OK);
}

void BackupExtExtension::ExecCallOnProcessTask(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario)
{
    HILOGI("Begin");
    isFirstCallOnProcess_.store(false);
    while (!stopCallJsOnProcess_.load()) {
        std::unique_lock<std::mutex> lock(onProcessLock_);
        execOnProcessCon_.wait_for(lock, std::chrono::seconds(BConstants::CALL_APP_ON_PROCESS_TIME_INTERVAL),
            [this] { return this->stopCallJsOnProcess_.load(); });
        if (stopCallJsOnProcess_.load()) {
            HILOGE("Current extension execute js onProcess method finished");
            return;
        }
        HILOGI("Continue call js method onProcess");
        StartOnProcessTimeOutTimer(obj, scenario);
        AsyncCallJsOnProcessTask(obj, scenario);
    }
    HILOGI("End");
}

void BackupExtExtension::OnBackupFinish()
{
    appStatistic_->onBackupSpend_.End();
}

void BackupExtExtension::OnBackupExFinish()
{
    appStatistic_->onBackupexSpend_.End();
}

void BackupExtExtension::OnRestoreFinish()
{
    appStatistic_->onRestoreSpend_.End();
}

void BackupExtExtension::OnRestoreExFinish()
{
    appStatistic_->onRestoreexSpend_.End();
}

void BackupExtExtension::DoBackupStart()
{
    appStatistic_->doBackupSpend_.Start();
}

void BackupExtExtension::DoBackupEnd()
{
    appStatistic_->doBackupSpend_.End();
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
    onProcessTaskPool_.AddTask([task]() { task(); });
    HILOGI("End");
}

void BackupExtExtension::SyncCallJsOnProcessTask(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario)
{
    HILOGI("Begin");
    if (stopCallJsOnProcess_.load()) {
        HILOGE("Current extension execute finished");
        return;
    }
    auto callBack = ReportOnProcessResultCallback(obj, scenario);
    auto extenionPtr = obj.promote();
    if (extenionPtr == nullptr || extenionPtr->extension_ == nullptr) {
        HILOGE("Async call js onProcess failed, extenionPtr is empty");
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
    std::unique_lock<std::mutex> lock(onProcessLock_);
    stopCallJsOnProcess_.store(true);
    isFirstCallOnProcess_.store(false);
    isExecAppDone_.store(false);
    onProcessTimeoutCnt_ = 0;
    execOnProcessCon_.notify_one();
    lock.unlock();
    if (callJsOnProcessThread_.joinable()) {
        callJsOnProcessThread_.join();
    }
    HILOGI("End");
}

void BackupExtExtension::StartOnProcessTimeOutTimer(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario)
{
    HILOGI("Begin");
    if (stopCallJsOnProcess_.load()) {
        HILOGE("Current extension execute finished");
        return;
    }
    auto timeoutCallback = [obj, scenario]() {
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("Start Create timeout callback failed, extPtr is empty");
            return;
        }
        if (extPtr->onProcessTimeoutCnt_.load() >= BConstants::APP_ON_PROCESS_TIMEOUT_MAX_COUNT ||
            extPtr->isFirstCallOnProcess_.load()) {
            HILOGE("The extension invokes the onProcess for more than three times or the first invoking of the "
                "onProcess times out, timeoutCnt:%{public}d", extPtr->onProcessTimeoutCnt_.load());
            std::unique_lock<std::mutex> lock(extPtr->onProcessLock_);
            extPtr->stopCallJsOnProcess_.store(true);
            extPtr->isFirstCallOnProcess_.store(false);
            extPtr->isExecAppDone_.store(true);
            extPtr->onProcessTimeoutCnt_ = 0;
            extPtr->execOnProcessCon_.notify_one();
            lock.unlock();
            if (scenario == BackupRestoreScenario::FULL_BACKUP || scenario == BackupRestoreScenario::FULL_RESTORE) {
                extPtr->AppDone(BError(BError::Codes::EXT_ABILITY_TIMEOUT));
            } else if (scenario == BackupRestoreScenario::INCREMENTAL_BACKUP ||
                scenario == BackupRestoreScenario::INCREMENTAL_RESTORE) {
                extPtr->AppIncrementalDone(BError(BError::Codes::EXT_ABILITY_TIMEOUT));
            }
            return;
        }
        extPtr->onProcessTimeoutCnt_++;
        extPtr->onProcessTimeout_.store(true);
        HILOGE("Extension onProcess timeout, Increase cnt:%{public}d", extPtr->onProcessTimeoutCnt_.load());
    };
    int timeout = isFirstCallOnProcess_.load() ? BConstants::FIRST_CALL_APP_ON_PROCESS_MAX_TIMEOUT :
                                                 BConstants::APP_ON_PROCESS_MAX_TIMEOUT;
    uint32_t timerId = onProcessTimeoutTimer_.Register(timeoutCallback, timeout, true);
    onProcessTimeoutTimerId_ = timerId;
    HILOGI("End");
}

void BackupExtExtension::CloseOnProcessTimeOutTimer()
{
    HILOGI("Begin");
    onProcessTimeoutTimer_.Unregister(onProcessTimeoutTimerId_);
    HILOGI("End");
}

void BackupExtExtension::AppIncrementalDone(ErrCode errCode)
{
    HILOGI("Begin");
    ReportAppStatistic("AppIncrementalDone", errCode);
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to obtain the ServiceClient handle");
        DoClear();
        return;
    }
    auto ret = proxy->AppIncrementalDone(errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed to notify the app done. err = %{public}d", ret);
    }
    if (HandleGetExtOnRelease()) {
        HandleExtOnRelease(false, errCode);
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
        HILOGI("GetBackupInfo callBackup start. errCode = %{public}d, result = %{public}s", errCode, result.c_str());
        if (errCode == ERR_OK) {
            ptr->backupInfo_ = result;
        }
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

ErrCode BackupExtExtension::UpdateFdSendRate(const std::string &bundleName, int32_t sendRate)
{
    try {
        std::lock_guard<std::mutex> lock(updateSendRateLock_);
        HILOGI("Update SendRate, bundleName:%{public}s, sendRate:%{public}d", bundleName.c_str(), sendRate);
        VerifyCaller();
        bundleName_ = bundleName;
        sendRate_ = sendRate;
        if (sendRate > 0) {
            appStatistic_->UpdateSendRateZeroSpend();
            startSendFdRateCon_.notify_one();
        } else {
            appStatistic_->sendRateZeroStart_ = TimeUtils::GetTimeUS();
        }
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to UpdateFdSendRate");
        return BError(BError::Codes::EXT_BROKEN_IPC).GetCode();
    }
}

ErrCode BackupExtExtension::UpdateDfxInfo(int64_t uniqId, uint32_t extConnectSpend, const std::string &bundleName)
{
    appStatistic_->SetUniqId(uniqId);
    appStatistic_->extConnectSpend_ = extConnectSpend;
    appStatistic_->appCaller_ = bundleName;
    return ERR_OK;
}

bool BackupExtExtension::SetStagingPathProperties()
{
    struct stat curBundleStat {};
    if (lstat(BConstants::BUNDLE_BASE_DIR.c_str(), &curBundleStat) != 0) {
        HILOGE("Failed to lstat, err = %{public}d", errno);
        return false;
    }
    if (lchown(string(BConstants::PATH_BUNDLE_BACKUP_HOME).c_str(),
        curBundleStat.st_uid, curBundleStat.st_gid) != 0) {
        HILOGE("Failed to lchown, err = %{public}d", errno);
        return false;
    }
    if (lchown(string(BConstants::PATH_BUNDLE_BACKUP_HOME_EL1).c_str(),
        curBundleStat.st_uid, curBundleStat.st_gid) != 0) {
        HILOGE("Failed to lchown, err = %{public}d", errno);
        return false;
    }
    return true;
}

bool BackupExtExtension::IfAllowToBackupRestore()
{
    if (extension_ == nullptr) {
        HILOGE("Failed to handle backup, extension is nullptr");
        return false;
    }
    string usrConfig = extension_->GetUsrConfig();
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(usrConfig);
    auto cache = cachedEntity.Structuralize();
    if (!cache.GetAllowToBackupRestore()) {
        HILOGE("Application does not allow backup or restore");
        return false;
    }
    return true;
}

ErrCode BackupExtExtension::User0OnBackup()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (!IfAllowToBackupRestore()) {
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE, "Application does not allow backup or restore")
            .GetCode();
    }
    AsyncTaskUser0Backup();
    return ERR_OK;
}

void BackupExtExtension::AsyncTaskUser0Backup()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        const string config = ptr->extension_->GetUsrConfig();
        try {
            HILOGI("Do backup, start fwk timer begin.");
            bool isFwkStart;
            ptr->StartFwkTimer(isFwkStart);
            if (!isFwkStart) {
                HILOGE("Do backup, start fwk timer fail.");
                return;
            }
            HILOGI("Do backup, start fwk timer end.");
            BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(config);
            auto cache = cachedEntity.Structuralize();
            auto ret = ptr->User0DoBackup(cache);
            if (ret != ERR_OK) {
                HILOGE("User0DoBackup, err = %{public}d", ret);
                ptr->AppIncrementalDone(BError::GetCodeByErrno(ret));
            }
        } catch (const BError &e) {
            HILOGE("extension: AsyncTaskBackup error, err code:%{public}d", e.GetCode());
            ptr->AppIncrementalDone(e.GetCode());
        } catch (const exception &e) {
            HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
            ptr->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        }
    };

    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

void BackupExtExtension::DoUser0Backup(const BJsonEntityExtensionConfig &usrConfig)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
    if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
        throw BError(errno);
    }
    vector<string> includes = usrConfig.GetIncludes();
    vector<string> excludes = usrConfig.GetExcludes();
    auto task = [obj {wptr<BackupExtExtension>(this)}, includes, excludes]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        try {
            auto [bigFile, smallFile] = BDir::GetBackupList(includes, excludes);
            vector<struct ReportFileInfo> allFiles;
            vector<struct ReportFileInfo> smallFiles;
            vector<struct ReportFileInfo> bigFiles;
            BDir::GetUser0FileStat(move(bigFile), move(smallFile), allFiles, smallFiles, bigFiles);
            auto ret = ptr->DoIncrementalBackup(allFiles, smallFiles, bigFiles);
            ptr->AppIncrementalDone(ret);
            HILOGI("User0 backup app done %{public}d", ret);
        } catch (const BError &e) {
            ptr->AppIncrementalDone(e.GetCode());
        } catch (const exception &e) {
            ptr->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        }
    };

    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

int BackupExtExtension::User0DoBackup(const BJsonEntityExtensionConfig &usrConfig)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Start Do User0Backup");
    if (extension_ == nullptr) {
        HILOGE("Failed to do backup, extension is nullptr");
        return BError(BError::Codes::EXT_INVAL_ARG);
    }
    if (extension_->GetExtensionAction() != BConstants::ExtensionAction::BACKUP) {
        return EPERM;
    }
    DoUser0Backup(usrConfig);
    return ERR_OK;
}

void BackupExtExtension::UpdateFileStat(std::string filePath, uint64_t fileSize)
{
    appStatistic_->UpdateFileDist(ExtractFileExt(filePath), fileSize);
    uint32_t dirDepth = 0;
    const char* pstr = filePath.c_str();
    char pre = '-';
    uint32_t pathLen = filePath.size();
    for (uint32_t i = 0; i < pathLen; i++) {
        if (pstr[i] == '/' && pre != '/') {
            dirDepth++;
        }
        pre = pstr[i];
    }
    if (dirDepth > appStatistic_->dirDepth_) {
        appStatistic_->dirDepth_ = dirDepth;
    }
}

/**
 * 获取增量的大文件的信息
 */
TarMap BackupExtExtension::GetIncrmentBigInfos(const vector<struct ReportFileInfo> &files)
{
    auto getStringHash = [](const TarMap &tarMap, const string &str) -> string {
        ostringstream strHex;
        strHex << hex;

        hash<string> strHash;
        size_t szHash = strHash(str);
        strHex << setfill('0') << setw(BConstants::BIG_FILE_NAME_SIZE) << szHash;
        string name = strHex.str();
        for (int i = 0; tarMap.find(name) != tarMap.end(); ++i, strHex.str("")) {
            szHash = strHash(str + to_string(i));
            strHex << setfill('0') << setw(BConstants::BIG_FILE_NAME_SIZE) << szHash;
            name = strHex.str();
        }

        return name;
    };

    TarMap bigFiles;
    for (const auto &item : files) {
        struct stat sta = {};
        if (stat(item.filePath.c_str(), &sta) != 0) {
            HILOGE("Failed to stat file %{public}s, err = %{public}d", item.filePath.c_str(), errno);
            throw errno;
        }
        appStatistic_->bigFileSize_ += static_cast<uint64_t>(sta.st_size);
        UpdateFileStat(item.filePath, sta.st_size);
        uint64_t hashStart = static_cast<uint64_t>(TimeUtils::GetTimeUS());
        string md5Name = getStringHash(bigFiles, item.filePath);
        appStatistic_->hashSpendUS_ += TimeUtils::GetSpendUS(hashStart);
        if (!md5Name.empty()) {
            bigFiles.emplace(md5Name, make_tuple(item.filePath, sta, true));
        }
    }

    return bigFiles;
}

int BackupExtExtension::DoIncrementalBackupTask(UniqueFd incrementalFd, UniqueFd manifestFd)
{
    auto start = std::chrono::system_clock::now();
    vector<struct ReportFileInfo> allFiles;
    vector<struct ReportFileInfo> smallFiles;
    vector<struct ReportFileInfo> bigFiles;
    appStatistic_->doBackupSpend_.Start();
    FillFileInfos(move(incrementalFd), move(manifestFd), allFiles, smallFiles, bigFiles);
    auto ret = DoIncrementalBackup(allFiles, smallFiles, bigFiles);
    appStatistic_->doBackupSpend_.End();
    if (ret == ERR_OK) {
        auto end = std::chrono::system_clock::now();
        auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (cost >= BConstants::MAX_TIME_COST) {
            std::stringstream ss;
            ss << R"("spendTime": )"<< cost << R"(ms, "totalFiles": )" << allFiles.size() << R"(, "smallFiles": )"
                << smallFiles.size() << R"(, "bigFiles": )" << bigFiles.size();
            AppRadar::Info info(bundleName_, "", ss.str());
            AppRadar::GetInstance().RecordBackupFuncRes(info, "BackupExtExtension::DoIncrementalBackupTask",
                AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_DO_BACKUP, static_cast<int32_t>(ret));
        }
    }
    return ret;
}

void BackupExtExtension::AsyncTaskDoIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd)
{
    HILOGI("Do IncrementalBackup, start fwk timer begin.");
    bool isFwkStart;
    StartFwkTimer(isFwkStart);
    if (!isFwkStart) {
        HILOGE("Do IncrementalBackup, start fwk timer fail.");
        return;
    }
    HILOGI("Do IncrementalBackup, start fwk timer end.");
    int incrementalFdDup = dup(incrementalFd);
    int manifestFdDup = dup(manifestFd);
    if (incrementalFdDup < 0) {
        throw BError(BError::Codes::EXT_INVAL_ARG, "dup failed");
    }
    auto task = [obj {wptr<BackupExtExtension>(this)}, manifestFdDup, incrementalFdDup]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        try {
            UniqueFd incrementalDupFd(dup(incrementalFdDup));
            UniqueFd manifestDupFd(dup(manifestFdDup));
            if (incrementalDupFd < 0) {
                throw BError(BError::Codes::EXT_INVAL_ARG, "dup failed");
            }
            close(incrementalFdDup);
            close(manifestFdDup);
            auto ret = ptr->DoIncrementalBackupTask(move(incrementalDupFd), move(manifestDupFd));
            ptr->AppIncrementalDone(ret);
            HILOGI("Incremental backup app done %{public}d", ret);
        } catch (const BError &e) {
            ptr->AppIncrementalDone(e.GetCode());
        } catch (const exception &e) {
            HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
            ptr->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        }
    };

    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

void BackupExtExtension::AsyncTaskOnIncrementalBackup()
{
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        try {
            if ((ptr->StartOnProcessTaskThread(obj, BackupRestoreScenario::INCREMENTAL_BACKUP)) != ERR_OK) {
                HILOGE("Call onProcess result is timeout");
                return;
            }
            auto callBackup = ptr->IncOnBackupCallback(obj);
            auto callBackupEx = ptr->IncOnBackupExCallback(obj);
            ptr->UpdateOnStartTime();
            ErrCode err = ptr->extension_->OnBackup(callBackup, callBackupEx);
            if (err != ERR_OK) {
                HILOGE("OnBackup done, err = %{public}d", err);
                ptr->AppIncrementalDone(BError::GetCodeByErrno(err));
            }
        } catch (const BError &e) {
            ptr->AppIncrementalDone(e.GetCode());
        } catch (const exception &e) {
            HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
            ptr->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        }
    };

    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

static string GetIncrmentPartName()
{
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(duration);

    return to_string(milliseconds.count()) + "_part";
}

void BackupExtExtension::UpdateTarStat(uint64_t tarFileSize)
{
    appStatistic_->tarFileCount_++;
    appStatistic_->tarFileSize_ += tarFileSize;
}

void BackupExtExtension::IncrementalPacket(const vector<struct ReportFileInfo> &infos, TarMap &tar,
    sptr<IService> proxy)
{
    HILOGI("IncrementalPacket begin, infos count: %{public}zu", infos.size());
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
    uint64_t totalSize = 0;
    uint32_t fileCount = 0;
    vector<string> packFiles;
    vector<struct ReportFileInfo> tarInfos;
    TarFile::GetInstance().SetPacketMode(true); // 设置下打包模式
    auto startTime = std::chrono::system_clock::now();
    int fdNum = 0;
    string partName = GetIncrmentPartName();
    auto reportCb = ReportErrFileByProc(wptr<BackupExtExtension> {this}, curScenario_);
    uint64_t tarStart = static_cast<uint64_t>(TimeUtils::GetTimeMS());
    for (const auto &small : infos) {
        appStatistic_->smallFileSize_ += small.size;
        UpdateFileStat(small.filePath, small.size);
        totalSize += static_cast<uint64_t>(small.size);
        fileCount += 1;
        packFiles.emplace_back(small.filePath);
        tarInfos.emplace_back(small);
        if (totalSize >= BConstants::DEFAULT_SLICE_SIZE || fileCount >= BConstants::MAX_FILE_COUNT) {
            TarMap tarMap {};
            TarFile::GetInstance().Packet(packFiles, partName, path, tarMap, reportCb);
            UpdateTarStat(TarFile::GetInstance().GetTarFileSize());
            tar.insert(tarMap.begin(), tarMap.end());
            // 执行tar包回传功能
            WaitToSendFd(startTime, fdNum);
            IncrementalTarFileReady(tarMap, tarInfos, proxy);
            totalSize = 0;
            fileCount = 0;
            packFiles.clear();
            tarInfos.clear();
            fdNum += BConstants::FILE_AND_MANIFEST_FD_COUNT;
            RefreshTimeInfo(startTime, fdNum);
        }
    }
    if (fileCount > 0) {
        // 打包回传
        TarMap tarMap {};
        TarFile::GetInstance().Packet(packFiles, partName, path, tarMap, reportCb);
        UpdateTarStat(TarFile::GetInstance().GetTarFileSize());
        IncrementalTarFileReady(tarMap, tarInfos, proxy);
        fdNum = 1;
        WaitToSendFd(startTime, fdNum);
        tar.insert(tarMap.begin(), tarMap.end());
        packFiles.clear();
        tarInfos.clear();
        RefreshTimeInfo(startTime, fdNum);
    }
    appStatistic_->tarSpend_ = TimeUtils::GetSpendMS(tarStart);
}

int BackupExtExtension::DoIncrementalBackup(const vector<struct ReportFileInfo> &allFiles,
                                            const vector<struct ReportFileInfo> &smallFiles,
                                            const vector<struct ReportFileInfo> &bigFiles)
{
    HILOGI("Do increment backup begin");
    if (extension_ == nullptr) {
        HILOGE("Failed to do incremental backup, extension is nullptr");
        throw BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr");
    }
    if (extension_->GetExtensionAction() != BConstants::ExtensionAction::BACKUP) {
        return EPERM;
    }
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
    if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
        throw BError(errno);
    }
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
    }
    // 获取增量文件和全量数据
    if (smallFiles.empty() && bigFiles.empty()) {
        // 没有增量，则不需要上传
        TarMap tMap;
        ErrCode err = IncrementalAllFileReady(tMap, allFiles, proxy);
        HILOGI("Do increment backup, IncrementalAllFileReady end, file empty");
        return err;
    }
    // tar包数据
    TarMap tarMap;
    IncrementalPacket(smallFiles, tarMap, proxy);
    HILOGI("Do increment backup, IncrementalPacket end");
    // 最后回传大文件
    TarMap bigMap = GetIncrmentBigInfos(bigFiles);
    IncrementalBigFileReady(bigMap, bigFiles, proxy);
    HILOGI("Do increment backup, IncrementalBigFileReady end");
    bigMap.insert(tarMap.begin(), tarMap.end());
    // 回传manage.json和全量文件
    ErrCode err = IncrementalAllFileReady(bigMap, allFiles, proxy);
    HILOGI("End, bigFiles num:%{public}zu, smallFiles num:%{public}zu, allFiles num:%{public}zu", bigFiles.size(),
        smallFiles.size(), allFiles.size());
    return err;
}

std::function<void(ErrCode, std::string)> BackupExtExtension::ReportOnProcessResultCallback(
    wptr<BackupExtExtension> obj, BackupRestoreScenario scenario)
{
    return [obj, scenario](ErrCode errCode, const std::string processInfo) {
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("Async call js onPreocess callback failed, exPtr is empty");
            return;
        }
        if (extPtr->onProcessTimeout_.load()) {
            HILOGE("The result of invoking onProcess is timeout.");
            extPtr->onProcessTimeout_.store(false);
            return;
        }
        extPtr->CloseOnProcessTimeOutTimer();
        extPtr->isFirstCallOnProcess_.store(false);
        extPtr->onProcessTimeout_.store(false);
        if (extPtr->onProcessTimeoutCnt_.load() > 0) {
            extPtr->onProcessTimeoutCnt_ = 0;
            HILOGI("onProcess execute success, reset onProcessTimeoutCnt");
        }
        if (processInfo.size() == 0) {
            HILOGE("Current extension has no js method named onProcess.");
            std::unique_lock<std::mutex> lock(extPtr->onProcessLock_);
            extPtr->isFirstCallOnProcess_.store(false);
            extPtr->stopCallJsOnProcess_.store(true);
            extPtr->execOnProcessCon_.notify_one();
            lock.unlock();
            return;
        }
        std::string processInfoJsonStr;
        BJsonUtil::BuildOnProcessRetInfo(processInfoJsonStr, processInfo);
        auto task = [obj, scenario, processInfoJsonStr]() {
            auto reportExtPtr = obj.promote();
            if (reportExtPtr == nullptr) {
                HILOGE("Report onProcess Result error, reportExtPtr is empty");
                return;
            }
            reportExtPtr->ReportAppProcessInfo(processInfoJsonStr, scenario);
        };
        extPtr->reportOnProcessRetPool_.AddTask([task]() { task(); });
    };
}

void BackupExtExtension::FillFileInfosWithoutCmp(vector<struct ReportFileInfo> &allFiles,
                                                 vector<struct ReportFileInfo> &smallFiles,
                                                 vector<struct ReportFileInfo> &bigFiles,
                                                 UniqueFd incrementalFd)
{
    HILOGI("Fill file info without cmp begin");
    BReportEntity storageRp(move(incrementalFd));
    unordered_map<string, struct ReportFileInfo> localFilesInfo;
    while (storageRp.GetStorageReportInfos(localFilesInfo)) {
        for (auto localIter = localFilesInfo.begin(); localIter != localFilesInfo.end(); ++localIter) {
            const string &path = localIter->second.filePath;
            if (path.empty()) {
                HILOGE("GetStorageReportInfos failed");
                continue;
            }
            if (localIter->second.isIncremental && localIter->second.isDir) {
                smallFiles.emplace_back(localIter->second);
            }
            if (localIter->second.isDir) {
                allFiles.emplace_back(localIter->second);
                continue;
            }
            auto [res, fileHash] = BackupFileHash::HashWithSHA256(path);
            if (fileHash.empty()) {
                HILOGE("Do hash err, fileHash is empty, path: %{public}s", GetAnonyPath(path).c_str());
                continue;
            }
            localIter->second.hash = fileHash;
            if (ExtractFileExt(path) == "tar") {
                localIter->second.userTar = 1; // 1: default value, means true
            }
            allFiles.emplace_back(localIter->second);
            if (!localIter->second.isIncremental) {
                HILOGE("It's not incre, no need record %{public}s", GetAnonyPath(path).c_str());
                continue;
            }
            if (localIter->second.size <= BConstants::BIG_FILE_BOUNDARY) {
                smallFiles.emplace_back(localIter->second);
                continue;
            }
            bigFiles.emplace_back(localIter->second);
        }
        localFilesInfo.clear();
    }
}

void BackupExtExtension::FillFileInfosWithCmp(vector<struct ReportFileInfo> &allFiles,
                                              vector<struct ReportFileInfo> &smallFiles,
                                              vector<struct ReportFileInfo> &bigFiles,
                                              const unordered_map<string, struct ReportFileInfo> &cloudFiles,
                                              UniqueFd incrementalFd)
{
    HILOGI("Fill file info with cmp begin");
    BReportEntity storageRp(move(incrementalFd));
    unordered_map<string, struct ReportFileInfo> localFilesInfo;
    while (storageRp.GetStorageReportInfos(localFilesInfo)) {
        CompareFiles(allFiles, smallFiles, bigFiles, cloudFiles, localFilesInfo);
        localFilesInfo.clear();
    }
}

void BackupExtExtension::CompareFiles(vector<struct ReportFileInfo> &allFiles,
                                      vector<struct ReportFileInfo> &smallFiles,
                                      vector<struct ReportFileInfo> &bigFiles,
                                      const unordered_map<string, struct ReportFileInfo> &cloudFiles,
                                      unordered_map<string, struct ReportFileInfo> &localFilesInfo)
{
    for (auto localIter = localFilesInfo.begin(); localIter != localFilesInfo.end(); ++localIter) {
        // 进行文件对比, 当后续使用 isUserTar 字段时需注意 字段解析函数
        const string &path = localIter->second.filePath;
        if (path.empty()) {
            HILOGE("GetStorageReportInfos failed");
            continue;
        }
        auto it = cloudFiles.find(path);
        bool isExist = (it != cloudFiles.end());
        if (localIter->second.isIncremental && !isExist && localIter->second.isDir) {
            smallFiles.emplace_back(localIter->second);
        }
        if (localIter->second.isDir) {
            allFiles.emplace_back(localIter->second);
            continue;
        }
        bool isChange = !(isExist && localIter->second.size == it->second.size &&
            localIter->second.mtime == it->second.mtime);
        if (isChange) {
            auto [res, fileHash] = BackupFileHash::HashWithSHA256(path);
            if (fileHash.empty()) {
                HILOGE("Do hash err, fileHash is empty");
                continue;
            }
            localIter->second.hash = fileHash;
        } else {
            localIter->second.hash = it->second.hash;
        }

        if (ExtractFileExt(path) == "tar") {
            localIter->second.userTar = 1; // 1: default value, means true
        }

        allFiles.emplace_back(localIter->second);
        if (localIter->second.isIncremental && (!isExist ||
            it->second.hash != localIter->second.hash)) {
            // 在云空间简报里不存在或者hash不一致
            if (localIter->second.size <= BConstants::BIG_FILE_BOUNDARY) {
                smallFiles.emplace_back(localIter->second);
                continue;
            }
            bigFiles.emplace_back(localIter->second);
        }
    }
}

bool BackupExtExtension::IfCloudSpecialRestore(string tarName)
{
    unordered_map<string, struct ReportFileInfo> result;
    GetTarIncludes(tarName, result);
    if (result.empty()) {
        HILOGI("is not CloudSpecialRestore");
        return false;
    }
    HILOGI("is CloudSpecialRestore");
    return true;
}

ErrCode BackupExtExtension::CloudSpecialRestore(string tarName, string untarPath, off_t tarFileSize)
{
    unordered_map<string, struct ReportFileInfo> result;
    GetTarIncludes(tarName, result);
    if (isDebug_) {
        FillEndFileInfos(untarPath, result);
    }
    auto unPacketRes = UntarFile::GetInstance().IncrementalUnPacket(tarName, untarPath, result);
    ErrCode err = ERR_OK;
    err = std::get<FIRST_PARAM>(unPacketRes);
    DealIncreUnPacketResult(tarFileSize, tarName, unPacketRes);
    HILOGI("Application recovered successfully, package path is %{public}s", tarName.c_str());
    DeleteBackupIncrementalTars(tarName);
    return err;
}

void BackupExtExtension::RmBigFileReportForSpecialCloneCloud(const std::string &srcFileName)
{
    // 删除大文件的rp文件
    string reportFileName = GetReportFileName(srcFileName);
    std::unique_lock<std::mutex> lock(reportHashLock_);
    if (reportHashSrcPathMap_.empty()) {
        if (!RemoveFile(reportFileName)) {
            HILOGE("Failed to delete backup report %{public}s, err = %{public}d",
                GetAnonyPath(reportFileName).c_str(), errno);
        }
        return;
    }
    auto iter = reportHashSrcPathMap_.find(srcFileName);
    if (iter == reportHashSrcPathMap_.end()) {
        if (!RemoveFile(reportFileName)) {
            HILOGE("Failed to remove cuurent file report %{public}s, err = %{public}d",
                GetAnonyPath(reportFileName).c_str(), errno);
        }
        return;
    }
    std::string reportHashFilePath = iter->second;
    HILOGI("Will remove current reportHashFile, reportHashFilePath:%{public}s",
        GetAnonyPath(reportHashFilePath).c_str());
    if (!RemoveFile(reportHashFilePath)) {
        HILOGE("Failed to delete backup report %{public}s, err = %{public}d",
            GetAnonyPath(reportHashFilePath).c_str(), errno);
    }
    reportHashSrcPathMap_.erase(iter);
}

void BackupExtExtension::CalculateDataSizeTask(const string &config)
{
    if (!StopExtTimer()) {
        throw BError(BError::Codes::EXT_TIMER_ERROR, "Failed to stop extTimer");
    }
    int64_t totalSize = 0;
    TarMap bigFileInfo;
    map<string, size_t> smallFiles;
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(config);
    auto cache = cachedEntity.Structuralize();
    DoBackupStart();
    auto [err, includeSize, excludeSize] = CalculateDataSize(cache, totalSize, bigFileInfo, smallFiles);
    ScanFileSingleton::GetInstance().SetIncludeSize(includeSize);
    ScanFileSingleton::GetInstance().SetExcludeSize(excludeSize);
    if (err != ERR_OK) {
        throw BError(BError::Codes::EXT_INVAL_ARG, "Failed to mkdir");
    }
    if (!RefreshDataSize(totalSize)) {
        throw BError(BError::Codes::EXT_INVAL_ARG, "Failed to RefreshDataSize");
    }
    bool isFwkStart;
    StartFwkTimer(isFwkStart);
    if (!isFwkStart) {
        HILOGE("Do backup, start fwk timer fail.");
        throw BError(BError::Codes::EXT_TIMER_ERROR, "Failed to start fwkTimer");
    }
}

void BackupExtExtension::DoBackUpTask(const string &config)
{
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(config);
    auto cache = cachedEntity.Structuralize();
    vector<string> excludes = cache.GetExcludes();
    vector<string> endExcludes = excludes;
    PreDealExcludes(endExcludes);

    int ret = 0;
    TarMap fileBackupedInfo;
    while (!ScanFileSingleton::GetInstance().GetCompletedFlag()) {
        ScanFileSingleton::GetInstance().WaitForFiles();
        std::map<std::string, struct stat> incFiles = ScanFileSingleton::GetInstance().GetAllBigFiles();
        if (incFiles.empty()) {
            continue;
        }
        map<string, struct stat> bigFiles = MatchFiles(incFiles, endExcludes);
        TarMap bigFileInfo = convertFileToBigFiles(bigFiles);
        ret = DoBackupBigFiles(bigFileInfo, fileBackupedInfo.size());
        fileBackupedInfo.insert(bigFileInfo.begin(), bigFileInfo.end());
    }

    map<string, size_t> incSmallFiles = ScanFileSingleton::GetInstance().GetAllSmallFiles();
    map<string, size_t> smallFiles = MatchFiles(incSmallFiles, endExcludes);

    std::map<std::string, struct stat> incFiles = ScanFileSingleton::GetInstance().GetAllBigFiles();
    map<string, struct stat> bigFiles = MatchFiles(incFiles, endExcludes);
    TarMap bigFileInfo = convertFileToBigFiles(bigFiles);
    uint32_t includeSize = ScanFileSingleton::GetInstance().GetIncludeSize();
    uint32_t excludeSize = ScanFileSingleton::GetInstance().GetExcludeSize();

    ret = DoBackup(bigFileInfo, fileBackupedInfo, smallFiles, includeSize, excludeSize);
    DoBackupEnd();
    ScanFileSingleton::GetInstance().SetCompletedFlag(false);
    AppDone(ret);
    HILOGI("backup app done %{public}d", ret);
}

template <typename T>
std::map<string, T> BackupExtExtension::MatchFiles(map<string, T> files, vector<string> endExcludes)
{
    auto isMatch = [](const vector<string> &s, const string &str) -> bool {
        if (str.empty() || s.empty()) {
            return false;
        }
        for (const string &item : s) {
            if (fnmatch(item.data(), str.data(), FNM_LEADING_DIR) == 0) {
                return true;
            }
        }
        return false;
    };

    std::map<std::string, T> excludesFiles;
    for (const auto &item : files) {
        if (!isMatch(endExcludes, item.first)) {
            excludesFiles.emplace(item);
        }
    }
    return excludesFiles;
}

int BackupExtExtension::DoBackupBigFiles(TarMap &bigFileInfo, uint32_t backupedFileSize)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Start do backup big files, bigFileInfo size: %{public}zu", bigFileInfo.size());
    if (extension_ == nullptr) {
        HILOGE("Failed to do backup big files, extension is nullptr.");
        return EPERM;
    }
    if (extension_->GetExtensionAction() != BConstants::ExtensionAction::BACKUP) {
        HILOGE("Failed to do backup big files, extension action is not back up.");
        return EPERM;
    }

    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to do backup big files, proxy is nullptr.");
        return EPERM;
    }

    auto res = BigFileReady(bigFileInfo, proxy, backupedFileSize);
    HILOGI("HandleBackup finish, ret = %{public}d", res);
    return res;
}

TarMap BackupExtExtension::convertFileToBigFiles(std::map<std::string, struct stat> files)
{
    auto getStringHash = [](const TarMap &m, const string &str) -> string {
        ostringstream strHex;
        strHex << hex;

        hash<string> strHash;
        size_t szHash = strHash(str);
        strHex << setfill('0') << setw(BConstants::BIG_FILE_NAME_SIZE) << szHash;
        string name = strHex.str();
        for (int i = 0; m.find(name) != m.end(); ++i, strHex.str("")) {
            szHash = strHash(str + to_string(i));
            strHex << setfill('0') << setw(BConstants::BIG_FILE_NAME_SIZE) << szHash;
            name = strHex.str();
        }
        return name;
    };

    TarMap bigFileInfo;
    for (const auto& item : files) {
        string md5Name = getStringHash(bigFileInfo, item.first);
        if (!md5Name.empty()) {
            bigFileInfo.emplace(md5Name, make_tuple(item.first, item.second, true));
        }
    }
    return bigFileInfo;
}

void BackupExtExtension::PreDealExcludes(std::vector<std::string> &excludes)
{
    size_t lenEx = excludes.size();
    int j = 0;
    for (size_t i = 0; i < lenEx; ++i) {
        if (!excludes[i].empty()) {
            if (excludes[i].at(excludes[i].size() - 1) == BConstants::FILE_SEPARATOR_CHAR) {
                excludes[i] += "*";
            }
            if (excludes[i].find(BConstants::FILE_SEPARATOR_CHAR) != string::npos &&
                excludes[i].at(0) != BConstants::FILE_SEPARATOR_CHAR) {
                excludes[i] = BConstants::FILE_SEPARATOR_CHAR + excludes[i];
            }
            excludes[j++] = excludes[i];
        }
    }
    excludes.resize(j);
}

ErrCode BackupExtExtension::GetIncrementalBackupFileHandle(int &fd, int &reportFd)
{
    auto [fdval, reportFdval] = GetIncrementalBackupFileHandle();
    fd = dup(fdval.Get());
    reportFd = dup(reportFdval.Get());
    return BError(BError::Codes::OK).GetCode();
}

tuple<UniqueFd, UniqueFd> BackupExtExtension::GetIncrementalBackupFileHandle()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    return {UniqueFd(BConstants::INVALID_FD_NUM), UniqueFd(BConstants::INVALID_FD_NUM)};
}

std::function<void(ErrCode, const std::string)> BackupExtExtension::OnReleaseCallback(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get HandleOnReleaseCallback");
    return [obj](ErrCode errCode, std::string errMsg) {
        HILOGI("OnReleaseCallback: App onRelease end");
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        if (extPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        if (extPtr->stopWaitOnRelease_.load()) {
            HILOGE("App onRelease timeout");
            return;
        }
        std::unique_lock<std::mutex> lock(extPtr->execOnReleaseLock_);
        extPtr->stopWaitOnRelease_.store(true);
        extPtr->execOnReleaseCon_.notify_all();
        HILOGI("Extension onRelease end, errCode: %{public}d, errInfo: %{public}s", errCode, errMsg.c_str());
    };
}

ErrCode BackupExtExtension::HandleOnRelease(int32_t scenario)
{
    try {
        HILOGI("HandleOnRelease Begin, scenario: %{public}d", scenario);
        VerifyCaller();
        std::unique_lock<std::mutex> onReleaseLock(onReleaseLock_);
        if (isOnReleased_.load()) {
            HILOGE("onRelease done, not need to do again");
            return ERR_OK;
        }
        auto task = [obj {wptr<BackupExtExtension>(this)}, scenario]() {
            auto extPtr = obj.promote();
            if (extPtr == nullptr || extPtr->extension_ == nullptr) {
                HILOGE("Call js onRelease failed, extensionPtr is empty");
                return;
            }
            auto callback = extPtr->OnReleaseCallback(obj);
            ErrCode ret = extPtr->extension_->OnRelease(callback, scenario);
            if (ret != ERR_OK) {
                HILOGE("Call onRelease failed, ret = %{public}d", ret);
            }
        };
        onReleaseTaskPool_.AddTask([task]() { task(); });
        HILOGI("Wait onRelease to do");
        std::unique_lock<std::mutex> lock(execOnReleaseLock_);
        execOnReleaseCon_.wait_for(lock, std::chrono::milliseconds(BConstants::APP_ON_RELEASE_MAX_TIMEOUT),
            [this] { return stopWaitOnRelease_.load(); });
        stopWaitOnRelease_.store(true);
        isOnReleased_.store(true);
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to HandleOnRelease");
        return BError(BError::Codes::EXT_BROKEN_IPC).GetCode();
    }
}

void BackupExtExtension::HandleExtDisconnect(bool isAppResultReport, ErrCode errCode)
{
    HILOGI("Begin, scenario:%{public}d, isAppResultReport:%{public}d, errCode:%{public}d", curScenario_,
        isAppResultReport, errCode);
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to obtain the ServiceClient handle");
        return;
    }
    auto ret = proxy->HandleExtDisconnect(curScenario_, isAppResultReport, errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed to HandleExtDisconnect. err = %{public}d", ret);
    }
}

bool BackupExtExtension::HandleGetExtOnRelease()
{
    HILOGI("HandleGetExtOnRelease begin");
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to obtain the ServiceClient handle");
        return false;
    }
    bool isExtOnRelease = false;
    auto ret = proxy->GetExtOnRelease(isExtOnRelease);
    if (ret != ERR_OK) {
        HILOGE("Failed to GetExtOnRelease. err = %{public}d", ret);
    }
    return isExtOnRelease;
}

void BackupExtExtension::HandleExtOnRelease(bool isAppResultReport, ErrCode errCode)
{
    HILOGI("HandleExtOnRelease begin");
    int32_t scenario = static_cast<int32_t>(BConstants::ExtensionScenario::RESTORE);
    if (curScenario_ == BackupRestoreScenario::FULL_BACKUP ||
        curScenario_ == BackupRestoreScenario::INCREMENTAL_BACKUP) {
        scenario = static_cast<int32_t>(BConstants::ExtensionScenario::BACKUP);
    }
    auto task = [obj {wptr<BackupExtExtension>(this)}, scenario, isAppResultReport, errCode]() {
        auto extPtr = obj.promote();
        if (extPtr == nullptr || extPtr->extension_ == nullptr) {
            HILOGE("Call js onRelease failed, extensionPtr is empty");
            return;
        }
        {
            std::unique_lock<std::mutex> onReleaseLock(extPtr->onReleaseLock_);
            if (extPtr->isOnReleased_.load()) {
                HILOGE("onRelease done, not need to do again");
                return;
            }
            auto callback = extPtr->OnReleaseCallback(obj);
            ErrCode ret = extPtr->extension_->OnRelease(callback, scenario);
            if (ret != ERR_OK) {
                HILOGE("Call onRelease failed, ret = %{public}d", ret);
                return;
            }
            std::unique_lock<std::mutex> lock(extPtr->execOnReleaseLock_);
            extPtr->execOnReleaseCon_.wait_for(lock, std::chrono::milliseconds(BConstants::APP_ON_RELEASE_MAX_TIMEOUT),
                [extPtr] { return extPtr->stopWaitOnRelease_.load(); });
            extPtr->stopWaitOnRelease_.store(true);
            extPtr->isOnReleased_.store(true);
        }
        extPtr->HandleExtDisconnect(isAppResultReport, errCode);
    };
    onReleaseTaskPool_.AddTask([task]() { task(); });
}

std::function<void(ErrCode, const std::string)> BackupExtExtension::GetComInfoCallback(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get GetComInfoCallback");
    return [obj](ErrCode errCode, std::string compatibilityInfo) {
        HILOGI("GetComInfoCallback: App getCompatibilityInfo end");
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        if (extPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        if (extPtr->stopGetComInfo_.load()) {
            HILOGE("App getCompatibilityInfo timeout");
            return;
        }
        HILOGI("GetCompatibilityInfo end, errCode: %{public}d, compatibilityInfo size is %{public}zu",
            errCode, compatibilityInfo.size());
        std::unique_lock<std::mutex> lock(extPtr->getCompatibilityInfoLock_);
        extPtr->compatibilityInfo_ = compatibilityInfo;
        extPtr->stopGetComInfo_.store(true);
        extPtr->getCompatibilityInfoCon_.notify_all();
    };
}

ErrCode BackupExtExtension::HandleGetCompatibilityInfo(const string &extInfo, int32_t scenario,
    string &compatibilityInfo)
{
    try {
        HILOGI("Begin, scenario: %{public}d, extInfo size: %{public}zu", scenario, extInfo.size());
        VerifyCaller();
        auto ptr = wptr<BackupExtExtension>(this);
        auto callback = GetComInfoCallback(ptr);
        ErrCode ret = ERR_OK;
        compatibilityInfo = "";
        if (scenario == BConstants::ExtensionScenario::BACKUP) {
            ret = extension_->GetBackupCompatibilityInfo(callback, extInfo);
        } else if (scenario == BConstants::ExtensionScenario::RESTORE) {
            ret = extension_->GetRestoreCompatibilityInfo(callback, extInfo);
        } else {
            return BError(BError::Codes::EXT_INVAL_ARG).GetCode();
        }
        if (ret != ERR_OK) {
            HILOGE("Call GetCompatibilityInfo failed, ret = %{public}d", ret);
            return ret;
        }
        HILOGI("wait GetCompatibilityInfo");
        std::unique_lock<std::mutex> lock(getCompatibilityInfoLock_);
        auto noTimeout = getCompatibilityInfoCon_.wait_for(lock,
            std::chrono::milliseconds(BConstants::APP_GETCOMINFO_MAX_TIMEOUT),
            [this] { return this->stopGetComInfo_.load(); });
        if (noTimeout) {
            compatibilityInfo = compatibilityInfo_;
        }
        HILOGI("getCompatibilityInfo size: %{public}zu", compatibilityInfo.size());
        return ret;
    } catch (...) {
        HILOGE("Failed to HandleGetCompatibilityInfo");
        return BError(BError::Codes::EXT_BROKEN_IPC).GetCode();
    }
}
} // namespace OHOS::FileManagement::Backup
