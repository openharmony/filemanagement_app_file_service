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

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "uri.h"

#include "common_func.h"
#include "log.h"
#include "sandbox_helper.h"
#include "parameter.h"

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
const std::string BACKFLASH = "/";
const std::string FULL_MOUNT_ENABLE_PARAMETER = "const.filemanager.full_mount.enable";
static bool CheckFileManagerFullMountEnable()
{
    char value[] = "false";
    int retSystem = GetParameter(FULL_MOUNT_ENABLE_PARAMETER.c_str(), "false", value, sizeof(value));
    if (retSystem > 0 && !strcmp(value, "true")) {
        LOGD("The full mount enable parameter is true");
        return true;
    }
    LOGD("The full mount enable parameter is false");
    return false;
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
        if (pos == string::npos) {
            return "";
        }
        return sandboxPath.substr(0, pos);
    }

    return sandboxPath;
}

string FileUri::GetRealPath()
{
    string sandboxPath = SandboxHelper::Decode(uri_.GetPath());
    string realPath = sandboxPath;
    string bundleName = uri_.GetAuthority();
    if (bundleName == FILE_MANAGER_AUTHORITY &&
        uri_.ToString().find(NETWORK_PARA) == string::npos &&
        (access(realPath.c_str(), F_OK) == 0 || CheckFileManagerFullMountEnable())) {
        return realPath;
    }

    if (((bundleName != "") && (bundleName != CommonFunc::GetSelfBundleName())) ||
        uri_.ToString().find(NETWORK_PARA) != string::npos) {
        realPath = PATH_SHARE + MODE_RW + bundleName + sandboxPath;
        if (access(realPath.c_str(), F_OK) != 0) {
            realPath = PATH_SHARE + MODE_R + bundleName + sandboxPath;
        }
    }
    return realPath;
}

string FileUri::GetRealPathBySA(const std::string &targetBundleName)
{
    string sandboxPath = SandboxHelper::Decode(uri_.GetPath());
    string realPath = sandboxPath;
    string bundleName = uri_.GetAuthority();
    if (bundleName == FILE_MANAGER_AUTHORITY &&
        uri_.ToString().find(NETWORK_PARA) == string::npos &&
        (access(realPath.c_str(), F_OK) == 0 || CheckFileManagerFullMountEnable())) {
        return realPath;
    }
    realPath = PATH_SHARE + MODE_R + bundleName + sandboxPath;
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

bool FileUri::IsRemoteUri()
{
    size_t pos = uri_.ToString().find(NETWORK_PARA);
    if (pos != string::npos && pos > 0 && pos < uri_.ToString().size() - NETWORK_PARA.size()) {
        if (uri_.ToString().substr(pos + NETWORK_PARA.size()).find(BACKFLASH) == string::npos) {
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
    return true;
}

FileUri::FileUri(const string &uriOrPath): uri_(
    (uriOrPath.find(FILE_SCHEME_PREFIX) == 0) ? uriOrPath : CommonFunc::GetUriFromPath(uriOrPath)
)
{}
}
}  // namespace AppFileService
}  // namespace OHOS