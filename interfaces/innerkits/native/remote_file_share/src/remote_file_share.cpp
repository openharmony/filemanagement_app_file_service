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

#include <climits>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"
#ifdef ENABLE_DEVICE_MANAGER
#include "device_manager.h"
#include "device_manager_callback.h"
#endif
#include "iservice_registry.h"
#include "istorage_manager.h"
#include "sandbox_helper.h"
#include "securec.h"
#include "system_ability_definition.h"
#include "unique_fd.h"
#include "uri.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleRemoteFileShare {
#ifdef ENABLE_DEVICE_MANAGER
using namespace OHOS::DistributedHardware;
#endif
namespace {
const int E_OK = 0;
const int HMDFS_CID_SIZE = 64;
const std::string FILE_SCHEME = "file";
const std::string BACKSLASH = "/";
const std::string DISTRIBUTED_DIR_PATH = "/data/storage/el2/distributedfiles";
const std::string DST_PATH_HEAD = "/data/service/el2/";
const std::string DST_PATH_MID = "/hmdfs/account/data/";
const std::string DST_PATH_MID_FILES = "/hmdfs/account/files/";
const std::string SHARE_PATH_HEAD = "/mnt/hmdfs/";
const std::string SHARE_PATH_MID = "/account/merge_view/services/";
const std::string SHARE_PATH_DIR = "/.share";
const std::string REMOTE_SHARE_PATH_DIR = "/.remote_share";
const std::string MEDIA_AUTHORITY = "media";
const std::string FILE_MANAGER_AUTHORITY = "docs";
const std::string PACKAGE_NAME = "get_dfs_uri_from_local";
const std::string NETWORK_PARA = "?networkid=";
const std::string MEDIA_BUNDLE_NAME = "com.ohos.medialibrary.medialibrarydata";
const std::string FILE_MANAGER_URI_HEAD = "/storage/";
const std::string REMOTE_SHARE_PATH_MID = "hmdfs/";
const std::string SANDBOX_BASE_DIR = "/data/storage/el2/base";
const std::string SANDBOX_CLOUD_DIR = "/data/storage/el2/cloud";
const std::string MEDIA_PHOTO_PATH = "/Photo";
const std::string MEDIA_PATH_PREFIX_ONE = "/account/cloud_merge_view/files";
const std::string MEDIA_PATH_PREFIX_TWO = "/account/merge_view/files";
constexpr int32_t GET_CLIENT_RETRY_TIMES = 5;
constexpr int32_t SLEEP_TIME = 1;
} //namespace


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

#ifdef ENABLE_DEVICE_MANAGER
class InitDMCallback : public DmInitCallback {
public:
    InitDMCallback() = default;
    ~InitDMCallback() override = default;
    void OnRemoteDied() override {};
};
#endif

static int GetMediaDistributedDir(const int &userId, std::string &distributedDir, const std::string &networkId)
{
    distributedDir = DST_PATH_HEAD + std::to_string(userId) + DST_PATH_MID + MEDIA_BUNDLE_NAME +
        REMOTE_SHARE_PATH_DIR + BACKSLASH + networkId + MEDIA_PHOTO_PATH;
    if (distributedDir.size() >= PATH_MAX) {
        LOGE("Path is too long with %{public}zu", distributedDir.size());
        return -EINVAL;
    }
    LOGI("media distributedDir: %{private}s", distributedDir.c_str());
    return E_OK;
}

static std::string GetSandboxDir(Uri uri)
{
    std::string sandboxPath = SandboxHelper::Decode(uri.GetPath());
    std::string sandboxDir = "";
    if (sandboxPath.rfind(DISTRIBUTED_DIR_PATH, 0) == 0) {
        sandboxDir = DISTRIBUTED_DIR_PATH;
    } else if (sandboxPath.rfind(SANDBOX_BASE_DIR, 0) == 0) {
        sandboxDir = SANDBOX_BASE_DIR;
    } else if (sandboxPath.rfind(SANDBOX_CLOUD_DIR, 0) == 0) {
        sandboxDir = SANDBOX_CLOUD_DIR;
    } else {
        LOGE("Get distributed dir failed.");
        return "";
    }
    return sandboxDir;
}

static bool IsDistributedFileDir(Uri uri)
{
    std::string sandboxDir = GetSandboxDir(uri);
    return sandboxDir == DISTRIBUTED_DIR_PATH ? true : false;
}

