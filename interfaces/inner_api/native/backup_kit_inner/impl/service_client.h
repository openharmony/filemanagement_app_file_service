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

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_CLIENT_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_CLIENT_H

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include "backup_file_info.h"
#include "backup_incremental_data.h"
#include "iservice.h"
#include "system_ability_load_callback_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceClient {

public:
    static sptr<IService> GetServiceProxyPointer();
    static sptr<IService> GetInstance();
    static void InvaildInstance();
    bool CheckServiceProxy();
public:
 explicit ServiceClient();
    ~ServiceClient();
public:
    class ServiceProxyLoadCallback : public SystemAbilityLoadCallbackStub {
    public:
        void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject) override;
        void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;

    public:
        std::condition_variable proxyConVar_;
        std::atomic<bool> isLoadSuccess_ = {false};
    };

private:
    static inline std::mutex proxyMutex_;
    static inline std::mutex getInstanceMutex_;
    static inline sptr<IService> serviceProxy_ = nullptr;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_CLIENT_H