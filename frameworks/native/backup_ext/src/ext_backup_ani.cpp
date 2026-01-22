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

#include "ext_backup_ani.h"

#include "ani_utils.h"
#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "filemgmt_libhilog.h"
#include "ext_backup_context_ets.h"
#include "ani_common_util.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

std::string ExtBackupAni::GetETSError(ani_env *aniEnv)
{
    HILOGI("GetETSErrorObject called");
    ani_boolean errorExists = ANI_FALSE;
    ani_status status = ANI_ERROR;
    std::string errorMsg;
    if ((status = aniEnv->ExistUnhandledError(&errorExists)) != ANI_OK) {
        HILOGE("ExistUnhandledError failed, status : %{public}d", status);
        return errorMsg;
    }
    if (errorExists == ANI_FALSE) {
        HILOGE("not exist error");
        return errorMsg;
    }
    ani_error aniError = nullptr;
    if ((status = aniEnv->GetUnhandledError(&aniError)) != ANI_OK) {
        HILOGE("GetUnhandledError failed, status : %{public}d", status);
        return errorMsg;
    }
    if ((status = aniEnv->ResetError()) != ANI_OK) {
        HILOGE("ResetError failed, status : %{public}d", status);
        return errorMsg;
    }
    errorMsg = GetErrorProperty(aniEnv, aniError, "<get>message");
    return errorMsg;
}

std::string ExtBackupAni::GetErrorProperty(ani_env *aniEnv, ani_error aniError, const char *property)
{
    HILOGI("GetErrorProperty called");
    const uint32_t MAX_STRING_LENGTH = 2048;
    std::string propertyValue;
    ani_status status = ANI_ERROR;
    ani_type errorType = nullptr;
    if ((status = aniEnv->Object_GetType(aniError, &errorType)) != ANI_OK) {
        HILOGE("Object_GetType failed, status : %{public}d", status);
        return propertyValue;
    }
    auto errorClass = static_cast<ani_class>(errorType);
    ani_method getterMethod = nullptr;
    if ((status = aniEnv->Class_FindMethod(errorClass, property, nullptr, &getterMethod)) != ANI_OK) {
        HILOGE("Class_FindMethod failed, status : %{public}d", status);
        return propertyValue;
    }
    ani_ref aniRef = nullptr;
    if ((status = aniEnv->Object_CallMethod_Ref(aniError, getterMethod, &aniRef)) != ANI_OK) {
        HILOGE("Object_CallMethod_Ref failed, status : %{public}d", status);
        return propertyValue;
    }
    ani_string aniString = reinterpret_cast<ani_string>(aniRef);
    ani_size sz {};
    if ((status = aniEnv->String_GetUTF8Size(aniString, &sz)) != ANI_OK) {
        HILOGE("String_GetUTF8Size failed, status : %{public}d", status);
        return propertyValue;
    }
    if (sz == 0 || sz > MAX_STRING_LENGTH) {
        HILOGE("Invalid string size: %{public}zu", sz);
        return propertyValue;
    }
    propertyValue.resize(sz + 1);
    if ((status = aniEnv->String_GetUTF8SubString(
        aniString, 0, sz, propertyValue.data(), propertyValue.size(), &sz))!= ANI_OK) {
        HILOGE("String_GetUTF8SubString failed, status : %{public}d", status);
        return propertyValue;
    }
    propertyValue.resize(sz);
    return propertyValue;
}

ExtBackupAni::ExtBackupAni(AbilityRuntime::ETSRuntime &runtime) : etsRuntime_(runtime)
{
    HILOGI("Create as an BackupExtensionAbility");
}

