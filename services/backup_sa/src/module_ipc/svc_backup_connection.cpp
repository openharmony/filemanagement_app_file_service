/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "module_ipc/svc_backup_connection.h"

#include <chrono>

#include "ability_manager_client.h"
#include "b_utils/b_time.h"
#include "filemgmt_libhilog.h"
#include "hisysevent.h"
#include "iextension.h"
#include "module_ipc/svc_session_manager.h"

namespace OHOS::FileManagement::Backup {
constexpr int WAIT_TIME = 3;
constexpr int WAIT_DEATH_TIME_MS = 500;
using namespace std;

void SvcBackupConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
                                               const sptr<IRemoteObject> &remoteObject,
                                               int resultCode)
{
    string bundleName = element.GetBundleName();
    HILOGI("OnAbilityConnectDone, bundleName:%{public}s, bundleNameIndexInfo:%{public}s", bundleName.c_str(),
           bundleNameIndexInfo_.c_str());
    connectSpend_.End();
    if (remoteObject == nullptr) {
        HILOGE("Failed to ability connect done, remote is nullptr");
        return;
    }
    std::function<void(const std::string &)> onConnectedCb;
    sptr<IRemoteObject> remoteObj;
    sptr<IRemoteObject::DeathRecipient> recipient;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (connState_ != ConnState::CONNECTING) {
            HILOGI("State already changed to %{public}d, skip on-connected, name:%{public}s",
                   static_cast<int>(connState_), bundleNameIndexInfo_.c_str());
            return;
        }
        backupProxy_ = iface_cast<IExtension>(remoteObject);
        if (backupProxy_ == nullptr) {
            HILOGE("Failed to ability connect done, iface_cast returns nullptr, name:%{public}s",
                   bundleNameIndexInfo_.c_str());
            backupProxy_ = nullptr;
            return;
        }
        PrepareDeathRecipientLocked();
        recipient = deathRecipient_;
        remoteObj = backupProxy_->AsObject();
        connState_ = ConnState::CONNECTED;
        onConnectedCb = onConnectedCb_;
        onConnectedCb_ = nullptr;
    }

    // AddDeathRecipient 是 IPC 框架远程调用，在 mutex_ 外执行以避免持锁进入 binder 内部锁。
    // 若对象已死亡则返回 false（框架不会同步回调）；若期间连接被拆除，回调内 promote/谓词检查会安全跳过。
    if (remoteObj != nullptr && recipient != nullptr && !remoteObj->AddDeathRecipient(recipient)) {
        HILOGE("Failed to add death recipient, name:%{public}s", bundleNameIndexInfo_.c_str());
    }
    if (onConnectedCb) {
        try {
            onConnectedCb(bundleNameIndexInfo_);
        } catch (...) {
            HILOGE("OnConnectedCb throw exception, name:%{public}s", bundleNameIndexInfo_.c_str());
        }
    }
    HILOGI("called end, name: %{public}s", bundleNameIndexInfo_.c_str());
}

void SvcBackupConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    string bundleName = element.GetBundleName();
    HILOGI("OnAbilityDisconnectDone, bundleName:%{public}s, bundleNameIndexInfo:%{public}s", bundleName.c_str(),
           bundleNameIndexInfo_.c_str());
    std::function<void(const std::string &, bool)> onDiedCb;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        SnapshotOnDiedCbLocked(onDiedCb, resultCode, true);
    }
    InvokeOnDiedCb(onDiedCb);
    connCond_.notify_all();
    disconnectCond_.notify_all();
    HILOGI("called end, name: %{public}s", bundleNameIndexInfo_.c_str());
}

void SvcBackupConnection::OnExtensionDied(const wptr<IRemoteObject> &remoteObject)
{
    HILOGI("called begin, bundleNameIndexInfo:%{public}s, wait for OnAbilityDisconnectDone or %{public}dms timeout",
           bundleNameIndexInfo_.c_str(), WAIT_DEATH_TIME_MS);
    // 等待 OnAbilityDisconnectDone 到达，优先由其携带 resultCode 处理 onDiedCb_
    std::function<void(const std::string &, bool)> onDiedCb;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        disconnectCond_.wait_for(lock, std::chrono::milliseconds(WAIT_DEATH_TIME_MS), [this]() { return !onDiedCb_; });
        if (!onDiedCb_) {
            HILOGI("OnAbilityDisconnectDone already handled, skip, name:%{public}s", bundleNameIndexInfo_.c_str());
            return;
        }
        SnapshotOnDiedCbLocked(onDiedCb, 0, false);
    }
    InvokeOnDiedCb(onDiedCb);
    connCond_.notify_all();
    HILOGI("called end, name:%{public}s", bundleNameIndexInfo_.c_str());
}

ErrCode SvcBackupConnection::ConnectBackupExtAbility(AAFwk::Want &want, int32_t userId, bool isCleanMode)
{
    int32_t amsTimeout = TimeUtils::GetAmsTimeout();
    HILOGI("Called begin, bundleNameIndexInfo:%{public}s, isCleanMode:%{public}d, timeout:%{public}d",
           bundleNameIndexInfo_.c_str(), isCleanMode, amsTimeout);
    std::unique_lock<std::mutex> lock(mutex_);
    if (connState_ != ConnState::IDLE) {
        HILOGE("Connection already used, cannot reuse, name:%{public}s", bundleNameIndexInfo_.c_str());
        return BError(BError::Codes::SA_REFUSED_ACT).GetCode();
    }
    connState_ = ConnState::CONNECTING;
    isCleanMode_ = isCleanMode;
    connectSpend_.Start();
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, this, userId, amsTimeout);
    HILOGI("Called end, ret:%{public}d, userId:%{public}d.", ret, userId);
    return ret;
}

ErrCode SvcBackupConnection::DisconnectBackupExtAbility()
{
    HILOGI("called begin, bundleNameIndexInfo:%{public}s", bundleNameIndexInfo_.c_str());
    std::unique_lock<std::mutex> lock(mutex_);
    onDiedCb_ = nullptr;
    // 主动清空 onDiedCb_ 后唤醒可能在 disconnectCond_ 上等待的 OnExtensionDied，
    // 否则其 wait_for 仅靠 500ms 超时退出，会无意义阻塞 binder 线程。
    disconnectCond_.notify_all();
    ErrCode ret = AppExecFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(this);
    auto callback = [extConn {wptr(this)}] {
        auto extPtr = extConn.promote();
        if (!extPtr) {
            HILOGE("Dis connect failed");
            return false;
        }
        return extPtr->connState_ != ConnState::CONNECTED;
    };
    if (connCond_.wait_for(lock, std::chrono::seconds(WAIT_TIME), callback)) {
        HILOGI("Wait until the connection ends");
    }
    HILOGI("called end, ret:%{public}d, name: %{public}s", ret, bundleNameIndexInfo_.c_str());
    return ret;
}

bool SvcBackupConnection::IsExtAbilityConnected()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return connState_ == ConnState::CONNECTED;
}

bool SvcBackupConnection::GetWasEverConnected()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return connState_ == ConnState::CONNECTED || connState_ == ConnState::DISCONNECTED;
}

sptr<IExtension> SvcBackupConnection::GetBackupExtProxy()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (connState_ != ConnState::CONNECTED || backupProxy_ == nullptr) {
        return nullptr;
    }
    return backupProxy_;
}

bool SvcBackupConnection::WaitDisconnectDone()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (connCond_.wait_for(lock, std::chrono::seconds(WAIT_TIME),
                           [this]() { return connState_ != ConnState::CONNECTED; })) {
        HILOGI("Wait disconnected done success");
        return true;
    }
    return false;
}

void SvcBackupConnection::SetOnConnectedCb(function<void(const std::string &)> onConnectedCb)
{
    std::unique_lock<std::mutex> lock(mutex_);
    onConnectedCb_ = onConnectedCb;
}

void SvcBackupConnection::SetOnDiedCb(function<void(const std::string &, bool)> onDiedCb)
{
    std::unique_lock<std::mutex> lock(mutex_);
    onDiedCb_ = onDiedCb;
}

void SvcBackupConnection::SnapshotOnDiedCbLocked(std::function<void(const std::string &, bool)> &onDiedCb,
                                                 int errCode,
                                                 bool removeDeathRecipient)
{
    bool wasEverConnected = (connState_ == ConnState::CONNECTED);
    connState_ = wasEverConnected ? ConnState::DISCONNECTED : ConnState::FAILED;
    if (removeDeathRecipient) {
        RemoveDeathRecipientLocked();
    }
    backupProxy_ = nullptr;
    onDiedCb = onDiedCb_;
    onDiedCb_ = nullptr;
    if (!wasEverConnected) {
        connectSpend_.End();
    }
    // 仅当存在死亡回调时才生成错误码：主动断开（DisconnectBackupExtAbility 已清空 onDiedCb_）
    // 或已被对端处理过的不应覆盖 error_。
    if (onDiedCb) {
        GenErrorByStatus(errCode, wasEverConnected, isCleanMode_);
    }
}

void SvcBackupConnection::InvokeOnDiedCb(std::function<void(const std::string &, bool)> &onDiedCb)
{
    if (!onDiedCb) {
        return;
    }
    HILOGE("Backup extension died, name:%{public}s, isCleanCalled:%{public}d", bundleNameIndexInfo_.c_str(),
           isCleanMode_);
    try {
        onDiedCb(bundleNameIndexInfo_, isCleanMode_);
    } catch (...) {
        HILOGE("OnDiedCb throw exception, name:%{public}s", bundleNameIndexInfo_.c_str());
    }
}

void SvcBackupConnection::GenErrorByStatus(int errCode, bool wasEverConnected, bool isCleanMode)
{
    HILOGI("GenErrorByStatus");
    std::unique_lock<std::shared_mutex> lock(errMutex_);
    if (wasEverConnected) {
        string errMsg = "{\"connectSpend\": " + to_string(connectSpend_.GetSpan()) +
                        ", \"connectStartTime\":" + to_string(connectSpend_.startMilli_) +
                        ",\"isCleanCalled\":" + to_string(isCleanMode) + "}";
        error_ = BError(errCode, BError::Codes::EXT_ABILITY_DIED, errMsg);
    } else {
        uint32_t failSpend = connectSpend_.GetSpan();
        string errMsg =
            "{\"failSpend\": " + to_string(failSpend) + ",\"isCleanCalled\":" + to_string(isCleanMode) + "}";
        if (failSpend >= TimeUtils::GetAmsTimeout() * SECOND_TO_MS) {
            error_ = BError(errCode, BError::Codes::SA_BOOT_EXT_TIMEOUT, errMsg);
        } else {
            error_ = BError(errCode, BError::Codes::EXT_ABILITY_DIED, errMsg);
        }
    }
}

void SvcBackupConnection::PrepareDeathRecipientLocked()
{
    if (backupProxy_ == nullptr) {
        return;
    }
    auto deathCallback = [extConn {wptr(this)}](const wptr<IRemoteObject> &object) {
        auto extPtr = extConn.promote();
        if (!extPtr) {
            HILOGE("Backup connection has been destroyed, skip death notification");
            return;
        }
        extPtr->OnExtensionDied(object);
    };
    RemoveDeathRecipientLocked();
    deathRecipient_ = sptr(new SvcDeathRecipient(deathCallback));
}

void SvcBackupConnection::RemoveDeathRecipientLocked()
{
    if (backupProxy_ == nullptr || deathRecipient_ == nullptr) {
        return;
    }
    sptr<IRemoteObject> remoteObj = backupProxy_->AsObject();
    if (remoteObj == nullptr) {
        deathRecipient_ = nullptr;
        return;
    }
    if (!remoteObj->IsObjectDead() && !remoteObj->RemoveDeathRecipient(deathRecipient_)) {
        HILOGE("Failed to remove death recipient, name:%{public}s", bundleNameIndexInfo_.c_str());
    }
    deathRecipient_ = nullptr;
}

} // namespace OHOS::FileManagement::Backup
