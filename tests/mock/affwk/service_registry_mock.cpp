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

#include <gtest/gtest.h>

#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"
#include "system_ability_manager_proxy.h"

namespace OHOS {
static sptr<AppExecFwk::BundleMgrStub> g_appMgrService = nullptr;

SystemAbilityManagerClient &SystemAbilityManagerClient::GetInstance()
{
    static auto instance = new SystemAbilityManagerClient();
    return *instance;
}

sptr<IRemoteObject> SystemAbilityManagerProxy::GetSystemAbility(int32_t systemAbilityId)
{
    sptr<IRemoteObject> remote = nullptr;
    switch (systemAbilityId) {
        case BUNDLE_MGR_SERVICE_SYS_ABILITY_ID:
            if (!g_appMgrService) {
                GTEST_LOG_(INFO) << "GetSystemAbility(" << systemAbilityId << "): return Mock";
                g_appMgrService = new AppExecFwk::BundleMgrStub();
            }
            remote = g_appMgrService;
            break;
        default:
            GTEST_LOG_(INFO) << "This service is not dummy!!!!" << systemAbilityId;
            break;
    }
    return remote;
}

sptr<ISystemAbilityManager> SystemAbilityManagerClient::GetSystemAbilityManager()
{
    std::lock_guard<std::mutex> lock(systemAbilityManagerLock_);
    if (systemAbilityManager_ != nullptr) {
        return systemAbilityManager_;
    }

    systemAbilityManager_ = new SystemAbilityManagerProxy(nullptr);
    return systemAbilityManager_;
}

sptr<IRemoteObject> SystemAbilityManagerProxy::GetSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    return GetSystemAbility(systemAbilityId);
}
} // namespace OHOS