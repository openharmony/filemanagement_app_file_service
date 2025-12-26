/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#include "ani.h"
#include "uv.h"
#include "native_reference.h"
#include "native_value.h"
#include "general_callbacks_taihe.h"
#include "backup_file_info.h"
#include "unique_fd.h"
#include "taihe/runtime.hpp"
#include "taihe/platform/ani.hpp"
#include "ohos.file.backup.impl.hpp"
#include "ohos.file.backup.proj.hpp"
#include "ohos.file.backup.ani.hpp"
#include "ohos.file.backup.File.proj.0.hpp"
#include "ohos.file.backup.File.proj.1.hpp"
#include "ohos.file.backup.File.proj.2.hpp"
#include "ohos.file.backup.File.ani.0.hpp"
#include "ohos.file.backup.File.ani.1.hpp"

#define  ONE_PARAMS 1
#define  TWO_PARAMS 2

namespace OHOS::FileManagement::Backup::TAIHE {
using namespace std;

ani_object WrapBusinessError(ani_env* env, const std::string& msg)
{
    ani_class cls {};
    ani_method method {};
    ani_object obj = nullptr;
    ani_status status = ANI_ERROR;
    if (env == nullptr) {
        return nullptr;
    }

    ani_string aniMsg = nullptr;
    if ((status = env->String_NewUTF8(msg.c_str(), msg.size(), &aniMsg)) != ANI_OK) {
        return nullptr;
    }

    ani_ref undefRef;
    if ((status = env->GetUndefined(&undefRef)) != ANI_OK) {
        return nullptr;
    }

    if ((status = env->FindClass("Lescompat/Error;", &cls)) != ANI_OK) {
        return nullptr;
    }
    if ((status = env->Class_FindMethod(cls, "<ctor>", "Lstd/core/String;Lescompat/ErrorOptions;:V", &method)) !=
        ANI_OK) {
        return nullptr;
    }

    if ((status = env->Object_New(cls, method, &obj, aniMsg, undefRef)) != ANI_OK) {
        return nullptr;
    }
    return obj;
}

ani_ref CreateBusinessError(ani_env* env, ani_int code, const std::string& msg)
{
    ani_class cls;
    ani_status status = ANI_OK;
    if ((status = env->FindClass("L@ohos/base/BusinessError;", &cls)) != ANI_OK) {
        return nullptr;
    }
    ani_method ctor;
    if ((status = env->Class_FindMethod(cls, "<ctor>", "DLescompat/Error;:V", &ctor)) != ANI_OK) {
        return nullptr;
    }
    ani_object error = WrapBusinessError(env, msg);
    if (error == nullptr) {
        return nullptr;
    }
    ani_object obj = nullptr;
    ani_double dCode(code);
    if ((status = env->Object_New(cls, ctor, &obj, dCode, error)) != ANI_OK) {
        return nullptr;
    }
    return reinterpret_cast<ani_ref>(obj);
}

ani_status ExecAsyncCallBack(ani_env *env, ani_object businessError,
    ani_object param, ani_object callbackFunc)
{
    ani_status status = ANI_ERROR;
    ani_ref ani_argv[] = {businessError, param};
    ani_ref ani_result;
    ani_class cls;
    if ((status = env->FindClass("Lstd/core/Function2;", &cls)) != ANI_OK) {
        return status;
    }
    ani_boolean ret;
    env->Object_InstanceOf(callbackFunc, cls, &ret);
    if (!ret) {
        return status;
    }
    if ((status = env->FunctionalObject_Call(static_cast<ani_fn_object>(callbackFunc), TWO_PARAMS,
        ani_argv, &ani_result)) != ANI_OK) {
        return status;
    }
    return status;
}

ani_status ExecCallBack(ani_env *env, ani_object param, ani_object callbackFunc)
{
    ani_status status = ANI_ERROR;
    ani_ref ani_argv[] = {param};
    ani_ref ani_result;
    ani_class cls;
    if ((status = env->FindClass("Lstd/core/Function1;", &cls)) != ANI_OK) {
        return status;
    }
    ani_boolean ret;
    env->Object_InstanceOf(callbackFunc, cls, &ret);
    if (!ret) {
        return status;
    }
    if ((status = env->FunctionalObject_Call(static_cast<ani_fn_object>(callbackFunc), ONE_PARAMS,
        ani_argv, &ani_result)) != ANI_OK) {
        return status;
    }
    return status;
}

void TaiheGeneralCallbacks::onFileReady(const BFileInfo& info, UniqueFd fd, ErrCode code)
{
    if (!vm_) {
        return;
    }
    ani_env* env = nullptr;
    ani_options aniArgs {0, nullptr};
    if (ANI_OK != vm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env)) {
        env = nullptr;
        if (ANI_OK != vm_->GetEnv(ANI_VERSION_1, &env)) {
            return;
        }
    }

