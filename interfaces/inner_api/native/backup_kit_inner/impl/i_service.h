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

#ifndef OHOS_FILEMGMT_BACKUP_I_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_I_SERVICE_H

#include <string>
#include <tuple>

#include <unique_fd.h>

#include "b_file_info.h"
#include "i_service_reverse.h"
#include "iremote_broker.h"

namespace OHOS::FileManagement::Backup {
class IService : public IRemoteBroker {
public:
    enum {
        SERVICE_CMD_INIT_RESTORE_SESSION,
        SERVICE_CMD_INIT_BACKUP_SESSION,
        SERVICE_CMD_GET_LOCAL_CAPABILITIES,
        SERVICE_CMD_PUBLISH_FILE,
        SERVICE_CMD_APP_FILE_READY,
        SERVICE_CMD_APP_DONE,
        SERVICE_CMD_START,
        SERVICE_CMD_GET_EXT_FILE_NAME,
        SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION,
        SERVICE_CMD_APPEND_BUNDLES_BACKUP_SESSION,
        SERVICE_CMD_FINISH,
    };

    virtual ErrCode InitRestoreSession(sptr<IServiceReverse> remote, const std::vector<BundleName> &bundleNames) = 0;
    virtual ErrCode InitBackupSession(sptr<IServiceReverse> remote,
                                      UniqueFd fd,
                                      const std::vector<BundleName> &bundleNames) = 0;
    virtual ErrCode Start() = 0;
    virtual UniqueFd GetLocalCapabilities() = 0;
    virtual ErrCode PublishFile(const BFileInfo &fileInfo) = 0;
    virtual ErrCode AppFileReady(const std::string &fileName, UniqueFd fd) = 0;
    virtual ErrCode AppDone(ErrCode errCode) = 0;
    virtual ErrCode GetExtFileName(std::string &bundleName, std::string &fileName) = 0;
    virtual ErrCode AppendBundlesRestoreSession(UniqueFd fd, const std::vector<BundleName> &bundleNames) = 0;
    virtual ErrCode AppendBundlesBackupSession(const std::vector<BundleName> &bundleNames) = 0;
    virtual ErrCode Finish() = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Filemanagement.Backup.IService")
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_I_SERVICE_H