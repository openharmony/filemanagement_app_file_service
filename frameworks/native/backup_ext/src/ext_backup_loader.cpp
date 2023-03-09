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

#include "ext_backup_loader.h"

#include "ext_backup.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ExtBackupLoader::ExtBackupLoader() = default;
ExtBackupLoader::~ExtBackupLoader() = default;

AbilityRuntime::Extension *ExtBackupLoader::Create(const unique_ptr<AbilityRuntime::Runtime> &runtime) const
{
    HILOGI("Create as an BackupExtensionAbility(Loader)");
    return ExtBackup::Create(runtime);
}

map<string, string> ExtBackupLoader::GetParams()
{
    HILOGI("Register as an extension ability");
    return {
        // Type 即为 Extension 类型，定义在 ExtensionAbilityType 这一枚举类中。具体位置见 extension_ability_info.h
        {"type", "9"},
        {"name", "BackupExtensionAbility"},
    };
}

extern "C" __attribute__((visibility("default"))) void *OHOS_EXTENSION_GetExtensionModule()
{
    HILOGI("Load as a library");
    return &ExtBackupLoader::GetInstance();
}
} // namespace OHOS::FileManagement::Backup