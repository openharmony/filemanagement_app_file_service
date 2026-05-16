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
#include <fuzzer/FuzzedDataProvider.h>
#include "module_ipc/svc_restore_deps_manager.h"
#include "b_json/b_json_entity_caps.h"

using namespace OHOS::FileManagement::Backup;
using namespace std;

namespace OHOS {
constexpr int32_t MIN_BUNDLE_COUNT = 0;
constexpr int32_t MAX_BUNDLE_COUNT = 10;
constexpr size_t MAX_STRING_LENGTH = 256;
constexpr int32_t MIN_RESTORE_TYPE = 0;
constexpr int32_t MAX_RESTORE_TYPE = 1;

vector<BJsonEntityCaps::BundleInfo> GenerateBundleInfos(FuzzedDataProvider &fdp)
{
    vector<BJsonEntityCaps::BundleInfo> infos;
    int32_t count = fdp.ConsumeIntegralInRange<int32_t>(MIN_BUNDLE_COUNT, MAX_BUNDLE_COUNT);
    for (int32_t i = 0; i < count; i++) {
        BJsonEntityCaps::BundleInfo info;
        info.name = fdp.ConsumeRandomLengthString(MAX_STRING_LENGTH);
        info.versionName = fdp.ConsumeRandomLengthString(MAX_STRING_LENGTH);
        info.extensionName = fdp.ConsumeRandomLengthString(MAX_STRING_LENGTH);
        info.restoreDeps = fdp.ConsumeRandomLengthString(MAX_STRING_LENGTH);
        info.supportScene = fdp.ConsumeRandomLengthString(MAX_STRING_LENGTH);
        info.allToBackup = fdp.ConsumeBool();
        info.versionCode = fdp.ConsumeIntegral<int64_t>();
        info.spaceOccupied = fdp.ConsumeIntegral<int64_t>();
        infos.push_back(info);
    }
    return infos;
}

bool SvcRestoreDepsManagerFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    RestoreTypeEnum restoreType =
        static_cast<RestoreTypeEnum>(fdp.ConsumeIntegralInRange<int32_t>(MIN_RESTORE_TYPE, MAX_RESTORE_TYPE));
    vector<BJsonEntityCaps::BundleInfo> bundleInfos = GenerateBundleInfos(fdp);

    SvcRestoreDepsManager::GetInstance().GetRestoreBundleNames(bundleInfos, restoreType);
    SvcRestoreDepsManager::GetInstance().GetRestoreBundleMap();

    string bundleName = fdp.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    string fileName = fdp.ConsumeRandomLengthString(MAX_STRING_LENGTH);

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

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::SvcRestoreDepsManagerFuzzTest(data, size);
    return 0;
}