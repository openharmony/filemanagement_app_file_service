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

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_EXTENDED_ENTRY_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_EXTENDED_ENTRY_H

#include <optional>
#include <sys/stat.h>

#include "b_resources/b_constants.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BTarballPosixExtendedEntry {
public:
    static std::optional<BTarballPosixExtendedEntry> TryToGetEntry(BConstants::EntryKey entryKey,
                                                                   const std::string &pathName,
                                                                   const struct stat &statInfo);

public:
    size_t GetEntrySize();
    void Publish(const UniqueFd &outFile);

public:
    ~BTarballPosixExtendedEntry() = default;

private:
    size_t entrySize_ {0};
    std::string entryName_;
    std::string entryValue_;
    std::string entry_;

private:
    BTarballPosixExtendedEntry(const std::string &entryName, const std::string &entryValue);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_EXTENDED_ENTRY_H