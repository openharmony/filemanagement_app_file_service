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
#include <fstream>
#include <stack>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "hap_token_info.h"
#include "log.h"
#include "sandbox_helper.h"
#include "common_func.h"
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
const string BROKER_SCHEME_PREFIX = "content://";
const string DATA_APP_EL2_PATH = "/data/service/el2/";
const string SHARE_R_PATH = "/r/";
const string SHARE_RW_PATH = "/rw/";
const string SHARE_PATH = "/share/";
const string EXTERNAL_PATH = "file://docs/storage/External";
const string NETWORK_PARA = "networkid=";
const int32_t DLP_COMMON = 0;
const std::string BACKSLASH = "/";
const string DOCS_TYPE = "docs";
const string FILE_DEFAULT_PATH = "/storage/Users/currentUser/";
const string FILE_PATH_TAIL = "/files/Docs";
const string FILE_PATH_HEAD = "/mnt/hmdfs/";
const string FILE_PATH_MID = "/account/device_view/";
const string FILE_MANAGER_BUNDLE_NAME = ".filemanager";
const int32_t FM_LEN = 27;
}

struct FileShareInfo {
    string providerBundleName_;
    string targetBundleName_;
    string providerLowerPath_;
    string providerSandboxPath_;
    vector<string> sharePath_;
    string currentUid_;
    ShareFileType type_;
    ino_t stIno_;
};

mutex FileShare::mapMutex_;

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
    string dirName = "" ;
    if (index != 0) {
        if (hapInfo.dlpType != DLP_COMMON) {
            bundleName = to_string(index) + "_" + bundleName;
        } else if (CommonFunc::GetDirByBundleNameAndAppIndex(bundleName, index, dirName)) {
            bundleName = dirName;
        }
    }
    return 0;
}

static void GetProviderInfo(string uriStr, FileShareInfo &info)
{
    Uri uri(uriStr);
    info.providerBundleName_ = uri.GetAuthority();
    info.providerSandboxPath_ = SandboxHelper::Decode(uri.GetPath());
    if (info.providerBundleName_ == DOCS_TYPE && info.targetBundleName_.size() == FM_LEN
        && CommonFunc::EndsWith(info.targetBundleName_, BConstants::BUNDLE_FILE_MANAGER)) {
        info.providerSandboxPath_ = FILE_DEFAULT_PATH;
    }
}

static bool IsExistDir(const string &path, FileShareInfo &info)
{
    struct stat buf = {};
    if (stat(path.c_str(), &buf) != 0) {
        return false;
    }
    info.stIno_ = buf.st_ino;
    return S_ISDIR(buf.st_mode);
}

static bool IsExistFile(const string &path, FileShareInfo &info)
{
    struct stat buf = {};
    if (stat(path.c_str(), &buf) != 0) {
        LOGE("Get path stat failed, err %{public}d", errno);
        return false;
    }
    info.stIno_ = buf.st_ino;
    return S_ISREG(buf.st_mode);
}

static bool CheckIfNeedShare(const string &uriStr, ShareFileType type, FileShareInfo &info, const string &path)
{
    if (type == ShareFileType::DIR_TYPE) {
        return true;
    }

    struct stat buf = {};
    if (stat(path.c_str(), &buf) != 0) {
        return true;
    }

    Uri uri(uriStr);
    string networkIdInfo = uri.GetQuery();
    if ((buf.st_nlink != 0 || (!networkIdInfo.empty() && networkIdInfo.find(NETWORK_PARA) == 0)) &&
        buf.st_ino == info.stIno_) {
        LOGI("no need create again");
        return false;
    }

    return true;
}

static void DelSharePath(const string &delPath)
{
    if (!SandboxHelper::CheckValidPath(delPath)) {
        LOGE("DelSharePath, umount path is invalid");
        return;
    }

    if (access(delPath.c_str(), F_OK) == 0) {
        if (umount2(delPath.c_str(), MNT_DETACH) != 0) {
            LOGE("DelSharePath, umount failed with %{public}d", errno);
        }
        if (remove(delPath.c_str()) != 0) {
            LOGE("DelSharePath, remove failed with %{public}d", errno);
        }
    }
}

