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

#include "backuponbundlestarted_fuzzer.h"

#include <string>
 
#include "b_error/b_error.h"
#include "b_session_backup.h"
#include "b_session_restore.h"
#include "b_incremental_backup_session.h"
#include "b_incremental_restore_session.h"
#include "filemgmt_libhilog.h"
#include "message_parcel.h"
#include "service_reverse_stub.h"
#include "service_reverse.h"
 
namespace OHOS {
using namespace std;
using namespace FileManagement::Backup;

template<class T>
T TypeCast(const uint8_t *data, int *pos = nullptr)
{
    if (pos) {
        *pos += sizeof(T);
    }
    return *(reinterpret_cast<const T*>(data));
}

bool BackupOnBundleStartedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceReverseIpcCode::COMMAND_BACKUP_ON_BUNDLE_STARTED);

    try {
        int pos = 0;
        int32_t errCode = TypeCast<int32_t>(data, &pos);
        msg.WriteInt32(errCode);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::FileManagement::Backup::BSessionBackup::Callbacks backupCallbacks;
    OHOS::sptr backupService(new OHOS::FileManagement::Backup::ServiceReverse(backupCallbacks));
    if (!backupService) {
        HILOGE("service handler is nullptr");
        return 0;
    }

    try {
        OHOS::BackupOnBundleStartedFuzzTest(backupService, data, size);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        HILOGE("BackupOnBundleStartedFuzzTest error");
    } catch (...) {
        HILOGE("BackupOnBundleStartedFuzzTest exception");
    }
    return 0;
}