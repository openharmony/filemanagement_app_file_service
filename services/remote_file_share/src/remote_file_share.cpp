/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "remote_file_share.h"
#include "remote_file_share_log.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <climits>
#include <pthread.h>

#include "securec.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleRemoteFileShare {
namespace {
    const int HMDFS_CID_SIZE = 64;
    const int USER_ID_INIT = 100;
    const unsigned HMDFS_IOC = 0xf2;
    const std::string SHAER_PATH_HEAD = "/mnt/hmdfs/";
    const std::string SHAER_PATH_MID = "/account/merge_view/services/";
    const std::string LOWER_SHARE_PATH_HEAD = "/mnt/hmdfs/";
    const std::string LOWER_SHARE_PATH_MID = "/account/device_view/local/services/";
}

#define HMDFS_IOC_SET_SHARE_PATH    _IOW(HMDFS_IOC, 1, struct HmdfsShareControl)

struct HmdfsShareControl {
    int fd;
    char deviceId[HMDFS_CID_SIZE];
};

static std::string GetProcessName()
{
    char pthreadName[PATH_MAX];
    int ret = pthread_getname_np(pthread_self(), pthreadName, sizeof(pthreadName));
    if (ret != 0) {
        LOGE("RemoteFileShare::GetProcessName, pthread_getname_np failed with %{public}d", errno);
        return "";
    }
    std::string pthreadNameStr = pthreadName;
    LOGI("RemoteFileShare::GetProcessName, thread name is %{public}s", pthreadNameStr.c_str());
    return pthreadNameStr;
}

static std::string GetFileName(const int &fd)
{
    char buf[PATH_MAX] = {'\0'};
    char filePath[PATH_MAX] = {'\0'};

    int ret = snprintf_s(buf, sizeof(buf), sizeof(buf), "/proc/self/fd/%d", fd);
    if (ret < 0) {
        LOGE("RemoteFileShare::GetFileName, snprintf failed with %{public}d", errno);
        return "";
    }

    ret = readlink(buf, filePath, PATH_MAX - 1);
    if (ret < 0) {
        LOGE("RemoteFileShare::GetFileName, readlink failed with %{public}d", errno);
        return "";
    }

    std::string fileName = filePath;
    std::size_t firstSlash = fileName.rfind("/");
    if (firstSlash == fileName.npos) {
        LOGE("RemoteFileShare::GetFileName, get error path with %{public}s", fileName.c_str());
        return "";
    }
    fileName = fileName.substr(firstSlash + 1, fileName.size() - firstSlash);
    return fileName;
}

static int CreateShareDir(const std::string &path)
{
    if (access(path.c_str(), F_OK) != 0) {
        int ret = mkdir(path.c_str(), S_IRWXU);
        if (ret != 0) {
            LOGE("RemoteFileShare::CreateShareDir, make dir failed with %{public}d", errno);
            return errno;
        }
    }
    return 0;
}

static std::string GetSharePath(const int &userId, const std::string &packageName)
{
    return SHAER_PATH_HEAD + std::to_string(userId) + SHAER_PATH_MID + packageName;
}

static std::string GetLowerSharePath(const int &userId, const std::string &packageName)
{
    return LOWER_SHARE_PATH_HEAD + std::to_string(userId) + LOWER_SHARE_PATH_MID + packageName;
}

static bool DeleteShareDir(const std::string &PACKAGE_PATH, const std::string &SHARE_PATH)
{
    bool result = true;
    if (access(SHARE_PATH.c_str(), F_OK) == 0) {
        int ret = rmdir(SHARE_PATH.c_str());
        if (ret != 0) {
            LOGE("RemoteFileShare::DeleteShareDir, delete dir failed with %{public}d", errno);
            result = false;
        } else {
            LOGI("RemoteFileShare::DeleteShareDir, delete %{public}s path successfully", SHARE_PATH.c_str());
        }
    }
    if (access(PACKAGE_PATH.c_str(), F_OK) == 0) {
        int ret = rmdir(PACKAGE_PATH.c_str());
        if (ret != 0) {
            LOGE("RemoteFileShare::DeleteShareDir, delete dir failed with %{public}d", errno);
            result = false;
        } else {
            LOGI("RemoteFileShare::DeleteShareDir, delete %{public}s path successfully", PACKAGE_PATH.c_str());
        }
    }
    return result;
}

