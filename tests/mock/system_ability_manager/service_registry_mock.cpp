/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "iservice_registry.h"

#include <cstddef>
#include <future>
#include <gtest/gtest.h>

#include "system_ability_definition.h"
#include "system_ability_manager_proxy.h"
#include "utils_mock_global_variable.h"

namespace OHOS {
using namespace FileManagement::Backup;

SystemAbilityManagerClient &SystemAbilityManagerClient::GetInstance()
{
    static auto instance = new SystemAbilityManagerClient();
    return *instance;
}

sptr<ISystemAbilityManager> SystemAbilityManagerClient::GetSystemAbilityManager()
{
    GTEST_LOG_(INFO) << "GetSystemAbilityManager is ok";
    std::lock_guard<std::mutex> lock(systemAbilityManagerLock_);
    if (systemAbilityManager_ != nullptr) {
        return systemAbilityManager_;
    }

    systemAbilityManager_ = new SystemAbilityManagerProxy(nullptr);
    return systemAbilityManager_;
}

int32_t SystemAbilityManagerProxy::LoadSystemAbility(int32_t systemAbilityId,
                                                     const sptr<ISystemAbilityLoadCallback> &callback)
{
    GTEST_LOG_(INFO) << "LoadSystemAbility is ok";
    if (!GetMockLoadSystemAbility()) {
        return -1;
    }
    return ERR_OK;
}
} // namespace OHOS