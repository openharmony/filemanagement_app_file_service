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
#include <cstddef>
#include <cstdint>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>

#include <directory_ex.h>
#include <unique_fd.h>

#include "accesstoken_kit.h"
#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_caps.h"
#include "b_ohos/startup/backup_para.h"
#include "b_process/b_multiuser.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "ipc_skeleton.h"
#include "module_external/bms_adapter.h"
#include "module_ipc/svc_backup_connection.h"
#include "module_ipc/svc_restore_deps_manager.h"
#include "parameter.h"
#include "system_ability_definition.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
constexpr int32_t DEBUG_ID = 100;
} // namespace

static inline int32_t GetUserIdDefault()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto [isDebug, debugId] = BackupPara().GetBackupDebugOverrideAccount();
    if (isDebug && debugId > DEBUG_ID) {
        return debugId;
    }
    auto multiuser = BMultiuser::ParseUid(IPCSkeleton::GetCallingUid());
    if ((multiuser.userId == BConstants::SYSTEM_UID) || (multiuser.userId == BConstants::XTS_UID)) {
        return BConstants::DEFAULT_USER_ID;
    }
    return multiuser.userId;
}

ErrCode Service::Release()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("KILL");
    VerifyCaller(session_->GetScenario());
    SessionDeactive();
    return BError(BError::Codes::OK);
}

UniqueFd Service::GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        /*
         Only called by restore app before InitBackupSession,
           so there must be set init userId.
        */
        session_->IncreaseSessionCnt();
        session_->SetSessionUserId(GetUserIdDefault());
        VerifyCaller();
        string path = BConstants::GetSaBundleBackupRootDir(session_->GetSessionUserId());
        BExcepUltils::VerifyPath(path, false);
        UniqueFd fd(open(path.data(), O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR));
        if (fd < 0) {
            HILOGE("GetLocalCapabilitiesIncremental: open file failed, err = %{public}d", errno);
            session_->DecreaseSessionCnt();
            return UniqueFd(-ENOENT);
        }
        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(move(fd));
        auto cache = cachedEntity.Structuralize();

        cache.SetSystemFullName(GetOSFullName());
        cache.SetDeviceType(GetDeviceType());
        auto bundleInfos = BundleMgrAdapter::GetBundleInfosForIncremental(bundleNames, session_->GetSessionUserId());
        cache.SetBundleInfos(bundleInfos);
        cachedEntity.Persist();
        HILOGI("Service GetLocalCapabilitiesIncremental persist");
        session_->DecreaseSessionCnt();
        return move(cachedEntity.GetFd());
    } catch (const BError &e) {
        session_->DecreaseSessionCnt();
        HILOGE("GetLocalCapabilitiesIncremental failed, errCode = %{public}d", e.GetCode());
        return UniqueFd(-e.GetCode());
    } catch (const exception &e) {
        session_->DecreaseSessionCnt();
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return UniqueFd(-EPERM);
    } catch (...) {
        session_->DecreaseSessionCnt();
        HILOGI("Unexpected exception");
        return UniqueFd(-EPERM);
    }
}

ErrCode Service::InitIncrementalBackupSession(sptr<IServiceReverse> remote)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Begin");
    try {
        VerifyCaller();
        session_->Active({.clientToken = IPCSkeleton::GetCallingTokenID(),
                          .scenario = IServiceReverse::Scenario::BACKUP,
                          .clientProxy = remote,
                          .userId = GetUserIdDefault(),
                          .isIncrementalBackup = true});
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        StopAll(nullptr, true);
        return e.GetCode();
    }
}

