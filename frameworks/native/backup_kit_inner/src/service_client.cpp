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

#include <sstream>
#include "iremote_object.h"
#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_radar/b_radar.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "service_client.h"
#include "system_ability_definition.h"
#include "svc_death_recipient.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

bool ServiceClient::CheckServiceProxy()
{
    serviceProxy_ = ServiceClient::GetInstance();
    bool isNull = false;
    if (serviceProxy_ != nullptr) {
        isNull = true;
    }
    return isNull;
}

sptr<IService> ServiceClient::GetServiceProxyPointer()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    unique_lock<mutex> lock(proxyMutex_);
    return serviceProxy_;
}

sptr<IService> ServiceClient::GetInstance()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    unique_lock<mutex> getInstanceLock(getInstanceMutex_);
    unique_lock<mutex> lock(proxyMutex_);
    if (serviceProxy_ != nullptr) {
        return serviceProxy_;
    }
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOGE("Get an empty samgr");
        return nullptr;
    }
    sptr<ServiceProxyLoadCallback> loadCallback = sptr(new ServiceProxyLoadCallback());
    if (loadCallback == nullptr) {
        HILOGE("loadCallback is nullptr.");
        return nullptr;
    }
    int32_t ret = samgr->LoadSystemAbility(FILEMANAGEMENT_BACKUP_SERVICE_SA_ID, loadCallback);
    if (ret != ERR_OK) {
        HILOGE("Failed to Load systemAbility, systemAbilityId:%{private}d, ret code:%{public}d",
               FILEMANAGEMENT_BACKUP_SERVICE_SA_ID, ret);
        return nullptr;
    }
    auto waitStatus =
        loadCallback->proxyConVar_.wait_for(lock, std::chrono::milliseconds(BConstants::BACKUP_LOADSA_TIMEOUT_MS),
                                            [loadCallback]() { return loadCallback->isLoadSuccess_.load(); });
    if (!waitStatus) {
        HILOGE("Load backup sa timeout");
        AppRadar::Info info("", "", "\"reason\":\"Load backup sa timeout\"");
        AppRadar::GetInstance().RecordBackupFuncRes(info, "ServiceClient::GetInstance",
            AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_BOOT_BACKUP_SA_FAIL,
            BError(BError::Codes::SA_INVAL_ARG).GetCode());
        return nullptr;
    }
    return serviceProxy_;
}

void ServiceClient::InvaildInstance()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Invalid Instance");
    unique_lock<mutex> lock(proxyMutex_);
    serviceProxy_ = nullptr;
}

void ServiceClient::ServiceProxyLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
                                                                         const OHOS::sptr<IRemoteObject> &remoteObject)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Load backup sa success, systemAbilityId: %{public}d, remoteObject result:%{public}s", systemAbilityId,
           (remoteObject != nullptr) ? "true" : "false");
    if (systemAbilityId != FILEMANAGEMENT_BACKUP_SERVICE_SA_ID || remoteObject == nullptr) {
        isLoadSuccess_.store(false);
        proxyConVar_.notify_one();
        return;
    }
    unique_lock<mutex> lock(proxyMutex_);
    serviceProxy_ = iface_cast<IService>(remoteObject);
    if (serviceProxy_ == nullptr) {
        HILOGE("serviceProxy_ is nullptr");
        return;
    }
    auto remoteObj = serviceProxy_->AsObject();
    if (!remoteObj) {
        HILOGE("Failed to get remote object");
        serviceProxy_ = nullptr;
        isLoadSuccess_.store(false);
        proxyConVar_.notify_one();
        return;
    }
    auto callback = [](const wptr<IRemoteObject> &obj) {
        ServiceClient::InvaildInstance();
    };
    sptr<SvcDeathRecipient> deathRecipient = sptr(new SvcDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient);
    isLoadSuccess_.store(true);
    proxyConVar_.notify_one();
}

void ServiceClient::ServiceProxyLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGE("Load backup sa failed, systemAbilityId:%{private}d", systemAbilityId);
    unique_lock<mutex> lock(proxyMutex_);
    serviceProxy_ = nullptr;
    isLoadSuccess_.store(false);
    AppRadar::Info info("", "", "\"reason\":\"Load backup sa fail\"");
    AppRadar::GetInstance().RecordBackupFuncRes(info, "ServiceProxyLoadCallback::OnLoadSystemAbilityFail",
        AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_BOOT_BACKUP_SA_FAIL,
        static_cast<int32_t>(BError::Codes::SA_INVAL_ARG));
    proxyConVar_.notify_one();
}
} // namespace OHOS::FileManagement::Backup
