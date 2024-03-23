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

/*
 * 注意：
 *     - 注意点1：本文件原则上只处理与IPC无关的业务逻辑
 *     - 注意点2：This document, in principle, captures all exceptions.
 *               Prevent exceptions from spreading to insecure modules.
 */
#include "module_ipc/service.h"

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <regex>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>

#include <directory_ex.h>

#include "ability_manager_client.h"
#include "accesstoken_kit.h"
#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_file_info.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_caps.h"
#include "b_ohos/startup/backup_para.h"
#include "b_process/b_multiuser.h"
#include "b_resources/b_constants.h"
#include "bundle_mgr_client.h"
#include "filemgmt_libhilog.h"
#include "ipc_skeleton.h"
#include "module_app_gallery/app_gallery_dispose_proxy.h"
#include "module_external/bms_adapter.h"
#include "module_external/sms_adapter.h"
#include "module_ipc/svc_backup_connection.h"
#include "module_ipc/svc_restore_deps_manager.h"
#include "parameter.h"
#include "system_ability_definition.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

REGISTER_SYSTEM_ABILITY_BY_ID(Service, FILEMANAGEMENT_BACKUP_SERVICE_SA_ID, false);

namespace {
constexpr int32_t DEBUG_ID = 100;
const int32_t CONNECT_WAIT_TIME = 5;
} // namespace

/* Shell/Xts user id equal to 0/1, we need set default 100 */
static inline int32_t GetUserIdDefault()
{
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

void Service::OnStart()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    bool res = SystemAbility::Publish(sptr(this));
    sched_ = sptr(new SchedScheduler(wptr(this), wptr(session_)));
    sched_->StartTimer();
    HILOGI("End, res = %{public}d", res);
}

void Service::OnStop()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Called");
    int32_t oldMemoryParaSize = BConstants::DEFAULT_VFS_CACHE_PRESSURE;
    if (session_ != nullptr) {
        oldMemoryParaSize = session_->GetMemParaCurSize();
    }
    StorageMgrAdapter::UpdateMemPara(oldMemoryParaSize);
}

UniqueFd Service::GetLocalCapabilities()
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
        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(
            UniqueFd(open(path.data(), O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR)));

        auto cache = cachedEntity.Structuralize();

        cache.SetSystemFullName(GetOSFullName());
        cache.SetDeviceType(GetDeviceType());
        auto bundleInfos = BundleMgrAdapter::GetBundleInfosForIncremental(session_->GetSessionUserId());
        cache.SetBundleInfos(bundleInfos);
        cachedEntity.Persist();
        session_->DecreaseSessionCnt();
        return move(cachedEntity.GetFd());
    } catch (const BError &e) {
        session_->DecreaseSessionCnt();
        HILOGE("GetLocalCapabilities failed, errCode = %{public}d", e.GetCode());
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

void Service::StopAll(const wptr<IRemoteObject> &obj, bool force)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    session_->Deactive(obj, force);
}

string Service::VerifyCallerAndGetCallerName()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        Security::AccessToken::HapTokenInfo hapTokenInfo;
        if (Security::AccessToken::AccessTokenKit::GetHapTokenInfo(tokenCaller, hapTokenInfo) != 0) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Get hap token info failed");
        }
        session_->VerifyBundleName(hapTokenInfo.bundleName);
        return hapTokenInfo.bundleName;
    } else {
        HILOGE("tokenID = %{private}d", tokenCaller);
        throw BError(BError::Codes::SA_INVAL_ARG, string("Invalid token type ").append(to_string(tokenType)));
    }
}

void Service::VerifyCaller()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
    switch (tokenType) {
        case Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE: /* Update Service */
        case Security::AccessToken::ATokenTypeEnum::TOKEN_HAP: {
            const string permission = "ohos.permission.BACKUP";
            if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission) ==
                Security::AccessToken::TypePermissionState::PERMISSION_DENIED) {
                throw BError(BError::Codes::SA_INVAL_ARG,
                             string("Permission denied, token type is ").append(to_string(tokenType)));
            }
            break;
        }
        case Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL:
            if (IPCSkeleton::GetCallingUid() != BConstants::SYSTEM_UID) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Calling uid is invalid");
            }
            break;
        default:
            throw BError(BError::Codes::SA_INVAL_ARG, string("Invalid token type ").append(to_string(tokenType)));
            break;
    }
}

