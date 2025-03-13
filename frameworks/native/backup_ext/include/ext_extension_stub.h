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

#ifndef OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_STUB_H
#define OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_STUB_H

#include <map>

#include "iextension.h"
#include "iremote_stub.h"

namespace OHOS::FileManagement::Backup {
class ExtExtensionStub : public IRemoteStub<IExtension> {
public:
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

public:
    ExtExtensionStub();
    ~ExtExtensionStub() = default;

private:
    ErrCode CmdGetFileHandle(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdHandleClear(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdHandleBackup(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdPublishFile(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdHandleRestore(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdGetIncrementalFileHandle(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdPublishIncrementalFile(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdHandleIncrementalBackup(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdIncrementalOnBackup(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdGetIncrementalBackupFileHandle(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdGetBackupInfo(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdUpdateFdSendRate(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdHandleUser0Backup(MessageParcel &data, MessageParcel &reply);

private:
    using ExtensionInterface = int32_t (ExtExtensionStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, ExtensionInterface> opToInterfaceMap_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_STUB_H