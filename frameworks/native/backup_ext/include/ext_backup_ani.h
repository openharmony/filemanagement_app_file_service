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

public:
    explicit ExtBackupAni(AbilityRuntime::Runtime &runtime);
    virtual ~ExtBackupAni() override;

private:
    ErrCode CallEtsOnBackup();
    ErrCode CallEtsOnRestore();

private:
    AbilityRuntime::ETSRuntime &stsRuntime_;
    std::unique_ptr<AppExecFwk::ETSNativeReference> etsObj_;
};

} // namespace OHOS::FileManagement::Backup

#endif // EXT_BACKUP_ANI_H