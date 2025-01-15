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

#include "module_ipc/svc_session_manager.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <regex>
#include <sstream>
#include <string>

#include "b_anony/b_anony.h"
#include "b_error/b_error.h"
#include "b_file_info.h"
#include "b_json/b_json_entity_caps.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_radar/b_radar.h"
#include "b_resources/b_constants.h"
#include "b_sa/b_sa_utils.h"
#include "b_utils/b_time.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/service.h"
#include "module_ipc/svc_restore_deps_manager.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ErrCode SvcSessionManager::VerifyCallerAndScenario(uint32_t clientToken, IServiceReverse::Scenario scenario) const
{
    shared_lock<shared_mutex> lock(lock_);
    if (impl_.scenario != scenario) {
        HILOGE("Verify caller failed, Inconsistent scenario, impl scenario:%{public}d", impl_.scenario);
        AppRadar::Info info("", "", "Inconsistent scenario");
        AppRadar::GetInstance().RecordDefaultFuncRes(info, "SvcSessionManager::VerifyCallerAndScenario", impl_.userId,
                                                     BizStageBackup::BIZ_STAGE_PERMISSION_CHECK_FAIL,
                                                     BError(BError::Codes::SDK_MIXED_SCENARIO).GetCode());
        return BError(BError::Codes::SDK_MIXED_SCENARIO);
    }
    if (impl_.clientToken != clientToken) {
        HILOGE("Verify caller failed, Caller mismatched");
        AppRadar::Info info2("", "", "Caller mismatched");
        AppRadar::GetInstance().RecordDefaultFuncRes(info2, "SvcSessionManager::VerifyCallerAndScenario", impl_.userId,
                                                     BizStageBackup::BIZ_STAGE_PERMISSION_CHECK_FAIL,
                                                     BError(BError::Codes::SDK_MIXED_SCENARIO).GetCode());

        return BError(BError::Codes::SA_REFUSED_ACT);
    }
    HILOGD("Succeed to verify the caller");
    return BError(BError::Codes::OK);
}

SvcSessionManager::Impl SvcSessionManager::GetImpl()
{
    return impl_;
}

int SvcSessionManager::GetSessionCnt()
{
    return sessionCnt_.load();
}

ErrCode SvcSessionManager::Active(Impl newImpl, bool isOccupyingSession)
{
    unique_lock<shared_mutex> lock(lock_);
    const Impl &oldImpl = impl_;
    if (oldImpl.clientToken) {
        HILOGE("Already have an active session, userId=%{public}d, caller=%{public}s, activeTime=%{public}s",
            impl_.userId, impl_.callerName.c_str(), impl_.activeTime.c_str());
        return BError(BError::Codes::SA_SESSION_CONFLICT);
    }

    if (!isOccupyingSession && !newImpl.clientToken) {
        HILOGE("Active session fail, No caller token was specified");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    if (!isOccupyingSession && newImpl.scenario == IServiceReverse::Scenario::UNDEFINED) {
        HILOGE("Active session fail, No scenario was specified");
        return BError(BError::Codes::SA_INVAL_ARG);
    }

    if (!isOccupyingSession) {
        ErrCode ret = InitClient(newImpl);
        if (ret != BError(BError::Codes::OK)) {
            HILOGE("Active session failed, init client error");
            return ret;
        }
    }
    impl_ = newImpl;
    IncreaseSessionCnt(__PRETTY_FUNCTION__);
    return BError(BError::Codes::OK);
}

ErrCode SvcSessionManager::Deactive(const wptr<IRemoteObject> &remoteInAction, bool force)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("Deactive session fail, caller token is invalid");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    if (!force && (!impl_.clientToken || !impl_.clientProxy)) {
        HILOGE("Deactive session fail, client proxy is invalid");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    if (!force && (remoteInAction != impl_.clientProxy->AsObject())) {
        HILOGE("Deactive session fail, Only the client actived the session can deactive it");
        return BError(BError::Codes::SA_INVAL_ARG);
    }

    deathRecipient_ = nullptr;
    AppRadar::Info info("", "", "deactive session success");
    if (impl_.scenario == IServiceReverse::Scenario::RESTORE) {
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "SvcSessionManager::Deactive", impl_.userId,
            BizStageRestore::BIZ_STAGE_DEACTIVE_SESSION, ERR_OK);
    } else if (impl_.scenario == IServiceReverse::Scenario::BACKUP) {
        AppRadar::GetInstance().RecordBackupFuncRes(info, "SvcSessionManager::Deactive", impl_.userId,
            BizStageBackup::BIZ_STAGE_DEACTIVE_SESSION, ERR_OK);
    }
    HILOGI("Succeed to deactive a session");
    impl_ = {};
    extConnectNum_ = 0;
    DecreaseSessionCnt(__PRETTY_FUNCTION__);
    return BError(BError::Codes::OK);
}

