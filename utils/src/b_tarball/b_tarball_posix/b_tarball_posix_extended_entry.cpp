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

#include "b_tarball/b_tarball_posix/b_tarball_posix_extended_entry.h"

#include <cstdio>
#include <linux/limits.h>
#include <sys/stat.h>

#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

optional<BTarballPosixExtendedEntry> BTarballPosixExtendedEntry::TryToGetEntry(BConstants::EntryKey entryKey,
                                                                               const string &pathName,
                                                                               const struct stat &statInfo)
{
    switch (entryKey) {
        case BConstants::SUPER_LONG_PATH: {
            string::size_type firstNotSlashIndex = pathName.find_first_not_of('/');
            string tmpPathName = pathName.substr(firstNotSlashIndex);
            if (tmpPathName.size() > BConstants::PATHNAME_MAX_SIZE) {
                return BTarballPosixExtendedEntry(BConstants::ENTRY_NAME_PATH, tmpPathName);
            }
            break;
        }
        case BConstants::SUPER_LONG_LINK_PATH: {
            if ((statInfo.st_mode & S_IFMT) == S_IFLNK) {
                char linkName[PATH_MAX] {};
                ssize_t linkSize = readlink(pathName.c_str(), linkName, PATH_MAX);
                if (linkSize > BConstants::LINKNAME_MAX_SIZE) {
                    return BTarballPosixExtendedEntry(BConstants::ENTRY_NAME_LINKPATH, linkName);
                }
            }
            break;
        }
        case BConstants::SUPER_LONG_SIZE: {
            if (statInfo.st_size > BConstants::FILESIZE_MAX) {
                return BTarballPosixExtendedEntry(BConstants::ENTRY_NAME_SIZE, to_string(statInfo.st_size));
            }
            break;
        }
    }
    return {};
}

size_t BTarballPosixExtendedEntry::GetEntrySize()
{
    return entrySize_;
}

void BTarballPosixExtendedEntry::Publish(const UniqueFd &outFile)
{
    if (write(outFile, entry_.c_str(), entry_.size()) == -1) {
        printf("BTarballPosixExtendedEntry::Publish: write\n");
        throw exception();
    }
}

BTarballPosixExtendedEntry::BTarballPosixExtendedEntry(const string &entryName, const string &entryValue)
    : entryName_(entryName), entryValue_(entryValue)
{
    size_t fieldSize = entryName_.size() + entryValue_.size() + string_view(" =\n").size();
    size_t digitsNum = 0;
    size_t tmp = fieldSize;
    size_t nextHigherPowerOfTen = 1;
    while (tmp > 0) {
        tmp /= BConstants::DECIMAL_BASE;
        ++digitsNum;
        nextHigherPowerOfTen *= BConstants::DECIMAL_BASE;
    }
    if (digitsNum + fieldSize >= nextHigherPowerOfTen) {
        ++digitsNum;
    }
    entrySize_ = digitsNum + fieldSize;
    entry_ = to_string(entrySize_) + " " + entryName_ + "=" + entryValue_ + "\n";
}
} // namespace OHOS::FileManagement::Backup