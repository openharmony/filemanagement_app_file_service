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

#include "module_external/sms_adapter.h"

#include <refbase.h>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static sptr<StorageManager::IStorageManager> GetStorageManager()
{
    auto saMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saMgr == nullptr) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to get system ability manager");
    }

    auto storageObj = saMgr->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    if (storageObj == nullptr) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to get storage manager service");
    }

    return iface_cast<StorageManager::IStorageManager>(storageObj);
}

StorageManager::BundleStats StorageMgrAdapter::GetBundleStats(const string &bundleName)
{
    StorageManager::BundleStats bundleStats;
    auto storageMgr = GetStorageManager();
    if (storageMgr->GetBundleStats(bundleName, bundleStats)) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to get bundle stats");
    }
    return bundleStats;
}
} // namespace OHOS::FileManagement::Backup