ErrCode Service::AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        session_->IncreaseSessionCnt(); // BundleMgrAdapter::GetBundleInfos可能耗时
        VerifyCaller(IServiceReverse::Scenario::BACKUP);
        vector<string> bundleNames {};
        for (auto &bundle : bundlesToBackup) {
            bundleNames.emplace_back(bundle.bundleName);
        }
        auto backupInfos = BundleMgrAdapter::GetBundleInfos(bundleNames, session_->GetSessionUserId());
        session_->AppendBundles(bundleNames);
        for (auto info : backupInfos) {
            session_->SetBundleDataSize(info.name, info.spaceOccupied);
            session_->SetBackupExtName(info.name, info.extensionName);
            if (info.allToBackup == false) {
                session_->GetServiceReverseProxy()->IncrementalBackupOnBundleStarted(
                    BError(BError::Codes::SA_FORBID_BACKUP_RESTORE), info.name);
                session_->RemoveExtInfo(info.name);
            }
        }
        for (auto &bundleInfo : bundlesToBackup) {
            session_->SetIncrementalData(bundleInfo);
        }
        OnStartSched();
        session_->DecreaseSessionCnt();
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        session_->DecreaseSessionCnt();
        HILOGE("Failed, errCode = %{public}d", e.GetCode());
        return e.GetCode();
    } catch (...) {
        session_->DecreaseSessionCnt();
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::PublishIncrementalFile(const BFileInfo &fileInfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        VerifyCaller(IServiceReverse::Scenario::RESTORE);

        auto backUpConnection = session_->GetExtConnection(fileInfo.owner);
        auto proxy = backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
        }
        ErrCode res = proxy->PublishIncrementalFile(fileInfo.fileName);
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

ErrCode Service::AppIncrementalFileReady(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        string callerName = VerifyCallerAndGetCallerName();
        if (session_->GetScenario() == IServiceReverse::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnFileReady(callerName, fileName, move(fd),
                                                                              move(manifestFd));
            return BError(BError::Codes::OK);
        }

        if (fileName == BConstants::EXT_BACKUP_MANAGE) {
            fd = session_->OnBunleExtManageInfo(callerName, move(fd));
        }

        session_->GetServiceReverseProxy()->IncrementalBackupOnFileReady(callerName, fileName, move(fd),
                                                                         move(manifestFd));
        if (session_->OnBunleFileReady(callerName, fileName)) {
            auto backUpConnection = session_->GetExtConnection(callerName);
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            // 通知extension清空缓存
            proxy->HandleClear();
            // 清除Timer
            session_->BundleExtTimerStop(callerName);
            // 通知TOOL 备份完成
            session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK),
                                                                                  callerName);
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

ErrCode Service::AppIncrementalDone(ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        string callerName = VerifyCallerAndGetCallerName();
        if (session_->OnBunleFileReady(callerName)) {
            auto backUpConnection = session_->GetExtConnection(callerName);
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            proxy->HandleClear();
            session_->BundleExtTimerStop(callerName);
            IServiceReverse::Scenario scenario = session_->GetScenario();
            if (scenario == IServiceReverse::Scenario::BACKUP) {
                session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(errCode, callerName);
            } else if (scenario == IServiceReverse::Scenario::RESTORE) {
                session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleFinished(errCode, callerName);
            }
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

ErrCode Service::GetIncrementalFileHandle(const std::string &bundleName, const std::string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        VerifyCaller(IServiceReverse::Scenario::RESTORE);
        auto action = session_->GetServiceSchedAction(bundleName);
        if (action == BConstants::ServiceSchedAction::RUNNING) {
            auto backUpConnection = session_->GetExtConnection(bundleName);
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            int res = proxy->GetIncrementalFileHandle(fileName);
            if (res) {
                HILOGE("Failed to extension file handle");
            }
        } else {
            SvcRestoreDepsManager::GetInstance().UpdateToRestoreBundleMap(bundleName, fileName);
            session_->SetExtFileNameRequest(bundleName, fileName);
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

bool Service::IncrementalBackup(const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    IServiceReverse::Scenario scenario = session_->GetScenario();
    auto backUpConnection = session_->GetExtConnection(bundleName);
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
    }
    if (scenario == IServiceReverse::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
        //  本地全量数据
        string path = BConstants::GetSaBundleBackupRootDir(session_->GetSessionUserId()).
                      append(bundleName).
                      append("/").
                      append(BConstants::BACKUP_STAT_SYMBOL).
                      append(to_string(session_->GetLastIncrementalTime(bundleName)));
        HILOGI("path = %{public}s", path.c_str());
        UniqueFd fdLocal(open(path.data(), O_RDWR, S_IRGRP | S_IWGRP));
        // BFile::SendFile(incrementalFd, fdLocal);
        //  云上存在的数据
        UniqueFd lastManifestFd(session_->GetIncrementalManifestFd(bundleName));
        auto ret = proxy->HandleIncrementalBackup(move(fdLocal), move(lastManifestFd));
        session_->GetServiceReverseProxy()->IncrementalBackupOnBundleStarted(ret, bundleName);
        if (ret) {
            ClearSessionAndSchedInfo(bundleName);
            NoticeClientFinish(callName, BError(BError::Codes::EXT_ABILITY_DIED));
        }
        return true;
    } else if (scenario == IServiceReverse::Scenario::RESTORE && BackupPara().GetBackupOverrideIncrementalRestore() &&
               session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
        auto ret = proxy->HandleRestore();
        session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(ret, bundleName);
        auto fileNameVec = session_->GetExtFileNameRequest(bundleName);
        for (auto &fileName : fileNameVec) {
            ret = proxy->GetIncrementalFileHandle(fileName);
            if (ret) {
                HILOGE("Failed to extension file handle %{public}s", fileName.c_str());
            }
        }
        return true;
    }
    return false;
}
} // namespace OHOS::FileManagement::Backup
