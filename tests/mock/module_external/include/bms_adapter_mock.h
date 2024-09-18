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

#ifndef OHOS_FILEMGMT_BACKUP_BMS_ADAPTER_MOCK_H
#define OHOS_FILEMGMT_BACKUP_BMS_ADAPTER_MOCK_H

#include <gmock/gmock.h>

#include "module_external/bms_adapter.h"

namespace OHOS::FileManagement::Backup {
class BBundleMgrAdapter {
public:
    virtual std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfos(const std::vector<std::string>&, int32_t) = 0;
    virtual std::string GetAppGalleryBundleName() = 0;
    virtual std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfosForIncremental(
        const std::vector<BIncrementalData>&, int32_t) = 0;
    virtual std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfosForIncremental(int32_t,
        const std::vector<BIncrementalData>&) = 0;
    virtual std::string GetExtName(string, int32_t) = 0;
    virtual std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfosForSA() = 0;
    virtual void GetBundleInfoForSA(std::string, std::vector<BJsonEntityCaps::BundleInfo>&) = 0;
public:
    BBundleMgrAdapter() = default;
    virtual ~BBundleMgrAdapter() = default;
public:
    static inline std::shared_ptr<BBundleMgrAdapter> bms = nullptr;
};

class BundleMgrAdapterMock : public BBundleMgrAdapter {
public:
    MOCK_METHOD((std::vector<BJsonEntityCaps::BundleInfo>), GetBundleInfos,
        ((const std::vector<std::string>&), int32_t));
    MOCK_METHOD(std::string,  GetAppGalleryBundleName, ());
    MOCK_METHOD((std::vector<BJsonEntityCaps::BundleInfo>), GetBundleInfosForIncremental,
        ((const std::vector<BIncrementalData>&), int32_t));
    MOCK_METHOD((std::vector<BJsonEntityCaps::BundleInfo>), GetBundleInfosForIncremental, (int32_t,
        (const std::vector<BIncrementalData>&)));
    MOCK_METHOD(std::string, GetExtName, (std::string, int32_t));
    MOCK_METHOD((std::vector<BJsonEntityCaps::BundleInfo>), GetBundleInfosForSA, ());
    MOCK_METHOD(void, GetBundleInfoForSA, (std::string, (std::vector<BJsonEntityCaps::BundleInfo>&)));
    MOCK_METHOD((std::vector<BJsonEntityCaps::BundleInfo>), GetBundleInfosForAppend,
        ((const std::vector<BIncrementalData>&), int32_t));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_BMS_ADAPTER_MOCK_H
