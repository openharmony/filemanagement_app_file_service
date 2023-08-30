/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "file_share.h"

#include <dirent.h>
#include <fcntl.h>
#include <stack>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "hap_token_info.h"
#include "log.h"
#include "sandbox_helper.h"
#include "uri.h"

namespace OHOS {
namespace AppFileService {
#define DIR_MODE (S_IRWXU | S_IXGRP | S_IXOTH)
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define READ_URI_PERMISSION OHOS::AAFwk::Want::FLAG_AUTH_READ_URI_PERMISSION
#define WRITE_URI_PERMISSION OHOS::AAFwk::Want::FLAG_AUTH_WRITE_URI_PERMISSION
#define PERSISTABLE_URI_PERMISSION OHOS::AAFwk::Want::FLAG_AUTH_PERSISTABLE_URI_PERMISSION

enum ShareFileType {
    DIR_TYPE = 0,
    FILE_TYPE = 1,
};

namespace {
const string FILE_SCHEME = "file";
const string DATA_APP_EL2_PATH = "/data/service/el2/";
const string SHARE_R_PATH = "/r/";
const string SHARE_RW_PATH = "/rw/";
const string SHARE_PATH = "/share/";
}

struct FileShareInfo {
    string providerBundleName_;
    string targetBundleName_;
    string providerLowerPath_;
    string providerSandboxPath_;
    vector<string> sharePath_;
    string currentUid_;
    ShareFileType type_;
};

static int32_t GetTargetInfo(uint32_t tokenId, string &bundleName, string &currentUid)
{
    Security::AccessToken::HapTokenInfo hapInfo;
    int32_t result = Security::AccessToken::AccessTokenKit::GetHapTokenInfo(tokenId, hapInfo);
    if (result != 0) {
        LOGE("Failed to get hap token info %{public}d", result);
        return result;
    }
    bundleName = hapInfo.bundleName;
    currentUid = to_string(hapInfo.userID);

    int index = hapInfo.instIndex;
    if (index != 0) {
        bundleName += "_" + to_string(index);
    }
    return 0;
}

static void GetProviderInfo(string uriStr, FileShareInfo &info)
{
    Uri uri(uriStr);
    info.providerBundleName_ = uri.GetAuthority();
    info.providerSandboxPath_ = uri.GetPath();
}

static bool IsExistDir(const string &path)
{
    struct stat buf = {};
    if (stat(path.c_str(), &buf) != 0) {
        return false;
    }
    return S_ISDIR(buf.st_mode);
}

static bool IsExistFile(const string &path)
{
    struct stat buf = {};
    if (stat(path.c_str(), &buf) != 0) {
        LOGE("Get path stat failed, path: %s err %{public}d", path.c_str(), errno);
        return false;
    }
    return S_ISREG(buf.st_mode);
}

static void GetSharePath(FileShareInfo &info, uint32_t flag)
{
    string shareRPath = DATA_APP_EL2_PATH + info.currentUid_ + SHARE_PATH +info.targetBundleName_ +
                        SHARE_R_PATH + info.providerBundleName_ + info.providerSandboxPath_;
    string shareRWPath = DATA_APP_EL2_PATH + info.currentUid_ + SHARE_PATH +info.targetBundleName_ +
                         SHARE_RW_PATH + info.providerBundleName_ + info.providerSandboxPath_;
    if ((flag & WRITE_URI_PERMISSION) == WRITE_URI_PERMISSION) {
        info.sharePath_.push_back(shareRWPath);
        info.sharePath_.push_back(shareRPath);
    } else if ((flag & READ_URI_PERMISSION) == READ_URI_PERMISSION) {
        info.sharePath_.push_back(shareRPath);
    }
}

static int32_t GetShareFileType(FileShareInfo &info)
{
    if (IsExistFile(info.providerLowerPath_)) {
        info.type_ = ShareFileType::FILE_TYPE;
        return 0;
    } else if (IsExistDir(info.providerLowerPath_)) {
        info.type_ = ShareFileType::DIR_TYPE;
        return 0;
    }
    return -ENOENT;
}

static int32_t GetFileShareInfo(const string &uri, uint32_t tokenId, uint32_t flag, FileShareInfo &info)
{
    int32_t ret = 0;
    ret = GetTargetInfo(tokenId, info.targetBundleName_, info.currentUid_);
    if (ret != 0 || info.currentUid_ == "0") {
        LOGE("Failed to get target info %{public}d", ret);
        return ret;
    }

    GetProviderInfo(uri, info);

    ret = SandboxHelper::GetPhysicalPath(uri, info.currentUid_, info.providerLowerPath_);
    if (ret != 0) {
        LOGE("Failed to get lower path %{public}d", ret);
        return ret;
    }

    ret = GetShareFileType(info);
    if (ret != 0) {
        LOGE("Failed to get share file type %{public}d", ret);
        return ret;
    }

    GetSharePath(info, flag);
    return 0;
}

static bool MakeDir(const string &path)
{
    string::size_type index = 0;
    string subPath;
    do {
        index = path.find('/', index + 1);
        if (index == string::npos) {
            subPath = path;
        } else {
            subPath = path.substr(0, index);
        }

        if (access(subPath.c_str(), 0) != 0) {
            if (mkdir(subPath.c_str(), DIR_MODE) != 0) {
                LOGE("Failed to make dir with %{public}d", errno);
                return false;
            }
        }
    } while (index != string::npos);

    return true;
}

static void DeleteExistShareFile(const string &path)
{
    if (access(path.c_str(), F_OK) == 0) {
        if (umount2(path.c_str(), MNT_DETACH) != 0) {
            LOGE("Umount failed with %{public}d", errno);
        }
        remove(path.c_str());
    }
}

static void DelSharePath(const string &delPath)
{
    if (!SandboxHelper::CheckValidPath(delPath)) {
        LOGE("DelSharePath, umount path is invalid, path = %{private}s", delPath.c_str());
        return;
    }

    if (access(delPath.c_str(), F_OK) == 0) {
        if (umount2(delPath.c_str(), MNT_DETACH) != 0) {
            LOGE("DelSharePath, umount failed with %{public}d", errno);
        }
        remove(delPath.c_str());
    }
}

static void UmountDelUris(vector<string> sharePathList, string currentUid, string bundleNameSelf)
{
    string delPathPrefix = DATA_APP_EL2_PATH + currentUid + SHARE_PATH + bundleNameSelf;
    for (size_t i = 0; i < sharePathList.size(); i++) {
        Uri uri(SandboxHelper::Decode(sharePathList[i]));
        string path = uri.GetPath();
        string bundleName = uri.GetAuthority();

        string delRPath = delPathPrefix + SHARE_R_PATH + bundleName + path;
        DelSharePath(delRPath);

        string delRWPath = delPathPrefix + SHARE_RW_PATH + bundleName + path;
        DelSharePath(delRWPath);
    }
}

static int32_t PreparePreShareDir(FileShareInfo &info)
{
    if (!SandboxHelper::CheckValidPath(info.providerLowerPath_)) {
        LOGE("Invalid share path with %{private}s", info.providerLowerPath_.c_str());
        return -EINVAL;
    }

    for (size_t i = 0; i < info.sharePath_.size(); i++) {
        if (access(info.sharePath_[i].c_str(), F_OK) != 0) {
            string sharePathDir = info.sharePath_[i];
            size_t posLast = info.sharePath_[i].find_last_of("/");
            sharePathDir = info.sharePath_[i].substr(0, posLast);
            if (!MakeDir(sharePathDir.c_str())) {
                LOGE("Make dir failed with %{public}d", errno);
                return -errno;
            }
        } else {
            DeleteExistShareFile(info.sharePath_[i]);
        }
    }
    return 0;
}

int32_t CreateShareFile(const string &uri, uint32_t tokenId, uint32_t flag)
{
    FileShareInfo info;
    string decodeUri = SandboxHelper::Decode(uri);
    LOGD("CreateShareFile begin with uri %{private}s decodeUri %{private}s",
         uri.c_str(), decodeUri.c_str());
    int32_t ret = GetFileShareInfo(decodeUri, tokenId, flag, info);
    if (ret != 0 || info.currentUid_ == "0" || (flag & PERSISTABLE_URI_PERMISSION) != 0) {
        LOGE("Failed to get FileShareInfo with %{public}d", ret);
        return ret;
    }

    if ((ret = PreparePreShareDir(info)) != 0) {
        LOGE("PreparePreShareDir failed");
        return ret;
    }

    for (size_t i = 0; i < info.sharePath_.size(); i++) {
        if (info.type_ == ShareFileType::FILE_TYPE) {
            if ((ret = creat(info.sharePath_[i].c_str(), FILE_MODE)) < 0) {
                LOGE("Create file failed with %{public}d", errno);
                return -errno;
            }
            close(ret);
        } else {
            LOGE("Invalid argument not support dir to share");
            return -EINVAL;
        }

        if (mount(info.providerLowerPath_.c_str(), info.sharePath_[i].c_str(),
                  nullptr, MS_BIND, nullptr) != 0) {
            LOGE("Mount failed with %{public}d", errno);
            return -errno;
        }
    }
    LOGI("Create Share File Successfully!");
    return 0;
}

int32_t DeleteShareFile(uint32_t tokenId, vector<string> sharePathList)
{
    string bundleName, currentUid;
    int32_t ret = GetTargetInfo(tokenId, bundleName, currentUid);
    if (ret != 0) {
        LOGE("Failed to delete share file %{public}d", -EINVAL);
        return -EINVAL;
    }
    UmountDelUris(sharePathList, currentUid, bundleName);

    LOGI("Delete Share File Successfully!");
    return 0;
}
} // namespace AppFileService
} // namespace OHOS