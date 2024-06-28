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

#include "sandbox_mock.h"

using namespace OHOS::AccessControl::SandboxManager;
int32_t CheckPersistPolicy(uint64_t tokenId, const std::vector<PolicyInfo> &policy, std::vector<bool> &result)
{
    return SandboxManagerKit::sandboxManagerKit->CheckPersistPolicy(tokenId, policy, result);
}

int32_t PersistPolicy(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result)
{
    return SandboxManagerKit::sandboxManagerKit->PersistPolicy(policy, result);
}

int32_t UnPersistPolicy(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result)
{
    return SandboxManagerKit::sandboxManagerKit->UnPersistPolicy(policy, result);
}

int32_t StartAccessingPolicy(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result)
{
    return SandboxManagerKit::sandboxManagerKit->StartAccessingPolicy(policy, result);
}

int32_t StopAccessingPolicy(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result)
{
    return SandboxManagerKit::sandboxManagerKit->StopAccessingPolicy(policy, result);
}