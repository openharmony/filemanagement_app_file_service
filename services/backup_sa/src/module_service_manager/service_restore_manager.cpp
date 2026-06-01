/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "module_external/bms_adapter.h"
#include "module_notify/notify_work_service.h"
#include "module_strategy/service_strategy.h"
#include "hitrace_meter.h"
#include "module_ipc/service.h"
#include "module_ipc/svc_backup_connection.h"
#include "module_ipc/svc_restore_deps_manager.h"

#include "b_sa/b_sa_utils.h"
#include "b_filesystem/b_dir.h"
#include "b_ohos/startup/backup_para.h"
#include "b_error/b_excep_utils.h"

namespace OHOS::FileManagement::Backup {

static bool SpecialDefaultVersion(const string &versionName)
{
    string versionNameFlag = versionName.substr(0, versionName.find_first_of(BConstants::VERSION_NAME_SEPARATOR_CHAR));
    auto iter = find_if(BConstants::DEFAULT_VERSION_NAMES_VEC.begin(), BConstants::DEFAULT_VERSION_NAMES_VEC.end(),
                        [&versionNameFlag](const auto &version) { return version == versionNameFlag; });
    if (iter != BConstants::DEFAULT_VERSION_NAMES_VEC.end()) {
        return true;
    }
    return false;
}

ErrCode Service::SetSessPropertiesRestore(const std::vector<std::string> &restoreBundleNames,
    vector<BJsonEntityCaps::BundleInfo> &restoreBundleInfos)
{
    session_->SetOldBackupVersion(oldBackupVersion_);
    std::vector<std::string> strategies = {
        "RestoreBasePropertyStrategy",
        "DefaultPropertyStrategy",
        "DataSizePropertyStrategy"
    };
    for (const auto &restoreInfo : restoreBundleInfos) {
        auto it = find_if(restoreBundleNames.begin(), restoreBundleNames.end(), [&restoreInfo](const auto &bundleName) {
            std::string bundleNameIndex = BJsonUtil::BuildBundleNameIndexInfo(restoreInfo.name, restoreInfo.appIndex);
            return bundleName == bundleNameIndex;
        });
        if (it == restoreBundleNames.end()) {
            HILOGE("Can not find current bundle, bundleName:%{public}s, appIndex:%{public}d", restoreInfo.name.c_str(),
                restoreInfo.appIndex);
            continue;
        }
        HILOGI("bundleName: %{public}s, extensionName: %{public}s", restoreInfo.name.c_str(),
            restoreInfo.extensionName.c_str());
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(restoreInfo.name, restoreInfo.appIndex);
        if ((!restoreInfo.allToBackup && !SpecialDefaultVersion(restoreInfo.versionName)) ||
            (restoreInfo.extensionName.empty() && !SAUtils::IsSABundleName(restoreInfo.name))) {
            AppStatReportErr(restoreInfo.name, "SetCurrentSessProperties",
                RadarError(MODULE_BMS, BError(BError::Codes::SA_FORBID_BACKUP_RESTORE)));
            OnBundleStarted(BError(BError::Codes::SA_FORBID_BACKUP_RESTORE), session_, bundleNameIndexInfo);
            session_->RemoveExtInfo(bundleNameIndexInfo);
            continue;
        }
        if (BundleMgrAdapter::IsUser0BundleName(bundleNameIndexInfo, session_->GetSessionUserId())) {
            SendUserIdToApp(bundleNameIndexInfo, session_->GetSessionUserId());
        }
        StrategyContext context;
        context.bundleName = bundleNameIndexInfo;
        context.bundleNameIndexInfo = bundleNameIndexInfo;
        context.bundleInfo = restoreInfo;
        context.userId = session_->GetSessionUserId();
        context.restoreType = restoreType_;
        context.session = wptr<SvcSessionManager>(session_);
        context.service = wptr<Service>(this);
        PropertyStrategyExecutor::GetInstance().ExecuteStrategies(context, strategies);
    }
    return BError(BError::Codes::OK);
}

ErrCode Service::SetSessPropertiesWithDetailRestore(const std::vector<std::string> &restoreBundleNames,
    vector<BJsonEntityCaps::BundleInfo> &restoreBundleInfos,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> &bundleNameDetailMap,
    std::map<std::string, bool> &isClearDataFlags)
{
    session_->SetOldBackupVersion(oldBackupVersion_);
    std::vector<std::string> strategies = {
        "RestoreBasePropertyStrategy", "DefaultPropertyStrategy", "DataSizePropertyStrategy",
        "ClearDataFlagPropertyStrategy", "RestoreExtraPropertyStrategy"
    };
    for (const auto &restoreInfo : restoreBundleInfos) {
        auto it = find_if(restoreBundleNames.begin(), restoreBundleNames.end(), [&restoreInfo](const auto &bundleName) {
            std::string bundleNameIndex = BJsonUtil::BuildBundleNameIndexInfo(restoreInfo.name, restoreInfo.appIndex);
            return bundleName == bundleNameIndex;
        });
        if (it == restoreBundleNames.end()) {
            HILOGE("Can not find current bundle, bundleName:%{public}s, appIndex:%{public}d", restoreInfo.name.c_str(),
                restoreInfo.appIndex);
            continue;
        }
        HILOGI("bundleName: %{public}s, extensionName: %{public}s", restoreInfo.name.c_str(),
            restoreInfo.extensionName.c_str());
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(restoreInfo.name, restoreInfo.appIndex);
        if (((!restoreInfo.allToBackup && !SpecialDefaultVersion(restoreInfo.versionName)) ||
            (restoreInfo.extensionName.empty() && !SAUtils::IsSABundleName(restoreInfo.name))) &&
            !GetDefaultBundleResult(restoreInfo.name)) {
            AppStatReportErr(restoreInfo.name, "SetCurrentSessProperties",
                RadarError(MODULE_BMS, BError(BError::Codes::SA_FORBID_BACKUP_RESTORE)));
            OnBundleStarted(BError(BError::Codes::SA_FORBID_BACKUP_RESTORE), session_, bundleNameIndexInfo);
            session_->RemoveExtInfo(bundleNameIndexInfo);
            continue;
        }
        if (BundleMgrAdapter::IsUser0BundleName(bundleNameIndexInfo, session_->GetSessionUserId())) {
            SendUserIdToApp(bundleNameIndexInfo, session_->GetSessionUserId());
        }
        StrategyContext context;
        context.bundleName = bundleNameIndexInfo;
        context.bundleNameIndexInfo = bundleNameIndexInfo;
        context.bundleInfo = restoreInfo;
        context.userId = session_->GetSessionUserId();
        context.restoreType = restoreType_;
        context.session = wptr<SvcSessionManager>(session_);
        context.service = wptr<Service>(this);
        context.bundleNameDetailMap = &bundleNameDetailMap;
        context.isClearDataFlags = &isClearDataFlags;
        PropertyStrategyExecutor::GetInstance().ExecuteStrategies(context, strategies);
    }
    return BError(BError::Codes::OK);
}

std::vector<std::string> Service::GetSupportBundleNamesRestore(const vector<BundleName> &bundleNames,
    std::vector<BJsonEntityCaps::BundleInfo> &restoreInfos, UniqueFd fd)
{
    restoreInfos = GetRestoreBundleNames(move(fd), session_, bundleNames, oldBackupVersion_);
    auto supportNames = SvcRestoreDepsManager::GetInstance().GetRestoreBundleNames(restoreInfos, restoreType_);
    HandleExceptionOnAppendBundles(session_, bundleNames, supportNames);
    if (supportNames.empty()) {
        HILOGE("AppendBundlesRestoreSession failed, restoreBundleNames is empty.");
        return {};
    }
    return supportNames;
}

void Service::CallStartDefaultBundleTask(const std::string &bundleName, IServiceReverseType::Scenario &scenario)
{
    if (scenario == IServiceReverseType::Scenario::BACKUP) {
        StartBundleTaskBackup(bundleName);
    } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
        StartBundleTaskRestore(bundleName);
    }
}