ExtBackupAni::~ExtBackupAni()
{
    HILOGI("ExtBackupAni::~ExtBackupAni.");
    auto env = etsRuntime_.GetAniEnv();
    if (env == nullptr) {
        HILOGI("env null");
        return;
    }
    if (etsAbilityObj_ == nullptr) {
        HILOGI("etsAbilityObj_ null");
        return;
    }
    std::lock_guard<std::mutex> lock(refMutex_);
    auto obj = std::atomic_exchange(&etsAbilityObj_,
    std::shared_ptr<AppExecFwk::ETSNativeReference>(nullptr));
    if (obj != nullptr && obj->aniRef != nullptr) {
        ani_ref ref = obj->aniRef;
        obj->aniRef = nullptr;
        env->GlobalReference_Delete(ref);
    }
}

void ExtBackupAni::Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
                        const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
                        std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
                        const sptr<IRemoteObject> &token)
{
    HILOGI("Init the BackupExtensionAbility");
    if (record == nullptr) {
        HILOGE("null localAbilityRecord");
        return;
    }
    auto abilityInfo = record->GetAbilityInfo();
    if (abilityInfo == nullptr) {
        HILOGE("null abilityInfo");
        return;
    }
    ExtBackup::Init(record, application, handler, token);

    std::string srcPath;
    GetSrcPath(srcPath);
    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    HILOGD("moduleName:%{public}s,srcPath:%{public}s, compileMode :%{public}d",
        moduleName.c_str(), srcPath.c_str(), abilityInfo_->compileMode);
    BindContext(abilityInfo, moduleName, srcPath);
}

void ExtBackupAni::GetSrcPath(std::string &srcPath)
{
    if (!Extension::abilityInfo_->isModuleJson) {
        srcPath.append(Extension::abilityInfo_->package);
        srcPath.append("/assets/js/");
        if (!Extension::abilityInfo_->srcPath.empty()) {
            srcPath.append(Extension::abilityInfo_->srcPath);
        }
        srcPath.append("/").append(Extension::abilityInfo_->name).append(".abc");
        return;
    }

    if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        auto pos = srcPath.rfind(".");
        if (pos != std::string::npos) {
            srcPath.erase(pos);
            srcPath.append(".abc");
        }
    }
}

void ExtBackupAni::UpdateExtBackupObj(
    std::shared_ptr<AppExecFwk::AbilityInfo> &abilityInfo, const std::string &moduleName, const std::string &srcPath)
{
    HILOGE("ExtBackupAni call");
    etsAbilityObj_ = etsRuntime_.LoadModule(moduleName, srcPath, abilityInfo->hapPath,
        abilityInfo->compileMode == AppExecFwk::CompileMode::ES_MODULE, false, abilityInfo_->srcEntrance);
    if (etsAbilityObj_ == nullptr) {
        HILOGE("null etsAbilityObj_");
        return;
    }
    HILOGE("ExtBackupAni End");
}

void ExtBackupAni::BindContext(std::shared_ptr<AppExecFwk::AbilityInfo> &abilityInfo,
    const std::string &moduleName, const std::string &srcPath)
{
    UpdateExtBackupObj(abilityInfo, moduleName, srcPath);
    if (etsAbilityObj_ == nullptr) {
        HILOGE("etsAbilityObj_ is null");
        return;
    }
    auto env = etsRuntime_.GetAniEnv();
    if (env == nullptr) {
        HILOGE("env null");
        return;
    }
    ani_vm *aniVm = nullptr;
    if (env->GetVM(&aniVm) != ANI_OK) {
        HILOG_INFO("GetVM error");
        return;
    }
    if (aniVm == nullptr) {
        HILOG_INFO("aniVm nullptr");
        return;
    }
    etsVm_ = aniVm;
    auto context = GetContext();
    if (context == nullptr) {
        HILOGE("get context error");
        return;
    }
    ani_ref contextObj = CreateExtBackupETSContext(env, context);
    if (contextObj == nullptr) {
        HILOGE("Create context obj error");
        return;
    }
    ani_ref contextGlobalRef = nullptr;
    ani_field field = nullptr;
    ani_status status = ANI_ERROR;
    std::lock_guard<std::mutex> lock(refMutex_);
    if ((status = env->GlobalReference_Create(contextObj, &contextGlobalRef)) != ANI_OK) {
        HILOGE("GlobalReference_Create failed, status : %{public}d", status);
        return;
    }
    if ((status = env->Class_FindField(etsAbilityObj_->aniCls, "context", &field)) != ANI_OK) {
        HILOGE("Class_FindField failed, status : %{public}d", status);
        return;
    }
    if ((status = env->Object_SetField_Ref(etsAbilityObj_->aniObj, field, contextGlobalRef)) != ANI_OK) {
        HILOGE("Object_SetField_Ref failed, status : %{public}d", status);
        env->GlobalReference_Delete(contextGlobalRef);
        contextGlobalRef = nullptr;
        return;
    }
}

ExtBackupAni *ExtBackupAni::Create(const std::unique_ptr<AbilityRuntime::Runtime> &runtime)
{
    HILOGI("Create as an BackupExtensionAbility");
    return new ExtBackupAni(static_cast<AbilityRuntime::ETSRuntime &>(*runtime));
}

ErrCode ExtBackupAni::InvokeAppExtMethod(ErrCode errCode, const std::string result)
{
    HILOGI("Start Get onBackupEx/onRestoreEx method result, errCode: %{public}d, result: %{public}s",
        errCode, result.c_str());
    if ((result.size() == 0) && (errCode == BError(BError::Codes::OK))) {
        callExtDefaultFunc_.store(true);
    } else {
        callExtDefaultFunc_.store(false);
    }
    callJsExMethodDone_.store(true);
    std::unique_lock<std::mutex> lock(callJsMutex_);
    callJsCon_.notify_one();
    HILOGI("End Get App onBackupEx/onRestoreEx method result");
    return ERR_OK;
}

ErrCode ExtBackupAni::OnBackup(function<void(ErrCode, std::string)> callback,
                               std::function<void(ErrCode, const std::string)> callbackEx)
{
    HILOGI("ExtBackupAni OnBackup");
    BExcepUltils::BAssert(etsAbilityObj_, BError::Codes::EXT_BROKEN_FRAMEWORK, "etsAbilityObj_ is nullptr");
    BExcepUltils::BAssert(callback, BError::Codes::EXT_BROKEN_FRAMEWORK, "OnBackup callback is nullptr.");
    BExcepUltils::BAssert(callbackEx, BError::Codes::EXT_BROKEN_FRAMEWORK, "OnBackup callbackEx is nullptr.");
    callExtDefaultFunc_.store(false);
    callJsExMethodDone_.store(false);

    std::string result;
    std::string exception;
    bool isAttachThread = false;
    ani_env *env = AppExecFwk::AttachAniEnv(etsVm_, isAttachThread);
    if (env == nullptr) {
        HILOG_ERROR("env null");
        return EINVAL;
    }
    if (CallEtsOnBackupEx(env, result, exception) == ERR_OK) {
        callbackEx(BError(BError::Codes::OK), result);
    } else {
        callbackEx(BError(BError::Codes::EXT_THROW_EXCEPTION), exception);
    }

    std::unique_lock<std::mutex> lock(callJsMutex_);
    callJsCon_.wait(lock, [this] { return callJsExMethodDone_.load(); });

    if (!callExtDefaultFunc_.load()) {
        AppExecFwk::DetachAniEnv(etsVm_, isAttachThread);
        HILOGI("Call Js method onBackupEx done");
        return ERR_OK;
    }
    exception = "";
    ErrCode ret = CallEtsOnBackup(env, exception);
    if (ret == ERR_OK) {
        callback(BError(BError::Codes::OK), "");
    } else {
        callback(BError(BError::Codes::EXT_THROW_EXCEPTION), "call failed");
    }
    AppExecFwk::DetachAniEnv(etsVm_, isAttachThread);
    return ret;
}

