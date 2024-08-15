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

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "b_json/b_json_entity_caps.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_resources/b_constants.h"
#include "b_sa/b_sa_utils.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/service.h"
#include "module_ipc/svc_restore_deps_manager.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void SvcSessionManager::VerifyCallerAndScenario(uint32_t clientToken, IServiceReverse::Scenario scenario) const
{
    shared_lock<shared_mutex> lock(lock_);
    if (impl_.scenario != scenario) {
        throw BError(BError::Codes::SDK_MIXED_SCENARIO);
    }
    if (impl_.clientToken != clientToken) {
        throw BError(BError::Codes::SA_REFUSED_ACT, "Caller mismatched");
    }
    HILOGD("Succeed to verify the caller");
}

SvcSessionManager::Impl SvcSessionManager::GetImpl()
{
    return impl_;
}

int SvcSessionManager::GetSessionCnt()
{
    return sessionCnt_.load();
}

ErrCode SvcSessionManager::Active(Impl newImpl)
{
    unique_lock<shared_mutex> lock(lock_);
    const Impl &oldImpl = impl_;
    if (oldImpl.clientToken) {
        HILOGE("Already have an active session");
        return BError(BError::Codes::SA_REFUSED_ACT);
    }

    if (!newImpl.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    if (newImpl.scenario == IServiceReverse::Scenario::UNDEFINED) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No scenario was specified");
    }

    InitClient(newImpl);
    impl_ = newImpl;
    unloadSAFlag_ = false;
    return BError(BError::Codes::OK);
}

void SvcSessionManager::Deactive(const wptr<IRemoteObject> &remoteInAction, bool force)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken || !impl_.clientProxy) {
        HILOGI("Empty session");
        return;
    }
    if (!force && (!impl_.clientToken || !impl_.clientProxy)) {
        throw BError(BError::Codes::SA_REFUSED_ACT, "Try to deactive an empty session");
    }
    auto remoteHeldByProxy = impl_.clientProxy->AsObject();
    if (!force && (remoteInAction != remoteHeldByProxy)) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Only the client actived the session can deactive it");
    }

    deathRecipient_ = nullptr;
    HILOGI("Succeed to deactive a session");
    impl_ = {};
    unloadSAFlag_ = true;
    extConnectNum_ = 0;
}

void SvcSessionManager::VerifyBundleName(string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    auto asVerify = [&bundleName](const auto &it) { return it.first == bundleName; };
    if (none_of(impl_.backupExtNameMap.begin(), impl_.backupExtNameMap.end(), asVerify)) {
        stringstream ss;
        ss << "Could not find the " << bundleName << " from current session";
        throw BError(BError::Codes::SA_REFUSED_ACT, ss.str());
    }
    HILOGD("Succeed to verify the bundleName");
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
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
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

bool SvcSessionManager::OnBundleFileReady(const string &bundleName, const string &fileName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    HILOGD("Begin, bundleName name is:%{public}s", bundleName.c_str());
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        stringstream ss;
        ss << "Could not find the " << bundleName << " from current session";
        throw BError(BError::Codes::SA_REFUSED_ACT, ss.str());
    }

    // 判断是否结束 通知EXTENTION清理资源  TOOL应用完成备份
    if (impl_.scenario == IServiceReverse::Scenario::RESTORE || SAUtils::IsSABundleName(bundleName)) {
        it->second.isBundleFinished = true;
        return true;
    } else if (impl_.scenario == IServiceReverse::Scenario::BACKUP) {
        if (!fileName.empty() && fileName != BConstants::EXT_BACKUP_MANAGE) {
            auto ret = it->second.fileNameInfo.insert(fileName);
            if (!ret.second) {
                it->second.fileNameInfo.erase(fileName);
            }
        } else if (fileName.empty()) {
            it->second.receExtAppDone = true;
        }
        if (it->second.receExtManageJson && it->second.fileNameInfo.empty() && it->second.receExtAppDone) {
            HILOGI("The bundle manage json info and file info support current app done");
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
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    if (impl_.scenario != IServiceReverse::Scenario::BACKUP) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid Scenario");
    }

    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManage();

    for (auto &fileName : info) {
        HILOGE("fileName %{public}s", fileName.data());
        OnBundleFileReady(bundleName, fileName);
    }

    unique_lock<shared_mutex> lock(lock_);
    auto it = GetBackupExtNameMap(bundleName);
    it->second.receExtManageJson = true;
    return move(cachedEntity.GetFd());
}

void SvcSessionManager::RemoveExtInfo(const string &bundleName)
{
    HILOGD("svcMrg:RemoveExt, bundleName:%{public}s", bundleName.c_str());
    unique_lock<shared_mutex> lock(lock_);
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return;
    }
    impl_.backupExtNameMap.erase(it);
    if (extConnectNum_) {
        extConnectNum_--;
    }
}

wptr<SvcBackupConnection> SvcSessionManager::GetExtConnection(const BundleName &bundleName)
{
    HILOGD("svcMrg:GetExt, bundleName:%{public}s", bundleName.c_str());
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    if (!it->second.backUpConnection) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Svc backup connection is empty");
    }

    return wptr(it->second.backUpConnection);
}

