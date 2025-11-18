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

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_ONBACKUPEX_RET_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_ONBACKUPEX_RET_H

#include <string>
#include <unordered_set>

#include "b_json/b_json_cached_entity.h"
#include "json/json.h"

namespace OHOS::FileManagement::Backup {

class BJsonEntityOnBackupExRet : public BJsonEntity {
public:
    /**
     * @brief 获取开启的兼容性路径列表(待恢复路径)
     *
     * @return unordered_set<std::string>
     */
    std::unordered_set<std::string> GetCompatibleDirs() const;

    /**
     * @brief 构造方法，具备T(Json::Value&, std::any)能力的构造函数
     *
     * @param obj Json对象引用
     * @param option 任意类型对象
     */
    explicit BJsonEntityOnBackupExRet(Json::Value &obj, std::any option = std::any()) : BJsonEntity(obj, option) {}
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_ONBACKUPEX_RET_H
