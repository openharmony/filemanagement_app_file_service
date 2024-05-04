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

#include "module_ipc/sa_backup_connection.h"
#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "filemgmt_libhilog.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ErrCode SABackupConnection::ConnectBackupSAExt(std::string bundleName, std::string extension, std::string extInfo)
{
    bundleName_ = bundleName;
    extension_ = extension;
    extInfo_ = extInfo;
    saId_ = std::atoi(bundleName.c_str());
    return LoadBackupSAExt();
}

ErrCode SABackupConnection::DisconnectBackupSAExt()
{
    HILOGI("called begin");
    if (!isLoaded_.load() || !isConnected_.load() || !proxy_) {
        HILOGD("No need to unload, connect first");
        return BError(BError::Codes::OK);
    }
    isConnected_.store(false);
    isLoaded_.store(false);
    proxy_ = nullptr;
    string().swap(bundleName_);
    saId_ = BConstants::BACKUP_DEFAULT_SA_ID;
    reloadNum_.store(0);
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!samgrProxy) {
        HILOGE("SamgrProxy is nullptr");
        return BError(BError::Codes::SA_EXT_ERR_SAMGR);
    }
    int32_t result = samgrProxy->UnloadSystemAbility(saId_);
    if (result != ERR_OK) {
        HILOGE("UnloadSA, UnloadSystemAbility %{public}s result: %{public}d", bundleName_.c_str(), result);
    }
    return BError(BError::Codes::OK);
}

bool SABackupConnection::IsSAExtConnected()
{
    HILOGI("called begin");
    return isConnected_.load();
}

sptr<ILocalAbilityManager> SABackupConnection::GetBackupSAExtProxy()
{
    return proxy_;
}

ErrCode SABackupConnection::LoadBackupSAExt()
{
    if (proxy_ && isConnected_.load()) {
        HILOGI("SA %{public}d is running.", saId_);
        callConnected_(move(bundleName_));
        return BError(BError::Codes::OK);
    }
    if (reloadNum_.load() >= BConstants::BACKUP_SA_RELOAD_MAX) {
        HILOGI("SA %{public}d reload done and return", saId_);
        callDied_(move(bundleName_));
        return BError(BError::Codes::SA_EXT_RELOAD_FAIL);
    }
    vector<ISystemAbilityManager::SaExtensionInfo> saExtentionInfos;
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgrProxy) {
        HILOGE("SamgrProxy is nullptr");
        return BError(BError::Codes::SA_EXT_ERR_SAMGR);
    }
    int32_t ret = samgrProxy->GetRunningSaExtensionInfoList(extension_, saExtentionInfos);
    if (ret != ERR_OK) {
        HILOGE("GetExtensionSaIds err, ret %{public}d.", ret);
        return BError(BError::Codes::SA_EXT_ERR_SAMGR);
    }
    for (ISystemAbilityManager::SaExtensionInfo saExtentionInfo : saExtentionInfos) {
        if (saExtentionInfo.saId == saId_) {
            isConnected_.store(true);
            isLoaded_.store(false);
            HILOGI("Get running sa proxy success.");
            proxy_ = iface_cast<ILocalAbilityManager>(saExtentionInfo.processObj);
            callConnected_(move(bundleName_));
            return BError(BError::Codes::OK);
        }
    }
    return LoadBackupSAExtInner();
}

ErrCode SABackupConnection::LoadBackupSAExtInner()
{
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgrProxy) {
        HILOGE("SamgrProxy is nullptr");
        return BError(BError::Codes::SA_EXT_ERR_SAMGR);
    }
    sptr<SALoadCallback> loadCallback = new SALoadCallback();
    if (loadCallback == nullptr) {
        HILOGE("loadCallback is nullptr");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    int32_t ret = samgrProxy->LoadSystemAbility(saId_, loadCallback);
    if (ret != ERR_OK) {
        HILOGE("Failed to Load systemAbility, systemAbility:%{private}d. ret code:%{public}d", saId_, ret);
        return BError(BError::Codes::SA_EXT_ERR_SAMGR);
    }
    isLoaded_.store(true);
    reloadNum_.store(reloadNum_.load() + 1);
    unique_lock<mutex> lock(mutex_);
    auto waitStatus =
        loadCallback->proxyConVar_.wait_for(lock, std::chrono::milliseconds(BConstants::BACKUP_LOADSA_TIMEOUT_MS),
                                            [loadCallback]() { return loadCallback->isLoadSuccess_.load(); });
    if (!waitStatus) {
        HILOGE("Load sa %{public}d timeout", saId_);
        isConnected_.store(false);
        return LoadBackupSAExt();
    }
    isConnected_.store(true);
    callConnected_(move(bundleName_));
    return LoadBackupSAExt();
}

bool SABackupConnection::InputParaSet(MessageParcel &data)
{
    if (extension_ == BConstants::EXTENSION_BACKUP) {
        if (!data.WriteString(extInfo_)) {
            HILOGE("InputParaSet WriteString failed sa: %{public}d, extInfo: %{public}s, extension: %{public}s",
                saId_, extInfo_.c_str(), extension_.c_str());
            return false;
        }
        return true;
    } else if (extension_ == BConstants::EXTENSION_RESTORE) {
        if (!data.WriteFileDescriptor(fd_) || !data.WriteString(extInfo_)) {
            HILOGE("InputParaSet WriteString failed sa: %{public}d, extension: %{public}s",
                saId_, extension_.c_str());
            return false;
        }
        return true;
    }

    HILOGD("SABackupExtentionPara InFunc sa: %{public}d, extension: %{public}s", saId_, extension_.c_str());
    return false;
}

bool SABackupConnection::OutputParaGet(MessageParcel &reply)
{
    parcel_.ClearFileDescriptor();
    if (!parcel_.Append(reply)) {
        HILOGE("OutputParaGet append failed sa is %{public}d, extension: %{public}s", saId_, extension_.c_str());
        return false;
    }
    HILOGD("SABackupExtentionPara OutFunc sa is %{public}d, extension: %{public}s", saId_, extension_.c_str());
    return true;
}

ErrCode SABackupConnection::CallBackupSA()
{
    auto task = [this]() {
        HILOGI("called begin");
        if (proxy_) {
            HILOGI("SA backup called begin");
            int32_t ret = proxy_->SystemAbilityExtProc(BConstants::EXTENSION_BACKUP, saId_, this);
            HILOGI("SA backup done %{public}d", ret);
            int fd = parcel_.ReadFileDescriptor();
            std::string result = parcel_.ReadString();
            callBackup_(move(bundleName_), move(fd), move(result), move(ret));
        }
    };

    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
    return BError(BError::Codes::OK);
}

ErrCode SABackupConnection::CallRestoreSA(UniqueFd fd)
{
    fd_ = move(fd);
    auto task = [this]() {
        HILOGI("called begin");
        if (proxy_) {
            HILOGI("SA restore called begin");
            int32_t ret = proxy_->SystemAbilityExtProc(BConstants::EXTENSION_RESTORE, saId_, this);
            HILOGI("SA restore done %{public}d", ret);
            std::string result = parcel_.ReadString();
            callRestore_(move(bundleName_), move(result), move(ret));
        }
    };

    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
    return BError(BError::Codes::OK);
}

void SABackupConnection::SALoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
                                                                    const OHOS::sptr<IRemoteObject> &remoteObject)
{
    HILOGI("Load backup sa success, systemAbilityId: %{private}d, remoteObject result:%{private}s", systemAbilityId,
           (remoteObject != nullptr) ? "true" : "false");
    if (remoteObject == nullptr) {
        isLoadSuccess_.store(false);
        proxyConVar_.notify_one();
        return;
    }
    isLoadSuccess_.store(true);
    proxyConVar_.notify_one();
}

void SABackupConnection::SALoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    HILOGE("Load backup sa failed, systemAbilityId:%{private}d", systemAbilityId);
    isLoadSuccess_.store(false);
    proxyConVar_.notify_one();
}
} // namespace OHOS::FileManagement::Backup
