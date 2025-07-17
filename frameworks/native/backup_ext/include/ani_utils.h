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

#ifndef ANI_UTILS_H
#define ANI_UTILS_H

#include <ani.h>

#include <cstdarg>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class AniObjectUtils {
public:
    static ani_object Create(ani_env *env, const char *nsName, const char *clsName, ...)
    {
        ani_object nullobj {};

        ani_namespace ns;
        if (ANI_OK != env->FindNamespace(nsName, &ns)) {
            std::cerr << "[ANI] Not found namespace " << nsName << std::endl;
            return nullobj;
        }

        ani_class cls;
        const std::string fullClsName = std::string(nsName).append(".").append(clsName);
        if (ANI_OK != env->FindClass(fullClsName.c_str(), &cls)) {
            std::cerr << "[ANI] Not found class " << clsName << std::endl;
            return nullobj;
        }

        ani_method ctor;
        if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
            std::cerr << "[ANI] Not found <ctor> for class " << clsName << std::endl;
            return nullobj;
        }

        std::cerr << "[ANI] AniObjectUtils Object_New_V " << clsName << std::endl;
        ani_object obj;
        va_list args;
        va_start(args, clsName);
        ani_status status = env->Object_New_V(cls, ctor, &obj, args);
        va_end(args);
        if (ANI_OK != status) {
            std::cerr << "[ANI] Failed to Object_New for class " << cls << std::endl;
            return nullobj;
        }
        return obj;
    }

    static ani_object Create(ani_env *env, const char *clsName, ...)
    {
        ani_object nullobj {};

        ani_class cls;
        if (ANI_OK != env->FindClass(clsName, &cls)) {
            std::cerr << "[ANI] Not found class " << clsName << std::endl;
            return nullobj;
        }

        ani_method ctor;
        if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
            std::cerr << "[ANI] Not found <ctor> for class " << clsName << std::endl;
            return nullobj;
        }

        ani_object obj;
        va_list args;
        va_start(args, clsName);
        ani_status status = env->Object_New_V(cls, ctor, &obj, args);
        va_end(args);
        if (ANI_OK != status) {
            std::cerr << "[ANI] Failed to Object_New for class " << cls << std::endl;
            return nullobj;
        }
        return obj;
    }

    static ani_object Create(ani_env *env, ani_class cls, ...)
    {
        ani_object nullobj {};

        ani_method ctor;
        if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
            std::cerr << "[ANI] Not found <ctor> for class" << std::endl;
            return nullobj;
        }

        ani_object obj;
        va_list args;
        va_start(args, cls);
        ani_status status = env->Object_New_V(cls, ctor, &obj, args);
        va_end(args);
        if (ANI_OK != status) {
            std::cerr << "[ANI] Failed to Object_New for class " << cls << std::endl;
            return nullobj;
        }
        return obj;
    }

    template <typename T>
    static ani_status Wrap(ani_env *env, ani_object object, T *nativePtr, const char *propName = "nativePtr")
    {
        return env->Object_SetFieldByName_Long(object, propName, reinterpret_cast<ani_long>(nativePtr));
    }

    template <typename T>
    static T *Unwrap(ani_env *env, ani_object object, const char *propName = "nativePtr")
    {
        ani_long nativePtr;
        if (ANI_OK != env->Object_GetFieldByName_Long(object, propName, &nativePtr)) {
            return nullptr;
        }
        return reinterpret_cast<T *>(nativePtr);
    }
};

class AniStringUtils {
public:
    static std::string ToStd(ani_env *env, ani_string ani_str)
    {
        ani_size strSize;
        env->String_GetUTF8Size(ani_str, &strSize);

        std::vector<char> buffer(strSize + 1); // +1 for null terminator
        char *utf8_buffer = buffer.data();

        // String_GetUTF8 Supportted by https://gitee.com/openharmony/arkcompiler_runtime_core/pulls/3416
        ani_size bytes_written = 0;
        env->String_GetUTF8(ani_str, utf8_buffer, strSize + 1, &bytes_written);

        if (bytes_written <= strSize) {
            utf8_buffer[bytes_written] = '\0';
        } else {
            utf8_buffer[strSize] = '\0';
        }
        std::string content = std::string(utf8_buffer);
        return content;
    }

