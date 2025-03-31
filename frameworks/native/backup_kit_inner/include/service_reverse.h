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
const int DEFAULT_RESTORE_TYPE = 0;
const int DEFAULT_USER_ID = -1;

class ServiceReverse final : public ServiceReverseStub {
public:
    ErrCode BackupOnFileReady(const std::string &bundleName,
                              const std::string &fileName,
                              int fd,
                              int32_t errCode) override;
    ErrCode BackupOnBundleStarted(int32_t errCode, const std::string &bundleName) override;
    ErrCode BackupOnResultReport(const std::string &result, const std::string &bundleName) override;
    ErrCode BackupOnBundleFinished(int32_t errCode, const std::string &bundleName) override;
    ErrCode BackupOnAllBundlesFinished(int32_t errCode) override;
    ErrCode BackupOnProcessInfo(const std::string &bundleName, const std::string &processInfo) override;
    ErrCode BackupOnScanningInfo(const std::string &scannedInfo) override;

    ErrCode RestoreOnBundleStarted(int32_t errCode, const std::string &bundleName) override;
    ErrCode RestoreOnBundleFinished(int32_t errCode, const std::string &bundleName) override;
    ErrCode RestoreOnAllBundlesFinished(int32_t errCode) override;
    ErrCode RestoreOnFileReady(const std::string &bundleName,
                               const std::string &fileName,
                               int fd,
                               int32_t errCode) override;
    ErrCode RestoreOnResultReport(const std::string &result,
                                  const std::string &bundleName,
                                  ErrCode errCode = 0) override;
    ErrCode RestoreOnProcessInfo(const std::string &bundleName, const std::string &processInfo) override;

    ErrCode IncrementalBackupOnFileReady(const std::string &bundleName,
                                         const std::string &fileName,
                                         int fd,
                                         int manifestFd,
                                         int32_t errCode) override;
    ErrCode IncrementalBackupOnBundleStarted(int32_t errCode, const std::string &bundleName) override;
    ErrCode IncrementalBackupOnResultReport(const std::string &result, const std::string &bundleName) override;
    ErrCode IncrementalBackupOnBundleFinished(int32_t errCode, const std::string &bundleName) override;
    ErrCode IncrementalBackupOnAllBundlesFinished(int32_t errCode) override;
    ErrCode IncrementalBackupOnProcessInfo(const std::string &bundleName, const std::string &processInfo) override;
    ErrCode IncrementalBackupOnScanningInfo(const std::string &scannedInfo) override;

    ErrCode IncrementalRestoreOnBundleStarted(int32_t errCode, const std::string &bundleName) override;
    ErrCode IncrementalRestoreOnBundleFinished(int32_t errCode, const std::string &bundleName) override;
    ErrCode IncrementalRestoreOnAllBundlesFinished(int32_t errCode) override;
    ErrCode IncrementalRestoreOnFileReady(const std::string &bundleName,
                                          const std::string &fileName,
                                          int fd,
                                          int manifestFd,
                                          int32_t errCode) override;
    ErrCode IncrementalRestoreOnResultReport(const std::string &result,
                                             const std::string &bundleName,
                                             ErrCode errCode) override;
    ErrCode IncrementalRestoreOnProcessInfo(const std::string &bundleName, const std::string &processInfo) override;

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