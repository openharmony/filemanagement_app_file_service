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

#ifndef OHOS_FILEMGMT_BACKUP_SVC_RESTORE_DEPS_MANAGER_H
#define OHOS_FILEMGMT_BACKUP_SVC_RESTORE_DEPS_MANAGER_H

#include <string>
#include <vector>

#include "b_json/b_json_entity_caps.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class SvcRestoreDepsManager {
public:
    static SvcRestoreDepsManager &GetInstance()
    {
        static SvcRestoreDepsManager manager;
        return manager;
    }

    vector<string> GetRestoreBundleNames(const vector<BJsonEntityCaps::BundleInfo> &infos);
    void AddRestoredBundles(const string &bundleName);
    vector<string> GetRestoreBundleNames();
    vector<BJsonEntityCaps::BundleInfo> GetAllBundles() const;
    bool IsAllBundlesRestored() const;

private:
    SvcRestoreDepsManager() {}
    ~SvcRestoreDepsManager() = default;
    SvcRestoreDepsManager(const SvcRestoreDepsManager &manager) = delete;
    SvcRestoreDepsManager &operator=(const SvcRestoreDepsManager &manager) = delete;

    void BuildDepsMap(const vector<BJsonEntityCaps::BundleInfo> &infos);
    vector<string> SplitString(const string &str, char delim);
    void AddBundles(vector<string> &bundles, const string &bundleName);
    bool IsAllDepsRestored(const string &bundleName);

    map<string, vector<string>> depsMap_ {};
    vector<BJsonEntityCaps::BundleInfo> allBundles_ {}; // 所有的应用

    vector<string> toRestoreBundles_ {}; // 有依赖的应用
    vector<string> restoredBundles_ {};  // 已经恢复完成的应用
};

} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SVC_RESTORE_DEPS_MANAGER_H