static int32_t GetSharePath(const string &uri, FileShareInfo &info, uint32_t flag)
{
    string networkId = "";
    SandboxHelper::GetNetworkIdFromUri(uri, networkId);
    string shareRPath = DATA_APP_EL2_PATH + info.currentUid_ + SHARE_PATH + info.targetBundleName_ +
                        SHARE_R_PATH + info.providerBundleName_ + info.providerSandboxPath_;
    string shareRWPath = DATA_APP_EL2_PATH + info.currentUid_ + SHARE_PATH + info.targetBundleName_ +
                         SHARE_RW_PATH + info.providerBundleName_ + info.providerSandboxPath_;

    if (!networkId.empty()) {
        shareRPath = DATA_APP_EL2_PATH + info.currentUid_ + SHARE_PATH + info.targetBundleName_ + SHARE_R_PATH +
                            networkId + BACKSLASH + info.providerBundleName_ + info.providerSandboxPath_;
        shareRWPath = DATA_APP_EL2_PATH + info.currentUid_ + SHARE_PATH + info.targetBundleName_ + SHARE_RW_PATH +
                            networkId + BACKSLASH + info.providerBundleName_ + info.providerSandboxPath_;
    }
    if (!SandboxHelper::IsValidPath(shareRPath) || !SandboxHelper::IsValidPath(shareRWPath)) {
        LOGE("Invalid share path");
        return -EINVAL;
    }

    if (CheckIfNeedShare(uri, info.type_, info, shareRPath)) {
        info.sharePath_.push_back(shareRPath);
        DelSharePath(shareRWPath);
    }

    if ((flag & WRITE_URI_PERMISSION) == WRITE_URI_PERMISSION &&
        CheckIfNeedShare(uri, info.type_, info, shareRWPath)) {
        info.sharePath_.push_back(shareRWPath);
    }

    return 0;
}

static int32_t GetShareFileType(FileShareInfo &info)
{
    LOGI("GetShareFileType start");
    if (!SandboxHelper::CheckValidPath(info.providerLowerPath_)) {
        LOGE("info.providerLowerPath_ is invalid");
        return -EINVAL;
    }
    if (IsExistFile(info.providerLowerPath_, info)) {
        info.type_ = ShareFileType::FILE_TYPE;
        return 0;
    } else if (IsExistDir(info.providerLowerPath_, info)) {
        info.type_ = ShareFileType::DIR_TYPE;
        return 0;
    }
    LOGI("GetShareFileType end");
    return -ENOENT;
}

static int32_t GetDocsDir(const string &uri, FileShareInfo &info)
{
    string networkId = "";
    SandboxHelper::GetNetworkIdFromUri(uri, networkId);
    if (networkId == "") {
        LOGE("get network id failed.");
        return -EINVAL;
    }

    const string filePathPrefix = FILE_PATH_HEAD + info.currentUid_ + FILE_PATH_MID + networkId + FILE_PATH_TAIL;
    string tmpPath = info.providerLowerPath_;
    string::size_type prefixMatchLen = filePathPrefix.length();
    if (tmpPath.length() < prefixMatchLen) {
        LOGE("get docs dir failed");
        return -EINVAL;
    }

    string filePathTemp = tmpPath.substr(0, prefixMatchLen);
    if (filePathTemp == filePathPrefix) {
        info.providerLowerPath_ = filePathTemp;
        LOGI("docs dir: %{private}s", info.providerLowerPath_.c_str());
        return 0;
    }
    return -EINVAL;
}

static int32_t GetFileShareInfo(const string &uri, uint32_t tokenId, uint32_t flag, FileShareInfo &info)
{
    int32_t ret = 0;
    GetProviderInfo(uri, info);

    ret = SandboxHelper::GetPhysicalPath(uri, info.currentUid_, info.providerLowerPath_);
    if (ret != 0) {
        LOGE("Failed to get lower path %{public}d", ret);
        return ret;
    }

    if (info.providerBundleName_ == DOCS_TYPE && info.targetBundleName_.size() == FM_LEN
        && CommonFunc::EndsWith(info.targetBundleName_, BConstants::BUNDLE_FILE_MANAGER)) {
        ret = GetDocsDir(uri, info);
        if (ret != 0) {
            LOGE("Failed to get docs dir, errno: %{public}d", ret);
            return ret;
        }
    }

    ret = GetShareFileType(info);
    if (ret != 0) {
        LOGE("Failed to get share file type %{public}d", ret);
        return ret;
    }

    ret = GetSharePath(uri, info, flag);
    if (ret != 0) {
        LOGE("Failed to get share path %{public}d", ret);
        return ret;
    }
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

static bool DeleteExistShareFile(const string &path)
{
    if (access(path.c_str(), F_OK) == 0) {
        if (umount2(path.c_str(), MNT_DETACH) != 0 && errno == EBUSY) {
            LOGE("Umount failed with %{public}d", errno);
            return false;
        }
        if (remove(path.c_str()) != 0 && errno == EBUSY) {
            LOGE("DeleteExistShareFile, remove failed with %{public}d", errno);
            return false;
        }
    }
    return true;
}

static void UmountDelUris(vector<string> sharePathList, string currentUid, string bundleNameSelf)
{
    string delPathPrefix = DATA_APP_EL2_PATH + currentUid + SHARE_PATH + bundleNameSelf;
    for (size_t i = 0; i < sharePathList.size(); i++) {
        Uri uri(sharePathList[i]);
        string path = SandboxHelper::Decode(uri.GetPath());
        string bundleName = uri.GetAuthority();

        string delRPath = delPathPrefix + SHARE_R_PATH + bundleName + path;
        DelSharePath(delRPath);

        string delRWPath = delPathPrefix + SHARE_RW_PATH + bundleName + path;
        DelSharePath(delRWPath);
    }
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
            if (!DeleteExistShareFile(info.sharePath_[i])) {
                return -errno;
            }
        }
    }
    return 0;
}