static bool IsCloudFileDir(Uri uri)
{
    std::string sandboxDir = GetSandboxDir(uri);
    return sandboxDir == SANDBOX_CLOUD_DIR ? true : false;
}

static int GetDistributedDir(const int &userId, std::string &distributedDir, const std::string &bundleName,
    const std::string &networkId, Uri uri)
{
    std::string sandboxDir = GetSandboxDir(uri);
    if (sandboxDir == "") {
        LOGE("Get sandbox dir failed");
        return -EINVAL;
    }

    distributedDir = DST_PATH_HEAD + std::to_string(userId) + DST_PATH_MID + bundleName + REMOTE_SHARE_PATH_DIR +
        BACKSLASH + networkId + sandboxDir;
    if (distributedDir.size() >= PATH_MAX) {
        LOGE("Path is too long with %{public}zu", distributedDir.size());
        return -EINVAL;
    }
    LOGI("bundleName: %{public}s, distributedDir: %{private}s", bundleName.c_str(), distributedDir.c_str());
    return E_OK;
}

static int32_t GetMediaPhysicalDir(const int32_t &userId, std::string &physicalDir, const std::string &bundleName)
{
    if (bundleName != MEDIA_BUNDLE_NAME) {
        LOGE("bundleName is not media type.");
        return -EINVAL;
    }
    physicalDir = DST_PATH_HEAD + std::to_string(userId) + DST_PATH_MID_FILES + MEDIA_PHOTO_PATH;
    if (physicalDir.size() >= PATH_MAX) {
        LOGE("Path is too long with %{public}zu", physicalDir.size());
        return -EINVAL;
    }
    LOGI("bundleName: %{public}s, physicalDir: %{private}s", bundleName.c_str(), physicalDir.c_str());
    return E_OK;
}

static std::string GetPhysicalDir(Uri &uri, const int32_t &userId)
{
    std::string sandboxPath = SandboxHelper::Decode(uri.GetPath());
    if (!SandboxHelper::IsValidPath(sandboxPath) || uri.GetScheme() != FILE_SCHEME) {
        LOGE("Sandbox path from uri is error");
        return "";
    }

    std::string physicalDir = "";
    int ret = SandboxHelper::GetPhysicalDir(uri.ToString(), std::to_string(userId), physicalDir);
    if (ret != E_OK) {
        LOGE("Get physical path failed with %{public}d", ret);
        return "";
    }
    return physicalDir;
}

static std::string GetPhysicalPath(Uri &uri, const std::string &userId)
{
    std::string sandboxPath = SandboxHelper::Decode(uri.GetPath());
    if (!SandboxHelper::IsValidPath(sandboxPath) || uri.GetScheme() != FILE_SCHEME) {
        LOGE("Sandbox path from uri is error");
        return "";
    }

    std::string physicalPath = "";
    int ret = SandboxHelper::GetPhysicalPath(uri.ToString(), userId, physicalPath);
    if (ret != E_OK) {
        LOGE("Get physical path failed with %{public}d", ret);
        return "";
    }
    return physicalPath;
}

static std::string GetLocalNetworkId()
{
    const std::string LOCAL = "local";
    std::string networkId = LOCAL;
#ifdef ENABLE_DEVICE_MANAGER
    auto callback = std::make_shared<InitDMCallback>();
    int32_t ret = DeviceManager::GetInstance().InitDeviceManager(PACKAGE_NAME, callback);
    if (ret != 0) {
        return "";
    }

    DmDeviceInfo info;
    ret = DeviceManager::GetInstance().GetLocalDeviceInfo(PACKAGE_NAME, info);
    networkId = std::string(info.networkId);
    LOGD("GetLocalNetworkId :%{private}s", networkId.c_str());
    if (ret != 0 || networkId.empty()) {
        return "";
    }
#endif
    return networkId;
}

static int32_t SetFileSize(const std::string &physicalPath, struct HmdfsUriInfo &hui)
{
    struct stat buf = {};
    if (stat(physicalPath.c_str(), &buf) != E_OK) {
        int32_t ret = -errno;
        LOGE("Failed to get physical path stat with %{public}d", ret);
        return ret;
    }
    hui.fileSize = static_cast<size_t>(buf.st_size);
    return E_OK;
}

