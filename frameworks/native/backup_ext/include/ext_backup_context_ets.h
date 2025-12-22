/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_EXT_BACKUP_CONTEXT_ETS_H
#define OHOS_FILEMGMT_BACKUP_EXT_BACKUP_CONTEXT_ETS_H

#include "ani.h"
#include "ext_backup_context.h"

namespace OHOS::FileManagement::Backup {
ani_object CreateExtBackupETSContext(ani_env *env, std::shared_ptr<ExtBackupContext> &context);

class ExtBackupContextETS final {
public:
    explicit ExtBackupContextETS(const std::shared_ptr<ExtBackupContext>& context) : context_(context) {}
    ~ExtBackupContextETS() = default;

    static ExtBackupContextETS *GetEtsAbilityContext(ani_env *env, ani_object obj);
    static void Finalizer(ani_env *env, ani_object obj);
    static ani_string NativeBackupDir(ani_env *env, ani_object aniObj);
    std::weak_ptr<ExtBackupContext> GetAbilityContext()
    {
        return context_;
    }
private:
    ani_string GetBackupDir(ani_env *env, ani_object obj);
    std::weak_ptr<ExtBackupContext> context_;
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_EXT_BACKUP_CONTEXT_ETS_H