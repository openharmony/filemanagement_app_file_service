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

#include <ani.h>
#include "backup_session.h"
#include "backup_session_transfer.h"
#include "incremental_backup_session.h"
#include "incr_backup_session_transfer.h"
#include "restore_session.h"
#include "restore_session_transfer.h"

namespace OHOS::FileManagement::Backup {
extern "C" {
    ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
    {
        ani_env *env;
        if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
            return ANI_ERROR;
        }
        IncreBackupSessionTransfer::Init(env);
        IncrementalBackupSession::Init(env);
        BackupSession::Init(env);
        BackupSessionTransfer::Init(env);
        RestoreSession::Init(env);
        RestoreSessionTransfer::Init(env);
        *result = ANI_VERSION_1;
        return ANI_OK;
    }
}
}