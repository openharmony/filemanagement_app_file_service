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

#ifndef OHOS_FILEMGMT_BACKUP_EXT_BACKUP_H
#define OHOS_FILEMGMT_BACKUP_EXT_BACKUP_H

#include "ext_backup_context.h"
#include "extension_base.h"
#include "runtime.h"

namespace OHOS::FileManagement::Backup {
class ExtBackup : public AbilityRuntime::ExtensionBase<ExtBackupContext> {
public:
    /**
     * @brief Init the extension.
     *
     * @param record the extension record.
     * @param application the application info.
     * @param handler the extension handler.
     * @param token the remote token.
     */
    void Init(const std::shared_ptr<AbilityRuntime::AbilityLocalRecord> &record,
              const std::shared_ptr<AbilityRuntime::OHOSApplication> &application,
              std::shared_ptr<AbilityRuntime::AbilityHandler> &handler,
              const sptr<IRemoteObject> &token) override;

    /**
     * @brief Create Extension.
     *
     * @param runtime The runtime.
     * @return The ServiceExtension instance.
     */
    static ExtBackup *Create(const std::unique_ptr<AbilityRuntime::Runtime> &runtime);

public:
    ExtBackup() = default;
    ~ExtBackup() override = default;

private:
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_EXT_BACKUP_H