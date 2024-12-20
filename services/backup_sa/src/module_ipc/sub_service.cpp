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
    for (auto bundleName : bundleNames) {
        bundleDetails.emplace_back(BIncrementalData {bundleName, 0});
    }
    return bundleDetails;
}

ErrCode Service::Finish()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        VerifyCaller(session_->GetScenario());
        session_->Finish();
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
    try {
        if (session_ == nullptr) {
            HILOGE("PublishFile error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        VerifyCaller(IServiceReverse::Scenario::RESTORE);
        if (!fileInfo.fileName.empty()) {
            HILOGE("Forbit to use publishFile with fileName for App");
            return EPERM;
        }
        auto backUpConnection = session_->GetExtConnection(fileInfo.owner);
        if (backUpConnection == nullptr) {
            HILOGE("PublishFile error, backUpConnection is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        auto proxy = backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            HILOGE("PublishFile error, Extension backup Proxy is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        ErrCode res = proxy->PublishFile(fileInfo.fileName);
        if (res) {
            HILOGE("Failed to publish file for backup extension");
        }
        return res;
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::AppFileReady(const string &fileName, UniqueFd fd, int32_t errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_ == nullptr) {
            HILOGE("AppFileReady error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        string callerName = VerifyCallerAndGetCallerName();
        if (fileName.find('/') != string::npos) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not valid");
        }
        if (fileName == BConstants::EXT_BACKUP_MANAGE) {
            fd = session_->OnBundleExtManageInfo(callerName, move(fd));
        }
        session_->GetServiceReverseProxy()->BackupOnFileReady(callerName, fileName, move(fd), errCode);
        FileReadyRadarReport(callerName, fileName, errCode, session_->GetScenario());
        AuditLog auditLog = { false, "Backup File Ready", "ADD", "", 1, "SUCCESS", "AppFileReady",
            callerName, GetAnonyPath(fileName) };
        HiAudit::GetInstance(true).Write(auditLog);
        if (session_->OnBundleFileReady(callerName, fileName)) {
            auto backUpConnection = session_->GetExtConnection(callerName);
            if (backUpConnection == nullptr) {
                HILOGE("AppFileReady error, backUpConnection is empty");
                return BError(BError::Codes::SA_INVAL_ARG);
            }
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                HILOGE("AppFileReady error, Extension backup Proxy is empty");
                return BError(BError::Codes::SA_INVAL_ARG);
            }
            // 通知extension清空缓存
            proxy->HandleClear();
            // 清除Timer
            session_->StopFwkTimer(callerName);
            session_->StopExtTimer(callerName);
            // 通知TOOL 备份完成
            session_->GetServiceReverseProxy()->BackupOnBundleFinished(BError(BError::Codes::OK), callerName);
            BundleEndRadarReport(callerName, BError(BError::Codes::OK), session_->GetScenario());
            // 断开extension
            backUpConnection->DisconnectBackupExtAbility();
            ClearSessionAndSchedInfo(callerName);
        }
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGI("Unexpected exception");
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
        string callerName = VerifyCallerAndGetCallerName();
        HILOGI("Begin, callerName is: %{public}s, errCode: %{public}d", callerName.c_str(), errCode);
        if (session_->OnBundleFileReady(callerName) || errCode != BError(BError::Codes::OK)) {
            std::shared_ptr<ExtensionMutexInfo> mutexPtr = GetExtensionMutex(callerName);
            if (mutexPtr == nullptr) {
                HILOGE("extension mutex ptr is nullptr");
                return BError(BError::Codes::SA_INVAL_ARG);
            }
            std::lock_guard<std::mutex> lock(mutexPtr->callbackMutex);
            auto backUpConnection = session_->GetExtConnection(callerName);
            if (backUpConnection == nullptr) {
                HILOGE("App finish error, backUpConnection is empty");
                return BError(BError::Codes::SA_INVAL_ARG);
            }
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            proxy->HandleClear();
            session_->StopFwkTimer(callerName);
            session_->StopExtTimer(callerName);
            backUpConnection->DisconnectBackupExtAbility();
            ClearSessionAndSchedInfo(callerName);
            NotifyCallerCurAppDone(errCode, callerName);
        }
        RemoveExtensionMutex(callerName);
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        ReleaseOnException();
        HILOGE("AppDone error, err code is: %{public}d", e.GetCode());
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (const exception &e) {
        ReleaseOnException();
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::LaunchBackupExtension(const BundleName &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("begin %{public}s", bundleName.data());
        IServiceReverse::Scenario scenario = session_->GetScenario();
        BConstants::ExtensionAction action;
        if (scenario == IServiceReverse::Scenario::BACKUP || scenario == IServiceReverse::Scenario::CLEAN) {
            action = BConstants::ExtensionAction::BACKUP;
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            action = BConstants::ExtensionAction::RESTORE;
        } else {
            throw BError(BError::Codes::SA_INVAL_ARG, "Failed to scenario");
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
        ErrCode ret = backUpConnection->ConnectBackupExtAbility(want, session_->GetSessionUserId(),
            bundleAction == BConstants::ServiceSchedAction::CLEAN);
        if (ret) {
            HILOGE("ConnectBackupExtAbility faild, bundleName:%{public}s, ret:%{public}d", bundleName.c_str(), ret);
            ExtensionConnectFailRadarReport(bundleName, ret, scenario);
            return BError(BError::Codes::SA_BOOT_EXT_FAIL);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGI("Unexpected exception");
        return EPERM;
    }
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
    want.SetElementName(bundleDetail.bundleName, backupExtName);
    want.SetParam(BConstants::EXTENSION_ACTION_PARA, static_cast<int>(action));
    want.SetParam(BConstants::EXTENSION_VERSION_CODE_PARA, static_cast<long>(versionCode));
    want.SetParam(BConstants::EXTENSION_RESTORE_TYPE_PARA, static_cast<int>(restoreType));
    want.SetParam(BConstants::EXTENSION_VERSION_NAME_PARA, versionName);
    want.SetParam(BConstants::EXTENSION_RESTORE_EXT_INFO_PARA, bundleExtInfo);
    want.SetParam(BConstants::EXTENSION_BACKUP_EXT_INFO_PARA, bundleExtInfo);
    want.SetParam(BConstants::EXTENSION_APP_CLONE_INDEX_PARA, bundleDetail.bundleIndex);
}

std::vector<std::string> Service::GetSupportBackupBundleNames(vector<BJsonEntityCaps::BundleInfo> &backupInfos,
    bool isIncBackup, const std::vector<std::string> &srcBundleNames)
{
    HILOGI("Begin");
    std::vector<std::string> supportBackupNames;
    for (auto info : backupInfos) {
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

void Service::HandleNotSupportBundleNames(const std::vector<std::string> &srcBundleNames,
    std::vector<std::string> &supportBundleNames, bool isIncBackup)
{
    for (auto bundleName : srcBundleNames) {
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
}