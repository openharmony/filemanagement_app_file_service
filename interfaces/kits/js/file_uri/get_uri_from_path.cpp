/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "get_uri_from_path.h"

#include "bundle_mgr_proxy.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "log.h"
#include "status_receiver_host.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileUri {
using namespace OHOS::FileManagement::LibN;
using namespace OHOS::AppExecFwk;

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

static string GetBundleName()
{
    int uid = -1;
    uid = IPCSkeleton::GetCallingUid();

    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        LOGE("GetBundleName: bundle mgr proxy is nullptr.");
        return nullptr;
    }

    string bundleName;
    if (!bundleMgrProxy->GetBundleNameForUid(uid, bundleName)) {
        LOGE("GetBundleName: bundleName get fail. uid is %{public}d", uid);
        return nullptr;
    }

    return bundleName;
}

napi_value GetUriFromPath::Sync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("GetUriFromPath::Sync Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto [succPath, path, ignore] = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succPath) {
        LOGE("GetUriFromPath::Sync get path parameter failed!");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    string packageName = GetBundleName();
    string uri = SCHEME + SCHEME_SEPARATOR + PATH_SYMBOLS + packageName + path.get();
    return NVal::CreateUTF8String(env, uri).val_;
}

} // namespace ModuleFileUri
} // namespace AppFileService
} // namespace OHOS