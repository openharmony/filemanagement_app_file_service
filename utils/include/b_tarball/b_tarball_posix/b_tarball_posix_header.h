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

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_HEADER_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_HEADER_H

#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
struct BTarballPosixHeader {
    char pathName[BConstants::PATHNAME_MAX_SIZE] {};
    char mode[BConstants::MODE_MAX_SIZE] {};
    char uid[BConstants::UGID_MAX_SIZE] {};
    char gid[BConstants::UGID_MAX_SIZE] {};
    char fileSize[BConstants::FILESIZE_MAX_SIZE] {};
    char mtime[BConstants::TIME_MAX_SIZE] {};
    char chksum[BConstants::CHKSUM_MAX_SIZE] {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    char typeFlag {};
    char linkName[BConstants::LINKNAME_MAX_SIZE] {};
    char magic[BConstants::MAGIC_SIZE] {"ustar"};
    char version[BConstants::VERSION_SIZE] {'0', '0'};
    char uname[BConstants::UGNAME_MAX_SIZE] {};
    char gname[BConstants::UGNAME_MAX_SIZE] {};
    char devMajor[BConstants::DEV_MAX_SIZE] {};
    char devMinor[BConstants::DEV_MAX_SIZE] {};
    char prefix[BConstants::PREFIX_SIZE] {};
    char padding[BConstants::PADDING_SIZE] {};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_HEADER_H