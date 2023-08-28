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

#include "sandbox_helper.h"

#include <iomanip>
#include <sstream>
#include <unordered_set>
#include <vector>

#include "log.h"
#include "json_utils.h"
#include "uri.h"

using namespace std;

namespace OHOS {
namespace AppFileService {
namespace {
    const string PACKAGE_NAME_FLAG = "<PackageName>";
    const string CURRENT_USER_ID_FLAG = "<currentUserId>";
    const string PHYSICAL_PATH_KEY = "src-path";
    const string SANDBOX_PATH_KEY = "sandbox-path";
    const string MOUNT_PATH_MAP_KEY = "mount-path-map";
    const string SANDBOX_JSON_FILE_PATH = "/etc/app_file_service/file_share_sandbox.json";
    const std::string SHAER_PATH_HEAD = "/mnt/hmdfs/";
    const std::string SHAER_PATH_MID = "/account/cloud_merge_view/files/";
    const string FILE_MANAGER_URI_HEAD = "/storage/";
    const string FILE_MANAGER_AUTHORITY = "docs";
    const string DLP_MANAGER_BUNDLE_NAME = "com.ohos.dlpmanager";
    const string FUSE_URI_HEAD = "/mnt/data/fuse";
    const string BACKFLASH = "/";
    const string MEDIA = "media";
    const int ASSET_IN_BUCKET_NUM_MAX = 1000;
    const int ASSET_DIR_START_NUM = 16;
}

struct MediaUriInfo {
    string mediaType;
    string fileId;
    string realName;
    string displayName;
};

std::unordered_map<std::string, std::string> SandboxHelper::sandboxPathMap_;

string SandboxHelper::Encode(const string &uri)
{
    const unordered_set<char> uriCompentsSet = {
        ';', ',', '/', '?', ':', '@', '&',
        '=', '+', '$', '-', '_', '.', '!',
        '~', '*', '(', ')', '#', '\''
    };
    const int32_t encodeLen = 2;
    ostringstream outPutStream;
    outPutStream.fill('0');
    outPutStream << std::hex;

    for (unsigned char tmpChar : uri) {
        if (std::isalnum(tmpChar) || uriCompentsSet.find(tmpChar) != uriCompentsSet.end()) {
            outPutStream << tmpChar;
        } else {
            outPutStream << std::uppercase;
            outPutStream << '%' << std::setw(encodeLen) << static_cast<unsigned int>(tmpChar);
            outPutStream << std::nouppercase;
        }
    }

    return outPutStream.str();
}

string SandboxHelper::Decode(const string &uri)
{
    std::ostringstream outPutStream;
    const int32_t encodeLen = 2;
    size_t index = 0;
    while (index < uri.length()) {
        if (uri[index] == '%') {
            int hex = 0;
            std::istringstream inputStream(uri.substr(index + 1, encodeLen));
            inputStream >> std::hex >> hex;
            outPutStream << static_cast<char>(hex);
            index += encodeLen + 1;
        } else {
            outPutStream << uri[index];
            index++;
        }
    }

    return outPutStream.str();
}

static string GetLowerPath(string &lowerPathHead, const string &lowerPathTail,
                           const string &userId, const string &bundleName)
{
    if (lowerPathHead.find(CURRENT_USER_ID_FLAG) != string::npos) {
        lowerPathHead = lowerPathHead.replace(lowerPathHead.find(CURRENT_USER_ID_FLAG),
                                              CURRENT_USER_ID_FLAG.length(), userId);
    }

    if (lowerPathHead.find(PACKAGE_NAME_FLAG) != string::npos) {
        lowerPathHead = lowerPathHead.replace(lowerPathHead.find(PACKAGE_NAME_FLAG),
                                              PACKAGE_NAME_FLAG.length(), bundleName);
    }

    return lowerPathHead + lowerPathTail;
}

static void GetSandboxPathMap(unordered_map<string, string> &sandboxPathMap)
{
    nlohmann::json jsonObj;
    int ret = JsonUtils::GetJsonObjFromPath(jsonObj, SANDBOX_JSON_FILE_PATH);
    if (ret != 0) {
        LOGE("Get json object failed from %{public}s with %{public}d", SANDBOX_JSON_FILE_PATH.c_str(), ret);
        return;
    }

    if (jsonObj.find(MOUNT_PATH_MAP_KEY) == jsonObj.end()) {
        LOGE("Json object find mount path map failed");
        return;
    }

    nlohmann::json mountPathMap = jsonObj[MOUNT_PATH_MAP_KEY];
    for (size_t i = 0; i < mountPathMap.size(); i++) {
        string srcPath = mountPathMap[i][PHYSICAL_PATH_KEY];
        string sandboxPath = mountPathMap[i][SANDBOX_PATH_KEY];
        sandboxPathMap[sandboxPath] = srcPath;
    }
    return;
}

static int32_t GetPathSuffix(const std::string &path, string &pathSuffix)
{
    size_t pos = path.rfind('.');
    if (pos != string::npos) {
        pathSuffix = path.substr(pos);
        return 0;
    }
    return -EINVAL;
}

static int32_t CalAssetBucket(const int32_t &fileId)
{
    int32_t bucketNum = 0;
    if (fileId < 0) {
        LOGE("input fileId %{private}d is invalid", fileId);
        return -EINVAL;
    }

    int32_t start = ASSET_DIR_START_NUM;
    int32_t divider = ASSET_DIR_START_NUM;
    while (fileId > start * ASSET_IN_BUCKET_NUM_MAX) {
        divider = start;
        start <<= 1;
    }

    int32_t fileIdRemainder = fileId % divider;
    if (fileIdRemainder == 0) {
        bucketNum = start + fileIdRemainder;
    } else {
        bucketNum = (start - divider) + fileIdRemainder;
    }
    return bucketNum;
}

static int32_t GetFileIdFromFileName(const std::string &fileName)
{
    size_t pos = fileName.find_last_of('_');
    if (pos == std::string::npos || pos == fileName.size() - 1) {
        return -EINVAL;
    }

    std::string idStr = fileName.substr(pos + 1);
    if (!std::all_of(idStr.begin(), idStr.end(), ::isdigit)) {
        return -EINVAL;
    }

    return std::stoi(idStr);
}

static int32_t GetBucketNum(const std::string &fileName)
{
    int32_t fileId = GetFileIdFromFileName(fileName);
    if (fileId < 0) {
        LOGE("GetFileIdFromFileName failed with %{public}s", fileName.c_str());
        return fileId;
    }
    return CalAssetBucket(fileId);
}

static void ParseMediaSandboxPath(const string &sandboxPath, MediaUriInfo &mediaUriInfo)
{
    string path = sandboxPath;
    std::replace(path.begin(), path.end(), '/', ' ');
    stringstream ss;
    ss << path;
    ss >> mediaUriInfo.mediaType >> mediaUriInfo.fileId >> mediaUriInfo.realName >> mediaUriInfo.displayName;
}

static int32_t GetMediaPhysicalPath(const std::string &sandboxPath, const std::string &userId,
                                    std::string &physicalPath)
{
    MediaUriInfo mediaUriInfo;
    ParseMediaSandboxPath(sandboxPath, mediaUriInfo);

    int32_t bucketNum = GetBucketNum(mediaUriInfo.realName);
    if (bucketNum < 0) {
        return -EINVAL;
    }

    std::string mediaSuffix;
    if (GetPathSuffix(sandboxPath, mediaSuffix) != 0) {
        LOGE("GetPathSuffix failed");
        return -EINVAL;
    }

    physicalPath = SHAER_PATH_HEAD + userId + SHAER_PATH_MID + mediaUriInfo.mediaType +
                   BACKFLASH + to_string(bucketNum) + BACKFLASH + mediaUriInfo.realName + mediaSuffix;
    return 0;
}

int32_t SandboxHelper::GetPhysicalPath(const std::string &fileUri, const std::string &userId,
                                       std::string &physicalPath)
{
    Uri uri(fileUri);
    string bundleName = uri.GetAuthority();
    string sandboxPath = uri.GetPath();

    if (bundleName == MEDIA) {
        return GetMediaPhysicalPath(sandboxPath, userId, physicalPath);
    }

    if ((sandboxPath.find(FILE_MANAGER_URI_HEAD) == 0 && bundleName != FILE_MANAGER_AUTHORITY) ||
        (sandboxPath.find(FUSE_URI_HEAD) == 0 && bundleName != DLP_MANAGER_BUNDLE_NAME)) {
        return -EINVAL;
    }

    string lowerPathTail = "";
    string lowerPathHead = "";

    if (sandboxPathMap_.size() == 0) {
        GetSandboxPathMap(sandboxPathMap_);
    }

    string::size_type curPrefixMatchLen = 0;
    for (auto it = sandboxPathMap_.begin(); it != sandboxPathMap_.end(); it++) {
        string sandboxPathPrefix = it->first;
        string::size_type prefixMatchLen = sandboxPathPrefix.length();
        if (sandboxPath.length() >= prefixMatchLen) {
            string sandboxPathTemp = sandboxPath.substr(0, prefixMatchLen);
            if (sandboxPathTemp == sandboxPathPrefix && curPrefixMatchLen <= prefixMatchLen) {
                curPrefixMatchLen = prefixMatchLen;
                lowerPathHead = it->second;
                lowerPathTail = sandboxPath.substr(prefixMatchLen);
            }
        }
    }

    if (lowerPathHead == "") {
        return -EINVAL;
    } else {
        physicalPath = GetLowerPath(lowerPathHead, lowerPathTail, userId, bundleName);
        return 0;
    }
}

bool SandboxHelper::CheckValidPath(const std::string &filePath)
{
    if (filePath.empty() || filePath.size() >= PATH_MAX) {
        return false;
    }

    char realPath[PATH_MAX]{'\0'};
    if (realpath(filePath.c_str(), realPath) != nullptr &&
        strncmp(realPath, filePath.c_str(), filePath.size()) == 0) {
        return true;
    } else {
        return false;
    }
}
} // namespace AppFileService
} // namespace OHOS

