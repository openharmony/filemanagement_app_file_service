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
#include "parameter.h"
#include "system_ability_definition.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
const std::string FILE_BACKUP_EVENTS = "FILE_BACKUP_EVENTS";

namespace {
constexpr int32_t INDEX = 3;
constexpr int32_t MS_1000 = 1000;
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
    IServiceReverse::Scenario scenario = session_->GetScenario();
    VerifyCaller(scenario);
    AppRadar::Info info("", "", "call release");
    if (scenario == IServiceReverse::Scenario::RESTORE) {
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::Release", session_->GetSessionUserId(),
            BizStageRestore::BIZ_STAGE_RELEASE, ERR_OK);
    } else if (scenario == IServiceReverse::Scenario::BACKUP) {
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

UniqueFd Service::GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        /*
         Only called by restore app before InitBackupSession,
           so there must be set init userId.
        */
        HILOGI("Begin");
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("Get LocalCapabilities Incremental Error, session is empty or cleaning up the service");
            return UniqueFd(-ENOENT);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        ErrCode errCode = VerifyCaller();
        if (errCode != ERR_OK) {
            HILOGE("Get local abilities info failed, Verify caller failed, errCode:%{public}d", errCode);
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return UniqueFd(-ENOENT);
        }
        string path = BConstants::GetSaBundleBackupRootDir(GetUserIdDefault());
        BExcepUltils::VerifyPath(path, false);
        CreateDirIfNotExist(path);
        UniqueFd fd(open(path.data(), O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR));
        if (fd < 0) {
            HILOGE("GetLocalCapabilitiesIncremental: open file failed, err = %{public}d", errno);
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return UniqueFd(-ENOENT);
        }
        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(move(fd));
        auto cache = cachedEntity.Structuralize();
        std::string backupVersion = BJsonUtil::ParseBackupVersion();
        cache.SetBackupVersion(backupVersion);
        cache.SetSystemFullName(GetOSFullName());
        cache.SetDeviceType(GetDeviceType());
        auto bundleInfos = BundleMgrAdapter::GetBundleInfosForIncremental(GetUserIdDefault(), bundleNames);
        cache.SetBundleInfos(bundleInfos, true);
        cachedEntity.Persist();
        HILOGI("Service GetLocalCapabilitiesIncremental persist");
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("End, bundleInfos size:%{public}zu", bundleInfos.size());
        return move(cachedEntity.GetFd());
    } catch (const BError &e) {
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGE("GetLocalCapabilitiesIncremental failed, errCode = %{public}d", e.GetCode());
        return UniqueFd(-e.GetCode());
    } catch (...) {
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("Unexpected exception");
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
        auto ret = proxy->User0OnBackup();
        if (ret) {
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
        append(bundleName).append("/").append(BConstants::BACKUP_STAT_SYMBOL).append(to_string(lastTime));
    HILOGD("path = %{public}s,bundleName = %{public}s", path.c_str(), bundleName.c_str());
    UniqueFd fdLocal(open(path.data(), O_RDWR, S_IRGRP | S_IWGRP));
    if (fdLocal < 0) {
        HILOGD("fdLocal open fail, error = %{public}d", errno);
        throw BError(BError::Codes::SA_INVAL_ARG, "open local Manifest file failed");
    }
    UniqueFd lastManifestFd(session->GetIncrementalManifestFd(bundleName));
    auto ret = proxy->HandleIncrementalBackup(move(fdLocal), move(lastManifestFd));
    if (ret) {
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
    for (auto &info : newBundleInfos) {
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
            } catch (const exception &e) {
                HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
            } catch (...) {
                HILOGI("Unexpected exception");
            }
        });
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGE("GetAppLocalListAndDoIncrementalBackup failed, errCode = %{public}d", e.GetCode());
        return e.GetCode();
    } catch (...) {
        HILOGE("Unexpected exception");
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

ErrCode Service::InitIncrementalBackupSession(sptr<IServiceReverse> remote)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
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
                                .scenario = IServiceReverse::Scenario::BACKUP,
                                .clientProxy = remote,
                                .userId = GetUserIdDefault(),
                                .isIncrementalBackup = true,
                                .callerName = GetCallerName(),
                                .activeTime = TimeUtils::GetCurrentTime()});
    if (errCode != ERR_OK) {
        HILOGE("Active incremental backup session error, Already have a session");
        StopAll(nullptr, true);
        return errCode;
    }
    ClearFailedBundles();
    successBundlesNum_ = 0;
    return BError(BError::Codes::OK);
}

