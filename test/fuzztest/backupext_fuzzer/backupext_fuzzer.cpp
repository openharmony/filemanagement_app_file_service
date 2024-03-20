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
#include "backupext_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "message_parcel.h"
#include "ext_extension_stub.h"
#include "ext_extension.h"
#include "ext_backup.h"
#include "securec.h"

using namespace OHOS::FileManagement::Backup;

namespace OHOS {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr uint8_t MAX_CALL_TRANSACTION = 10;
constexpr size_t U32_AT_SIZE = 4;

std::shared_ptr<ExtBackup> extBackup = std::make_shared<ExtBackup>();
std::shared_ptr<BackupExtExtension> backupExtExtensionPtr =
    std::make_shared<BackupExtExtension>(extBackup);

uint32_t GetU32Data(const char* ptr)
{
    // 将第0个数字左移24位，将第1个数字左移16位，将第2个数字左移8位，第3个数字不左移
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
}

bool BackupExtFuzzTest(std::unique_ptr<char[]> data, size_t size)
{
    uint32_t code = GetU32Data(data.get());
    MessageParcel datas;
    datas.WriteInterfaceToken(ExtExtensionStub::GetDescriptor());
    datas.WriteBuffer(data.get(), size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    backupExtExtensionPtr->OnRemoteRequest(code % MAX_CALL_TRANSACTION, datas, reply, option);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    /* Validate the length of size */
    if (size < OHOS::U32_AT_SIZE || size > OHOS::FOO_MAX_LEN) {
        return 0;
    }

    auto str = std::make_unique<char[]>(size + 1);
    (void)memset_s(str.get(), size + 1, 0x00, size + 1);
    if (memcpy_s(str.get(), size, data, size) != EOK) {
        return 0;
    }
    OHOS::BackupExtFuzzTest(move(str), size);
    return 0;
}

