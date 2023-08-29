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

#include "remote_file_share.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <climits>
#include <pthread.h>

#include "log.h"
#include "device_manager.h"
#include "device_manager_callback.h"
#include "sandbox_helper.h"
#include "securec.h"
#include "uri.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleRemoteFileShare {
using namespace OHOS::DistributedHardware;
namespace {
    const int HMDFS_CID_SIZE = 64;
    const int USER_ID_INIT = 100;
    const unsigned HMDFS_IOC = 0xf2;
    const std::string FILE_SCHEME = "file";
    const std::string DISTRIBUTED_DIR_PATH = "/data/storage/el2/distributedfiles";
    const std::string DST_PATH_HEAD = "/data/service/el2/";
    const std::string DST_PATH_MID = "/hmdfs/account/data/";
    const std::string SHAER_PATH_HEAD = "/mnt/hmdfs/";
    const std::string SHAER_PATH_MID = "/account/merge_view/services/";
    const std::string LOWER_SHARE_PATH_HEAD = "/mnt/hmdfs/";
    const std::string LOWER_SHARE_PATH_MID = "/account/device_view/local/services/";
    const std::string SHARE_PATH_DIR = "/.share";
    const std::string REMOTE_SHARE_PATH_DIR = "/.remote_share";
    const std::string MEDIA_AUTHORITY = "media";
    const std::string FILE_MANAGER_AUTHORITY = "docs";
    const std::string PACKAGE_NAME = "get_dfs_uri_from_local";
    const std::string NETWORK_PARA = "?networkid=";
}

#define HMDFS_IOC_SET_SHARE_PATH    _IOW(HMDFS_IOC, 1, struct HmdfsShareControl)
#define HMDFS_IOC_GET_DST_PATH _IOR(HMDFS_IOC, 3, unsigned int)

struct HmdfsShareControl {
    int fd;
    char deviceId[HMDFS_CID_SIZE];
};

struct HmdfsDstInfo {
    uint64_t localLen;
    uint64_t localPathIndex;
    uint64_t distributedLen;
    uint64_t distributedPathIndex;
    uint64_t bundleNameLen;
    uint64_t bundleNameIndex;
    uint64_t size;
};

class InitDMCallback : public DmInitCallback {
public:
    InitDMCallback() = default;
    ~InitDMCallback() override = default;
    void OnRemoteDied() override {};
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

