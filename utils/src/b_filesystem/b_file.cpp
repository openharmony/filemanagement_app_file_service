/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <cstring>
#include <fcntl.h>
#include <libgen.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "securec.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

unique_ptr<char[]> BFile::ReadFile(const UniqueFd &fd)
{
    if (lseek(fd, 0, SEEK_SET) == -1) {
        throw BError(errno);
    }

    struct stat stat = {};
    if (fstat(fd, &stat) == -1) {
        throw BError(errno);
    }
    off_t fileSize = stat.st_size;
    if (fileSize == 0) {
        HILOGI("Deserialized an empty file");
        return make_unique<char[]>(1);
    }

    auto buf = make_unique<char[]>(fileSize + 1);
    if (read(fd, buf.get(), fileSize) == -1) {
        throw BError(errno);
    }
    return buf;
}

void BFile::SendFile(int outFd, int inFd)
{
    off_t offset = 0;
    long ret = 0;
    if (lseek(outFd, 0, SEEK_SET) == -1) {
        throw BError(errno);
    }
    if (lseek(inFd, 0, SEEK_SET) == -1) {
        throw BError(errno);
    }
    struct stat stat = {};
    if (fstat(inFd, &stat) == -1) {
        throw BError(errno);
    }
    while ((ret = sendfile(outFd, inFd, &offset, stat.st_size)) > 0) {
    };

    if (ret == -1) {
        throw BError(errno);
    }
    ret = ftruncate(outFd, offset);
    if (ret == -1) {
        throw BError(errno);
    }
}

void BFile::Write(const UniqueFd &fd, const string &str)
{
    int ret = pwrite(fd, str.c_str(), str.length(), 0);
    if (ret == -1) {
        throw BError(errno);
    }
    if (ftruncate(fd, ret) == -1) {
        throw BError(errno);
    }
}

bool BFile::CopyFile(const string &from, const string &to)
{
    if (from == to) {
        return true;
    }

    try {
        auto resolvedPath = make_unique<char[]>(PATH_MAX + 1); // 1: size for '\0'
        if (!realpath(from.data(), resolvedPath.get())) {
            HILOGI("failed to real path for the file %{public}s", from.c_str());
            return false;
        }
        string oldPath(resolvedPath.get());
        UniqueFd fdFrom(open(oldPath.data(), O_RDONLY));
        if (fdFrom == -1) { // -1: fd error code
            HILOGE("failed to open the file %{public}s", from.c_str());
            throw BError(errno);
        }

        unique_ptr<char, function<void(void *p)>> dir {strdup(to.data()), free};
        if (!dir) {
            throw BError(errno);
        }
        if (!realpath(dirname(dir.get()), resolvedPath.get())) {
            HILOGE("failed to real path for %{public}s", to.c_str());
            return false;
        }
        string newPath(resolvedPath.get());
        unique_ptr<char, function<void(void *p)>> name {strdup(to.data()), free};
        if (!name) {
            throw BError(errno);
        }
        newPath.append("/").append(basename(name.get()));
        UniqueFd fdTo(open(newPath.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
        if (fdTo == -1) { // -1: fd error code
            HILOGE("failed to open the file %{public}s", to.c_str());
            throw BError(errno);
        }

        SendFile(fdTo, fdFrom);
        return true;
    } catch (const BError &e) {
        HILOGE("%{public}s", e.what());
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
    } catch (...) {
        HILOGE("");
    }
    return false;
}

bool BFile::GetRealPath(const string &path, string &realPath)
{
    auto tmpPath = make_unique<char[]>(PATH_MAX + 1);
    int ret = memset_s(tmpPath.get(), PATH_MAX + 1, 0, PATH_MAX + 1);
    if (ret != EOK) {
        HILOGE("Failed to call path memset_s, err = %{public}d", ret);
        return false;
    }
    if (!realpath(path.c_str(), tmpPath.get())) {
        HILOGE("failed to real path for the file %{public}s, errno: %{public}d",
            GetAnonyPath(path).c_str(), errno);
        return false;
    }
    realPath = string(tmpPath.get());
    return true;
}

bool BFile::MoveFile(const string &from, const string &to)
{
    if (from.size() >= PATH_MAX || to.size() >= PATH_MAX) {
        HILOGE("from or to path err (Exceeding the maximum length)");
        return false;
    }
    if (from == to) {
        return true;
    }
    try {
        std::string oldPath = "";
        if (!GetRealPath(from, oldPath)) {
            return false;
        }
        unique_ptr<char, function<void(void *p)>> dir {strdup(to.data()), free};
        if (!dir) {
            throw BError(errno);
        }
        std::string newPath = "";
        if (!GetRealPath(dirname(dir.get()), newPath)) {
            return false;
        }
        unique_ptr<char, function<void(void *p)>> name {strdup(to.data()), free};
        if (!name) {
            throw BError(errno);
        }
        newPath.append("/").append(basename(name.get()));
        if (rename(oldPath.c_str(), newPath.c_str())) {
            HILOGE("failed to rename path, oldPath: %{public}s, newPath: %{public}s, errno: %{public}d",
                GetAnonyPath(oldPath).c_str(), GetAnonyPath(newPath).c_str(), errno);
            throw BError(errno);
        }
        return true;
    } catch (const BError &e) {
        HILOGE("%{public}s", e.what());
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
    } catch (...) {
        HILOGE("unknow error");
    }
    return false;
}
} // namespace OHOS::FileManagement::Backup