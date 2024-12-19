/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifdef SANDBOX_MANAGER
#include "upms_mock.h"

namespace OHOS {
using namespace AppFileService;
using namespace OHOS::AAFwk;
using namespace OHOS::AccessControl::SandboxManager;
UriPermissionManagerClient &UriPermissionManagerClient::GetInstance()
{
    static UriPermissionManagerClient instance;
    return instance;
}
int32_t UriPermissionManagerClient::Active(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result)
{
    if (UpmsManagerKitMock::upmsManagerKitMock == nullptr) {
        return -1;
    }
    return UpmsManagerKitMock::upmsManagerKitMock->Active(policy, result);
}

} // namespace OHOS
#endif