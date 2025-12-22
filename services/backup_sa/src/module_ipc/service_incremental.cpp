/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include <fcntl.h>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>

#include <directory_ex.h>
#include <unique_fd.h>

#include "accesstoken_kit.h"
#include "b_anony/b_anony.h"
#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_filesystem/b_dir.h"
#include "b_hiaudit/hi_audit.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_caps.h"
#include "b_ohos/startup/backup_para.h"
#include "b_process/b_multiuser.h"
#include "b_radar/b_radar.h"
#include "b_resources/b_constants.h"
#include "b_sa/b_sa_utils.h"
#include "b_utils/b_time.h"
#include "filemgmt_libhilog.h"
#include "hisysevent.h"
#include "ipc_skeleton.h"
#include "module_external/bms_adapter.h"
#include "module_ipc/svc_backup_connection.h"
#include "module_ipc/svc_restore_deps_manager.h"
#include "module_notify/notify_work_service.h"
#include "parameter.h"
#include "system_ability_definition.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
const std::string FILE_BACKUP_EVENTS = "FILE_BACKUP_EVENTS";

namespace {
constexpr int32_t INDEX = 3;
constexpr int32_t MS_1000 = 1000;
constexpr int32_t INCREMENTAL_COUNT = 1;
const static string UNICAST_TYPE = "unicast";
} // namespace

ErrCode Service::Release()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("KILL");
    if (session_ == nullptr) {
        HILOGE("Release error, session is empty");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    VerifyCaller(scenario);
    AppRadar::Info info("", "", "call release");
    if (scenario == IServiceReverseType::Scenario::RESTORE) {
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::Release", session_->GetSessionUserId(),
            BizStageRestore::BIZ_STAGE_RELEASE, ERR_OK);
    } else if (scenario == IServiceReverseType::Scenario::BACKUP) {
        AppRadar::GetInstance().RecordBackupFuncRes(info, "Service::Release", session_->GetSessionUserId(),
            BizStageBackup::BIZ_STAGE_RELEASE, ERR_OK);
    }
    SessionDeactive();
    return BError(BError::Codes::OK);
}

std::shared_ptr<ExtensionMutexInfo> Service::GetExtensionMutex(const BundleName &bundleName)
{
    std::unique_lock<std::mutex> lock(extensionMutexLock_);
    auto it = backupExtMutexMap_.find(bundleName);
    if (it == backupExtMutexMap_.end()) {
        HILOGI("BackupExtMutexMap not contain %{public}s", bundleName.c_str());
        backupExtMutexMap_[bundleName] = std::make_shared<ExtensionMutexInfo>(bundleName);
        return backupExtMutexMap_[bundleName];
    }
    HILOGI("BackupExtMutexMap contain %{public}s", bundleName.c_str());
    return it->second;
}

void Service::RemoveExtensionMutex(const BundleName &bundleName)
{
    std::unique_lock<std::mutex> lock(extensionMutexLock_);
    auto it = backupExtMutexMap_.find(bundleName);
    if (it == backupExtMutexMap_.end()) {
        HILOGI("BackupExtMutexMap not contain %{public}s", bundleName.c_str());
        return;
    }
    backupExtMutexMap_.erase(it);
}

void Service::CreateDirIfNotExist(const std::string &path)
{
    if (access(path.c_str(), F_OK) != 0) {
        bool created = ForceCreateDirectory(path.data());
        if (created) {
            HILOGI("Create directory successfully.");
        } else {
            HILOGE("Failed to create directory, path = %{private}s, err = %{public}d", path.c_str(), errno);
        }
    }
}
ErrCode Service::GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames, int &fd)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    UniqueFd fdResult(GetLocalCapabilitiesIncremental(bundleNames));
    fd = dup(fdResult.Get());
    return BError(BError::Codes::OK); // anytime return OK
}

BJsonCachedEntity<BJsonEntityCaps> Service::CreateJsonEntity(UniqueFd &fd,
    vector<BJsonEntityCaps::BundleInfo>& bundleInfos, const std::vector<BIncrementalData> &bundleNames)
{
    BJsonCachedEntity<BJsonEntityCaps> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    std::string backupVersion = BJsonUtil::ParseBackupVersion();
    cache.SetBackupVersion(backupVersion);
    cache.SetSystemFullName(GetOSFullName());
    cache.SetDeviceType(GetDeviceType());
    cache.SetIncreAppIndex(true);
    bundleInfos = BundleMgrAdapter::GetBundleInfosForIncremental(GetUserIdDefault(), bundleNames);
    cache.SetBundleInfos(bundleInfos, true);
    cachedEntity.Persist();
    HILOGI("Service GetLocalCapabilitiesIncremental persist, bundleInfos size:%{public}zu", bundleInfos.size());
    isCreatingIncreaseFile_.fetch_sub(INCREMENTAL_COUNT);
    if (isCreatingIncreaseFile_.load() <= 0) {
        isCreatingIncreaseFile_.store(0);
        const int userId = GetUserIdDefault();
        for (const auto &bundleInfo : bundleInfos) {
            ClearIncrementalStatFile(userId, bundleInfo.name);
        }
        HILOGI("do ClearIncrementalStatFile finished");
    }
    return cachedEntity;
}

UniqueFd Service::GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("Get LocalCapabilities Incremental Error, session is empty or cleaning up the service");
            return UniqueFd(-ENOENT);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        isCreatingIncreaseFile_.fetch_add(INCREMENTAL_COUNT);
        ErrCode errCode = VerifyCaller();
        if (errCode != ERR_OK) {
            HILOGE("Get local abilities info failed, Verify caller failed, errCode:%{public}d", errCode);
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            isCreatingIncreaseFile_.fetch_sub(INCREMENTAL_COUNT);
            return UniqueFd(-ENOENT);
        }
        string path = BConstants::GetSaBundleBackupRootDir(GetUserIdDefault());
        BExcepUltils::VerifyPath(path, false);
        CreateDirIfNotExist(path);
        UniqueFd fd(open(path.data(), O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR));
        if (fd < 0) {
            HILOGE("GetLocalCapabilitiesIncremental: open file failed, err = %{public}d", errno);
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            isCreatingIncreaseFile_.fetch_sub(INCREMENTAL_COUNT);
            return UniqueFd(-ENOENT);
        }
        vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        BJsonCachedEntity<BJsonEntityCaps> cachedEntity = CreateJsonEntity(fd, bundleInfos, bundleNames);
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return move(cachedEntity.GetFd());
    } catch (const BError &e) {
        isCreatingIncreaseFile_.fetch_sub(INCREMENTAL_COUNT);
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGE("GetLocalCapabilitiesIncremental failed, errCode = %{public}d", e.GetCode());
        return UniqueFd(-e.GetCode());
    } catch (...) {
        isCreatingIncreaseFile_.fetch_sub(INCREMENTAL_COUNT);
        HILOGE("Unexpected exception");
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return UniqueFd(-EPERM);
    }
}

void Service::StartGetFdTask(std::string bundleName, wptr<Service> ptr)
{
    auto thisPtr = ptr.promote();
    if (!thisPtr) {
        HILOGE("this pointer is null");
        return;
    }
    auto session = thisPtr->session_;
    if (session == nullptr) {
        throw BError(BError::Codes::SA_INVAL_ARG, "session is nullptr");
    }
    auto backUpConnection = session->GetExtConnection(bundleName);
    if (backUpConnection == nullptr) {
        throw BError(BError::Codes::SA_INVAL_ARG, "backUpConnection is empty");
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
    }
    // distinguish whether it is 0 user
    if (BundleMgrAdapter::IsUser0BundleName(bundleName, session_->GetSessionUserId())) {
        if (proxy->User0OnBackup()) {
            SendEndAppGalleryNotify(bundleName);
            thisPtr->ClearSessionAndSchedInfo(bundleName);
            thisPtr->NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_DIED));
        }
        return;
    }
    if (!session->StopExtTimer(bundleName)) {
        throw BError(BError::Codes::SA_INVAL_ARG, "StopExtTimer error");
    }
    int64_t lastTime = session->GetLastIncrementalTime(bundleName);
    std::vector<BIncrementalData> bundleNames;
    bundleNames.emplace_back(BIncrementalData {bundleName, lastTime});
    auto newBundleInfos = BundleMgrAdapter::GetBundleInfosForIncremental(bundleNames, session->GetSessionUserId());
    RefreshBundleDataSize(newBundleInfos, bundleName, ptr);
    string path = BConstants::GetSaBundleBackupRootDir(session->GetSessionUserId()).
        append(BundleMgrAdapter::GetBundleIndexName(bundleName)).
        append("/").append(BConstants::BACKUP_STAT_SYMBOL).append(to_string(lastTime));
    UniqueFd fdLocal(open(path.data(), O_RDWR, S_IRGRP | S_IWGRP));
    if (fdLocal < 0) {
        HILOGD("fdLocal open fail, error = %{public}d", errno);
        throw BError(BError::Codes::SA_INVAL_ARG, "open local Manifest file failed");
    }
    UniqueFd lastManifestFd(session->GetIncrementalManifestFd(bundleName));
    auto ret = proxy->HandleIncrementalBackup(move(fdLocal), move(lastManifestFd));
    if (ret) {
        SendEndAppGalleryNotify(bundleName);
        thisPtr->ClearSessionAndSchedInfo(bundleName);
        thisPtr->NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_DIED));
    }
}

void Service::RefreshBundleDataSize(const vector<BJsonEntityCaps::BundleInfo> &newBundleInfos,
    std::string bundleName, wptr<Service> ptr)
{
    auto thisPtr = ptr.promote();
    if (!thisPtr) {
        HILOGE("this pointer is null");
        return;
    }
    auto session = thisPtr->session_;
    if (session == nullptr) {
        HILOGE("session is nullptr");
        return;
    }
    BJsonUtil::BundleDetailInfo bundleInfo = BJsonUtil::ParseBundleNameIndexStr(bundleName);
    for (const auto &info : newBundleInfos) {
        if (info.name == bundleInfo.bundleName && info.appIndex == bundleInfo.bundleIndex) {
            session->SetBundleDataSize(bundleName, info.increSpaceOccupied);
            HILOGI("RefreshBundleDataSize, bundlename = %{public}s , datasize = %{public}" PRId64 "",
                bundleName.c_str(), info.increSpaceOccupied);
        }
    }
}

ErrCode Service::GetAppLocalListAndDoIncrementalBackup()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("session is nullptr");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        session_->SetSessionUserId(GetUserIdDefault());
        std::string bundleName;
        ErrCode ret = VerifyCallerAndGetCallerName(bundleName);
        if (ret != ERR_OK) {
            HILOGE("Get AppLocalList failed, Get bundle failed, ret:%{public}d", ret);
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return ret;
        }
        auto task = [this, bundleName]() {
            StartGetFdTask(bundleName, wptr(this));
        };
        threadPool_.AddTask([task]() {
            try {
                task();
            } catch (const BError &e) {
                HILOGE("GetAppLocalListAndDoIncrementalBackup failed, errCode = %{public}d", e.GetCode());
            } catch (...) {
                HILOGE("Unexpected exception");
            }
        });
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("GetAppLocalListAndDoIncrementalBackup failed, errCode = %{public}d", e.GetCode());
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return e.GetCode();
    } catch (...) {
        HILOGE("Unexpected exception");
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

ErrCode Service::InitIncrementalBackupSession(const sptr<IServiceReverse>& remote)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    Duration totalSpend;
    totalSpend.Start();
    ErrCode errCode = VerifyCaller();
    if (errCode != ERR_OK) {
        HILOGE("Init incremental backup session fail, Verify caller failed, errCode:%{public}d", errCode);
        return errCode;
    }
    if (session_ == nullptr) {
        HILOGE("Init Incremental backup session  error, session is empty");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    errCode = session_->Active({.clientToken = IPCSkeleton::GetCallingTokenID(),
                                .scenario = IServiceReverseType::Scenario::BACKUP,
                                .clientProxy = remote,
                                .userId = GetUserIdDefault(),
                                .isIncrementalBackup = true,
                                .callerName = GetCallerName(),
                                .activeTime = TimeUtils::GetCurrentTime()});
    if (errCode == ERR_OK) {
        HILOGE("Success to init a new incremental backup session");
        TotalStatStart(BizScene::BACKUP, GetCallerName(), totalSpend.startMilli_, Mode::INCREMENTAL);
        ClearFailedBundles();
        CreateRunningLock();
        successBundlesNum_ = 0;
        ClearBundleRadarReport();
        ClearFileReadyRadarReport();
        return errCode;
    }
    if (errCode == BError(BError::Codes::SA_SESSION_CONFLICT)) {
        HILOGE("Active incremental backup error, Already have a session");
        return errCode;
    }
    HILOGE("Active backup session error");
    StopAll(nullptr, true);
    return errCode;
}

ErrCode Service::InitIncrementalBackupSessionWithErrMsg(const sptr<IServiceReverse>& remote, int32_t &errCodeForMsg,
                                                        std::string &errMsg)
{
    errCodeForMsg = InitIncrementalBackupSession(remote, errMsg);
    HILOGI("Start InitIncrementalBackupSessionWithErrMsg, errCode:%{public}d, Msg :%{public}s",
           errCodeForMsg,
           errMsg.c_str());
    return ERR_OK;
}

ErrCode Service::InitIncrementalBackupSession(const sptr<IServiceReverse>& remote, std::string &errMsg)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    Duration totalSpend;
    totalSpend.Start();
    ErrCode errCode = VerifyCaller();
    if (errCode != ERR_OK) {
        HILOGE("Init incremental backup session fail, Verify caller failed, errCode:%{public}d", errCode);
        return errCode;
    }
    if (session_ == nullptr) {
        HILOGE("Init Incremental backup session  error, session is empty");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    errCode = session_->Active({.clientToken = IPCSkeleton::GetCallingTokenID(),
                                .scenario = IServiceReverseType::Scenario::BACKUP,
                                .clientProxy = remote,
                                .userId = GetUserIdDefault(),
                                .isIncrementalBackup = true,
                                .callerName = GetCallerName(),
                                .activeTime = TimeUtils::GetCurrentTime()});
    if (errCode == ERR_OK) {
        TotalStatStart(BizScene::BACKUP, GetCallerName(), totalSpend.startMilli_, Mode::INCREMENTAL);
        ClearFailedBundles();
        CreateRunningLock();
        successBundlesNum_ = 0;
        ClearBundleRadarReport();
        ClearFileReadyRadarReport();
        return errCode;
    }
    if (errCode == BError(BError::Codes::SA_SESSION_CONFLICT)) {
        errMsg = BJsonUtil::BuildInitSessionErrInfo(session_->GetSessionUserId(),
                                                    session_->GetSessionCallerName(),
                                                    session_->GetSessionActiveTime(),
                                                    session_->GetScenarioStr());
        HILOGE("Active incremental backup session error, Already have a session");
        return errCode;
    }
    HILOGE("Active incremental backup session error");
    StopAll(nullptr, true);
    return errCode;
}

vector<string> Service::GetBundleNameByDetails(const std::vector<BIncrementalData> &bundlesToBackup)
{
    vector<string> bundleNames {};
    for (const auto &bundle : bundlesToBackup) {
        bundleNames.emplace_back(bundle.bundleName);
    }
    return bundleNames;
}

ErrCode Service::AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    vector<string> bundleNames;
    try {
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("Init Incremental backup session  error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__); // BundleMgrAdapter::GetBundleInfos可能耗时
        bundleNames = GetBundleNameByDetails(bundlesToBackup);
        ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::BACKUP);
        if (ret != ERR_OK) {
            HILOGE("Append bundles incremental session failed, verify caller failed, ret:%{public}d", ret);
            HandleExceptionOnAppendBundles(session_, bundleNames, {});
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return ret;
        }
        auto backupInfos = BundleMgrAdapter::GetBundleInfosForAppendBundles(bundlesToBackup,
            session_->GetSessionUserId());
        std::vector<std::string> supportBackupNames = GetSupportBackupBundleNames(backupInfos, true, bundleNames);
        AppendBundles(supportBackupNames);
        SetBundleIncDataInfo(bundlesToBackup, supportBackupNames);
        SetCurrentBackupSessProperties(supportBackupNames, session_->GetSessionUserId(), backupInfos, true);
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("Failed, errCode = %{public}d", e.GetCode());
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
ErrCode Service::AppendBundlesIncrementalBackupSessionWithBundleInfos(
    const std::vector<BIncrementalData> &bundlesToBackup, const std::vector<std::string> &bundleInfos)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    return AppendBundlesIncrementalBackupSession(bundlesToBackup, bundleInfos);
}

ErrCode Service::AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup,
    const std::vector<std::string> &infos)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    vector<string> bundleNames;
    try {
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("Init Incremental backup session error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__); // BundleMgrAdapter::GetBundleInfos可能耗时
        bundleNames = GetBundleNameByDetails(bundlesToBackup);
        ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::BACKUP);
        if (ret != ERR_OK) {
            HILOGE("Append bundles incremental session with infos failed, verify caller failed, ret:%{public}d", ret);
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            HandleExceptionOnAppendBundles(session_, bundleNames, {});
            return ret;
        }
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, bool> isClearDataFlags;
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap =
            BJsonUtil::BuildBundleInfos(bundleNames, infos, bundleNamesOnly,
            session_->GetSessionUserId(), isClearDataFlags);
        auto backupInfos = BundleMgrAdapter::GetBundleInfosForAppendBundles(bundlesToBackup,
            session_->GetSessionUserId());
        std::vector<std::string> supportBackupNames = GetSupportBackupBundleNames(backupInfos, true, bundleNames);
        AppendBundles(supportBackupNames);
        SetBundleIncDataInfo(bundlesToBackup, supportBackupNames);
        HandleCurGroupIncBackupInfos(backupInfos, bundleNameDetailMap, isClearDataFlags);
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("Failed, errCode = %{public}d", e.GetCode());
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

void Service::HandleCurGroupIncBackupInfos(vector<BJsonEntityCaps::BundleInfo> &backupInfos,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> &bundleNameDetailMap,
    std::map<std::string, bool> &isClearDataFlags)
{
    for (auto &info : backupInfos) {
        HILOGI("Current backupInfo bundleName:%{public}s, index:%{public}d, extName:%{public}s", info.name.c_str(),
            info.appIndex, info.extensionName.c_str());
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(info.name, info.appIndex);
        SetCurrentSessProperties(info, isClearDataFlags, bundleNameIndexInfo);
        session_->SetBundleDataSize(bundleNameIndexInfo, info.increSpaceOccupied);
        BJsonUtil::BundleDetailInfo uniCastInfo;
        if (BJsonUtil::FindBundleInfoByName(bundleNameDetailMap, bundleNameIndexInfo, UNICAST_TYPE, uniCastInfo)) {
            HILOGI("current bundle:%{public}s, unicast info:%{public}s", bundleNameIndexInfo.c_str(),
                GetAnonyString(uniCastInfo.detail).c_str());
            session_->SetBackupExtInfo(bundleNameIndexInfo, uniCastInfo.detail);
        }
        session_->SetBackupExtName(bundleNameIndexInfo, info.extensionName);
        session_->SetIsReadyLaunch(bundleNameIndexInfo);
    }
}

ErrCode Service::PublishIncrementalFile(const BFileInfo &fileInfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::RESTORE);
    if (ret != ERR_OK) {
        HILOGE("Publish incremental file failed, bundleName:%{public}s", fileInfo.owner.c_str());
        return ret;
    }
    HILOGI("Start get ExtConnection, bundleName:%{public}s", fileInfo.owner.c_str());
    if (!fileInfo.fileName.empty()) {
        HILOGE("Forbit to use PublishIncrementalFile with fileName for App");
        return EPERM;
    }
    if (session_ == nullptr) {
        HILOGE("session is empty, bundleName:%{public}s", fileInfo.owner.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    session_->SetPublishFlag(fileInfo.owner);
    auto backUpConnection = session_->GetExtConnection(fileInfo.owner);
    if (backUpConnection == nullptr) {
        HILOGE("backUpConnection is empty, bundle:%{public}s", fileInfo.owner.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("Publish Incremental file failed, bundleName:%{public}s", fileInfo.owner.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    ret = proxy->PublishIncrementalFile(fileInfo.fileName);
    if (ret != ERR_OK) {
        HILOGE("Failed to publish file for backup extension, bundleName:%{public}s", fileInfo.owner.c_str());
        return ret;
    }
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishSAIncrementalFile(const BFileInfo& fileInfo, int fd)
{
    UniqueFd uniquedParameter(fd);
    return PublishSAIncrementalFile(fileInfo, std::move(uniquedParameter));
}

ErrCode Service::PublishSAIncrementalFile(const BFileInfo &fileInfo, UniqueFd fd)
{
    std::string bundleName = fileInfo.owner;
    if (totalStatistic_ != nullptr) {
        std::unique_lock<std::shared_mutex> mapLock(statMapMutex_);
        std::shared_ptr<RadarAppStatistic> saStatistic = std::make_shared<RadarAppStatistic>(bundleName,
            totalStatistic_->GetUniqId(), totalStatistic_->GetBizScene());
        saStatistic->doRestoreStart_ = static_cast<uint64_t>(TimeUtils::GetTimeMS());
        saStatisticMap_[bundleName] = saStatistic;
    }
    ErrCode errCode = VerifyCaller();
    if (errCode != ERR_OK) {
        HILOGE("PublishSAIncrementalFile failed, verify caller failed, bundleName:%{public}s, errCode:%{public}d",
            bundleName.c_str(), errCode);
        return errCode;
    }
    if (!SAUtils::IsSABundleName(bundleName)) {
        HILOGE("Bundle name %{public}s is not sa", bundleName.c_str());
        return BError(BError::Codes::SA_EXT_ERR_CALL);
    }
    HILOGI("Bundle name %{public}s is sa, publish sa incremental file", bundleName.c_str());
    auto backupConnection = session_->GetSAExtConnection(bundleName);
    std::shared_ptr<SABackupConnection> saConnection = backupConnection.lock();
    if (saConnection == nullptr) {
        HILOGE("lock sa connection ptr is nullptr");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    return saConnection->CallRestoreSA(move(fd));
}

ErrCode Service::AppIncrementalFileReady(const std::string &bundleName, const std::string &fileName, UniqueFd fd,
                                         UniqueFd manifestFd, int32_t errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        bool fdFlag = (fd < 0 || manifestFd < 0) ? true : false;
        if (session_->GetScenario() == IServiceReverseType::Scenario::RESTORE) {
            fdFlag ? session_->GetServiceReverseProxy()->IncrementalRestoreOnFileReadyWithoutFd(bundleName, fileName,
                                                                                                errCode) :
                     session_->GetServiceReverseProxy()->IncrementalRestoreOnFileReady(bundleName, fileName, move(fd),
                                                                                       move(manifestFd), errCode);
            FileReadyRadarReport(bundleName, fileName, errCode, IServiceReverseType::Scenario::RESTORE);
            return BError(BError::Codes::OK);
        }
        if (fileName == BConstants::EXT_BACKUP_MANAGE) {
            fd = session_->OnBundleExtManageInfo(bundleName, move(fd));
        }
        fdFlag = (fd < 0 || manifestFd < 0) ? true : false;
        fdFlag ? session_->GetServiceReverseProxy()->IncrementalBackupOnFileReadyWithoutFd(bundleName, fileName,
                                                                                           errCode) :
                 session_->GetServiceReverseProxy()->IncrementalBackupOnFileReady(bundleName, fileName, move(fd),
                                                                                  move(manifestFd), errCode);
        FileReadyRadarReport(bundleName, fileName, errCode, IServiceReverseType::Scenario::BACKUP);
        if (session_->OnBundleFileReady(bundleName, fileName)) {
            ErrCode ret = HandleCurBundleFileReady(bundleName, fileName, true);
            if (ret != ERR_OK) {
                HILOGE("Handle current file failed, bundleName:%{public}s, fileName:%{public}s",
                    bundleName.c_str(), GetAnonyPath(fileName).c_str());
                return ret;
            }
        }
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("AppIncrementalFileReady exception");
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (...) {
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::AppIncrementalFileReady(const std::string &fileName, int fd, int manifestFd,
                                         int32_t appIncrementalFileReadyErrCode)
{
    HILOGI("Begin AppIncrementalFileReady:%{public}s, fd:%{public}d, manifestFd:%{public}d, errcode:%{public}d",
           fileName.c_str(), fd, manifestFd, appIncrementalFileReadyErrCode);
    UniqueFd fdUnique(fd);
    UniqueFd manifestFdUnique(manifestFd);
    return AppIncrementalFileReady(fileName, std::move(fdUnique), std::move(manifestFdUnique),
                                   appIncrementalFileReadyErrCode);
}

ErrCode Service::AppIncrementalFileReadyWithoutFd(const std::string &fileName, int32_t appIncrementalFileReadyErrCode)
{
    return AppIncrementalFileReady(fileName, UniqueFd(BConstants::INVALID_FD_NUM),
                                   UniqueFd(BConstants::INVALID_FD_NUM),
                                   appIncrementalFileReadyErrCode);
}

ErrCode Service::AppIncrementalFileReady(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd, int32_t errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName);
        if (ret != ERR_OK) {
            HILOGE("Verify caller failed, ret:%{public}d", ret);
            return ret;
        }
        bool fdFlag = (fd < 0 || manifestFd < 0) ? true : false;
        if (session_->GetScenario() == IServiceReverseType::Scenario::RESTORE) {
            fdFlag ? session_->GetServiceReverseProxy()->IncrementalRestoreOnFileReadyWithoutFd(callerName, fileName,
                                                                                                errCode) :
                     session_->GetServiceReverseProxy()->IncrementalRestoreOnFileReady(callerName, fileName, move(fd),
                                                                                       move(manifestFd), errCode);
            FileReadyRadarReport(callerName, fileName, errCode, IServiceReverseType::Scenario::RESTORE);
            return BError(BError::Codes::OK);
        }
        if (fileName == BConstants::EXT_BACKUP_MANAGE) {
            fd = session_->OnBundleExtManageInfo(callerName, move(fd));
        }
        fdFlag = (fd < 0 || manifestFd < 0) ? true : false;
        fdFlag ? session_->GetServiceReverseProxy()->IncrementalBackupOnFileReadyWithoutFd(callerName, fileName,
                                                                                           errCode) :
                 session_->GetServiceReverseProxy()->IncrementalBackupOnFileReady(callerName, fileName, move(fd),
                                                                                  move(manifestFd), errCode);
        FileReadyRadarReport(callerName, fileName, errCode, IServiceReverseType::Scenario::BACKUP);
        if (session_->OnBundleFileReady(callerName, fileName)) {
            ErrCode ret = HandleCurBundleFileReady(callerName, fileName, true);
            if (ret != ERR_OK) {
                HILOGE("Handle current file failed, bundleName:%{public}s, fileName:%{public}s",
                    callerName.c_str(), GetAnonyPath(fileName).c_str());
                return ret;
            }
        }
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("AppIncrementalFileReady exception");
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (...) {
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::AppIncrementalDone(ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_ == nullptr) {
            HILOGE("AppIncrementalDone error, session is null");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName);
        if (ret != ERR_OK) {
            HILOGE("App incremental done fail, ret:%{public}d", ret);
            return ret;
        }
        HILOGI("Service AppIncrementalDone start, callerName is %{public}s, errCode is: %{public}d",
            callerName.c_str(), errCode);
        ClearIncrementalStatFile(GetUserIdDefault(), callerName);
        if (session_->OnBundleFileReady(callerName) || errCode != BError(BError::Codes::OK)) {
            SetExtOnRelease(callerName, true);
            return BError(BError::Codes::OK);
        }
        RemoveExtensionMutex(callerName);
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        ReleaseOnException();
        HILOGE("AppIncrementalDone error, err code is:%{public}d", e.GetCode());
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (...) {
        ReleaseOnException();
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::GetIncrementalFileHandle(const std::string &bundleName, const std::string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin get incrementalFileHandle");
        if (session_ == nullptr) {
            HILOGE("GetIncrementalFileHandle error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::RESTORE);
        if (ret != ERR_OK) {
            HILOGE("Error, bundleName:%{public}s, fileName:%{public}s", bundleName.c_str(),
                   GetAnonyPath(fileName).c_str());
            return ret;
        }
        if (!BDir::IsFilePathValid(fileName)) {
            HILOGE("path is forbidden, path : %{public}s", GetAnonyPath(fileName).c_str());
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        auto action = session_->GetServiceSchedAction(bundleName);
        if (action == BConstants::ServiceSchedAction::UNKNOWN) {
            HILOGE("action is unknown, bundleName:%{public}s", bundleName.c_str());
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        if (action == BConstants::ServiceSchedAction::RUNNING) {
            HILOGI("Restore getIncrementalFileHandle begin, bundleName:%{public}s, fileName:%{public}s",
                bundleName.c_str(), GetAnonyPath(fileName).c_str());
            auto err = SendIncrementalFileHandle(bundleName, fileName);
            if (err != ERR_OK) {
                HILOGE("SendIncrementalFileHandle failed, bundle:%{public}s", bundleName.c_str());
                return err;
            }
        } else {
            SvcRestoreDepsManager::GetInstance().UpdateToRestoreBundleMap(bundleName, fileName);
            session_->SetExtFileNameRequest(bundleName, fileName);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("GetIncrementalFileHandle exception, bundleName:%{public}s", bundleName.c_str());
        return e.GetCode();
    }
}

ErrCode Service::SendIncrementalFileHandle(const std::string &bundleName, const std::string &fileName)
{
    auto backUpConnection = session_->GetExtConnection(bundleName);
    if (backUpConnection == nullptr) {
        HILOGE("backUpConnection is empty, bundle:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("GetIncrementalFileHandle failed, bundleName:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    int fdVal = BConstants::INVALID_FD_NUM;
    int reportFdVal = BConstants::INVALID_FD_NUM;
    int errCode = BConstants::INVALID_FD_NUM;
    proxy->GetIncrementalFileHandle(fileName, fdVal, reportFdVal, errCode);
    UniqueFd fd(fdVal);
    UniqueFd reportFd(reportFdVal);
    auto err = AppIncrementalFileReady(bundleName, fileName, move(fd), move(reportFd), errCode);
    if (err != ERR_OK) {
        HILOGE("Failed to send file handle, bundleName:%{public}s, fileName:%{public}s",
            bundleName.c_str(), GetAnonyPath(fileName).c_str());
        AppRadar::Info info (bundleName, "", "");
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::GetIncrementalFileHandle",
            GetUserIdDefault(), BizStageRestore::BIZ_STAGE_GET_FILE_HANDLE_FAIL, err);
    }
    return BError(BError::Codes::OK);
}

bool Service::IncrementalBackup(const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    auto backUpConnection = session_->GetExtConnection(bundleName);
    if (backUpConnection == nullptr) {
        HILOGE("backUpConnection is empty, bundle:%{public}s", bundleName.c_str());
        SendEndAppGalleryNotify(bundleName);
        NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_DIED));
        return true;
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("Increment backup error, extension proxy is empty, bundleName:%{public}s", bundleName.c_str());
        SendEndAppGalleryNotify(bundleName);
        NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_DIED));
        return true;
    }
    if (scenario == IServiceReverseType::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
        auto ret = proxy->IncrementalOnBackup(session_->GetClearDataFlag(bundleName));
        session_->GetServiceReverseProxy()->IncrementalBackupOnBundleStarted(ret, bundleName);
        BundleBeginRadarReport(bundleName, ret, IServiceReverseType::Scenario::BACKUP);
        if (ret) {
            SendEndAppGalleryNotify(bundleName);
            ClearSessionAndSchedInfo(bundleName);
            NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_DIED));
        }
        return true;
    } else if (scenario == IServiceReverseType::Scenario::RESTORE &&
               BackupPara().GetBackupOverrideIncrementalRestore() &&
               session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
        auto ret = proxy->HandleRestore(session_->GetClearDataFlag(bundleName));
        session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(ret, bundleName);
        std::string oldBackupVersion = session_->GetOldBackupVersion();
        if (oldBackupVersion.empty()) {
            HILOGE("Failed to get backupVersion of old device");
        }
        HILOGD("backupVersion of old device = %{public}s", oldBackupVersion.c_str());
        BundleBeginRadarReport(bundleName, ret, IServiceReverseType::Scenario::RESTORE);
        auto fileNameVec = session_->GetExtFileNameRequest(bundleName);
        for (const auto &fileName : fileNameVec) {
            ret = HelpToAppIncrementalFileReady(bundleName, fileName, proxy);
            if (ret) {
                HILOGE("Failed to send file handle %{public}s", GetAnonyString(fileName).c_str());
            }
        }
        return true;
    }
    return false;
}
ErrCode Service::HelpToAppIncrementalFileReady(const string &bundleName, const string &fileName, sptr<IExtension> proxy)
{
    int fdVal = BConstants::INVALID_FD_NUM;
    int reportFdVal = BConstants::INVALID_FD_NUM;
    int errCode = BConstants::INVALID_FD_NUM;
    proxy->GetIncrementalFileHandle(fileName, fdVal, reportFdVal, errCode);
    UniqueFd fd(fdVal);
    UniqueFd reportFd(reportFdVal);
    auto ret = AppIncrementalFileReady(bundleName, fileName, move(fd), move(reportFd), errCode);
    return ret;
}

ErrCode Service::IncrementalBackupSA(std::string bundleName)
{
    HILOGI("IncrementalBackupSA begin %{public}s", bundleName.c_str());
    if (totalStatistic_ != nullptr) {
        std::unique_lock<std::shared_mutex> mapLock(statMapMutex_);
        std::shared_ptr<RadarAppStatistic> saStatistic = std::make_shared<RadarAppStatistic>(bundleName,
            totalStatistic_->GetUniqId(), totalStatistic_->GetBizScene());
        saStatistic->doBackupSpend_.Start();
        saStatisticMap_[bundleName] = saStatistic;
    }
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    auto backUpConnection = session_->GetSAExtConnection(bundleName);
    std::shared_ptr<SABackupConnection> saConnection = backUpConnection.lock();
    if (saConnection == nullptr) {
        HILOGE("lock sa connection ptr is nullptr");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    if (scenario == IServiceReverseType::Scenario::BACKUP) {
        auto ret = saConnection->CallBackupSA();
        session_->GetServiceReverseProxy()->IncrementalBackupOnBundleStarted(ret, bundleName);
        BundleBeginRadarReport(bundleName, ret, scenario);
        if (ret) {
            HILOGE("IncrementalBackupSA ret is %{public}d", ret);
            ClearSessionAndSchedInfo(bundleName);
            NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_DIED));
            return BError(ret);
        }
    } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
        session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), bundleName);
    }
    return BError(BError::Codes::OK);
}

void Service::NotifyCallerCurAppIncrementDone(ErrCode errCode, const std::string &callerName)
{
    UpdateHandleCnt(errCode);
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    if (scenario == IServiceReverseType::Scenario::BACKUP) {
        HILOGI("will notify clone data, scenario is incremental backup");
        session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(errCode, callerName);
        BundleEndRadarReport(callerName, errCode, scenario);
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        std::stringstream strTime;
        strTime << (std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S:")) << (std::setfill('0'))
            << (std::setw(INDEX)) << (ms.count() % MS_1000);
        HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
            FILE_BACKUP_EVENTS,
            OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "PROC_NAME", "ohos.appfileservice", "BUNDLENAME", callerName,
            "PID", getpid(), "TIME", strTime.str()
        );
    } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
        HILOGI("will notify clone data, scenario is Restore");
        session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleFinished(errCode, callerName);
        BundleEndRadarReport(callerName, errCode, scenario);
    }
}

void Service::SendUserIdToApp(string &bundleName, int32_t userId)
{
    if (session_ == nullptr) {
        HILOGI("session_ is nullptr");
        return;
    }
    HILOGI("Begin, bundleName: %{public}s", bundleName.c_str());
    string detailInfo;
    if (!BJsonUtil::BuildBundleInfoJson(userId, detailInfo)) {
        HILOGE("BuildBundleInfoJson failed, bundleName : %{public}s", bundleName.c_str());
        return;
    }
    session_->SetBackupExtInfo(bundleName, detailInfo);
    HILOGI("End, bundleName:%{public}s, unicast info:%{public}s", bundleName.c_str(),
        GetAnonyString(detailInfo).c_str());
}

void Service::SetCurrentBackupSessProperties(const vector<string> &bundleNames, int32_t userId,
    vector<BJsonEntityCaps::BundleInfo> &backupBundleInfos, bool isIncBackup)
{
    HILOGI("start SetCurrentBackupSessProperties");
    std::map<std::string, BJsonEntityCaps::BundleInfo> bundleNameIndexBundleInfoMap;
    for (const auto &bundleInfo : backupBundleInfos) {
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(bundleInfo.name, bundleInfo.appIndex);
        bundleNameIndexBundleInfoMap[bundleNameIndexInfo] = bundleInfo;
    }
    for (const auto &item : bundleNames) {
        std::string bundleName = item;
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
        auto bundleInfo = it->second;
        if (isIncBackup) {
            session_->SetBundleDataSize(bundleName, bundleInfo.increSpaceOccupied);
        } else {
            session_->SetBundleDataSize(bundleName, bundleInfo.spaceOccupied);
        }
        session_->SetBundleUserId(bundleName, userId);
        session_->SetBackupExtName(bundleName, bundleInfo.extensionName);
        session_->SetIsReadyLaunch(bundleName);
    }
    HILOGI("end SetCurrentBackupSessProperties");
}


void Service::SetBundleIncDataInfo(const std::vector<BIncrementalData>& bundlesToBackup,
    std::vector<std::string> &supportBundleNames)
{
    for (const auto &bundleInfo : bundlesToBackup) {
        std::string bundleName = bundleInfo.bundleName;
        auto it = std::find(supportBundleNames.begin(), supportBundleNames.end(), bundleName);
        if (it == supportBundleNames.end()) {
            HILOGE("Current bundle is not support to backup, bundleName:%{public}s", bundleName.c_str());
            continue;
        }
        session_->SetIncrementalData(bundleInfo);
    }
}

bool Service::CancelSessionClean(sptr<SvcSessionManager> session, std::string bundleName)
{
    if (session == nullptr) {
        HILOGE("Session is nullptr");
        return false;
    }
    auto connectionWptr = session->GetExtConnection(bundleName);
    if (connectionWptr == nullptr) {
        HILOGE("connectionWptr is null.");
        return false;
    }
    auto backUpConnection = connectionWptr.promote();
    if (backUpConnection == nullptr) {
        HILOGE("Promote backUpConnection ptr is null.");
        return false;
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("Extension backup Proxy is empty.");
        return false;
    }
    proxy->HandleClear();
    session->StopFwkTimer(bundleName);
    session->StopExtTimer(bundleName);
    session->HandleOnRelease(proxy);
    backUpConnection->DisconnectBackupExtAbility();
    return true;
}

void Service::CancelTask(std::string bundleName, wptr<Service> ptr)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto thisPtr = ptr.promote();
    if (!thisPtr) {
        HILOGE("this pointer is null");
        return;
    }
    auto session = thisPtr->session_;
    if (session == nullptr) {
        HILOGE("Session is nullptr");
        return;
    }
    HILOGI("Service CancelTask start, bundleName is %{public}s", bundleName.c_str());
    std::shared_ptr<ExtensionMutexInfo> mutexPtr = thisPtr->GetExtensionMutex(bundleName);
    if (mutexPtr == nullptr) {
        HILOGE("Extension mutex ptr is nullptr");
        return;
    }
    do {
        std::lock_guard<std::mutex> lock(mutexPtr->callbackMutex);
        if (!CancelSessionClean(session, bundleName)) {
            break;
        }
        thisPtr->ClearSessionAndSchedInfo(bundleName);
        IServiceReverseType::Scenario scenario = session->GetScenario();
        if ((scenario == IServiceReverseType::Scenario::BACKUP && session->GetIsIncrementalBackup()) ||
            (scenario == IServiceReverseType::Scenario::RESTORE &&
            session->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND))) {
            thisPtr->NotifyCallerCurAppIncrementDone(BError(BError::Codes::OK), bundleName);
        } else {
            thisPtr->NotifyCallerCurAppDone(BError(BError::Codes::OK), bundleName);
        }
    } while (0);
    thisPtr->RemoveExtensionMutex(bundleName);
    thisPtr->OnAllBundlesFinished(BError(BError::Codes::OK));
}

ErrCode Service::CancelForResult(const std::string& bundleName, int32_t &result)
{
    ErrCode errCode = Cancel(bundleName, result);
    if (errCode != 0) {
        HILOGE("Cancel failed, errCode:%{public}d.", errCode);
    }
    return BError(BError::Codes::OK);
}

ErrCode Service::Cancel(const std::string& bundleName, int32_t &result)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Begin, bundle name:%{public}s", bundleName.c_str());
    if (session_ == nullptr) {
        HILOGE("Cancel error, session is null");
        return BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK);
    }
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    ErrCode ret = VerifyCaller(scenario);
    if (ret != ERR_OK) {
        HILOGE("Verify caller failed, bundleName:%{public}s, scenario:%{public}d", bundleName.c_str(), scenario);
        return BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK);
    }
    BroadCastRestore(bundleName, BConstants::BROADCAST_RESTORE_END);
    auto impl = session_->GetImpl();
    auto it = impl.backupExtNameMap.find(bundleName);
    if (it == impl.backupExtNameMap.end()) {
        result = BError::BackupErrorCode::E_CANCEL_NO_TASK;
        return BError(BError::Codes::OK);
    }
    auto action = session_->GetServiceSchedAction(bundleName);
    if (action == BConstants::ServiceSchedAction::UNKNOWN) {
        HILOGE("action is unknown, bundleName:%{public}s", bundleName.c_str());
        result = BError::BackupErrorCode::E_CANCEL_NO_TASK;
        return BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK);
    }
    auto task = [this, bundleName]() {
        try {
            CancelTask(bundleName, wptr(this));
        } catch (const BError &e) {
            HILOGE("CancelTask failed, errCode = %{public}d", e.GetCode());
        } catch (...) {
            HILOGE("Unexpected exception");
        }
    };
    if (action == BConstants::ServiceSchedAction::RUNNING) {
        threadPool_.AddTask(task);
        result = BError(BError::Codes::OK);
        return BError(BError::Codes::OK);
    }
    if (action == BConstants::ServiceSchedAction::CLEAN) {
        result = BError::BackupErrorCode::E_CANCEL_NO_TASK;
    } else {
        result = BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK;
    }
    return BError(BError::Codes::OK);
}

