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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace OHOS::FileManagement::Backup {
StorageManager::BundleStats StorageMgrAdapter::GetBundleStats(const std::string &bundleName)
{
    StorageManager::BundleStats bundleStats;
    return bundleStats;
}

int32_t StorageMgrAdapter::UpdateMemPara(int32_t size)
{
    return 0;
}

int32_t StorageMgrAdapter::GetBundleStatsForIncrease(uint32_t userId, const std::vector<std::string> &bundleNames,
    const std::vector<int64_t> &incrementalBackTimes, std::vector<int64_t> &pkgFileSizes,
    std::vector<int64_t> &incPkgFileSizes)
{
    return 0;
}
} // namespace OHOS::FileManagement::Backup
