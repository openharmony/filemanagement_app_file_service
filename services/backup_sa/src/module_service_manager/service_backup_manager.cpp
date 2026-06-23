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

#include "module_ipc/service.h"
#include "module_ipc/svc_backup_connection.h"
#include "module_external/bms_adapter.h"
#include "module_external/sms_adapter.h"
#include "module_strategy/service_strategy.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {

static const std::vector<std::string> STRATEGY_LIST = {
"DefaultPropertyStrategy",
"DataSizePropertyStrategy",
"ClearDataFlagPropertyStrategy",
"BackupExtraPropertyStrategy"
};

static std::string GetKeyInMap(const std::map<std::string, bool> &map, const std::string& fileName)
{
    for (const auto& pair : map) {
        if (pair.second == true && fileName.find(pair.first) != string::npos) {
            return pair.first;
        }
    }
    return "";
}

// 初始化migrate
sptr<MigrateManager> Service::GetMigrateInstance(wptr<Service> servicePtr,
    const std::string &bundleName, int32_t userId)
{
    std::lock_guard<std::mutex> lock(migrateInstanceLock_);
    auto it = migrateMap_.find(bundleName);
    if (it == migrateMap_.end()) {
        auto instance = sptr(new MigrateManager(servicePtr, bundleName, userId));
        migrateMap_[bundleName] = instance;
        return instance;
    }
    return it->second;
}

std::vector<std::string> Service::CallGetSupportBundleNames(const vector<BundleName> &bundleNames,
    vector<BJsonEntityCaps::BundleInfo> &bundleInfos, BizScene &scene, UniqueFd fd)
{
    std::vector<std::string> result = {};
    if (scene == BizScene::BACKUP) {
        result = GetSupportBundleNamesBackup(bundleNames, bundleInfos);
    } else if (scene == BizScene::RESTORE) {
        result = GetSupportBundleNamesRestore(bundleNames, bundleInfos, move(fd));
    }
    return result;
}

ErrCode Service::CallSetSessProperties(const vector<BundleName> &bundleNames,
    vector<BJsonEntityCaps::BundleInfo> &bundleInfos, BizScene &scene)
{
    ErrCode result = ERR_OK;
    if (scene == BizScene::BACKUP) {
        result = SetSessPropertiesBackup(bundleNames, bundleInfos);
    } else if (scene == BizScene::RESTORE) {
        result = SetSessPropertiesRestore(bundleNames, bundleInfos);
    }
    return result;
}

ErrCode Service::CallSetSessPropertiesWithDetail(
    const vector<BundleName> &bundleNames,
    vector<BJsonEntityCaps::BundleInfo> &bundleInfos,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> &bundleNameDetailMap,
    std::map<std::string, BJsonUtil::BundleSettingInfo> bundleSettingInfos,
    BizScene &scene)
{
    ErrCode result = ERR_OK;
    if (scene == BizScene::BACKUP) {
        result = SetSessPropertiesWithDetailBackup(bundleNames, bundleInfos, bundleNameDetailMap, bundleSettingInfos);
    } else if (scene == BizScene::RESTORE) {
        result = SetSessPropertiesWithDetailRestore(bundleNames, bundleInfos, bundleNameDetailMap, bundleSettingInfos);
    }
    return result;
}

ErrCode Service::AppendBundlesBackupSession(const vector<BundleName> &bundleNames)
{
    if (session_ == nullptr || isOccupyingSession_.load()) {
        HILOGE("AppendBundles restore session with infos error, session is empty");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto bizScene = BizScene::BACKUP;
    ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::BACKUP);
    if (ret != ERR_OK) {
        HILOGE("AppendBundles BACKUP session with infos error, verify caller failed, ret:%{public}d", ret);
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        return ret;
    }
    return AppendBundlesSession(bundleNames, bizScene);
}

