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

#include "ext_backup_context_js.h"

#include "filemgmt_libhilog.h"
#include "js_extension_context.h"
#include "js_runtime.h"

namespace OHOS::FileManagement::Backup {
napi_value CreateExtBackupJsContext(napi_env env, std::shared_ptr<ExtBackupContext> context)
{
    if (context == nullptr) {
        HILOGE("Failed to CreateExtBackupJsContext, context is nullptr.");
        return nullptr;
    }
    std::shared_ptr<OHOS::AppExecFwk::AbilityInfo> abilityInfo = context->GetAbilityInfo();
    napi_value object = CreateJsExtensionContext(env, context, abilityInfo);
    if (object == nullptr) {
        HILOGE("Failed to CreateJsServiceExtensionContext, context is nullptr.");
        return nullptr;
    }
    std::unique_ptr<ExtBackupContextJS> jsContext = std::make_unique<ExtBackupContextJS>(context);
    napi_wrap(env, object, jsContext.release(), ExtBackupContextJS::Finalizer, nullptr, nullptr);
    AbilityRuntime::BindNativeProperty(env, object, "backupDir", ExtBackupContextJS::GetBackupDir);
    return object;
}

napi_value ExtBackupContextJS::GetBackupDir(napi_env env, napi_callback_info info)
{
    using namespace AbilityRuntime;
    HILOGI("GetBackupDir Called.");
    GET_NAPI_INFO_AND_CALL(env, info, ExtBackupContextJS, OnGetBackupDir);
}

void ExtBackupContextJS::Finalizer(napi_env env, void* data, void* hint)
{
    HILOGI("Finalizer Called.");
    std::unique_ptr<ExtBackupContextJS>(static_cast<ExtBackupContextJS*>(data));
}

napi_value ExtBackupContextJS::OnGetBackupDir(napi_env env, [[maybe_unused]]AbilityRuntime::NapiCallbackInfo& info)
{
    napi_value result = nullptr;
    auto context = context_.lock();
    if (!context) {
        HILOGE("context is nullptr.");
        return nullptr;
    }
    std::string backupDir = context->GetBackupDir();
    napi_create_string_utf8(env, backupDir.c_str(), backupDir.size(), &result);
    return result;
}
} // namespace OHOS::FileManagement::Backup