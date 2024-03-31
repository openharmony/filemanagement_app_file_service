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

#ifndef OHOS_FILEMGMT_BACKUP_I_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_I_SERVICE_H

#include <string>
#include <tuple>

#include <unique_fd.h>

#include "b_file_info.h"
#include "b_incremental_data.h"
#include "i_service_ipc_interface_code.h"
#include "i_service_reverse.h"
#include "iremote_broker.h"

namespace OHOS::FileManagement::Backup {
const int DEFAULT_INVAL_VALUE = -1;

typedef enum TypeRestoreTypeEnum {
    RESTORE_DATA_WAIT_SEND = 0,
    RESTORE_DATA_READDY = 1,
} RestoreTypeEnum;

class IService : public IRemoteBroker {
public:
    virtual ErrCode InitRestoreSession(sptr<IServiceReverse> remote) = 0;
    virtual ErrCode InitBackupSession(sptr<IServiceReverse> remote) = 0;
    virtual ErrCode Start() = 0;
    virtual UniqueFd GetLocalCapabilities() = 0;
    virtual ErrCode PublishFile(const BFileInfo &fileInfo) = 0;
    virtual ErrCode AppFileReady(const std::string &fileName, UniqueFd fd) = 0;
    virtual ErrCode AppDone(ErrCode errCode) = 0;
    virtual ErrCode ServiceResultReport(const std::string &restoreRetInfo) = 0;
    virtual ErrCode GetFileHandle(const std::string &bundleName, const std::string &fileName) = 0;
    virtual ErrCode AppendBundlesRestoreSession(UniqueFd fd,
                                                const std::vector<BundleName> &bundleNames,
                                                const std::vector<std::string> &detailInfos,
                                                RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND,
                                                int32_t userId = DEFAULT_INVAL_VALUE) = 0;
    virtual ErrCode AppendBundlesBackupSession(const std::vector<BundleName> &bundleNames) = 0;
    virtual ErrCode Finish() = 0;
    virtual ErrCode Release() = 0;
    virtual UniqueFd GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames) = 0;
    virtual ErrCode InitIncrementalBackupSession(sptr<IServiceReverse> remote) = 0;
    virtual ErrCode AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup) = 0;

    virtual ErrCode PublishIncrementalFile(const BFileInfo &fileInfo) = 0;
    virtual ErrCode AppIncrementalFileReady(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd) = 0;
    virtual ErrCode AppIncrementalDone(ErrCode errCode) = 0;
    virtual ErrCode GetIncrementalFileHandle(const std::string &bundleName, const std::string &fileName) = 0;
    virtual ErrCode GetBackupInfo(BundleName &bundleName, std::string &result) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Filemanagement.Backup.IService")
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_I_SERVICE_H