static int32_t SetPublicDirHmdfsInfo(const std::string &physicalPath, const std::string &uriStr,
                                     struct HmdfsUriInfo &hui, const std::string &networkId)
{
    hui.uriStr = uriStr + NETWORK_PARA + networkId;
    struct stat buf = {};
    if (stat(physicalPath.c_str(), &buf) != E_OK) {
        LOGE("Failed to get physical path stat with %{public}d", -errno);
        return -errno;
    }
    hui.fileSize = static_cast<size_t>(buf.st_size);
    return E_OK;
}

static int32_t UriCategoryByType(const std::vector<std::string> &uriList,
                                 std::vector<std::string> &mediaUriList,
                                 std::vector<std::string> &otherUriList)
{
    LOGI("GetDfsUrisDirFromLocal UriCategoryByType start");
    if (uriList.size() == 0) {
        LOGE("Parameter uriList is NULL");
        return -EINVAL;
    }
    for (auto &uriStr : uriList) {
        Uri uri(uriStr);
        std::string bundleName = uri.GetAuthority();
        if (bundleName == MEDIA_AUTHORITY) {
            mediaUriList.push_back(uriStr);
        } else {
            otherUriList.push_back(uriStr);
        }
    }
    return E_OK;
}

static sptr<StorageManager::IStorageManager> GetStorageManager()
{
    auto saMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saMgr == nullptr) {
        LOGI("Get samgr failed.");
        return nullptr;
    }

    int32_t count = 0;
    sptr<StorageManager::IStorageManager> storageManager = nullptr;
    while (storageManager == nullptr && count < GET_CLIENT_RETRY_TIMES) {
        count++;
        auto storageObj = saMgr->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
        if (storageObj == nullptr) {
            LOGE("Get starage manger failed.");
            sleep(SLEEP_TIME);
            continue;
        }

        storageManager = iface_cast<StorageManager::IStorageManager>(storageObj);
        if (storageManager == nullptr) {
            LOGE("Iface_cast failed.");
            sleep(SLEEP_TIME);
            continue;
        }
    }
    return storageManager;
}

static int32_t MountDisFileShare(const int32_t &userId, const std::string &distributedDir,
    const std::string &physicalDir)
{
    if (distributedDir == "" || physicalDir == "") {
        LOGE("Invalid distributedDir or physicalDir.");
        return -EINVAL;
    }
    auto storageMgr = GetStorageManager();
    if (storageMgr == nullptr) {
        LOGE("Get storage manager failed.");
        return -EINVAL;
    }

    std::map<std::string, std::string> shareFiles = {
        {distributedDir, physicalDir}
    };
    int32_t ret = storageMgr->MountDisShareFile(userId, shareFiles);
    if (ret != E_OK) {
        LOGE("MountDisShareFile failed.");
        return ret;
    }
    return E_OK;
}

static std::string GetMediaSandboxPath(const std::string &physicalPath, const std::string &usrId)
{
    const std::vector<std::string> prefixList = {
        SHARE_PATH_HEAD + usrId + MEDIA_PATH_PREFIX_ONE,
        SHARE_PATH_HEAD + usrId + MEDIA_PATH_PREFIX_TWO
    };
    std::string mediaSandboxPath = "";
    for (size_t i = 0; i < prefixList.size(); i++) {
        std::string sandboxPathPrefix = prefixList[i];
        std::string::size_type prefixMatchLen = sandboxPathPrefix.length();
        if (physicalPath.length() >= prefixMatchLen) {
            std::string sandboxPathTemp = physicalPath.substr(0, prefixMatchLen);
            if (sandboxPathTemp == sandboxPathPrefix) {
                mediaSandboxPath = physicalPath.substr(prefixMatchLen);
                LOGI("mediaSandboxPath: %{private}s", mediaSandboxPath.c_str());
                return mediaSandboxPath;
            }
        }
    }
    return mediaSandboxPath;
}

static int32_t SetHmdfsUriDirInfo(struct HmdfsUriInfo &hui, Uri &uri, const std::string &physicalPath,
    const std::string &networkId, const std::string &bundleName)
{
    hui.uriStr = FILE_SCHEME + "://" + bundleName + DISTRIBUTED_DIR_PATH + REMOTE_SHARE_PATH_DIR + BACKSLASH +
        networkId + uri.GetPath() + NETWORK_PARA + networkId;
    return SetFileSize(physicalPath, hui);
}

static int32_t SetDistributedfilesHmdfsUriDirInfo(const std::string &physicalPath, const std::string &uriStr,
    struct HmdfsUriInfo &hui, const std::string &networkId)
{
    hui.uriStr = uriStr + NETWORK_PARA + networkId;
    return SetFileSize(physicalPath, hui);
}