void Service::VerifyCaller(IServiceReverse::Scenario scenario)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    session_->VerifyCallerAndScenario(IPCSkeleton::GetCallingTokenID(), scenario);
    VerifyCaller();
}

ErrCode Service::InitRestoreSession(sptr<IServiceReverse> remote)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        VerifyCaller();
        session_->Active({
            .clientToken = IPCSkeleton::GetCallingTokenID(),
            .scenario = IServiceReverse::Scenario::RESTORE,
            .clientProxy = remote,
            .userId = GetUserIdDefault(),
        });
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        StopAll(nullptr, true);
        return e.GetCode();
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::InitBackupSession(sptr<IServiceReverse> remote)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        VerifyCaller();
        int32_t oldSize = StorageMgrAdapter::UpdateMemPara(BConstants::BACKUP_VFS_CACHE_PRESSURE);
        HILOGE("InitBackupSession oldSize %{public}d", oldSize);
        session_->SetMemParaCurSize(oldSize);
        session_->Active({
            .clientToken = IPCSkeleton::GetCallingTokenID(),
            .scenario = IServiceReverse::Scenario::BACKUP,
            .clientProxy = remote,
            .userId = GetUserIdDefault(),
        });
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        StopAll(nullptr, true);
        return e.GetCode();
    }
}

ErrCode Service::Start()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Begin");
    VerifyCaller(session_->GetScenario());
    session_->Start();
    OnStartSched();
    return BError(BError::Codes::OK);
}

static bool SpeicalVersion(const string &versionName, uint32_t versionCode)
{
    auto iter = find_if(BConstants::DEFAULT_VERSION_NAMES_VEC.begin(), BConstants::DEFAULT_VERSION_NAMES_VEC.end(),
                        [&versionName](const auto &version) { return version == versionName; });
    if (versionCode == BConstants::DEFAULT_VERSION_CODE && iter != BConstants::DEFAULT_VERSION_NAMES_VEC.end()) {
        return true;
    }
    return false;
}

static void OnBundleStarted(BError error, sptr<SvcSessionManager> session, const BundleName &bundleName)
{
    IServiceReverse::Scenario scenario = session->GetScenario();
    if (scenario == IServiceReverse::Scenario::RESTORE && BackupPara().GetBackupOverrideIncrementalRestore() &&
        session->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
        session->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(error, bundleName);
    } else if (scenario == IServiceReverse::Scenario::RESTORE) {
        session->GetServiceReverseProxy()->RestoreOnBundleStarted(error, bundleName);
    }
}

static vector<BJsonEntityCaps::BundleInfo> GetRestoreBundleNames(UniqueFd fd,
                                                                 sptr<SvcSessionManager> session,
                                                                 const vector<BundleName> &bundleNames)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    // BundleMgrAdapter::GetBundleInfos可能耗时
    auto restoreInfos = BundleMgrAdapter::GetBundleInfos(bundleNames, session->GetSessionUserId());
    BJsonCachedEntity<BJsonEntityCaps> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto bundleInfos = cache.GetBundleInfos();
    if (!bundleInfos.size()) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Json entity caps is empty");
    }
    vector<BJsonEntityCaps::BundleInfo> restoreBundleInfos {};
    for (auto &restoreInfo : restoreInfos) {
        auto it = find_if(bundleInfos.begin(), bundleInfos.end(),
                          [&restoreInfo](const auto &obj) { return obj.name == restoreInfo.name; });
        if (it == bundleInfos.end()) {
            OnBundleStarted(BError(BError::Codes::SA_BUNDLE_INFO_EMPTY), session, restoreInfo.name);
            continue;
        }
        BJsonEntityCaps::BundleInfo info = {.name = (*it).name,
                                            .versionCode = (*it).versionCode,
                                            .versionName = (*it).versionName,
                                            .spaceOccupied = (*it).spaceOccupied,
                                            .allToBackup = (*it).allToBackup,
                                            .extensionName = restoreInfo.extensionName,
                                            .restoreDeps = restoreInfo.restoreDeps};
        restoreBundleInfos.emplace_back(info);
    }
    return restoreBundleInfos;
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd,
                                             const vector<BundleName> &bundleNames,
                                             RestoreTypeEnum restoreType,
                                             int32_t userId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        session_->IncreaseSessionCnt();
        if (userId != DEFAULT_INVAL_VALUE) { /* multi user scenario */
            session_->SetSessionUserId(userId);
        }
        VerifyCaller(IServiceReverse::Scenario::RESTORE);
        auto restoreInfos = GetRestoreBundleNames(move(fd), session_, bundleNames);
        auto restoreBundleNames = SvcRestoreDepsManager::GetInstance().GetRestoreBundleNames(restoreInfos, restoreType);
        if (restoreBundleNames.empty()) {
            session_->DecreaseSessionCnt();
            return BError(BError::Codes::OK);
        }
        session_->AppendBundles(restoreBundleNames);
        for (auto restoreInfo : restoreInfos) {
            auto it = find_if(restoreBundleNames.begin(), restoreBundleNames.end(),
                              [&restoreInfo](const auto &bundleName) { return bundleName == restoreInfo.name; });
            if (it == restoreBundleNames.end()) {
                throw BError(BError::Codes::SA_BUNDLE_INFO_EMPTY, "Can't find bundle name");
            }
            HILOGD("bundleName: %{public}s, extensionName: %{public}s", restoreInfo.name.c_str(),
                   restoreInfo.extensionName.c_str());
            if ((restoreInfo.allToBackup == false &&
                 !SpeicalVersion(restoreInfo.versionName, restoreInfo.versionCode)) ||
                restoreInfo.extensionName.empty()) {
                OnBundleStarted(BError(BError::Codes::SA_FORBID_BACKUP_RESTORE), session_, restoreInfo.name);
                session_->RemoveExtInfo(restoreInfo.name);
                continue;
            }
            session_->SetBundleRestoreType(restoreInfo.name, restoreType);
            session_->SetBundleVersionCode(restoreInfo.name, restoreInfo.versionCode);
            session_->SetBundleVersionName(restoreInfo.name, restoreInfo.versionName);
            session_->SetBundleDataSize(restoreInfo.name, restoreInfo.spaceOccupied);
            session_->SetBackupExtName(restoreInfo.name, restoreInfo.extensionName);
        }
        OnStartSched();
        session_->DecreaseSessionCnt();
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        session_->DecreaseSessionCnt();
        return e.GetCode();
    } catch (...) {
        session_->DecreaseSessionCnt();
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::AppendBundlesBackupSession(const vector<BundleName> &bundleNames)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        session_->IncreaseSessionCnt(); // BundleMgrAdapter::GetBundleInfos可能耗时
        VerifyCaller(IServiceReverse::Scenario::BACKUP);
        auto backupInfos = BundleMgrAdapter::GetBundleInfos(bundleNames, session_->GetSessionUserId());
        session_->AppendBundles(bundleNames);
        for (auto info : backupInfos) {
            session_->SetBundleDataSize(info.name, info.spaceOccupied);
            session_->SetBackupExtName(info.name, info.extensionName);
            if (info.allToBackup == false) {
                session_->GetServiceReverseProxy()->BackupOnBundleStarted(
                    BError(BError::Codes::SA_FORBID_BACKUP_RESTORE), info.name);
                session_->RemoveExtInfo(info.name);
            }
        }
        OnStartSched();
        session_->DecreaseSessionCnt();
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        session_->DecreaseSessionCnt();
        HILOGE("Failed, errCode = %{public}d", e.GetCode());
        return e.GetCode();
    } catch (const exception &e) {
        session_->DecreaseSessionCnt();
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        session_->DecreaseSessionCnt();
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::Finish()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Begin");
    VerifyCaller(session_->GetScenario());
    session_->Finish();
    OnAllBundlesFinished(BError(BError::Codes::OK));
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGD("Begin");
        VerifyCaller(IServiceReverse::Scenario::RESTORE);

        auto backUpConnection = session_->GetExtConnection(fileInfo.owner);

        auto proxy = backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
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

ErrCode Service::AppFileReady(const string &fileName, UniqueFd fd)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        string callerName = VerifyCallerAndGetCallerName();
        if (fileName.find('/') != string::npos) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not valid");
        }
        if (fileName == BConstants::EXT_BACKUP_MANAGE) {
            fd = session_->OnBunleExtManageInfo(callerName, move(fd));
        }

        session_->GetServiceReverseProxy()->BackupOnFileReady(callerName, fileName, move(fd));

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
            session_->GetServiceReverseProxy()->BackupOnBundleFinished(BError(BError::Codes::OK), callerName);
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
                session_->GetServiceReverseProxy()->BackupOnBundleFinished(errCode, callerName);
            } else if (scenario == IServiceReverse::Scenario::RESTORE) {
                session_->GetServiceReverseProxy()->RestoreOnBundleFinished(errCode, callerName);
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

ErrCode Service::LaunchBackupExtension(const BundleName &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGE("begin %{public}s", bundleName.data());
        IServiceReverse::Scenario scenario = session_->GetScenario();
        BConstants::ExtensionAction action;
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            action = BConstants::ExtensionAction::BACKUP;
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            action = BConstants::ExtensionAction::RESTORE;
        } else {
            throw BError(BError::Codes::SA_INVAL_ARG, "Failed to scenario");
        }

        AAFwk::Want want;
        string backupExtName = session_->GetBackupExtName(bundleName); /* new device app ext name */
        HILOGD("backupExtName: %{public}s, bundleName: %{public}s", backupExtName.data(), bundleName.data());
        string versionName = session_->GetBundleVersionName(bundleName);          /* old device app version name */
        uint32_t versionCode = session_->GetBundleVersionCode(bundleName);        /* old device app version code */
        RestoreTypeEnum restoreType = session_->GetBundleRestoreType(bundleName); /* app restore type */

        want.SetElementName(bundleName, backupExtName);
        want.SetParam(BConstants::EXTENSION_ACTION_PARA, static_cast<int>(action));
        want.SetParam(BConstants::EXTENSION_VERSION_CODE_PARA, static_cast<int>(versionCode));
        want.SetParam(BConstants::EXTENSION_RESTORE_TYPE_PARA, static_cast<int>(restoreType));
        want.SetParam(BConstants::EXTENSION_VERSION_NAME_PARA, versionName);

        auto backUpConnection = session_->GetExtConnection(bundleName);

        ErrCode ret = backUpConnection->ConnectBackupExtAbility(want, session_->GetSessionUserId());
        return ret;
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

ErrCode Service::GetFileHandle(const string &bundleName, const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGD("Begin");
        VerifyCaller(IServiceReverse::Scenario::RESTORE);

        bool updateRes = SvcRestoreDepsManager::GetInstance().UpdateToRestoreBundleMap(bundleName, fileName);
        if (updateRes) {
            return BError(BError::Codes::OK);
        }
        auto action = session_->GetServiceSchedAction(bundleName);
        if (action == BConstants::ServiceSchedAction::RUNNING) {
            auto backUpConnection = session_->GetExtConnection(bundleName);
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            UniqueFd fd = proxy->GetFileHandle(fileName);
            if (fd < 0) {
                HILOGE("Failed to extension file handle");
            }
            session_->GetServiceReverseProxy()->RestoreOnFileReady(bundleName, fileName, move(fd));
        } else {
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

void Service::OnBackupExtensionDied(const string &&bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        string callName = move(bundleName);
        session_->VerifyBundleName(callName);

        /* Standard Log Output, for testers */
        HILOGE("Backup <%{public}s> Extension Process Died", callName.data());
        string versionName = session_->GetBundleVersionName(bundleName);   /* old device app version name */
        uint32_t versionCode = session_->GetBundleVersionCode(bundleName); /* old device app version code */
        if (versionCode == BConstants::DEFAULT_VERSION_CODE && versionName == BConstants::DEFAULT_VERSION_NAME) {
            ExtConnectDied(bundleName);
            return;
        }
        // 重新连接清理缓存
        HILOGE("Clear backup extension data, bundleName: %{public}s", bundleName.data());
        auto backUpConnection = session_->GetExtConnection(bundleName);
        auto callConnDone = [ptr {wptr(this)}](const string &&bundleName) {
            auto thisPtr = ptr.promote();
            if (!thisPtr) {
                HILOGW("this pointer is null.");
                return;
            }
            auto sessionConnection = thisPtr->session_->GetExtConnection(bundleName);
            if (sessionConnection->IsExtAbilityConnected()) {
                sessionConnection->DisconnectBackupExtAbility();
            }
            thisPtr->ExtConnectDied(bundleName);
        };
        backUpConnection->SetCallback(callConnDone);
        auto ret = LaunchBackupExtension(bundleName);
        if (ret) {
            ExtConnectDied(bundleName);
            return;
        }
    } catch (...) {
        HILOGE("Unexpected exception");
        return;
    }
}

void Service::ExtConnectDied(const string &callName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        /* Clear Timer */
        session_->BundleExtTimerStop(callName);
        auto backUpConnection = session_->GetExtConnection(callName);
        if (backUpConnection->IsExtAbilityConnected()) {
            backUpConnection->DisconnectBackupExtAbility();
        }
        /* Clear Session before notice client finish event */
        ClearSessionAndSchedInfo(callName);
        /* Notice Client Ext Ability Process Died */
        NoticeClientFinish(callName, BError(BError::Codes::EXT_ABILITY_DIED));
    } catch (...) {
        HILOGE("Unexpected exception");
        return;
    }
}

void Service::ExtStart(const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGE("begin %{public}s", bundleName.data());
        if (IncrementalBackup(bundleName)) {
            return;
        }
        IServiceReverse::Scenario scenario = session_->GetScenario();
        auto backUpConnection = session_->GetExtConnection(bundleName);
        auto proxy = backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
        }
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            auto ret = proxy->HandleBackup();
            session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
            if (ret) {
                ClearSessionAndSchedInfo(bundleName);
            }
            return;
        }
        if (scenario != IServiceReverse::Scenario::RESTORE) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Failed to scenario");
        }
        auto ret = proxy->HandleRestore();
        session_->GetServiceReverseProxy()->RestoreOnBundleStarted(ret, bundleName);
        auto fileNameVec = session_->GetExtFileNameRequest(bundleName);
        for (auto &fileName : fileNameVec) {
            UniqueFd fd = proxy->GetFileHandle(fileName);
            if (fd < 0) {
                HILOGE("Failed to extension file handle");
            }
            session_->GetServiceReverseProxy()->RestoreOnFileReady(bundleName, fileName, move(fd));
        }
        return;
    } catch (const BError &e) {
        return;
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return;
    } catch (...) {
        HILOGI("Unexpected exception");
        return;
    }
}

int Service::Dump(int fd, const vector<u16string> &args)
{
    if (fd < 0) {
        HILOGI("HiDumper handle invalid");
        return -1;
    }

    session_->DumpInfo(fd, args);
    return 0;
}

void Service::ExtConnectFailed(const string &bundleName, ErrCode ret)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGE("begin %{public}s", bundleName.data());
        IServiceReverse::Scenario scenario = session_->GetScenario();
        if (scenario == IServiceReverse::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnBundleStarted(ret, bundleName);
        } else if (scenario == IServiceReverse::Scenario::RESTORE &&
                   BackupPara().GetBackupOverrideIncrementalRestore() &&
                   session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(ret, bundleName);

            DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->EndRestore(bundleName);
            HILOGI("ExtConnectFailed EndRestore, code=%{public}d, bundleName=%{public}s", disposeErr,
                   bundleName.c_str());
        } else if (scenario == IServiceReverse::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnBundleStarted(ret, bundleName);

            DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->EndRestore(bundleName);
            HILOGI("ExtConnectFailed EndRestore, code=%{public}d, bundleName=%{public}s", disposeErr,
                   bundleName.c_str());
        }
        ClearSessionAndSchedInfo(bundleName);
        return;
    } catch (const BError &e) {
        return;
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return;
    } catch (...) {
        HILOGI("Unexpected exception");
        return;
    }
}

