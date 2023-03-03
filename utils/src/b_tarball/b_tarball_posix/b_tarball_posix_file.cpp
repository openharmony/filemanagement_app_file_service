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

#include "b_tarball/b_tarball_posix/b_tarball_posix_file.h"

#include <cerrno>
#include <cstdio>
#include <fcntl.h>

#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BTarballPosixFile::BTarballPosixFile(const string &pathName) : pathName_(pathName)
{
    struct stat inStat = {};
    file_ = UniqueFd(open(pathName.c_str(), O_RDONLY | O_NOFOLLOW));
    if (file_ == -1) {
        if (errno == ELOOP) { // if this file is a symbolic link
            if (lstat(pathName.c_str(), &inStat) == -1) {
                printf("BTarballPosixFile::BTarballPosixFile: lstat\n");
                throw exception();
            }
        } else {
            printf("BTarballPosixFile::BTarballPosixFile: open\n");
            throw exception();
        }
    } else {
        if (fstat(file_, &inStat) == -1) {
            printf("BTarballPosixFile::BTarballPosixFile: fstat\n");
            throw exception();
        }
    }

    extData_ = BTarballPosixExtendedData(pathName, inStat);
    extHdr_ = BTarballPosixExtendedHeader(extData_);
    paxHdr_ = BTarballPosixPaxHeader(pathName, inStat);
    fileData_ = BTarballPosixFileData(inStat);
}

void BTarballPosixFile::Publish(const UniqueFd &outFile)
{
    extHdr_.Publish(outFile);
    extData_.Publish(outFile);
    paxHdr_.Publish(outFile);
    fileData_.Publish(file_, outFile);
}
} // namespace OHOS::FileManagement::Backup