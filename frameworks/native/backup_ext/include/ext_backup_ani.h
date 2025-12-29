/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef EXT_BACKUP_ANI_H
#define EXT_BACKUP_ANI_H

#include <memory>
#include "ext_backup_context.h"
#include "ext_backup.h"
#include "runtime.h"
#include "ets_native_reference.h"
#include "ets_runtime.h"

namespace OHOS::FileManagement::Backup {

class ExtBackupAni : public ExtBackup {
public:
    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
              const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
              std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
              const sptr<IRemoteObject> &token) override;

public:
    static ExtBackupAni *Create(const std::unique_ptr<AbilityRuntime::Runtime> &runtime);

    ErrCode OnBackup(std::function<void(ErrCode, std::string)> callback,
        std::function<void(ErrCode, const std::string)> callbackEx) override;
    ErrCode OnRestore(std::function<void(ErrCode, std::string)> callback,
        std::function<void(ErrCode, const std::string)> callbackEx) override;
    ErrCode GetBackupInfo(std::function<void(ErrCode, const std::string)> callback) override;
    ErrCode InvokeAppExtMethod(ErrCode, const std::string) override;
    ErrCode OnProcess(std::function<void(ErrCode, const std::string)> callback) override;
    ErrCode OnRelease(std::function<void(ErrCode, const std::string)> callback, int32_t scenario) override;
    ErrCode GetBackupCompatibilityInfo(std::function<void(ErrCode, const std::string)> callbackEx,
        std::string extInfo) override;
    ErrCode GetRestoreCompatibilityInfo(std::function<void(ErrCode, const std::string)> callbackEx,
        std::string extInfo) override

public:
    explicit ExtBackupAni(AbilityRuntime::ETSRuntime &runtime);
    virtual ~ExtBackupAni() override;

private:
    struct EtsRestoreInfo {
        ani_long code = 0;
        ani_string name;
        ani_string restoreInfo;
    };

    ErrCode CallEtsOnBackupEx(ani_env *env, std::string &result, std::string &exception);
    ErrCode CallEtsOnBackup(ani_env *env, std::string &exception);
    ErrCode CallEtsOnRestoreEx(ani_env *env, const EtsRestoreInfo &info, std::string &result, std::string &exception);
    ErrCode CallEtsOnRestore(ani_env *env, const EtsRestoreInfo &info, std::string &exception);
    ErrCode CallEtsOnProcess(ani_env *env, std::string &result, std::string &exception);
    ErrCode CallEtsGetBackupCompatibilityInfo(ani_env *env, std::string &result, std::string &exception);
    ErrCode CallEtsGetRestoreCompatibilityInfo(ani_env *env, std::string &result, std::string &exception);
    
    ErrCode CallEtsGetBackupInfo(ani_env *env, std::string &result, std::string &exception);
    ErrCode CallEtsOnRelease(ani_env *env, int32_t scenario, std::string &exception);
    void GetSrcPath(std::string &srcPath);
    void BindContext(std::shared_ptr<AppExecFwk::AbilityInfo> &abilityInfo,
    const std::string &moduleName, const std::string &srcPath);
    void UpdateExtBackupObj(std::shared_ptr<AppExecFwk::AbilityInfo> &abilityInfo,
        const std::string &moduleName, const std::string &srcPath);
    std::string GetErrorProperty(ani_env *aniEnv, ani_error aniError, const char *property);
    std::string GetETSError(ani_env *aniEnv);
    ErrCode CallObjectMethodVoid(ani_env *env, std::string &exception, const char *name, const char *signature, ...);
    ani_ref CallObjectMethodRef(ani_env *env, std::string &exception, const char *name, const char *signature, ...);

private:
    AbilityRuntime::ETSRuntime &etsRuntime_;
    std::shared_ptr<AppExecFwk::ETSNativeReference> etsAbilityObj_;
    std::atomic<bool> callExtDefaultFunc_ {false}; // extension default method, onBackup or onRestore
    std::atomic<bool> callJsExMethodDone_ {false};
    std::condition_variable callJsCon_;
    std::mutex callJsMutex_;
    std::mutex refMutex_;
    ani_vm *etsVm_ = nullptr;
};

} // namespace OHOS::FileManagement::Backup

#endif // EXT_BACKUP_ANI_H