    ani_string ani_owner = {};
    env->String_NewUTF8(info.owner.c_str(), info.owner.size(), &ani_owner);

    ani_string ani_url = {};
    env->String_NewUTF8(info.fileName.c_str(), info.fileName.size(), &ani_url);

    ani_int ani_fd = static_cast<ani_int>(fd.Get());
    ani_int ani_manifestFd = static_cast<ani_int>(info.sn);
    ani_object ani_obj = {};

    env->Object_New(TH_ANI_FIND_CLASS(env, "@ohos.file.backup.backup._taihe_File_inner"),
        TH_ANI_FIND_CLASS_METHOD(env, "@ohos.file.backup.backup._taihe_File_inner", "<ctor>", nullptr),
        &ani_obj, ani_owner, ani_url, ani_fd, ani_manifestFd);
    ani_ref businessError = CreateBusinessError(env, static_cast<ani_int>(code), std::string("onFileReady error"));
    ani_object callback = reinterpret_cast<ani_object>(*getRef());

    ani_ref method = {};
    env->Object_GetPropertyByName_Ref(callback, "onFileReady", &method);
    ExecAsyncCallBack(env, static_cast<ani_object>(businessError), ani_obj, static_cast<ani_object>(method));
    vm_->DetachCurrentThread();
}

void TaiheGeneralCallbacks::onBundleStarted(ErrCode code, const BundleName bundleName)
{
    if (!vm_) {
        return;
    }

    ani_env* env = nullptr;
    ani_options aniArgs {0, nullptr};
    if (ANI_OK != vm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env)) {
        env = nullptr;
        if (ANI_OK != vm_->GetEnv(ANI_VERSION_1, &env)) {
            return;
            }
    }

    ani_string ani_bundleName = {};
    env->String_NewUTF8(bundleName.c_str(), bundleName.size(), &ani_bundleName);

    ani_ref businessError = CreateBusinessError(env, static_cast<ani_int>(code), std::string("onBundleStarted error"));
    ani_object callback = reinterpret_cast<ani_object>(*getRef());

    ani_ref method = {};
    env->Object_GetPropertyByName_Ref(callback, "onBundleBegin", &method);
    ExecAsyncCallBack(env, static_cast<ani_object>(businessError),
        static_cast<ani_object>(ani_bundleName), static_cast<ani_object>(method));
    vm_->DetachCurrentThread();
}

void TaiheGeneralCallbacks::onBundleFinished(ErrCode code, const BundleName bundleName)
{
    if (!vm_) {
        return;
    }
    ani_env* env = nullptr;
    ani_options aniArgs {0, nullptr};
    if (ANI_OK != vm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env)) {
        env = nullptr;
        if (ANI_OK != vm_->GetEnv(ANI_VERSION_1, &env)) {
            return;
        }
    }
    ani_string ani_bundleName = {};
    env->String_NewUTF8(bundleName.c_str(), bundleName.size(), &ani_bundleName);
    ani_ref businessError = CreateBusinessError(env, static_cast<ani_int>(code),
                                                std::string("onBundleFinished error"));
    ani_object callback = reinterpret_cast<ani_object>(*getRef());
    ani_ref method = {};
    env->Object_GetPropertyByName_Ref(callback, "onBundleEnd", &method);
    ExecAsyncCallBack(env, static_cast<ani_object>(businessError),
        static_cast<ani_object>(ani_bundleName), static_cast<ani_object>(method));
    vm_->DetachCurrentThread();
}

void TaiheGeneralCallbacks::onAllBundlesFinished(ErrCode code)
{
    if (!vm_) {
        return;
    }
    ani_env* env = nullptr;
    ani_options aniArgs {0, nullptr};
    if (ANI_OK != vm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env)) {
        env = nullptr;
        if (ANI_OK != vm_->GetEnv(ANI_VERSION_1, &env)) {
            return;
        }
    }
    ani_object ani_obj = {};
    ani_ref businessError = CreateBusinessError(env, static_cast<ani_int>(code),
                                                std::string("onAllBundlesFinished error"));
    ani_object callback = reinterpret_cast<ani_object>(*getRef());
    ani_ref method = {};
    env->Object_GetPropertyByName_Ref(callback, "onAllBundlesEnd", &method);
    ExecAsyncCallBack(env, static_cast<ani_object>(businessError), ani_obj, static_cast<ani_object>(method));
    vm_->DetachCurrentThread();
}