void Service::StartBundleTaskRestore(const std::string &bundleName)
{
    auto scenario = IServiceReverseType::Scenario::RESTORE;
    HILOGI("Begin handle current bundle restore, bundleName:%{public}s", bundleName.c_str());
    auto ret = ERR_OK;
    if (!BackupPara().GetBackupOverrideIncrementalRestore() &&
        !session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
        return;
    }
    session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(ret, bundleName);
    GetOldDeviceBackupVersion();
    BundleBeginRadarReport(bundleName, ret, scenario);
    auto fileNameVec = session_->GetExtFileNameRequest(bundleName);
    for (const auto &fileName : fileNameVec) {
        auto err = SendIncrementalFileHandle(bundleName, fileName);
        if (err != ERR_OK) {
            HILOGE("SendIncrementalFileHandle failed code: %{public}d", err);
        }
    }
}

ErrCode Service::SendDefaultIncrementalFileHandle(const std::string &bundleName, const std::string &fileName)
{
    UniqueFd fd = UniqueFd(BConstants::INVALID_FD_NUM);
    UniqueFd reportFd = UniqueFd(BConstants::INVALID_FD_NUM);
    int errCode = BConstants::INVALID_FD_NUM;
    if (!BDir::IsFilePathValid(fileName)) {
        auto ret = AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG));
        if (ret != ERR_OK) {
            HILOGE("Failed to notify app incre done. err = %{public}d", ret);
        }
        return BError(BError::Codes::EXT_INVAL_ARG);
    }
    auto instance = GetMigrateInstance(wptr<Service>(this), bundleName, GetUserIdDefault());
    if (instance == nullptr) {
        HILOGE("Failed to GetMigrateInstance");
        return BError(BError::Codes::EXT_INVAL_ARG);
    }
    auto ret = instance->GetIncrementalFileHandle(fileName, fd, reportFd, errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed to get file handle, err = %{public}d", errCode);
        return BError(BError::Codes::EXT_INVAL_ARG);
    }
    auto err = AppIncrementalFileReady(bundleName, fileName, move(fd), move(reportFd), errCode);
    if (err != ERR_OK) {
        HILOGE("Failed to send file handle, bundleName:%{public}s, fileName:%{public}s",
            bundleName.c_str(), GetAnonyPath(fileName).c_str());
    }
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishDefaultIncrementalFile(const BFileInfo &fileInfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::RESTORE);
    if (ret != ERR_OK) {
        HILOGE("Publish incremental file failed, bundleName:%{public}s", fileInfo.owner.c_str());
        return ret;
    }
    HILOGI("Start Default publish, bundleName:%{public}s", fileInfo.owner.c_str());
    if (!fileInfo.fileName.empty()) {
        HILOGE("Forbid to use PublishIncrementalFile with fileName for App");
        return EPERM;
    }
    if (session_ == nullptr) {
        HILOGE("session is empty, bundleName:%{public}s", fileInfo.owner.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    session_->SetPublishFlag(fileInfo.owner);
    auto instance = GetMigrateInstance(wptr<Service>(this), fileInfo.owner, GetUserIdDefault());
    if (instance == nullptr) {
        HILOGE("Failed to GetMigrateInstance");
        return BError(BError::Codes::EXT_INVAL_ARG);
    }
    ret = instance->PublishIncrementalFile(fileInfo.fileName);
    if (ret != ERR_OK) {
        HILOGE("Failed to publish file for backup extension, bundleName:%{public}s", fileInfo.owner.c_str());
        return ret;
    }
    return BError(BError::Codes::OK);
}
} // OHOS::FileManagement::Backup