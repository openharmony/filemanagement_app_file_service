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

#include "ext_backup_ani_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS {

void AniError::ThrowBusinessError(ani_env *env, errorCode errCode, const std::string &errMsg)
{
    static const char *errorClsName = "L@ohos/base/BusinessError;";
    HILOGD("Begin ThrowBusinessError.");

    ani_class cls {};
    if (ANI_OK != env->FindClass(errorClsName, &cls)) {
        HILOGE("find class BusinessError %{public}s failed", errorClsName);
        return;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ":V", &ctor)) {
        HILOGE("find method BusinessError.constructor failed");
        return;
    }

    ani_object errorObject;
    if (ANI_OK != env->Object_New(cls, ctor, &errorObject)) {
        HILOGE("create BusinessError object failed");
        return;
    }

    ani_double aniErrCode = static_cast<ani_double>(errCode);
    ani_string errMsgStr;
    if (ANI_OK != env->String_NewUTF8(errMsg.c_str(), errMsg.size(), &errMsgStr)) {
        HILOGE("convert errMsg to ani_string failed");
    }

    HILOGD("ThrowBusinessError: errMsg: %{public}s.", errMsg.c_str());
    if (ANI_OK != env->Object_SetFieldByName_Double(errorObject, "code", aniErrCode)) {
        HILOGE("set error code failed");
    }

    if (ANI_OK != env->Object_SetPropertyByName_Ref(errorObject, "message", errMsgStr)) {
        HILOGE("set error message failed");
    }
    env->ThrowError(static_cast<ani_error>(errorObject));
    return;
}

} // namespace OHOS