ErrCode SvcSessionManager::VerifyBundleName(string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("Verify bundle name failed, No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto asVerify = [&bundleName](const auto &it) { return it.first == bundleName; };
    if (none_of(impl_.backupExtNameMap.begin(), impl_.backupExtNameMap.end(), asVerify)) {
        HILOGE("Could not find the bundle from current session, bundleName:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_REFUSED_ACT);
    }
    HILOGD("Succeed to verify the bundleName");
    return BError(BError::Codes::OK);
}

sptr<IServiceReverse> SvcSessionManager::GetServiceReverseProxy()
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientProxy) {
        throw BError(BError::Codes::SA_REFUSED_ACT, "Try to deactive an empty session");
    }
    return impl_.clientProxy;
}

IServiceReverse::Scenario SvcSessionManager::GetScenario()
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("Get scenario failed, No caller token was specified");
        return IServiceReverse::Scenario::UNDEFINED;
    }
    return impl_.scenario;
}

int32_t SvcSessionManager::GetSessionUserId()
{
    return impl_.userId;
}

void SvcSessionManager::SetSessionUserId(int32_t userId)
{
    impl_.userId = userId;
}

string SvcSessionManager::GetSessionCallerName()
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("Get scenario failed, No caller token was specified");
        return "";
    }
    return impl_.callerName;
}

string SvcSessionManager::GetSessionActiveTime()
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("Get scenario failed, No caller token was specified");
        return "";
    }
    return impl_.activeTime;
}

bool SvcSessionManager::OnBundleFileReady(const string &bundleName, const string &fileName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("OnBundleFileReady failed, bundleName:%{public}s, fileName:%{public}s", bundleName.c_str(),
            GetAnonyPath(fileName).c_str());
        return false;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find the bundle:%{public}s", bundleName.c_str());
        return false;
    }
    // 判断是否结束 通知EXTENTION清理资源  TOOL应用完成备份
    if (impl_.scenario == IServiceReverse::Scenario::RESTORE || SAUtils::IsSABundleName(bundleName)) {
        it->second.isBundleFinished = true;
        return true;
    } else if (impl_.scenario == IServiceReverse::Scenario::BACKUP) {
        if (!fileName.empty() && fileName != BConstants::EXT_BACKUP_MANAGE) {
            auto ret = it->second.fileNameInfo.emplace(fileName);
            if (!ret.second) {
                it->second.fileNameInfo.erase(fileName);
            }
        } else if (fileName.empty()) {
            it->second.receExtAppDone = true;
        }
        if (it->second.receExtManageJson && it->second.fileNameInfo.empty() && it->second.receExtAppDone) {
            HILOGI("The bundle manage json info and file info support current app done, bundle:%{public}s",
                bundleName.c_str());
            it->second.isBundleFinished = true;
            return true;
        }
    }
    HILOGD("End, bundleName name is:%{public}s", bundleName.c_str());
    return false;
}

UniqueFd SvcSessionManager::OnBundleExtManageInfo(const string &bundleName, UniqueFd fd)
{
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return UniqueFd(-EPERM);
    }
    if (impl_.scenario != IServiceReverse::Scenario::BACKUP) {
        HILOGE("Invalid Scenario, bundleName:%{public}s", bundleName.c_str());
        return UniqueFd(-EPERM);
    }

    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManage();

    for (const auto &fileName : info) {
        HILOGE("fileName %{public}s", GetAnonyPath(fileName).data());
        OnBundleFileReady(bundleName, fileName);
    }

    unique_lock<shared_mutex> lock(lock_);
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find the bundle:%{public}s", bundleName.c_str());
        return UniqueFd(-EPERM);
    }
    it->second.receExtManageJson = true;
    return move(cachedEntity.GetFd());
}

void SvcSessionManager::RemoveExtInfo(const string &bundleName)
{
    HILOGD("svcMrg:RemoveExt, bundleName:%{public}s", bundleName.c_str());
    unique_lock<shared_mutex> lock(lock_);
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        HILOGE("BackupExtNameMap not contain %{public}s", bundleName.c_str());
        return;
    }
    if (extConnectNum_) {
        extConnectNum_--;
    }
    int32_t &appendNum = impl_.backupExtNameMap[bundleName].appendNum;
    if (--appendNum > 0) {
        HILOGI("No need remove bundleName:%{public}s, appendNum=%{public}d", bundleName.c_str(), appendNum);
        return;
    }
    impl_.backupExtNameMap.erase(it);
}

