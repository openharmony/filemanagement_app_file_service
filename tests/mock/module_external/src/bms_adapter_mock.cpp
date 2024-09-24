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

#include "bms_adapter_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfos(const vector<string> &bundleNames, int32_t userId)
{
    return BBundleMgrAdapter::bms->GetBundleInfos(bundleNames, userId);
}

string BundleMgrAdapter::GetAppGalleryBundleName()
{
    return BBundleMgrAdapter::bms->GetAppGalleryBundleName();
}

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfosForIncremental(
    const vector<BIncrementalData> &incrementalDataList, int32_t userId)
{
    return BBundleMgrAdapter::bms->GetBundleInfosForIncremental(incrementalDataList, userId);
}

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfosForIncremental(int32_t userId,
    const std::vector<BIncrementalData> &extraIncreData)
{
    return BBundleMgrAdapter::bms->GetBundleInfosForIncremental(userId, extraIncreData);
}

string BundleMgrAdapter::GetExtName(string bundleName, int32_t userId)
{
    return BBundleMgrAdapter::bms->GetExtName(bundleName, userId);
}

std::vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfosForSA()
{
    return BBundleMgrAdapter::bms->GetBundleInfosForSA();
}

void BundleMgrAdapter::GetBundleInfoForSA(std::string bundleName, std::vector<BJsonEntityCaps::BundleInfo>& bundleInfos)
{
    return BBundleMgrAdapter::bms->GetBundleInfoForSA(bundleName, bundleInfos);
}

bool BundleMgrAdapter::IsUser0BundleName(std::string bundleName, int32_t userId)
{
    return BBundleMgrAdapter::bms->IsUser0BundleName(bundleName, userId);
}

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfosForAppend(
    const vector<BIncrementalData> &incrementalDataList, int32_t userId)
{
    return BBundleMgrAdapter::bms->GetBundleInfosForAppend(incrementalDataList, userId);
}
} // namespace OHOS::FileManagement::Backup