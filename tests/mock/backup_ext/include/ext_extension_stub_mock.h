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

#ifndef OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_STUB_MOCK_H
#define OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_STUB_MOCK_H

#include <gmock/gmock.h>

#include "ext_extension_stub.h"

namespace OHOS::FileManagement::Backup {
class ExtExtensionStubMock : public ExtExtensionStub {
public:
    MOCK_METHOD(int, OnRemoteRequest, (uint32_t, MessageParcel &, MessageParcel &, MessageOption &));
    MOCK_METHOD(ErrCode, CmdGetFileHandle, (MessageParcel &, MessageParcel &));
    MOCK_METHOD(ErrCode, CmdHandleClear, (MessageParcel &, MessageParcel &));
    MOCK_METHOD(ErrCode, CmdHandleBackup, (MessageParcel &, MessageParcel &));
    MOCK_METHOD(ErrCode, CmdPublishFile, (MessageParcel &, MessageParcel &));
    MOCK_METHOD(ErrCode, CmdHandleRestore, (MessageParcel &, MessageParcel &));
    MOCK_METHOD(ErrCode, CmdGetIncrementalFileHandle, (MessageParcel &, MessageParcel &));
    MOCK_METHOD(ErrCode, CmdPublishIncrementalFile, (MessageParcel &, MessageParcel &));
    MOCK_METHOD(ErrCode, CmdHandleIncrementalBackup, (MessageParcel &, MessageParcel &));
    MOCK_METHOD(ErrCode, CmdIncrementalOnBackup, (MessageParcel &, MessageParcel &));
    MOCK_METHOD(ErrCode, CmdGetIncrementalBackupFileHandle, (MessageParcel &, MessageParcel &));
    MOCK_METHOD(ErrCode, CmdGetBackupInfo, (MessageParcel &, MessageParcel &));
    MOCK_METHOD(ErrCode, CmdUpdateFdSendRate, (MessageParcel &, MessageParcel &));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_STUB_MOCK_H
