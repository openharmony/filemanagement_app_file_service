/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include <chrono>
#include <cerrno>
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
#include "b_file_info.h"
#include "b_hiaudit/hi_audit.h"
#include "b_json/b_json_cached_entity.h"
#include "b_jsonutil/b_jsonutil.h"
#include "b_ohos/startup/backup_para.h"
#include "b_process/b_multiuser.h"
#include "b_radar/b_radar.h"
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

vector<BIncrementalData> Service::MakeDetailList(const vector<BundleName> &bundleNames)
{
    vector<BIncrementalData> bundleDetails {};
    for (const auto &bundleName : bundleNames) {
        bundleDetails.emplace_back(BIncrementalData {bundleName, 0});
    }
    return bundleDetails;
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

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (session_ == nullptr) {
        HILOGE("PublishFile error, session is empty");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    ErrCode ret = VerifyCaller(IServiceReverse::Scenario::RESTORE);
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
            ret = HandleCurAppDone(errCode, callerName, false);
            if (ret != ERR_OK) {
                HILOGE("Handle current app done error, bundleName:%{public}s", callerName.c_str());
                return ret;
            }
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
    IServiceReverse::Scenario scenario = session_->GetScenario();
    BConstants::ExtensionAction action;
    if (scenario == IServiceReverse::Scenario::BACKUP || scenario == IServiceReverse::Scenario::CLEAN) {
        action = BConstants::ExtensionAction::BACKUP;
    } else if (scenario == IServiceReverse::Scenario::RESTORE) {
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
                IServiceReverse::Scenario::BACKUP);
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
        }
        bool needCleanData = session_->GetClearDataFlag(callName);
        if (!needCleanData) {
            HILOGE("Current extension is died, but not need clean data, bundleName:%{public}s", callName.c_str());
            ClearSessionAndSchedInfo(callName);
            NoticeClientFinish(callName, BError(BError::Codes::EXT_ABILITY_DIED));
            return;
        }
        session_->SetServiceSchedAction(callName, BConstants::ServiceSchedAction::CLEAN);
        auto ret = LaunchBackupExtension(callName);
        if (ret) {
            /* Clear Session before notice client finish event */
            HILOGE("Current bundle launch extension failed, bundleName:%{public}s", callName.c_str());
            ClearSessionAndSchedInfo(callName);
        }
        /* Notice Client Ext Ability Process Died */
        NoticeClientFinish(callName, BError(BError::Codes::EXT_ABILITY_DIED));
    } catch (...) {
        HILOGE("Unexpected exception, bundleName: %{public}s", callName.c_str());
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
        SendEndAppGalleryNotify(bundleName);
        auto scenario = session_->GetScenario();
        if (scenario == IServiceReverse::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(errCode, bundleName);
        } else if (scenario == IServiceReverse::Scenario::RESTORE &&
                BackupPara().GetBackupOverrideIncrementalRestore() &&
                session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleFinished(errCode, bundleName);
        } else if (scenario == IServiceReverse::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnBundleFinished(errCode, bundleName);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
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

void Service::OnAllBundlesFinished(ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("called begin.");
    if (session_->IsOnAllBundlesFinished()) {
        IServiceReverse::Scenario scenario = session_->GetScenario();
        if (isInRelease_.load() && (scenario == IServiceReverse::Scenario::RESTORE)) {
            HILOGI("Will destory session info");
            SessionDeactive();
        }
        if (scenario == IServiceReverse::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnAllBundlesFinished(errCode);
        } else if (scenario == IServiceReverse::Scenario::RESTORE &&
                   BackupPara().GetBackupOverrideIncrementalRestore() &&
                   session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnAllBundlesFinished(errCode);
        } else if (scenario == IServiceReverse::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnAllBundlesFinished(errCode);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
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
    IServiceReverse::Scenario scenario = session_ -> GetScenario();
    if (scenario != IServiceReverse::Scenario::BACKUP) {
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
        BundleEndRadarReport(bundleName, BError(BError::Codes::OK), IServiceReverse::Scenario::BACKUP);
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
    SendEndAppGalleryNotify(callerName);
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

ErrCode Service::InitRestoreSession(sptr<IServiceReverse> remote, std::string &errMsg)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    ErrCode ret = VerifyCaller();
    if (ret != ERR_OK) {
        HILOGE("Init restore session failed, verify caller failed");
        return ret;
    }
    ret = session_->Active({
        .clientToken = IPCSkeleton::GetCallingTokenID(),
        .scenario = IServiceReverse::Scenario::RESTORE,
        .clientProxy = remote,
        .userId = GetUserIdDefault(),
        .callerName = GetCallerName(),
        .activeTime = TimeUtils::GetCurrentTime(),
    });
    if (ret == ERR_OK) {
        ClearFailedBundles();
        successBundlesNum_ = 0;
        return ret;
    }
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

ErrCode Service::InitBackupSession(sptr<IServiceReverse> remote, std::string &errMsg)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    ErrCode ret = VerifyCaller();
    if (ret != ERR_OK) {
        HILOGE("Init full backup session fail, verify caller failed");
        return ret;
    }
    int32_t oldSize = StorageMgrAdapter::UpdateMemPara(BConstants::BACKUP_VFS_CACHE_PRESSURE);
    HILOGI("InitBackupSession oldSize %{public}d", oldSize);
    session_->SetMemParaCurSize(oldSize);
    ret = session_->Active({
        .clientToken = IPCSkeleton::GetCallingTokenID(),
        .scenario = IServiceReverse::Scenario::BACKUP,
        .clientProxy = remote,
        .userId = GetUserIdDefault(),
        .callerName = GetCallerName(),
        .activeTime = TimeUtils::GetCurrentTime(),
    });
    if (ret == ERR_OK) {
        ClearFailedBundles();
        successBundlesNum_ = 0;
        return ret;
    }
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
    if (scenario == BackupRestoreScenario::FULL_RESTORE) {
        session_->GetServiceReverseProxy()->RestoreOnBundleFinished(errCode, bundleName);
    } else if (scenario == BackupRestoreScenario::INCREMENTAL_RESTORE) {
        session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleFinished(errCode, bundleName);
    } else if (scenario == BackupRestoreScenario::FULL_BACKUP) {
        session_->GetServiceReverseProxy()->BackupOnBundleFinished(errCode, bundleName);
    } else if (scenario == BackupRestoreScenario::INCREMENTAL_BACKUP) {
        session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(errCode, bundleName);
    }
}
}