wptr<SvcBackupConnection> SvcSessionManager::GetExtConnection(const BundleName &bundleName)
{
    HILOGD("svcMrg:GetExt, bundleName:%{public}s", bundleName.c_str());
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("GetExt connection failed, No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return nullptr;
    }
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        HILOGE("Could not find the bundle from current session, bundleName:%{public}s", bundleName.c_str());
        return nullptr;
    }
    if (!it->second.backUpConnection) {
        HILOGE("Current bundle extension connection is empty, bundleName:%{public}s", bundleName.c_str());
        return nullptr;
    }
    return wptr(it->second.backUpConnection);
}

std::weak_ptr<SABackupConnection> SvcSessionManager::GetSAExtConnection(const BundleName &bundleName)
{
    HILOGD("svcMrg:GetExt, bundleName:%{public}s", bundleName.c_str());
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return std::weak_ptr<SABackupConnection>();
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find current bundle, bundleName:%{public}s", bundleName.c_str());
        return std::weak_ptr<SABackupConnection>();
    }
    if (!it->second.saBackupConnection) {
        HILOGE("SA backup connection is empty, bundleName:%{public}s", bundleName.c_str());
        return std::weak_ptr<SABackupConnection>();
    }

    return std::weak_ptr<SABackupConnection>(it->second.saBackupConnection);
}

sptr<SvcBackupConnection> SvcSessionManager::GetBackupAbilityExt(const string &bundleName)
{
    auto callDied = [revPtr {reversePtr_}](const string &&bundleName, bool isCleanCalled = false) {
        auto revPtrStrong = revPtr.promote();
        if (!revPtrStrong) {
            // 服务先于客户端死亡是一种异常场景，但该场景对本流程来说也没什么影响，所以只是简单记录一下
            HILOGW("It's curious that the backup sa dies before the backup client");
            return;
        }
        revPtrStrong->OnBackupExtensionDied(move(bundleName), isCleanCalled);
    };

    auto callConnected = [revPtr {reversePtr_}](const string &&bundleName) {
        auto revPtrStrong = revPtr.promote();
        if (!revPtrStrong) {
            // 服务先于客户端死亡是一种异常场景，但该场景对本流程来说也没什么影响，所以只是简单记录一下
            HILOGW("It's curious that the backup sa dies before the backup client");
            return;
        }
        revPtrStrong->ExtConnectDone(move(bundleName));
    };

    return sptr<SvcBackupConnection>(new SvcBackupConnection(callDied, callConnected, bundleName));
}

std::shared_ptr<SABackupConnection> SvcSessionManager::GetBackupSAExt(const std::string &bundleName)
{
    auto callDied = [revPtr {reversePtr_}](const string &&bundleName) {
        auto revPtrStrong = revPtr.promote();
        if (!revPtrStrong) {
            // 服务先于客户端死亡是一种异常场景，但该场景对本流程来说也没什么影响，所以只是简单记录一下
            HILOGW("It's curious that the backup sa dies before the backup client");
            return;
        }
        revPtrStrong->OnBackupExtensionDied(move(bundleName));
    };

    auto callConnected = [revPtr {reversePtr_}](const string &&bundleName) {
        auto revPtrStrong = revPtr.promote();
        if (!revPtrStrong) {
            // 服务先于客户端死亡是一种异常场景，但该场景对本流程来说也没什么影响，所以只是简单记录一下
            HILOGW("It's curious that the backup sa dies before the backup client");
            return;
        }
        revPtrStrong->ExtConnectDone(move(bundleName));
    };

    auto callBackup = [revPtr {reversePtr_}](const string &&bundleName, const int &&fd, const std::string result,
                                             const ErrCode &&errCode) {
        auto revPtrStrong = revPtr.promote();
        if (!revPtrStrong) {
            // 服务先于客户端死亡是一种异常场景，但该场景对本流程来说也没什么影响，所以只是简单记录一下
            HILOGW("It's curious that the backup sa dies before the backup client");
            return;
        }
        revPtrStrong->OnSABackup(move(bundleName), move(fd), move(result), move(errCode));
    };

    auto callRestore = [revPtr {reversePtr_}](const string &&bundleName, const std::string result,
                                              const ErrCode &&errCode) {
        auto revPtrStrong = revPtr.promote();
        if (!revPtrStrong) {
            // 服务先于客户端死亡是一种异常场景，但该场景对本流程来说也没什么影响，所以只是简单记录一下
            HILOGW("It's curious that the backup sa dies before the backup client");
            return;
        }
        revPtrStrong->OnSARestore(move(bundleName), move(result), move(errCode));
    };

    return std::make_shared<SABackupConnection>(callDied, callConnected, callBackup, callRestore);
}

