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
constexpr size_t U32_AT_SIZE = 4;
constexpr uint8_t MAX_CALL_TRANSACTION = 24;
constexpr int32_t SERVICE_ID = 5203;

uint32_t GetU32Data(const char* ptr)
{
    // 将第0个数字左移24位，将第1个数字左移16位，将第2个数字左移8位，第3个数字不左移
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
}

bool BackupSaFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < U32_AT_SIZE) {
        return true;
    }

    sptr service = sptr(new Service(SERVICE_ID));
    uint32_t code = GetU32Data(reinterpret_cast<const char*>(data));
    if (code == 0) {
        return true;
    }
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    datas.WriteBuffer(reinterpret_cast<const char*>(data + U32_AT_SIZE), size - U32_AT_SIZE);
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
    OHOS::BackupSaFuzzTest(data, dataSize);
    return 0;
}