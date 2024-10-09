/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License") = 0;
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

#ifndef OHOS_FILEMGMT_BACKUP_IPC_SVC_RESTORE_DEPS_MANAGER_MOCK_H
#define OHOS_FILEMGMT_BACKUP_IPC_SVC_RESTORE_DEPS_MANAGER_MOCK_H

#include <gmock/gmock.h>

#include "svc_restore_deps_manager.h"

namespace OHOS::FileManagement::Backup {
class BSvcRestoreDepsManager {
public:
    virtual std::vector<std::string> GetRestoreBundleNames(const std::vector<BJsonEntityCaps::BundleInfo>&,
        RestoreTypeEnum) = 0;
    virtual std::map<std::string, SvcRestoreDepsManager::RestoreInfo> GetRestoreBundleMap();
    virtual std::vector<BJsonEntityCaps::BundleInfo> GetAllBundles() const = 0;
    virtual bool IsAllBundlesRestored() const = 0;
    virtual bool UpdateToRestoreBundleMap(const std::string&, const std::string&) = 0;
public:
    BSvcRestoreDepsManager() = default;
    virtual ~BSvcRestoreDepsManager() = default;
public:
    static inline std::shared_ptr<BSvcRestoreDepsManager> manager = nullptr;
};

class SvcRestoreDepsManagerMock : public BSvcRestoreDepsManager {
public:
    MOCK_METHOD((std::vector<std::string>), GetRestoreBundleNames, ((const std::vector<BJsonEntityCaps::BundleInfo>&),
        RestoreTypeEnum));
    MOCK_METHOD((std::map<std::string, SvcRestoreDepsManager::RestoreInfo>), GetRestoreBundleMap, ());
    MOCK_METHOD((std::vector<BJsonEntityCaps::BundleInfo>), GetAllBundles, (), (const));
    MOCK_METHOD(bool, IsAllBundlesRestored, (), (const));
    MOCK_METHOD(bool, UpdateToRestoreBundleMap, (const std::string&, const std::string&));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_IPC_SVC_RESTORE_DEPS_MANAGER_MOCK_H