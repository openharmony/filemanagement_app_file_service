/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "b_utils/storage_manager_helper.h"
#include <unistd.h>
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {

constexpr int32_t GET_CLIENT_RETRY_TIMES = 5;
constexpr int32_t SLEEP_TIME = 1;

StorageManagerHelper& StorageManagerHelper::GetInstance()
{
    static StorageManagerHelper instance;
    return instance;
}

sptr<StorageManager::IStorageManager> StorageManagerHelper::GetStorageManagerProxy()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (storageManagerProxy_ != nullptr) {
        return storageManagerProxy_;
    }

    auto saMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saMgr == nullptr) {
        HILOGE("Get samgr failed");
        return nullptr;
    }

    int32_t count = 0;
    while (count < GET_CLIENT_RETRY_TIMES) {
        count++;
        auto storageObj = saMgr->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
        if (storageObj == nullptr) {
            HILOGE("Get storage manager service failed");
            sleep(SLEEP_TIME);
            continue;
        }

        storageManagerProxy_ = iface_cast<StorageManager::IStorageManager>(storageObj);
        if (storageManagerProxy_ == nullptr) {
            HILOGE("Iface cast failed");
            sleep(SLEEP_TIME);
            continue;
        }
        return storageManagerProxy_;
    }
    return nullptr;
}

uint64_t StorageManagerHelper::GetFreeSize()
{
    auto mgr = GetStorageManagerProxy();
    if (mgr == nullptr) {
        HILOGE("GetStorageManagerProxy failed, return 0");
        return 0;
    }

    int64_t freeSize = 0;
    int32_t ret = mgr->GetFreeSize(freeSize);
    if (ret != 0 || freeSize < 0) {
        HILOGE("GetFreeSize failed, ret=%{public}d", ret);
        return 0;
    }
    return static_cast<uint64_t>(freeSize);
}
} // namespace OHOS::FileManagement::Backup