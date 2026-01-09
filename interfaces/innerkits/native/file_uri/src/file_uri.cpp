/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "file_uri.h"

#include <mutex>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "uri.h"

#include "common_func.h"
#include "log.h"
#include "os_account_manager.h"
#include "parameter.h"
#include "sandbox_helper.h"

using namespace std;
namespace OHOS {
namespace AppFileService {
namespace ModuleFileUri {
const std::string PATH_SHARE = "/data/storage/el2/share";
const std::string MEDIA_FUSE_PATH_HEAD = "/data/storage/el2/";
const std::string MODE_RW = "/rw/";
const std::string MODE_R = "/r/";
const std::string FILE_SCHEME_PREFIX = "file://";
const std::string FILE_MANAGER_AUTHORITY = "docs";
const std::string MEDIA_AUTHORITY = "media";
const std::string NETWORK_PARA = "?networkid=";
const std::string FILE_MANAGER_URI_HEAD = "/storage/Users/";
const std::string DEFAULT_USERNAME = "currentUser";
const std::string FILE_HAP_URI_HEAD = "/data/storage/";
const std::string FILE_APP_DATA = "appdata";
const std::string FILE_MNT_DATA = "/mnt/data/fuse";
const std::string DLP_PACKAGE_NAME = "com.ohos.dlpmanager";
const std::string BACKSLASH = "/";
const int DECODE_FORMAT_NUM = 16;
const int32_t DECODE_LEN = 2;
std::string BUNDLE_NAME = "";
std::mutex g_globalMutex;
static string GetNextDirName(string &path)
{
    if (path.find(BACKSLASH) == 0) {
        path = path.substr(1);
    }
    string dirname = "";
    if (path.find(BACKSLASH) != string::npos) {
        size_t pos = path.find(BACKSLASH);
        dirname = path.substr(0, pos);
        path = path.substr(pos);
    }
    return dirname;
}

static std::string GetUserName()
{
    std::string userName;
    ErrCode errCode = OHOS::AccountSA::OsAccountManager::GetOsAccountShortName(userName);
    if (errCode != ERR_OK || userName.empty()) {
        LOGD("Reserved for multi-user adaptation");
    }
    userName = DEFAULT_USERNAME;
    return userName;
}

string FileUri::GetName()
{
    string sandboxPath = SandboxHelper::Decode(uri_.GetPath());
    size_t posLast = sandboxPath.find_last_of("/");
    if (posLast == string::npos) {
        return "";
    }

    if (posLast == (sandboxPath.size() - 1)) {
        return "";
    }

    return sandboxPath.substr(posLast + 1);
}

static string DecodeBySA(const string &uri)
{
    std::string outPutStr;
    size_t index = 0;
    while (index < uri.length()) {
        if (uri[index] == '%') {
            std::string inputStr(uri.substr(index + 1, DECODE_LEN));
            errno = 0;
            auto ret = strtol(inputStr.c_str(), nullptr, DECODE_FORMAT_NUM);
            if (ret == 0 || errno != 0) {
                LOGE("strtol Failed! ret: %{public}lu, %{public}d", ret, errno);
                return "";
            }
            outPutStr += static_cast<char>(ret);
            index += DECODE_LEN + 1;
        } else {
            outPutStr += uri[index];
            index++;
        }
    }
    return outPutStr;
}

string GetOpenPath(string &path, string bundleName)
{
    if (path.find(FILE_HAP_URI_HEAD) == 0) {
        path = path.substr(FILE_HAP_URI_HEAD.size());
        string encryptionLevel = GetNextDirName(path);
        string dataPart = GetNextDirName(path);
        if (dataPart.empty()) {
            if (path.empty()) {
                LOGE("The directory at the same Base level is missing");
                return "";
            }
            dataPart = path;
            path = "";
        }
        return FILE_MANAGER_URI_HEAD + GetUserName() + BACKSLASH + FILE_APP_DATA + BACKSLASH + encryptionLevel +
               BACKSLASH + dataPart + BACKSLASH + bundleName + path;
    }
    LOGE("The security encryption level directory is missing");
    return "";
}

string FileUri::GetPath()
{
    string sandboxPath = SandboxHelper::Decode(uri_.GetPath());
    string bundleName = uri_.GetAuthority();
    if (bundleName == MEDIA_AUTHORITY && sandboxPath.find(".") != string::npos) {
        size_t pos = sandboxPath.rfind("/");
        if (pos == string::npos) {
            return "";
        }
        return sandboxPath.substr(0, pos);
    }

    return sandboxPath;
}

string FileUri::GetRealPath()
{
    LOGD("GetRealPath uri is ,%{private}s", uri_.ToString().c_str());
    string sandboxPath = SandboxHelper::Decode(uri_.GetPath());
    string realPath = sandboxPath;
    string bundleName = uri_.GetAuthority();
    if (bundleName == FILE_MANAGER_AUTHORITY && uri_.ToString().find(NETWORK_PARA) == string::npos) {
        LOGD("GetRealPath return path is ,%{private}s", realPath.c_str());
        return realPath;
    }
    if (bundleName == DLP_PACKAGE_NAME && realPath.find(FILE_MNT_DATA) == 0) {
        return realPath;
    }
    if (bundleName == MEDIA_AUTHORITY) {
        realPath = MEDIA_FUSE_PATH_HEAD + bundleName + sandboxPath;
        return realPath;
    }
    if (bundleName == FILE_MANAGER_AUTHORITY && uri_.ToString().find(NETWORK_PARA) != string::npos) {
        string networkId = "";
        SandboxHelper::GetNetworkIdFromUri(uri_.ToString(), networkId);
        if (!networkId.empty()) {
            realPath = PATH_SHARE + MODE_RW + networkId + BACKSLASH + bundleName + sandboxPath;
        } else {
            realPath = PATH_SHARE + MODE_RW + bundleName + sandboxPath;
        }
        if (access(realPath.c_str(), F_OK) != 0) {
            if (!networkId.empty()) {
                realPath = PATH_SHARE + MODE_R + networkId + BACKSLASH + bundleName + sandboxPath;
            } else {
                realPath = PATH_SHARE + MODE_R + bundleName + sandboxPath;
            }
        }
        LOGD("GetRealPath return path is ,%{private}s", realPath.c_str());
        return realPath;
    }
    {
        std::lock_guard<std::mutex> lock(g_globalMutex);
        if (BUNDLE_NAME.empty()) {
            BUNDLE_NAME = CommonFunc::GetSelfBundleName();
        }
    }
    if ((!bundleName.empty()) && (bundleName != BUNDLE_NAME)) {
        realPath = GetOpenPath(realPath, bundleName);
    }
    LOGD("GetRealPath return path is ,%{private}s", realPath.c_str());
    return realPath;
}

string FileUri::GetRealPathBySA(const std::string &targeBundleName)
{
    string sandboxPath = DecodeBySA(uri_.GetPath());
    if (sandboxPath.empty() || !SandboxHelper::IsValidPath(sandboxPath)) {
        LOGE("path is ValidPath, The path contains '../' characters");
        return "";
    }
    string realPath = sandboxPath;
    string bundleName = uri_.GetAuthority();
    if (bundleName == FILE_MANAGER_AUTHORITY || bundleName == targeBundleName) {
        return realPath;
    }
    if (bundleName == DLP_PACKAGE_NAME && realPath.find(FILE_MNT_DATA) == 0) {
        return realPath;
    }
    realPath = GetOpenPath(realPath, bundleName);
    LOGI("GetRealPath return path is ,%{private}s", realPath.c_str());
    return realPath;
}

string FileUri::ToString()
{
    return uri_.ToString();
}

string FileUri::GetFullDirectoryUri()
{
    string uri = uri_.ToString();
    if (!SandboxHelper::IsValidPath(uri)) {
        LOGE("uri is ValidUri, The uri contains '../' characters");
        return "";
    }
    struct stat fileInfo;
    if (stat(GetRealPath().c_str(), &fileInfo) != 0) {
        LOGE("fileInfo is error,%{public}s", strerror(errno));
        return "";
    }
    if (S_ISREG(fileInfo.st_mode)) {
        LOGD("uri's st_mode is reg");
        size_t pos = uri.rfind("/");
        return uri.substr(0, pos);
    }
    if (S_ISDIR(fileInfo.st_mode)) {
        LOGD("uri's st_mode is dir");
        return uri;
    }
    LOGD("uri's st_mode is not reg and dir");
    return "";
}

bool FileUri::IsRemoteUri()
{
    size_t pos = uri_.ToString().find(NETWORK_PARA);
    if (pos != string::npos && pos > 0 && pos < uri_.ToString().size() - NETWORK_PARA.size()) {
        if (uri_.ToString().substr(pos + NETWORK_PARA.size()).find(BACKSLASH) == string::npos) {
            return true;
        }
    }
    return false;
}

bool FileUri::CheckUriFormat(const std::string &uri)
{
    if (uri.find(FILE_SCHEME_PREFIX) != 0) {
        LOGE("URI is missing file://");
        return false;
    }
    if (!SandboxHelper::IsValidPath(uri)) {
        LOGE("uri is ValidPath, The uri contains '../' characters");
        return false;
    }
    return true;
}

FileUri::FileUri(const string &uriOrPath)
    : uri_((uriOrPath.find(FILE_SCHEME_PREFIX) == 0) ? uriOrPath : CommonFunc::GetUriFromPath(uriOrPath))
{
}
} // namespace ModuleFileUri
} // namespace AppFileService
} // namespace OHOS