static int32_t SetMediaHmdfsUriDirInfo(struct HmdfsUriInfo &hui, Uri &uri, const std::string &physicalPath,
    const std::string &networkId, const std::string &usrId)
{
    std::string authority = uri.GetAuthority();
    if (authority != MEDIA_AUTHORITY) {
        LOGE("not meida authority.");
        return -EINVAL;
    }
    std::string mediaSandboxPath = GetMediaSandboxPath(physicalPath, usrId);
    if (mediaSandboxPath == "") {
        LOGE("mediaSandboxPath is null.");
        return -EINVAL;
    }
    hui.uriStr = FILE_SCHEME + "://" + MEDIA_BUNDLE_NAME + DISTRIBUTED_DIR_PATH + REMOTE_SHARE_PATH_DIR + BACKSLASH +
        networkId + mediaSandboxPath + NETWORK_PARA + networkId;
    return SetFileSize(physicalPath, hui);
}


static int32_t GetMediaDfsUrisDirFromLocal(const std::vector<std::string> &uriList, const int32_t &userId,
    std::unordered_map<std::string, HmdfsUriInfo> &uriToDfsUriMaps)
{
    if (uriList.size() <= 0) {
        LOGE("Failed to uri GetMediaDfsDirFromLocal, list is null or empty");
        return -EINVAL;
    }

    std::string networkId = GetLocalNetworkId();
    std::string distributedDir;
    int ret = GetMediaDistributedDir(userId, distributedDir, networkId);
    if (ret != E_OK) {
        LOGE("Failed to uri get distribute dir.");
        return ret;
    }

    std::string physicalDir = "";
    ret = GetMediaPhysicalDir(userId, physicalDir, MEDIA_BUNDLE_NAME);
    if (ret != E_OK) {
        LOGE("Failed to uri get physical dir.");
        return ret;
    }

    ret = MountDisFileShare(userId, distributedDir, physicalDir);
    if (ret != E_OK) {
        LOGE("Failed to mount.");
        return ret;
    }

    std::vector<std::string> physicalPaths;
    int getPhysicalPathRet = SandboxHelper::GetMediaSharePath(uriList, physicalPaths);
    if (getPhysicalPathRet != E_OK) {
        LOGE("Failed to get physical path.");
        return -EINVAL;
    }
    for (size_t i = 0; i < uriList.size(); i++) {
        Uri uri(uriList[i]);
        HmdfsUriInfo dfsUriInfo;
        SetMediaHmdfsUriDirInfo(dfsUriInfo, uri, physicalPaths[i], networkId, std::to_string(userId));
        uriToDfsUriMaps.insert({uriList[i], dfsUriInfo});
        LOGI("dfsUri: %{private}s", dfsUriInfo.uriStr.c_str());
    }
    return E_OK;
}

static int32_t DoMount(const int &userId, const std::string &bundleName, const std::string &networkId, Uri uri)
{
    std::string distributedDir;
    int32_t ret = GetDistributedDir(userId, distributedDir, bundleName, networkId, uri);
    if (ret != E_OK) {
        LOGE("Failed to get distributed dir.");
        return ret;
    }

    std::string physicalDir = GetPhysicalDir(uri, userId);
    if (physicalDir == "") {
        LOGE("Failed to get physicalDir dir.");
        return -EINVAL;
    }

    ret = MountDisFileShare(userId, distributedDir, physicalDir);
    if (ret != E_OK) {
        LOGE("Failed to mount, errno: %{public}d.", ret);
        return ret;
    }
    return E_OK;
}

static int32_t CheckIfNeedMount(const std::string &bundleName, const std::string &networkId, Uri uri,
    const std::string &physicalPath, std::unordered_map<std::string, HmdfsUriInfo> &uriToDfsUriMaps)
{
    HmdfsUriInfo dfsUriInfo;
    std::string uriStr = uri.ToString();
    if (bundleName == FILE_MANAGER_AUTHORITY) {
        (void)SetPublicDirHmdfsInfo(physicalPath, uriStr, dfsUriInfo, networkId);
        uriToDfsUriMaps.insert({uriStr, dfsUriInfo});
        LOGI("bundleName: %{public}s, dfsUri: %{private}s", uriStr.c_str(), dfsUriInfo.uriStr.c_str());
        return E_OK;
    }
    if (IsDistributedFileDir(uri) || IsCloudFileDir(uri)) {
        (void)SetDistributedfilesHmdfsUriDirInfo(physicalPath, uriStr, dfsUriInfo, networkId);
        uriToDfsUriMaps.insert({uriStr, dfsUriInfo});
        LOGI("bundleName: %{public}s, dfsUri: %{private}s", uriStr.c_str(), dfsUriInfo.uriStr.c_str());
        return E_OK;
    }
    return -EINVAL;
}

