/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INCR_BACKUP_SESSION_TRANSFER_H
#define INCR_BACKUP_SESSION_TRANSFER_H

#include <memory>
#include <ani.h>
#include "native_engine/native_engine.h"

namespace OHOS::FileManagement::Backup {
class IncreBackupSessionTransfer {
public:
    IncreBackupSessionTransfer() = default;
    ~IncreBackupSessionTransfer() = default;

    IncreBackupSessionTransfer(const IncreBackupSessionTransfer&) = delete;
    IncreBackupSessionTransfer(IncreBackupSessionTransfer&&) = delete;
    IncreBackupSessionTransfer& operator=(const IncreBackupSessionTransfer&) = delete;
    IncreBackupSessionTransfer& operator=(IncreBackupSessionTransfer&&) = delete;

    static void Init(ani_env *aniEnv);
    static ani_object TransferStaticSession(ani_env *aniEnv, ani_class aniCls, ani_object input);
    static ani_ref TransferDynamicSession(ani_env *aniEnv, ani_class aniCls, ani_object input);
};
} // namespace OHOS::FileManagement::Backup
#endif // INCR_BACKUP_SESSION_TRANSFER_H