void Service::NoticeClientFinish(const string &bundleName, ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto scenario = session_->GetScenario();
    if (scenario == IServiceReverse::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
        session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(errCode, bundleName);
    } else if (scenario == IServiceReverse::Scenario::RESTORE && BackupPara().GetBackupOverrideIncrementalRestore() &&
               session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
        session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleFinished(errCode, bundleName);
    } else if (scenario == IServiceReverse::Scenario::BACKUP) {
        session_->GetServiceReverseProxy()->BackupOnBundleFinished(errCode, bundleName);
    } else if (scenario == IServiceReverse::Scenario::RESTORE) {
        session_->GetServiceReverseProxy()->RestoreOnBundleFinished(errCode, bundleName);
    };
    /* If all bundle ext process finish, notice client. */
    OnAllBundlesFinished(BError(BError::Codes::OK));
}

void Service::ExtConnectDone(string bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    /* Callback for App Ext Timeout Process. */
    auto timeoutCallback = [ptr {wptr(this)}, bundleName]() {
        auto thisPtr = ptr.promote();
        if (!thisPtr) {
            HILOGW("this pointer is null.");
            return;
        }
        auto sessionPtr = ptr->session_;
        auto sessionConnection = sessionPtr->GetExtConnection(bundleName);
        /* Standard Log Output, for testers */
        HILOGE("Backup <%{public}s> Extension Process Timeout", bundleName.data());
        sessionPtr->BundleExtTimerStop(bundleName);
        sessionConnection->DisconnectBackupExtAbility();
        /* Must clear bundle session before call NoticeClientFinish. */
        thisPtr->ClearSessionAndSchedInfo(bundleName);
        thisPtr->NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_TIMEOUT));
    };

    try {
        HILOGE("begin %{public}s", bundleName.data());
        session_->BundleExtTimerStart(bundleName, timeoutCallback);
        session_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::RUNNING);
        sched_->Sched(bundleName);
    } catch (const BError &e) {
        return;
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return;
    } catch (...) {
        HILOGI("Unexpected exception");
        return;
    }
}

