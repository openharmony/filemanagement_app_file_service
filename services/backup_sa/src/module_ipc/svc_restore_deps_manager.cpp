/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "module_ipc/svc_restore_deps_manager.h"

#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {

vector<string> SvcRestoreDepsManager::GetRestoreBundleNames(const vector<BJsonEntityCaps::BundleInfo> &bundleInfos)
{
    HILOGI("SvcRestoreDepsManager::GetRestoreBundleNames, bundleInfos.size():%{public}d", bundleInfos.size());
    vector<string> restoreBundleNames {}; // 需要恢复的应用列表

    BuildDepsMap(bundleInfos); // 构建依赖Map

    for (auto &bundleInfo : bundleInfos) {
        string restoreDeps = bundleInfo.restoreDeps;
        if (restoreDeps.empty()) {
            // 将没有依赖的应用加入到需要恢复的应用列表
            restoreBundleNames.emplace_back(bundleInfo.name);
        } else {
            AddBundles(toRestoreBundles_, bundleInfo.name);

            // 如果该应用的依赖项已经完成备份，也需要加到需要恢复的应用列表
            if (IsAllDepsRestored(bundleInfo.name)) {
                AddBundles(restoreBundleNames, bundleInfo.name);
                toRestoreBundles_.erase(remove(toRestoreBundles_.begin(), toRestoreBundles_.end(), bundleInfo.name),
                                        toRestoreBundles_.end());
            }
        }
    }

    return restoreBundleNames;
}

vector<string> SvcRestoreDepsManager::GetRestoreBundleNames()
{
    HILOGI("SvcRestoreDepsManager::GetRestoreBundleNames");
    vector<string> restoreBundleNames {};    // 需要恢复的应用列表
    for (auto &bundle : toRestoreBundles_) { // 所有有依赖的应用
        // 如果该应用的依赖项已经完成备份，也需要加到 restoreBundleNames
        if (IsAllDepsRestored(bundle)) {
            AddBundles(restoreBundleNames, bundle);
            toRestoreBundles_.erase(remove(toRestoreBundles_.begin(), toRestoreBundles_.end(), bundle),
                                    toRestoreBundles_.end());
        }
    }
    return restoreBundleNames;
}

bool SvcRestoreDepsManager::IsAllDepsRestored(const string &bundle)
{
    HILOGI("SvcRestoreDepsManager::IsAllDepsRestored, bundle:%{public}s", bundle.c_str());
    if (depsMap_.find(bundle) == depsMap_.end()) {
        return false;
    }
    bool isAllDepsRestored = true;
    vector<string> depList = depsMap_[bundle];
    for (auto &dep : depList) {
        if (find(restoredBundles_.begin(), restoredBundles_.end(), dep) == restoredBundles_.end()) {
            isAllDepsRestored = false;
            break;
        }
    }
    HILOGI("SvcRestoreDepsManager::IsAllDepsRestored, isAllDepsRestored:%{public}d", isAllDepsRestored);
    return isAllDepsRestored;
}

void SvcRestoreDepsManager::BuildDepsMap(const vector<BJsonEntityCaps::BundleInfo> &bundleInfos)
{
    HILOGI("SvcRestoreDepsManager::BuildDepsMap, bundleInfos.size():%{public}d", bundleInfos.size());
    for (auto &bundleInfo : bundleInfos) {
        if (depsMap_.find(bundleInfo.name) != depsMap_.end()) {
            continue;
        }
        allBundles_.emplace_back(bundleInfo);

        vector<string> depsList {};
        string restoreDeps = bundleInfo.restoreDeps;
        if (restoreDeps.find(",") != string::npos) {
            depsList = SplitString(restoreDeps, ',');
        } else {
            if (!restoreDeps.empty()) {
                depsList.emplace_back(restoreDeps);
            }
        }

        depsMap_.insert(make_pair(bundleInfo.name, depsList));
    }
}

vector<string> SvcRestoreDepsManager::SplitString(const string &str, char delim)
{
    HILOGI("SvcRestoreDepsManager::SplitString, str:%{public}s", str.c_str());
    stringstream ss(str);
    string item {};
    vector<string> results {};
    while (getline(ss, item, delim)) {
        if (!item.empty()) {
            results.push_back(item);
        }
    }
    HILOGI("SvcRestoreDepsManager::SplitString, results.size():%{public}d", results.size());
    return results;
}

void SvcRestoreDepsManager::AddBundles(vector<string> &bundles, const string &bundleName)
{
    if (find(bundles.begin(), bundles.end(), bundleName) == bundles.end()) {
        HILOGI("SvcRestoreDepsManager::AddBundles, bundleName:%{public}s", bundleName.c_str());
        bundles.emplace_back(bundleName);
    }
}

void SvcRestoreDepsManager::AddRestoredBundles(const string &bundleName)
{
    HILOGI("SvcRestoreDepsManager::AddRestoredBundles, bundleName:%{public}s", bundleName.c_str());
    AddBundles(restoredBundles_, bundleName);
}

vector<BJsonEntityCaps::BundleInfo> SvcRestoreDepsManager::GetAllBundles() const
{
    return allBundles_;
}

bool SvcRestoreDepsManager::IsAllBundlesRestored() const
{
    return toRestoreBundles_.empty();
}

} // namespace OHOS::FileManagement::Backup