void SvcSessionManager::DumpInfo(const int fd, const std::vector<std::u16string> &args)
{
    dprintf(fd, "---------------------backup info--------------------\n");
    dprintf(fd, "Scenario: %d\n", impl_.scenario);
}

ErrCode SvcSessionManager::InitClient(Impl &newImpl)
{
    if (!newImpl.clientProxy) {
        HILOGE("Init client error, Invalid client");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto remoteObj = newImpl.clientProxy->AsObject();
    if (!remoteObj) {
        HILOGE("Init client error, Proxy's remote object can't be nullptr");
        return BError(BError::Codes::SA_BROKEN_IPC);
    }

    auto callback = [revPtr {reversePtr_}](const wptr<IRemoteObject> &obj) {
        HILOGI("Client died.");

        auto revPtrStrong = revPtr.promote();
        if (!revPtrStrong) {
            // 服务先于客户端死亡是一种异常场景，但该场景对本流程来说也没什么影响，所以只是简单记录一下
            HILOGW("It's curious that the backup sa dies before the backup client");
            return;
        }
        (void)revPtrStrong->SessionDeactive();
    };
    deathRecipient_ = sptr(new SvcDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient_);
    AppRadar::Info info("", "", "active session success");
    if (newImpl.scenario == IServiceReverse::Scenario::RESTORE) {
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "SvcSessionManager::InitClient", newImpl.userId,
            BizStageRestore::BIZ_STAGE_ACTIVE_SESSION, ERR_OK);
    } else if (newImpl.scenario == IServiceReverse::Scenario::BACKUP) {
        AppRadar::GetInstance().RecordBackupFuncRes(info, "SvcSessionManager::InitClient", newImpl.userId,
            BizStageBackup::BIZ_STAGE_ACTIVE_SESSION, ERR_OK);
    }
    HILOGI("Succeed to active a session");
    return BError(BError::Codes::OK);
}

void SvcSessionManager::SetExtFileNameRequest(const string &bundleName, const string &fileName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find current bundle, bundleName:%{public}s", bundleName.c_str());
        return;
    }
    it->second.fileNameInfo.emplace(fileName);
}

std::set<std::string> SvcSessionManager::GetExtFileNameRequest(const std::string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return std::set<std::string>();
    }

    if (impl_.scenario != IServiceReverse::Scenario::RESTORE) {
        HILOGE("Invalid Scenario, bundleName:%{public}s", bundleName.c_str());
        return std::set<std::string>();
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return std::set<std::string>();
    }
    set<string> fileNameInfo = it->second.fileNameInfo;
    it->second.fileNameInfo.clear();
    return fileNameInfo;
}

std::tuple<bool, std::map<BundleName, BackupExtInfo>::iterator> SvcSessionManager::GetBackupExtNameMap(
    const string &bundleName)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        HILOGE("Could not find the bundle from current session, bundleName:%{public}s", bundleName.c_str());
        return {false, impl_.backupExtNameMap.end()};
    }
    return {true, it};
}

bool SvcSessionManager::GetSchedBundleName(string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (extConnectNum_ >= BConstants::EXT_CONNECT_MAX_COUNT) {
        HILOGE("Sched bundle count is too many");
        return false;
    }

    for (auto &&it : impl_.backupExtNameMap) {
        if (it.second.schedAction == BConstants::ServiceSchedAction::WAIT) {
            bundleName = it.first;
            if (!it.second.isReadyLaunch) {
                HILOGE("Current bundle:%{public}s can not sched, baseInfo is not set done", bundleName.c_str());
                return false;
            }
            it.second.schedAction = BConstants::ServiceSchedAction::START;
            extConnectNum_++;
            return true;
        }
    }
    return false;
}

BConstants::ServiceSchedAction SvcSessionManager::GetServiceSchedAction(const std::string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return BConstants::ServiceSchedAction::UNKNOWN;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return BConstants::ServiceSchedAction::UNKNOWN;
    }
    return it->second.schedAction;
}

void SvcSessionManager::SetServiceSchedAction(const string &bundleName, BConstants::ServiceSchedAction action)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        throw BError(BError::Codes::SA_REFUSED_ACT, "BackupExtNameMap can not find bundle");
    }
    it->second.schedAction = action;
    if (it->second.schedAction == BConstants::ServiceSchedAction::START) {
        extConnectNum_++;
    }
}

