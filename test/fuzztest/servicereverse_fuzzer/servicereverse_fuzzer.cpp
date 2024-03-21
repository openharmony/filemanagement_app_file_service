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

#include "servicereverse_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <climits>
#include "message_parcel.h"
#include "b_session_backup.h"
#include "b_session_restore.h"
#include "b_incremental_backup_session.h"
#include "b_incremental_restore_session.h"
#include "i_service_reverse_ipc_interface_code.h"
#include "service_reverse_stub.h"
#include "service_reverse.h"
#include "securec.h"
#include "system_ability.h"

using namespace OHOS::FileManagement::Backup;

namespace OHOS {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
constexpr uint8_t MAX_CALL_TRANSACTION = 24;
constexpr int32_t SHIFT_FIRST = 24;
constexpr int32_t SHIFT_SECOND = 16;
constexpr int32_t SHIFT_THIRD = 8;
constexpr int32_t ZERO_NUM = 0;
constexpr int32_t FIRST_NUM = 1;
constexpr int32_t SECOND_NUM = 2;
constexpr int32_t THIRD_NUM = 3;

uint32_t ConvertToUint32(const uint8_t* ptr)
{
    if (ptr == nullptr) {
        return 0;
    }
    return (ptr[ZERO_NUM] << SHIFT_FIRST) | (ptr[FIRST_NUM] << SHIFT_SECOND) |
        (ptr[SECOND_NUM] << SHIFT_THIRD) | (ptr[THIRD_NUM]);
}

bool BackupFuzzTest(const uint8_t *data, size_t size)
{
    uint32_t code = ConvertToUint32(data);
    if (code > static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_BACKUP_ON_TASK_FINISHED)) {
        return true;
    }

    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceReverseStub::GetDescriptor());
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    BSessionBackup::Callbacks callbacks;
    std::shared_ptr<ServiceReverse> backupPtr =
        std::make_shared<ServiceReverse>(callbacks);
    backupPtr->OnRemoteRequest(code % MAX_CALL_TRANSACTION, datas, reply, option);
    return true;
}

bool RestoreFuzzTest(const uint8_t *data, size_t size)
{
    uint32_t code = ConvertToUint32(data);
    if (code < static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_SUB_TASK_STARTED) ||
        code > static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_FILE_READY)) {
        return true;
    }

    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceReverseStub::GetDescriptor());
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    BSessionRestore::Callbacks callbacks;
    std::shared_ptr<ServiceReverse> restorePtr =
        std::make_shared<ServiceReverse>(callbacks);
    restorePtr->OnRemoteRequest(code % MAX_CALL_TRANSACTION, datas, reply, option);
    return true;
}

bool IncrementalBackupFuzzTest(const uint8_t *data, size_t size)
{
    uint32_t code = ConvertToUint32(data);
    if (code < static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_BACKUP_ON_FILE_READY) ||
        code > static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_BACKUP_ON_TASK_FINISHED)) {
        return true;
    }

    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceReverseStub::GetDescriptor());
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    BIncrementalBackupSession::Callbacks callbacks;
    std::shared_ptr<ServiceReverse> backupPtr =
        std::make_shared<ServiceReverse>(callbacks);
    backupPtr->OnRemoteRequest(code % MAX_CALL_TRANSACTION, datas, reply, option);
    return true;
}

bool IncrementalRestoreFuzzTest(const uint8_t *data, size_t size)
{
    uint32_t code = ConvertToUint32(data);
    if (code < static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_RESTORE_ON_SUB_TASK_STARTED) ||
        code > static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_RESTORE_ON_FILE_READY)) {
        return true;
    }

    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceReverseStub::GetDescriptor());
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    BIncrementalRestoreSession::Callbacks callbacks;
    std::shared_ptr<ServiceReverse> restorePtr =
        std::make_shared<ServiceReverse>(callbacks);
    restorePtr->OnRemoteRequest(code % MAX_CALL_TRANSACTION, datas, reply, option);
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
    OHOS::BackupFuzzTest(data, size);
    OHOS::RestoreFuzzTest(data, size);
    OHOS::IncrementalBackupFuzzTest(data, size);
    OHOS::IncrementalRestoreFuzzTest(data, size);
    return 0;
}