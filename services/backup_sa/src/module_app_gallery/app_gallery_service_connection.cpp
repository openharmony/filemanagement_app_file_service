/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "module_app_gallery/app_gallery_service_connection.h"

#include <condition_variable>
#include <chrono>

#include "module_external/bms_adapter.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
const int32_t CONNECT_TIME = 3;

void AppGalleryConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    std::string uri = element.GetURI();
    HILOGI("OnAbilityConnectDone, uri = %{public}s", uri.c_str());
    std::unique_lock<std::mutex> uniqueLock(appRemoteObjLock_);
    appRemoteObj_ = remoteObject;
    conditionVal_.notify_one();
}

void AppGalleryConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    std::string uri = element.GetURI();
    HILOGI("OnAbilityDisconnectDone, uri = %{public}s", uri.c_str());
    std::unique_lock<std::mutex> uniqueLock(appRemoteObjLock_);
    appRemoteObj_ = nullptr;
}

bool AppGalleryConnection::ConnectExtAbility(std::string abilityName)
{
    HILOGI("ConnectExtAbility called, userId = %{public}d", userId);
    std::unique_lock<std::mutex> uniqueLock(appRemoteObjLock_);
    if (appRemoteObj_ != nullptr) {
        return true;
    }

    auto appGalleryBundleName = BundleMgrAdapter::GetAppGalleryBundleName();
    if (appGalleryBundleName.empty()) {
        HILOGE("ConnectExtAbility GetAppGalleryBundleName failed, userId = %{public}d", userId);
        return false;
    }

    Want want;
    want.SetElementName(appGalleryBundleName.c_str(), abilityName);
    auto ret = AbilityManagerClient::GetInstance()->ConnectAbility(want, this, userId);
    if (ret != IAppGalleryService::ERR_OK) {
        HILOGE("ConnectExtAbility failed, ret=%{public}d, userId = %{public}d", ret, userId);
        return false;
    }
    conditionVal_.wait_for(uniqueLock, std::chrono::seconds(CONNECT_TIME),
        [this] { return appRemoteObj_ != nullptr; });
    if (appRemoteObj_ == nullptr) {
        HILOGE("ConnectExtAbility failed, ret=%{public}d, userId = %{public}d", ret, userId);
        return false;
    }
    HILOGI("ConnectExtAbility success, ret=%{public}d, userId = %{public}d", ret, userId);
    return true;
}

sptr<IRemoteObject> AppGalleryConnection::GetRemoteObj()
{
    std::unique_lock<std::mutex> uniqueLock(appRemoteObjLock_);
    return appRemoteObj_;
}
} // namespace OHOS::FileManagement::Backup