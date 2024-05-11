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

#include "ext_extension_stub.h"

#include <cstdint>
#include <sstream>

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ExtExtensionStub::ExtExtensionStub()
{
    opToInterfaceMap_[static_cast<uint32_t>(IExtensionInterfaceCode::CMD_GET_FILE_HANDLE)] =
        &ExtExtensionStub::CmdGetFileHandle;
    opToInterfaceMap_[static_cast<uint32_t>(IExtensionInterfaceCode::CMD_HANDLE_CLAER)] =
        &ExtExtensionStub::CmdHandleClear;
    opToInterfaceMap_[static_cast<uint32_t>(IExtensionInterfaceCode::CMD_HANDLE_BACKUP)] =
        &ExtExtensionStub::CmdHandleBackup;
    opToInterfaceMap_[static_cast<uint32_t>(IExtensionInterfaceCode::CMD_PUBLISH_FILE)] =
        &ExtExtensionStub::CmdPublishFile;
    opToInterfaceMap_[static_cast<uint32_t>(IExtensionInterfaceCode::CMD_HANDLE_RESTORE)] =
        &ExtExtensionStub::CmdHandleRestore;
    opToInterfaceMap_[static_cast<uint32_t>(IExtensionInterfaceCode::CMD_GET_INCREMENTAL_FILE_HANDLE)] =
        &ExtExtensionStub::CmdGetIncrementalFileHandle;
    opToInterfaceMap_[static_cast<uint32_t>(IExtensionInterfaceCode::CMD_PUBLISH_INCREMENTAL_FILE)] =
        &ExtExtensionStub::CmdPublishIncrementalFile;
    opToInterfaceMap_[static_cast<uint32_t>(IExtensionInterfaceCode::CMD_HANDLE_INCREMENTAL_BACKUP)] =
        &ExtExtensionStub::CmdHandleIncrementalBackup;
    opToInterfaceMap_[static_cast<uint32_t>(IExtensionInterfaceCode::CMD_GET_INCREMENTAL_BACKUP_FILE_HANDLE)] =
        &ExtExtensionStub::CmdGetIncrementalBackupFileHandle;
    opToInterfaceMap_[static_cast<uint32_t>(IExtensionInterfaceCode::CMD_GET_BACKUP_INFO)] =
        &ExtExtensionStub::CmdGetBackupInfo;
    opToInterfaceMap_[static_cast<uint32_t>(IExtensionInterfaceCode::CMD_INCREMENTAL_ON_BACKUP)] =
        &ExtExtensionStub::CmdIncrementalOnBackup;
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

    return BExcepUltils::ExceptionCatcherLocked(
        [&]() { return ErrCode((this->*(interfaceIndex->second))(data, reply)); });
}

ErrCode ExtExtensionStub::CmdGetFileHandle(MessageParcel &data, MessageParcel &reply)
{
    string fileName;
    if (!data.ReadString(fileName)) {
        return BError(BError::Codes::EXT_INVAL_ARG, "Failed to receive fileName").GetCode();
    }

    UniqueFd fd = GetFileHandle(fileName);
    bool fdFlag = fd < 0 ? false : true;
    reply.WriteBool(fdFlag);
    if (fdFlag == true && !reply.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::EXT_BROKEN_IPC, "Failed to send out the file").GetCode();
    }
    return BError(BError::Codes::OK);
}

ErrCode ExtExtensionStub::CmdHandleClear(MessageParcel &data, MessageParcel &reply)
{
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

ErrCode ExtExtensionStub::CmdHandleRestore(MessageParcel &data, MessageParcel &reply)
{
    ErrCode res = HandleRestore();
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::EXT_BROKEN_IPC, ss.str()).GetCode();
    }
    return BError(BError::Codes::OK);
}

ErrCode ExtExtensionStub::CmdGetIncrementalFileHandle(MessageParcel &data, MessageParcel &reply)
{
    string fileName;
    if (!data.ReadString(fileName)) {
        return BError(BError::Codes::EXT_INVAL_ARG, "Failed to receive fileName").GetCode();
    }

    ErrCode res = GetIncrementalFileHandle(fileName);
    if (!reply.WriteInt32(res)) {
        return BError(BError::Codes::EXT_BROKEN_IPC, "Failed to send out the file").GetCode();
    }
    return BError(BError::Codes::OK);
}

ErrCode ExtExtensionStub::CmdPublishIncrementalFile(MessageParcel &data, MessageParcel &reply)
{
    string fileName;
    if (!data.ReadString(fileName)) {
        return BError(BError::Codes::EXT_INVAL_ARG, "Failed to receive fileName");
    }

    ErrCode res = PublishIncrementalFile(fileName);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::EXT_BROKEN_IPC, ss.str()).GetCode();
    }
    return BError(BError::Codes::OK);
}

ErrCode ExtExtensionStub::CmdHandleIncrementalBackup(MessageParcel &data, MessageParcel &reply)
{
    UniqueFd incrementalFd(data.ReadFileDescriptor());
    UniqueFd manifestFd(data.ReadFileDescriptor());
    ErrCode res = HandleIncrementalBackup(move(incrementalFd), move(manifestFd));
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::EXT_BROKEN_IPC, ss.str()).GetCode();
    }
    return BError(BError::Codes::OK);
}

ErrCode ExtExtensionStub::CmdIncrementalOnBackup(MessageParcel &data, MessageParcel &reply)
{
    ErrCode res = IncrementalOnBackup();
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::EXT_BROKEN_IPC, ss.str()).GetCode();
    }
    return BError(BError::Codes::OK);
}

ErrCode ExtExtensionStub::CmdGetIncrementalBackupFileHandle(MessageParcel &data, MessageParcel &reply)
{
    auto [incrementalFd, manifestFd] = GetIncrementalBackupFileHandle();
    if (!reply.WriteFileDescriptor(incrementalFd)) {
        return BError(BError::Codes::EXT_BROKEN_IPC, "Failed to send out the file").GetCode();
    }
    if (!reply.WriteFileDescriptor(manifestFd)) {
        return BError(BError::Codes::EXT_BROKEN_IPC, "Failed to send out the file").GetCode();
    }
    return BError(BError::Codes::OK);
}

ErrCode ExtExtensionStub::CmdGetBackupInfo(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("CmdGetBackupInfo Begin");
    std::string result;
    int ret = GetBackupInfo(result);
    if (!reply.WriteInt32(ret)) {
        return BError(BError::Codes::EXT_BROKEN_IPC, "Failed to send out the ret").GetCode();
    }
    if (!reply.WriteString(result)) {
        return BError(BError::Codes::EXT_BROKEN_IPC, "Failed to send out the result").GetCode();
    }
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup