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

#include "ext_extension_stub.h"

#include <cstdint>
#include <sstream>

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace BExcepUltils;

ExtExtensionStub::ExtExtensionStub()
{
    opToInterfaceMap_[CMD_GET_FILE_HANDLE] = &ExtExtensionStub::CmdGetFileHandle;
    opToInterfaceMap_[CMD_HANDLE_CLAER] = &ExtExtensionStub::CmdHandleClear;
    opToInterfaceMap_[CMD_HANDLE_BACKUP] = &ExtExtensionStub::CmdHandleBackup;
    opToInterfaceMap_[CMD_PUBLISH_FILE] = &ExtExtensionStub::CmdPublishFile;
}

int32_t ExtExtensionStub::OnRemoteRequest(uint32_t code,
                                          MessageParcel &data,
                                          MessageParcel &reply,
                                          MessageOption &option)
{
    const std::u16string descriptor = ExtExtensionStub::GetDescriptor();
    const std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return BError(BError::Codes::EXT_INVAL_ARG, "Invalid remote descriptor");
    }

    HILOGI("Begin to call procedure indexed %{public}u", code);
    auto interfaceIndex = opToInterfaceMap_.find(code);
    if (interfaceIndex == opToInterfaceMap_.end() || !interfaceIndex->second) {
        HILOGE("Cannot response request %{public}d : unknown procedure", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return ExceptionCatcherLocked([&]() { return ErrCode((this->*(interfaceIndex->second))(data, reply)); });
}

ErrCode ExtExtensionStub::CmdGetFileHandle(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("Begin");
    string fileName;
    if (!data.ReadString(fileName)) {
        return BError(BError::Codes::EXT_INVAL_ARG, "Failed to receive fileName").GetCode();
    }

    UniqueFd fd = GetFileHandle(fileName);
    if (!reply.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::EXT_BROKEN_IPC, "Failed to send out the file").GetCode();
    }
    return BError(BError::Codes::OK);
}

ErrCode ExtExtensionStub::CmdHandleClear(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("Begin");
    ErrCode res = HandleClear();
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::EXT_BROKEN_IPC, ss.str()).GetCode();
    }
    return BError(BError::Codes::OK);
}

ErrCode ExtExtensionStub::CmdHandleBackup(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("Begin");
    ErrCode res = HandleBackup();
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::EXT_BROKEN_IPC, ss.str()).GetCode();
    }
    return BError(BError::Codes::OK);
}

ErrCode ExtExtensionStub::CmdPublishFile(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("Begin");
    string fileName;
    if (!data.ReadString(fileName)) {
        return BError(BError::Codes::EXT_INVAL_ARG, "Failed to receive fileName");
    }

    ErrCode res = PublishFile(fileName);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::EXT_BROKEN_IPC, ss.str()).GetCode();
    }
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup