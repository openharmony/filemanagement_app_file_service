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

#ifndef OHOS_FILEMGMT_BACKUP_SMS_ADAPTER_MOCK_H
#define OHOS_FILEMGMT_BACKUP_SMS_ADAPTER_MOCK_H

#include <gmock/gmock.h>

#include "module_external/sms_adapter.h"

namespace OHOS::FileManagement::Backup {
class BStorageMgrAdapter {
public:
    virtual StorageManager::BundleStats GetBundleStats(const std::string&) = 0;
    virtual int64_t GetUserStorageStats(const std::string&, int32_t) = 0;
    virtual int32_t UpdateMemPara(int32_t) = 0;
    virtual int32_t GetBundleStatsForIncrease(uint32_t, const std::vector<std::string>&,
        const std::vector<int64_t>&, std::vector<int64_t>&, std::vector<int64_t>&) = 0;
public:
    BStorageMgrAdapter() = default;
    virtual ~BStorageMgrAdapter() = default;
public:
    static inline std::shared_ptr<BStorageMgrAdapter> sms = nullptr;
};

class StorageMgrAdapterMock : public BStorageMgrAdapter {
public:
    MOCK_METHOD(StorageManager::BundleStats, GetBundleStats, (const std::string&));
    MOCK_METHOD(int64_t, GetUserStorageStats, (const std::string&, int32_t));
    MOCK_METHOD(int32_t, UpdateMemPara, (int32_t));
    MOCK_METHOD(int32_t, GetBundleStatsForIncrease, (uint32_t, (const std::vector<std::string>&),
        (const std::vector<int64_t>&), (std::vector<int64_t>&), (std::vector<int64_t>&)));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_SMS_ADAPTER_MOCK_H
