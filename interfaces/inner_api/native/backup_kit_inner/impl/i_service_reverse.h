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

#ifndef OHOS_FILEMGMT_BACKUP_I_SERVICE_REVERSE_H
#define OHOS_FILEMGMT_BACKUP_I_SERVICE_REVERSE_H

#include <cstdint>

#include "iremote_broker.h"

namespace OHOS::FileManagement::Backup {
class IServiceReverse : public IRemoteBroker {
public:
    enum class Scenario {
        UNDEFINED,
        BACKUP,
        RESTORE,
    };

    enum {
        SERVICER_BACKUP_ON_FILE_READY,
        SERVICER_BACKUP_ON_SUB_TASK_STARTED,
        SERVICER_BACKUP_ON_SUB_TASK_FINISHED,
        SERVICER_BACKUP_ON_TASK_FINISHED,

        SERVICER_RESTORE_ON_SUB_TASK_STARTED,
        SERVICER_RESTORE_ON_SUB_TASK_FINISHED,
        SERVICER_RESTORE_ON_TASK_FINISHED,
        SERVICER_RESTORE_ON_FILE_READY,
    };

public:
    virtual void BackupOnFileReady(std::string bundleName, std::string fileName, int fd) = 0;
    virtual void BackupOnBundleStarted(int32_t errCode, std::string bundleName) = 0;
    virtual void BackupOnBundleFinished(int32_t errCode, std::string bundleName) = 0;
    virtual void BackupOnAllBundlesFinished(int32_t errCode) = 0;

    virtual void RestoreOnBundleStarted(int32_t errCode, std::string bundleName) = 0;
    virtual void RestoreOnBundleFinished(int32_t errCode, std::string bundleName) = 0;
    virtual void RestoreOnAllBundlesFinished(int32_t errCode) = 0;
    virtual void RestoreOnFileReady(std::string bundleName, std::string fileName, int fd) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.FileManagement.Backup.IServiceReverse")
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_I_SERVICE_REVERSE_H