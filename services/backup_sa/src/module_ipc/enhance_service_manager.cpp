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

#include "module_ipc/enhance_service_manager.h"

#include "b_radar/b_radar.h"
#include "filemgmt_libhilog.h"

#include <cstddef>
#include <dlfcn.h>
#include <memory>
#include <sstream>

namespace OHOS::FileManagement::Backup {
constexpr const char *ENHANCE_SERVICE_SO_NAME = "libbackup_ext_interface.z.so";

EnhanceServiceManager::~EnhanceServiceManager()
{
    UnloadService();
}

EnhanceServiceManager &EnhanceServiceManager::GetInstance()
{
    static EnhanceServiceManager gEnhanceServiceManager;
    return gEnhanceServiceManager;
}

void EnhanceServiceManager::LoadService()
{
    HILOGI("enter");
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (service_ != nullptr) {
        HILOGI("service ptr is not null");
        return;
    }
    std::stringstream errInfo;
    do {
        void *handle = dlopen(ENHANCE_SERVICE_SO_NAME, RTLD_LAZY);
        if (handle == nullptr) {
            errInfo << "fail to dlopen, err = " << dlerror();
            HILOGE("%{public}s", errInfo.str().c_str());
            break;
        }
        CreateFuncType createFunc = (CreateFuncType)dlsym(handle, "Create");
        if (createFunc == nullptr) {
            errInfo << "fail to dlsym Create, err = " << dlerror();
            HILOGE("%{public}s", errInfo.str().c_str());
            dlclose(handle);
            break;
        }
        DestroyFuncType destroyFunc = (DestroyFuncType)dlsym(handle, "Destroy");
        if (destroyFunc == nullptr) {
            errInfo << "fail to dlsym Destroy, err = " << dlerror();
            HILOGE("%{public}s", errInfo.str().c_str());
            dlclose(handle);
            break;
        }
        auto service = createFunc();
        if (service == nullptr) {
            errInfo << "fail to create service";
            HILOGE("%{public}s", errInfo.str().c_str());
            dlclose(handle);
            break;
        }

        handle_ = handle;
        service_ = service;
        destroyFunc_ = destroyFunc;
        HILOGI("success");
        return;
    } while (0);

    AppRadar::Info info("", "", errInfo.str());
    AppRadar::GetInstance().RecordDefaultFuncRes(
        info, "EnhanceServiceManager::LoadService", AppRadar::GetInstance().GetUserId(),
        BizStageBackup::BIZ_STAGE_GET_ENHANCE_SERVICE_FAIL, BError(BError::Codes::OK).GetCode());
}

void EnhanceServiceManager::UnloadService()
{
    HILOGI("enter");
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (destroyFunc_ != nullptr) {
        destroyFunc_(service_);
        service_ = nullptr;
    }

    if (handle_ != nullptr) {
        if (dlclose(handle_) != 0) {
            HILOGE("fail to dlclose %{public}s, errno = %{public}s", ENHANCE_SERVICE_SO_NAME, dlerror());
            return;
        }
        handle_ = nullptr;
    }

    destroyFunc_ = nullptr;
    HILOGI("success");
}

IEnhanceService *EnhanceServiceManager::GetServiceInstance()
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return service_;
}
}  // namespace OHOS::FileManagement::Backup
