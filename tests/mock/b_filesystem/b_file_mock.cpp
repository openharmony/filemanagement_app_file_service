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

#include "b_filesystem/b_file.h"

#include <sys/sendfile.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <unique_fd.h>

namespace OHOS::FileManagement::Backup {
using namespace std;

unique_ptr<char[]> BFile::ReadFile(const UniqueFd &fd)
{
    if (lseek(fd, 0, SEEK_SET) == -1) {
        GTEST_LOG_(INFO) << "BFile::ReadFile lseek error " << errno;
        return make_unique<char[]>(1);
    }

    struct stat stat = {};
    if (fstat(fd, &stat) == -1) {
        GTEST_LOG_(INFO) << "BFile::ReadFile fstat error " << errno;
        return make_unique<char[]>(1);
    }
    off_t fileSize = stat.st_size;
    if (fileSize == 0) {
        GTEST_LOG_(INFO) << "BFile::ReadFile fileSize error " << fileSize;
        return make_unique<char[]>(1);
    }

    auto buf = make_unique<char[]>(fileSize + 1);
    if (read(fd, buf.get(), fileSize) == -1) {
        GTEST_LOG_(INFO) << "BFile::ReadFile read error " << errno;
    }
    return buf;
}

void BFile::SendFile(int outFd, int inFd)
{
    if (outFd <= 0 || inFd <= 0) {
        return;
    }
    off_t offset = 0;
    long ret = 0;
    if (lseek(outFd, 0, SEEK_SET) == -1) {
        GTEST_LOG_(INFO) << "BFile::SendFile lseek1 error " << errno;
        return;
    }
    if (lseek(inFd, 0, SEEK_SET) == -1) {
        GTEST_LOG_(INFO) << "BFile::SendFile lseek2 error " << errno;
        return;
    }
    struct stat stat = {};
    if (fstat(inFd, &stat) == -1) {
        GTEST_LOG_(INFO) << "BFile::SendFile fstat error " << errno;
        return;
    }
    while ((ret = sendfile(outFd, inFd, &offset, stat.st_size)) > 0) {
    };

    if (ret == -1) {
        GTEST_LOG_(INFO) << "BFile::SendFile ret error " << ret;
    }
}

void BFile::Write(const UniqueFd &fd, const string &str) {}

bool BFile::CopyFile(const string &from, const string &to)
{
    return true;
}
} // namespace OHOS::FileManagement::Backup