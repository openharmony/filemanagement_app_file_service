/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "module_ipc/service.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <regex>

#include <fcntl.h>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>

#include <directory_ex.h>

#include "ability_manager_client.h"
#include "accesstoken_kit.h"
#include "b_anony/b_anony.h"
#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_filesystem/b_dir.h"
#include "b_file_info.h"
#include "b_hiaudit/hi_audit.h"
#include "b_json/b_json_cached_entity.h"
#include "b_jsonutil/b_jsonutil.h"
#include "b_ohos/startup/backup_para.h"
#include "b_process/b_multiuser.h"
#include "b_radar/b_radar.h"
#include "b_radar/radar_app_statistic.h"
#include "b_resources/b_constants.h"
#include "b_sa/b_sa_utils.h"
#include "b_utils/b_time.h"
#include "bundle_mgr_client.h"
#include "filemgmt_libhilog.h"
#include "hisysevent.h"
#include "hitrace_meter.h"
#include "ipc_skeleton.h"
#include "access_token.h"
#include "tokenid_kit.h"
#include "module_app_gallery/app_gallery_dispose_proxy.h"
#include "module_external/bms_adapter.h"
#include "module_external/sms_adapter.h"
#include "module_ipc/svc_backup_connection.h"
#include "module_ipc/svc_restore_deps_manager.h"
#include "module_notify/notify_work_service.h"
#include "parameter.h"
#include "parameters.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
const int32_t MAX_FILE_READY_REPORT_TIME = 2;
const int32_t WAIT_SCANNING_INFO_SEND_TIME = 5;
const int ERR_SIZE = -1;
} // namespace

void Service::AppendBundles(const std::vector<std::string> &bundleNames)
{
    std::vector<std::string> failedBundles;
    session_->AppendBundles(bundleNames, failedBundles);
    if (!failedBundles.empty()) {
        HILOGE("Handle exception on failed bundles, size = %{public}zu", failedBundles.size());
        HandleExceptionOnAppendBundles(session_, failedBundles, {});
    }
}

void Service::ReportOnBundleStarted(IServiceReverseType::Scenario scenario, const std::string &bundleName)
{
    if (scenario == IServiceReverseType::Scenario::BACKUP) {
        session_->GetServiceReverseProxy()->BackupOnBundleStarted(BError(BError::Codes::SA_INVAL_ARG), bundleName);
    } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
        session_->GetServiceReverseProxy()->RestoreOnBundleStarted(BError(BError::Codes::SA_INVAL_ARG), bundleName);
    }
}

string Service::BundleNameWithUserId(const string& bundleName, const int32_t userId)
{
    return to_string(userId) + "-" + bundleName;
}

std::tuple<std::string, int32_t> Service::SplitBundleName(const string& bundleNameWithId)
{
    size_t found = bundleNameWithId.find('-');
    if (found == std::string::npos) {
        HILOGE("Can not split bundleName = %{public}s", bundleNameWithId.c_str());
        return { "", -1 };
    }
    std::string bundleName = bundleNameWithId.substr(found + 1, bundleNameWithId.length());
    int32_t userId = std::atoi(bundleNameWithId.substr(0, found).c_str());
    return { bundleName, userId };
}

vector<BIncrementalData> Service::MakeDetailList(const vector<BundleName> &bundleNames)
{
    vector<BIncrementalData> bundleDetails {};
    for (const auto &bundleName : bundleNames) {
        bundleDetails.emplace_back(BIncrementalData {bundleName, 0});
    }
    return bundleDetails;
}

void Service::UpdateBundleRadarReport(const std::string &bundleName)
{
    std::unique_lock<std::mutex> lock(bundleExecRadarLock_);
    bundleExecRadarSet_.insert(bundleName);
}

void Service::ClearBundleRadarReport()
{
    std::unique_lock<std::mutex> lock(bundleExecRadarLock_);
    bundleExecRadarSet_.clear();
}

bool Service::IsReportBundleExecFail(const std::string &bundleName)
{
    std::unique_lock<std::mutex> lock(bundleExecRadarLock_);
    auto it = bundleExecRadarSet_.find(bundleName);
    if (it != bundleExecRadarSet_.end()) {
        HILOGI("BundleName %{public}s has already been reported", bundleName.c_str());
        return false;
    }
    return true;
}

void Service::ClearFileReadyRadarReport()
{
    std::unique_lock<std::mutex> lock(fileReadyRadarLock_);
    fileReadyRadarMap_.clear();
}

bool Service::IsReportFileReadyFail(const std::string &bundleName)
{
    std::unique_lock<std::mutex> lock(fileReadyRadarLock_);
    auto it = fileReadyRadarMap_.find(bundleName);
    if (it != fileReadyRadarMap_.end()) {
        it->second++;
    } else {
        fileReadyRadarMap_[bundleName] = 1;
    }
    if (it->second > MAX_FILE_READY_REPORT_TIME) {
        HILOGI("FileReady radar report more than %{public}d times, bundleName = %{public}s",
            MAX_FILE_READY_REPORT_TIME, bundleName.c_str());
        return false;
    }
    return true;
}

void Service::TimeoutRadarReport(IServiceReverseType::Scenario scenario, std::string &bundleName)
{
    if (!IsReportBundleExecFail(bundleName)) {
        return;
    }
    UpdateBundleRadarReport(bundleName);
    int32_t errCode = BError::BackupErrorCode::E_ETO;
    if (session_->GetTimeoutValue(bundleName) == 0) {
        errCode = BError::BackupErrorCode::E_FORCE_TIMEOUT;
    }
    if (scenario == IServiceReverseType::Scenario::BACKUP) {
        AppRadar::Info info(bundleName, "", "on backup timeout");
        AppRadar::GetInstance().RecordBackupFuncRes(info, "Service::TimeOutCallback", GetUserIdDefault(),
            BizStageBackup::BIZ_STAGE_ON_BACKUP, errCode);
    } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
        AppRadar::Info info(bundleName, "", "on restore timeout");
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::TimeOutCallback", GetUserIdDefault(),
            BizStageRestore::BIZ_STAGE_ON_RESTORE, errCode);
    }
}

ErrCode Service::Finish()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        ErrCode ret = VerifyCaller(session_->GetScenario());
        if (ret != ERR_OK) {
            HILOGE("Failde to Finish, verify caller failed, ret:%{public}d", ret);
            ReleaseOnException();
            return ret;
        }
        ret = session_->Finish();
        if (ret != ERR_OK) {
            HILOGE("Failde to Finish, session finish failed, ret:%{public}d", ret);
            ReleaseOnException();
            return ret;
        }
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        ReleaseOnException();
        HILOGE("Failde to Finish");
        return e.GetCode();
    }
}

