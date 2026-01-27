/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
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

#include <mutex>
#include <vector>

#include <singleton.h>
#include "bundle_mgr_client.h"
#include "bundle_mgr_interface.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "log.h"
#include "sandbox_helper.h"

using namespace std;

namespace OHOS {
namespace AppFileService {
using namespace OHOS::AppExecFwk;
namespace {
const std::string FILE_SCHEME_PREFIX = "file://";
const char BACKSLASH = '/';
const std::string FILE_MANAGER_URI_HEAD = "/storage/";
const std::string FILE_HAP_URI_HEAD = "/data/storage";
const std::string FILE_MANAGER_AUTHORITY = "docs";
const std::string FILE_APP_DATA = "appdata";
const std::string FILE_BASE = "base/";
const std::string FILE_USERS = "Users";
const std::string FILE_MNT_DATA = "/mnt/data/fuse";
const std::string DLP_PACKAGE_NAME = "com.ohos.dlpmanager";
const std::string MEDIA_FUSE_PATH_HEAD = "/data/storage/el2/media";
const std::string MEDIA_AUTHORITY = "file://media";
std::string g_bundleName = "";
std::mutex g_globalMutex;
} // namespace

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

static sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy()
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

    return iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
}

bool CommonFunc::GetDirByBundleNameAndAppIndex(const std::string &bundleName, int32_t appIndex, std::string &dirName)
{
    auto bmsClient = DelayedSingleton<AppExecFwk::BundleMgrClient>::GetInstance();
    if (bmsClient == nullptr) {
        LOGE("bundleMgrClient is nullptr.");
        return false;
    }
    auto bmsRet = bmsClient->GetDirByBundleNameAndAppIndex(bundleName, appIndex, dirName);
    if (bmsRet != ERR_OK) {
        LOGE("GetDirByBundleNameAndAppIndex failed, ret:%{public}d", bmsRet);
        return false;
    }
    return true;
}

string CommonFunc::GetSelfBundleName()
{
    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        LOGE("GetSelfBundleName: bundle mgr proxy is nullptr.");
        return "";
    }

    BundleInfo bundleInfo;
    auto ret = bundleMgrProxy->GetBundleInfoForSelf(0, bundleInfo);
    if (ret != ERR_OK) {
        LOGE("GetSelfBundleName: bundleName get fail.");
        return "";
    }
    string dirName = "" ;
    if (GetDirByBundleNameAndAppIndex(bundleInfo.name, bundleInfo.appIndex, dirName)) {
        return dirName;
    }
    return bundleInfo.name;
}

static void NormalizePath(string &path)
{
    if (path.size() == 0) {
        return;
    }

    if (path[0] != BACKSLASH) {
        path.insert(0, 1, BACKSLASH);
    }
}

static bool GetResultIsUri(string &path)
{
    string realPath = path;
    if (realPath.find(FILE_MANAGER_URI_HEAD) == 0) {
        realPath = realPath.substr(FILE_MANAGER_URI_HEAD.size());
        string pathType = GetNextDirName(realPath);
        if (pathType != FILE_USERS) {
            return false;
        }
        string userName = GetNextDirName(realPath);
        if (GetNextDirName(realPath) == FILE_APP_DATA) {
            string encryptionLevel = GetNextDirName(realPath);
            string dataPart = GetNextDirName(realPath);
            string bundleName = GetNextDirName(realPath);
            if (bundleName.empty()) {
                return false;
            }
            path = FILE_SCHEME_PREFIX + bundleName + FILE_HAP_URI_HEAD + BACKSLASH + encryptionLevel + BACKSLASH +
                   dataPart + SandboxHelper::Encode(realPath);
            return true;
        }
    }
    return false;
}

string CommonFunc::GetUriFromPath(const string &path)
{
    if (!SandboxHelper::IsValidPath(path)) {
        LOGE("path is ValidPath, The path contains '../' characters");
        return "";
    }
    if (path.find(FILE_SCHEME_PREFIX) == 0) {
        return path;
    }
    string realPath = path;
    NormalizePath(realPath);
    if (realPath.find(MEDIA_FUSE_PATH_HEAD) == 0) {
        realPath = SandboxHelper::Encode(realPath);
        return realPath.replace(realPath.find(MEDIA_FUSE_PATH_HEAD), MEDIA_FUSE_PATH_HEAD.length(), MEDIA_AUTHORITY);
    }
    string packageName;
    if (GetResultIsUri(realPath)) {
        return realPath;
    }
    if (realPath.find(FILE_MNT_DATA) == 0) {
        return FILE_SCHEME_PREFIX + DLP_PACKAGE_NAME + SandboxHelper::Encode(realPath);
    }
    {
        std::lock_guard<std::mutex> lock(g_globalMutex);
        if (g_bundleName == "") {
            g_bundleName = GetSelfBundleName();
        }
        packageName = (realPath.find(FILE_MANAGER_URI_HEAD) == 0) ? FILE_MANAGER_AUTHORITY : g_bundleName;
    }
    realPath = FILE_SCHEME_PREFIX + packageName + SandboxHelper::Encode(realPath);
    return realPath;
}

bool CommonFunc::EndsWith(const std::string &str, const std::string &suffix)
{
    if (suffix.length() > str.length()) {
        return false;
    }
    return (str.rfind(suffix) == (str.length() - suffix.length()));
}
} // namespace AppFileService
} // namespace OHOS
