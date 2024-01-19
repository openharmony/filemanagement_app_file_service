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

#include "module_ipc/svc_restore_deps_manager.h"

#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

vector<string> SvcRestoreDepsManager::GetRestoreBundleNames(const vector<BJsonEntityCaps::BundleInfo> &bundleInfos,
                                                            RestoreTypeEnum restoreType)
{
    unique_lock<shared_mutex> lock(lock_);
    vector<string> restoreBundleNames {}; // 需要恢复的应用列表
    BuildDepsMap(bundleInfos);            // 构建依赖Map

    for (auto &bundleInfo : bundleInfos) {
        string restoreDeps = bundleInfo.restoreDeps;
        if (restoreDeps.empty()) {
            // 将没有依赖的应用加入到需要恢复的应用列表
            restoreBundleNames.emplace_back(bundleInfo.name);
        } else {
            RestoreInfo info {};
            info.restoreType_ = restoreType;
            toRestoreBundleMap_.insert(make_pair(bundleInfo.name, info));

            // 如果该应用的依赖项已经完成备份，也需要加到需要恢复的应用列表
            if (IsAllDepsRestored(bundleInfo.name)) {
                restoreBundleNames.emplace_back(bundleInfo.name);
                toRestoreBundleMap_.erase(bundleInfo.name);
            }
        }
    }

    return restoreBundleNames;
}

map<string, SvcRestoreDepsManager::RestoreInfo> SvcRestoreDepsManager::GetRestoreBundleMap()
{
    unique_lock<shared_mutex> lock(lock_);
    map<string, RestoreInfo> restoreBundleMap {};                                   // 需要恢复的应用列表
    for (auto it = toRestoreBundleMap_.begin(); it != toRestoreBundleMap_.end();) { // 所有有依赖的应用
        // 如果该应用的依赖项已经完成备份，也需要加到 restoreBundleNames
        string bundleName = it->first;
        if (IsAllDepsRestored(bundleName)) {
            RestoreInfo restoreInfo = it->second;
            restoreBundleMap.insert(make_pair(bundleName, restoreInfo));
            toRestoreBundleMap_.erase(it++);
        } else {
            it++;
        }
    }
    return restoreBundleMap;
}

bool SvcRestoreDepsManager::IsAllDepsRestored(const string &bundle)
{
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
    return isAllDepsRestored;
}

void SvcRestoreDepsManager::BuildDepsMap(const vector<BJsonEntityCaps::BundleInfo> &bundleInfos)
{
    for (auto &bundleInfo : bundleInfos) {
        if (depsMap_.find(bundleInfo.name) != depsMap_.end()) {
            continue;
        }
        allBundles_.emplace_back(bundleInfo);

        vector<string> depsList {};
        string restoreDeps = bundleInfo.restoreDeps;
        if (restoreDeps.find(",") != string::npos) {
            depsList = SplitString(restoreDeps, ",");
        } else {
            if (!restoreDeps.empty()) {
                depsList.emplace_back(restoreDeps);
            }
        }

        depsMap_.insert(make_pair(bundleInfo.name, depsList));
    }
}

vector<string> SvcRestoreDepsManager::SplitString(const string &srcStr, const string &separator)
{
    HILOGI("srcStr:%{public}s, separator:%{public}s", srcStr.c_str(), separator.c_str());
    vector<string> dst;
    if (srcStr.empty() || separator.empty()) {
        return dst;
    }
    size_t start = 0;
    size_t index = srcStr.find_first_of(separator, 0);
    while (index != srcStr.npos) {
        if (start != index) {
            string tempStr = srcStr.substr(start, index - start);
            tempStr.erase(0, tempStr.find_first_not_of(" "));
            tempStr.erase(tempStr.find_last_not_of(" ") + 1);
            tempStr.erase(tempStr.find_last_not_of("\r") + 1);
            dst.push_back(tempStr);
        }
        start = index + 1;
        index = srcStr.find_first_of(separator, start);
    }

    if (!srcStr.substr(start).empty()) {
        string tempStr = srcStr.substr(start);
        tempStr.erase(0, tempStr.find_first_not_of(" "));
        tempStr.erase(tempStr.find_last_not_of(" ") + 1);
        tempStr.erase(tempStr.find_last_not_of("\r") + 1);
        dst.push_back(tempStr);
    }
    return dst;
}

void SvcRestoreDepsManager::AddRestoredBundles(const string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    restoredBundles_.insert(bundleName);
}

vector<BJsonEntityCaps::BundleInfo> SvcRestoreDepsManager::GetAllBundles() const
{
    return allBundles_;
}

bool SvcRestoreDepsManager::IsAllBundlesRestored() const
{
    return toRestoreBundleMap_.empty();
}

bool SvcRestoreDepsManager::UpdateToRestoreBundleMap(const string &bundleName, const string &fileName)
{
    unique_lock<shared_mutex> lock(lock_);
    auto it = toRestoreBundleMap_.find(bundleName);
    if (it != toRestoreBundleMap_.end()) {
        it->second.fileNames_.insert(fileName);
        return true;
    }
    return false;
}

} // namespace OHOS::FileManagement::Backup