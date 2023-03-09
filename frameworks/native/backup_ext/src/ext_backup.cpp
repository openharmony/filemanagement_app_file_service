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

#include "ext_backup.h"

#include "ext_backup_js.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
void ExtBackup::Init(const shared_ptr<AbilityRuntime::AbilityLocalRecord> &record,
                     const shared_ptr<AbilityRuntime::OHOSApplication> &application,
                     shared_ptr<AbilityRuntime::AbilityHandler> &handler,
                     const sptr<IRemoteObject> &token)
{
    HILOGI("Init the BackupExtensionAbility(Base)");
    AbilityRuntime::ExtensionBase<ExtBackupContext>::Init(record, application, handler, token);
}

ExtBackup *ExtBackup::Create(const unique_ptr<AbilityRuntime::Runtime> &runtime)
{
    HILOGI("Create as an BackupExtensionAbility(Base)");
    if (!runtime) {
        return new ExtBackup();
    }

    switch (runtime->GetLanguage()) {
        case AbilityRuntime::Runtime::Language::JS:
            return ExtBackupJs::Create(runtime);

        default:
            return new ExtBackup();
    }
}
} // namespace OHOS::FileManagement::Backup