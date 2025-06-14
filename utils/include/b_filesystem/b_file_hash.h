/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_B_FILE_HASH_H
#define OHOS_FILEMGMT_BACKUP_B_FILE_HASH_H

#include <string>
#include <tuple>

namespace OHOS::FileManagement::Backup {
class BackupFileHash {
public:
    static std::tuple<int, std::string> HashWithSHA256(const std::string &fpath);
    static std::string HashFilePath(const std::string &fileName);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_FILE_HASH_H