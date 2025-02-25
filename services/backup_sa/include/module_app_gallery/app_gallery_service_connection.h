/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "ability_manager_client.h"
#include "ability_connect_callback_stub.h"
#include "want.h"

#include "i_appgallery_service.h"

namespace OHOS::FileManagement::Backup {
using namespace OHOS::AppExecFwk;

class AppGalleryConnection : public AbilityConnectionStub {
public:
    AppGalleryConnection(int32_t userId) : userId(userId) {}

    void OnAbilityConnectDone(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject,
        int resultCode);

    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode);

    bool ConnectExtAbility(std::string abilityName);

    sptr<IRemoteObject> GetRemoteObj();

private:
    int32_t userId = 0;
    std::mutex conditionMutex_;
    std::condition_variable conditionVal_;
    std::mutex connectMutex;
    std::mutex appRemoteObjLock_;
    sptr<IRemoteObject> appRemoteObj_;
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_APP_GALLERY_SERVICE_CONNECTION_H