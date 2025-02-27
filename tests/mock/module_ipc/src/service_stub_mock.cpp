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

#include "service_stub.h"
#include "service_stub_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ServiceStub::ServiceStub()
{
}

int32_t ServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string localDescriptor = GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (localDescriptor != remoteDescriptor) {
        return ERR_TRANSACTION_FAILED;
    }
    switch (static_cast<IServiceReverseIpcCode>(code)) {
        case IServiceReverseIpcCode::COMMAND_BACKUP_ON_BUNDLE_STARTED: {
            int32_t backupOnBundleStartedErrCode = data.ReadInt32();
            std::string bundleName = Str16ToStr8(data.ReadString16());
            ErrCode errCode = BackupOnBundleStarted(backupOnBundleStartedErrCode, bundleName);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_BACKUP_ON_FILE_READY: {
            std::string bundleName = Str16ToStr8(data.ReadString16());
            std::string fileName = Str16ToStr8(data.ReadString16());
            int fd = data.ReadFileDescriptor();
            int32_t backupOnFileReadyErrCode = data.ReadInt32();
            ErrCode errCode = BackupOnFileReady(bundleName, fileName, fd, backupOnFileReadyErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_BACKUP_ON_RESULT_REPORT: {
            std::string myResult = Str16ToStr8(data.ReadString16());
            std::string bundleName = Str16ToStr8(data.ReadString16());
            ErrCode errCode = BackupOnResultReport(myResult, bundleName);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_BACKUP_ON_BUNDLE_FINISHED: {
            int32_t backupOnBundleFinishedErrCode = data.ReadInt32();
            std::string bundleName = Str16ToStr8(data.ReadString16());
            ErrCode errCode = BackupOnBundleFinished(backupOnBundleFinishedErrCode, bundleName);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_BACKUP_ON_ALL_BUNDLES_FINISHED: {
            int32_t backupOnAllBundlesFinishedErrCode = data.ReadInt32();
            ErrCode errCode = BackupOnAllBundlesFinished(backupOnAllBundlesFinishedErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_BACKUP_ON_PROCESS_INFO: {
            std::string bundleName = Str16ToStr8(data.ReadString16());
            std::string processInfo = Str16ToStr8(data.ReadString16());
            ErrCode errCode = BackupOnProcessInfo(bundleName, processInfo);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_RESTORE_ON_BUNDLE_STARTED: {
            int32_t restoreOnBundleStartedErrCode = data.ReadInt32();
            std::string bundleName = Str16ToStr8(data.ReadString16());
            ErrCode errCode = RestoreOnBundleStarted(restoreOnBundleStartedErrCode, bundleName);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_RESTORE_ON_FILE_READY: {
            std::string bundleName = Str16ToStr8(data.ReadString16());
            std::string fileName = Str16ToStr8(data.ReadString16());
            int fd = data.ReadFileDescriptor();
            int32_t restoreOnFileReadyErrCode = data.ReadInt32();
            ErrCode errCode = RestoreOnFileReady(bundleName, fileName, fd, restoreOnFileReadyErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_RESTORE_ON_RESULT_REPORT: {
            std::string myResult = Str16ToStr8(data.ReadString16());
            std::string bundleName = Str16ToStr8(data.ReadString16());
            int32_t restoreOnResultReportErrCode = data.ReadInt32();
            ErrCode errCode = RestoreOnResultReport(myResult, bundleName, restoreOnResultReportErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_RESTORE_ON_BUNDLE_FINISHED: {
            int32_t restoreOnBundleFinishedErrCode = data.ReadInt32();
            std::string bundleName = Str16ToStr8(data.ReadString16());
            ErrCode errCode = RestoreOnBundleFinished(restoreOnBundleFinishedErrCode, bundleName);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_RESTORE_ON_ALL_BUNDLES_FINISHED: {
            int32_t restoreOnAllBundlesFinishedErrCode = data.ReadInt32();
            ErrCode errCode = RestoreOnAllBundlesFinished(restoreOnAllBundlesFinishedErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_RESTORE_ON_PROCESS_INFO: {
            std::string bundleName = Str16ToStr8(data.ReadString16());
            std::string processInfo = Str16ToStr8(data.ReadString16());
            ErrCode errCode = RestoreOnProcessInfo(bundleName, processInfo);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_INCREMENTAL_BACKUP_ON_BUNDLE_STARTED: {
            int32_t incrementalBackupOnBundleStartedErrCode = data.ReadInt32();
            std::string bundleName = Str16ToStr8(data.ReadString16());
            ErrCode errCode = IncrementalBackupOnBundleStarted(incrementalBackupOnBundleStartedErrCode, bundleName);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_INCREMENTAL_BACKUP_ON_FILE_READY: {
            std::string bundleName = Str16ToStr8(data.ReadString16());
            std::string fileName = Str16ToStr8(data.ReadString16());
            int fd = data.ReadFileDescriptor();
            int manifestFd = data.ReadFileDescriptor();
            int32_t incrementalBackupOnFileReadyErrCode = data.ReadInt32();
            ErrCode errCode =
                IncrementalBackupOnFileReady(bundleName, fileName, fd, manifestFd, incrementalBackupOnFileReadyErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_INCREMENTAL_BACKUP_ON_RESULT_REPORT: {
            std::string myResult = Str16ToStr8(data.ReadString16());
            std::string bundleName = Str16ToStr8(data.ReadString16());
            ErrCode errCode = IncrementalBackupOnResultReport(myResult, bundleName);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_INCREMENTAL_BACKUP_ON_BUNDLE_FINISHED: {
            int32_t incrementalBackupOnBundleFinishedErrCode = data.ReadInt32();
            std::string bundleName = Str16ToStr8(data.ReadString16());
            ErrCode errCode = IncrementalBackupOnBundleFinished(incrementalBackupOnBundleFinishedErrCode, bundleName);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_INCREMENTAL_BACKUP_ON_ALL_BUNDLES_FINISHED: {
            int32_t incrementalBackupOnAllBundlesFinishedErrCode = data.ReadInt32();
            ErrCode errCode = IncrementalBackupOnAllBundlesFinished(incrementalBackupOnAllBundlesFinishedErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_INCREMENTAL_BACKUP_ON_PROCESS_INFO: {
            std::string bundleName = Str16ToStr8(data.ReadString16());
            std::string processInfo = Str16ToStr8(data.ReadString16());
            ErrCode errCode = IncrementalBackupOnProcessInfo(bundleName, processInfo);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_INCREMENTAL_RESTORE_ON_BUNDLE_STARTED: {
            int32_t incrementalRestoreOnBundleStartedErrCode = data.ReadInt32();
            std::string bundleName = Str16ToStr8(data.ReadString16());
            ErrCode errCode = IncrementalRestoreOnBundleStarted(incrementalRestoreOnBundleStartedErrCode, bundleName);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_INCREMENTAL_RESTORE_ON_FILE_READY: {
            std::string bundleName = Str16ToStr8(data.ReadString16());
            std::string fileName = Str16ToStr8(data.ReadString16());
            int fd = data.ReadFileDescriptor();
            int manifestFd = data.ReadFileDescriptor();
            int32_t incrementalRestoreOnFileReadyErrCode = data.ReadInt32();
            ErrCode errCode = IncrementalRestoreOnFileReady(bundleName, fileName, fd, manifestFd,
                                                            incrementalRestoreOnFileReadyErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_INCREMENTAL_RESTORE_ON_RESULT_REPORT: {
            std::string myResult = Str16ToStr8(data.ReadString16());
            std::string bundleName = Str16ToStr8(data.ReadString16());
            int32_t incrementalRestoreOnResultReportErrCode = data.ReadInt32();
            ErrCode errCode =
                IncrementalRestoreOnResultReport(myResult, bundleName, incrementalRestoreOnResultReportErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_INCREMENTAL_RESTORE_ON_BUNDLE_FINISHED: {
            int32_t incrementalRestoreOnBundleFinishedErrCode = data.ReadInt32();
            std::string bundleName = Str16ToStr8(data.ReadString16());
            ErrCode errCode = IncrementalRestoreOnBundleFinished(incrementalRestoreOnBundleFinishedErrCode, bundleName);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_INCREMENTAL_RESTORE_ON_ALL_BUNDLES_FINISHED: {
            int32_t incrementalRestoreOnAllBundlesFinishedErrCode = data.ReadInt32();
            ErrCode errCode = IncrementalRestoreOnAllBundlesFinished(incrementalRestoreOnAllBundlesFinishedErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceReverseIpcCode::COMMAND_INCREMENTAL_RESTORE_ON_PROCESS_INFO: {
            std::string bundleName = Str16ToStr8(data.ReadString16());
            std::string processInfo = Str16ToStr8(data.ReadString16());
            ErrCode errCode = IncrementalRestoreOnProcessInfo(bundleName, processInfo);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return ERR_TRANSACTION_FAILED;
}

} // namespace OHOS::FileManagement::Backup
