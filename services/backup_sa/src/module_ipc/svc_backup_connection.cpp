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

#include "module_ipc/svc_backup_connection.h"

#include "ability_manager_client.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/svc_extension_proxy.h"
#include "module_ipc/svc_session_manager.h"

namespace OHOS::FileManagement::Backup {
constexpr int WAIT_TIME = 3;
using namespace std;

void SvcBackupConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
                                               const sptr<IRemoteObject> &remoteObject,
                                               int resultCode)
{
    HILOGI("called begin");
    if (remoteObject == nullptr) {
        HILOGE("Failed to ability connect done, remote is nullptr");
        return;
    }
    backupProxy_ = iface_cast<SvcExtensionProxy>(remoteObject);
    if (backupProxy_ == nullptr) {
        HILOGE("Failed to ability connect done, backupProxy_ is nullptr");
        return;
    }
    isConnected_.store(true);
    string bundleName = element.GetBundleName();
    callConnDone_(move(bundleName));
    HILOGI("called end");
}

void SvcBackupConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    HILOGI("called begin");
    isConnected_.store(false);
    backupProxy_ = nullptr;
    if (isConnectedDone_ == false) {
        isConnectedDone_.store(true);
        string bundleName = element.GetBundleName();
        HILOGE("It's error that the backup extension dies before the backup sa. name : %{public}s", bundleName.data());
        callDied_(move(bundleName));
    }
    condition_.notify_all();
    HILOGI("called end");
}

ErrCode SvcBackupConnection::ConnectBackupExtAbility(AAFwk::Want &want, int32_t userId)
{
    HILOGI("called begin");
    std::unique_lock<std::mutex> lock(mutex_);
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, this, userId);
    HILOGI("called end, ret=%{public}d", ret);
    return ret;
}

ErrCode SvcBackupConnection::DisconnectBackupExtAbility()
{
    HILOGI("called begin");
    isConnectedDone_.store(true);
    std::unique_lock<std::mutex> lock(mutex_);
    ErrCode ret = AppExecFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(this);
    auto callback = [extConn {wptr(this)}] {
        auto extPtr = extConn.promote();
        if (!extPtr) {
            HILOGE("Dis connect failed");
            return false;
        }
        return extPtr->GetBackupExtProxy() == nullptr;
    };
    if (condition_.wait_for(lock, std::chrono::seconds(WAIT_TIME), callback)) {
        HILOGI("Wait until the connection ends");
    }
    HILOGI("called end, ret=%{public}d", ret);
    return ret;
}

bool SvcBackupConnection::IsExtAbilityConnected()
{
    return isConnected_.load();
}

sptr<IExtension> SvcBackupConnection::GetBackupExtProxy()
{
    return backupProxy_;
}

void SvcBackupConnection::SetCallback(function<void(const std::string &&)> callConnDone)
{
    callConnDone_ = callConnDone;
}
} // namespace OHOS::FileManagement::Backup