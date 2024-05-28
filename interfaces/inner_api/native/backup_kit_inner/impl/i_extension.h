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

#ifndef OHOS_FILEMGMT_BACKUP_I_EXTENSION_H
#define OHOS_FILEMGMT_BACKUP_I_EXTENSION_H

#include "errors.h"
#include "i_extension_ipc_interface_code.h"
#include "iremote_broker.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class IExtension : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.FileManagement.Backup.IExtension");

public:
    virtual ~IExtension() = default;
    virtual UniqueFd GetFileHandle(const std::string &fileName) = 0;
    virtual ErrCode HandleClear() = 0;
    virtual ErrCode HandleBackup() = 0;
    virtual ErrCode PublishFile(const std::string &fileName) = 0;
    virtual ErrCode HandleRestore() = 0;
    virtual ErrCode GetIncrementalFileHandle(const std::string &fileName) = 0;
    virtual ErrCode PublishIncrementalFile(const std::string &fileName) = 0;
    virtual ErrCode HandleIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd) = 0;
    virtual ErrCode IncrementalOnBackup() = 0;
    virtual std::tuple<UniqueFd, UniqueFd> GetIncrementalBackupFileHandle() = 0;
    virtual ErrCode GetBackupInfo(std::string &result) = 0;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_I_EXTENSION_H