vector<string> Service::GetBundleNameByDetails(const std::vector<BIncrementalData> &bundlesToBackup)
{
    vector<string> bundleNames {};
    for (auto bundle : bundlesToBackup) {
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
        ErrCode ret = VerifyCaller(IServiceReverse::Scenario::BACKUP);
        if (ret != ERR_OK) {
            HILOGE("Append bundles incremental session failed, verify caller failed, ret:%{public}d", ret);
            HandleExceptionOnAppendBundles(session_, bundleNames, {});
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return ret;
        }
        auto backupInfos = BundleMgrAdapter::GetBundleInfosForAppend(bundlesToBackup,
            session_->GetSessionUserId());
        std::vector<std::string> supportBackupNames = GetSupportBackupBundleNames(backupInfos, true, bundleNames);
        session_->AppendBundles(supportBackupNames);
        SetBundleIncDataInfo(bundlesToBackup, supportBackupNames);
        SetCurrentBackupSessProperties(supportBackupNames, session_->GetSessionUserId(), backupInfos, true);
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGE("Failed, errCode = %{public}d", e.GetCode());
        return e.GetCode();
    } catch (...) {
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("Unexpected exception");
        return EPERM;
    }
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
        ErrCode ret = VerifyCaller(IServiceReverse::Scenario::BACKUP);
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
        auto backupInfos = BundleMgrAdapter::GetBundleInfosForAppend(bundlesToBackup,
            session_->GetSessionUserId());
        std::vector<std::string> supportBackupNames = GetSupportBackupBundleNames(backupInfos, true, bundleNames);
        session_->AppendBundles(supportBackupNames);
        SetBundleIncDataInfo(bundlesToBackup, supportBackupNames);
        HandleCurGroupIncBackupInfos(backupInfos, bundleNameDetailMap, isClearDataFlags);
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGE("Failed, errCode = %{public}d", e.GetCode());
        return e.GetCode();
    } catch (...) {
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("Unexpected exception");
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
    ErrCode ret = VerifyCaller(IServiceReverse::Scenario::RESTORE);
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

ErrCode Service::PublishSAIncrementalFile(const BFileInfo &fileInfo, UniqueFd fd)
{
    std::string bundleName = fileInfo.owner;
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
        if (session_->GetScenario() == IServiceReverse::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnFileReady(bundleName, fileName, move(fd),
                                                                              move(manifestFd), errCode);
            FileReadyRadarReport(bundleName, fileName, errCode, IServiceReverse::Scenario::RESTORE);
            return BError(BError::Codes::OK);
        }
        if (fileName == BConstants::EXT_BACKUP_MANAGE) {
            fd = session_->OnBundleExtManageInfo(bundleName, move(fd));
        }
        session_->GetServiceReverseProxy()->IncrementalBackupOnFileReady(bundleName, fileName, move(fd),
            move(manifestFd), errCode);
        FileReadyRadarReport(bundleName, fileName, errCode, IServiceReverse::Scenario::BACKUP);
        if (session_->OnBundleFileReady(bundleName, fileName)) {
            auto backUpConnection = session_->GetExtConnection(bundleName);
            if (backUpConnection == nullptr) {
                return BError(BError::Codes::SA_INVAL_ARG, "backup connection is empty");
            }
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                return BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            // 通知extension清空缓存
            proxy->HandleClear();
            // 清除Timer
            session_->StopFwkTimer(bundleName);
            session_->StopExtTimer(bundleName);
            // 通知TOOL 备份完成
            HILOGI("reverse: Will notify IncrementalBackupOnBundleFinished");
            session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK),
                                                                                  bundleName);
            BundleEndRadarReport(bundleName, BError(BError::Codes::OK), IServiceReverse::Scenario::BACKUP);
            // 断开extension
            backUpConnection->DisconnectBackupExtAbility();
            ClearSessionAndSchedInfo(bundleName);
        }
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (...) {
        HILOGI("Unexpected exception");
        return EPERM;
    }
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
        if (session_->GetScenario() == IServiceReverse::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnFileReady(callerName, fileName, move(fd),
                                                                              move(manifestFd), errCode);
            FileReadyRadarReport(callerName, fileName, errCode, IServiceReverse::Scenario::RESTORE);
            return BError(BError::Codes::OK);
        }
        if (fileName == BConstants::EXT_BACKUP_MANAGE) {
            fd = session_->OnBundleExtManageInfo(callerName, move(fd));
        }
        session_->GetServiceReverseProxy()->IncrementalBackupOnFileReady(callerName, fileName, move(fd),
            move(manifestFd), errCode);
        FileReadyRadarReport(callerName, fileName, errCode, IServiceReverse::Scenario::BACKUP);
        if (session_->OnBundleFileReady(callerName, fileName)) {
            auto backUpConnection = session_->GetExtConnection(callerName);
            if (backUpConnection == nullptr) {
                return BError(BError::Codes::SA_INVAL_ARG, "Extension backup connection is empty");
            }
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            // 通知extension清空缓存
            proxy->HandleClear();
            // 清除Timer
            session_->StopFwkTimer(callerName);
            session_->StopExtTimer(callerName);
            // 通知TOOL 备份完成
            HILOGI("reverse: Will notify IncrementalBackupOnBundleFinished");
            session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK),
                                                                                  callerName);
            BundleEndRadarReport(callerName, BError(BError::Codes::OK), IServiceReverse::Scenario::BACKUP);
            // 断开extension
            backUpConnection->DisconnectBackupExtAbility();
            ClearSessionAndSchedInfo(callerName);
        }
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode(); // 任意异常产生，终止监听该任务
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
        HILOGI("Begin, callerName is %{public}s, errCode is: %{public}d", callerName.c_str(), errCode);
        if (session_->OnBundleFileReady(callerName) || errCode != BError(BError::Codes::OK)) {
            std::shared_ptr<ExtensionMutexInfo> mutexPtr = GetExtensionMutex(callerName);
            if (mutexPtr == nullptr) {
                return BError(BError::Codes::SA_INVAL_ARG, "Extension mutex ptr is null.");
            }
            std::lock_guard<std::mutex> lock(mutexPtr->callbackMutex);
            auto backUpConnection = session_->GetExtConnection(callerName);
            if (backUpConnection == nullptr) {
                return BError(BError::Codes::SA_INVAL_ARG, "Promote backUpConnection ptr is null.");
            }
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                return BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            proxy->HandleClear();
            session_->StopFwkTimer(callerName);
            session_->StopExtTimer(callerName);
            backUpConnection->DisconnectBackupExtAbility();
            ClearSessionAndSchedInfo(callerName);
            NotifyCallerCurAppIncrementDone(errCode, callerName);
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
        if (session_ == nullptr) {
            HILOGE("GetIncrementalFileHandle error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        ErrCode ret = VerifyCaller(IServiceReverse::Scenario::RESTORE);
        if (ret != ERR_OK) {
            HILOGE("Error, bundleName:%{public}s, fileName:%{public}s", bundleName.c_str(),
                GetAnonyPath(fileName).c_str());
            return ret;
        }
        auto action = session_->GetServiceSchedAction(bundleName);
        if (action == BConstants::ServiceSchedAction::RUNNING) {
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
            auto[errCode, fd, reportFd] = proxy->GetIncrementalFileHandle(fileName);
            auto err = AppIncrementalFileReady(bundleName, fileName, move(fd), move(reportFd), errCode);
            if (err != ERR_OK) {
                HILOGE("Failed to send file handle");
                AppRadar::Info info (bundleName, "", "");
                AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::GetIncrementalFileHandle",
                    GetUserIdDefault(), BizStageRestore::BIZ_STAGE_GET_FILE_HANDLE_FAIL, err);
            }
        } else {
            SvcRestoreDepsManager::GetInstance().UpdateToRestoreBundleMap(bundleName, fileName);
            session_->SetExtFileNameRequest(bundleName, fileName);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

bool Service::IncrementalBackup(const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    IServiceReverse::Scenario scenario = session_->GetScenario();
    auto backUpConnection = session_->GetExtConnection(bundleName);
    if (backUpConnection == nullptr) {
        HILOGE("backUpConnection is empty, bundle:%{public}s", bundleName.c_str());
        NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_DIED));
        return true;
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("Increment backup error, extension proxy is empty, bundleName:%{public}s", bundleName.c_str());
        NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_DIED));
        return true;
    }
    if (scenario == IServiceReverse::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
        auto ret = proxy->IncrementalOnBackup(session_->GetClearDataFlag(bundleName));
        session_->GetServiceReverseProxy()->IncrementalBackupOnBundleStarted(ret, bundleName);
        BundleBeginRadarReport(bundleName, ret, IServiceReverse::Scenario::BACKUP);
        if (ret) {
            ClearSessionAndSchedInfo(bundleName);
            NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_DIED));
        }
        return true;
    } else if (scenario == IServiceReverse::Scenario::RESTORE && BackupPara().GetBackupOverrideIncrementalRestore() &&
               session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
        auto ret = proxy->HandleRestore(session_->GetClearDataFlag(bundleName));
        session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(ret, bundleName);
        std::string oldBackupVersion = session_->GetOldBackupVersion();
        if (oldBackupVersion.empty()) {
            HILOGE("Failed to get backupVersion of old device");
        }
        HILOGD("backupVersion of old device = %{public}s", oldBackupVersion.c_str());
        BundleBeginRadarReport(bundleName, ret, IServiceReverse::Scenario::RESTORE);
        auto fileNameVec = session_->GetExtFileNameRequest(bundleName);
        for (auto &fileName : fileNameVec) {
            auto[errCode, fd, reportFd] = proxy->GetIncrementalFileHandle(fileName);
            ret = AppIncrementalFileReady(bundleName, fileName, move(fd), move(reportFd), errCode);
            if (ret) {
                HILOGE("Failed to send file handle %{public}s", GetAnonyString(fileName).c_str());
            }
        }
        return true;
    }
    return false;
}

