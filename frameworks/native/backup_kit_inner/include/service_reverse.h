/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_H

#include "b_session_backup.h"
#include "b_session_restore.h"
#include "b_incremental_backup_session.h"
#include "b_incremental_restore_session.h"
#include "service_reverse_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceReverse final : public ServiceReverseStub {
public:
    void BackupOnFileReady(std::string bundleName, std::string fileName, int fd, int32_t errCode) override;
    void BackupOnBundleStarted(int32_t errCode, std::string bundleName) override;
    void BackupOnResultReport(std::string result, std::string bundleName) override;
    void BackupOnBundleFinished(int32_t errCode, std::string bundleName) override;
    void BackupOnAllBundlesFinished(int32_t errCode) override;
    void BackupOnProcessInfo(std::string bundleName, std::string processInfo) override;
    void BackupOnScanningInfo(std::string scannedInfo) override;

    void RestoreOnBundleStarted(int32_t errCode, std::string bundleName) override;
    void RestoreOnBundleFinished(int32_t errCode, std::string bundleName) override;
    void RestoreOnAllBundlesFinished(int32_t errCode) override;
    void RestoreOnFileReady(std::string bundleName, std::string fileName, int fd, int32_t errCode) override;
    void RestoreOnResultReport(std::string result, std::string bundleName,
        ErrCode errCode = 0) override;
    void RestoreOnProcessInfo(std::string bundleName, std::string processInfo) override;

    void IncrementalBackupOnFileReady(std::string bundleName, std::string fileName, int fd, int manifestFd,
        int32_t errCode) override;
    void IncrementalBackupOnBundleStarted(int32_t errCode, std::string bundleName) override;
    void IncrementalBackupOnResultReport(std::string result, std::string bundleName) override;
    void IncrementalBackupOnBundleFinished(int32_t errCode, std::string bundleName) override;
    void IncrementalBackupOnAllBundlesFinished(int32_t errCode) override;
    void IncrementalBackupOnProcessInfo(std::string bundleName, std::string processInfo) override;
    void IncrementalBackupOnScanningInfo(std::string scannedInfo) override;

    void IncrementalRestoreOnBundleStarted(int32_t errCode, std::string bundleName) override;
    void IncrementalRestoreOnBundleFinished(int32_t errCode, std::string bundleName) override;
    void IncrementalRestoreOnAllBundlesFinished(int32_t errCode) override;
    void IncrementalRestoreOnFileReady(std::string bundleName, std::string fileName, int fd, int manifestFd,
        int32_t errCode) override;
    void IncrementalRestoreOnResultReport(std::string result, std::string bundleName,
        ErrCode errCode = 0) override;
    void IncrementalRestoreOnProcessInfo(std::string bundleName, std::string processInfo) override;

public:
    ServiceReverse() = delete;
    explicit ServiceReverse(BSessionRestore::Callbacks callbacks);
    explicit ServiceReverse(BSessionBackup::Callbacks callbacks);
    explicit ServiceReverse(BIncrementalBackupSession::Callbacks callbacks);
    explicit ServiceReverse(BIncrementalRestoreSession::Callbacks callbacks);
    ~ServiceReverse() override = default;

private:
    Scenario scenario_ {Scenario::UNDEFINED};
    BSessionBackup::Callbacks callbacksBackup_;
    BSessionRestore::Callbacks callbacksRestore_;
    BIncrementalBackupSession::Callbacks callbacksIncrementalBackup_;
    BIncrementalRestoreSession::Callbacks callbacksIncrementalRestore_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_H