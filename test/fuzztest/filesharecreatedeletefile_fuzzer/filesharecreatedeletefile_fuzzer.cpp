/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "filesharecreatedeletefile_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "file_share.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
using namespace AppFileService;
using namespace Security::AccessToken;

constexpr size_t THRESHOLD = 8;
constexpr size_t URI_PREFIX_LEN = 7;
constexpr size_t MAX_URI_COUNT = 5;
constexpr size_t PERM_NUM = 1;

uint32_t ConvertToUint32(const uint8_t *ptr, size_t size)
{
    if (ptr == nullptr || (size < sizeof(uint32_t))) {
        return 0;
    }
    return *(reinterpret_cast<const uint32_t *>(ptr));
}

void GrantNativePermission()
{
    const char **perms = new const char *[PERM_NUM];
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

bool FileShareCreateShareFileFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < THRESHOLD) {
        return false;
    }

    GrantNativePermission();

    uint32_t tokenId = ConvertToUint32(data, sizeof(uint32_t));
    uint32_t flag = ConvertToUint32(data + sizeof(uint32_t), sizeof(uint32_t));

    std::vector<std::string> uriList;
    if (size > THRESHOLD) {
        size_t uriDataLen = size - THRESHOLD;
        std::string uriData(reinterpret_cast<const char*>(data + THRESHOLD), uriDataLen);
        std::string uri = "file://" + uriData;
        uriList.push_back(uri);
    }

    std::vector<int32_t> retList;
    FileShare::CreateShareFile(uriList, tokenId, flag, retList);
    return true;
}

bool FileShareCreateShareFileMultiUriFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < THRESHOLD + URI_PREFIX_LEN) {
        return false;
    }

    GrantNativePermission();

    uint32_t tokenId = ConvertToUint32(data, sizeof(uint32_t));
    uint32_t flag = ConvertToUint32(data + sizeof(uint32_t), sizeof(uint32_t));

    std::vector<std::string> uriList;
    size_t remaining = size - THRESHOLD;
    size_t uriCount = (data[0] % MAX_URI_COUNT) + 1;
    size_t uriLen = remaining / uriCount;

    for (size_t i = 0; i < uriCount && uriLen > 0; i++) {
        size_t offset = THRESHOLD + i * uriLen;
        size_t len = (i == uriCount - 1) ? (size - offset) : uriLen;
        std::string uriData(reinterpret_cast<const char*>(data + offset), len);
        std::string uri = "file://" + uriData;
        uriList.push_back(uri);
    }

    std::vector<int32_t> retList;
    FileShare::CreateShareFile(uriList, tokenId, flag, retList);
    return true;
}

bool FileShareDeleteShareFileFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(uint32_t)) {
        return false;
    }

    GrantNativePermission();

    uint32_t tokenId = ConvertToUint32(data, sizeof(uint32_t));

    std::vector<std::string> uriList;
    if (size > sizeof(uint32_t)) {
        size_t uriDataLen = size - sizeof(uint32_t);
        std::string uriData(reinterpret_cast<const char*>(data + sizeof(uint32_t)), uriDataLen);
        std::string uri = "file://" + uriData;
        uriList.push_back(uri);
    }

    FileShare::DeleteShareFile(tokenId, uriList);
    return true;
}

bool FileShareDeleteShareFileMultiUriFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(uint32_t) + URI_PREFIX_LEN) {
        return false;
    }

    GrantNativePermission();

    uint32_t tokenId = ConvertToUint32(data, sizeof(uint32_t));

    std::vector<std::string> uriList;
    size_t remaining = size - sizeof(uint32_t);
    size_t uriCount = (data[0] % MAX_URI_COUNT) + 1;
    size_t uriLen = remaining / uriCount;

    for (size_t i = 0; i < uriCount && uriLen > 0; i++) {
        size_t offset = sizeof(uint32_t) + i * uriLen;
        size_t len = (i == uriCount - 1) ? (size - offset) : uriLen;
        std::string uriData(reinterpret_cast<const char*>(data + offset), len);
        std::string uri = "file://" + uriData;
        uriList.push_back(uri);
    }

    FileShare::DeleteShareFile(tokenId, uriList);
    return true;
}

bool FileShareCreateAndDeleteFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < THRESHOLD) {
        return false;
    }

    GrantNativePermission();

    uint32_t tokenId = ConvertToUint32(data, sizeof(uint32_t));
    uint32_t flag = ConvertToUint32(data + sizeof(uint32_t), sizeof(uint32_t));

    std::vector<std::string> uriList;
    if (size > THRESHOLD) {
        size_t uriDataLen = size - THRESHOLD;
        std::string uriData(reinterpret_cast<const char*>(data + THRESHOLD), uriDataLen);
        std::string uri = "file://" + uriData;
        uriList.push_back(uri);
    }

    std::vector<int32_t> retList;
    FileShare::CreateShareFile(uriList, tokenId, flag, retList);
    FileShare::DeleteShareFile(tokenId, uriList);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::FileShareCreateShareFileFuzzTest(data, size);
    OHOS::FileShareCreateShareFileMultiUriFuzzTest(data, size);
    OHOS::FileShareDeleteShareFileFuzzTest(data, size);
    OHOS::FileShareDeleteShareFileMultiUriFuzzTest(data, size);
    OHOS::FileShareCreateAndDeleteFuzzTest(data, size);
    return 0;
}