    ret = readlink(buf, filePath, PATH_MAX);
    if (ret < 0 || ret >= PATH_MAX) {
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

static bool IsValidPath(const std::string &path)
{
    if (path.find("/./") != std::string::npos ||
        path.find("/../") != std::string::npos) {
        return false;
    }
    return true;
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
    if (!IsValidPath(PACKAGE_PATH)) {
        LOGE("RemoteFileShare::CreateSharePath, GetLowerSharePath failed with %{private}s", PACKAGE_PATH.c_str());
        return EACCES;
    }

    const std::string LOWER_SHARE_PATH = PACKAGE_PATH + SHARE_PATH_DIR;
    if (CreateShareDir(PACKAGE_PATH) != 0)
        return errno;
    if (CreateShareDir(LOWER_SHARE_PATH) != 0) {
        DeleteShareDir(PACKAGE_PATH, LOWER_SHARE_PATH);
        return errno;
    }

    const std::string SHARE_PATH = GetSharePath(userId, processName) + SHARE_PATH_DIR;
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

static int GetDistributedPath(Uri &uri, const int &userId, std::string &distributedPath)
{
    distributedPath = DST_PATH_HEAD + std::to_string(userId) + DST_PATH_MID +
                      uri.GetAuthority() + REMOTE_SHARE_PATH_DIR + uri.GetPath();
    if (distributedPath.size() >= PATH_MAX) {
        return -EINVAL;
    }

    return 0;
}

static std::string GetPhysicalPath(Uri &uri, const std::string &userId)
{
    std::string sandboxPath = uri.GetPath();
    if (!IsValidPath(sandboxPath) || uri.GetScheme() != FILE_SCHEME) {
        LOGE("Sandbox path from uri is error with %{public}s", sandboxPath.c_str());
        return "";
    }

    std::string physicalPath = "";
    int ret = SandboxHelper::GetPhysicalPath(uri.ToString(), userId, physicalPath);
    if (ret != 0) {
        LOGE("Get physical path failed with %{public}d", ret);
        return "";
    }
    return physicalPath;
}

static void InitHmdfsInfo(struct HmdfsDstInfo &hdi, const std::string &physicalPath,
                          const std::string &distributedPath, const std::string &bundleName)
{
    hdi.localLen = physicalPath.size() + 1;
    hdi.localPathIndex = reinterpret_cast<uint64_t>(physicalPath.c_str());

    hdi.distributedLen = distributedPath.size() + 1;
    hdi.distributedPathIndex = reinterpret_cast<uint64_t>(distributedPath.c_str());

    hdi.bundleNameLen = bundleName.size() + 1;
    hdi.bundleNameIndex = reinterpret_cast<uint64_t>(bundleName.c_str());

    hdi.size = reinterpret_cast<uint64_t>(&hdi.size);
}

static std::string GetLocalNetworkId()
{
    auto callback = std::make_shared<InitDMCallback>();
    int32_t ret = DeviceManager::GetInstance().InitDeviceManager(PACKAGE_NAME, callback);
    if (ret != 0) {
        return "";
    }

    DmDeviceInfo info;
    ret = DeviceManager::GetInstance().GetLocalDeviceInfo(PACKAGE_NAME, info);
    auto networkId = std::string(info.networkId);
    LOGD("GetLocalNetworkId :%{private}s", networkId.c_str());
    if (ret != 0 || networkId.empty()) {
        return "";
    }
    return networkId;
}

static void SetHmdfsUriInfo(struct HmdfsUriInfo &hui, Uri &uri, uint64_t fileSize)
{
    std::string bundleName = uri.GetAuthority();
    std::string path = uri.GetPath();
    std::string networkId = NETWORK_PARA + GetLocalNetworkId();

    hui.uriStr = SandboxHelper::Encode(FILE_SCHEME + "://" + bundleName + DISTRIBUTED_DIR_PATH +
                                       REMOTE_SHARE_PATH_DIR + path + networkId);
    hui.fileSize = fileSize;
    return;
}

static int32_t SetPublicDirHmdfsInfo(const std::string &physicalPath, const std::string &uriStr,
                                     struct HmdfsUriInfo &hui)
{
    hui.uriStr = uriStr + NETWORK_PARA + GetLocalNetworkId();
    struct stat buf = {};
    if (stat(physicalPath.c_str(), &buf) != 0) {
        LOGE("Failed to get physical path stat with %{public}d", -errno);
        return -errno;
    }
    hui.fileSize = static_cast<size_t>(buf.st_size);
    return 0;
}

int32_t RemoteFileShare::GetDfsUriFromLocal(const std::string &uriStr, const int32_t &userId,
                                            struct HmdfsUriInfo &hui)
{
    Uri uri(SandboxHelper::Decode(uriStr));
    std::string bundleName = uri.GetAuthority();
    LOGD("GetDfsUriFromLocal begin with uri:%{private}s, decode uri:%{private}s",
         uriStr.c_str(), uri.ToString().c_str());
    std::string physicalPath = GetPhysicalPath(uri, std::to_string(userId));
    if (physicalPath == "") {
        LOGE("Failed to get physical path");
        return -EINVAL;
    }

    if (bundleName == MEDIA_AUTHORITY || bundleName == FILE_MANAGER_AUTHORITY) {
        (void)SetPublicDirHmdfsInfo(physicalPath, uriStr, hui);
        LOGD("GetDfsUriFromLocal successfully with %{private}s", hui.uriStr.c_str());
        return 0;
    }

    std::string distributedPath;
    int ret = GetDistributedPath(uri, userId, distributedPath);
    if (ret != 0) {
        LOGE("Path is too long with %{public}d", ret);
        return ret;
    }

    struct HmdfsDstInfo hdi;
    LOGD("PhysicalPath: %{private}s DistributedPath: %{private}s BundleName: %{private}s",
         physicalPath.c_str(), distributedPath.c_str(), bundleName.c_str());
    InitHmdfsInfo(hdi, physicalPath, distributedPath, bundleName);

    std::string ioctlDir = SHAER_PATH_HEAD + std::to_string(userId) + SHAER_PATH_MID;
    int32_t dirFd = open(ioctlDir.c_str(), O_RDONLY);
    if (dirFd < 0) {
        LOGE("Open share path failed with %{public}d", errno);
        return errno;
    }

    ret = ioctl(dirFd, HMDFS_IOC_GET_DST_PATH, &hdi);
    if (ret != 0) {
        LOGE("Ioctl failed with %{public}d", errno);
        close(dirFd);
        return -errno;
    }

    close(dirFd);
    SetHmdfsUriInfo(hui, uri, hdi.size);
    LOGD("GetDfsUriFromLocal successfully with %{private}s", hui.uriStr.c_str());
    return 0;
}
} // namespace ModuleRemoteFileShare
} // namespace AppFileService
} // namespace OHOS