ErrCode ExtBackupAni::OnRestore(std::function<void(ErrCode, std::string)> callback,
                                std::function<void(ErrCode, const std::string)> callbackEx)
{
    HILOGI("BackupExtensionAbility(ETS) OnRestore.");
    BExcepUltils::BAssert(etsAbilityObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
        "The app does not provide the onRestore interface.");
    BExcepUltils::BAssert(callback, BError::Codes::EXT_BROKEN_FRAMEWORK, "OnRestore callback is nullptr.");
    BExcepUltils::BAssert(callbackEx, BError::Codes::EXT_BROKEN_FRAMEWORK, "OnRestore callbackEx is nullptr.");
    callExtDefaultFunc_.store(false);
    callJsExMethodDone_.store(false);
    std::string result;
    std::string exception;
    bool isAttachThread = false;
    ani_env *env = AppExecFwk::AttachAniEnv(etsVm_, isAttachThread);
    if (env == nullptr) {
        HILOG_ERROR("env null");
        return EINVAL;
    }
    EtsRestoreInfo info = {appVersionCode_, AppExecFwk::GetAniString(env, appVersionStr_),
        AppExecFwk::GetAniString(env, restoreExtInfo_)};
    if (CallEtsOnRestoreEx(env, info, result, exception) == ERR_OK) {
        callbackEx(BError(BError::Codes::OK), result);
    } else {
        callbackEx(BError(BError::Codes::EXT_THROW_EXCEPTION), exception);
    }

    std::unique_lock<std::mutex> lock(callJsMutex_);
    callJsCon_.wait(lock, [this] { return callJsExMethodDone_.load(); });

    if (!callExtDefaultFunc_.load()) {
        HILOGI("Call Js method onBackupEx done");
        AppExecFwk::DetachAniEnv(etsVm_, isAttachThread);
        return ERR_OK;
    }
    exception = "";
    ErrCode ret = CallEtsOnRestore(env, info, exception);
    if (ret == ERR_OK) {
        callback(BError(BError::Codes::OK), "");
    } else {
        callback(BError(BError::Codes::EXT_THROW_EXCEPTION), exception);
    }
    AppExecFwk::DetachAniEnv(etsVm_, isAttachThread);
    return ret;
}

ErrCode ExtBackupAni::GetBackupCompatibilityInfo(std::function<void(ErrCode, const std::string)> callbackEx,
    std::string extInfo)
{
    HILOGI("BackupExtensionAbility(ETS) GetBackupCompatibilityInfo begin.");
    BExcepUltils::BAssert(etsAbilityObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
        "The app does not provide the GetBackupCompatibilityInfo interface.");
    std::string result;
    std::string exception;
    bool isAttachThread = false;
    ani_env *env = AppExecFwk::AttachAniEnv(etsVm_, isAttachThread);
    if (env == nullptr) {
        HILOG_ERROR("env null");
        return EINVAL;
    }
    ErrCode ret = CallEtsGetBackupCompatibilityInfo(env, result, exception);
    if (ret == ERR_OK) {
        callbackEx(BError(BError::Codes::OK), result);
    } else {
        callbackEx(BError(BError::Codes::EXT_THROW_EXCEPTION), exception);
    }
    AppExecFwk::DetachAniEnv(etsVm_, isAttachThread);
    return ret;
}

ErrCode ExtBackupAni::GetRestoreCompatibilityInfo(std::function<void(ErrCode, const std::string)> callbackEx,
    std::string extInfo)
{
    HILOGI("BackupExtensionAbility(ETS) GetRestoreCompatibilityInfo begin.");
    BExcepUltils::BAssert(etsAbilityObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
        "The app does not provide the GetRestoreCompatibilityInfo interface.");
    std::string result;
    std::string exception;
    bool isAttachThread = false;
    ani_env *env = AppExecFwk::AttachAniEnv(etsVm_, isAttachThread);
    if (env == nullptr) {
        HILOG_ERROR("env null");
        return EINVAL;
    }
    ErrCode ret = CallEtsGetRestoreCompatibilityInfo(env, result, exception);
    if (ret == ERR_OK) {
        callbackEx(BError(BError::Codes::OK), result);
    } else {
        callbackEx(BError(BError::Codes::EXT_THROW_EXCEPTION), exception);
    }
    AppExecFwk::DetachAniEnv(etsVm_, isAttachThread);
    return ret;
}

