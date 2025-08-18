/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_SESSION_INCREMENTAL_BACKUP_N_EXPORTER_H
#define INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_SESSION_INCREMENTAL_BACKUP_N_EXPORTER_H

#include <memory>

#include "b_incremental_backup_session.h"
#include "general_callbacks.h"
#include "n_exporter.h"

namespace OHOS::FileManagement::Backup {
struct IncrBackupEntity {
    std::unique_ptr<BIncrementalBackupSession> session;
    std::shared_ptr<GeneralCallbacks> callbacks;
};

class SessionIncrementalBackupNExporter final : public LibN::NExporter {
public:
    inline static const std::string CLASS_NAME = "IncrementalBackupSession";
    inline static const std::string NAPI_CLASS_NAME = "NapiIncrementalBackupSession";

    bool Export() override;
    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value GetLocalCapabilities(napi_env env, napi_callback_info cbinfo);
    static napi_value GetBackupDataSize(napi_env env, napi_callback_info cbinfo);
    static napi_value AppendBundles(napi_env env, napi_callback_info cbinfo);
    static napi_value Release(napi_env env, napi_callback_info cbinfo);
    static napi_value Cancel(napi_env env, napi_callback_info cbinfo);
    static napi_value CleanBundleTempDir(napi_env env, napi_callback_info cbinfo);

    static napi_value ConstructorFromEntity(napi_env env, napi_callback_info cbinfo);
    static napi_value CreateByEntity(napi_env env, IncrBackupEntity* entity);

    SessionIncrementalBackupNExporter(napi_env env, napi_value exports);
    ~SessionIncrementalBackupNExporter() override;
};
} // namespace OHOS::FileManagement::Backup
#endif // INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_SESSION_INCREMENTAL_BACKUP_N_EXPORTER_H