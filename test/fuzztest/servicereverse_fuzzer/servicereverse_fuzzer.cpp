/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include <string>

#include "b_error/b_error.h"
#include "b_session_backup.h"
#include "b_session_restore.h"
#include "b_incremental_backup_session.h"
#include "b_incremental_restore_session.h"
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

bool BackupOnFileReadyFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(bool) + sizeof(int)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int fd = TypeCast<int>(data, &pos);
        bool fdFlag = TypeCast<bool>(data + pos, &pos);
        int len = ((size - pos) >> 1);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos + len), size - pos - len));
        msg.WriteBool(fdFlag);
        if (fdFlag) {
            msg.WriteFileDescriptor(fd);
        }
        service->CmdBackupOnFileReady(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool BackupOnBundleStartedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int32_t errCode = TypeCast<int32_t>(data, &pos);
        msg.WriteInt32(errCode);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        service->CmdBackupOnBundleStarted(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool BackupOnResultReportFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int len = (size >> 1);
        msg.WriteString(string(reinterpret_cast<const char*>(data), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + len), size - len));
        service->CmdBackupOnResultReport(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool BackupOnBundleFinishedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int32_t errCode = TypeCast<int32_t>(data, &pos);
        msg.WriteInt32(errCode);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        service->CmdBackupOnBundleFinished(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool BackupOnAllBundlesFinishedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int32_t errCode = TypeCast<int32_t>(data);
        msg.WriteInt32(errCode);
        service->CmdBackupOnAllBundlesFinished(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool BackupOnProcessInfoFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int len = (size >> 1);
        msg.WriteString(string(reinterpret_cast<const char*>(data), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + len), size - len));
        service->CmdBackupOnProcessInfo(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool BackupOnScanningInfoFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        msg.WriteString(string(reinterpret_cast<const char*>(data), size));
        service->CmdBackupOnScanningInfo(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool RestoreOnBundleStartedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int32_t errCode = TypeCast<int32_t>(data, &pos);
        msg.WriteInt32(errCode);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        service->CmdRestoreOnBundleStarted(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool RestoreOnBundleFinishedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int32_t errCode = TypeCast<int32_t>(data, &pos);
        msg.WriteInt32(errCode);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        service->CmdRestoreOnBundleFinished(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool RestoreOnAllBundlesFinishedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int32_t errCode = TypeCast<int32_t>(data);
        msg.WriteInt32(errCode);
        service->CmdRestoreOnAllBundlesFinished(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool RestoreOnFileReadyFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(bool) + sizeof(int) + sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int fd = TypeCast<int>(data, &pos);
        bool fdFlag = TypeCast<bool>(data + pos, &pos);
        int32_t errCode = TypeCast<int32_t>(data + pos, &pos);
        int len = ((size - pos) >> 1);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos + len), size - pos - len));
        msg.WriteBool(fdFlag);
        if (fdFlag) {
            msg.WriteFileDescriptor(fd);
        }
        msg.WriteInt32(errCode);
        service->CmdRestoreOnFileReady(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool RestoreOnResultReportFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int32_t errCode = TypeCast<int32_t>(data, &pos);
        int len = ((size - pos) >> 1);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos + len), size - pos - len));
        msg.WriteInt32(errCode);
        service->CmdRestoreOnResultReport(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool RestoreOnProcessInfoFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int len = (size >> 1);
        msg.WriteString(string(reinterpret_cast<const char*>(data), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + len), size - len));
        service->CmdRestoreOnProcessInfo(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalBackupOnFileReadyFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(bool) + sizeof(int) + sizeof(int) + sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int fd = TypeCast<int>(data, &pos);
        int manifestFd = TypeCast<int>(data + pos, &pos);
        bool fdFlag = TypeCast<bool>(data + pos, &pos);
        int32_t errCode = TypeCast<int32_t>(data + pos, &pos);
        int len = ((size - pos) >> 1);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos + len), size - pos - len));
        msg.WriteBool(fdFlag);
        if (fdFlag) {
            msg.WriteFileDescriptor(fd);
            msg.WriteFileDescriptor(manifestFd);
        }
        msg.WriteInt32(errCode);
        service->CmdIncrementalBackupOnFileReady(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalBackupOnBundleStartedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int32_t errCode = TypeCast<int32_t>(data, &pos);
        msg.WriteInt32(errCode);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        service->CmdIncrementalBackupOnBundleStarted(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalBackupOnResultReportFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int len = (size >> 1);
        msg.WriteString(string(reinterpret_cast<const char*>(data), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + len), size - len));
        service->CmdIncrementalBackupOnResultReport(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalBackupOnBundleFinishedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int32_t errCode = TypeCast<int32_t>(data, &pos);
        msg.WriteInt32(errCode);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        service->CmdIncrementalBackupOnBundleFinished(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalBackupOnAllBundlesFinishedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int32_t errCode = TypeCast<int32_t>(data);
        msg.WriteInt32(errCode);
        service->CmdIncrementalBackupOnAllBundlesFinished(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalBackupOnProcessInfoFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int len = (size >> 1);
        msg.WriteString(string(reinterpret_cast<const char*>(data), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + len), size - len));
        service->CmdIncrementalBackupOnProcessInfo(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalBackupOnScanningInfoFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        msg.WriteString(string(reinterpret_cast<const char*>(data), size));
        service->CmdIncrementalBackupOnScanningInfo(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalRestoreOnBundleStartedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int32_t errCode = TypeCast<int32_t>(data, &pos);
        msg.WriteInt32(errCode);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        service->CmdIncrementalRestoreOnBundleStarted(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalRestoreOnBundleFinishedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int32_t errCode = TypeCast<int32_t>(data, &pos);
        msg.WriteInt32(errCode);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        service->CmdIncrementalRestoreOnBundleFinished(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalRestoreOnAllBundlesFinishedFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int32_t errCode = TypeCast<int32_t>(data);
        msg.WriteInt32(errCode);
        service->CmdIncrementalRestoreOnAllBundlesFinished(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalRestoreOnFileReadyFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(bool) + sizeof(int) + sizeof(int) + sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int fd = TypeCast<int>(data, &pos);
        int manifestFd = TypeCast<int>(data + pos, &pos);
        bool fdFlag = TypeCast<bool>(data + pos, &pos);
        int32_t errCode = TypeCast<int32_t>(data + pos, &pos);
        int len = ((size - pos) >> 1);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos + len), size - pos - len));
        msg.WriteBool(fdFlag);
        if (fdFlag) {
            msg.WriteFileDescriptor(fd);
            msg.WriteFileDescriptor(manifestFd);
        }
        msg.WriteInt32(errCode);
        service->CmdIncrementalRestoreOnFileReady(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalRestoreOnResultReportFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int32_t errCode = TypeCast<int32_t>(data, &pos);
        int len = ((size - pos) >> 1);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos + len), size - pos - len));
        msg.WriteInt32(errCode);
        service->CmdIncrementalRestoreOnResultReport(msg, reply);
    } catch (BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool IncrementalRestoreOnProcessInfoFuzzTest(sptr<ServiceReverse> service, const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int len = (size >> 1);
        msg.WriteString(string(reinterpret_cast<const char*>(data), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + len), size - len));
        service->CmdIncrementalRestoreOnProcessInfo(msg, reply);
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
    if (backupService == nullptr) {
        return 0;
    }
    OHOS::FileManagement::Backup::BSessionRestore::Callbacks restoreCallbacks;
    OHOS::sptr restoreService(new OHOS::FileManagement::Backup::ServiceReverse(restoreCallbacks));
    if (restoreService == nullptr) {
        return 0;
    }
    OHOS::FileManagement::Backup::BIncrementalBackupSession::Callbacks incrementalBackupCallbacks;
    OHOS::sptr incrementalBackupService(new OHOS::FileManagement::Backup::ServiceReverse(incrementalBackupCallbacks));
    if (incrementalBackupService == nullptr) {
        return 0;
    }
    OHOS::FileManagement::Backup::BIncrementalRestoreSession::Callbacks incrementalRestoreCallbacks;
    OHOS::sptr incrementalRestoreService(new OHOS::FileManagement::Backup::ServiceReverse(incrementalRestoreCallbacks));
    if (incrementalRestoreService == nullptr) {
        return 0;
    }

    OHOS::BackupOnFileReadyFuzzTest(backupService, data, size);
    OHOS::BackupOnBundleStartedFuzzTest(backupService, data, size);
    OHOS::BackupOnResultReportFuzzTest(backupService, data, size);
    OHOS::BackupOnBundleFinishedFuzzTest(backupService, data, size);
    OHOS::BackupOnAllBundlesFinishedFuzzTest(backupService, data, size);
    OHOS::BackupOnProcessInfoFuzzTest(backupService, data, size);
    OHOS::BackupOnScanningInfoFuzzTest(backupService, data, size);

    OHOS::RestoreOnBundleStartedFuzzTest(restoreService, data, size);
    OHOS::RestoreOnBundleFinishedFuzzTest(restoreService, data, size);
    OHOS::RestoreOnAllBundlesFinishedFuzzTest(restoreService, data, size);
    OHOS::RestoreOnFileReadyFuzzTest(restoreService, data, size);
    OHOS::RestoreOnResultReportFuzzTest(restoreService, data, size);
    OHOS::RestoreOnProcessInfoFuzzTest(restoreService, data, size);

    OHOS::IncrementalBackupOnFileReadyFuzzTest(incrementalBackupService, data, size);
    OHOS::IncrementalBackupOnBundleStartedFuzzTest(incrementalBackupService, data, size);
    OHOS::IncrementalBackupOnResultReportFuzzTest(incrementalBackupService, data, size);
    OHOS::IncrementalBackupOnBundleFinishedFuzzTest(incrementalBackupService, data, size);
    OHOS::IncrementalBackupOnAllBundlesFinishedFuzzTest(incrementalBackupService, data, size);
    OHOS::IncrementalBackupOnProcessInfoFuzzTest(incrementalBackupService, data, size);
    OHOS::IncrementalBackupOnScanningInfoFuzzTest(incrementalBackupService, data, size);

    OHOS::IncrementalRestoreOnBundleStartedFuzzTest(incrementalRestoreService, data, size);
    OHOS::IncrementalRestoreOnBundleFinishedFuzzTest(incrementalRestoreService, data, size);
    OHOS::IncrementalRestoreOnAllBundlesFinishedFuzzTest(incrementalRestoreService, data, size);
    OHOS::IncrementalRestoreOnFileReadyFuzzTest(incrementalRestoreService, data, size);
    OHOS::IncrementalRestoreOnResultReportFuzzTest(incrementalRestoreService, data, size);
    OHOS::IncrementalRestoreOnProcessInfoFuzzTest(incrementalRestoreService, data, size);
    return 0;
}