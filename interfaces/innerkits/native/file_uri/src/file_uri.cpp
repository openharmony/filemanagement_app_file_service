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

#include "file_uri.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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
const std::string MODE_RW = "/rw/";
const std::string MODE_R = "/r/";
const std::string FILE_SCHEME_PREFIX = "file://";
const std::string FILE_MANAGER_AUTHORITY = "docs";
const std::string MEDIA_AUTHORITY = "media";
const std::string NETWORK_PARA = "?networkid=";
const std::string STORAGE_USERS = "storage/Users/";
const std::string EL1_APPDATA = "/appdata/el1/";
const std::string EL2_APPDATA = "/appdata/el2/";
const std::string APP_EL1_SANDBOX_HEAD = "/data/storage/el1/base";
const std::string APP_EL2_SANDBOX_HEAD = "/data/storage/el2/base";
const std::string DEFAULT_USERNAME = "currentUser";
const char *g_fileManagerFullMountEnableParameter = "const.filemanager.full_mout.enable";
bool CheckFileManagerFullMountEnable()
{
    char value[] = "false";
    int retSystem = GetParameter(g_fileManagerFullMountEnableParameter, "false", value, sizeof(value));
    if (retSystem > 0 && !std::strcmp(value, "true")) {
        return true;
    }
    LOGD("Not supporting all mounts");
    return false;
}
bool checkInvalidUri(const string &bundleName)
{
    if (bundleName == MEDIA_AUTHORITY) {
        return true;
    }
    if (bundleName == CommonFunc::GetSelfBundleName()) {
        return true;
    }
    return false;
}
static string GetUserName()
{
    std::string userName;
    ErrCode errCode = OHOS::AccountSA::OsAccountManager::GetOsAccountShortName(userName);
    if (errCode != ERR_OK || userName.empty()) {
        LOGD("Get userName Failed");
        userName = DEFAULT_USERNAME;
    }
    return userName;
}
static string GetPathFromShareUri(string &realPath, std::string &bundleName)
{
    if(bundleName == FILE_MANAGER_AUTHORITY){
        return  realPath;
    }
    string securityLevel = EL1_APPDATA;
    string sandboxHead = APP_EL1_SANDBOX_HEAD;
    if (realPath.find(APP_EL1_SANDBOX_HEAD) == string::npos) {
        securityLevel = EL2_APPDATA;
        sandboxHead = APP_EL2_SANDBOX_HEAD;
    }
    size_t posBegin = realPath.find(sandboxHead) + sandboxHead.size();
    string fileLowerPath = realPath.substr(posBegin, realPath.size() - 1);
    string userName = GetUserName();
    return STORAGE_USERS + userName + securityLevel + bundleName + fileLowerPath;
}
string FileUri::GetName()
{
    string sandboxPath = SandboxHelper::Decode(uri_.GetPath());
    size_t posLast = sandboxPath.find_last_of("/");
    if (posLast == string::npos) {
        return "";
    }

    if (posLast == sandboxPath.size()) {
        return "";
    }

    return sandboxPath.substr(posLast + 1);
}

string FileUri::GetPath()
{
    string sandboxPath = SandboxHelper::Decode(uri_.GetPath());
    string bundleName = uri_.GetAuthority();
    if (bundleName == MEDIA_AUTHORITY && sandboxPath.find(".") != string::npos) {
        size_t pos = sandboxPath.rfind("/");
        return sandboxPath.substr(0, pos);
    }

    return sandboxPath;
}

string FileUri::GetRealPath()
{
    string sandboxPath = SandboxHelper::Decode(uri_.GetPath());
    string realPath = sandboxPath;
    string bundleName = uri_.GetAuthority();
    if (CheckFileManagerFullMountEnable() && !checkInvalidUri(bundleName)) {
        return GetPathFromShareUri(realPath, bundleName);
    }
    if (bundleName == FILE_MANAGER_AUTHORITY && uri_.ToString().find(NETWORK_PARA) == string::npos &&
        access(realPath.c_str(), F_OK) == 0) {
        return realPath;
    }

    if ((bundleName != "") && (bundleName != CommonFunc::GetSelfBundleName())) {
        realPath = PATH_SHARE + MODE_RW + bundleName + sandboxPath;
        if (access(realPath.c_str(), F_OK) != 0) {
            realPath = PATH_SHARE + MODE_R + bundleName + sandboxPath;
        }
    }
    return realPath;
}

string FileUri::ToString()
{
    return uri_.ToString();
}

string FileUri::GetFullDirectoryUri()
{
    string uri = uri_.ToString();
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

FileUri::FileUri(const string &uriOrPath): uri_(
    (uriOrPath.find(FILE_SCHEME_PREFIX) == 0) ? uriOrPath : CommonFunc::GetUriFromPath(uriOrPath)
)
{}
}
}  // namespace AppFileService
}  // namespace OHOS