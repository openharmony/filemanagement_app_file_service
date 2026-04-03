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
 
#ifndef OHOS_FILEMGMT_BACKUP_ENHANCE_SERVICE_MANAGER_H
#define OHOS_FILEMGMT_BACKUP_ENHANCE_SERVICE_MANAGER_H
 
#include "ienhance_service.h"
 
#include <unordered_map>
#include <shared_mutex>
 
namespace OHOS::FileManagement::Backup {
 
class EnhanceServiceManager {
    using CreateFuncType = IEnhanceService *(*)();
    using DestroyFuncType = void(*)(IEnhanceService *);
public:
    EnhanceServiceManager(const EnhanceServiceManager&) = delete;
    EnhanceServiceManager& operator=(const EnhanceServiceManager&) = delete;
    EnhanceServiceManager(EnhanceServiceManager&&) = delete;
    EnhanceServiceManager& operator=(EnhanceServiceManager&&) = delete;
 
    static EnhanceServiceManager &GetInstance();
 
    void LoadService();
    void UnloadService();
    IEnhanceService* GetServiceInstance();
 
private:
    EnhanceServiceManager() = default;
    virtual ~EnhanceServiceManager();
 
    std::shared_mutex mutex_;
    void* handle_ { nullptr };
    DestroyFuncType destroyFunc_ { nullptr };
    IEnhanceService* service_ { nullptr };
};
}  // namespace OHOS::FileManagement::Backup
#endif