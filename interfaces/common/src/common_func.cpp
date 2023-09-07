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

#include "bundle_mgr_proxy.h"
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
    const char BACKFLASH = '/';
    const std::string FILE_MANAGER_URI_HEAD = "/storage/";
    const std::string FILE_MANAGER_AUTHORITY = "docs";
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

static void NormalizePath(string &path)
{
    if (path.size() == 0) {
        return;
    }

    if (path[0] != BACKFLASH) {
        path.insert(0, 1, BACKFLASH);
    }
}

string CommonFunc::GetUriFromPath(const string &path)
{
    string realPath = path;
    NormalizePath(realPath);

    string packageName = (path.find(FILE_MANAGER_URI_HEAD) == 0) ? FILE_MANAGER_AUTHORITY : GetSelfBundleName();
    realPath = FILE_SCHEME_PREFIX + packageName + realPath;
    return SandboxHelper::Encode(realPath);
}
} // namespace AppFileService
} // namespace OHOS