void Service::ClearSessionAndSchedInfo(const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        session_->RemoveExtInfo(bundleName);
        sched_->RemoveExtConn(bundleName);
        HandleRestoreDepsBundle(bundleName);
        sched_->Sched();
    } catch (const BError &e) {
        return;
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return;
    } catch (...) {
        HILOGI("Unexpected exception");
        return;
    }
}

void Service::HandleRestoreDepsBundle(const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (session_->GetScenario() != IServiceReverse::Scenario::RESTORE) {
        return;
    }
    HILOGI("Begin, bundleName: %{public}s", bundleName.c_str());
    SvcRestoreDepsManager::GetInstance().AddRestoredBundles(bundleName);
    // 该应用恢复完成，判断依赖hap的前置hap是否全部恢复完成，如果成了，追加该依赖hap
    auto restoreBundleMap = SvcRestoreDepsManager::GetInstance().GetRestoreBundleMap();
    if (restoreBundleMap.empty()) {
        HILOGI("restoreBundleMap is empty.");
        return;
    }
    // 启动恢复会话
    vector<string> restoreBundleNames {};
    for (const auto &bundle : restoreBundleMap) {
        HILOGI("Start restore session, bundle: %{public}s", bundle.first.c_str());
        restoreBundleNames.emplace_back(bundle.first);
    }
    session_->AppendBundles(restoreBundleNames);
    for (const auto &bundle : restoreBundleMap) {
        for (auto &bundleInfo : SvcRestoreDepsManager::GetInstance().GetAllBundles()) {
            if (bundle.first != bundleInfo.name) {
                continue;
            }
            SvcRestoreDepsManager::RestoreInfo info = bundle.second;
            session_->SetBundleRestoreType(bundleInfo.name, info.restoreType_);
            session_->SetBundleVersionCode(bundleInfo.name, bundleInfo.versionCode);
            session_->SetBundleVersionName(bundleInfo.name, bundleInfo.versionName);
            session_->SetBundleDataSize(bundleInfo.name, bundleInfo.spaceOccupied);
            session_->SetBackupExtName(bundleInfo.name, bundleInfo.extensionName);
            for (auto &fileName : info.fileNames_) {
                session_->SetExtFileNameRequest(bundleInfo.name, fileName);
            }
        }
    }
    HILOGI("End");
}

void Service::OnAllBundlesFinished(ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (session_->IsOnAllBundlesFinished()) {
        IServiceReverse::Scenario scenario = session_->GetScenario();
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
            sched_->TryUnloadServiceTimer(true);
        }
    }
}

