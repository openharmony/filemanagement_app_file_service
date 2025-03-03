/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <dlfcn.h>
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
typedef void (*ConvertFileUriToMntPath)(const std::vector<std::string> &fileUris,
                                        std::vector<std::string> &physicalPaths);
namespace {
    const string PACKAGE_NAME_FLAG = "<PackageName>";
    const string CURRENT_USER_ID_FLAG = "<currentUserId>";
    const string PHYSICAL_PATH_KEY = "src-path";
    const string SANDBOX_PATH_KEY = "sandbox-path";
    const string MOUNT_PATH_MAP_KEY = "mount-path-map";
    const string SANDBOX_JSON_FILE_PATH = "/etc/app_file_service/file_share_sandbox.json";
    const string BACKUP_SANDBOX_JSON_FILE_PATH = "/etc/app_file_service/backup_sandbox.json";
    const std::string SHARE_PATH_HEAD = "/mnt/hmdfs/";
    const std::string SHARE_PATH_MID = "/account/cloud_merge_view/files/";
    const string FILE_MANAGER_URI_HEAD = "/storage/";
    const string FILE_MANAGER_AUTHORITY = "docs";
    const string DLP_MANAGER_BUNDLE_NAME = "com.ohos.dlpmanager";
    const string FUSE_URI_HEAD = "/mnt/data/fuse";
    const char BACKSLASH = '/';
    const string MEDIA = "media";
    const string NETWORK_ID_FLAG = "<networkId>";
    const string LOCAL = "local";
    const int ASSET_IN_BUCKET_NUM_MAX = 1000;
    const int ASSET_DIR_START_NUM = 16;
    const int DECODE_FORMAT_NUM = 16;
    const std::string PATH_INVALID_FLAG1 = "../";
    const std::string PATH_INVALID_FLAG2 = "/..";
    const uint32_t PATH_INVALID_FLAG_LEN = 3;
    const std::string NETWORK_PARA = "?networkid=";
    const std::string AMPERSAND = "&";
}

struct MediaUriInfo {
    string mediaType;
    string fileId;
    string realName;
    string displayName;
};

std::unordered_map<std::string, std::string> SandboxHelper::sandboxPathMap_;
std::unordered_map<std::string, std::string> SandboxHelper::backupSandboxPathMap_;
std::mutex SandboxHelper::mapMutex_;
void* SandboxHelper::libMediaHandle_;

string SandboxHelper::Encode(const string &uri)
{
    const unordered_set<char> uriCompentsSet = {
        '/', '-', '_', '.', '!',
        '~', '*', '(', ')', '\''
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
    std::string outPutStr;
    const int32_t encodeLen = 2;
    size_t index = 0;
    while (index < uri.length()) {
        if (uri[index] == '%') {
            std::string inputStr(uri.substr(index + 1, encodeLen));
            outPutStr += static_cast<char>(strtol(inputStr.c_str(), nullptr, DECODE_FORMAT_NUM));
            index += encodeLen + 1;
        } else {
            outPutStr += uri[index];
            index++;
        }
    }

    return outPutStr;
}

static string GetLowerPath(string &lowerPathHead, const string &lowerPathTail,
                           const string &userId, const string &bundleName,
                           const string &networkId)
{
    if (lowerPathHead.find(CURRENT_USER_ID_FLAG) != string::npos) {
        lowerPathHead = lowerPathHead.replace(lowerPathHead.find(CURRENT_USER_ID_FLAG),
                                              CURRENT_USER_ID_FLAG.length(), userId);
    }

    if (lowerPathHead.find(PACKAGE_NAME_FLAG) != string::npos) {
        lowerPathHead = lowerPathHead.replace(lowerPathHead.find(PACKAGE_NAME_FLAG),
                                              PACKAGE_NAME_FLAG.length(), bundleName);
    }

    if (lowerPathHead.find(NETWORK_ID_FLAG) != string::npos) {
        lowerPathHead = lowerPathHead.replace(lowerPathHead.find(NETWORK_ID_FLAG),
                                              NETWORK_ID_FLAG.length(), networkId);
    }

    return lowerPathHead + lowerPathTail;
}

bool SandboxHelper::GetSandboxPathMap()
{
    lock_guard<mutex> lock(mapMutex_);
    if (sandboxPathMap_.size() > 0) {
        return true;
    }

    nlohmann::json jsonObj;
    int ret = JsonUtils::GetJsonObjFromPath(jsonObj, SANDBOX_JSON_FILE_PATH);
    if (ret != 0) {
        LOGE("Get json object failed with %{public}d", ret);
        return false;
    }

    if (jsonObj.find(MOUNT_PATH_MAP_KEY) == jsonObj.end()) {
        LOGE("Json object find mount path map failed");
        return false;
    }

    nlohmann::json mountPathMap = jsonObj[MOUNT_PATH_MAP_KEY];
    for (size_t i = 0; i < mountPathMap.size(); i++) {
        string srcPath = mountPathMap[i][PHYSICAL_PATH_KEY];
        string sandboxPath = mountPathMap[i][SANDBOX_PATH_KEY];
        sandboxPathMap_[sandboxPath] = srcPath;
    }

    if (sandboxPathMap_.size() == 0) {
        return false;
    }

    return true;
}

bool SandboxHelper::GetBackupSandboxPathMap()
{
    lock_guard<mutex> lock(mapMutex_);
    if (backupSandboxPathMap_.size() > 0) {
        return true;
    }

    nlohmann::json jsonObj;
    int ret = JsonUtils::GetJsonObjFromPath(jsonObj, BACKUP_SANDBOX_JSON_FILE_PATH);
    if (ret != 0) {
        LOGE("Get json object failed with %{public}d", ret);
        return false;
    }

    if (jsonObj.find(MOUNT_PATH_MAP_KEY) == jsonObj.end()) {
        LOGE("Json object find mount path map failed");
        return false;
    }

    nlohmann::json mountPathMap = jsonObj[MOUNT_PATH_MAP_KEY];
    for (size_t i = 0; i < mountPathMap.size(); i++) {
        string srcPath = mountPathMap[i][PHYSICAL_PATH_KEY];
        string sandboxPath = mountPathMap[i][SANDBOX_PATH_KEY];
        backupSandboxPathMap_[sandboxPath] = srcPath;
    }

    if (backupSandboxPathMap_.size() == 0) {
        return false;
    }

    return true;
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
    if (fileName.empty()) {
        return -EINVAL;
    }

    string tmpName = fileName;
    std::replace(tmpName.begin(), tmpName.end(), '_', ' ');
    stringstream ss;
    ss << tmpName;

    string mediaType;
    string dateTime;
    string idStr;
    string other;
    ss >> mediaType >> dateTime >> idStr >> other;
    if (idStr.empty()) {
        return -EINVAL;
    }

    if (!std::all_of(idStr.begin(), idStr.end(), ::isdigit)) {
        return -EINVAL;
    }

    return std::atoi(idStr.c_str());
}

static int32_t GetBucketNum(const std::string &fileName)
{
    int32_t fileId = GetFileIdFromFileName(fileName);
    if (fileId < 0) {
        LOGE("GetFileIdFromFileName failed with %{private}s", fileName.c_str());
        return fileId;
    }
    return CalAssetBucket(fileId);
}

static bool ParseMediaSandboxPath(const string &sandboxPath, MediaUriInfo &mediaUriInfo)
{
    string path = sandboxPath;
    std::replace(path.begin(), path.end(), '/', ' ');
    stringstream ss;
    ss << path;
    ss >> mediaUriInfo.mediaType >> mediaUriInfo.fileId >> mediaUriInfo.realName >> mediaUriInfo.displayName;

    string buf;
    ss >> buf;
    if (!buf.empty()) {
        LOGE("media sandboxPath is invalid");
        return false;
    }

    return true;
}

static int32_t GetMediaPhysicalPath(const std::string &sandboxPath, const std::string &userId,
                                    std::string &physicalPath)
{
    MediaUriInfo mediaUriInfo;
    if (!ParseMediaSandboxPath(sandboxPath, mediaUriInfo)) {
        return -EINVAL;
    }

    int32_t bucketNum = GetBucketNum(mediaUriInfo.realName);
    if (bucketNum < 0) {
        return -EINVAL;
    }

    std::string mediaSuffix;
    if (GetPathSuffix(sandboxPath, mediaSuffix) != 0) {
        LOGE("GetPathSuffix failed");
        return -EINVAL;
    }

    physicalPath = SHARE_PATH_HEAD + userId + SHARE_PATH_MID + mediaUriInfo.mediaType +
                   BACKSLASH + to_string(bucketNum) + BACKSLASH + mediaUriInfo.realName + mediaSuffix;
    return 0;
}

int32_t SandboxHelper::GetMediaSharePath(const std::vector<std::string> &fileUris,
                                         std::vector<std::string> &physicalPaths)
{
    if (libMediaHandle_ == nullptr) {
        libMediaHandle_ = dlopen("libmedia_library_handler.z.so", RTLD_LAZY | RTLD_GLOBAL);
    }
    if (libMediaHandle_ == nullptr) {
        LOGE("dlopen libmedia_library_handler.z.so failed, errno = %{public}s", dlerror());
        return -EINVAL;
    }
    ConvertFileUriToMntPath convertFileUriToMntPath =
        (ConvertFileUriToMntPath)dlsym(libMediaHandle_, "ConvertFileUriToMntPath");
    if (convertFileUriToMntPath == nullptr) {
        LOGE("GetMediaSharePath dlsym failed, errno %{public}s", dlerror());
        return -EINVAL;
    }
    convertFileUriToMntPath(fileUris, physicalPaths);
    if (fileUris.size() != physicalPaths.size()) {
        LOGE("GetMediaSharePath returns fewer results than the output parameters");
        return -EINVAL;
    }
    return 0;
}

void SandboxHelper::GetNetworkIdFromUri(const std::string &fileUri, std::string &networkId)
{
    std::string uri = fileUri;
    size_t pos = uri.find(NETWORK_PARA);
    if (pos != string::npos && pos > 0 && pos < uri.size() - NETWORK_PARA.size()) {
        if (uri.substr(pos + NETWORK_PARA.size()).find(BACKSLASH) == string::npos) {
            size_t endPos = uri.substr(pos + NETWORK_PARA.size()).find(AMPERSAND);
            if (endPos != string::npos) {
                networkId = uri.substr(pos + NETWORK_PARA.size(), endPos);
            } else {
                networkId = uri.substr(pos + NETWORK_PARA.size());
            }
        }
    }
}

static void DoGetPhysicalPath(string &lowerPathTail, string &lowerPathHead, const string &sandboxPath,
    std::unordered_map<std::string, std::string> &sandboxPathMap)
{
    string::size_type curPrefixMatchLen = 0;
    for (auto it = sandboxPathMap.begin(); it != sandboxPathMap.end(); it++) {
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
}

int32_t SandboxHelper::GetPhysicalPath(const std::string &fileUri, const std::string &userId,
                                       std::string &physicalPath)
{
    if (!IsValidPath(fileUri)) {
        LOGE("fileUri is ValidUri, The fileUri contains '/./' or '../'characters");
        return -EINVAL;
    }
    Uri uri(fileUri);
    string bundleName = uri.GetAuthority();
    if (bundleName == MEDIA) {
        return GetMediaPhysicalPath(uri.GetPath(), userId, physicalPath);
    }

    string sandboxPath = SandboxHelper::Decode(uri.GetPath());
    if ((sandboxPath.find(FILE_MANAGER_URI_HEAD) == 0 && bundleName != FILE_MANAGER_AUTHORITY) ||
        (sandboxPath.find(FUSE_URI_HEAD) == 0 && bundleName != DLP_MANAGER_BUNDLE_NAME)) {
        return -EINVAL;
    }

    if (!GetSandboxPathMap()) {
        LOGE("GetSandboxPathMap failed");
        return -EINVAL;
    }

    string lowerPathTail = "";
    string lowerPathHead = "";
    DoGetPhysicalPath(lowerPathTail, lowerPathHead, sandboxPath, sandboxPathMap_);

    if (lowerPathHead == "") {
        LOGE("lowerPathHead is invalid");
        return -EINVAL;
    }

    string networkId = LOCAL;
    GetNetworkIdFromUri(fileUri, networkId);

    physicalPath = GetLowerPath(lowerPathHead, lowerPathTail, userId, bundleName, networkId);
    return 0;
}

int32_t SandboxHelper::GetBackupPhysicalPath(const std::string &fileUri, const std::string &userId,
                                             std::string &physicalPath)
{
    if (!IsValidPath(fileUri)) {
        LOGE("fileUri is ValidUri, The fileUri contains '/./' or '../'characters");
        return -EINVAL;
    }
    Uri uri(fileUri);
    string bundleName = uri.GetAuthority();
    if (bundleName == MEDIA) {
        return GetMediaPhysicalPath(uri.GetPath(), userId, physicalPath);
    }

    string sandboxPath = SandboxHelper::Decode(uri.GetPath());
    if ((sandboxPath.find(FILE_MANAGER_URI_HEAD) == 0 && bundleName != FILE_MANAGER_AUTHORITY) ||
        (sandboxPath.find(FUSE_URI_HEAD) == 0 && bundleName != DLP_MANAGER_BUNDLE_NAME)) {
        return -EINVAL;
    }

    if (!GetBackupSandboxPathMap()) {
        LOGE("GetBackupSandboxPathMap failed");
        return -EINVAL;
    }

    string lowerPathTail = "";
    string lowerPathHead = "";
    DoGetPhysicalPath(lowerPathTail, lowerPathHead, sandboxPath, backupSandboxPathMap_);

    if (lowerPathHead == "") {
        LOGE("lowerPathHead is invalid");
        return -EINVAL;
    }

    string networkId = LOCAL;
    GetNetworkIdFromUri(fileUri, networkId);

    physicalPath = GetLowerPath(lowerPathHead, lowerPathTail, userId, bundleName, networkId);
    return 0;
}

bool SandboxHelper::IsValidPath(const std::string &filePath)
{
    if (filePath.find("/./") != std::string::npos) {
        return false;
    }
    size_t pos = filePath.find(PATH_INVALID_FLAG1);
    while (pos != string::npos) {
        if (pos == 0 || filePath[pos - 1] == BACKSLASH) {
            LOGE("Relative path is not allowed, path contain ../");
            return false;
        }
        pos = filePath.find(PATH_INVALID_FLAG1, pos + PATH_INVALID_FLAG_LEN);
    }
    pos = filePath.rfind(PATH_INVALID_FLAG2);
    if ((pos != string::npos) && (filePath.size() - pos == PATH_INVALID_FLAG_LEN)) {
        LOGE("Relative path is not allowed, path tail is /..");
        return false;
    }
    return true;
}

bool SandboxHelper::CheckValidPath(const std::string &filePath)
{
    if (filePath.empty() || filePath.size() >= PATH_MAX) {
        LOGE("filePath is invalid, size = %{public}zu", filePath.size());
        return false;
    }

    char realPath[PATH_MAX]{'\0'};
    if (realpath(filePath.c_str(), realPath) == nullptr) {
        LOGE("realpath failed with errno = %{public}d, filePath = %{private}s", errno, filePath.c_str());
        return false;
    }

    if (strncmp(realPath, filePath.c_str(), filePath.size()) != 0) {
        LOGE("filePath is not equal to realPath, realPath.size = %{public}zu, filePath.size() = %{public}zu",
            string_view(realPath).size(), filePath.size());
        return false;
    }

    return true;
}
} // namespace AppFileService
} // namespace OHOS