ErrCode ExtBackupAni::OnRelease(std::function<void(ErrCode, const std::string)> callback, int32_t scenario)
{
    HILOGI("BackupExtensionAbility(ETS) OnRestore.");
    BExcepUltils::BAssert(etsAbilityObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
        "The app does not provide the onRestore interface.");
    std::string exception;
    bool isAttachThread = false;
    ani_env *env = AppExecFwk::AttachAniEnv(etsVm_, isAttachThread);
    if (env == nullptr) {
        HILOG_ERROR("env null");
        return EINVAL;
    }
    ErrCode ret = CallEtsOnRelease(env, scenario, exception);
    if (ret == ERR_OK) {
        callback(BError(BError::Codes::OK), "");
    } else {
        callback(BError(BError::Codes::EXT_THROW_EXCEPTION), exception);
    }
    AppExecFwk::DetachAniEnv(etsVm_, isAttachThread);
    return ret;
}

ErrCode ExtBackupAni::GetBackupInfo(std::function<void(ErrCode, const std::string)> callback)
{
    HILOGI("BackupExtensionAbility(ETS) GetBackupInfo begin.");
    BExcepUltils::BAssert(etsAbilityObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
                          "The app does not provide the GetBackupInfo interface.");
    std::string result;
    std::string exception;
    bool isAttachThread = false;
    ani_env *env = AppExecFwk::AttachAniEnv(etsVm_, isAttachThread);
    if (env == nullptr) {
        HILOG_ERROR("env null");
        return EINVAL;
    }
    ErrCode ret = CallEtsGetBackupInfo(env, result, exception);
    if (ret == ERR_OK) {
        callback(BError(BError::Codes::OK), result);
    } else {
        callback(BError(BError::Codes::EXT_THROW_EXCEPTION), exception);
    }
    AppExecFwk::DetachAniEnv(etsVm_, isAttachThread);
    return ret;
}

ErrCode ExtBackupAni::OnProcess(std::function<void(ErrCode, const std::string)> callback)
{
    HILOGI("BackupExtensionAbility(ETS) OnProcess begin.");
    BExcepUltils::BAssert(etsAbilityObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
                          "The app does not provide the OnProcess interface.");
    std::string result;
    std::string exception;
    bool isAttachThread = false;
    ani_env *env = AppExecFwk::AttachAniEnv(etsVm_, isAttachThread);
    if (env == nullptr) {
        HILOG_ERROR("env null");
        return EINVAL;
    }
    ErrCode ret = CallEtsOnProcess(env, result, exception);
    if (ret == ERR_OK) {
        callback(BError(BError::Codes::OK), result);
    } else {
        callback(BError(BError::Codes::EXT_THROW_EXCEPTION), exception);
    }
    AppExecFwk::DetachAniEnv(etsVm_, isAttachThread);
    return ret;
}

ErrCode ExtBackupAni::CallEtsOnProcess(ani_env *env, std::string &result, std::string &exception)
{
    ani_ref aniRef = CallObjectMethodRef(env, exception, "onProcess", ":C{std.core.String}");
    if (aniRef == nullptr) {
        HILOGE("CallObjectMethodRef failed");
        return EINVAL;
    }
    ani_string aniString = reinterpret_cast<ani_string>(aniRef);
    if (!AppExecFwk::GetStdString(env, aniString, result)) {
        HILOGE("GetStdString failed");
        return EINVAL;
    }
    return ERR_OK;
}

