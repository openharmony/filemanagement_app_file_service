/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "service_reverse_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceReverse final : public ServiceReverseStub {
public:
    void BackupOnFileReady(std::string bundleName, std::string fileName, int fd) override;
    void BackupOnBundleStarted(int32_t errCode, std::string bundleName) override;
    void BackupOnBundleFinished(int32_t errCode, std::string bundleName) override;
    void BackupOnAllBundlesFinished(int32_t errCode) override;

    void RestoreOnBundleStarted(int32_t errCode, std::string bundleName) override;
    void RestoreOnBundleFinished(int32_t errCode, std::string bundleName) override;
    void RestoreOnAllBundlesFinished(int32_t errCode) override;
    void RestoreOnFileReady(std::string bundleName, std::string fileName, int fd) override;

public:
    ServiceReverse() = delete;
    explicit ServiceReverse(BSessionRestore::Callbacks callbacks);
    explicit ServiceReverse(BSessionBackup::Callbacks callbacks);
    ~ServiceReverse() override = default;

private:
    Scenario scenario_ {Scenario::UNDEFINED};
    BSessionBackup::Callbacks callbacksBackup_;
    BSessionRestore::Callbacks callbacksRestore_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_H