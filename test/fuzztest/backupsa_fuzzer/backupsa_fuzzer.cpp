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

#include "backupsa_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <climits>
#include "message_parcel.h"
#include "service_stub.h"
#include "service.h"
#include "securec.h"
#include "system_ability.h"

#include "filemgmt_libhilog.h"

using namespace OHOS::FileManagement::Backup;

namespace OHOS {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
constexpr uint8_t MAX_CALL_TRANSACTION = 16;
constexpr int32_t SERVICE_ID = 5203;

uint32_t GetU32Data(const char* ptr)
{
    // 将第0个数字左移24位，将第1个数字左移16位，将第2个数字左移8位，第3个数字不左移
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
}

bool BackupSaFuzzTest(std::unique_ptr<char[]> data, size_t size)
{
    sptr service = sptr(new Service(SERVICE_ID));
    uint32_t code = GetU32Data(data.get());
    if (code == 0) {
        return true;
    }
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    datas.WriteBuffer(data.get(), size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    service->OnRemoteRequest(code % MAX_CALL_TRANSACTION, datas, reply, option);
    service = nullptr;
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t dataSize)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    /* Validate the length of size */
    if (dataSize < OHOS::U32_AT_SIZE || dataSize > OHOS::FOO_MAX_LEN) {
        return 0;
    }

    auto str = std::make_unique<char[]>(dataSize + 1);
    (void)memset_s(str.get(), dataSize + 1, 0x00, dataSize + 1);
    if (memcpy_s(str.get(), dataSize, data, dataSize) != EOK) {
        return 0;
    }
    OHOS::BackupSaFuzzTest(move(str), dataSize);
    return 0;
}