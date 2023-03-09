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

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_EXTENSION_CONFIG_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_EXTENSION_CONFIG_H

#include <string>
#include <vector>

#include "b_json/b_json_cached_entity.h"
#include "json/json.h"

namespace OHOS::FileManagement::Backup {
class BJsonEntityExtensionConfig : public BJsonEntity {
public:
    /**
     * @brief 从JSon对象中通过includes字段获取待备份目录模式串清单
     *
     * @return 待备份目录模式串清单
     * @note 如果用户没有配置该字段，则返回默认打包目录{@link BConstants::PATHES_TO_BACKUP}
     * @note 如果用户配置了空数组，则返回""表示生成空打包文件
     */
    std::vector<std::string> GetIncludes() const;

    /**
     * @brief 从JSon对象中获取排除目录列表
     *
     * @return 排除目录
     */
    std::vector<std::string> GetExcludes() const;

    /**
     * @brief 从JSon对象中获取备份恢复权限
     *
     * @return 备份权限
     */
    bool GetAllowToBackupRestore() const;

public:
    std::string GetJSonSource(std::string_view jsonFromRealWorld, std::any option);

public:
    /**
     * @brief 构造方法，具备T(Json::Value&, std::any)能力的构造函数
     *
     * @param obj Json对象引用
     * @param option 任意类型对象
     */
    explicit BJsonEntityExtensionConfig(Json::Value &obj, std::any option = std::any()) : BJsonEntity(obj, option) {}
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_EXTENSION_CONFIG_H