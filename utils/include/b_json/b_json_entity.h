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

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_H

#include <any>
#include <string>
#include <string_view>

#include "filemgmt_libhilog.h"
#include "json/json.h"

namespace OHOS::FileManagement::Backup {
class BJsonEntity {
public:
    /**
     * @brief 获取JSon数据源
     *
     * @param jsonFromRealWorld 直接来自真实用户场景的配置文件（即，非override配置）
     * @param option 任意类型对象
     * @return std::string 取决于具体情况的配置文件
     */
    std::string GetJSonSource(std::string_view jsonFromRealWorld, std::any option = std::any())
    {
        return std::string(jsonFromRealWorld);
    }

    /**
     * @brief Judge the key exist and isArray
     *
     * @return bool
     */
    bool HasArray(const std::string& key) const
    {
        if (obj_.isNull()) {
            HILOGE("Uninitialized JSon Object reference");
            return false;
        }
        if (!obj_.isMember(key)) {
            HILOGE("'%{public}s' field not found", key.c_str());
            return false;
        }
        if (!obj_[key].isArray()) {
            HILOGE("'%{public}s' field must be an array", key.c_str());
            return false;
        }
        return true;
    }

    /**
     * @brief 构造方法，具备T(Json::Value&, std::any)能力的构造函数
     *
     * @param obj Json对象引用
     * @param option 任意类型对象
     */
    explicit BJsonEntity(Json::Value &obj, std::any option = std::any()) : obj_(obj), option_(option) {}
    BJsonEntity() = delete;
    virtual ~BJsonEntity() = default;

protected:
    Json::Value &obj_;
    std::any option_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_H