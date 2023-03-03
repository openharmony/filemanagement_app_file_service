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

#include "b_tarball/b_tarball_posix/b_tarball_posix_tarball.h"

#include <cstdio>
#include <fcntl.h>

#include "b_resources/b_constants.h"
#include "b_tarball/b_tarball_posix/b_tarball_posix_file.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BTarballPosixTarball::BTarballPosixTarball(const string pathTarball)
{
    fileTarball_ = UniqueFd(open(pathTarball.c_str(), O_WRONLY | O_CREAT, 0664));
    if (fileTarball_ == -1) {
        printf("BTarballPosixTarball::BTarballPosixTarball: open\n");
        throw exception();
    }
}

void BTarballPosixTarball::Emplace(const string pathName)
{
    BTarballPosixFile file(pathName);
    if (isPublished_) {
        lseek(fileTarball_, -BConstants::BLOCK_PADDING_SIZE, SEEK_CUR);
        isPublished_ = false;
    }
    file.Publish(fileTarball_);
}

void BTarballPosixTarball::Publish()
{
    if (!isPublished_) {
        char block[BConstants::BLOCK_PADDING_SIZE] {};
        if (write(fileTarball_, block, BConstants::BLOCK_PADDING_SIZE) == -1) {
            printf("BTarballPosixTarball::Publish: write\n");
            throw exception();
        }
        isPublished_ = true;
    }
}

void BTarballPosixTarball::Clear()
{
    ftruncate(fileTarball_, 0);
    lseek(fileTarball_, 0, SEEK_SET);
}
} // namespace OHOS::FileManagement::Backup