ErrCode Service::GetFileHandle(const string &bundleName, const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_ == nullptr) {
            HILOGE("GetFileHandle error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::RESTORE);
        if (ret != ERR_OK) {
            HILOGE("verify caller failed, bundleName:%{public}s", bundleName.c_str());
            return ret;
        }
        if (!BDir::IsFilePathValid(fileName)) {
            HILOGE("path is forbidden, path : %{public}s", GetAnonyPath(fileName).c_str());
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        bool updateRes = SvcRestoreDepsManager::GetInstance().UpdateToRestoreBundleMap(bundleName, fileName);
        if (updateRes) {
            return BError(BError::Codes::OK);
        }
        auto action = session_->GetServiceSchedAction(bundleName);
        if (action == BConstants::ServiceSchedAction::UNKNOWN) {
            HILOGE("action is unknown, bundleName:%{public}s", bundleName.c_str());
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        if (action == BConstants::ServiceSchedAction::RUNNING) {
            auto err = SendFileHandle(bundleName, fileName);
            if (err != ERR_OK) {
                HILOGE("SendFileHandle failed, bundle:%{public}s", bundleName.c_str());
                return err;
            }
        } else {
            session_->SetExtFileNameRequest(bundleName, fileName);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    }
}

ErrCode Service::SendFileHandle(const std::string &bundleName, const std::string &fileName)
{
    auto backUpConnection = session_->GetExtConnection(bundleName);
    if (backUpConnection == nullptr) {
        HILOGE("backUpConnection is empty, bundle:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("GetFileHandle failed, bundleName:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    int32_t errCode = 0;
    int32_t fdCode = 0;
    proxy->GetFileHandleWithUniqueFd(fileName, errCode, fdCode);
    UniqueFd fd(fdCode);
    if (errCode != ERR_OK) {
        AppRadar::Info info(bundleName, "", "");
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::GetFileHandle", GetUserIdDefault(),
                                                     BizStageRestore::BIZ_STAGE_GET_FILE_HANDLE_FAIL, errCode);
    }
    session_->GetServiceReverseProxy()->RestoreOnFileReady(bundleName, fileName, move(fd), errCode);
    FileReadyRadarReport(bundleName, fileName, errCode, IServiceReverseType::Scenario::RESTORE);
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (session_ == nullptr) {
        HILOGE("PublishFile error, session is empty");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::RESTORE);
    if (ret != ERR_OK) {
        HILOGE("PublishFile error, verify caller by scenario failed, ret:%{public}d", ret);
        return ret;
    }
    if (!fileInfo.fileName.empty()) {
        HILOGE("Forbit to use publishFile with fileName for App");
        return EPERM;
    }
    auto backUpConnection = session_->GetExtConnection(fileInfo.owner);
    if (backUpConnection == nullptr) {
        HILOGE("backUpConnection is empty, bundle:%{public}s", fileInfo.owner.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("PublishFile error, Extension backup Proxy is empty");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    ret = proxy->PublishFile(fileInfo.fileName);
    if (ret != ERR_OK) {
        HILOGE("Failed to publish file for backup extension, ret:%{public}d", ret);
        return ret;
    }
    return BError(BError::Codes::OK);
}

ErrCode Service::AppFileReady(const std::string &fileName, int fd, int32_t errCode)
{
    HILOGI("Begin fileName =%{public}s, fd = %{public}d, errCode = %{public}d", fileName.c_str(), fd, errCode);
    UniqueFd fdUnique(fd);
    return AppFileReady(fileName, std::move(fdUnique), errCode);
}

ErrCode Service::AppFileReadyWithoutFd(const std::string &fileName, int32_t errCode)
{
    return AppFileReady(fileName, UniqueFd(INVALID_FD), errCode);
}

ErrCode Service::AppFileReady(const string &fileName, UniqueFd fd, int32_t errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_ == nullptr) {
            HILOGE("AppFileReady error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName);
        if (ret != ERR_OK) {
            HILOGE("AppFileReady error, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        if (fileName.find('/') != string::npos) {
            HILOGE("AppFileReady error, filename is not valid, fileName:%{public}s", GetAnonyPath(fileName).c_str());
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        if (fileName == BConstants::EXT_BACKUP_MANAGE) {
            fd = session_->OnBundleExtManageInfo(callerName, move(fd));
        }
        bool fdFlag = fd < 0 ? true : false;
        fdFlag ? session_->GetServiceReverseProxy()->BackupOnFileReadyWithoutFd(callerName, fileName, errCode) :
                 session_->GetServiceReverseProxy()->BackupOnFileReady(callerName, fileName, move(fd), errCode);
        FileReadyRadarReport(callerName, fileName, errCode, session_->GetScenario());
        if (session_->OnBundleFileReady(callerName, fileName)) {
            ret = HandleCurBundleFileReady(callerName, fileName, false);
            if (ret != ERR_OK) {
                HILOGE("Handle current bundle file failed, bundleName:%{public}s, fileName:%{public}s",
                    callerName.c_str(),  GetAnonyPath(fileName).c_str());
                return ret;
            }
        }
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (...) {
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::AppDone(ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_ == nullptr) {
            HILOGE("App finish error, session info is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName);
        if (ret != ERR_OK) {
            HILOGE("App done failed, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        HILOGI("Begin, callerName is: %{public}s, errCode: %{public}d", callerName.c_str(), errCode);
        if (session_->OnBundleFileReady(callerName) || errCode != BError(BError::Codes::OK)) {
            std::shared_ptr<ExtensionMutexInfo> mutexPtr = GetExtensionMutex(callerName);
            if (mutexPtr == nullptr) {
                HILOGE("extension mutex ptr is nullptr, bundleName:%{public}s", callerName.c_str());
                return BError(BError::Codes::SA_INVAL_ARG);
            }
            std::lock_guard<std::mutex> lock(mutexPtr->callbackMutex);
            SetExtOnRelease(callerName, true);
            return BError(BError::Codes::OK);
        }
        RemoveExtensionMutex(callerName);
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        ReleaseOnException();
        HILOGE("AppDone error, err code is: %{public}d", e.GetCode());
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (...) {
        HILOGE("Unexpected exception");
        ReleaseOnException();
        return EPERM;
    }
}

ErrCode Service::LaunchBackupExtension(const BundleName &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("begin %{public}s", bundleName.data());
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    BConstants::ExtensionAction action;
    if (scenario == IServiceReverseType::Scenario::BACKUP || scenario == IServiceReverseType::Scenario::CLEAN) {
        action = BConstants::ExtensionAction::BACKUP;
    } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
        action = BConstants::ExtensionAction::RESTORE;
    } else {
        action = BConstants::ExtensionAction::INVALID;
        HILOGE("Launch current bundle backupExtension failed, action is unknown, bundleName:%{public}s",
            bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    if (SAUtils::IsSABundleName(bundleName)) {
        return LaunchBackupSAExtension(bundleName);
    }
    AAFwk::Want want;
    SetWant(want, bundleName, action);
    auto backUpConnection = session_->GetExtConnection(bundleName);
    if (backUpConnection == nullptr) {
        HILOGE("LaunchBackupExtension error, backUpConnection is empty");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    if (backUpConnection->IsExtAbilityConnected() && !backUpConnection->WaitDisconnectDone()) {
        HILOGE("LaunchBackupExtension error, WaitDisconnectDone failed");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    BConstants::ServiceSchedAction bundleAction = session_->GetServiceSchedAction(bundleName);
    if (bundleAction == BConstants::ServiceSchedAction::UNKNOWN) {
        HILOGE("action is unknown, bundleName:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    ErrCode ret = backUpConnection->ConnectBackupExtAbility(want, session_->GetSessionUserId(),
        bundleAction == BConstants::ServiceSchedAction::CLEAN);
    if (ret != ERR_OK) {
        HILOGE("ConnectBackupExtAbility failed, bundleName:%{public}s, ret:%{public}d", bundleName.c_str(), ret);
        ExtensionConnectFailRadarReport(bundleName, ret, scenario);
        return BError(BError::Codes::SA_BOOT_EXT_FAIL);
    }
    return BError(BError::Codes::OK);
}

void Service::SetWant(AAFwk::Want &want, const BundleName &bundleName, const BConstants::ExtensionAction &action)
{
    BJsonUtil::BundleDetailInfo bundleDetail = BJsonUtil::ParseBundleNameIndexStr(bundleName);
    string backupExtName = session_->GetBackupExtName(bundleName); /* new device app ext name */
    HILOGI("BackupExtName: %{public}s, bundleName: %{public}s", backupExtName.data(), bundleName.data());
    string versionName = session_->GetBundleVersionName(bundleName); /* old device app version name */
    int64_t versionCode = session_->GetBundleVersionCode(bundleName); /* old device app version code */
    RestoreTypeEnum restoreType = session_->GetBundleRestoreType(bundleName); /* app restore type */
    string bundleExtInfo = session_->GetBackupExtInfo(bundleName);
    HILOGI("BundleExtInfo is:%{public}s", GetAnonyString(bundleExtInfo).c_str());
    string oldBackupVersion = session_->GetOldBackupVersion(); /* old device backup version */
    if (oldBackupVersion.empty()) {
        HILOGE("Failed to get backupVersion of old device");
    }
    want.SetElementName(bundleDetail.bundleName, backupExtName);
    want.SetParam(BConstants::EXTENSION_ACTION_PARA, static_cast<int>(action));
    want.SetParam(BConstants::EXTENSION_VERSION_CODE_PARA, static_cast<long>(versionCode));
    want.SetParam(BConstants::EXTENSION_RESTORE_TYPE_PARA, static_cast<int>(restoreType));
    want.SetParam(BConstants::EXTENSION_VERSION_NAME_PARA, versionName);
    want.SetParam(BConstants::EXTENSION_RESTORE_EXT_INFO_PARA, bundleExtInfo);
    want.SetParam(BConstants::EXTENSION_BACKUP_EXT_INFO_PARA, bundleExtInfo);
    want.SetParam(BConstants::EXTENSION_APP_CLONE_INDEX_PARA, bundleDetail.bundleIndex);
    want.SetParam(BConstants::EXTENSION_OLD_BACKUP_VERSION_PARA, oldBackupVersion);
}

std::vector<std::string> Service::GetSupportBackupBundleNames(vector<BJsonEntityCaps::BundleInfo> &backupInfos,
    bool isIncBackup, const std::vector<std::string> &srcBundleNames)
{
    HILOGI("Begin");
    std::vector<std::string> supportBackupNames;
    for (const auto &info : backupInfos) {
        HILOGI("Current backupInfo bundleName:%{public}s, index:%{public}d, extName:%{public}s", info.name.c_str(),
            info.appIndex, info.extensionName.c_str());
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(info.name, info.appIndex);
        if (!info.allToBackup) {
            if (isIncBackup) {
                session_->GetServiceReverseProxy()->IncrementalBackupOnBundleStarted(
                    BError(BError::Codes::SA_FORBID_BACKUP_RESTORE), bundleNameIndexInfo);
            } else {
                session_->GetServiceReverseProxy()->BackupOnBundleStarted(
                    BError(BError::Codes::SA_FORBID_BACKUP_RESTORE), bundleNameIndexInfo);
            }
            BundleBeginRadarReport(bundleNameIndexInfo, BError(BError::Codes::SA_FORBID_BACKUP_RESTORE).GetCode(),
                IServiceReverseType::Scenario::BACKUP);
            continue;
        }
        supportBackupNames.emplace_back(bundleNameIndexInfo);
    }
    HandleNotSupportBundleNames(srcBundleNames, supportBackupNames, isIncBackup);
    HILOGI("End");
    return supportBackupNames;
}

void Service::SetCurrentSessProperties(BJsonEntityCaps::BundleInfo &info,
    std::map<std::string, bool> &isClearDataFlags, const std::string &bundleNameIndexInfo)
{
    HILOGI("Begin");
    if (session_ == nullptr) {
        HILOGE("Set currrent session properties error, session is empty");
        return;
    }
    session_->SetBundleDataSize(bundleNameIndexInfo, info.spaceOccupied);
    auto iter = isClearDataFlags.find(bundleNameIndexInfo);
    if (iter != isClearDataFlags.end()) {
        session_->SetClearDataFlag(bundleNameIndexInfo, iter->second);
    }
    HILOGI("End");
}

ErrCode Service::RefreshDataSize(int64_t totalDataSize)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_ == nullptr) {
            HILOGE("session is nullptr");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        std::string bundleName;
        ErrCode ret = VerifyCallerAndGetCallerName(bundleName);
        if (ret != ERR_OK) {
            HILOGE("Refresh data size failed, bundleName:%{public}s, ret:%{public}d", bundleName.c_str(), ret);
            return ret;
        }
        session_->SetBundleDataSize(bundleName, totalDataSize);
        HILOGI("RefreshDataSize, bundleName:%{public}s ,datasize = %{public}" PRId64 "",
            bundleName.c_str(), totalDataSize);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    }
}


void Service::HandleNotSupportBundleNames(const std::vector<std::string> &srcBundleNames,
    std::vector<std::string> &supportBundleNames, bool isIncBackup)
{
    for (const auto &bundleName : srcBundleNames) {
        auto it = std::find(supportBundleNames.begin(), supportBundleNames.end(), bundleName);
        if (it != supportBundleNames.end()) {
            continue;
        }
        HILOGE("bundleName:%{public}s, can not find from supportBundleNames", bundleName.c_str());
        AppStatReportErr(bundleName, "HandleNotSupportBundleNames",
            RadarError(MODULE_HAP, BError(BError::Codes::SA_BUNDLE_INFO_EMPTY)));
        if (isIncBackup) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnBundleStarted(
                BError(BError::Codes::SA_BUNDLE_INFO_EMPTY), bundleName);
        } else {
            session_->GetServiceReverseProxy()->BackupOnBundleStarted(
                BError(BError::Codes::SA_BUNDLE_INFO_EMPTY), bundleName);
        }
    }
}

ErrCode Service::StopExtTimer(bool &isExtStop)
{
    try {
        HILOGI("Service::StopExtTimer begin.");
        if (session_ == nullptr) {
            HILOGE("StopExtTimer error, session_ is nullptr.");
            isExtStop = false;
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        string bundleName;
        ErrCode ret = VerifyCallerAndGetCallerName(bundleName);
        if (ret != ERR_OK) {
            HILOGE("Stop extension error, ret:%{public}d", ret);
            isExtStop = false;
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return ret;
        }
        isExtStop = session_->StopExtTimer(bundleName);
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (...) {
        HILOGE("Unexpected exception");
        isExtStop = false;
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

void Service::ClearFailedBundles()
{
    std::lock_guard<std::mutex> lock(failedBundlesLock_);
    failedBundles_.clear();
}

void Service::UpdateFailedBundles(const std::string &bundleName, BundleTaskInfo taskInfo)
{
    std::lock_guard<std::mutex> lock(failedBundlesLock_);
    failedBundles_[bundleName] = taskInfo;
}

void Service::GetOldDeviceBackupVersion()
{
    std::string oldBackupVersion = session_->GetOldBackupVersion();
    if (oldBackupVersion.empty()) {
        HILOGE("Failed to get backupVersion of old device");
    }
    HILOGI("backupVersion of old device = %{public}s", oldBackupVersion.c_str());
}

void Service::AppStatReportErr(const string &bundleName, const string &func, RadarError error)
{
    if (totalStatistic_ == nullptr) {
        HILOGE("totalStat is null. appStatReport func:%{public}s, err: %{public}s", func.c_str(),
            error.errMsg_.c_str());
        return;
    }
    RadarAppStatistic appStatistic(bundleName, totalStatistic_->GetUniqId(), totalStatistic_->GetBizScene());
    appStatistic.ReportError(func, error);
}

void Service::SaStatReport(const string &bundleName, const string &func, RadarError err)
{
    if (totalStatistic_ == nullptr) {
        HILOGE("totalStat is null. appStatReport func:%{public}s, err: %{public}s", func.c_str(),
            err.errMsg_.c_str());
        return;
    }

    std::shared_ptr<RadarAppStatistic> saStatistic = nullptr;
    {
        std::shared_lock<std::shared_mutex> mapLock(statMapMutex_);
        if (saStatisticMap_.count(bundleName) > 0) {
            saStatistic = saStatisticMap_[bundleName];
            saStatistic->doBackupSpend_.End();
            saStatistic->doRestoreSpend_ = TimeUtils::GetSpendMS(saStatistic->doRestoreStart_);
        } else {
            saStatistic = std::make_shared<RadarAppStatistic>(bundleName, totalStatistic_->GetUniqId(),
                totalStatistic_->GetBizScene());
        }
    }
    saStatistic->ReportSA(func, err);
}

void Service::ExtConnectDied(const string &callName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin, bundleName: %{public}s", callName.c_str());
        std::shared_ptr<ExtensionMutexInfo> mutexPtr = GetExtensionMutex(callName);
        if (mutexPtr == nullptr) {
            HILOGE("extension mutex ptr is nullptr");
            return;
        }
        std::lock_guard<std::mutex> lock(mutexPtr->callbackMutex);
        /* Clear Timer */
        session_->StopFwkTimer(callName);
        session_->StopExtTimer(callName);
        auto backUpConnection = session_->GetExtConnection(callName);
        if (backUpConnection != nullptr && backUpConnection->IsExtAbilityConnected()) {
            backUpConnection->DisconnectBackupExtAbility();
            AppStatReportErr(callName, "ExtConnectDied", RadarError(MODULE_ABILITY_MGR_SVC,
                backUpConnection->GetError()));
        }
        bool needCleanData = session_->GetClearDataFlag(callName);
        if (!needCleanData || SAUtils::IsSABundleName(callName)) {
            HILOGE("Current extension is died, but not need clean data, bundleName:%{public}s", callName.c_str());
            SendEndAppGalleryNotify(callName);
            ClearSessionAndSchedInfo(callName);
            NoticeClientFinish(callName, BError(BError::Codes::EXT_ABILITY_DIED));
            return;
        }
        session_->SetServiceSchedAction(callName, BConstants::ServiceSchedAction::CLEAN);
        auto ret = LaunchBackupExtension(callName);
        if (ret) {
            /* Clear Session before notice client finish event */
            HILOGE("Current bundle launch extension failed, bundleName:%{public}s", callName.c_str());
            SendEndAppGalleryNotify(callName);
            ClearSessionAndSchedInfo(callName);
        }
        /* Notice Client Ext Ability Process Died */
        NoticeClientFinish(callName, BError(BError::Codes::EXT_ABILITY_DIED));
    } catch (...) {
        HILOGE("Unexpected exception, bundleName: %{public}s", callName.c_str());
        SendEndAppGalleryNotify(callName);
        ClearSessionAndSchedInfo(callName);
        NoticeClientFinish(callName, BError(BError::Codes::EXT_ABILITY_DIED));
    }
    RemoveExtensionMutex(callName);
}

void Service::OnBackupExtensionDied(const string &&bundleName, bool isCleanCalled)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (isCleanCalled) {
        HILOGE("Backup <%{public}s> Extension Process second Died", bundleName.c_str());
        ClearSessionAndSchedInfo(bundleName);
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return;
    }
    try {
        string callName = move(bundleName);
        HILOGE("Backup <%{public}s> Extension Process Died", callName.c_str());
        ErrCode ret = session_->VerifyBundleName(callName);
        if (ret != ERR_OK) {
            HILOGE("Backup Extension died error, verify bundleName failed, bundleName:%{public}s, ret:%{public}d",
                bundleName.c_str(), ret);
            ExtConnectDied(bundleName);
            return;
        }
        // 重新连接清理缓存
        HILOGI("Clear backup extension data, bundleName: %{public}s", callName.c_str());
        ExtConnectDied(callName);
    } catch (...) {
        HILOGE("Unexpected exception, bundleName: %{public}s", bundleName.c_str());
        ExtConnectDied(bundleName);
        return;
    }
}

void Service::NoticeClientFinish(const string &bundleName, ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("begin %{public}s", bundleName.c_str());
    try {
        auto scenario = session_->GetScenario();
        if (scenario == IServiceReverseType::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(errCode, bundleName);
        } else if (scenario == IServiceReverseType::Scenario::RESTORE &&
                BackupPara().GetBackupOverrideIncrementalRestore() &&
                session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleFinished(errCode, bundleName);
        } else if (scenario == IServiceReverseType::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnBundleFinished(errCode, bundleName);
        } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnBundleFinished(errCode, bundleName);
        };
        BundleEndRadarReport(bundleName, errCode, scenario);
        /* If all bundle ext process finish, notice client. */
        OnAllBundlesFinished(BError(BError::Codes::OK));
    } catch(const BError &e) {
        HILOGE("NoticeClientFinish exception, bundleName:%{public}s", bundleName.c_str());
        ReleaseOnException();
        return;
    } catch (...) {
        HILOGE("Unexpected exception");
        ReleaseOnException();
        return;
    }
}

void Service::TotalStatReport(ErrCode errCode)
{
    if (totalStatistic_ == nullptr) {
        HILOGE("totalStat is null");
        return;
    }
    totalStatistic_->totalSpendTime_.End();
    totalStatistic_->Report("OnAllBundlesFinished", errCode);
}

void Service::OnAllBundlesFinished(ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("called begin.");
    if (session_->IsOnAllBundlesFinished()) {
        IServiceReverseType::Scenario scenario = session_->GetScenario();
        TotalStatReport(errCode);
        if (isInRelease_.load() && (scenario == IServiceReverseType::Scenario::RESTORE)) {
            HILOGI("Will destory session info");
            SessionDeactive();
        }
        if (scenario == IServiceReverseType::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnAllBundlesFinished(errCode);
        } else if (scenario == IServiceReverseType::Scenario::RESTORE &&
                   BackupPara().GetBackupOverrideIncrementalRestore() &&
                   session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnAllBundlesFinished(errCode);
        } else if (scenario == IServiceReverseType::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnAllBundlesFinished(errCode);
        } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnAllBundlesFinished(errCode);
        }
        if (!BackupPara().GetBackupOverrideBackupSARelease()) {
            HILOGI("Will unload backup sa");
            sched_->TryUnloadServiceTimer(true);
        }
    }
    HILOGI("called end.");
}

ErrCode Service::VerifySendRateParam()
{
    ErrCode ret = VerifyCaller();
    if (ret != ERR_OK) {
        HILOGE("Update send rate fail, verify caller failed, ret:%{public}d", ret);
        return ret;
    }
    IServiceReverseType::Scenario scenario = session_ -> GetScenario();
    if (scenario != IServiceReverseType::Scenario::BACKUP) {
        HILOGE("This method is applicable to the backup scenario");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    return BError(BError::Codes::OK);
}

ErrCode Service::HandleCurBundleFileReady(const std::string &bundleName, const std::string &fileName, bool isIncBackup)
{
    auto backUpConnection = session_->GetExtConnection(bundleName);
    if (backUpConnection == nullptr) {
        HILOGE("AppFileReady error, backUpConnection is empty, bundleName:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("AppFileReady error, Extension backup Proxy is empty, bundleName:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    // 通知extension清空缓存
    proxy->HandleClear();
    // 清除Timer
    session_->StopFwkTimer(bundleName);
    session_->StopExtTimer(bundleName);
    // 通知TOOL 备份完成
    if (isIncBackup) {
        session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK),
            bundleName);
        BundleEndRadarReport(bundleName, BError(BError::Codes::OK), IServiceReverseType::Scenario::BACKUP);
    } else {
        session_->GetServiceReverseProxy()->BackupOnBundleFinished(BError(BError::Codes::OK), bundleName);
        BundleEndRadarReport(bundleName, BError(BError::Codes::OK), session_->GetScenario());
    }
    // 断开extension
    backUpConnection->DisconnectBackupExtAbility();
    ClearSessionAndSchedInfo(bundleName);
    return BError(BError::Codes::OK);
}

ErrCode Service::HandleCurAppDone(ErrCode errCode, const std::string &bundleName, bool isIncBackup)
{
    auto backUpConnection = session_->GetExtConnection(bundleName);
    if (backUpConnection == nullptr) {
        HILOGE("App finish error, backUpConnection is empty, bundleName:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("App finish error, extension backup Proxy is empty, bundleName:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    proxy->HandleClear();
    session_->StopFwkTimer(bundleName);
    session_->StopExtTimer(bundleName);
    SendEndAppGalleryNotify(bundleName);
    backUpConnection->DisconnectBackupExtAbility();
    ClearSessionAndSchedInfo(bundleName);
    if (isIncBackup) {
        HILOGI("Incremental backup or restore app done, bundleName:%{public}s, errCode:%{public}d",
            bundleName.c_str(), errCode);
        NotifyCallerCurAppIncrementDone(errCode, bundleName);
    } else {
        HILOGI("Full backup or restore app done, bundleName:%{public}s, errCode:%{public}d",
            bundleName.c_str(), errCode);
        NotifyCallerCurAppDone(errCode, bundleName);
    }
    return BError(BError::Codes::OK);
}

std::string Service::GetCallerName()
{
    std::string callerName;
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
    switch (tokenType) {
        case Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE: { /* Update Service */
            Security::AccessToken::NativeTokenInfo nativeTokenInfo;
            if (Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(tokenCaller, nativeTokenInfo) != 0) {
                HILOGE("Failed to get native token info");
                break;
            }
            callerName = nativeTokenInfo.processName;
            break;
        }
        case Security::AccessToken::ATokenTypeEnum::TOKEN_HAP: {
            Security::AccessToken::HapTokenInfo hapTokenInfo;
            if (Security::AccessToken::AccessTokenKit::GetHapTokenInfo(tokenCaller, hapTokenInfo) != 0) {
                HILOGE("Failed to get hap token info");
                break;
            }
            callerName = hapTokenInfo.bundleName;
            break;
        }
        default:
            HILOGE("Invalid token type, %{public}s", to_string(tokenType).c_str());
            break;
    }
    return callerName;
}

ErrCode Service::InitRestoreSessionWithErrMsg(const sptr<IServiceReverse> &remote,
                                              int32_t &errCodeForMsg, std::string &errMsg)
{
    errCodeForMsg = InitRestoreSession(remote, errMsg);
    HILOGI("Start InitRestoreSessionWithErrMsg, errCode:%{public}d, Msg :%{public}s",
           errCodeForMsg,
           errMsg.c_str());
    return ERR_OK;
}

ErrCode Service::InitRestoreSession(const sptr<IServiceReverse>& remote, std::string &errMsg)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    totalStatistic_ = std::make_shared<RadarTotalStatistic>(BizScene::RESTORE, GetCallerName());
    ErrCode ret = VerifyCaller();
    if (ret != ERR_OK) {
        HILOGE("Init restore session failed, verify caller failed");
        totalStatistic_->Report("InitRestoreSession", MODULE_INIT, ret);
        return ret;
    }
    ret = session_->Active({
        .clientToken = IPCSkeleton::GetCallingTokenID(),
        .scenario = IServiceReverseType::Scenario::RESTORE,
        .clientProxy = remote,
        .userId = GetUserIdDefault(),
        .callerName = GetCallerName(),
        .activeTime = TimeUtils::GetCurrentTime(),
    });
    if (ret == ERR_OK) {
        ClearFailedBundles();
        successBundlesNum_ = 0;
        ClearBundleRadarReport();
        ClearFileReadyRadarReport();
        return ret;
    }
    totalStatistic_->Report("InitRestoreSession", MODULE_INIT, ret);
    if (ret == BError(BError::Codes::SA_SESSION_CONFLICT)) {
        errMsg = BJsonUtil::BuildInitSessionErrInfo(session_->GetSessionUserId(),
                                                    session_->GetSessionCallerName(),
                                                    session_->GetSessionActiveTime());
        HILOGE("Active restore session error, Already have a session");
        return ret;
    }
    HILOGE("Active restore session error");
    StopAll(nullptr, true);
    return ret;
}

ErrCode Service::InitBackupSessionWithErrMsg(const sptr<IServiceReverse>& remote,
                                             int32_t &errCodeForMsg, std::string &errMsg)
{
    errCodeForMsg = InitBackupSession(remote, errMsg);
    HILOGI("Start InitBackupSessionWithErrMsg, errCode:%{public}d, Msg :%{public}s",
           errCodeForMsg,
           errMsg.c_str());
    return ERR_OK;
}

ErrCode Service::InitBackupSession(const sptr<IServiceReverse>& remote, std::string &errMsg)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    totalStatistic_ = std::make_shared<RadarTotalStatistic>(BizScene::BACKUP, GetCallerName());
    ErrCode ret = VerifyCaller();
    if (ret != ERR_OK) {
        HILOGE("Init full backup session fail, verify caller failed");
        totalStatistic_->Report("InitBackupSessionWithErrMsg", MODULE_INIT, ret);
        return ret;
    }
    int32_t oldSize = StorageMgrAdapter::UpdateMemPara(BConstants::BACKUP_VFS_CACHE_PRESSURE);
    HILOGI("InitBackupSession oldSize %{public}d", oldSize);
    session_->SetMemParaCurSize(oldSize);
    ret = session_->Active({
        .clientToken = IPCSkeleton::GetCallingTokenID(),
        .scenario = IServiceReverseType::Scenario::BACKUP,
        .clientProxy = remote,
        .userId = GetUserIdDefault(),
        .callerName = GetCallerName(),
        .activeTime = TimeUtils::GetCurrentTime(),
    });
    if (ret == ERR_OK) {
        ClearFailedBundles();
        successBundlesNum_ = 0;
        ClearBundleRadarReport();
        ClearFileReadyRadarReport();
        return ret;
    }
    totalStatistic_->Report("InitBackupSessionWithErrMsg", MODULE_INIT, ret);
    if (ret == BError(BError::Codes::SA_SESSION_CONFLICT)) {
        errMsg = BJsonUtil::BuildInitSessionErrInfo(session_->GetSessionUserId(),
                                                    session_->GetSessionCallerName(),
                                                    session_->GetSessionActiveTime());
        HILOGE("Active backup session error, Already have a session");
        return ret;
    }
    HILOGE("Active backup session error");
    StopAll(nullptr, true);
    return ret;
}

ErrCode Service::GetLocalCapabilitiesForBundleInfos(int &fd)
{
    UniqueFd uniqueFd(GetLocalCapabilitiesForBundleInfos());
    fd = uniqueFd.Release();
    return ErrCode(BError::Codes::OK);
}

UniqueFd Service::GetLocalCapabilitiesForBundleInfos()
{
    try {
        HILOGI("start GetLocalCapabilitiesForBundleInfos");
        if (session_ == nullptr) {
            HILOGE("GetLocalCapabilitiesForBundleInfos failed, session is nullptr");
            return UniqueFd(-EPERM);
        }
        ErrCode ret = VerifyCaller();
        if (ret != ERR_OK) {
            HILOGE("GetLocalCapabilitiesForBundleInfos failed, verify caller failed");
            return UniqueFd(-EPERM);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        string path = BConstants::GetSaBundleBackupRootDir(GetUserIdDefault());
        BExcepUltils::VerifyPath(path, false);
        CreateDirIfNotExist(path);
        UniqueFd fd(open(path.data(), O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR));
        if (fd < 0) {
            HILOGE("Failed to open config file = %{private}s, err = %{public}d", path.c_str(), errno);
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return UniqueFd(-EPERM);
        }
        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(std::move(fd));
        auto cache = cachedEntity.Structuralize();
        cache.SetSystemFullName(GetOSFullName());
        cache.SetDeviceType(GetDeviceType());
        auto bundleInfos = BundleMgrAdapter::GetBundleInfosForLocalCapabilities(GetUserIdDefault());
        if (bundleInfos.size() == 0) {
            HILOGE("getBundleInfos failed, size = 0");
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return UniqueFd(-EPERM);
        }
        cache.SetBundleInfos(bundleInfos);
        cachedEntity.Persist();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("End");
        return move(cachedEntity.GetFd());
    } catch (const BError &e) {
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGE("GetLocalCapabilitiesForBundleInfos failed, errCode = %{public}d", e.GetCode());
        return UniqueFd(-e.GetCode());
    } catch (...) {
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("Unexpected exception");
        return UniqueFd(-EPERM);
    }
}

void Service::CallOnBundleEndByScenario(const std::string &bundleName, BackupRestoreScenario scenario, ErrCode errCode)
{
    if (session_ == nullptr) {
        HILOGE("Session is empty, bundleName:%{public}s", bundleName.c_str());
        return;
    }
    HILOGI("Begin");
    try {
        if (scenario == BackupRestoreScenario::FULL_RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnBundleFinished(errCode, bundleName);
        } else if (scenario == BackupRestoreScenario::INCREMENTAL_RESTORE) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleFinished(errCode, bundleName);
        } else if (scenario == BackupRestoreScenario::FULL_BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnBundleFinished(errCode, bundleName);
        } else if (scenario == BackupRestoreScenario::INCREMENTAL_BACKUP) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(errCode, bundleName);
        }
    } catch (const BError &e) {
        HILOGE("Call onBundleFinished error, client is died");
        return;
    }
}

ErrCode Service::GetBackupDataSize(bool isPreciseScan, const std::vector<BIncrementalData>& bundleNameList)
{
    try {
        HILOGI("start GetBackupDataSize");
        if (session_ == nullptr || onScanning_.load()) {
            HILOGE("GetBackupDataSize error 1.session is nullptr 2.onScanning_ = %{public}d", onScanning_.load());
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        onScanning_.store(true);
        ErrCode ret = VerifyCaller();
        if (ret != ERR_OK) {
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return BError(BError::Codes::SA_INVAL_ARG, "verify caller failed");
        }
        BundleMgrAdapter::CreatBackupEnv(bundleNameList, GetUserIdDefault());
        CyclicSendScannedInfo(isPreciseScan, bundleNameList);
        return BError(BError::Codes::OK);
    } catch (...) {
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::SA_INVAL_ARG);
    }
}

void Service::CyclicSendScannedInfo(bool isPreciseScan, vector<BIncrementalData> bundleNameList)
{
    auto task = [isPreciseScan {isPreciseScan}, bundleNameList {move(bundleNameList)},
        obj {wptr<Service>(this)}, session {session_}]() {
        auto ptr = obj.promote();
        if (ptr == nullptr) {
            HILOGE("ptr is nullptr");
            session->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return;
        }
        size_t listSize = bundleNameList.size();
        HILOGI("need scanf num = %{public}zu", listSize);
        string scanning;
        size_t allScannedSize = 0;
        ptr->GetDataSizeStepByStep(isPreciseScan, bundleNameList, scanning);
        while (!ptr->isScannedEnd_.load()) {
            std::unique_lock<mutex> lock(ptr->getDataSizeLock_);
            ptr->getDataSizeCon_.wait_for(lock, std::chrono::seconds(WAIT_SCANNING_INFO_SEND_TIME),
                [ptr] { return ptr->isScannedEnd_.load(); });
            auto scannedSize = ptr->bundleDataSizeList_.size();
            allScannedSize += scannedSize;
            HILOGI("ScannedSize = %{public}zu, allScannedSize = %{public}zu", scannedSize, allScannedSize);
            if (!ptr->GetScanningInfo(obj, scannedSize, scanning)) {
                ptr->SendScannedInfo("", session);
                continue;
            }
            ptr->DeleteFromList(scannedSize);
            ptr->SendScannedInfo(ptr->scannedInfo_, session);
        }
        ptr->isScannedEnd_.store(false);
        session->DecreaseSessionCnt(__PRETTY_FUNCTION__);
    };

    callbackScannedInfoThreadPool_.AddTask([task, session {session_}]() {
        try {
            task();
        } catch (...) {
            session->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            HILOGE("Failed to add task to thread pool");
        }
    });
}

void Service::GetDataSizeStepByStep(bool isPreciseScan, vector<BIncrementalData> bundleNameList, string &scanning)
{
    auto task = [isPreciseScan {isPreciseScan}, bundleNameList {move(bundleNameList)},
        &scanning, obj {wptr<Service>(this)}]() {
        auto ptr = obj.promote();
        if (ptr == nullptr) {
            HILOGE("ptr is nullptr");
            return;
        }
        if (!isPreciseScan) {
            HILOGI("start GetPresumablySize");
            ptr->GetPresumablySize(bundleNameList, scanning);
        } else {
            HILOGI("start GetPrecisesSize");
            ptr->GetPrecisesSize(bundleNameList, scanning);
        }
        std::lock_guard lock(ptr->getDataSizeLock_);
        ptr->isScannedEnd_.store(true);
        ptr->getDataSizeCon_.notify_all();
        ptr->onScanning_.store(false);
    };

    getDataSizeThreadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

void Service::GetPresumablySize(vector<BIncrementalData> bundleNameList, string &scanning)
{
    int32_t userId = GetUserIdDefault();
    for (const auto &bundleData : bundleNameList) {
        string name = bundleData.bundleName;
        SetScanningInfo(scanning, name);
        if (SAUtils::IsSABundleName(name)) {
            WriteScannedInfoToList(name, 0, ERR_SIZE);
            continue;
        }
        int64_t dataSize = BundleMgrAdapter::GetBundleDataSize(name, userId);
        if (dataSize == 0) {
            WriteScannedInfoToList(name, ERR_SIZE, ERR_SIZE);
            continue;
        }
        WriteScannedInfoToList(name, dataSize, ERR_SIZE);
    }
    SetScanningInfo(scanning, "");
    HILOGI("GetPresumablySize end");
}

void Service::GetPrecisesSize(vector<BIncrementalData> bundleNameList, string &scanning)
{
    for (const auto &bundleData : bundleNameList) {
        vector<string> bundleNames;
        vector<int64_t> lastBackTimes;
        string name = bundleData.bundleName;
        SetScanningInfo(scanning, name);
        BJsonUtil::BundleDetailInfo bundleDetail = BJsonUtil::ParseBundleNameIndexStr(name);
        if (bundleDetail.bundleIndex > 0) {
            std::string bundleNameIndex  = "+clone-" + std::to_string(bundleDetail.bundleIndex) + "+" +
            bundleDetail.bundleName;
            bundleNames.push_back(bundleNameIndex);
        } else {
            bundleNames.push_back(name);
        }
        int64_t lastTime = bundleData.lastIncrementalTime;
        lastBackTimes.push_back(lastTime);
        vector<int64_t> pkgFileSizes {};
        vector<int64_t> incPkgFileSizes {};
        int32_t err = StorageMgrAdapter::GetBundleStatsForIncrease(GetUserIdDefault(), bundleNames, lastBackTimes,
            pkgFileSizes, incPkgFileSizes);
        if (err != 0) {
            HILOGE("filed to get datasize from storage, err =%{public}d, bundlename = %{public}s, index = %{public}d",
                err, name.c_str(), bundleDetail.bundleIndex);
            WriteScannedInfoToList(name, ERR_SIZE, ERR_SIZE);
            continue;
        }
        if (lastTime == 0 && pkgFileSizes.size() > 0) {
            WriteScannedInfoToList(name, pkgFileSizes[0], ERR_SIZE);
        } else if (pkgFileSizes.size() > 0 && incPkgFileSizes.size() > 0) {
            WriteScannedInfoToList(name, pkgFileSizes[0], incPkgFileSizes[0]);
        } else {
            HILOGE ("pkgFileSizes or incPkgFileSizes error, %{public}zu, %{public}zu",
                pkgFileSizes.size(), incPkgFileSizes.size());
        }
    }
    SetScanningInfo(scanning, "");
    HILOGI("GetPrecisesSize end");
}

void Service::WriteToList(BJsonUtil::BundleDataSize bundleDataSize)
{
    std::lock_guard<std::mutex> lock(scannedListLock_);
    bundleDataSizeList_.push_back(bundleDataSize);
}

void Service::DeleteFromList(size_t scannedSize)
{
    std::lock_guard<std::mutex> lock(scannedListLock_);
    bundleDataSizeList_.erase(bundleDataSizeList_.begin(), bundleDataSizeList_.begin() + scannedSize);
}

bool Service::GetScanningInfo(wptr<Service> obj, size_t scannedSize, string &scanning)
{
    auto ptr = obj.promote();
    if (ptr == nullptr) {
        HILOGE("ptr is nullptr");
        return false;
    }
    std::lock_guard<std::mutex> lock(scannedListLock_);
    if (!BJsonUtil::WriteToStr(ptr->bundleDataSizeList_, scannedSize, scanning, ptr->scannedInfo_)) {
        return false;
    }
    return true;
}

void Service::SetScanningInfo(string &scanning, string name)
{
    std::lock_guard<std::mutex> lock(scannedListLock_);
    scanning = name;
}

void Service::WriteScannedInfoToList(const string &bundleName, int64_t dataSize, int64_t incDataSize)
{
    BJsonUtil::BundleDataSize bundleDataSize;
    bundleDataSize.bundleName = bundleName;
    bundleDataSize.dataSize = dataSize;
    bundleDataSize.incDataSize = incDataSize;
    HILOGI("name = %{public}s, size = %{public}" PRId64 ", incSize = %{public}" PRId64 "",
        bundleName.c_str(), dataSize, incDataSize);
    WriteToList(bundleDataSize);
}

void Service::SendScannedInfo(const string&scannendInfos, sptr<SvcSessionManager> session)
{
    if (scannendInfos.empty()) {
        HILOGE("write json failed , info is null");
    }
    if (session == nullptr) {
        HILOGE("session is nullptr");
        return;
    }
    HILOGI("start send scanned info");
    auto task = [session {session}, scannendInfos {scannendInfos}]() {
        if (session->GetScenario() == IServiceReverseType::Scenario::BACKUP && session->GetIsIncrementalBackup()) {
            HILOGI("this is incremental backup sending info");
            session->GetServiceReverseProxy()->IncrementalBackupOnScanningInfo(scannendInfos);
            return;
        }
        HILOGI("this is full backup sending info");
        session->GetServiceReverseProxy()->BackupOnScanningInfo(scannendInfos);
    };

    sendScannendResultThreadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

ErrCode Service::StartExtTimer(bool &isExtStart)
{
    try {
        HILOGI("Service::StartExtTimer begin.");
        if (session_ == nullptr) {
            HILOGE("StartExtTimer error, session_ is nullptr.");
            isExtStart = false;
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        string bundleName;
        ErrCode ret = VerifyCallerAndGetCallerName(bundleName);
        if (ret != ERR_OK) {
            HILOGE("Start extension timer fail, get bundleName failed, ret:%{public}d", ret);
            isExtStart = false;
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return ret;
        }
        auto timeoutCallback = TimeOutCallback(wptr<Service>(this), bundleName);
        session_->StopFwkTimer(bundleName);
        isExtStart = session_->StartExtTimer(bundleName, timeoutCallback);
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (...) {
        HILOGE("Unexpected exception");
        isExtStart = false;
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

ErrCode Service::StartFwkTimer(bool &isFwkStart)
{
    try {
        HILOGI("Service::StartFwkTimer begin.");
        if (session_ == nullptr) {
            HILOGE("StartFwkTimer error, session_ is nullptr.");
            isFwkStart = false;
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        std::string bundleName;
        ErrCode ret = VerifyCallerAndGetCallerName(bundleName);
        if (ret != ERR_OK) {
            HILOGE("Start fwk timer fail, get bundleName failed, ret:%{public}d", ret);
            isFwkStart = false;
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return ret;
        }
        auto timeoutCallback = TimeOutCallback(wptr<Service>(this), bundleName);
        session_->StopExtTimer(bundleName);
        isFwkStart = session_->StartFwkTimer(bundleName, timeoutCallback);
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (...) {
        HILOGE("Unexpected exception");
        isFwkStart = false;
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

ErrCode Service::TryToConnectExt(const std::string& bundleName, sptr<SvcBackupConnection>& extConnection)
{
    extConnection = session_->GetExtConnection(bundleName);
    if (extConnection != nullptr && extConnection->IsExtAbilityConnected()) {
        return BError(BError::Codes::OK);
    }
    if (extConnection == nullptr) {
        extConnection = session_->CreateBackupConnection(bundleName);
        if (extConnection == nullptr) {
            HILOGE("backupConnection is null, bundleName: %{public}s", bundleName.c_str());
            return BError(BError::Codes::SA_INVAL_ARG);
        }
    }
    auto callConnected = GetBackupInfoConnectDone(wptr(this), bundleName);
    auto callDied = GetBackupInfoConnectDied(wptr(this), bundleName);
    extConnection->SetCallback(callConnected);
    extConnection->SetCallDied(callDied);
    AAFwk::Want want = CreateConnectWant(bundleName);
    ErrCode err = extConnection->ConnectBackupExtAbility(want, GetUserIdDefault(), false);
    if (err != BError(BError::Codes::OK)) {
        HILOGE("ConnectBackupExtAbility failed, bundleName:%{public}s, ret:%{public}d", bundleName.c_str(), err);
        return BError(BError::Codes::SA_BOOT_EXT_FAIL);
    }
    return BError(BError::Codes::OK);
}

ErrCode Service::CleanBundleTempDir(const string &bundleName)
{
    HILOGI("Service::CleanBundleTempDir");
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    ErrCode err = VerifyCaller();
    if (err != ERR_OK) {
        HILOGE("VerifyCaller failed");
        return err;
    }
    if (session_ == nullptr) {
        HILOGE("session is empty.");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    sptr<SvcBackupConnection> backupConnection;
    err = TryToConnectExt(bundleName, backupConnection);
    if (err != BError(BError::Codes::OK)) {return err;}

    std::unique_lock<std::mutex> lock(getBackupInfoSyncLock_);
    getBackupInfoCondition_.wait_for(lock, std::chrono::seconds(CONNECT_WAIT_TIME_S));
    if (isConnectDied_.load()) {
        HILOGE("GetBackupInfoConnectDied, please check bundleName: %{public}s", bundleName.c_str());
        isConnectDied_.store(false);
        return BError(BError::Codes::EXT_ABILITY_DIED);
    }

    session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
    if (backupConnection == nullptr) {
        HILOGE("backupConnection is empty.");
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto proxy = backupConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("Extension backup Proxy is empty.");
        backupConnection->DisconnectBackupExtAbility();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    proxy->CleanBundleTempDir();
    backupConnection->DisconnectBackupExtAbility();
    session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
    return BError(BError::Codes::OK);
}

ErrCode Service::HandleExtDisconnect(bool isIncBackup)
{
    HILOGI("begin, isIncBackup: %{public}d", isIncBackup);
    std::string callerName;
    auto ret = VerifyCallerAndGetCallerName(callerName);
    if (ret != ERR_OK) {
        HILOGE("HandleExtDisconnect VerifyCaller failed, get bundle failed, ret:%{public}d", ret);
        return ret;
    }
    std::shared_ptr<ExtensionMutexInfo> mutexPtr = GetExtensionMutex(callerName);
    if (mutexPtr == nullptr) {
        HILOGE("extension mutex ptr is nullptr, bundleName:%{public}s", callerName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    std::lock_guard<std::mutex> lock(mutexPtr->callbackMutex);
    ret = HandleCurAppDone(ret, callerName, isIncBackup);
    if (ret != ERR_OK) {
        HILOGE("Handle current app done error, bundleName:%{public}s", callerName.c_str());
        return ret;
    }
    RemoveExtensionMutex(callerName);
    RemoveExtOnRelease(callerName);
    OnAllBundlesFinished(BError(BError::Codes::OK));
    return ret;
}

ErrCode Service::GetExtOnRelease(bool &isExtOnRelease)
{
    std::string bundleName;
    auto ret = VerifyCallerAndGetCallerName(bundleName);
    if (ret != ERR_OK) {
        HILOGE("GetExtOnRelease VerifyCaller failed, get bundle failed, ret:%{public}d", ret);
        return ret;
    }
    auto it = backupExtOnReleaseMap_.find(bundleName);
    if (it == backupExtOnReleaseMap_.end()) {
        HILOGI("BackupExtOnReleaseMap not contain %{public}s", bundleName.c_str());
        backupExtOnReleaseMap_[bundleName] = false;
        isExtOnRelease = backupExtOnReleaseMap_[bundleName].load();
        return ret;
    }
    HILOGI("BackupExtOnReleaseMap contain %{public}s", bundleName.c_str());
    isExtOnRelease = backupExtOnReleaseMap_[bundleName].load();
    return ret;
}

void Service::SetExtOnRelease(const BundleName &bundleName, bool isOnRelease)
{
    HILOGI("Set bundleName:%{public}s isOnRelease:%{public}d", bundleName.c_str(), isOnRelease);
    auto it = backupExtOnReleaseMap_.find(bundleName);
    if (it == backupExtOnReleaseMap_.end()) {
        backupExtOnReleaseMap_[bundleName] = isOnRelease;
        return;
    }
    it->second.store(isOnRelease);
}

void Service::RemoveExtOnRelease(const BundleName &bundleName)
{
    auto it = backupExtOnReleaseMap_.find(bundleName);
    if (it == backupExtOnReleaseMap_.end()) {
        HILOGI("BackupExtOnReleaseMap not contain %{public}s", bundleName.c_str());
        return;
    }
    backupExtOnReleaseMap_.erase(it);
}

void Service::HandleOnReleaseAndDisconnect(sptr<SvcSessionManager> sessionPtr, const std::string &bundleName)
{
    if (sessionPtr == nullptr) {
        HILOGE("SessionPtr is nullptr.");
        return;
    }
    auto sessionConnection = sessionPtr->GetExtConnection(bundleName);
    if (sessionConnection == nullptr) {
        HILOGE("Error, sessionConnection is empty, bundleName:%{public}s", bundleName.c_str());
        return;
    }
    auto proxy = sessionConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("Extension backup Proxy is empty");
        return;
    }
    proxy->HandleOnRelease(static_cast<int32_t>(sessionPtr->GetScenario()));
    sessionConnection->DisconnectBackupExtAbility();
}
}