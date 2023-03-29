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

#include "module_ipc/service_stub.h"

#include <sstream>

#include "b_error/b_error.h"
#include "module_ipc/service_reverse_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ServiceStub::ServiceStub()
{
    opToInterfaceMap_[SERVICE_CMD_INIT_RESTORE_SESSION] = &ServiceStub::CmdInitRestoreSession;
    opToInterfaceMap_[SERVICE_CMD_INIT_BACKUP_SESSION] = &ServiceStub::CmdInitBackupSession;
    opToInterfaceMap_[SERVICE_CMD_GET_LOCAL_CAPABILITIES] = &ServiceStub::CmdGetLocalCapabilities;
    opToInterfaceMap_[SERVICE_CMD_PUBLISH_FILE] = &ServiceStub::CmdPublishFile;
    opToInterfaceMap_[SERVICE_CMD_APP_FILE_READY] = &ServiceStub::CmdAppFileReady;
    opToInterfaceMap_[SERVICE_CMD_APP_DONE] = &ServiceStub::CmdAppDone;
    opToInterfaceMap_[SERVICE_CMD_START] = &ServiceStub::CmdStart;
    opToInterfaceMap_[SERVICE_CMD_GET_FILE_NAME] = &ServiceStub::CmdGetFileHandle;
    opToInterfaceMap_[SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION] = &ServiceStub::CmdAppendBundlesRestoreSession;
    opToInterfaceMap_[SERVICE_CMD_APPEND_BUNDLES_BACKUP_SESSION] = &ServiceStub::CmdAppendBundlesBackupSession;
    opToInterfaceMap_[SERVICE_CMD_FINISH] = &ServiceStub::CmdFinish;
}

int32_t ServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    auto interfaceIndex = opToInterfaceMap_.find(code);
    if (interfaceIndex == opToInterfaceMap_.end() || !interfaceIndex->second) {
        return BError(BError::Codes::OK);
    }

    const std::u16string descriptor = ServiceStub::GetDescriptor();
    const std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return BError(BError::Codes::OK);
    }
    return (this->*(interfaceIndex->second))(data, reply);
}

int32_t ServiceStub::CmdInitRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    auto remote = data.ReadRemoteObject();
    auto iremote = iface_cast<IServiceReverse>(remote);

    int32_t res = InitRestoreSession(iremote);
    reply.WriteInt32(res);
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdInitBackupSession(MessageParcel &data, MessageParcel &reply)
{
    auto remote = data.ReadRemoteObject();
    auto iremote = iface_cast<IServiceReverse>(remote);

    int res = InitBackupSession(iremote);
    reply.WriteInt32(res);
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdStart(MessageParcel &data, MessageParcel &reply)
{
    int res = Start();
    reply.WriteInt32(res);
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetLocalCapabilities(MessageParcel &data, MessageParcel &reply)
{
    UniqueFd fd(GetLocalCapabilities());
    reply.WriteFileDescriptor(fd);
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdPublishFile(MessageParcel &data, MessageParcel &reply)
{
    unique_ptr<BFileInfo> fileInfo(data.ReadParcelable<BFileInfo>());
    int res = PublishFile(*fileInfo);
    reply.WriteInt32(res);
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppFileReady(MessageParcel &data, MessageParcel &reply)
{
    string fileName;
    data.ReadString(fileName);
    UniqueFd fd(data.ReadFileDescriptor());
    int res = AppFileReady(fileName, move(fd));
    reply.WriteInt32(res);
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppDone(MessageParcel &data, MessageParcel &reply)
{
    bool success;
    data.ReadBool(success);
    int res = AppDone(success);
    reply.WriteInt32(res);
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetFileHandle(MessageParcel &data, MessageParcel &reply)
{
    string bundleName;
    data.ReadString(bundleName);
    string fileName;
    data.ReadString(fileName);

    return GetFileHandle(bundleName, fileName);
}

int32_t ServiceStub::CmdAppendBundlesRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    UniqueFd fd(data.ReadFileDescriptor());
    std::vector<string> bundleNames;
    data.ReadStringVector(&bundleNames);

    int res = AppendBundlesRestoreSession(move(fd), bundleNames);
    reply.WriteInt32(res);
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppendBundlesBackupSession(MessageParcel &data, MessageParcel &reply)
{
    std::vector<string> bundleNames;
    data.ReadStringVector(&bundleNames);

    int res = AppendBundlesBackupSession(bundleNames);
    reply.WriteInt32(res);
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdFinish(MessageParcel &data, MessageParcel &reply)
{
    int res = Finish();
    reply.WriteInt32(res);
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup
