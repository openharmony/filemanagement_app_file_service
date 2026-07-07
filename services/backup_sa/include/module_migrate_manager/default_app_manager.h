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

#ifndef OHOS_FILEMGMT_BACKUP_DEFAULT_APP_MANAGER_H
#define OHOS_FILEMGMT_BACKUP_DEFAULT_APP_MANAGER_H

#include <map>
#include <shared_mutex>
#include <string>
#include <vector>

#include "module_migrate_manager/migrate_manager.h"

namespace OHOS::FileManagement::Backup {

class Service;

class DefaultAppManager {
public:
    explicit DefaultAppManager(wptr<Service> servicePtr);

    void SetDefaultBundleName(const std::vector<std::string> &bundleNames, bool isDefault);
    bool IsDefaultBundle(const std::vector<std::string> &bundleNames);
    bool IsDefaultBundle(const std::string &bundleName);
    sptr<MigrateManager> GetMigrateInstance(const std::string &bundleName, int32_t userId);
    bool ClearDefaultAppData();
    std::string GetCallerNameByFilePath(const std::string &filePath);

private:
    std::map<std::string, sptr<MigrateManager>> defaultAppMap_;
    std::shared_mutex defaultAppMutex_;
    wptr<Service> servicePtr_ = nullptr;
};

} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_DEFAULT_APP_MANAGER_H
