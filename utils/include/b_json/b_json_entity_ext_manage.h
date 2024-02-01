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

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_EXT_MANAGE_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_EXT_MANAGE_H

#include <map>
#include <set>
#include <string>
#include <sys/stat.h>

#include "b_json/b_json_cached_entity.h"
#include "json/json.h"

namespace OHOS::FileManagement::Backup {
struct ExtManageInfo {
    string hashName {""};
    string fileName {""};
    struct stat sta {};
    bool isUserTar {false};
    bool isBigFile {false};
};
class BJsonEntityExtManage : public BJsonEntity {
public:
    /**
     * @brief 设置索引文件
     *
     * @param info std::map<string, pair<string, struct stat>>
     */
    void SetExtManage(const map<string, tuple<string, struct stat, bool>> &info) const;

    /**
     * @brief 设置索引文件(克隆)
     *
     * @param info map<string, tuple<string, struct stat, bool, bool>>
     */
    void SetExtManageForClone(const map<string, tuple<string, struct stat, bool, bool>> &info) const;

    /**
     * @brief 获取索引文件
     *
     * @return std::set<std::string>
     */
    std::set<std::string> GetExtManage() const;

    /**
     * @brief 获取索引文件及详细信息
     *
     * @return map<string, pair<string, struct stat>>
     */
    std::vector<ExtManageInfo> GetExtManageInfo() const;

    /**
     * @brief Set the hard link Information
     *
     * @param origin 原始文件名
     * @param hardLinks 硬链接文件名
     * @return true 设置成功
     * @return false 设置失败
     */
    bool SetHardLinkInfo(const std::string origin, const std::set<std::string> hardLinks);

    /**
     * @brief Get the hard link Information
     *
     * @param origin 原始文件名
     * @return const 硬链接集合
     */
    const std::set<std::string> GetHardLinkInfo(const string origin);

public:
    /**
     * @brief 构造方法，具备T(Json::Value&, std::any)能力的构造函数
     *
     * @param obj Json对象引用
     * @param option 任意类型对象
     */
    explicit BJsonEntityExtManage(Json::Value &obj, std::any option = std::any()) : BJsonEntity(obj, option) {}
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_EXT_MANAGE_H