void Service::NotifyCallerCurAppIncrementDone(ErrCode errCode, const std::string &callerName)
{
    IServiceReverse::Scenario scenario = session_->GetScenario();
    if (scenario == IServiceReverse::Scenario::BACKUP) {
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
    } else if (scenario == IServiceReverse::Scenario::RESTORE) {
        HILOGI("will notify clone data, scenario is Restore");
        SendEndAppGalleryNotify(callerName);
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
    for (auto &bundleInfo : backupBundleInfos) {
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(bundleInfo.name, bundleInfo.appIndex);
        bundleNameIndexBundleInfoMap[bundleNameIndexInfo] = bundleInfo;
    }
    for (auto item : bundleNames) {
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
        session_->SetBackupExtName(bundleName, bundleInfo.extensionName);
        session_->SetIsReadyLaunch(bundleName);
    }
    HILOGI("end SetCurrentBackupSessProperties");
}


void Service::SetBundleIncDataInfo(const std::vector<BIncrementalData>& bundlesToBackup,
    std::vector<std::string>& supportBundleNames)
{
    for (auto &bundleInfo : bundlesToBackup) {
        std::string bundleName = bundleInfo.bundleName;
        auto it = std::find(supportBundleNames.begin(), supportBundleNames.end(), bundleName);
        if (it == supportBundleNames.end()) {
            HILOGE("Current bundle is not support to backup, bundleName:%{public}s", bundleName.c_str());
            continue;
        }
        session_->SetIncrementalData(bundleInfo);
    }
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
        auto backUpConnection = session->GetExtConnection(bundleName);
        if (backUpConnection == nullptr) {
            HILOGE("Promote backUpConnection ptr is null.");
            break;
        }
        auto proxy = backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            HILOGE("Extension backup Proxy is empty.");
            break;
        }
        proxy->HandleClear();
        session->StopFwkTimer(bundleName);
        session->StopExtTimer(bundleName);
        backUpConnection->DisconnectBackupExtAbility();
        thisPtr->ClearSessionAndSchedInfo(bundleName);
        IServiceReverse::Scenario scenario = session->GetScenario();
        if ((scenario == IServiceReverse::Scenario::BACKUP && session->GetIsIncrementalBackup()) ||
            (scenario == IServiceReverse::Scenario::RESTORE &&
            session->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND))) {
            thisPtr->NotifyCallerCurAppIncrementDone(BError(BError::Codes::OK), bundleName);
        } else {
            thisPtr->NotifyCallerCurAppDone(BError(BError::Codes::OK), bundleName);
        }
    } while (0);
    thisPtr->RemoveExtensionMutex(bundleName);
    thisPtr->OnAllBundlesFinished(BError(BError::Codes::OK));
}

ErrCode Service::Cancel(std::string bundleName, int32_t &result)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Begin, bundle name:%{public}s", bundleName.c_str());
    if (session_ == nullptr) {
        HILOGE("Cancel error, session is null");
        return BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK);
    }
    IServiceReverse::Scenario scenario = session_->GetScenario();
    ErrCode ret = VerifyCaller(scenario);
    if (ret != ERR_OK) {
        HILOGE("Verify caller failed, bundleName:%{public}s, scenario:%{public}d", bundleName.c_str(), scenario);
        return BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK);
    }
    auto impl = session_->GetImpl();
    auto it = impl.backupExtNameMap.find(bundleName);
    if (it == impl.backupExtNameMap.end()) {
        result = BError::BackupErrorCode::E_CANCEL_NO_TASK;
        return BError(BError::Codes::OK);
    }
    auto action = session_->GetServiceSchedAction(bundleName);
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
} // namespace OHOS::FileManagement::Backup
