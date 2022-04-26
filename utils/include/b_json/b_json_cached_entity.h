/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_CACHED_ENTITY_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_CACHED_ENTITY_H

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>

#include "b_filesystem/b_file.h"
#include "errors.h"
#include "filemgmt_libhilog.h"
#include "unique_fd.h"
#include "json/json.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
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
        return {obj_};
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

        int ret = write(srcFile_, jsonFileContent.c_str(), jsonFileContent.length());
        if (ret == -1) {
            HILOGE("Failed to persist because of %{public}s", strerror(errno));
            throw std::system_error(errno, std::generic_category());
        }
    }

    /**
     * @brief 从文件中重新加载JSon对象
     *
     * @throw std::system_error IO异常或解析异常
     */
    void ReloadFromFile()
    {
        Json::CharReaderBuilder builder;
        std::unique_ptr<Json::CharReader> const jsonReader(builder.newCharReader());
        Json::Value jValue;
        std::string errs;
        std::unique_ptr<char[]> rawBuf = BFile::ReadFile(srcFile_);
        std::string_view sv(rawBuf.get());

        bool res = jsonReader->parse(sv.data(), sv.data() + sv.length(), &jValue, &errs);
        if (!res || !errs.empty()) {
            HILOGE("Failed to parse the JsonFile at %{public}s", errs.c_str());
            throw std::system_error(EINVAL, std::generic_category());
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
    BJsonCachedEntity(UniqueFd fd) : srcFile_(std::move(fd)), entity_(std::ref(obj_))
    {
        try {
            ReloadFromFile();
        } catch (...) {
            // 认为对应 JSon 文件不存在 / 无法解析是一种正常情况，此时采取默认配置即可
        }
    }

private:
    UniqueFd srcFile_;
    Json::Value obj_;
    T entity_;
};
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_CACHED_ENTITY_H