void Service::OnStartSched()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (session_->IsOnOnStartSched()) {
        for (int num = 0; num < BConstants::EXT_CONNECT_MAX_COUNT; num++) {
            sched_->Sched();
        }
    }
}

void Service::SendAppGalleryNotify(const BundleName &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    IServiceReverse::Scenario scenario = session_->GetScenario();
    if (scenario == IServiceReverse::Scenario::RESTORE) {
        DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->StartRestore(bundleName);
        HILOGI("SendAppGalleryNotify StartRestore, code=%{public}d, bundleName=%{public}s", disposeErr,
               bundleName.c_str());
    }
}

void Service::SessionDeactive()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        // 结束定时器
        sched_->ClearSchedulerData();
        // 清除缓存数据
        session_->ClearSessionData();
        // 卸载服务
        sched_->TryUnloadService();
    } catch (...) {
        HILOGI("Unexpected exception");
        return;
    }
}

ErrCode Service::GetBackupInfo(BundleName &bundleName, std::string &result)
{
    try {
        HILOGI("Service::GetBackupInfo begin.");
        session_->IncreaseSessionCnt();
        session->CreateBackupConnection(bundleName);
        auto backupConnection = session_->GetExtConnection(bundleName);
        auto callConnDone = [ptr {wptr(this)}](const string &&bundleName) {
            HILOGI(callConnDone begin.);
            auto thisPtr = ptr.promote();
            if (!thisPtr) {
                HILOGW("this pointer is null.");
                return;
            }
            thisPtr->getBackupInfoCondition_.notify_one();
        };
        backupConnection->setCallback(callConnDone);

        IServiceReverse::Scenario scenario = session_->GetScenario();
        BConstants::ExtensionAction action;
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            action = BConstants::ExtensionAction::BACKUP;
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            action = BConstants::ExtensionAction::RESTORE;
        } else {
            throw BError(BError::Codes::SA_INVAL_ARG, "Failed to scenario");
        }

        AAFwk::Want want;
        string backupExtName = session_->GetBackupExtName(bundleName); /* new device app ext name */
        HILOGD("backupExtName: %{public}s, bundleName: %{public}s", backupExtName.data(), bundleName.data());
        string versionName = session_->GetBundleVersionName(bundleName);          /* old device app version name */
        uint32_t versionCode = session_->GetBundleVersionCode(bundleName);        /* old device app version code */
        RestoreTypeEnum restoreType = session_->GetBundleRestoreType(bundleName); /* app restore type */

        want.SetElementName(bundleName, backupExtName);
        want.SetParam(BConstants::EXTENSION_ACTION_PARA, static_cast<int>(action));
        want.SetParam(BConstants::EXTENSION_VERSION_CODE_PARA, static_cast<int>(versionCode));
        want.SetParam(BConstants::EXTENSION_RESTORE_TYPE_PARA, static_cast<int>(restoreType));
        want.SetParam(BConstants::EXTENSION_VERSION_NAME_PARA, versionName);

        backUpConnection->ConnectBackupExtAbility(want, session_->GetSessionUserId());

        HILOGD("GetBackupInfo getBackupInfoMtx_ lock.");
        std::unique_lock<std::mutex> lock(getBackupInfoMtx_);
        getBackupInfoCondition_.wait_for(lock, std::chrono::seconds(CONNECT_WAIT_TIME));
        HILOGD("GetBackupInfo getBackupInfoMtx_ unlock.");
        auto proxy = backupConnection->GetBackupExtProxy();
        if (!proxy) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
        }
        auto ret = proxy->GetBackupInfo(result);

        backUpConnection->DisconnectBackupExtAbility();
        if (ret != ERR_OK) {
            HILOGE("Call Ext GetBackupInfo faild.");
            return BError(BError::Codes::SA_INVAL_ARG);
        }

        HILOGI("Service::GetBackupInfo end. result: %s", result.c_str());
        session_->DeceaseSessionCnt();
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        session_->DecreaseSessionCnt();
        HILOGE("GetBackupInfo failed, errCode = %{public}d", e.GetCode());
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
} // namespace OHOS::FileManagement::Backup
