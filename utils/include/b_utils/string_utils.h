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

#ifndef OHOS_FILEMGMT_BACKUP_STRING_UTILS_H
#define OHOS_FILEMGMT_BACKUP_STRING_UTILS_H
#include <string>
#include <utility>
#include <vector>

namespace OHOS::FileManagement::Backup {

class StringUtils {
public:
    static bool EndsWith(const std::string& str, const std::string& suffix);
    static std::vector<std::string> Split(const std::string& str, const std::string& delimiter);
    static std::string Concat(const std::vector<std::string>& strs, const std::string& connector);

    static std::string PathAddDelimiter(const std::string& path);
    static std::string GenMappingDir(const std::string& backupDir, const std::string& restoreDir);
    // 返回值pair first为待备份的路径，pair second为待恢复的路径
    static std::pair<std::string, std::string> ParseMappingDir(const std::string& str);

    static std::string GenHashName(const std::string &str);
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_STRING_UTIL_H