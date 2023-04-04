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
#include "filemgmt_libhilog.h"
#include "module_external/bms_adapter.h"
#include "module_ipc/service.h"

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
    HILOGE("Succeed to verify the caller");
}

void SvcSessionManager::Active(Impl newImpl)
{
    unique_lock<shared_mutex> lock(lock_);
    const Impl &oldImpl = impl_;
    if (oldImpl.clientToken) {
        throw BError(BError::Codes::SA_REFUSED_ACT, "Already have an active session");
    }

    if (!newImpl.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    if (newImpl.scenario == IServiceReverse::Scenario::UNDEFINED) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No scenario was specified");
    }

    InitClient(newImpl);
    impl_ = newImpl;
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
    HILOGE("Succeed to verify the bundleName");
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

bool SvcSessionManager::OnBunleFileReady(const string &bundleName, const string &fileName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        stringstream ss;
        ss << "Could not find the " << bundleName << " from current session";
        throw BError(BError::Codes::SA_REFUSED_ACT, ss.str());
    }

    // 判断是否结束 通知EXTENTION清理资源  TOOL应用完成备份
    if (impl_.scenario == IServiceReverse::Scenario::RESTORE) {
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
            it->second.isBundleFinished = true;
            return true;
        }
    }
    return false;
}

UniqueFd SvcSessionManager::OnBunleExtManageInfo(const string &bundleName, UniqueFd fd)
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
        OnBunleFileReady(bundleName, fileName);
    }

    unique_lock<shared_mutex> lock(lock_);
    auto it = GetBackupExtNameMap(bundleName);
    it->second.receExtManageJson = true;
    return move(cachedEntity.GetFd());
}

void SvcSessionManager::RemoveExtInfo(const string &bundleName)
{
    HILOGI("Begin");
    unique_lock<shared_mutex> lock(lock_);
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return;
    }
    impl_.backupExtNameMap.erase(it);
    extConnectNum_--;
}

wptr<SvcBackupConnection> SvcSessionManager::GetExtConnection(const BundleName &bundleName)
{
    HILOGI("Begin");
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

sptr<SvcBackupConnection> SvcSessionManager::GetBackupExtAbility(const string &bundleName)
{
    HILOGI("Begin");
    auto callDied = [revPtr {reversePtr_}](const string &&bundleName) {
        auto revPtrStrong = revPtr.promote();
        if (!revPtrStrong) {
            // 服务先于客户端死亡是一种异常场景，但该场景对本流程来说也没什么影响，所以只是简单记录一下
            HILOGW("It's curious that the backup sa dies before the backup client");
            return;
        }
        revPtrStrong->OnBackupExtensionDied(move(bundleName), ESRCH);
    };

    auto callConnDone = [revPtr {reversePtr_}](const string &&bundleName) {
        auto revPtrStrong = revPtr.promote();
        if (!revPtrStrong) {
            // 服务先于客户端死亡是一种异常场景，但该场景对本流程来说也没什么影响，所以只是简单记录一下
            HILOGW("It's curious that the backup sa dies before the backup client");
            return;
        }
        revPtrStrong->ExtConnectDone(move(bundleName));
    };

    return sptr<SvcBackupConnection>(new SvcBackupConnection(callDied, callConnDone));
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
        (void)revPtrStrong->StopAll(obj);
    };
    deathRecipient_ = sptr(new SvcDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient_);
    HILOGI("Succeed to active a session");
}

void SvcSessionManager::SetExtFileNameRequest(const string &bundleName, const string &fileName)
{
    HILOGI("Begin");
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    it->second.fileNameInfo.insert(fileName);
}

std::set<std::string> SvcSessionManager::GetExtFileNameRequest(const std::string &bundleName)
{
    HILOGI("Begin");
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
    HILOGI("Begin");
    shared_lock<shared_mutex> lock(lock_);
    if (extConnectNum_ >= BConstants::EXT_CONNECT_MAX_COUNT) {
        return false;
    }

    for (auto &&it : impl_.backupExtNameMap) {
        if (it.second.schedAction == BConstants::ServiceSchedAction::WAIT) {
            bundleName = it.first;
            return true;
        }
    }
    return false;
}

BConstants::ServiceSchedAction SvcSessionManager::GetServiceSchedAction(const std::string &bundleName)
{
    HILOGI("Begin");
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    auto it = GetBackupExtNameMap(bundleName);
    return it->second.schedAction;
}

void SvcSessionManager::SetServiceSchedAction(const string &bundleName, BConstants::ServiceSchedAction action)
{
    HILOGI("Begin");
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    it->second.schedAction = action;
    if (it->second.schedAction == BConstants::ServiceSchedAction::INSTALLING) {
        extConnectNum_++;
    }
}

string SvcSessionManager::GetBackupExtName(const string &bundleName)
{
    HILOGI("Begin");
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    string name = bundleName;
    VerifyBundleName(name);
    auto bundleInfos = BundleMgrAdapter::GetBundleInfos({bundleName});
    for (auto &&bundleInfo : bundleInfos) {
        if (!bundleInfo.allToBackup || bundleInfo.extensionName.empty()) {
            throw BError(BError::Codes::SA_INVAL_ARG,
                         string(bundleInfo.name).append(" is not allow to backup or extension name is empty"));
        }
        if (bundleInfo.name == bundleName) {
            return bundleInfo.extensionName;
        }
    }
    throw BError(BError::Codes::SA_INVAL_ARG, "Could not find the extension name");
}

void SvcSessionManager::AppendBundles(const vector<BundleName> &bundleNames)
{
    HILOGI("Begin");
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    
    for (auto &&bundleName : bundleNames) {
        BackupExtInfo info {};
        info.backUpConnection = GetBackupExtAbility(bundleName);
        impl_.backupExtNameMap.insert(make_pair(bundleName, info));
    }
}

void SvcSessionManager::Start()
{
    HILOGI("Begin");
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    impl_.isBackupStart = true;
}

void SvcSessionManager::Finish()
{
    HILOGI("Begin");
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
    auto iter = find_if(impl_.backupExtNameMap.begin(), impl_.backupExtNameMap.end(),
                        [](auto &it) { return it.second.isBundleFinished == false; });
    if (iter == impl_.backupExtNameMap.end() && impl_.isAppendFinish) {
        return true;
    }
    return false;
}

bool SvcSessionManager::IsOnOnStartSched()
{
    shared_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    if (impl_.isBackupStart && impl_.backupExtNameMap.size()) {
        return true;
    }

    return false;
}

void SvcSessionManager::SetInstallState(const string &bundleName, const string &state)
{
    HILOGI("Begin");
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    it->second.installState = state;
}

string SvcSessionManager::GetInstallState(const string &bundleName)
{
    HILOGI("Begin");
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    return it->second.installState;
}

void SvcSessionManager::SetNeedToInstall(const std::string &bundleName, bool needToInstall)
{
    HILOGI("Begin");
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    it->second.bNeedToInstall = needToInstall;
}

bool SvcSessionManager::GetNeedToInstall(const std::string &bundleName)
{
    HILOGI("Begin");
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = GetBackupExtNameMap(bundleName);
    return it->second.bNeedToInstall;
}
} // namespace OHOS::FileManagement::Backup