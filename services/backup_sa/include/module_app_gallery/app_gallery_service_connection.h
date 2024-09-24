/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_APP_GALLERY_SERVICE_CONNECTION_H
#define OHOS_FILEMGMT_BACKUP_APP_GALLERY_SERVICE_CONNECTION_H

#include <condition_variable>
#include <mutex>
#include <chrono>

#include "ability_manager_client.h"
#include "ability_connect_callback_stub.h"
#include "want.h"

#include "i_appgallery_service.h"
#include "module_external/bms_adapter.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace OHOS::AppExecFwk;
const int32_t CONNECT_TIME = 3;

template <typename T> class AppGralleryConnection : public AbilityConnectionStub {
public:
    void OnAbilityConnectDone(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject,
        int resultCode)
    {
        std::string uri = element.GetURI();
        HILOGI("OnAbilityConnectDone, uri = %{public}s", uri.c_str());
        T::appRemoteObj_ = remoteObject;
        T::conditionVal_.notify_one();
    }

    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
    {
        std::string uri = element.GetURI();
        HILOGI("OnAbilityDisconnectDone, uri = %{public}s", uri.c_str());
        T::appRemoteObj_ = nullptr;
    }
};

template <typename T> bool ConnectExtAbility()
{
    HILOGI("ConnectExtAbility called");
    std::lock_guard<std::mutex> autoLock(T::appRemoteObjLock_);
    if (T::appRemoteObj_ != nullptr) {
        return true;
    }

    auto appGalleryBundleName = BundleMgrAdapter::GetAppGalleryBundleName();
    if (appGalleryBundleName.empty()) {
        HILOGI("ConnectExtAbility GetAppGalleryBundleName failed");
        return false;
    }

    Want want;
    want.SetElementName(appGalleryBundleName.c_str(), T::abilityName);
    sptr<IAbilityConnection> connect = new AppGralleryConnection<T>();
    auto ret = AbilityManagerClient::GetInstance()->ConnectAbility(want, connect, -1);

    std::unique_lock<std::mutex> uniqueLock(T::connectMutex);
    T::conditionVal_.wait_for(uniqueLock, std::chrono::seconds(CONNECT_TIME));
    if (ret != IAppGalleryService::ERR_OK || T::appRemoteObj_ == nullptr) {
        HILOGI("ConnectExtAbility failed, ret=%{public}d", ret);
        T::appRemoteObj_ = nullptr;
        return false;
    }
    HILOGI("ConnectExtAbility success, ret=%{public}d", ret);
    return true;
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_APP_GALLERY_SERVICE_CONNECTION_H