void Service::ClearIncrementalStatFile(int32_t userId, const string &bundleName)
{
    string backupSaBundleDir = BConstants::GetSaBundleBackupRootDir(userId).
        append(BundleMgrAdapter::GetBundleIndexName(bundleName)).append("/");
    if (access(backupSaBundleDir.c_str(), F_OK) != ERR_OK) {
        HILOGD("ClearIncrementalStatFile, access dir failed errno = %{public}d", errno);
        return;
    }
    if (!ForceRemoveDirectoryBMS(backupSaBundleDir.c_str())) {
        HILOGE("Failed to delete SaBundleBackupDir cache: %{public}s",
            backupSaBundleDir.c_str());
        return;
    }
}

void Service::UpdateGcProgress(std::shared_ptr<GcProgressInfo> gcProgress,
    int status, int errcode, unsigned int percent, unsigned int gap)
{
    gcProgress->status.store(status, std::memory_order_relaxed);
    gcProgress->percent.store(percent, std::memory_order_relaxed);
    gcProgress->gap.store(gap, std::memory_order_relaxed);
    gcProgress->errcode.store(errcode, std::memory_order_release);
    HILOGI("Get GC progress, status %{public}d, errcode: %{public}d, progress: %{public}d, gap: %{public}d",
        status, errcode, percent, gap);
}

ErrCode Service::DealWithGcErrcode(int GcErrCode)
{
    if (GcErrCode == 0) {
        return ERROR_OK;
    } else if (GcErrCode == GC_DEVICE_INCOMPATIBLE) {
        return static_cast<ErrCode> (BError::BackupErrorCode::E_INCOMPATIBLE);
    } else if (GcErrCode == GC_TASK_TIMEOUT) {
        return static_cast<ErrCode> (BError::BackupErrorCode::E_MISSION_TIMEOUT);
    } else {
        return static_cast<ErrCode> (BError::BackupErrorCode::E_GC_FAILED);
    }
}

ErrCode Service::StartCleanData(int triggerType, unsigned int writeSize, unsigned int waitTime)
{
    std::string bundleName = GetCallerName();
    if (bundleName != BConstants::BUNDLE_DATA_CLONE) {
        HILOGE("Caller: %{public}s has no permission", bundleName.data());
        return static_cast<ErrCode> (BError::BackupErrorCode::E_PERM);
    }
    void *handle = dlopen("/system/lib64/libioqos_service_client.z.so", RTLD_LAZY);
    if (!handle) {
        HILOGE("Dlopen libioqos_service_client.z.so failed, errno = %{public}s", dlerror());
        return static_cast<ErrCode> (BError::BackupErrorCode::E_INVAL); 
    }
    CallDeviceTaskRequest func = reinterpret_cast<CallDeviceTaskRequest>(dlsym(handle, "CallDeviceTaskRequest"));
    if (func == nullptr) {
        HILOGE("CallDeviceTaskRequest dlsym failed, errno = %{public}s", dlerror());
        dlclose(handle);
        return static_cast<ErrCode>(BError::BackupErrorCode::E_INVAL);
    }
    std::shared_ptr<GcProgressInfo> gcProgress = std::make_shared<GcProgressInfo>();
    CallbackFunc cb = [&](int status, int errcode, unsigned int percent, unsigned int gap) {
        std::lock_guard<std::mutex> lock(gcMtx_);
        UpdateGcProgress(gcProgress, status, errcode, percent, gap);
        if (status == 0 || status == 1 || status == 8) {
            gcVariable_.notify_one();
        }
        return ERROR_OK;
    };
    int ret = func(triggerType, writeSize, waitTime, cb);
    if (ret != 0) {
        HILOGE("CallDeviceTaskRequest failed, errno = %{public}d" ,ret);
        dlclose(handle);
        return static_cast<ErrCode>(BError::BackupErrorCode::E_GC_FAILED);
    }
    std::unique_lock<std::mutex> lock(gcMtx_);
    auto timeout = gcVariable_.wait_for(lock, std::chrono::seconds(GC_MAX_WAIT_TIME_S));
    auto resCode = gcProgress->errcode.load(std::memory_order_acquire);
    HILOGI("GC task final progress, status %{public}d, errcode: %{public}d, progress: %{public}d, gap: %{public}d",
        gcProgress->status.load(std::memory_order_relaxed), resCode,
        gcProgress->percent.load(std::memory_order_relaxed), gcProgress->gap.load(std::memory_order_relaxed));
    if (timeout ==std::cv_status::timeout) {
        dlclose(handle);
        return static_cast<ErrCode>(BError::BackupErrorCode::E_MISSION_TIMEOUT);
    } else {
        dlclose(handle);
        return DealWithGcErrcode(resCode);
    }
}
} // namespace OHOS::FileManagement::Backup