ErrCode ExtBackupAni::CallEtsGetBackupInfo(ani_env *env, std::string &result, std::string &exception)
{
    ani_ref aniRef = CallObjectMethodRef(env, exception, "getBackupInfo", ":C{std.core.String}");
    if (aniRef == nullptr) {
        HILOGE("CallObjectMethodRef failed");
        return EINVAL;
    }
    ani_string aniString = reinterpret_cast<ani_string>(aniRef);
    if (!AppExecFwk::GetStdString(env, aniString, result)) {
        HILOGE("GetStdString failed");
        return EINVAL;
    }
    return ERR_OK;
}

ErrCode ExtBackupAni::CallEtsOnRelease(ani_env *env, int32_t scenario, std::string &exception)
{
    return CallObjectMethodVoid(env, exception, "etsCallOnRelease", "i:", (ani_int)scenario);
}

ErrCode ExtBackupAni::CallEtsOnBackupEx(ani_env *env, std::string &result, std::string &exception)
{
    ani_string clsStr = AppExecFwk::GetAniString(env, backupExtInfo_);
    if (clsStr == nullptr) {
        HILOGE("GetAniString failed");
        return EINVAL;
    }
    ani_ref aniRef = CallObjectMethodRef(env, exception,
        "etsCallOnBackupEx", "C{std.core.String}:C{std.core.String}", clsStr);
    if (aniRef == nullptr) {
        HILOGE("CallObjectMethodRef failed");
        return EINVAL;
    }
    ani_string aniString = reinterpret_cast<ani_string>(aniRef);
    if (!AppExecFwk::GetStdString(env, aniString, result)) {
        HILOGE("GetStdString failed");
        return EINVAL;
    }
    return ERR_OK;
}

ErrCode ExtBackupAni::CallEtsOnBackup(ani_env *env, std::string &exception)
{
    return CallObjectMethodVoid(env, exception, "onBackup", ":");
}

ErrCode ExtBackupAni::CallEtsOnRestoreEx(
    ani_env *env, const EtsRestoreInfo &info, std::string &result, std::string &exception)
{
    const std::string className = "@ohos.application.BackupExtensionAbility.BundleVersionInner";
    ani_object bundleVersionObj = AniObjectUtils::Create(env, className.c_str(), info.code, info.name);
    if (nullptr == bundleVersionObj) {
        HILOGE("Failed to Create the BundleVersionInner");
        return EINVAL;
    }

    ani_ref aniRef = CallObjectMethodRef(env, exception, "etsCallOnRestoreEx",
        "C{@ohos.application.BackupExtensionAbility.BundleVersionInner}C{std.core.String}:C{std.core.String}",
        bundleVersionObj, info.restoreInfo);
    if (aniRef == nullptr) {
        HILOGE("CallObjectMethodRef failed");
        return EINVAL;
    }
    ani_string aniString = reinterpret_cast<ani_string>(aniRef);
    if (!AppExecFwk::GetStdString(env, aniString, result)) {
        HILOGE("GetStdString failed");
        return EINVAL;
    }
    return ERR_OK;
}

ErrCode ExtBackupAni::CallEtsOnRestore(ani_env *env, const EtsRestoreInfo &info, std::string &exception)
{
    const std::string className = "@ohos.application.BackupExtensionAbility.BundleVersionInner";
    ani_object obj = AniObjectUtils::Create(env, className.c_str(), info.code, info.name);
    if (nullptr == obj) {
        HILOGE("Failed to Create the BundleVersionInner");
        return EINVAL;
    }

    return CallObjectMethodVoid(
        env, exception, "onRestore", "C{@ohos.application.BackupExtensionAbility.BundleVersionInner}:", obj);
}

