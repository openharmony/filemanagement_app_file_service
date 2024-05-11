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

#ifndef OHOS_FILEMGMT_BACKUP_EXT_BACKUP_CONTEXT_JS_H
#define OHOS_FILEMGMT_BACKUP_EXT_BACKUP_CONTEXT_JS_H

#include "ext_backup_context.h"

#include "js_runtime_utils.h"

namespace OHOS::FileManagement::Backup {
class ExtBackupContextJS final {
public:
    explicit ExtBackupContextJS(const std::shared_ptr<ExtBackupContext>& context) : context_(context) {}
    ~ExtBackupContextJS() = default;

    static napi_value GetBackupDir(napi_env env, napi_callback_info info);
    static void Finalizer(napi_env env, void* data, void* hint);
private:
    napi_value OnGetBackupDir(napi_env env, [[maybe_unused]]AbilityRuntime::NapiCallbackInfo& info);
    std::weak_ptr<ExtBackupContext> context_;
};

napi_value CreateExtBackupJsContext(napi_env env, std::shared_ptr<ExtBackupContext> context);
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_EXT_BACKUP_CONTEXT_JS_H