void TaiheGeneralCallbacks::onResultReport(const std::string bundleName, const std::string result)
{
    if (!vm_) {
        return;
    }

    ani_env* env = nullptr;
    ani_options aniArgs {0, nullptr};
    if (ANI_OK != vm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env)) {
        env = nullptr;
        if (ANI_OK != vm_->GetEnv(ANI_VERSION_1, &env)) {
            return;
        }
    }

    ani_string ani_bundleName = {};
    env->String_NewUTF8(bundleName.c_str(), bundleName.size(), &ani_bundleName);

    ani_string ani_result = {};
    env->String_NewUTF8(result.c_str(), result.size(), &ani_result);
    ani_ref ani_argv[] = {ani_bundleName, ani_result};
    
    ani_object callback = reinterpret_cast<ani_object>(*getRef());

    ani_ref method = {};
    ani_ref aniResult = {};
    env->Object_GetPropertyByName_Ref(callback, "onResultReport", &method);
    env->FunctionalObject_Call(static_cast<ani_fn_object>(method), TWO_PARAMS, ani_argv, &aniResult);
    vm_->DetachCurrentThread();
}

void TaiheGeneralCallbacks::onBackupServiceDied()
{
    if (!vm_) {
        return;
    }

    ani_env* env = nullptr;
    ani_options aniArgs {0, nullptr};
    if (ANI_OK != vm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env)) {
        env = nullptr;
        if (ANI_OK != vm_->GetEnv(ANI_VERSION_1, &env)) {
            return;
        }
    }

    ani_ref aniUndefined;
    env->GetUndefined(&aniUndefined);
    ani_ref ani_argv[] = {aniUndefined};
    ani_object callback = reinterpret_cast<ani_object>(*getRef());

    ani_ref method = {};
    ani_ref result = {};
    env->Object_GetPropertyByName_Ref(callback, "onBackupServiceDied", &method);
    env->FunctionalObject_Call(static_cast<ani_fn_object>(method), ONE_PARAMS, ani_argv, &result);
    vm_->DetachCurrentThread();
}

void TaiheGeneralCallbacks::onProcess(const std::string bundleName, const std::string process)
{
    if (!vm_) {
        return;
    }

    ani_env* env = nullptr;
    ani_options aniArgs {0, nullptr};
    if (ANI_OK != vm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env)) {
        env = nullptr;
        if (ANI_OK != vm_->GetEnv(ANI_VERSION_1, &env)) {
            return;
        }
    }

    ani_string ani_bundleName = {};
    env->String_NewUTF8(bundleName.c_str(), bundleName.size(), &ani_bundleName);

    ani_string ani_process = {};
    env->String_NewUTF8(process.c_str(), process.size(), &ani_process);
    ani_ref ani_argv[] = {ani_bundleName, ani_process};
    
    ani_object callback = reinterpret_cast<ani_object>(*getRef());

    ani_ref method = {};
    ani_ref result = {};
    env->Object_GetPropertyByName_Ref(callback, "onProcess", &method);
    env->FunctionalObject_Call(static_cast<ani_fn_object>(method), TWO_PARAMS, ani_argv, &result);
    vm_->DetachCurrentThread();
}

void TaiheGeneralCallbacks::onBackupSizeReport(const std::string info)
{
    if (!vm_) {
        return;
    }
    ani_env* env = nullptr;
    ani_options aniArgs {0, nullptr};
    if (ANI_OK != vm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env)) {
        env = nullptr;
        if (ANI_OK != vm_->GetEnv(ANI_VERSION_1, &env)) {
            return;
        }
    }
    ani_string ani_param = {};
    env->String_NewUTF8(info.c_str(), info.size(), &ani_param);
    ani_ref ani_argv[] = {ani_param};
    ani_object callback = reinterpret_cast<ani_object>(*getRef());
    ani_ref method = {};
    ani_ref result = {};
    env->Object_GetPropertyByName_Ref(callback, "onBackupSizeReport", &method);
    env->FunctionalObject_Call(static_cast<ani_fn_object>(method), ONE_PARAMS, ani_argv, &result);
    vm_->DetachCurrentThread();
}
} // namespace OHOS::FileManagement::Backup