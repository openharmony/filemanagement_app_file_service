/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "module_external/bms_adapter.h"
#include "module_ipc/svc_backup_connection.h"
#include "parameter.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

REGISTER_SYSTEM_ABILITY_BY_ID(Service, FILEMANAGEMENT_BACKUP_SERVICE_SA_ID, false);

namespace {
constexpr int32_t DEBUG_ID = 100;
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
    bool res = SystemAbility::Publish(sptr(this));
    sched_ = sptr(new SchedScheduler(wptr(this), wptr(session_)));
    sched_->StartTimer();
    HILOGI("End, res = %{public}d", res);
}

void Service::OnStop()
{
    HILOGI("Called");
    sched_ = nullptr;
    session_ = nullptr;
}

UniqueFd Service::GetLocalCapabilities()
{
    try {
        HILOGI("Begin");
        /*
         Only called by restore app before InitBackupSession,
           so there must be set init userId.
        */
        session_->SetSessionUserId(GetUserIdDefault());
        VerifyCaller();
        string path = BConstants::GetSaBundleBackupRootDir(session_->GetSessionUserId());
        BExcepUltils::VerifyPath(path, false);
        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(
            UniqueFd(open(path.data(), O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR)));

        auto cache = cachedEntity.Structuralize();

        cache.SetSystemFullName(GetOSFullName());
        cache.SetDeviceType(GetDeviceType());
        auto bundleInfos = BundleMgrAdapter::GetBundleInfos(session_->GetSessionUserId());
        cache.SetBundleInfos(bundleInfos);
        cachedEntity.Persist();

        return move(cachedEntity.GetFd());
    } catch (const BError &e) {
        return UniqueFd(-e.GetCode());
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return UniqueFd(-EPERM);
    } catch (...) {
        HILOGI("Unexpected exception");
        return UniqueFd(-EPERM);
    }
}

void Service::StopAll(const wptr<IRemoteObject> &obj, bool force)
{
    session_->Deactive(obj, force);
}

string Service::VerifyCallerAndGetCallerName()
{
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
    session_->VerifyCallerAndScenario(IPCSkeleton::GetCallingTokenID(), scenario);
    VerifyCaller();
}