void SvcSessionManager::SetBackupExtName(const string &bundleName, const string &backupExtName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return;
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return;
    }
    it->second.backupExtName = backupExtName;
}

string SvcSessionManager::GetBackupExtName(const string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return "";
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return "";
    }
    return it->second.backupExtName;
}

void SvcSessionManager::SetBackupExtInfo(const string &bundleName, const string &extInfo)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return;
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return;
    }
    it->second.extInfo = extInfo;
}

std::string SvcSessionManager::GetBackupExtInfo(const string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return "";
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return "";
    }
    return it->second.extInfo;
}

void SvcSessionManager::SetBundleUserId(const string &bundleName, const int32_t userId)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return;
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return;
    }
    it->second.userId = userId;
}

int32_t SvcSessionManager::GetBundleUserId(const string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return BConstants::DEFAULT_USER_ID;
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return GetSessionUserId();
    }
    return it->second.userId;
}

void SvcSessionManager::AppendBundles(const vector<BundleName> &bundleNames, vector<BundleName> &failedBundles)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("AppendBundles error, No caller token was specified");
        return;
    }

    for (auto &&bundleName : bundleNames) {
        HILOGD("bundleName: %{public}s", bundleName.c_str());
        BackupExtInfo info {};
        auto it = impl_.backupExtNameMap.find(bundleName);
        if (it != impl_.backupExtNameMap.end()) {
            if (impl_.backupExtNameMap[bundleName].userId == GetSessionUserId()) {
                HILOGE("BackupExtNameMap already contain %{public}s", bundleName.c_str());
                info.backUpConnection = impl_.backupExtNameMap[bundleName].backUpConnection;
                info.saBackupConnection = impl_.backupExtNameMap[bundleName].saBackupConnection;
                info.appendNum = impl_.backupExtNameMap[bundleName].appendNum + 1;
                impl_.backupExtNameMap[bundleName] = info;
            } else {
                failedBundles.push_back(bundleName);
            }
            continue;
        }
        if (SAUtils::IsSABundleName(bundleName)) {
            info.saBackupConnection = GetBackupSAExt(bundleName);
        } else {
            info.backUpConnection = GetBackupAbilityExt(bundleName);
        }
        impl_.backupExtNameMap.emplace(make_pair(bundleName, info));
    }
    impl_.isBackupStart = true;
    impl_.isAppendFinish = true;
}

sptr<SvcBackupConnection> SvcSessionManager::CreateBackupConnection(BundleName &bundleName)
{
    HILOGD("SvcSessionManager::CreateBackupConnection begin.");
    return GetBackupAbilityExt(bundleName);
}

ErrCode SvcSessionManager::Start()
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("Start error, No caller token was specified");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    impl_.isBackupStart = true;
    return BError(BError::Codes::OK);
}

ErrCode SvcSessionManager::Finish()
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("Finish error, No caller token was specified");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    impl_.isAppendFinish = true;
    return BError(BError::Codes::OK);
}

bool SvcSessionManager::IsOnAllBundlesFinished()
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("IsOnAllBundlesFinished error, No caller token was specified");
        return false;
    }
    bool isAllBundlesFinished = !impl_.backupExtNameMap.size();
    if (impl_.scenario == IServiceReverse::Scenario::RESTORE) {
        bool isAllBundlesRestored = SvcRestoreDepsManager::GetInstance().IsAllBundlesRestored();
        isAllBundlesFinished = (isAllBundlesFinished && isAllBundlesRestored);
    }
    HILOGD("isAllBundlesFinished:%{public}d", isAllBundlesFinished);
    return isAllBundlesFinished;
}

bool SvcSessionManager::IsOnOnStartSched()
{
    HILOGI("Begin");
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("IsOnOnStartSched error, No caller token was specified");
        return false;
    }
    if (impl_.isBackupStart && impl_.backupExtNameMap.size()) {
        return true;
    }
    HILOGI("End");
    return false;
}

bool SvcSessionManager::NeedToUnloadService()
{
    unique_lock<shared_mutex> lock(lock_);
    if (impl_.restoreDataType == RestoreTypeEnum::RESTORE_DATA_READDY) {
        return false;
    }
    bool isNeedToUnloadService = (!impl_.backupExtNameMap.size() && (sessionCnt_.load() <= 0));
    if (impl_.scenario == IServiceReverse::Scenario::RESTORE) {
        bool isAllBundlesRestored = SvcRestoreDepsManager::GetInstance().IsAllBundlesRestored();
        isNeedToUnloadService = (isNeedToUnloadService && isAllBundlesRestored);
    }
    HILOGD("isNeedToUnloadService:%{public}d", isNeedToUnloadService);
    return isNeedToUnloadService;
}

