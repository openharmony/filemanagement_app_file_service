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

#ifndef INCREMENTAL_BACKUP_SESSION_H
#define INCREMENTAL_BACKUP_SESSION_H

#include <memory>
#include <vector>
#include <ani.h>
#include "b_incremental_backup_session.h"
#include "general_callbacks.h"
#include "native_engine/native_engine.h"
#include "session_incremental_backup_n_exporter.h"

namespace OHOS::FileManagement::Backup {
class IncrementalBackupSession {
public:
    ~IncrementalBackupSession() = default;

    IncrementalBackupSession(const IncrementalBackupSession&) = delete;
    IncrementalBackupSession(IncrementalBackupSession&&) = delete;
    IncrementalBackupSession& operator=(const IncrementalBackupSession&) = delete;
    IncrementalBackupSession& operator=(IncrementalBackupSession&&) = delete;

    static void Init(ani_env *aniEnv);
    static int32_t checkPermission(ani_env *aniEnv);
    static ani_int GetLocalCapabilities(ani_env *aniEnv, ani_object object);
    static ani_int GetBackupDataSize(ani_env *aniEnv, ani_object object, ani_boolean isPreciseScan,
        ani_array bundleNames);
    static ani_int AppendBundles(ani_env *aniEnv, ani_object object, ani_array bundleInfos);
    static ani_int Release(ani_env *aniEnv, ani_object object);
    static ani_int Cancel(ani_env *aniEnv, ani_object object, ani_string bundleName);
    static ani_int CleanBundleTempDir(ani_env *aniEnv, ani_object object, ani_string bundleName);

    static bool ParseIncrDataFromAniArray(ani_env *aniEnv, ani_array bundleInfos,
        std::vector<BIncrementalData>& result);
    static bool ParseBIncrementalData(ani_env *aniEnv, ani_object obj, BIncrementalData& data);

    std::unique_ptr<BIncrementalBackupSession> session;
    std::shared_ptr<GeneralCallbacks> callbacks;
};

class IncrBackupSessionCleaner {
public:
    static void Clean(ani_env *aniEnv, ani_object object);
};
} // namespace OHOS::FileManagement::Backup
#endif // INCREMENTAL_BACKUP_SESSION_H