ErrCode ExtBackupAni::CallEtsGetBackupCompatibilityInfo(ani_env *env, std::string &result, std::string &exception)
{
    ani_string clsStr = AppExecFwk::GetAniString(env, backupExtInfo_);
    if (clsStr == nullptr) {
        HILOGE("GetAniString failed");
        return EINVAL;
    }
    ani_ref aniRef = CallObjectMethodRef(env, exception,
        "etsCallGetBackupCompatibilityInfo", "C{std.core.String}:C{std.core.String}", clsStr);
    if (aniRef == nullptr) {
        HILOGE("CallObjectMethodRef failed");
        return EINVAL;
    }
    ani_string aniString = reinterpret_cast<ani_string>(aniRef);
    if (!AppExecFwk::GetStdString(env, aniString, result)) {
        HILOGE("GetStdString failed");
        return EINVAL;
    }
    return ERR_OK;
}

ErrCode ExtBackupAni::CallEtsGetRestoreCompatibilityInfo(ani_env *env, std::string &result, std::string &exception)
{
    ani_string clsStr = AppExecFwk::GetAniString(env, backupExtInfo_);
    if (clsStr == nullptr) {
        HILOGE("GetAniString failed");
        return EINVAL;
    }
    ani_ref aniRef = CallObjectMethodRef(env, exception,
        "etsCallGetRestoreCompatibilityInfo", "C{std.core.String}:C{std.core.String}", clsStr);
    if (aniRef == nullptr) {
        HILOGE("CallObjectMethodRef failed");
        return EINVAL;
    }
    ani_string aniString = reinterpret_cast<ani_string>(aniRef);
    if (!AppExecFwk::GetStdString(env, aniString, result)) {
        HILOGE("GetStdString failed");
        return EINVAL;
    }
    return ERR_OK;
}

ErrCode ExtBackupAni::CallObjectMethodVoid(
    ani_env *env, std::string &exception, const char *name, const char *signature, ...)
{
    HILOGI("CallObjectMethodVoid %{public}s", name);
    ani_status status = ANI_ERROR;
    ani_method method = nullptr;
    if ((status = env->Class_FindMethod(etsAbilityObj_->aniCls, name, signature, &method)) != ANI_OK) {
        HILOGE("Class_FindMethod status : %{public}d", status);
        return EINVAL;
    }
    if (method == nullptr) {
        return EINVAL;
    }
    struct VaListGuard {
        va_list& list;
        explicit VaListGuard(va_list& v) : list(v) {}
        ~VaListGuard()
        {
            va_end(list);
        }
    };
    va_list args;
    va_start(args, signature);
    VaListGuard guard(args);
    if ((status = env->Object_CallMethod_Void_V(etsAbilityObj_->aniObj, method, args)) != ANI_OK) {
        HILOGE("Object_CallMethod_Void_V status : %{public}d", status);
        exception = GetETSError(env);
        return EINVAL;
    }
    return ERR_OK;
}

ani_ref ExtBackupAni::CallObjectMethodRef(
    ani_env *env, std::string &exception, const char *name, const char *signature, ...)
{
    HILOGI("CallObjectMethodRef %{public}s", name);
    ani_status status = ANI_ERROR;
    ani_method method = nullptr;
    if ((status = env->Class_FindMethod(etsAbilityObj_->aniCls, name, signature, &method)) != ANI_OK) {
        HILOGE("Class_FindMethod status : %{public}d", status);
        return nullptr;
    }
    if (method == nullptr) {
        return nullptr;
    }

    struct VaListGuard {
        va_list& list;
        explicit VaListGuard(va_list& v) : list(v) {}
        ~VaListGuard()
        {
            va_end(list);
        }
    };
    ani_ref res = nullptr;
    va_list args;
    va_start(args, signature);
    VaListGuard guard(args);
    if ((status = env->Object_CallMethod_Ref_V(etsAbilityObj_->aniObj, method, &res, args)) != ANI_OK) {
        HILOGE("Object_CallMethod_Ref_V status : %{public}d", status);
        exception = GetETSError(env);
        return nullptr;
    }
    return res;
}
} // namespace OHOS::FileManagement::Backup