/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_CACHED_ENTITY_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_CACHED_ENTITY_H

#include <any>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <system_error>
#include <type_traits>

#include "b_error/b_error.h"
#include "b_filesystem/b_file.h"
#include "b_json/b_json_entity.h"
#include "filemgmt_libhilog.h"
#include "unique_fd.h"
#include "json/json.h"

namespace OHOS::FileManagement::Backup {
template <typename T>
class BJsonCachedEntity {
public:
    /**
     * @brief 获取结构化对象
     *
     * @return T 结构化对象（即实体）
     */
    T Structuralize()
    {
        static_assert(!std::is_default_constructible_v<T>);
        static_assert(!std::is_base_of_v<T, BJsonEntity>);
        return T(obj_);
    }

    /**
     * @brief 持久化JSon对象并完成
     *
     * @throw std::system_error IO异常
     */
    void Persist()
    {
        Json::StreamWriterBuilder builder;
        const std::string jsonFileContent = Json::writeString(builder, obj_);
        HILOGI("Try to persist a Json object, whose content reads: %{public}s", jsonFileContent.c_str());

        BFile::Write(srcFile_, jsonFileContent);
    }

    /**
     * @brief 从文件中重新加载JSon对象
     *
     * @throw std::system_error IO异常或解析异常
     */
    int ReloadFromFile()
    {
        Json::CharReaderBuilder builder;
        std::unique_ptr<Json::CharReader> const jsonReader(builder.newCharReader());
        Json::Value jValue;
        std::string errs;
        std::unique_ptr<char[]> rawBuf = BFile::ReadFile(srcFile_);
        std::string_view sv(rawBuf.get());

        if (sv.empty()) {
            HILOGI("This Json file is empty");
            return 0;
        }

        bool res = jsonReader->parse(sv.data(), sv.data() + sv.length(), &jValue, &errs);
        if (!res || !errs.empty()) {
            return BError(BError::Codes::UTILS_INVAL_JSON_ENTITY, errs).GetCode();
        }

        obj_ = std::move(jValue);
        return 0;
    }

    /**
     * @brief 根据字符串重新加载JSon对象
     *
     * @throw std::system_error IO异常或解析异常
     */
    void ReloadFromString(std::string_view sv)
    {
        Json::CharReaderBuilder builder;
        std::unique_ptr<Json::CharReader> const jsonReader(builder.newCharReader());
        Json::Value jValue;
        std::string errs;

        bool res = jsonReader->parse(sv.data(), sv.data() + sv.length(), &jValue, &errs);
        if (!res || !errs.empty()) {
            HILOGE("Json utils operated on an invalid file");
            return;
        }

        obj_ = std::move(jValue);
    }

    /**
     * @brief 获取JSon文件的文件描述符
     *
     * @return UniqueFd&
     */
    UniqueFd &GetFd()
    {
        return srcFile_;
    }

public:
    /**
     * @brief 构造方法，要求T必须具备T(Json::Value&)构造函数
     *
     * @param fd 用于加载/持久化JSon对象的文件
     */
    explicit BJsonCachedEntity(UniqueFd fd) : srcFile_(std::move(fd)), entity_(std::ref(obj_))
    {
        struct stat stat = {};
        if (fstat(srcFile_, &stat) == -1) {
            std::stringstream ss;
            ss << std::generic_category().message(errno) << " with fd eq" << srcFile_.Get();
            BError(BError::Codes::UTILS_INVAL_JSON_ENTITY, ss.str());
            return;
        }

        (void)ReloadFromFile();
    }

    /**
     * @brief 构造方法，要求T必须具备T(Json::Value&, std::any)构造函数
     *
     * @param sv 用于加载/持久化JSon对象的字符串
     * @param option 任意类型对象
     */
    explicit BJsonCachedEntity(std::string_view sv, std::any option = std::any()) : entity_(std::ref(obj_))
    {
        ReloadFromString(entity_.GetJSonSource(sv, option));
    }

private:
    UniqueFd srcFile_;
    Json::Value obj_;
    T entity_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_CACHED_ENTITY_H