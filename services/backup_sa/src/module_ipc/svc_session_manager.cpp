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

void SvcSessionManager::VerifyCallerAndScenario(uint32_t clientToken, IServiceReverse::Scenario scenario) const
{
    shared_lock<shared_mutex> lock(lock_);
    if (impl_.scenario != scenario) {
        HILOGE("Inconsistent scenario, impl scenario:%{public}d", impl_.scenario);
        AppRadar::Info info("", "", "Inconsistent scenario");
        AppRadar::GetInstance().RecordDefaultFuncRes(info, "SvcSessionManager::VerifyCallerAndScenario", impl_.userId,
                                                     BizStageBackup::BIZ_STAGE_PERMISSION_CHECK,
                                                     BError(BError::Codes::SDK_MIXED_SCENARIO).GetCode());
        throw BError(BError::Codes::SDK_MIXED_SCENARIO);
    }
    if (impl_.clientToken != clientToken) {
        AppRadar::Info info2("", "", "Caller mismatched");
        AppRadar::GetInstance().RecordDefaultFuncRes(info2, "SvcSessionManager::VerifyCallerAndScenario", impl_.userId,
                                                     BizStageBackup::BIZ_STAGE_PERMISSION_CHECK,
                                                     BError(BError::Codes::SDK_MIXED_SCENARIO).GetCode());
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

ErrCode SvcSessionManager::Active(Impl newImpl, bool force)
{
    unique_lock<shared_mutex> lock(lock_);
    const Impl &oldImpl = impl_;
    if (oldImpl.clientToken) {
        HILOGE("Already have an active session");
        return BError(BError::Codes::SA_REFUSED_ACT);
    }

    if (!force && !newImpl.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    if (!force && newImpl.scenario == IServiceReverse::Scenario::UNDEFINED) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No scenario was specified");
    }

    if (!force) {
        InitClient(newImpl);
    }
    impl_ = newImpl;
    IncreaseSessionCnt(__PRETTY_FUNCTION__);
    return BError(BError::Codes::OK);
}

void SvcSessionManager::Deactive(const wptr<IRemoteObject> &remoteInAction, bool force)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        HILOGI("Empty session");
        return;
    }
    if (!force && (!impl_.clientToken || !impl_.clientProxy)) {
        return;
    }
    if (!force && (remoteInAction != impl_.clientProxy->AsObject())) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Only the client actived the session can deactive it");
    }

    deathRecipient_ = nullptr;
    HILOGI("Succeed to deactive a session");
    impl_ = {};
    extConnectNum_ = 0;
    DecreaseSessionCnt(__PRETTY_FUNCTION__);
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
        HILOGE("fileName %{public}s", GetAnonyString(fileName).data());
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
        HILOGI("BackupExtNameMap not contain %{public}s", bundleName.c_str());
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
        AppRadar::Info info ("", "", "client died");
        AppRadar::GetInstance().RecordDefaultFuncRes(info, "SvcSessionManager::InitClient",
                                                     AppRadar::GetInstance().GetUserId(),
                                                     BizStageBackup::BIZ_STAGE_CLIENT_STATUS,
                                                     BError(BError::Codes::SA_BROKEN_IPC).GetCode());
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
        auto it = impl_.backupExtNameMap.find(bundleName);
        if (it != impl_.backupExtNameMap.end()) {
            HILOGI("BackupExtNameMap already contain %{public}s", bundleName.c_str());
            info.backUpConnection = impl_.backupExtNameMap[bundleName].backUpConnection;
            info.saBackupConnection = impl_.backupExtNameMap[bundleName].saBackupConnection;
            info.appendNum = impl_.backupExtNameMap[bundleName].appendNum + 1;
            impl_.backupExtNameMap[bundleName] = info;
            continue;
        }
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
    auto it = GetBackupExtNameMap(bundleName);
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
    auto it = GetBackupExtNameMap(bundleName);
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
    auto it = GetBackupExtNameMap(bundleName);
    if (it->second.extTimerStatus == true) {
        HILOGE("ExtTimer is registered, unregister first.");
        return false;
    }
    uint32_t timeout = it->second.timeCount;
    timeout = (timeout != 0) ? timeout : CalAppProcessTime(bundleName);
    it->second.extTimerStatus = true;
    it->second.startTime = static_cast<uint32_t>(TimeUtils::GetTimeMS());
    it->second.timeCount = timeout;
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
    auto it = GetBackupExtNameMap(bundleName);
    if (it->second.extTimerStatus == false) {
        HILOGE("ExtTimer is unregistered, register first.");
        return true;
    }

    it->second.extTimerStatus = false;
    it->second.startTime = 0;
    it->second.timeCount = 0;
    timer_.Unregister(it->second.timerId);
    HILOGI("StopExtTimer end bundleName %{public}s", bundleName.c_str());
    return true;
}

bool SvcSessionManager::UpdateTimer(const std::string &bundleName, uint32_t timeOut,
    const Utils::Timer::TimerCallback &callback)
{
    unique_lock<shared_mutex> lock(lock_);
    HILOGI("UpdateTimer begin bundleName %{public}s", bundleName.c_str());
    if (!impl_.clientToken) {
        HILOGE("No caller token was specified");
        return false;
    }

    auto it = GetBackupExtNameMap(bundleName);
    it->second.timeCount += timeOut;
    if (it->second.extTimerStatus == false) {
        HILOGI("ExtTimer is unregistered, just count. timeout %{public}u(ms), timeCount %{public}u(ms)",
            timeOut, it->second.timeCount);
        return true;
    }

    if (it->second.startTime == 0) {
        HILOGE("ExtTimer is registered, but start time is zero.");
        return false;
    }
    uint32_t updateTime = static_cast<uint32_t>(TimeUtils::GetTimeMS());
    uint32_t elapseTime = updateTime - it->second.startTime;
    uint32_t realTimeout = it->second.timeCount - elapseTime;
    timer_.Unregister(it->second.timerId);
    HILOGI("UpdateTimer timeout %{public}u(ms), timeCount %{public}u(ms), elapseTime %{public}u(ms),"
        "realTimeout %{public}u(ms), bundleName %{public}s ",
        timeOut, it->second.timeCount, elapseTime, realTimeout, bundleName.c_str());
    it->second.timerId = timer_.Register(callback, realTimeout, true);
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

void SvcSessionManager::SetClearDataFlag(const std::string &bundleName, bool isClearData)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    auto it = GetBackupExtNameMap(bundleName);
    it->second.isClearData = isClearData;
    HILOGI("bundleName:%{public}s, set clear data flag:%{public}d.", bundleName.c_str(), isClearData);
}
bool SvcSessionManager::GetClearDataFlag(const std::string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    auto it = GetBackupExtNameMap(bundleName);
    return it->second.isClearData;
}

bool SvcSessionManager::ValidRestoreDataType(RestoreTypeEnum restoreDataType)
{
    return impl_.restoreDataType == restoreDataType;
}
} // namespace OHOS::FileManagement::Backup
