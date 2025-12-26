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

#include "filemgmt_libhilog.h"
#include "ani_common_configuration.h"
#include "ani_common_want.h"
#include "ets_context_utils.h"
#include "ets_error_utils.h"
#include "ets_extension_context.h"
#include "ext_backup_context_ets.h"

namespace OHOS::FileManagement::Backup {
constexpr const char *EXT_BACKUP_CONTEXT_CLASS_NAME =
    "@ohos.file.BackupExtensionContext.BackupExtensionContext";
constexpr const char *CLEANER_CLASS_NAME = "@ohos.file.BackupExtensionContext.Cleaner";

bool BindNativeMethods(ani_env *env, ani_class &cls)
{
    ani_status status = ANI_ERROR;
    std::array functions = {
    ani_native_function { "nativeBackupDir", ":C{std.core.String}",
        reinterpret_cast<void *>(ExtBackupContextETS::NativeBackupDir) }
    };
    if ((status = env->Class_BindNativeMethods(cls, functions.data(), functions.size())) != ANI_OK
        && status != ANI_ALREADY_BINDED) {
        HILOGE("bind method status: %{public}d", status);
        return false;
    }
    ani_class cleanerCls = nullptr;
    status = env->FindClass(CLEANER_CLASS_NAME, &cleanerCls);
    if (status != ANI_OK || cleanerCls == nullptr) {
        HILOGE("Failed to find class, status: %{public}d", status);
        return false;
    }
    std::array CleanerMethods = {
        ani_native_function { "clean", nullptr, reinterpret_cast<void *>(ExtBackupContextETS::Finalizer) },
    };
    if ((status = env->Class_BindNativeMethods(cleanerCls, CleanerMethods.data(), CleanerMethods.size())) != ANI_OK
        && status != ANI_ALREADY_BINDED) {
        HILOGE("bind method status: %{public}d", status);
        return false;
    }
    return true;
}

ani_object CreateExtBackupETSContext(ani_env *env, std::shared_ptr<ExtBackupContext> &context)
{
    HILOGI("CreateETSInputMethodExtensionContext call");
    if (env == nullptr || context == nullptr) {
        HILOGE("null env or context");
        return nullptr;
    }
    ani_class cls = nullptr;
    ani_status status = ANI_ERROR;
    if ((status = env->FindClass(EXT_BACKUP_CONTEXT_CLASS_NAME, &cls)) != ANI_OK || cls == nullptr) {
        HILOGE("Failed to find class, status: %{public}d", status);
        return nullptr;
    }
    if (!BindNativeMethods(env, cls)) {
        HILOGE("Failed to BindNativeMethods");
        return nullptr;
    }
    ani_method method = nullptr;
    if ((status = env->Class_FindMethod(cls, "<ctor>", "l:", &method)) != ANI_OK || method == nullptr) {
        HILOGE("Failed to find constructor, status : %{public}d", status);
        return nullptr;
    }
    std::unique_ptr<ExtBackupContextETS> workContext =
        std::make_unique<ExtBackupContextETS>(context);
    auto distributeContextPtr = new std::weak_ptr<ExtBackupContext> (
        workContext->GetAbilityContext());
    if (distributeContextPtr == nullptr) {
        HILOGE("distributeContextPtr is nullptr");
        return nullptr;
    }
    ani_object contextObj = nullptr;
    if ((status = env->Object_New(cls, method, &contextObj, (ani_long)workContext.release())) != ANI_OK ||
        contextObj == nullptr) {
        HILOGE("Failed to create object, status : %{public}d", status);
        return nullptr;
    }
    if (!AbilityRuntime::ContextUtil::SetNativeContextLong(env, contextObj, (ani_long)(distributeContextPtr))) {
        HILOGE("Failed to setNativeContextLong ");
        return nullptr;
    }
    AbilityRuntime::ContextUtil::CreateEtsBaseContext(env, cls, contextObj, context);
    CreateEtsExtensionContext(env, cls, contextObj, context, context->GetAbilityInfo());
    return contextObj;
}

void ExtBackupContextETS::Finalizer(ani_env *env, ani_object obj)
{
    HILOGI("Finalizer");
    if (env == nullptr) {
        HILOGE("null env");
        return;
    }
    ani_long nativeEtsContextPtr;
    if (env->Object_GetFieldByName_Long(obj, "nativeEtsContext", &nativeEtsContextPtr) != ANI_OK) {
        HILOGE("Failed to get nativeEtsContext");
        return;
    }
    if (nativeEtsContextPtr != 0) {
        delete reinterpret_cast<ExtBackupContextETS *>(nativeEtsContextPtr);
    }
}

ExtBackupContextETS *ExtBackupContextETS::GetEtsAbilityContext(ani_env *env, ani_object obj)
{
    HILOGD("GetEtsAbilityContext");
    ani_class cls = nullptr;
    ani_long nativeContextLong;
    ani_field contextField = nullptr;
    ani_status status = ANI_ERROR;
    if (env == nullptr) {
        HILOGE("null env");
        return nullptr;
    }
    if ((status = env->FindClass(EXT_BACKUP_CONTEXT_CLASS_NAME, &cls)) != ANI_OK) {
        HILOGE("Failed to find class, status : %{public}d", status);
        return nullptr;
    }
    if ((status = env->Class_FindField(cls, "nativeEtsContext", &contextField)) != ANI_OK) {
        HILOGE("Failed to find field, status : %{public}d", status);
        return nullptr;
    }
    if ((status = env->Object_GetField_Long(obj, contextField, &nativeContextLong)) != ANI_OK) {
        HILOGE("Failed to get field, status : %{public}d", status);
        return nullptr;
    }
    auto weakContext = reinterpret_cast<ExtBackupContextETS *>(nativeContextLong);
    return weakContext;
}

ani_string ExtBackupContextETS::NativeBackupDir(ani_env *env, ani_object aniObj)
{
    HILOGI("NativeBackupDir");
    if (env == nullptr) {
        HILOGE("null env");
        return nullptr;
    }
    auto etsContext = ExtBackupContextETS::GetEtsAbilityContext(env, aniObj);
    if (etsContext == nullptr) {
        HILOGE("null ExtBackupContextETS");
        return nullptr;
    }
    return etsContext->GetBackupDir(env, aniObj);
}

ani_string ExtBackupContextETS::GetBackupDir(ani_env *env, ani_object aniObj)
{
    auto context = context_.lock();
    if (!context) {
        HILOGE("context is nullptr.");
        return nullptr;
    }
    std::string backupDir = context->GetBackupDir();
    return AppExecFwk::GetAniString(env, backupDir);
}
} // namespace OHOS::FileManagement::Backup