void SvcSessionManager::SetBundleRestoreType(const std::string &bundleName, RestoreTypeEnum restoreType)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return;
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return;
    }
    it->second.restoreType = restoreType;
}

RestoreTypeEnum SvcSessionManager::GetBundleRestoreType(const std::string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return RestoreTypeEnum::RESTORE_DATA_WAIT_SEND;
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return RestoreTypeEnum::RESTORE_DATA_WAIT_SEND;
    }
    return it->second.restoreType;
}

void SvcSessionManager::SetBundleVersionCode(const std::string &bundleName, int64_t versionCode)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return;
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return;
    }
    it->second.versionCode = versionCode;
}

int64_t SvcSessionManager::GetBundleVersionCode(const std::string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return 0;
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return 0;
    }
    return it->second.versionCode;
}

void SvcSessionManager::SetBundleVersionName(const std::string &bundleName, std::string versionName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return;
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return;
    }
    it->second.versionName = versionName;
}

std::string SvcSessionManager::GetBundleVersionName(const std::string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return "";
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return "";
    }
    return it->second.versionName;
}

void SvcSessionManager::SetBundleDataSize(const std::string &bundleName, int64_t dataSize)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return;
    }

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return;
    }
    it->second.dataSize = dataSize;
}

uint32_t SvcSessionManager::CalAppProcessTime(const std::string &bundleName)
{
    const int64_t minTimeout = 900;      /* 900 second */
    const int64_t defaultTimeout = 30;           /* 30 second */
    const int64_t processRate = 3 * 1024 * 1024; /* 3M/s */
    const int64_t multiple = 3;
    const int64_t invertMillisecond = 1000;
    int64_t timeout;
    uint32_t resTimeoutMs;

    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("CalAppProcessTime failed, BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        resTimeoutMs = (uint32_t)(minTimeout * invertMillisecond % UINT_MAX);
        HILOGE("Current app will run timeout=%{public}u(ms), bundleName=%{public}s ", resTimeoutMs, bundleName.c_str());
        return resTimeoutMs;
    }
    int64_t appSize = it->second.dataSize;
    /* timeout = (AppSize / 3Ms) * 3 + 30 */
    timeout = defaultTimeout + (appSize / processRate) * multiple;
    timeout = timeout < minTimeout ? minTimeout : timeout;
    resTimeoutMs = (uint32_t)(timeout * invertMillisecond % UINT_MAX); /* conver second to millisecond */
    HILOGI("Calculate App extension process run timeout=%{public}u(ms), bundleName=%{public}s ", resTimeoutMs,
           bundleName.c_str());
    return resTimeoutMs;
}

bool SvcSessionManager::StartFwkTimer(const std::string &bundleName, const Utils::Timer::TimerCallback &callback)
{
    unique_lock<shared_mutex> lock(lock_);
    HILOGI("StartFwkTimer begin bundleName %{public}s", bundleName.c_str());
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified");
        return false;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("Start fwk timer failed, BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    if (it->second.fwkTimerStatus == true) {
        HILOGE("FwkTimer is registered, unregister first.");
        return false;
    }
    uint32_t timeout = CalAppProcessTime(bundleName);

    it->second.fwkTimerStatus = true;
    it->second.timerId = timer_.Register(callback, timeout, true);
    HILOGI("StartFwkTimer end bundleName %{public}s", bundleName.c_str());
    return true;
}

bool SvcSessionManager::StopFwkTimer(const std::string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    HILOGI("StopFwkTimer begin bundleName %{public}s", bundleName.c_str());
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified");
        return false;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("Stop fwk timer failed, BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    if (it->second.fwkTimerStatus == false) {
        HILOGE("FwkTimer is unregistered, register first.");
        return true;
    }

    it->second.fwkTimerStatus = false;
    timer_.Unregister(it->second.timerId);
    HILOGI("StopFwkTimer end bundleName %{public}s", bundleName.c_str());
    return true;
}

bool SvcSessionManager::StartExtTimer(const std::string &bundleName, const Utils::Timer::TimerCallback &callback)
{
    unique_lock<shared_mutex> lock(lock_);
    HILOGI("StartExtTimer begin bundleName %{public}s", bundleName.c_str());
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified");
        return false;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("Start extension timer failed, BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    if (it->second.extTimerStatus == true) {
        HILOGE("ExtTimer is registered, unregister first.");
        return false;
    }
    uint32_t timeout = it->second.timeout;
    timeout = (timeout != BConstants::TIMEOUT_INVALID) ? timeout : BConstants::DEFAULT_TIMEOUT;
    it->second.extTimerStatus = true;
    it->second.timerId = timer_.Register(callback, timeout, true);
    HILOGI("StartExtTimer end, timeout %{public}u(ms), bundleName %{public}s", timeout, bundleName.c_str());
    return true;
}

