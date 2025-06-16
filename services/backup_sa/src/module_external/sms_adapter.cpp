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

#include "module_external/sms_adapter.h"

#include <refbase.h>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "module_external/storage_manager_service.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
const string MEDIA_LIBRARY_HAP = "com.ohos.medialibrary.medialibrarydata";
const string EXTERNAL_FILE_HAP = "com.ohos.UserFile.ExternalFileManager";
const int64_t ERR_SIZE = -1;
} // namespace

StorageManager::BundleStats StorageMgrAdapter::GetBundleStats(const string &bundleName)
{
    StorageManager::BundleStats bundleStats;
    if (!StorageManagerService::GetInstance().GetBundleStats(bundleName, bundleStats)) {
        HILOGE("An error occured StorageMgrAdapter GetBundleStats");
    }
    return bundleStats;
}

int64_t StorageMgrAdapter::GetUserStorageStats(const std::string &bundleName, int32_t userId)
{
    StorageManager::StorageStats bundleStats;
    if (bundleName == MEDIA_LIBRARY_HAP) {
        if (StorageManagerService::GetInstance().GetUserStorageStatsByType(userId, bundleStats, MEDIA_TYPE) != E_OK) {
            HILOGE("Failed to get user media storage stats");
            return ERR_SIZE;
        }
        return bundleStats.image_ + bundleStats.video_;
    } else if (bundleName == EXTERNAL_FILE_HAP) {
        if (StorageManagerService::GetInstance().GetUserStorageStatsByType(userId, bundleStats, FILE_TYPE) != E_OK) {
            HILOGE("Failed to get user file storage stats");
            return ERR_SIZE;
        }
        return bundleStats.file_;
    }
    return 0;
}

int32_t StorageMgrAdapter::UpdateMemPara(int32_t size)
{
    int32_t oldSize = BConstants::DEFAULT_VFS_CACHE_PRESSURE;
    if (StorageManagerService::GetInstance().UpdateMemoryPara(size, oldSize) != E_OK) {
        HILOGE("An error occured StorageMgrAdapter UpdateMemPara");
        return BConstants::DEFAULT_VFS_CACHE_PRESSURE;
    }
    return oldSize;
}

int32_t StorageMgrAdapter::GetBundleStatsForIncrease(uint32_t userId, const std::vector<std::string> &bundleNames,
    const std::vector<int64_t> &incrementalBackTimes, std::vector<int64_t> &pkgFileSizes,
    std::vector<int64_t> &incPkgFileSizes)
{
    if (StorageManagerService::GetInstance().GetBundleStatsForIncrease(userId, bundleNames, incrementalBackTimes,
        pkgFileSizes, incPkgFileSizes) != E_OK) {
        HILOGE("An error occured StorageManagerService GetBundleStatsForIncrease");
        return E_ERR;
    }
    return E_OK;
}
} // namespace OHOS::FileManagement::Backup