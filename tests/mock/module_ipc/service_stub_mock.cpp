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

#include "module_ipc/service_stub.h"

#include <sstream>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "module_ipc/service_reverse_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ServiceStub::ServiceStub()
{
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_FINISH)] = &ServiceStub::CmdFinish;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_RELSEASE_SESSION)] =
        &ServiceStub::CmdRelease;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_RESTORE_SESSION)] =
        &ServiceStub::CmdInitRestoreSession;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_BACKUP_SESSION)] =
        &ServiceStub::CmdInitBackupSession;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_LOCAL_CAPABILITIES)] =
        &ServiceStub::CmdGetLocalCapabilities;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_FILE_READY)] =
        &ServiceStub::CmdAppFileReady;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_PUBLISH_FILE)] =
        &ServiceStub::CmdPublishFile;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_DONE)] = &ServiceStub::CmdAppDone;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_RESULT_REPORT)] =
        &ServiceStub::CmdResultReport;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_START)] = &ServiceStub::CmdStart;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_FILE_NAME)] =
        &ServiceStub::CmdGetFileHandle;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION)] =
        &ServiceStub::CmdAppendBundlesRestoreSession;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION_DETAILS)]
        = &ServiceStub::CmdAppendBundlesDetailsRestoreSession;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_BACKUP_SESSION)] =
        &ServiceStub::CmdAppendBundlesBackupSession;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_LOCAL_CAPABILITIES_INCREMENTAL)] =
        &ServiceStub::CmdGetLocalCapabilitiesIncremental;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_INCREMENTAL_BACKUP_SESSION)] =
        &ServiceStub::CmdInitIncrementalBackupSession;
    opToInterfaceMap_[static_cast<uint32_t>(
        IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION_DETAILS)] =
        &ServiceStub::CmdAppendBundlesDetailsIncrementalBackupSession;
    opToInterfaceMap_[static_cast<uint32_t>(
        IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION)] =
        &ServiceStub::CmdAppendBundlesIncrementalBackupSession;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_PUBLISH_INCREMENTAL_FILE)] =
        &ServiceStub::CmdPublishIncrementalFile;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_INCREMENTAL_FILE_READY)] =
        &ServiceStub::CmdAppIncrementalFileReady;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_INCREMENTAL_FILE_NAME)] =
        &ServiceStub::CmdGetIncrementalFileHandle;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_BACKUP_INFO)] =
        &ServiceStub::CmdGetBackupInfo;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_UPDATE_TIMER)] =
        &ServiceStub::CmdUpdateTimer;
    opToInterfaceMap_[static_cast<uint32_t>(
        IServiceInterfaceCode::SERVICE_CMD_GET_APP_LOCAL_LIST_AND_DO_INCREMENTAL_BACKUP)] =
        &ServiceStub::CmdGetAppLocalListAndDoIncrementalBackup;
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
    int res = AppFileReady(fileName, move(fd), 0);
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

int32_t ServiceStub::CmdResultReport(MessageParcel &data, MessageParcel &reply)
{
    std::string restoreRetInfo;
    data.ReadString(restoreRetInfo);
    int32_t scenario;
    data.ReadInt32(scenario);
    BackupRestoreScenario type = static_cast<BackupRestoreScenario>(scenario);
    int res = ServiceResultReport(restoreRetInfo, type);
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

int32_t ServiceStub::CmdAppendBundlesDetailsRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    UniqueFd fd(data.ReadFileDescriptor());
    std::vector<string> bundleNames;
    data.ReadStringVector(&bundleNames);
    std::vector<string> detailInfos;
    data.ReadStringVector(&detailInfos);
    int res = AppendBundlesRestoreSession(move(fd), bundleNames, detailInfos);
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

int32_t ServiceStub::CmdGetBackupInfo(MessageParcel &data, MessageParcel &reply)
{
    int ret = ERR_OK;
    string bundleName;
    if (!data.ReadString(bundleName)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to recive bundleName"));
    }
    std::string result;
    ret = GetBackupInfo(bundleName, result);
    return ret;
}

int32_t ServiceStub::CmdUpdateTimer(MessageParcel &data, MessageParcel &reply)
{
    int ret = ERR_OK;
    string bundleName;
    if (!data.ReadString(bundleName)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to recive bundleName"));
    }
    uint32_t timeOut;
    if (!data.ReadUint32(timeOut)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to recive timeOut"));
    }
    bool result;
    ret = UpdateTimer(bundleName, timeOut, result);
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdRelease(MessageParcel &data, MessageParcel &reply)
{
    int res = Release();
    reply.WriteInt32(res);
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetLocalCapabilitiesIncremental(MessageParcel &data, MessageParcel &reply)
{
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetAppLocalListAndDoIncrementalBackup(MessageParcel &data, MessageParcel &reply)
{
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdInitIncrementalBackupSession(MessageParcel &data, MessageParcel &reply)
{
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppendBundlesIncrementalBackupSession(MessageParcel &data, MessageParcel &reply)
{
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppendBundlesDetailsIncrementalBackupSession(MessageParcel &data, MessageParcel &reply)
{
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdPublishIncrementalFile(MessageParcel &data, MessageParcel &reply)
{
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppIncrementalFileReady(MessageParcel &data, MessageParcel &reply)
{
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetIncrementalFileHandle(MessageParcel &data, MessageParcel &reply)
{
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup
