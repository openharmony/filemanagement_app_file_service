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

#include <chrono>
#include <iomanip>

#include "ability_manager_client.h"
#include "filemgmt_libhilog.h"
#include "hisysevent.h"
#include "module_ipc/svc_extension_proxy.h"
#include "module_ipc/svc_session_manager.h"

namespace OHOS::FileManagement::Backup {
constexpr int WAIT_TIME = 3;
constexpr int32_t INDEX = 3;
constexpr int32_t MS_1000 = 1000;
const std::string FILE_BACKUP_EVENTS = "FILE_BACKUP_EVENTS";
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
    HILOGI("%{public}s, OnAbilityConnectDone", bundleName.c_str());
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    std::stringstream strTime;
    strTime << (std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S:")) << (std::setfill('0'))
        << (std::setw(INDEX)) << (ms.count() % MS_1000);
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        FILE_BACKUP_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "PROC_NAME", "ohos.appfileservice",
        "BUNDLENAME", bundleName,
        "PID", getpid(),
        "TIME", strTime.str()
    );
    if (bundleNameIndexInfo_.find(bundleName) == string::npos) {
        HILOGE("Current bundle name is wrong, bundleNameIndexInfo:%{public}s, bundleName:%{public}s",
            bundleNameIndexInfo_.c_str(), bundleName.c_str());
        return;
    }
    callConnected_(move(bundleNameIndexInfo_));
    HILOGI("called end");
}

void SvcBackupConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    HILOGI("called begin");
    isConnected_.store(false);
    backupProxy_ = nullptr;
    string bundleName = element.GetBundleName();
    if (isConnectedDone_ == false) {
        isConnectedDone_.store(true);
        HILOGE("It's error that the backup extension dies before the backup sa. name : %{public}s", bundleName.data());
        callDied_(move(bundleName));
    }
    condition_.notify_all();
    waitCondition_.notify_all();
    HILOGI("called end, name: %{public}s", bundleName.c_str());
}

ErrCode SvcBackupConnection::ConnectBackupExtAbility(AAFwk::Want &want, int32_t userId)
{
    HILOGI("Called begin");
    std::unique_lock<std::mutex> lock(mutex_);
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, this, userId);
    HILOGI("Called end, ret=%{public}d, userId=%{public}d.", ret, userId);
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

bool SvcBackupConnection::WaitDisconnectDone()
{
    std::unique_lock<std::mutex> lock(waitMutex_);
    if (waitCondition_.wait_for(lock, std::chrono::seconds(WAIT_TIME),
        [this]() { return isConnected_.load() == false; })) {
        HILOGI("Wait disconnected done success");
        return true;
    }
    return false;
}

bool SvcBackupConnection::IsExtAbilityConnected()
{
    return isConnected_.load();
}

sptr<IExtension> SvcBackupConnection::GetBackupExtProxy()
{
    return backupProxy_;
}

void SvcBackupConnection::SetCallback(function<void(const std::string &&)> callConnected)
{
    callConnected_ = callConnected;
}

void SvcBackupConnection::SetCallDied(function<void(const std::string &&)> callDied)
{
    callDied_ = callDied;
}
} // namespace OHOS::FileManagement::Backup