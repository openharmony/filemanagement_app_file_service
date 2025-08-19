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

#ifndef RESTORE_SESSION_H
#define RESTORE_SESSION_H

#include <memory>
#include <ani.h>
#include "b_incremental_restore_session.h"
#include "b_session_restore.h"
#include "general_callbacks.h"
#include "native_engine/native_engine.h"

namespace OHOS::FileManagement::Backup {
class RestoreSession {
public:
    ~RestoreSession() = default;
    RestoreSession(const RestoreSession&) = delete;
    RestoreSession(RestoreSession&&) = delete;
    RestoreSession& operator=(const RestoreSession&) = delete;
    RestoreSession& operator=(RestoreSession&&) = delete;
    static void Init(ani_env *aniEnv);

    std::unique_ptr<BSessionRestore> session;
    std::unique_ptr<BIncrementalRestoreSession> incrSession;
    std::shared_ptr<GeneralCallbacks> callbacks;
};

class RestoreSessionCleaner {
public:
    static void Clean(ani_env *aniEnv, ani_object object);
};
} // namespace OHOS::FileManagement::Backup
#endif // RESTORE_SESSION_H