/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#ifndef INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_SESSION_RESTORE_N_EXPORTER_H
#define INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_SESSION_RESTORE_N_EXPORTER_H

#include <node_api.h>
#include <string>

#include "filemgmt_libn.h"

namespace OHOS::FileManagement::Backup {
class SessionRestoreNExporter final : public LibN::NExporter {
public:
    inline static const std::string className = "SessionRestore";

    bool Export() override;
    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value GetLocalCapabilities(napi_env env, napi_callback_info cbinfo);
    static napi_value AppendBundles(napi_env env, napi_callback_info cbinfo);
    static napi_value PublishFile(napi_env env, napi_callback_info cbinfo);
    static napi_value GetFileHandle(napi_env env, napi_callback_info cbinfo);
    static napi_value Release(napi_env env, napi_callback_info cbinfo);
    static napi_value Cancel(napi_env env, napi_callback_info cbinfo);
    static napi_value CleanBundleTempDir(napi_env env, napi_callback_info cbinfo);

    SessionRestoreNExporter(napi_env env, napi_value exports);
    ~SessionRestoreNExporter() override;
};
} // namespace OHOS::FileManagement::Backup
#endif // INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_SESSION_RESTORE_N_EXPORTER_H