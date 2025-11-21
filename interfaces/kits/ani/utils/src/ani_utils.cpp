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

#include "ani_utils.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
std::unordered_map<std::string, ani_class> AniUtils::clsMap_;
std::mutex AniUtils::mapMutex_;

std::string AniUtils::AniStringToStdString(ani_env *env, ani_string aniString)
{
    ani_size sz {};
    env->String_GetUTF8Size(aniString, &sz);

    std::string result(sz + 1, 0);
    env->String_GetUTF8(aniString, result.data(), result.size(), &sz);
    result.resize(sz);
    return result;
}

bool AniUtils::AniObjectToStdString(ani_env* env, ani_object obj, std::string& str)
{
    ani_class stringClass = GetAniClsByName(env, "std.core.String");
    ani_boolean isString = ANI_FALSE;
    env->Object_InstanceOf(obj, stringClass, &isString);
    if (!isString) {
        HILOGE("obj is not ani_string");
        return false;
    }
    str = AniStringToStdString(env, static_cast<ani_string>(obj));
    return true;
}

std::optional<ani_string> AniUtils::StdStringToAniString(ani_env *env, const std::string& str)
{
    ani_string result_string{};
    if (env->String_NewUTF8(str.c_str(), str.size(), &result_string) != ANI_OK) {
        return {};
    }
    return result_string;
}

bool AniUtils::AniArrayToStrVector(ani_env* env, ani_array arr, std::vector<std::string>& result)
{
    ani_size arrSize = 0;
    if (ANI_OK != env->Array_GetLength(arr, &arrSize)) {
        HILOGE("Array_GetLength fail");
        return false;
    }
    if (arrSize == 0) {
        return true;
    }
    for (ani_size idx = 0; idx < arrSize; idx++) {
        ani_ref item;
        if (ANI_OK != env->Array_Get(arr, idx, &item)) {
            HILOGE("Array_Get fail, idx=%{public}zu", idx);
            return false;
        }
        std::string str;
        if (!AniObjectToStdString(env, static_cast<ani_object>(item), str)) {
            HILOGE("ani_ref cast to string fail, idx=%{public}zu", idx);
            return false;
        }
        result.push_back(str);
    }
    return true;
}

ani_class AniUtils::GetAniClsByName(ani_env* env, std::string className)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    if (clsMap_.count(className) > 0) {
        return clsMap_[className];
    }
    ani_class cls {};
    if (ANI_OK != env->FindClass(className.c_str(), &cls)) {
        HILOGE("find class fail, class=%{public}s", className.c_str());
        return nullptr;
    }
    clsMap_[className] = cls;
    return cls;
}
}