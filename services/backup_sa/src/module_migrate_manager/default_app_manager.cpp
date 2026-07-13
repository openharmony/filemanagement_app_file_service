/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "module_migrate_manager/default_app_manager.h"
#include "b_resources/b_constants.h"
#include "module_ipc/service.h"

namespace OHOS::FileManagement::Backup {

DefaultAppManager::DefaultAppManager(wptr<Service> servicePtr) : servicePtr_(servicePtr) {}

sptr<MigrateManager> DefaultAppManager::GetMigrateInstance(const std::string &bundleName, int32_t userId)
{
    std::unique_lock<std::shared_mutex> lock(defaultAppMutex_);
    auto it = defaultAppMap_.find(bundleName);
    if (it == defaultAppMap_.end()) {
        return nullptr;
    }
    if (it->second != nullptr) {
        return it->second;
    }
    auto instance = sptr(new MigrateManager(servicePtr_, bundleName, userId));
    defaultAppMap_[bundleName] = instance;
    return instance;
}

void DefaultAppManager::SetDefaultBundleName(const std::vector<std::string> &bundleNames, bool isDefault)
{
    if (!isDefault) {
        return;
    }
    std::unique_lock<std::shared_mutex> lock(defaultAppMutex_);
    for (const auto &bundleName : bundleNames) {
        defaultAppMap_.try_emplace(bundleName, nullptr);
        HILOGI("bundleName:%{public}s isDefault", bundleName.c_str());
    }
}

bool DefaultAppManager::IsDefaultBundle(const std::vector<std::string> &bundleNames)
{
    for (const auto &bundleName : bundleNames) {
        return IsDefaultBundle(bundleName);
    }
    return false;
}

bool DefaultAppManager::IsDefaultBundle(const std::string &bundleName)
{
    std::shared_lock<std::shared_mutex> lock(defaultAppMutex_);
    return defaultAppMap_.find(bundleName) != defaultAppMap_.end();
}

bool DefaultAppManager::ClearDefaultAppData()
{
    HILOGI("ClearDefaultAppData enter");
    std::unique_lock<std::shared_mutex> lock(defaultAppMutex_);
    for (auto &[bundleName, manager] : defaultAppMap_) {
        if (manager != nullptr) {
            manager->DoClear();
        }
    }
    return true;
}

std::string DefaultAppManager::GetCallerNameByFilePath(const std::string &filePath)
{
    std::shared_lock<std::shared_mutex> lock(defaultAppMutex_);
    for (const auto &[bundleName, manager] : defaultAppMap_) {
        if (filePath.find(BConstants::GetBundleDir(BConstants::DEFAULT_USER_ID, bundleName)) == 0 ||
            filePath.find(BConstants::GetBundleDir(BConstants::DEFAULT_USER_ID, bundleName, false)) == 0) {
            return bundleName;
        }
    }
    return "";
}

} // namespace OHOS::FileManagement::Backup