int32_t RemoteFileShare::GetDfsUrisDirFromLocal(const std::vector<std::string> &uriList, const int32_t &userId,
    std::unordered_map<std::string, HmdfsUriInfo> &uriToDfsUriMaps)
{
    std::vector<std::string> otherUriList;
    std::vector<std::string> mediaUriList;
    int32_t ret = UriCategoryByType(uriList, mediaUriList, otherUriList);
    if (ret == E_OK && mediaUriList.size() != 0) {
        ret = GetMediaDfsUrisDirFromLocal(mediaUriList, userId, uriToDfsUriMaps);
    }
    if (ret != E_OK) {
        LOGE("uri category failed or process media uri failed.");
        return ret;
    }
    std::string networkId = GetLocalNetworkId();
    for (const auto &uriStr : otherUriList) {
        Uri uri(uriStr);
        std::string physicalPath = GetPhysicalPath(uri, std::to_string(userId));
        if (physicalPath == "") {
            LOGE("Failed to get physical path");
            return -EINVAL;
        }

        std::string bundleName = uri.GetAuthority();
        ret = CheckIfNeedMount(bundleName, networkId, uri, physicalPath, uriToDfsUriMaps);
        if (ret == E_OK) {
            continue;
        }

        ret = DoMount(userId, bundleName, networkId, uri);
        if (ret != E_OK) {
            LOGE("Failed to mount, %{public}d", ret);
            return ret;
        }
        HmdfsUriInfo dfsUriInfo;
        (void)SetHmdfsUriDirInfo(dfsUriInfo, uri, physicalPath, networkId, bundleName);
        uriToDfsUriMaps.insert({uriStr, dfsUriInfo});
        LOGI("bundleName: %{public}s, dfsUri: %{private}s", bundleName.c_str(), dfsUriInfo.uriStr.c_str());
    }
    LOGI("GetDfsUrisDirFromLocal successfully");
    return E_OK;
}

int32_t RemoteFileShare::TransRemoteUriToLocal(const std::vector<std::string> &uriList,
                                               const std::string &networkId,
                                               const std::string &deviceId,
                                               std::vector<std::string> &resultList)
{
    if (networkId.empty() || deviceId.empty()) {
        LOGE("RemoteFileShare::TransRemoteUriToLocal, invalid argument with %{public}d", EINVAL);
        return EINVAL;
    }
    constexpr int splitThree = 3;
    bool allValid = true;
    std::vector<std::string> tmpResultList;
    for (auto &uriStr : uriList) {
        Uri uri(uriStr);
        std::string bundleName = uri.GetAuthority();
        std::string sandboxPath = SandboxHelper::Decode(uri.GetPath());
        if (!SandboxHelper::IsValidPath(sandboxPath) || uri.GetScheme() != FILE_SCHEME) {
            LOGE("Sandbox path from uri is error");
            allValid = false;
            break;
        }
        if ((bundleName != FILE_MANAGER_AUTHORITY) || (sandboxPath.find(FILE_MANAGER_URI_HEAD) != 0)) {
            LOGE("Sandbox path doesn't begin with docs/storage");
            allValid = false;
            break;
        }
        int cnt = 0;
        size_t pos = 0;
        std::string part;
        while (cnt < splitThree && pos != std::string::npos) {
            pos = sandboxPath.find('/', pos + 1);
            cnt++;
        }
        if (pos != std::string::npos) {
            part = sandboxPath.substr(pos + 1);
        }
        if (part.empty()) {
            allValid = false;
            break;
        }
        std::string localUri = FILE_SCHEME + "://" + bundleName + FILE_MANAGER_URI_HEAD +
                               REMOTE_SHARE_PATH_MID + deviceId + "/" + part;
        tmpResultList.push_back(localUri);
    }
    if (!allValid) {
        LOGW("Failed to update uriList");
        resultList = uriList;
        return -EINVAL;
    }
    resultList = tmpResultList;
    return E_OK;
}
} // namespace ModuleRemoteFileShare
} // namespace AppFileService
} // namespace OHOS