    static ani_string ToAni(ani_env *env, const std::string &str)
    {
        ani_string aniStr = nullptr;
        if (ANI_OK != env->String_NewUTF8(str.data(), str.size(), &aniStr)) {
            std::cerr << "[ANI] Unsupported ANI_VERSION_1" << std::endl;
            return nullptr;
        }
        return aniStr;
    }
};

class UnionAccessor {
public:
    UnionAccessor(ani_env *env, ani_object &obj) : env_(env), obj_(obj) {}

    bool IsInstanceOf(const std::string &cls_name)
    {
        ani_class cls;
        ani_status status = env_->FindClass(cls_name.c_str(), &cls);
        if (status != ANI_OK) {
            return false;
        }

        ani_boolean ret;
        env_->Object_InstanceOf(obj_, cls, &ret);
        return ret;
    }

    template <typename T>
    bool IsInstanceOfType();

    template <typename T>
    bool TryConvert(T &value);

    template <typename T>
    bool TryConvertArray(std::vector<T> &value);

private:
    ani_env *env_;
    ani_object obj_;
};

template <>
inline bool UnionAccessor::IsInstanceOfType<bool>()
{
    return IsInstanceOf("Lstd/core/Boolean;");
}

template <>
inline bool UnionAccessor::IsInstanceOfType<int>()
{
    return IsInstanceOf("Lstd/core/Int;");
}

template <>
inline bool UnionAccessor::IsInstanceOfType<double>()
{
    return IsInstanceOf("Lstd/core/Double;");
}

template <>
inline bool UnionAccessor::IsInstanceOfType<std::string>()
{
    return IsInstanceOf("Lstd/core/String;");
}

template <>
inline bool UnionAccessor::TryConvert<bool>(bool &value)
{
    if (!IsInstanceOfType<bool>()) {
        return false;
    }

    ani_boolean aniValue;
    auto ret = env_->Object_CallMethodByName_Boolean(obj_, "unboxed", nullptr, &aniValue);
    if (ret != ANI_OK) {
        return false;
    }
    value = static_cast<bool>(aniValue);
    return true;
}

template <>
inline bool UnionAccessor::TryConvert<int>(int &value)
{
    if (!IsInstanceOfType<int>()) {
        return false;
    }

    ani_int aniValue;
    auto ret = env_->Object_CallMethodByName_Int(obj_, "unboxed", nullptr, &aniValue);
    if (ret != ANI_OK) {
        return false;
    }
    value = static_cast<int>(aniValue);
    return true;
}

template <>
inline bool UnionAccessor::TryConvert<double>(double &value)
{
    if (!IsInstanceOfType<double>()) {
        return false;
    }

    ani_double aniValue;
    auto ret = env_->Object_CallMethodByName_Double(obj_, "unboxed", nullptr, &aniValue);
    if (ret != ANI_OK) {
        return false;
    }
    value = static_cast<double>(aniValue);
    return true;
}

template <>
inline bool UnionAccessor::TryConvert<std::string>(std::string &value)
{
    if (!IsInstanceOfType<std::string>()) {
        return false;
    }

    value = AniStringUtils::ToStd(env_, static_cast<ani_string>(obj_));
    return true;
}

class OptionalAccessor {
public:
    OptionalAccessor(ani_env *env, ani_object &obj) : env_(env), obj_(obj) {}

    bool IsUndefined()
    {
        ani_boolean isUndefined;
        ani_status status = env_->Reference_IsUndefined(obj_, &isUndefined);
        if (status != ANI_OK) {
            return false;
        }
        return isUndefined;
    }

    template <typename T>
    std::optional<T> Convert();

private:
    ani_env *env_;
    ani_object obj_;
};

template <>
inline std::optional<double> OptionalAccessor::Convert<double>()
{
    if (IsUndefined()) {
        return std::nullopt;
    }

    ani_double aniValue;
    auto ret = env_->Object_CallMethodByName_Double(obj_, "doubleValue", nullptr, &aniValue);
    if (ret != ANI_OK) {
        return std::nullopt;
    }
    auto value = static_cast<double>(aniValue);
    return value;
}

#endif
