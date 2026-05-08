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

#ifndef OHOS_FILEMGMT_BACKUP_STORAGE_MANAGER_MOCK_H
#define OHOS_FILEMGMT_BACKUP_STORAGE_MANAGER_MOCK_H

#include <gmock/gmock.h>
#include <cstdint>

namespace OHOS::StorageManager {
class IStorageManager {
public:
    virtual ~IStorageManager() = default;
    virtual int32_t GetFreeSize(int64_t& freeSize) = 0;
};
} // namespace OHOS::StorageManager
#endif // OHOS_FILEMGMT_BACKUP_STORAGE_MANAGER_MOCK_H