bool SvcSessionManager::StopExtTimer(const std::string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    HILOGI("StopExtTimer begin bundleName %{public}s", bundleName.c_str());
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified");
        return false;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("Stop extension timer failed, BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    if (it->second.extTimerStatus == false) {
        HILOGE("ExtTimer is unregistered, register first.");
        return true;
    }

    it->second.extTimerStatus = false;
    it->second.timeout = BConstants::TIMEOUT_INVALID;
    timer_.Unregister(it->second.timerId);
    HILOGI("StopExtTimer end bundleName %{public}s", bundleName.c_str());
    return true;
}

bool SvcSessionManager::UpdateTimer(const std::string &bundleName, uint32_t timeout,
    const Utils::Timer::TimerCallback &callback)
{
    unique_lock<shared_mutex> lock(lock_);
    HILOGI("UpdateTimer begin bundleName %{public}s", bundleName.c_str());
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified");
        return false;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("Update timer failed, BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    it->second.timeout = timeout;
    if (it->second.extTimerStatus == false) {
        HILOGI("ExtTimer is unregistered, just store timeout %{public}u(ms)", timeout);
        return true;
    }

    timer_.Unregister(it->second.timerId);
    HILOGI("UpdateTimer timeout %{public}u(ms), bundleName %{public}s ", timeout, bundleName.c_str());
    it->second.timerId = timer_.Register(callback, timeout, true);
    it->second.extTimerStatus = true;
    HILOGI("UpdateTimer end bundleName %{public}s", bundleName.c_str());
    return true;
}

void SvcSessionManager::IncreaseSessionCnt(const std::string funcName)
{
    sessionCnt_++;
    HILOGI("func name:%{public}s, %{public}d.", funcName.c_str(), sessionCnt_.load());
}

void SvcSessionManager::DecreaseSessionCnt(const std::string funcName)
{
    if (sessionCnt_.load() > 0) {
        sessionCnt_--;
    } else {
        HILOGE("Invalid sessionCount.");
    }
    HILOGI("func name:%{public}s, %{public}d.", funcName.c_str(), sessionCnt_.load());
}

ErrCode SvcSessionManager::ClearSessionData()
{
    unique_lock<shared_mutex> lock(lock_);
    ErrCode ret = BError(BError::Codes::OK);
    for (auto &&it : impl_.backupExtNameMap) {
        // clear timer
        if (it.second.fwkTimerStatus == true || it.second.extTimerStatus == true) {
            it.second.fwkTimerStatus = false;
            it.second.extTimerStatus = false;
            timer_.Unregister(it.second.timerId);
        }
        // disconnect extension
        if (it.second.schedAction == BConstants::ServiceSchedAction::RUNNING) {
            auto backUpConnection = it.second.backUpConnection;
            if (backUpConnection == nullptr) {
                HILOGE("Clear session error, backUpConnection is empty");
                return BError(BError::Codes::SA_INVAL_ARG);
            }
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (proxy == nullptr) {
                HILOGE("Clear session error, proxy is empty");
                return BError(BError::Codes::EXT_INVAL_ARG);
            }
            if (impl_.restoreDataType != RestoreTypeEnum::RESTORE_DATA_READDY) {
                ret = proxy->HandleClear();
            }
            backUpConnection->DisconnectBackupExtAbility();
        }
        if (ret != BError(BError::Codes::OK)) {
            return ret;
        }
        // clear data
        it.second.schedAction = BConstants::ServiceSchedAction::FINISH;
    }
    impl_.backupExtNameMap.clear();
    return BError(BError::Codes::OK);
}

bool SvcSessionManager::GetIsIncrementalBackup()
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("GetIsIncrementalBackup error, No caller token was specified");
        return false;
    }
    return impl_.isIncrementalBackup;
}

void SvcSessionManager::SetIncrementalData(const BIncrementalData &incrementalData)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("SetIncrementalData error, No caller token was specified");
        return;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(incrementalData.bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", incrementalData.bundleName.c_str());
        return;
    }
    it->second.lastIncrementalTime = incrementalData.lastIncrementalTime;
    it->second.manifestFd = incrementalData.manifestFd;
    it->second.backupParameters = incrementalData.backupParameters;
    it->second.backupPriority = incrementalData.backupPriority;
}

int32_t SvcSessionManager::GetIncrementalManifestFd(const string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return BConstants::INVALID_FD_NUM;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return BConstants::INVALID_FD_NUM;
    }
    return it->second.manifestFd;
}