static int CreateShareFile(struct HmdfsShareControl &shareControl, const char* file,
                           const std::string &deviceId)
{
    int32_t dirFd = open(file, O_RDONLY);
    if (dirFd < 0) {
        LOGE("RemoteFileShare::CreateShareFile, open share path failed with %{public}d", errno);
        return errno;
    }

    memset_s(shareControl.deviceId, HMDFS_CID_SIZE, '\0', HMDFS_CID_SIZE);
    if (memcpy_s(shareControl.deviceId, HMDFS_CID_SIZE, deviceId.c_str(), deviceId.size()) != 0) {
        LOGE("RemoteFileShare::CreateShareFile, memcpy_s failed with %{public}d", errno);
        close(dirFd);
        return errno;
    }

    if (ioctl(dirFd, HMDFS_IOC_SET_SHARE_PATH, &shareControl) < 0) {
        LOGE("RemoteFileShare::CreateShareFile, ioctl failed with %{public}d", errno);
        close(dirFd);
        return errno;
    }
    close(dirFd);
    return 0;
}

static int CheckInputValidity(const int &fd, const int &userId, const std::string &deviceId)
{
    return (fd < 0) || (userId < USER_ID_INIT) || (deviceId != SHARE_ALL_DEVICE &&
                                                        deviceId.size() != HMDFS_CID_SIZE);
}

int RemoteFileShare::CreateSharePath(const int &fd, std::string &sharePath,
                                     const int &userId, const std::string &deviceId)
{
    struct HmdfsShareControl shareControl;
    shareControl.fd = fd;

    if (CheckInputValidity(fd, userId, deviceId) != 0) {
        LOGE("RemoteFileShare::CreateSharePath, invalid argument with %{public}d", EINVAL);
        return EINVAL;
    }
    
    const std::string processName = GetProcessName();
    if (processName == "") {
        LOGE("RemoteFileShare::CreateSharePath, GetProcessName failed with %{public}d", errno);
        return errno;
    }

    const std::string PACKAGE_PATH = GetLowerSharePath(userId, processName);
    const std::string LOWER_SHARE_PATH = PACKAGE_PATH + "/.share";
    if (CreateShareDir(PACKAGE_PATH) != 0)
        return errno;
    if (CreateShareDir(LOWER_SHARE_PATH) != 0) {
        DeleteShareDir(PACKAGE_PATH, LOWER_SHARE_PATH);
        return errno;
    }

    const std::string SHARE_PATH = GetSharePath(userId, processName) + "/.share";
    char realPath[PATH_MAX] = {'\0'};
    if (!realpath(SHARE_PATH.c_str(), realPath)) {
        LOGE("RemoteFileShare::CreateSharePath, realpath failed with %{public}d", errno);
        DeleteShareDir(PACKAGE_PATH, LOWER_SHARE_PATH);
        return errno;
    }

    std::string file_name = GetFileName(shareControl.fd);
    if (file_name == "") {
        LOGE("RemoteFileShare::CreateSharePath, get error file name");
        DeleteShareDir(PACKAGE_PATH, LOWER_SHARE_PATH);
        return EBADF;
    }
    sharePath = SHARE_PATH + "/" + file_name;

    if (CreateShareFile(shareControl, realPath, deviceId) != 0) {
        LOGE("RemoteFileShare::CreateSharePath, create share file failed with %{public}d", errno);
        /* When the file is exist, we should not delete the dictionary */
        if (errno == EEXIST) {
            return 0;
        }
        sharePath = "";
        DeleteShareDir(PACKAGE_PATH, LOWER_SHARE_PATH);
        return errno;
    }
    LOGI("RemoteFileShare::CreateSharePath, create %{public}s successfully", sharePath.c_str());
    return 0;
}
} // namespace ModuleRemoteFileShare
} // namespace AppFileService
} // namespace OHOS