std::weak_ptr<SABackupConnection> SvcSessionManager::GetSAExtConnection(const BundleName &bundleName)
{
    HILOGD("svcMrg:GetExt, bundleName:%{public}s", bundleName.c_str());
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    if (!it->second.saBackupConnection) {
        throw BError(BError::Codes::SA_INVAL_ARG, "SA backup connection is empty");
    }

    return std::weak_ptr<SABackupConnection>(it->second.saBackupConnection);
}

sptr<SvcBackupConnection> SvcSessionManager::GetBackupAbilityExt(const string &bundleName)
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

    return sptr<SvcBackupConnection>(new SvcBackupConnection(callDied, callConnected));
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

void SvcSessionManager::InitClient(Impl &newImpl)
{
    if (!newImpl.clientProxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid client");
    }
    auto remoteObj = newImpl.clientProxy->AsObject();
    if (!remoteObj) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Proxy's remote object can't be nullptr");
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
    HILOGI("Succeed to active a session");
}

void SvcSessionManager::SetExtFileNameRequest(const string &bundleName, const string &fileName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    it->second.fileNameInfo.insert(fileName);
}

std::set<std::string> SvcSessionManager::GetExtFileNameRequest(const std::string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    if (impl_.scenario != IServiceReverse::Scenario::RESTORE) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid Scenario");
    }
    auto it = GetBackupExtNameMap(bundleName);
    set<string> fileNameInfo = it->second.fileNameInfo;
    it->second.fileNameInfo.clear();
    return fileNameInfo;
}

map<BundleName, BackupExtInfo>::iterator SvcSessionManager::GetBackupExtNameMap(const string &bundleName)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        stringstream ss;
        ss << "Could not find the " << bundleName << " from current session";
        throw BError(BError::Codes::SA_REFUSED_ACT, ss.str());
    }
    return it;
}

bool SvcSessionManager::GetSchedBundleName(string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (extConnectNum_ >= BConstants::EXT_CONNECT_MAX_COUNT) {
        return false;
    }

    for (auto &&it : impl_.backupExtNameMap) {
        if (it.second.schedAction == BConstants::ServiceSchedAction::WAIT) {
            bundleName = it.first;
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
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    auto it = GetBackupExtNameMap(bundleName);
    return it->second.schedAction;
}

void SvcSessionManager::SetServiceSchedAction(const string &bundleName, BConstants::ServiceSchedAction action)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    it->second.schedAction = action;
    if (it->second.schedAction == BConstants::ServiceSchedAction::START) {
        extConnectNum_++;
    }
}

void SvcSessionManager::SetBackupExtName(const string &bundleName, const string &backupExtName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    it->second.backupExtName = backupExtName;
}

string SvcSessionManager::GetBackupExtName(const string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    return it->second.backupExtName;
}

void SvcSessionManager::SetBackupExtInfo(const string &bundleName, const string &extInfo)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    it->second.extInfo = extInfo;
}

std::string SvcSessionManager::GetBackupExtInfo(const string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    return it->second.extInfo;
}

void SvcSessionManager::AppendBundles(const vector<BundleName> &bundleNames)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    for (auto &&bundleName : bundleNames) {
        HILOGD("bundleName: %{public}s", bundleName.c_str());
        BackupExtInfo info {};
        if (SAUtils::IsSABundleName(bundleName)) {
            info.saBackupConnection = GetBackupSAExt(bundleName);
        } else {
            info.backUpConnection = GetBackupAbilityExt(bundleName);
        }
        impl_.backupExtNameMap.insert(make_pair(bundleName, info));
    }
    impl_.isBackupStart = true;
    impl_.isAppendFinish = true;
}

sptr<SvcBackupConnection> SvcSessionManager::CreateBackupConnection(BundleName &bundleName)
{
    HILOGD("SvcSessionManager::CreateBackupConnection begin.");
    return GetBackupAbilityExt(bundleName);
}

void SvcSessionManager::Start()
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    impl_.isBackupStart = true;
}

void SvcSessionManager::Finish()
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    impl_.isAppendFinish = true;
}

bool SvcSessionManager::IsOnAllBundlesFinished()
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
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
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
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
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    it->second.restoreType = restoreType;
    impl_.restoreDataType = restoreType;
}

RestoreTypeEnum SvcSessionManager::GetBundleRestoreType(const std::string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    return it->second.restoreType;
}

void SvcSessionManager::SetBundleVersionCode(const std::string &bundleName, int64_t versionCode)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    it->second.versionCode = versionCode;
}

int64_t SvcSessionManager::GetBundleVersionCode(const std::string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    return it->second.versionCode;
}

void SvcSessionManager::SetBundleVersionName(const std::string &bundleName, std::string versionName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    it->second.versionName = versionName;
}

std::string SvcSessionManager::GetBundleVersionName(const std::string &bundleName)
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    return it->second.versionName;
}

void SvcSessionManager::SetBundleDataSize(const std::string &bundleName, int64_t dataSize)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
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

    try {
        auto it = GetBackupExtNameMap(bundleName);
        int64_t appSize = it->second.dataSize;
        /* timeout = (AppSize / 3Ms) * 3 + 30 */
        timeout = defaultTimeout + (appSize / processRate) * multiple;
    } catch (const BError &e) {
        HILOGE("Failed to get app<%{public}s> dataInfo, err=%{public}d", bundleName.c_str(), e.GetCode());
        timeout = defaultTimeout;
    }
    timeout = timeout < minTimeout ? minTimeout : timeout;
    resTimeoutMs = (uint32_t)(timeout * invertMillisecond % UINT_MAX); /* conver second to millisecond */
    HILOGI("Calculate App extension process run timeout=%{public}u(us), bundleName=%{public}s ", resTimeoutMs,
           bundleName.c_str());
    return resTimeoutMs;
}

void SvcSessionManager::BundleExtTimerStart(const std::string &bundleName, const Utils::Timer::TimerCallback &callback)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    uint32_t timeout = CalAppProcessTime(bundleName);

    auto it = GetBackupExtNameMap(bundleName);
    if (it->second.timerStatus == false) {
        it->second.timerStatus = true;
        it->second.extTimerId = extBundleTimer.Register(callback, timeout, true);
    }
}

bool SvcSessionManager::UpdateTimer(const std::string &bundleName, uint32_t timeOut,
    const Utils::Timer::TimerCallback &callback)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    if (it->second.timerStatus == true) {
        // 定时器已存在，则先销毁，再重新注册
        it->second.timerStatus = false;
        extBundleTimer.Unregister(it->second.extTimerId);
        HILOGI("UpdateTimer timeout=%{public}u(ms), bundleName=%{public}s ",
            timeOut, bundleName.c_str());
        it->second.extTimerId = extBundleTimer.Register(callback, timeOut, true);
        it->second.timerStatus = true;
        return true;
    }
    return false;
}

void SvcSessionManager::BundleExtTimerStop(const std::string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    if (it->second.timerStatus == true) {
        it->second.timerStatus = false;
        extBundleTimer.Unregister(it->second.extTimerId);
    }
}

void SvcSessionManager::IncreaseSessionCnt()
{
    sessionCnt_++;
}

void SvcSessionManager::DecreaseSessionCnt()
{
    unique_lock<shared_mutex> lock(lock_);
    if (sessionCnt_.load() > 0) {
        sessionCnt_--;
    } else {
        HILOGE("Invalid sessionCount.");
        return;
    }
    if (reversePtr_ == nullptr) {
        HILOGE("Service reverse pointer is empty.");
        return;
    }
    if (sessionCnt_.load() <= 0 && unloadSAFlag_ == true) {
        HILOGI("do unload Service.");
        reversePtr_->UnloadService();
    }
}

void SvcSessionManager::ClearSessionData()
{
    unique_lock<shared_mutex> lock(lock_);
    for (auto &&it : impl_.backupExtNameMap) {
        // clear timer
        if (it.second.timerStatus == true) {
            it.second.timerStatus = false;
            extBundleTimer.Unregister(it.second.extTimerId);
        }
        // disconnect extension
        if (it.second.schedAction == BConstants::ServiceSchedAction::RUNNING) {
            auto backUpConnection = it.second.backUpConnection;
            if (backUpConnection == nullptr) {
                HILOGE("Clear session error, backUpConnection is empty");
                return;
            }
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (proxy == nullptr) {
                HILOGE("Clear session error, proxy is empty");
                return;
            }
            if (impl_.restoreDataType != RestoreTypeEnum::RESTORE_DATA_READDY) {
                proxy->HandleClear();
            }
            backUpConnection->DisconnectBackupExtAbility();
        }
        // clear data
        it.second.schedAction = BConstants::ServiceSchedAction::FINISH;
    }
    impl_.backupExtNameMap.clear();
}

bool SvcSessionManager::GetIsIncrementalBackup()
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    return impl_.isIncrementalBackup;
}

void SvcSessionManager::SetIncrementalData(const BIncrementalData &incrementalData)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    auto it = GetBackupExtNameMap(incrementalData.bundleName);
    it->second.lastIncrementalTime = incrementalData.lastIncrementalTime;
    it->second.manifestFd = incrementalData.manifestFd;
    it->second.backupParameters = incrementalData.backupParameters;
    it->second.backupPriority = incrementalData.backupPriority;
}

int32_t SvcSessionManager::GetIncrementalManifestFd(const string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    auto it = GetBackupExtNameMap(bundleName);
    return it->second.manifestFd;
}

int64_t SvcSessionManager::GetLastIncrementalTime(const string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    auto it = GetBackupExtNameMap(bundleName);
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

bool SvcSessionManager::ValidRestoreDataType(RestoreTypeEnum restoreDataType)
{
    return impl_.restoreDataType == restoreDataType;
}
} // namespace OHOS::FileManagement::Backup