int64_t SvcSessionManager::GetLastIncrementalTime(const string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return 0;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return 0;
    }
    return it->second.lastIncrementalTime;
}

int32_t SvcSessionManager::GetMemParaCurSize()
{
    return memoryParaCurSize_;
}

void SvcSessionManager::SetMemParaCurSize(int32_t size)
{
    memoryParaCurSize_ = size;
}

void SvcSessionManager::SetClearDataFlag(const std::string &bundleName, bool isClearData)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return;
    }
    it->second.isClearData = isClearData;
    HILOGI("bundleName:%{public}s, set clear data flag:%{public}d.", bundleName.c_str(), isClearData);
}

bool SvcSessionManager::GetClearDataFlag(const std::string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return true;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return true;
    }
    return it->second.isClearData;
}

uint32_t SvcSessionManager::GetTimeoutValue(const std::string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return BConstants::TIMEOUT_INVALID;
    }
    return it->second.timeout;
}

bool SvcSessionManager::ValidRestoreDataType(RestoreTypeEnum restoreDataType)
{
    return impl_.restoreDataType == restoreDataType;
}

bool SvcSessionManager::CleanAndCheckIfNeedWait(ErrCode &ret, std::vector<std::string> &bundleNameList)
{
    unique_lock<shared_mutex> lock(lock_);
    for (auto it = impl_.backupExtNameMap.begin(); it != impl_.backupExtNameMap.end();) {
        if (it->second.schedAction == BConstants::ServiceSchedAction::WAIT) {
            it = impl_.backupExtNameMap.erase(it);
        } else if (it->second.schedAction == BConstants::ServiceSchedAction::START ||
            (it->second.schedAction == BConstants::ServiceSchedAction::RUNNING && !it->second.isInPublishFile)) {
            if (it->second.fwkTimerStatus == true || it->second.extTimerStatus == true) {
                it->second.fwkTimerStatus = false;
                it->second.extTimerStatus = false;
                timer_.Unregister(it->second.timerId);
            }
            auto backUpConnection = it->second.backUpConnection;
            if (backUpConnection == nullptr) {
                HILOGE("Clear session error, backUpConnection is empty");
                it = impl_.backupExtNameMap.erase(it);
                continue;
            }
            auto proxy = backUpConnection->GetBackupExtProxy();
            // start action
            if (proxy == nullptr) {
                HILOGE("Clear session error, backUpConnection is empty");
                backUpConnection->DisconnectBackupExtAbility();
                it = impl_.backupExtNameMap.erase(it);
                continue;
            }
            // running action
            ErrCode retTmp = ERR_OK;
            if (impl_.restoreDataType != RestoreTypeEnum::RESTORE_DATA_READDY) {
                retTmp = proxy->HandleClear();
            }
            if (retTmp == ERR_OK) {
                bundleNameList.emplace_back(it->first);
            } else {
                ret = retTmp;
            }
            backUpConnection->DisconnectBackupExtAbility();
            it = impl_.backupExtNameMap.erase(it);
        } else {
            ++it;
        }
    }
    if (impl_.backupExtNameMap.empty()) {
        HILOGI("Release normally, no need wait");
        return false;
    }
    HILOGI("Release abnormally, need wait for restore");
    return true;
}

void SvcSessionManager::SetPublishFlag(const std::string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return;
    }
    it->second.isInPublishFile = true;
    HILOGE("Set PublishFile success, bundleName = %{public}s", bundleName.c_str());
}

void SvcSessionManager::SetOldBackupVersion(const std::string &backupVersion)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("Error, No caller token was specified");
        return;
    }
    impl_.oldBackupVersion = backupVersion;
}

std::string SvcSessionManager::GetOldBackupVersion()
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("Error, No caller token was specified");
        return "";
    }
    return impl_.oldBackupVersion;
}

void SvcSessionManager::SetImplRestoreType(const RestoreTypeEnum restoreType)
{
    impl_.restoreDataType = restoreType;
}

void SvcSessionManager::SetIsReadyLaunch(const std::string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified, bundleName:%{public}s", bundleName.c_str());
        return;
    }
    auto [findBundleSuc, it] = GetBackupExtNameMap(bundleName);
    if (!findBundleSuc) {
        HILOGE("BackupExtNameMap can not find bundle %{public}s", bundleName.c_str());
        return;
    }
    it->second.isReadyLaunch = true;
    HILOGE("SetIsReadyLaunch success, bundleName = %{public}s", bundleName.c_str());
}
} // namespace OHOS::FileManagement::Backup