ErrCode Service::AppendBundlesDetailsBackupSession(const std::vector<BundleName> &bundleNames,
    const std::vector<std::string> &bundleInfos)
{
    if (session_ == nullptr || isOccupyingSession_.load()) {
        HILOGE("AppendBundles restore session with infos error, session is empty");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto bizScene = BizScene::BACKUP;
    ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::BACKUP);
    if (ret != ERR_OK) {
        HILOGE("AppendBundles BACKUP session with infos error, verify caller failed, ret:%{public}d", ret);
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        return ret;
    }
    return AppendBundlesSessionWithDetail(bundleNames, bundleInfos, bizScene);
}

ErrCode Service::SetSessPropertiesBackup(const std::vector<std::string> &bundleNames,
    vector<BJsonEntityCaps::BundleInfo> &backupBundleInfos)
{
    if (session_ == nullptr) {
        HILOGE("Set current session properties error, session is empty");
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
        std::string userBundleName = bundleName;
        if (BundleMgrAdapter::IsUser0BundleName(userBundleName, userId)) {
            HILOGE("bundleName:%{public}s is zero user bundle", userBundleName.c_str());
            SendUserIdToApp(userBundleName, userId);
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
        context.session = wptr<SvcSessionManager>(session_);
        context.service = wptr<Service>(this);

        PropertyStrategyExecutor::GetInstance().ExecuteStrategies(context, strategies);
    }
    return BError(BError::Codes::OK);
}

ErrCode Service::SetSessPropertiesWithDetailBackup(const std::vector<std::string> &bundleNames,
    vector<BJsonEntityCaps::BundleInfo> &backupBundleInfos,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> &bundleNameDetailMap,
    std::map<std::string, BJsonUtil::BundleSettingInfo>& bundleSettingInfos)
{
    if (session_ == nullptr) {
        HILOGE("Set current session properties error, session is empty");
        return BError(BError::Codes::OK);
    }
    int32_t userId = session_->GetSessionUserId();
    std::map<std::string, BJsonEntityCaps::BundleInfo> bundleNameIndexBundleInfoMap;
    for (const auto &bundleInfo : backupBundleInfos) {
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(bundleInfo.name, bundleInfo.appIndex);
        bundleNameIndexBundleInfoMap[bundleNameIndexInfo] = bundleInfo;
    }

    std::map<std::string, bool> isClearDataFlags = {};
    for (const auto &[bundle, info] : bundleSettingInfos) {
        isClearDataFlags[bundle] = info.isClearData;
    }
    for (const auto &item : bundleNames) {
        string bundleName = item;
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
        auto iterSet = bundleSettingInfos.find(bundleName);
        if (iterSet != bundleSettingInfos.end()) {
            session_->SetClearDataFlag(bundleName, iterSet->second.isClearData);
            session_->SetSupportWithoutTar(bundleName, iterSet->second.isSupportWithoutTar);
            session_->SetBatchSize(bundleName, iterSet->second.batchSize);
        }

        StrategyContext context;
        context.bundleName = bundleName;
        context.bundleNameIndexInfo = bundleName;
        context.bundleInfo = it->second;
        context.userId = userId;
        context.isIncBackup = isIncBackup_;
        context.session = wptr<SvcSessionManager>(session_);
        context.service = wptr<Service>(this);
        context.bundleNameDetailMap = &bundleNameDetailMap;
        context.isClearDataFlags = &isClearDataFlags;

        PropertyStrategyExecutor::GetInstance().ExecuteStrategies(context, STRATEGY_LIST);
    }
    return BError(BError::Codes::OK);
}

std::vector<std::string> Service::GetSupportBundleNamesBackup(const vector<BundleName> &bundleNames,
    std::vector<BJsonEntityCaps::BundleInfo> &backupInfos)
{
    auto bundleDetails = MakeDetailList(bundleNames);
    auto defaultFlag = GetDefaultBundleResult(bundleNames);
    backupInfos = BundleMgrAdapter::GetBundleInfosForAppendBundles(bundleDetails,
        session_->GetSessionUserId(), defaultFlag);
    // 这里要走service里面的原逻辑，后面细化的时候再处理
    std::vector<std::string> supportBundleNames = GetSupportBackupBundleNames(backupInfos,
        false, bundleNames, defaultFlag);
    if (supportBundleNames.empty()) {
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGW("supportBundleNames is empty.");
    }
    return supportBundleNames;
}

void Service::SetDefaultApps(const vector<string> &bundleNames,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> detailMap)
{
    for (const auto &bundlePair : detailMap) {
        for (const auto &detailInfo : bundlePair.second) {
            SetDefaultBundleName(bundleNames, detailInfo.isDefaultBackupAndRestore);
        }
    }
}

void Service::SetDefaultBundleName(const vector<string> &bundleNames, bool result)
{
    for (const auto &bundleName : bundleNames) {
        defaultBundleMap_[bundleName] = result;
        HILOGI("bundleName:%{public}s, result:%{public}d", bundleName.c_str(), result);
    }
}

bool Service::GetDefaultBundleResult(const vector<string> &bundleNames)
{
    for (const auto &bundleName : bundleNames) {
        return GetDefaultBundleResult(bundleName);
    }
    return false;
}

bool Service::GetDefaultBundleResult(const string &bundleName)
{
    auto it = defaultBundleMap_.find(bundleName);
    if (it == defaultBundleMap_.end()) {
        return false;
    }
    return it->second;
}

void Service::StartBundleTaskBackup(const std::string &bundleName)
{
    HILOGI("Begin handle current bundle full backup or full restore, bundleName:%{public}s", bundleName.c_str());
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    ErrCode ret = ERR_OK;
    auto instance = GetMigrateInstance(wptr<Service>(this), bundleName, GetUserIdDefault());
    if (instance == nullptr) {
        HILOGE("Failed to GetMigrateInstance");
        return;
    }
    ret = instance->HandleBackup(session_->GetClearDataFlag(bundleName), bundleName);
    session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
    BundleBeginRadarReport(bundleName, ret, scenario);
    if (ret) {
        SendEndAppGalleryNotify(bundleName);
        ClearSessionAndSchedInfo(bundleName);
        NoticeClientFinish(bundleName, BError(BError::Codes::SA_INVAL_ARG));
    }
    HILOGI("End handle current bundle full backup or full restore, bundleName:%{public}s", bundleName.c_str());
}

ErrCode Service::DefaultAppFileReady(const std::string &fileName, const std::string &filePath,
    int fd, int32_t errCode)
{
    UniqueFd fdUnique(fd);
    return DefaultAppFileReady(fileName, filePath, std::move(fdUnique), errCode);
}

ErrCode Service::DefaultAppFileReadyWithoutFd(const std::string &fileName, const std::string &filePath,
    int32_t errCode)
{
    return DefaultAppFileReady(fileName, filePath, UniqueFd(INVALID_FD), errCode);
}

ErrCode Service::DefaultAppFileReady(const string &fileName, const std::string &filePath,
    UniqueFd fd, int32_t errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_ == nullptr) {
            HILOGE("AppFileReady error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        string callerName = GetKeyInMap(defaultBundleMap_, filePath);
        ErrCode ret = ERR_OK;
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

ErrCode Service::InitSession(const sptr<IServiceReverse>& remote,
    IServiceReverseType::Scenario &scenario, BizScene &scene)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    Duration totalSpend;
    totalSpend.Start();
    ErrCode ret = VerifyCaller();
    if (ret != ERR_OK) {
        HILOGE("Init full backup session fail, verify caller failed");
        return ret;
    }
    ret = session_->Active({
        .clientToken = IPCSkeleton::GetCallingTokenID(),
        .scenario = scenario,
        .clientProxy = remote,
        .userId = GetUserIdDefault(),
        .callerName = GetCallerName(),
        .activeTime = TimeUtils::GetCurrentTime(),
    });
    if (ret == ERR_OK) {
        TotalStatStart(scene, GetCallerName(), totalSpend.startMilli_);
        ClearFailedBundles();
        successBundlesNum_ = 0;
        CreateRunningLock();
        ClearBundleRadarReport();
        ClearFileReadyRadarReport();
        return ret;
    }
    if (ret == BError(BError::Codes::SA_SESSION_CONFLICT)) {
        HILOGE("Active backup session error, Already have a session");
        return ret;
    }
    HILOGE("Active backup session error");
    StopAll(nullptr, true);
    return ret;
}
ErrCode Service::InitSessionWithErrMsg(const sptr<IServiceReverse>& remote, IServiceReverseType::Scenario &scenario,
    BizScene &scene, int32_t &errCodeForMsg, std::string& errMsg)
{
    errCodeForMsg = InitSession(remote, scenario, scene);
    if (errCodeForMsg == BError(BError::Codes::SA_SESSION_CONFLICT)) {
        errMsg = BJsonUtil::BuildInitSessionErrInfo(session_->GetSessionUserId(),
                                                    session_->GetSessionCallerName(),
                                                    session_->GetSessionActiveTime(),
                                                    session_->GetScenarioStr());
    }
    HILOGI("Start InitBackupSessionWithErrMsg, errCode:%{public}d, Msg :%{public}s",
           errCodeForMsg,
           errMsg.c_str());
    return ERR_OK;
}
ErrCode Service::AppendBundlesSession(const std::vector<BundleName> &bundleNames, BizScene &scene,
    UniqueFd fd)
{
    try {
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        std::vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        vector<string> supportBundleNames = CallGetSupportBundleNames(bundleNames, bundleInfos, scene, move(fd));
        AppendBundles(supportBundleNames);
        CallSetSessProperties(supportBundleNames, bundleInfos, scene);
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("Catch exception");
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return e.GetCode();
    } catch (...) {
        HILOGE("Unexpected exception");
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

ErrCode Service::InitRestoreSession(const sptr<IServiceReverse> &remote)
{
    auto scenario = IServiceReverseType::Scenario::RESTORE;
    auto bizScene = BizScene::RESTORE;
    return InitSession(remote, scenario, bizScene);
}

ErrCode Service::InitRestoreSessionWithErrMsg(const sptr<IServiceReverse> &remote,
    int32_t &errCodeForMsg, std::string &errMsg)
{
    auto scenario = IServiceReverseType::Scenario::RESTORE;
    auto bizScene = BizScene::RESTORE;
    return InitSessionWithErrMsg(remote, scenario, bizScene, errCodeForMsg, errMsg);
}

ErrCode Service::InitBackupSession(const sptr<IServiceReverse> &remote)
{
    auto scenario = IServiceReverseType::Scenario::BACKUP;
    auto bizScene = BizScene::BACKUP;
    int32_t oldSize = StorageMgrAdapter::UpdateMemPara(BConstants::BACKUP_VFS_CACHE_PRESSURE);
    HILOGI("InitBackupSession oldSize %{public}d", oldSize);
    session_->SetMemParaCurSize(oldSize);
    return InitSession(remote, scenario, bizScene);
}

ErrCode Service::InitBackupSessionWithErrMsg(const sptr<IServiceReverse> &remote,
    int32_t &errCodeForMsg, std::string &errMsg)
{
    auto scenario = IServiceReverseType::Scenario::BACKUP;
    auto bizScene = BizScene::BACKUP;
    int32_t oldSize = StorageMgrAdapter::UpdateMemPara(BConstants::BACKUP_VFS_CACHE_PRESSURE);
    HILOGI("InitBackupSession oldSize %{public}d", oldSize);
    session_->SetMemParaCurSize(oldSize);
    return InitSessionWithErrMsg(remote, scenario, bizScene, errCodeForMsg, errMsg);
}

ErrCode Service::AppendBundlesRestoreSessionData(int fd, const std::vector<std::string> &bundleNames,
                                                 int32_t restoreType, int32_t userId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (session_ == nullptr || isOccupyingSession_.load()) {
        HILOGE("AppendBundles restore session with infos error, session is empty");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    if (restoreType < RestoreTypeEnum::RESTORE_DATA_WAIT_SEND || restoreType > RestoreTypeEnum::RESTORE_DATA_READDY) {
        HILOGE("restoreType is invalid:%{public}d", restoreType);
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    HILOGI("Begin fd = %{public}d,restoreType = %{public}d,userId=%{public}d", fd, restoreType, userId);
    UniqueFd fdUnique(fd);
    restoreType_ = static_cast<RestoreTypeEnum>(restoreType);
    SetUserIdAndRestoreType(restoreType_, userId);
    auto bizScene = BizScene::RESTORE;
    return AppendBundlesSession(bundleNames, bizScene, std::move(fdUnique));
}

ErrCode Service::AppendBundlesRestoreSessionDataByDetail(int fd, const std::vector<std::string> &bundleNames,
                                                         const std::vector<std::string> &detailInfos,
                                                         int32_t restoreType, int32_t userId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (session_ == nullptr || isOccupyingSession_.load()) {
        HILOGE("AppendBundles restore session with infos error, session is empty");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    if (restoreType < RestoreTypeEnum::RESTORE_DATA_WAIT_SEND || restoreType > RestoreTypeEnum::RESTORE_DATA_READDY) {
        HILOGE("restoreType is invalid:%{public}d", restoreType);
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    HILOGI("Begin fd = %{public}d,restoreType = %{public}d,userId=%{public}d", fd, restoreType, userId);
    UniqueFd fdUnique(fd);
    restoreType_ = static_cast<RestoreTypeEnum>(restoreType);
    SetUserIdAndRestoreType(restoreType_, userId);
    auto bizScene = BizScene::RESTORE;
    return AppendBundlesSessionWithDetail(bundleNames, detailInfos, bizScene, std::move(fdUnique));
}

ErrCode Service::AppendBundlesSessionWithDetail(const std::vector<BundleName> &bundleNames,
    const std::vector<std::string> &detailInfos,
    BizScene &scene,
    UniqueFd fd)
{
    try {
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, BJsonUtil::BundleSettingInfo> bundleSettingInfos;
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap =
            BJsonUtil::BuildBundleInfos(bundleNames, detailInfos, bundleNamesOnly,
            session_->GetSessionUserId(), bundleSettingInfos);
        std::vector<BundleName> newBundleNames = HandleBroadcastOnlyBundles(bundleNameDetailMap, bundleNames);
        if (newBundleNames.empty()) {
            HILOGE("newBundleNames is empty.");
            return BError(BError::Codes::OK);
        }
        SetDefaultApps(newBundleNames, bundleNameDetailMap);
        std::vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        vector<string> supportBundleNames = CallGetSupportBundleNames(newBundleNames, bundleInfos, scene, move(fd));
        AppendBundles(supportBundleNames);
        CallSetSessPropertiesWithDetail(supportBundleNames, bundleInfos,
            bundleNameDetailMap, bundleSettingInfos, scene);
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("Catch exception");
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return e.GetCode();
    } catch (...) {
        HILOGE("Unexpected exception");
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

ErrCode Service::AppDone(ErrCode errCode, const std::string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_ == nullptr) {
            HILOGE("App finish error, session info is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        HILOGI("Begin, bundleName is: %{public}s, errCode: %{public}d", bundleName.c_str(), errCode);
        if (session_->OnBundleFileReady(bundleName) || errCode != BError(BError::Codes::OK)) {
            SetExtOnRelease(bundleName, true);
            return BError(BError::Codes::OK);
        }
        RemoveExtensionMutex(bundleName);
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
}