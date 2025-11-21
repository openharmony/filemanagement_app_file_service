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

#ifndef BACKUP_SESSION_H
#define BACKUP_SESSION_H

#include <memory>
#include <ani.h>
#include "b_session_backup.h"
#include "general_callbacks.h"
#include "native_engine/native_engine.h"

namespace OHOS::FileManagement::Backup {
class BackupSession {
public:
    ~BackupSession() = default;
    BackupSession(const BackupSession&) = delete;
    BackupSession(BackupSession&&) = delete;
    BackupSession& operator=(const BackupSession&) = delete;
    BackupSession& operator=(BackupSession&&) = delete;
    static void Init(ani_env *aniEnv);

    std::unique_ptr<BSessionBackup> session;
    std::shared_ptr<GeneralCallbacks> callbacks;
};

class BackupSessionCleaner {
public:
    static void Clean(ani_env *aniEnv, ani_object object);
};
} // namespace OHOS::FileManagement::Backup
#endif // BACKUP_SESSION_H