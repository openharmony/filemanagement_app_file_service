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

#include "b_tarball/b_tarball_posix/b_tarball_posix_extended_header.h"

#include <cstdio>

#include "b_encryption/b_encryption.h"
#include "b_resources/b_constants.h"
#include "securec.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BTarballPosixExtendedHeader::BTarballPosixExtendedHeader(BTarballPosixExtendedData &extData)
{
    (void)snprintf_s(header_.fileSize, BConstants::FILESIZE_MAX_SIZE, BConstants::FILESIZE_MAX_SIZE - 1, "%lo",
                     extData.GetEntriesSize());
    header_.typeFlag = BConstants::TYPEFLAG_EXTENDED; // typeFlag = 'x'
    unsigned int chksum = BEncryption::CalculateChksum((char *)&header_, BConstants::HEADER_SIZE);
    (void)snprintf_s(header_.chksum, BConstants::CHKSUM_MAX_SIZE - 1, BConstants::CHKSUM_MAX_SIZE - 2, "%6o",
                     chksum); // chksum字段的字符串休止符在字段数组的倒数第2个位置
}

void BTarballPosixExtendedHeader::Publish(const UniqueFd &outFile)
{
    if (strcmp(header_.fileSize, "0") != 0) {
        if (write(outFile, &header_, BConstants::HEADER_SIZE) == -1) {
            printf("ExtendedHeader::Publish: write\n");
            throw exception();
        }
    }
}
} // namespace OHOS::FileManagement::Backup