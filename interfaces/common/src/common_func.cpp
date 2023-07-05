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

#include "common_func.h"

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
}
std::unordered_map<std::string, string> CommonFunc::sandboxPathMap_;

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

int32_t CommonFunc::GetPhysicalPath(const std::string &fileUri, const std::string &userId,
                                    std::string &physicalPath)
{
    Uri uri(fileUri);
    string bundleName = uri.GetAuthority();
    string sandboxPath = uri.GetPath();
    
    string lowerPathTail = "";
    string lowerPathHead = "";

    if (sandboxPathMap_.size() == 0) {
        GetSandboxPathMap(sandboxPathMap_);
    }

    for (auto it = sandboxPathMap_.begin(); it != sandboxPathMap_.end(); it++) {
        string sandboxPathPrefix = it->first;
        if (sandboxPath.length() >= sandboxPathPrefix.length()) {
            string sandboxPathTemp = sandboxPath.substr(0, sandboxPathPrefix.length());
            if (sandboxPathTemp == sandboxPathPrefix) {
                lowerPathHead = it->second;
                lowerPathTail = sandboxPath.substr(sandboxPathPrefix.length());
                break;
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

bool CommonFunc::CheckValidPath(const std::string &filePath)
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

