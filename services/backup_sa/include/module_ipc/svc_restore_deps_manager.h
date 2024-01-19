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

#ifndef OHOS_FILEMGMT_BACKUP_SVC_RESTORE_DEPS_MANAGER_H
#define OHOS_FILEMGMT_BACKUP_SVC_RESTORE_DEPS_MANAGER_H

#include <set>
#include <shared_mutex>
#include <string>
#include <vector>

#include "b_json/b_json_entity_caps.h"
#include "i_service.h"

namespace OHOS::FileManagement::Backup {

class SvcRestoreDepsManager {
public:
    static SvcRestoreDepsManager &GetInstance()
    {
        static SvcRestoreDepsManager manager;
        return manager;
    }
    struct RestoreInfo {
        RestoreTypeEnum restoreType_ {RESTORE_DATA_WAIT_SEND};
        set<string> fileNames_ {};
    };

    vector<string> GetRestoreBundleNames(const vector<BJsonEntityCaps::BundleInfo> &infos, RestoreTypeEnum restoreType);
    map<string, SvcRestoreDepsManager::RestoreInfo> GetRestoreBundleMap();
    void AddRestoredBundles(const string &bundleName);
    vector<BJsonEntityCaps::BundleInfo> GetAllBundles() const;
    bool IsAllBundlesRestored() const;
    bool UpdateToRestoreBundleMap(const string &bundleName, const string &fileName);

private:
    SvcRestoreDepsManager() {}
    ~SvcRestoreDepsManager() = default;
    SvcRestoreDepsManager(const SvcRestoreDepsManager &manager) = delete;
    SvcRestoreDepsManager &operator=(const SvcRestoreDepsManager &manager) = delete;

    void BuildDepsMap(const vector<BJsonEntityCaps::BundleInfo> &infos);
    vector<string> SplitString(const string &srcStr, const string &separator);
    bool IsAllDepsRestored(const string &bundleName);

    mutable std::shared_mutex lock_;
    map<string, vector<string>> depsMap_ {};
    vector<BJsonEntityCaps::BundleInfo> allBundles_ {}; // 所有的应用
    map<string, RestoreInfo> toRestoreBundleMap_ {};    // 有依赖的应用
    set<string> restoredBundles_ {};                    // 已经恢复完成的应用
};

} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SVC_RESTORE_DEPS_MANAGER_H
