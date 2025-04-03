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

#include <ani.h>
#include <iostream>
#include <vector>
#include "common_func.h"
#include "file_uri_entity.h"
#include "file_utils.h"
#include "log.h"

using namespace OHOS::AppFileService;

static std::string ParseObjToStr(ani_env *env, ani_string stringObj)
{
    ani_size  strSize;
    env->String_GetUTF8Size(stringObj, &strSize);
    std::vector<char> buffer(strSize + 1);
    char* utf8Buffer = buffer.data();

    ani_size byteswritten = 0;
    env->String_GetUTF8(stringObj, utf8Buffer, strSize + 1, &byteswritten);

    utf8Buffer[byteswritten] = '\0';
    std::string path = std::string(utf8Buffer);
    return path;
}

static ModuleFileUri::FileUriEntity *unwrapp(ani_env *env, ani_object object)
{
    ani_long fileuriEntity_;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "fileUriEntity_", &fileuriEntity_)) {
        return nullptr;
    }
    return reinterpret_cast<ModuleFileUri::FileUriEntity *>(fileuriEntity_);
}

static ani_error CreateAniError(ani_env *env, std::string&& errMsg)
{
    static const char *errorClsName = "Lescompat/Error;";
    ani_class cls {};
    if (ANI_OK != env->FindClass(errorClsName, &cls)) {
        LOGE("Not found %{public}s.", errorClsName);
        return nullptr;
    }
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "Lstd/core/String;:V", &ctor)) {
        LOGE("Not found method <ctor> in  %{public}s.", errorClsName);
        return nullptr;
    }
    ani_string error_msg;
    env->String_NewUTF8(errMsg.c_str(), 17U, &error_msg);
    ani_object errorObject;
    env->Object_New(cls, ctor, &errorObject, error_msg);
    return static_cast<ani_error>(errorObject);
}

static ani_string GetUriFromPath(ani_env *env, ani_string stringObj)
{
    LOGD("Enter GetUriFromPath");
    ani_string uriObj = nullptr;
    std::string path = ParseObjToStr(env, stringObj);
    if (path == "") {
        LOGE("GetUriFromPath get path parameter failed!");
        ani_error err = CreateAniError(env, "GetUriFromPath get path parameter failed!");
        env->ThrowError(err);
        return uriObj;
    }

    std::string uri = CommonFunc::GetUriFromPath(path);
    if (uri == "") {
        LOGE("CommonFunc::GetUriFromPath failed!");
        ani_error err = CreateAniError(env, "GetUriFromPath failed!");
        env->ThrowError(err);
        return uriObj;
    }
    LOGD("GetUriFromPath uri: %{public}s", uri.c_str());

    env->String_NewUTF8(uri.c_str(), uri.size(), &uriObj);

    return uriObj;
}

void FileUriConstructor(ani_env *env, ani_object obj, ani_string stringObj)
{
    LOGD("Enter FileUriConstructor");
    std::string path = ParseObjToStr(env, stringObj);
    if (path == "") {
        LOGE("FileUriConstructor get path parameter failed!");
        ani_error err = CreateAniError(env, "GetUriFromPath get path parameter failed!");
        env->ThrowError(err);
        return;
    }
    auto fileuriEntity = OHOS::FileManagement::CreateUniquePtr<ModuleFileUri::FileUriEntity>(path);
    if (fileuriEntity == nullptr) {
        LOGE("Failed to request heap memory.");
        ani_error err = CreateAniError(env, "Failed to request heap memory.");
        env->ThrowError(err);
        return;
    }
    LOGD("FileUriConstructor fileuriEntity:  %{public}p.", fileuriEntity.get());

    ani_namespace ns;
    if (env->FindNamespace("L@ohos/file/fileuri/fileUri;", &ns) != ANI_OK) {
        LOGE("Namespace L@ohos/file/fileuri/fileUri not found.");
        ani_error err = CreateAniError(env, "Namespace L@ohos/file/fileuri/fileUri not found.");
        env->ThrowError(err);
        return;
    };

    ani_class cls;
    static const char *className = "LFileUri;";
    if (env->Namespace_FindClass(ns, className, &cls) != ANI_OK) {
        LOGE("Not found class LFileUri in Namespace L@ohos/file/fileuri/fileUri.");
        ani_error err = CreateAniError(env, "Class LFileUri not found.");
        env->ThrowError(err);
        return;
    }

    ani_method acquireObj;
    if (ANI_OK != env->Class_FindMethod(cls, "acquireFileUriEntity", "J:V", &acquireObj)) {
        LOGE("Not found method acquireFileUriEntity in class LFileUri.");
        ani_error err = CreateAniError(env, "Method acquireFileUriEntity not found.");
        env->ThrowError(err);
        return;
    }

    if (ANI_OK != env->Object_CallMethod_Void(obj, acquireObj, reinterpret_cast<ani_long>(fileuriEntity.get()))) {
        LOGE("Call method acquireFileUriEntity failed.");
        ani_error err = CreateAniError(env, "Call method acquireFileUriEntity failed.");
        env->ThrowError(err);
        return;
    }
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    LOGD("Enter ANI_Constructor.");
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        LOGE("Unsupported ANI_VERSION_1");
        return ANI_ERROR;
    }

    static const char *nsName = "L@ohos/file/fileuri/fileUri;";
    ani_namespace ns;
    if (ANI_OK != env->FindNamespace(nsName, &ns)) {
        LOGE("Not found namespace %{public}s.", nsName);
        return ANI_NOT_FOUND;
    }
    std::array nsMethods = {
        ani_native_function {"getUriFromPath", nullptr, reinterpret_cast<void *>(GetUriFromPath)},
    };
    if (ANI_OK != env->Namespace_BindNativeFunctions(ns, nsMethods.data(), nsMethods.size())) {
        LOGE("Cannot bind native methods to namespace %{public}s.", nsName);
        return ANI_ERROR;
    };

    static const char *className = "LFileUri;";
    ani_class fileUriClass;
    if (ANI_OK != env->Namespace_FindClass(ns, className, &fileUriClass)) {
        LOGE("Not found class %{public}s in %{public}s.", nsName, nsName);
        return ANI_NOT_FOUND;
    }
    std::array classMethods = {
        ani_native_function {"<ctor>", "Lstd/core/String;:V", reinterpret_cast<void *>(FileUriConstructor)},
    };
    if (ANI_OK != env->Class_BindNativeMethods(fileUriClass, classMethods.data(), classMethods.size())) {
        LOGE("Cannot bind native methods to class %{public}s.", className);
        return ANI_ERROR;
    };

    *result = ANI_VERSION_1;
    return ANI_OK;
}