static bool NotRequiredBindMount(const FileShareInfo &info, uint32_t flag, const string &uri)
{
    return (info.currentUid_ == "0" ||
        ((flag & PERSISTABLE_URI_PERMISSION) != 0 && uri.find(EXTERNAL_PATH) != 0) ||
        uri.find(BROKER_SCHEME_PREFIX) == 0);
}

static int32_t StartShareFile(const FileShareInfo &info)
{
    for (size_t i = 0; i < info.sharePath_.size(); i++) {
        if (info.type_ == ShareFileType::FILE_TYPE) {
            int fd = open(info.sharePath_[i].c_str(), O_RDONLY | O_CREAT,
                          S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
            if (fd < 0) {
                LOGE("Create file failed with %{public}d", errno);
                return -errno;
            }
            close(fd);
        } else {
            if (access(info.sharePath_[i].c_str(), 0) != 0 &&
                mkdir(info.sharePath_[i].c_str(), DIR_MODE) != 0) {
                LOGE("Failed to make dir with %{public}d", errno);
                return -errno;
            }
        }

        if (mount(info.providerLowerPath_.c_str(), info.sharePath_[i].c_str(),
                  nullptr, MS_BIND, nullptr) != 0) {
            LOGE("Mount failed with %{public}d", errno);
            return -errno;
        }
    }

    return 0;
}

static int32_t CreateSingleShareFile(const string &uri, uint32_t tokenId, uint32_t flag, FileShareInfo &info)
{
    LOGD("CreateShareFile begin");
    if (NotRequiredBindMount(info, flag, uri)) {
        LOGD("Not required to bind mount");
        return 0;
    }

    int32_t ret = GetFileShareInfo(uri, tokenId, flag, info);
    if (ret != 0) {
        LOGE("Failed to get FileShareInfo with %{public}d", ret);
        return ret;
    }

    if (info.sharePath_.size() == 0) {
        LOGI("no need share path, Create Share File Successfully!");
        return 0;
    }

    ret = PreparePreShareDir(info);
    if (ret != 0) {
        return ret;
    }
    LOGI("StartShareFile start");
    ret = StartShareFile(info);
    if (ret != 0) {
        return ret;
    }

    info.sharePath_.clear();
    LOGI("Create Share File Successfully!");
    return 0;
}

int32_t FileShare::CreateShareFile(const vector<string> &uriList,
                                   uint32_t tokenId,
                                   uint32_t flag,
                                   vector<int32_t> &retList)
{
    LOGI("CreateShareFile start");
    lock_guard<mutex> lock(mapMutex_);
    FileShareInfo info;
    int32_t ret = GetTargetInfo(tokenId, info.targetBundleName_, info.currentUid_);
    if (ret != 0) {
        retList.push_back(ret);
        LOGE("Failed to get target info %{public}d", ret);
        return ret;
    }

    for (const auto &uri : uriList) {
        int32_t curRet = CreateSingleShareFile(uri, tokenId, flag, info);
        retList.push_back(curRet);
        if (curRet != 0) {
            ret = curRet;
            LOGE("Create share file failed with %{public}d", errno);
        }
    }
    return ret;
}

int32_t FileShare::DeleteShareFile(uint32_t tokenId, const vector<string> &uriList)
{
    lock_guard<mutex> lock(mapMutex_);
    string bundleName;
    string currentUid;
    int32_t ret = GetTargetInfo(tokenId, bundleName, currentUid);
    if (ret != 0) {
        LOGE("Failed to delete share file %{public}d", -EINVAL);
        return -EINVAL;
    }
    UmountDelUris(uriList, currentUid, bundleName);

    LOGI("Delete Share File Successfully!");
    return 0;
}
} // namespace AppFileService
} // namespace OHOS