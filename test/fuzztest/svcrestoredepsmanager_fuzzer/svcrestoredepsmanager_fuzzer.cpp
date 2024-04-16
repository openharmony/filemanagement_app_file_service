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

#include "svcrestoredepsmanager_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include "securec.h"

#include "module_ipc/svc_restore_deps_manager.h"

using namespace OHOS::FileManagement::Backup;
using namespace std;

namespace OHOS {
constexpr size_t U32_AT_SIZE = 3;
constexpr int SPLITE_SIZE = 5;
constexpr size_t REDUNDENT_SIZE = 2;
constexpr size_t DISTINCT_SIZE = 2;
void GetInfo(const uint8_t *data, size_t size, vector<BJsonEntityCaps::BundleInfo> &info)
{
    size_t base = size;
    if (size >= U32_AT_SIZE) {
        base = (size + 1) / REDUNDENT_SIZE;
    }

    for (size_t i = 0; i < size; i++) {
        BJsonEntityCaps::BundleInfo bundleInfo;
        int len = size / SPLITE_SIZE;
        int pos = 0;
        bundleInfo.name = string(reinterpret_cast<const char*>(data + pos), len) + to_string(i % base);
        pos += len;
        bundleInfo.versionName = string(reinterpret_cast<const char*>(data + pos), len) + to_string(i);
        pos += len;
        bundleInfo.extensionName = string(reinterpret_cast<const char*>(data + pos), len) + to_string(i);
        pos += len;

        if (i % DISTINCT_SIZE == 0) {
            bundleInfo.restoreDeps = string(reinterpret_cast<const char*>(data + pos), len) + to_string(i);
        } else {
            bundleInfo.restoreDeps = string(reinterpret_cast<const char*>(data + pos), len) + "," + to_string(i);
        }

        pos += len;
        bundleInfo.supportScene = string(reinterpret_cast<const char*>(data + pos), len) + to_string(i);

        if (size >= sizeof(bool)) {
            bundleInfo.allToBackup = *(reinterpret_cast<const bool*>(data));
        }

        if (size >= sizeof(uint32_t)) {
            bundleInfo.versionCode = *(reinterpret_cast<const uint32_t*>(data));
        }

        if (size >= sizeof(int64_t)) {
            bundleInfo.spaceOccupied = *(reinterpret_cast<const int64_t*>(data));
        }
        info.push_back(bundleInfo);
    }
}

bool SvcRestoreDepsManagerFuzzTest(const uint8_t *data, size_t size)
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND;
    if (size >= sizeof(RestoreTypeEnum)) {
        restoreType = *(reinterpret_cast<const RestoreTypeEnum*>(data));
    }
    SvcRestoreDepsManager::GetInstance().GetRestoreBundleNames(bundleInfos, restoreType);
    GetInfo(data, size, bundleInfos);

    BJsonEntityCaps::BundleInfo bundleInfo;
    bundleInfo.name = string(reinterpret_cast<const char*>(data), size);
    bundleInfos.push_back(bundleInfo);

    SvcRestoreDepsManager::GetInstance().GetRestoreBundleNames(bundleInfos, restoreType);
    SvcRestoreDepsManager::GetInstance().GetRestoreBundleMap();

    size_t pos = size >> 1;
    string bundleName(string(reinterpret_cast<const char*>(data), pos));
    string fileName(string(reinterpret_cast<const char*>(data + pos), size - pos));
    SvcRestoreDepsManager::GetInstance().AddRestoredBundles(bundleName);
    SvcRestoreDepsManager::GetInstance().GetAllBundles();

    SvcRestoreDepsManager::GetInstance().UpdateToRestoreBundleMap(bundleName, fileName);
    SvcRestoreDepsManager::GetInstance().UpdateToRestoreBundleMap(fileName, bundleName);
    SvcRestoreDepsManager::GetInstance().IsAllBundlesRestored();

    SvcRestoreDepsManager::GetInstance().depsMap_.clear();
    SvcRestoreDepsManager::GetInstance().allBundles_.clear();
    SvcRestoreDepsManager::GetInstance().toRestoreBundleMap_.clear();
    SvcRestoreDepsManager::GetInstance().restoredBundles_.clear();

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    OHOS::SvcRestoreDepsManagerFuzzTest(data, size);
    return 0;
}