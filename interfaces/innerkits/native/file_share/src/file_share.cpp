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
#include "uri.h"

namespace OHOS {
namespace AppFileService {

struct FileShareInfo {
    string providerBundleName_;
    string targetBundleName_;
    string providerLowerPath_;
    string providerSandboxPath_;
    vector<string> sharePath_;
    vector<bool> isExist_;
    string currentUid_;
    SHARE_FILE_TYPE type_;
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
    return 0;
}

static void GetProviderBundleName(string uriStr, string &bundleName)
{
    Uri uri(uriStr);
    bundleName = uri.GetAuthority();
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

static int32_t GetLowerPath(string &lowerPathHead, const string &lowerPathTail, FileShareInfo &info)
{
    if (lowerPathHead.empty()) {
        return -EINVAL;
    }

    if (lowerPathHead.find(CURRENT_USER_ID_FLAG) != string::npos) {
        lowerPathHead = lowerPathHead.replace(lowerPathHead.find(CURRENT_USER_ID_FLAG),
                                              CURRENT_USER_ID_FLAG.length(), info.currentUid_);
    }

    if (lowerPathHead.find(PACKAGE_NAME_FLAG) != string::npos) {
        lowerPathHead = lowerPathHead.replace(lowerPathHead.find(PACKAGE_NAME_FLAG),
                                              PACKAGE_NAME_FLAG.length(), info.providerBundleName_);
    }

    info.providerLowerPath_ = lowerPathHead + lowerPathTail;
    return 0;
}

static int32_t GetProviderPath(const string &uriStr, FileShareInfo &info)
{
    Uri uri(uriStr);
    string pathInProvider = uri.GetPath();
    string::size_type pos = pathInProvider.find(DATA_STORAGE_PATH);
    if (pos == string::npos) {
        return -EINVAL;
    }

    size_t num = SANDBOX_PATH.size();
    string lowerPathTail = "", lowerPathHead = "";
    for (size_t i = 0; i < num; i++) {
        if (pathInProvider.length() >= SANDBOX_PATH[i].length()) {
            string sandboxPathTemp = pathInProvider.substr(0, SANDBOX_PATH[i].length());
            if (sandboxPathTemp == SANDBOX_PATH[i]) {
                lowerPathHead = LOWER_PATH[i];
                lowerPathTail = pathInProvider.substr(SANDBOX_PATH[i].length());
                break;
            }
        }
    }

    int32_t ret = GetLowerPath(lowerPathHead, lowerPathTail, info);
    if (ret != 0) {
        LOGE("Get lower path failed with %{public}d", ret);
        return ret;
    }

    info.providerSandboxPath_ = pathInProvider;
    return 0;
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
        info.isExist_.push_back(false);
        info.isExist_.push_back(false);
    } else if ((flag & READ_URI_PERMISSION) == READ_URI_PERMISSION) {
        info.sharePath_.push_back(shareRPath);
        info.isExist_.push_back(false);
    }
}

static int32_t GetShareFileType(FileShareInfo &info)
{
    if (IsExistFile(info.providerLowerPath_)) {
        info.type_ = FILE_TYPE;
        return 0;
    } else if (IsExistDir(info.providerLowerPath_)) {
        info.type_ = DIR_TYPE;
        return 0;
    }
    return -ENOENT;
}

static int32_t GetFileShareInfo(const string &uri, uint32_t tokenId, uint32_t flag, FileShareInfo &info)
{
    int32_t ret = 0;
    ret = GetTargetInfo(tokenId, info.targetBundleName_, info.currentUid_);
    if (ret != 0) {
        LOGE("Failed to get target info %{public}d", ret);
        return ret;
    }

    GetProviderBundleName(uri, info.providerBundleName_);
    ret = GetProviderPath(uri, info);
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

static bool RemoveDir(const string& path)
{
    stack<string> dirs, delDirs;
    dirs.push(path);
    while (dirs.size() > 0) {
        string curPath = dirs.top();
        dirs.pop();
        DIR *curDir = opendir(curPath.c_str());
        struct dirent *ptr = readdir(curDir);
        while (ptr != nullptr) {
            string subPath = curPath + "/" + string(ptr->d_name);
            if (ptr->d_type == DT_DIR && strcmp(ptr->d_name, ".") != 0 &&
                strcmp(ptr->d_name, "..") != 0) {
                dirs.push(subPath);
                delDirs.push(subPath);
            } else if (ptr->d_type != DT_DIR && remove(subPath.c_str()) != 0) {
                umount2(subPath.c_str(), MNT_DETACH);
                remove(subPath.c_str());
                LOGE("Failed to remove dir with %{public}d", errno);
            }
            ptr = readdir(curDir);
        }
        closedir(curDir);
    }

    while (delDirs.size() > 0) {
        string curPath = delDirs.top();
        if (remove(curPath.c_str()) != 0) {
            LOGE("Failed to remove dir with %{public}d", errno);
        }
        delDirs.pop();
    }

    return true;
}

static bool DeleteDir(const string &path)
{
    if (IsExistDir(path)) {
        return RemoveDir(path);
    }
    return false;
}

static int32_t PreparePreShareDir(FileShareInfo &info)
{
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
            LOGE("File %{public}s already exists", info.sharePath_[i].c_str());
            info.isExist_[i] = true;
        }
    }
    return 0;
}

int32_t FileShare::CreateShareFile(const string &uri, uint32_t tokenId, uint32_t flag)
{
    FileShareInfo info;
    int32_t ret = GetFileShareInfo(uri, tokenId, flag, info);
    if (ret != 0) {
        LOGE("Failed to get FileShareInfo with %{public}d", ret);
        return ret;
    }

    if ((ret = PreparePreShareDir(info)) != 0) {
        LOGE("PreparePreShareDir failed");
        return ret;
    }

    for (size_t i = 0; i < info.sharePath_.size(); i++) {
        if (info.isExist_[i]) {
            continue;
        }

        if (info.type_ == FILE_TYPE) {
            if ((ret = creat(info.sharePath_[i].c_str(), FILE_MODE)) < 0) {
                LOGE("Create file failed with %{public}d", errno);
                return -errno;
            }
            close(ret);
        } else if (mkdir(info.sharePath_[i].c_str(), DIR_MODE) != 0) {
            LOGE("Create dir failed with %{public}d", errno);
            return -errno;
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

static void UmountDelUris(vector<string> sharePathList, string currentUid, string bundleNameSelf)
{
    for (size_t i = 0; i < sharePathList.size(); i++) {
        Uri uri(sharePathList[i]);
        string path = uri.GetPath();
        string bundleName = uri.GetAuthority();
        string delRPath = DATA_APP_EL2_PATH + currentUid + SHARE_PATH + bundleNameSelf + SHARE_R_PATH +
                          bundleName + path;
        string delRWPath = DATA_APP_EL2_PATH + currentUid + SHARE_PATH + bundleNameSelf + SHARE_RW_PATH +
                           bundleName + path;
        if (access(delRPath.c_str(), F_OK) == 0) {
            if (umount2(delRPath.c_str(), MNT_DETACH) != 0) {
                LOGE("UmountdelRPath, umount failed with %{public}d", errno);
            }
        }
        if (access(delRWPath.c_str(), F_OK) == 0) {
            if (umount2(delRWPath.c_str(), MNT_DETACH) != 0) {
                LOGE("UmountdelRWPath, umount failed with %{public}d", errno);
            }
        }
    }
}

int32_t FileShare::DeleteShareFile(uint32_t tokenId, vector<string> sharePathList)
{
    string bundleName, currentUid;
    int32_t ret = GetTargetInfo(tokenId, bundleName, currentUid);
    if (ret != 0) {
        LOGE("Failed to delete share file %{public}d", -EINVAL);
        return -EINVAL;
    }
    UmountDelUris(sharePathList, currentUid, bundleName);

    string sharePath = DATA_APP_EL2_PATH + currentUid + SHARE_PATH + bundleName;
    if ((access(sharePath.c_str(), F_OK) == 0) && !DeleteDir(sharePath)) {
        LOGE("Delete dir failed with %{public}d", errno);
        return -errno;
    }

    LOGI("Delete Share File Successfully!");
    return 0;
}
} // namespace AppFileService
} // namespace OHOS