ErrCode Service::InitRestoreSession(sptr<IServiceReverse> remote)
{
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
    try {
        VerifyCaller();
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
    HILOGI("Begin");
    VerifyCaller(session_->GetScenario());
    session_->Start();
    OnStartSched();
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd,
                                             const vector<BundleName> &bundleNames,
                                             RestoreTypeEnum restoreType,
                                             int32_t userId)
{
    HILOGI("Begin");
    if (userId != DEFAULT_INVAL_VALUE) { /* multi user scenario */
        session_->SetSessionUserId(userId);
    }
    VerifyCaller(IServiceReverse::Scenario::RESTORE);
    auto restoreInfos = BundleMgrAdapter::GetBundleInfos(bundleNames, session_->GetSessionUserId());
    BJsonCachedEntity<BJsonEntityCaps> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto bundleInfos = cache.GetBundleInfos();
    if (!bundleInfos.size()) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Json entity caps is empty");
    }
    session_->AppendBundles(bundleNames);
    for (auto bundleName : bundleNames) {
        for (auto &&bundleInfo : bundleInfos) {
            if (bundleInfo.name != bundleName) {
                continue;
            }
            session_->SetNeedToInstall(bundleInfo.name, bundleInfo.needToInstall);
            session_->SetBundleRestoreType(bundleInfo.name, restoreType);
            session_->SetBundleVersionCode(bundleInfo.name, bundleInfo.versionCode);
            session_->SetBundleVersionName(bundleInfo.name, bundleInfo.versionName);
        }
    }
    for (auto info : restoreInfos) {
        if (info.allToBackup == false) {
            session_->GetServiceReverseProxy()->RestoreOnBundleStarted(BError(BError::Codes::SA_REFUSED_ACT),
                                                                       info.name);
            session_->RemoveExtInfo(info.name);
        }
    }
    Start();
    Finish();
    OnStartSched();
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesBackupSession(const vector<BundleName> &bundleNames)
{
    HILOGI("Begin");
    VerifyCaller(IServiceReverse::Scenario::BACKUP);
    auto backupInfos = BundleMgrAdapter::GetBundleInfos(bundleNames, session_->GetSessionUserId());
    session_->AppendBundles(bundleNames);
    for (auto info : backupInfos) {
        if (info.allToBackup == false) {
            session_->GetServiceReverseProxy()->BackupOnBundleStarted(BError(BError::Codes::SA_REFUSED_ACT), info.name);
            session_->RemoveExtInfo(info.name);
        }
    }
    Start();
    Finish();
    OnStartSched();
    return BError(BError::Codes::OK);
}

ErrCode Service::Finish()
{
    HILOGI("Begin");
    VerifyCaller(session_->GetScenario());
    session_->Finish();
    OnAllBundlesFinished(BError(BError::Codes::OK));
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    try {
        HILOGI("Begin");
        VerifyCaller(IServiceReverse::Scenario::RESTORE);

        if (fileInfo.fileName == BConstants::RESTORE_INSTALL_PATH) {
            session_->SetInstallState(fileInfo.owner, "OK");
            sched_->Sched(fileInfo.owner);
            return BError(BError::Codes::OK);
        }
        if (!regex_match(fileInfo.fileName, regex("^[0-9a-zA-Z_.]+$"))) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not alphanumeric");
        }

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
    try {
        HILOGI("Begin");
        string callerName = VerifyCallerAndGetCallerName();
        if (!regex_match(fileName, regex("^[0-9a-zA-Z_.]+$"))) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not alphanumeric");
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
        string backupExtName = session_->GetBackupExtName(bundleName);            /* new device app ext name */
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
    try {
        HILOGI("Begin");
        VerifyCaller(IServiceReverse::Scenario::RESTORE);
        if (fileName == BConstants::RESTORE_INSTALL_PATH && regex_match(bundleName, regex("^[0-9a-zA-Z_.]+$"))) {
            session_->SetInstallState(bundleName, string(BConstants::RESTORE_INSTALL_PATH));
            auto action = session_->GetServiceSchedAction(bundleName);
            if (action == BConstants::ServiceSchedAction::INSTALLING) {
                sched_->Sched(bundleName);
            }
            return BError(BError::Codes::OK);
        }
        if (!regex_match(fileName, regex("^[0-9a-zA-Z_.]+$"))) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not alphanumeric");
        }
        session_->SetExtFileNameRequest(bundleName, fileName);
        auto action = session_->GetServiceSchedAction(bundleName);
        if (action == BConstants::ServiceSchedAction::RUNNING) {
            sched_->Sched(bundleName);
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

void Service::OnBackupExtensionDied(const string &&bundleName, ErrCode ret)
{
    try {
        HILOGI("extension died. Died bundleName = %{public}s", bundleName.data());
        string callName = move(bundleName);
        session_->VerifyBundleName(callName);
        auto scenario = session_->GetScenario();
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnBundleFinished(ret, callName);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnBundleFinished(ret, callName);
        }
        auto backUpConnection = session_->GetExtConnection(callName);
        if (backUpConnection->IsExtAbilityConnected()) {
            backUpConnection->DisconnectBackupExtAbility();
        }
        ClearSessionAndSchedInfo(bundleName);
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

void Service::ExtStart(const string &bundleName)
{
    try {
        HILOGE("begin %{public}s", bundleName.data());
        IServiceReverse::Scenario scenario = session_->GetScenario();
        auto backUpConnection = session_->GetExtConnection(bundleName);
        auto proxy = backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
        }
        if (session_->GetBundleVersionCode(bundleName) != BConstants::DEFAULT_VERSION_CODE &&
            session_->GetBundleVersionName(bundleName) != BConstants::DEFAULT_VERSION_NAME &&
            session_->GetBundleRestoreType(bundleName) != RestoreTypeEnum::RESTORE_DATA_READDY) {
            proxy->HandleClear();
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
        session_->GetServiceReverseProxy()->RestoreOnBundleStarted(BError(BError::Codes::OK), bundleName);
        auto fileNameVec = session_->GetExtFileNameRequest(bundleName);
        for (auto &fileName : fileNameVec) {
            UniqueFd fd = proxy->GetFileHandle(fileName);
            if (fd < 0) {
                HILOGE("Failed to extension file handle");
                OnBackupExtensionDied(move(bundleName), fd);
                return;
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
    try {
        HILOGE("begin %{public}s", bundleName.data());
        IServiceReverse::Scenario scenario = session_->GetScenario();
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnBundleStarted(ret, bundleName);
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

void Service::ExtConnectDone(string bundleName)
{
    try {
        HILOGE("begin %{public}s", bundleName.data());
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
    try {
        session_->RemoveExtInfo(bundleName);
        sched_->RemoveExtConn(bundleName);
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

void Service::OnAllBundlesFinished(ErrCode errCode)
{
    if (session_->IsOnAllBundlesFinished()) {
        IServiceReverse::Scenario scenario = session_->GetScenario();
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnAllBundlesFinished(errCode);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnAllBundlesFinished(errCode);
        }
        sched_->TryUnloadServiceTimer(true);
    }
}

void Service::OnStartSched()
{
    if (session_->IsOnOnStartSched()) {
        for (int num = 0; num < BConstants::EXT_CONNECT_MAX_COUNT; num++) {
            sched_->Sched();
        }
    }
}
} // namespace OHOS::FileManagement::Backup
