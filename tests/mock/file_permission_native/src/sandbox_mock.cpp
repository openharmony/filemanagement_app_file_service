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
#include "sandbox_mock.h"

namespace OHOS {
using namespace AppFileService;
using namespace OHOS::AccessControl::SandboxManager;
int32_t SandboxManagerKit::CheckPersistPolicy(uint32_t tokenId,
    const std::vector<PolicyInfo> &policy, std::vector<bool> &result)
{
    return SandboxManagerKitMock::sandboxManagerKitMock->CheckPersistPolicy(tokenId, policy, result);
}

int32_t SandboxManagerKit::CheckPolicy(uint32_t tokenId,
    const std::vector<PolicyInfo> &policy, std::vector<bool> &result)
{
    return SandboxManagerKitMock::sandboxManagerKitMock->CheckPolicy(tokenId, policy, result);
}

int32_t SandboxManagerKit::PersistPolicy(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result)
{
    return SandboxManagerKitMock::sandboxManagerKitMock->PersistPolicy(policy, result);
}

int32_t SandboxManagerKit::UnPersistPolicy(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result)
{
    return SandboxManagerKitMock::sandboxManagerKitMock->UnPersistPolicy(policy, result);
}

int32_t SandboxManagerKit::StartAccessingPolicy(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result)
{
    return SandboxManagerKitMock::sandboxManagerKitMock->StartAccessingPolicy(policy, result);
}

int32_t SandboxManagerKit::StopAccessingPolicy(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result)
{
    return SandboxManagerKitMock::sandboxManagerKitMock->StopAccessingPolicy(policy, result);
}

int32_t SandboxManagerKit::UnPersistPolicy(uint32_t tokenId)
{
    return SandboxManagerKitMock::sandboxManagerKitMock->UnPersistPolicyByTokenId(tokenId);
}

int32_t SandboxManagerKit::UnPersistPolicy(
    uint32_t tokenId, const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result)
{
    return SandboxManagerKitMock::sandboxManagerKitMock->UnPersistPolicyByTokenIdAndPolicies(tokenId, policy, result);
}

int32_t SandboxManagerKit::GetPersistPolicy(uint32_t tokenId, std::vector<PolicyInfo> &policy)
{
    return SandboxManagerKitMock::sandboxManagerKitMock->GetPersistPolicyByTokenId(tokenId, policy);
}

int32_t SandboxManagerKit::SetPolicyByBundleName(const std::string &bundleName, int32_t appCloneIndex,
    const std::vector<PolicyInfo> &policy, uint64_t policyFlag, std::vector<uint32_t> &result)
{
    return SandboxManagerKitMock::sandboxManagerKitMock->SetPolicyByBundleName(
        bundleName, appCloneIndex, policy, policyFlag, result);
}

int32_t SandboxManagerKit::GrantSharedDirectoryPermission()
{
    return SandboxManagerKitMock::sandboxManagerKitMock->GrantSharedDirectoryPermission();
}

int32_t SandboxManagerKit::RevokeSharedDirectoryPermission()
{
    return SandboxManagerKitMock::sandboxManagerKitMock->RevokeSharedDirectoryPermission();
}

int32_t SandboxManagerKit::GetSharedDirectoryInfo(std::vector<SharedDirectoryInfo> &result)
{
    return SandboxManagerKitMock::sandboxManagerKitMock->GetSharedDirectoryInfo(result);
}
}
#endif
