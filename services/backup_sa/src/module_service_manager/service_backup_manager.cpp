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

#include "module_service_manager/service_manager.h"
#include "module_ipc/svc_backup_connection.h"
#include "module_external/bms_adapter.h"
#include "module_strategy/service_strategy.h"
#include "hitrace_meter.h"


namespace OHOS::FileManagement::Backup {

static std::string GetKeyInMap(const std::map<std::string, bool> &map, const std::string& fileName)
{
    for (const auto& pair : map) {
        if (fileName.find(pair.first) != string::npos) {
            return pair.first;
        }
    }
    return "";
}

// 初始化migrate
bool Service::MigrateModuleInit(wptr<Service> servicePtr, const std::string &bundleName, int32_t userId)
{
    if (migrate_ == nullptr) {
        migrate_ = sptr(new MigrateManager(servicePtr, bundleName, userId));
    }
    return migrate_ != nullptr;
}

ErrCode Service::SetSessPropertiesBackup(const std::vector<std::string> &bundleNames,
    vector<BJsonEntityCaps::BundleInfo> &backupBundleInfos)
{
    if (session_ == nullptr) {
        HILOGE("Set currrent session properties error, session is empty");
        return BError(BError::Codes::OK);
    }
    int32_t userId = session_->GetSessionUserId();
    std::map<std::string, BJsonEntityCaps::BundleInfo> bundleNameIndexBundleInfoMap;
    for (const auto &bundleInfo : backupBundleInfos) {
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(bundleInfo.name, bundleInfo.appIndex);
        bundleNameIndexBundleInfoMap[bundleNameIndexInfo] = bundleInfo;
    }

    std::vector<std::string> strategies = {"DefaultPropertyStrategy", "DataSizePropertyStrategy"};

    for (const auto &bundleName : bundleNames) {
        auto it = bundleNameIndexBundleInfoMap.find(bundleName);
        if (it == bundleNameIndexBundleInfoMap.end()) {
            HILOGE("Current bundleName can not find bundleInfo, bundleName:%{public}s", bundleName.c_str());
            session_->RemoveExtInfo(bundleName);
            continue;
        }

        StrategyContext context;
        context.bundleName = bundleName;
        context.bundleNameIndexInfo = bundleName;
        context.bundleInfo = it->second;
        context.userId = userId;
        context.isIncBackup = isIncBackup_;
        context.session = session_;
        context.service = this;

        PropertyStrategyExecutor::GetInstance().ExecuteStrategies(context, strategies);
    }
    return BError(BError::Codes::OK);
}

ErrCode Service::SetSessPropertiesWithDetailBackup(const std::vector<std::string> &bundleNames,
    vector<BJsonEntityCaps::BundleInfo> &backupBundleInfos,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> &bundleNameDetailMap,
    std::map<std::string, bool> &isClearDataFlags)
{
    if (session_ == nullptr) {
        HILOGE("Set currrent session properties error, session is empty");
        return BError(BError::Codes::OK);
    }
    int32_t userId = session_->GetSessionUserId();
    std::map<std::string, BJsonEntityCaps::BundleInfo> bundleNameIndexBundleInfoMap;
    for (const auto &bundleInfo : backupBundleInfos) {
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(bundleInfo.name, bundleInfo.appIndex);
        bundleNameIndexBundleInfoMap[bundleNameIndexInfo] = bundleInfo;
    }

    std::vector<std::string> strategies = {
        "DefaultPropertyStrategy",
        "DataSizePropertyStrategy",
        "ClearDataFlagPropertyStrategy",
        "BackupExtraPropertyStrategy"
    };

    for (const auto &bundleName : bundleNames) {
        if (BundleMgrAdapter::IsUser0BundleName(bundleName, userId)) {
            HILOGE("bundleName:%{public}s is zero user bundle", bundleName.c_str());
            SendUserIdToApp(bundleName, userId);
        }

        auto it = bundleNameIndexBundleInfoMap.find(bundleName);
        if (it == bundleNameIndexBundleInfoMap.end()) {
            HILOGE("Current bundleName can not find bundleInfo, bundleName:%{public}s", bundleName.c_str());
            session_->RemoveExtInfo(bundleName);
            continue;
        }

        StrategyContext context;
        context.bundleName = bundleName;
        context.bundleNameIndexInfo = bundleName;
        context.bundleInfo = it->second;
        context.userId = userId;
        context.isIncBackup = isIncBackup_;
        context.session = session_;
        context.service = this;
        context.bundleNameDetailMap = &bundleNameDetailMap;
        context.isClearDataFlags = &isClearDataFlags;

        PropertyStrategyExecutor::GetInstance().ExecuteStrategies(context, strategies);
    }
    return BError(BError::Codes::OK);
}

std::vector<std::string> Service::GetSupportBundleNamesBackup(const vector<BundleName> &bundleNames,
    std::vector<BJsonEntityCaps::BundleInfo> &backupInfos)
{
    auto bundleDetails = MakeDetailList(bundleNames);
    backupInfos = BundleMgrAdapter::GetBundleInfosForAppendBundles(bundleDetails,
        session_->GetSessionUserId());
    // 这里要走service里面的原逻辑，后面细化的时候再处理
    std::vector<std::string> supportBundleNames = GetSupportBackupBundleNames(backupInfos, false, bundleNames);
    if (supportBundleNames.empty()) {
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGW("supportBundleNames is empty.");
    }
    return supportBundleNames;
}

void Service::StartBundleTaskBackup(const std::string &bundleName)
{
    HILOGI("Begin handle current bundle full backup or full restore, bundleName:%{public}s", bundleName.c_str());
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    auto ret = ERR_OK;
    if (MigrateModuleInit(wptr<Service>(this), bundleName, GetUserIdDefault())) {
        ret = migrate_->HandleBackup(session_->GetClearDataFlag(bundleName), bundleName);
    }

    session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
    BundleBeginRadarReport(bundleName, ret, scenario);
    if (ret) {
        SendEndAppGalleryNotify(bundleName);
        ClearSessionAndSchedInfo(bundleName);
        NoticeClientFinish(bundleName, BError(BError::Codes::SA_INVAL_ARG));
    }
    HILOGI("End handle current bundle full backup or full restore, bundleName:%{public}s", bundleName.c_str());
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

ErrCode Service::AppFileReady(const std::string &fileName, const std::string &filePath,
    int fd, int32_t errCode, const set<string> &fileNames)
{
    UniqueFd fdUnique(fd);
    return AppFileReady(fileName, filePath, std::move(fdUnique), errCode, fileNames);
}

ErrCode Service::AppFileReadyWithoutFd(const std::string &fileName, const std::string &filePath,
    UniqueFd fd, int32_t errCode, const set<string> &fileNames)
{
    return AppFileReady(fileName, filePath, UniqueFd(INVALID_FD), errCode, fileNames);
}

ErrCode Service::AppFileReady(const string &fileName, const std::string &filePath,
    UniqueFd fd, int32_t errCode, const set<string> &fileNames))
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_ == nullptr) {
            HILOGE("AppFileReady error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        string callerName = GetKeyInMap(defaultBundleMap_, filePath);
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

}