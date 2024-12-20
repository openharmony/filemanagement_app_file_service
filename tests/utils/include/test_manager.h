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

#ifndef OHOS_FILEMGMT_BACKUP_TEST_MANAGER_H
#define OHOS_FILEMGMT_BACKUP_TEST_MANAGER_H

#include <string>

namespace OHOS::FileManagement::Backup {
enum class MakeDirType {
    DEFAULT,
    CURRENTUSER
};
const std::string PATH_CURRENT_USER = "/storage/Users/currentUser/";
const char FILE_SEPARATOR_CHAR = '/';
class TestManager {
public:
    explicit TestManager(std::string functionName,  MakeDirType dirType = MakeDirType::DEFAULT);
    ~TestManager();

    /**
     * @brief 获取当前测试用例指定根目录
     *
     * @return std::string 根目录
     */
    std::string GetRootDirCurTest() const;

private:
    std::string rootDirCurTest_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_TEST_MANAGER_H
