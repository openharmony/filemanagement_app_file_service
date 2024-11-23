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

#include "fileshare_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "file_permission.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
constexpr size_t LENGTH = 4;
constexpr size_t REMAINDER = 3;
using namespace OHOS::Security::AccessToken;

uint32_t ConvertToUint32(const uint8_t *ptr, size_t size)
{
    if (ptr == nullptr || (size < sizeof(uint32_t))) {
        return 0;
    }
    return *(reinterpret_cast<const uint32_t *>(ptr));
}

void GetInfo(const uint8_t *data, size_t size, std::vector<OHOS::AppFileService::UriPolicyInfo> &info)
{
    OHOS::AppFileService::UriPolicyInfo policyInfo;
    size_t lenth = size - LENGTH;
    std::string uri(reinterpret_cast<const char *>(data), lenth);
    policyInfo.uri = uri;
    policyInfo.mode = ConvertToUint32(data + lenth, LENGTH) % REMAINDER;
    info.push_back(policyInfo);
}

void GrantNativePermission()
{
    const char **perms = new const char *[1];
    perms[0] = "ohos.permission.FILE_ACCESS_PERSIST";
    TokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "app_file_service",
        .aplStr = "system_core",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
}

void PersistPermissionFuzzTest(const uint8_t *data, size_t size)
{
    if (size < LENGTH) {
        return;
    }
    std::deque<OHOS::AppFileService::PolicyErrorResult> result;
    std::vector<OHOS::AppFileService::UriPolicyInfo> info;
    GetInfo(data, size, info);
    GrantNativePermission();
    OHOS::AppFileService::FilePermission::PersistPermission(info, result);
}

void RevokePermissionFuzzTest(const uint8_t *data, size_t size)
{
    if (size < LENGTH) {
        return;
    }
    std::deque<OHOS::AppFileService::PolicyErrorResult> result;
    std::vector<OHOS::AppFileService::UriPolicyInfo> info;
    GetInfo(data, size, info);
    GrantNativePermission();
    OHOS::AppFileService::FilePermission::RevokePermission(info, result);
}

void ActivatePermissionFuzzTest(const uint8_t *data, size_t size)
{
    if (size < LENGTH) {
        return;
    }
    std::deque<OHOS::AppFileService::PolicyErrorResult> result;
    std::vector<OHOS::AppFileService::UriPolicyInfo> info;
    GetInfo(data, size, info);
    GrantNativePermission();
    OHOS::AppFileService::FilePermission::ActivatePermission(info, result);
}

void DeactivatePermissionFuzzTest(const uint8_t *data, size_t size)
{
    if (size < LENGTH) {
        return;
    }
    std::deque<OHOS::AppFileService::PolicyErrorResult> result;
    std::vector<OHOS::AppFileService::UriPolicyInfo> info;
    GetInfo(data, size, info);
    GrantNativePermission();
    OHOS::AppFileService::FilePermission::DeactivatePermission(info, result);
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::PersistPermissionFuzzTest(data, size);
    OHOS::RevokePermissionFuzzTest(data, size);
    OHOS::ActivatePermissionFuzzTest(data, size);
    OHOS::DeactivatePermissionFuzzTest(data, size);
    return 0;
}
}
