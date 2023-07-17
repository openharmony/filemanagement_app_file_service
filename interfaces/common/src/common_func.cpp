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

#include "bundle_info.h"
#include "bundle_mgr_proxy.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "uri.h"
#include "system_ability_definition.h"

#include "log.h"
#include "json_utils.h"

using namespace std;

namespace OHOS {
namespace AppFileService {
using namespace OHOS::AppExecFwk;
namespace {
    const string PACKAGE_NAME_FLAG = "<PackageName>";
    const string CURRENT_USER_ID_FLAG = "<currentUserId>";
    const string PHYSICAL_PATH_KEY = "src-path";
    const string SANDBOX_PATH_KEY = "sandbox-path";
    const string MOUNT_PATH_MAP_KEY = "mount-path-map";
    const string SANDBOX_JSON_FILE_PATH = "/etc/app_file_service/file_share_sandbox.json";
    const std::string FILE_SCHEME_PREFIX = "file://";
    const char BACKFLASH = '/';
    const std::vector<std::string> PUBLIC_DIR_PATHS = {
        "/Documents"
    };
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

static sptr<BundleMgrProxy> GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        LOGE("fail to get system ability mgr.");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        LOGE("fail to get bundle manager proxy.");
        return nullptr;
    }

    return iface_cast<BundleMgrProxy>(remoteObject);
}

string CommonFunc::GetSelfBundleName()
{
    int uid = -1;
    uid = IPCSkeleton::GetCallingUid();

    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        LOGE("GetSelfBundleName: bundle mgr proxy is nullptr.");
        return "";
    }

    BundleInfo bundleInfo;
    auto ret = bundleMgrProxy->GetBundleInfoForSelf(uid, bundleInfo);
    if (ret != ERR_OK) {
        LOGE("GetSelfBundleName: bundleName get fail. uid is %{public}d", uid);
        return "";
    }

    return bundleInfo.name;
}

bool CommonFunc::CheckPublicDirPath(const std::string &sandboxPath)
{
    for (const std::string &path : PUBLIC_DIR_PATHS) {
        if (sandboxPath.find(path) == 0) {
            return true;
        }
    }
    return false;
}

static bool NormalizePath(string &path)
{
    if (path.size() <= 0) {
        return false;
    }

    if (path[0] != BACKFLASH) {
        path.insert(0, 1, BACKFLASH);
    }

    return true;
}

string CommonFunc::GetUriFromPath(const string &path)
{
    string realPath = path;
    if (!realPath.empty() && !NormalizePath(realPath)) {
        LOGE("GetUriFromPath::NormalizePath failed!");
        return "";
    }

    string packageName = GetSelfBundleName();
    realPath = FILE_SCHEME_PREFIX + packageName + realPath;
    return realPath;
}
} // namespace AppFileService
} // namespace OHOS

