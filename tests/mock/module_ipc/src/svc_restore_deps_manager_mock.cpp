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

#include "svc_restore_deps_manager_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

vector<string> SvcRestoreDepsManager::GetRestoreBundleNames(const vector<BJsonEntityCaps::BundleInfo> &bundleInfos,
    RestoreTypeEnum restoreType)
{
    return BSvcRestoreDepsManager::manager->GetRestoreBundleNames(bundleInfos, restoreType);
}

map<string, SvcRestoreDepsManager::RestoreInfo> SvcRestoreDepsManager::GetRestoreBundleMap()
{
    return BSvcRestoreDepsManager::manager->GetRestoreBundleMap();
}

void SvcRestoreDepsManager::AddRestoredBundles(const string &bundleName) {}

vector<BJsonEntityCaps::BundleInfo> SvcRestoreDepsManager::GetAllBundles() const
{
    return BSvcRestoreDepsManager::manager->GetAllBundles();
}

bool SvcRestoreDepsManager::IsAllBundlesRestored() const
{
    return BSvcRestoreDepsManager::manager->IsAllBundlesRestored();
}

bool SvcRestoreDepsManager::UpdateToRestoreBundleMap(const string &bundleName, const string &fileName)
{
    return BSvcRestoreDepsManager::manager->UpdateToRestoreBundleMap(bundleName, fileName);
}
} // namespace OHOS::FileManagement::Backup