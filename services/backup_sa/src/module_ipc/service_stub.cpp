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

/*
 * 注意：
 *     - 注意点1：本文件原则上只处理与IPC模块的IO，具体业务逻辑实现在service.cpp中
 *     - 注意点2：所有调用开头处打印 Begin 字样，通过BError返回正常结果/错误码，这是出于防抵赖的目的
 */

#include "module_ipc/service_stub.h"

#include <sstream>

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/service_reverse_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
const int INVALID_FD = -1;


void ServiceStub::ServiceStubSupplement()
{
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_UPDATE_TIMER)] =
        &ServiceStub::CmdUpdateTimer;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_UPDATE_SENDRATE)] =
        &ServiceStub::CmdUpdateSendRate;
    opToInterfaceMap_[static_cast<uint32_t>(
        IServiceInterfaceCode::SERVICE_CMD_GET_APP_LOCAL_LIST_AND_DO_INCREMENTAL_BACKUP)] =
        &ServiceStub::CmdGetAppLocalListAndDoIncrementalBackup;
    opToInterfaceMap_[static_cast<uint32_t>(
        IServiceInterfaceCode::SERVICE_CMD_REPORT_APP_PROCESS_INFO)] =
        &ServiceStub::CmdReportAppProcessInfo;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_START_EXT_TIMER)] =
        &ServiceStub::CmdStartExtTimer;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_START_FWK_TIMER)] =
        &ServiceStub::CmdStartFwkTimer;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_CANCEL_BUNDLE)] =
        &ServiceStub::CmdCancel;
}

void ServiceStub::ServiceStubSuppAppendBundles()
{
    opToInterfaceMap_[static_cast<uint32_t>(
        IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION)] =
        &ServiceStub::CmdAppendBundlesIncrementalBackupSession;
    opToInterfaceMap_[static_cast<uint32_t>(
        IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION_DETAILS)] =
        &ServiceStub::CmdAppendBundlesDetailsIncrementalBackupSession;
}

ServiceStub::ServiceStub()
{
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_RESTORE_SESSION)] =
        &ServiceStub::CmdInitRestoreSession;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_BACKUP_SESSION)] =
        &ServiceStub::CmdInitBackupSession;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_LOCAL_CAPABILITIES)] =
        &ServiceStub::CmdGetLocalCapabilities;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_PUBLISH_FILE)] =
        &ServiceStub::CmdPublishFile;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_FILE_READY)] =
        &ServiceStub::CmdAppFileReady;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_DONE)] = &ServiceStub::CmdAppDone;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_RESULT_REPORT)] =
        &ServiceStub::CmdResultReport;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_START)] = &ServiceStub::CmdStart;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_FILE_NAME)] =
        &ServiceStub::CmdGetFileHandle;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION)] =
        &ServiceStub::CmdAppendBundlesRestoreSession;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION_DETAIL)] =
        &ServiceStub::CmdAppendBundlesDetailsRestoreSession;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_BACKUP_SESSION)] =
        &ServiceStub::CmdAppendBundlesBackupSession;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_BACKUP_SESSION_DETAILS)] =
        &ServiceStub::CmdAppendBundlesDetailsBackupSession;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_FINISH)] = &ServiceStub::CmdFinish;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_RELSEASE_SESSION)] =
        &ServiceStub::CmdRelease;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_LOCAL_CAPABILITIES_INCREMENTAL)] =
        &ServiceStub::CmdGetLocalCapabilitiesIncremental;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_INCREMENTAL_BACKUP_SESSION)] =
        &ServiceStub::CmdInitIncrementalBackupSession;
    ServiceStubSuppAppendBundles();
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_PUBLISH_INCREMENTAL_FILE)] =
        &ServiceStub::CmdPublishIncrementalFile;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_PUBLISH_SA_INCREMENTAL_FILE)] =
        &ServiceStub::CmdPublishSAIncrementalFile;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_INCREMENTAL_FILE_READY)] =
        &ServiceStub::CmdAppIncrementalFileReady;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_INCREMENTAL_DONE)] =
        &ServiceStub::CmdAppIncrementalDone;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_INCREMENTAL_FILE_NAME)] =
        &ServiceStub::CmdGetIncrementalFileHandle;
    opToInterfaceMap_[static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_BACKUP_INFO)] =
        &ServiceStub::CmdGetBackupInfo;
    ServiceStubSupplement();
}

int32_t ServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    const std::u16string descriptor = ServiceStub::GetDescriptor();
    const std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return BError(BError::Codes::SA_INVAL_ARG, "Invalid remote descriptor");
    }

    HILOGD("Begin to call procedure indexed %{public}u", code);
    auto interfaceIndex = opToInterfaceMap_.find(code);
    if (interfaceIndex == opToInterfaceMap_.end() || !interfaceIndex->second) {
        HILOGE("Cannot response request %{public}d : unknown procedure", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return BExcepUltils::ExceptionCatcherLocked(
        [&]() { return ErrCode((this->*(interfaceIndex->second))(data, reply)); });
}

int32_t ServiceStub::CmdInitRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    auto remote = data.ReadRemoteObject();
    if (!remote) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive the reverse stub");
    }
    auto iremote = iface_cast<IServiceReverse>(remote);
    if (!iremote) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive the reverse stub");
    }

    int32_t res = InitRestoreSession(iremote);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdInitBackupSession(MessageParcel &data, MessageParcel &reply)
{
    auto remote = data.ReadRemoteObject();
    if (!remote) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive the reverse stub");
    }
    auto iremote = iface_cast<IServiceReverse>(remote);
    if (!iremote) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive the reverse stub");
    }

    int res = InitBackupSession(iremote);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdStart(MessageParcel &data, MessageParcel &reply)
{
    int res = Start();
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetLocalCapabilities(MessageParcel &data, MessageParcel &reply)
{
    UniqueFd fd(GetLocalCapabilities());
    if (!reply.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SA_BROKEN_IPC, "Failed to send out the file");
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdPublishFile(MessageParcel &data, MessageParcel &reply)
{
    unique_ptr<BFileInfo> fileInfo(data.ReadParcelable<BFileInfo>());
    if (!fileInfo) {
        return BError(BError::Codes::SA_BROKEN_IPC, "Failed to receive fileInfo");
    }
    int res = PublishFile(*fileInfo);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppFileReady(MessageParcel &data, MessageParcel &reply)
{
    string fileName;
    if (!data.ReadString(fileName)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fileName");
    }
    bool fdFlag = data.ReadBool();
    UniqueFd fd = UniqueFd(INVALID_FD);
    if (fdFlag == true) {
        fd = UniqueFd(data.ReadFileDescriptor());
        if (fd < 0) {
            return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fd");
        }
    }
    int32_t errCode = data.ReadInt32();

    int res = AppFileReady(fileName, move(fd), errCode);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppDone(MessageParcel &data, MessageParcel &reply)
{
    ErrCode errCode;
    if (!data.ReadInt32(errCode)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive errCode");
    }
    int res = AppDone(errCode);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdResultReport(MessageParcel &data, MessageParcel &reply)
{
    std::string restoreRetInfo;
    int32_t scenario;
    ErrCode errCode;
    if (!data.ReadString(restoreRetInfo)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive restoreRetInfo");
    }
    if (!data.ReadInt32(scenario)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive scenario");
    }
    BackupRestoreScenario secenrioInfo = static_cast<BackupRestoreScenario>(scenario);
    if (!data.ReadInt32(errCode)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive errCode");
    }
    int res = ServiceResultReport(restoreRetInfo, secenrioInfo, errCode);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetFileHandle(MessageParcel &data, MessageParcel &reply)
{
    string bundleName;
    if (!data.ReadString(bundleName)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleName").GetCode();
    }
    string fileName;
    if (!data.ReadString(fileName)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fileName").GetCode();
    }

    return GetFileHandle(bundleName, fileName);
}

int32_t ServiceStub::CmdAppendBundlesRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    UniqueFd fd(data.ReadFileDescriptor());
    if (fd < 0) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fd");
    }

    vector<string> bundleNames;
    if (!data.ReadStringVector(&bundleNames)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleNames");
    }
    int32_t type;
    if (!data.ReadInt32(type)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive restoreType");
    }
    RestoreTypeEnum restoreType = static_cast<RestoreTypeEnum>(type);
    int32_t userId;
    if (!data.ReadInt32(userId)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive userId");
    }

    int res = AppendBundlesRestoreSession(move(fd), bundleNames, restoreType, userId);
    if (!reply.WriteInt32(res)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to send the result ") + to_string(res));
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppendBundlesDetailsRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    UniqueFd fd(data.ReadFileDescriptor());
    if (fd < 0) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fd");
    }

    vector<string> bundleNames;
    if (!data.ReadStringVector(&bundleNames)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleNames");
    }
    vector<string> detailInfos;
    if (!data.ReadStringVector(&detailInfos)) {
        HILOGE("Failed to receive bundle detailInfos");
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive detailInfos");
    }
    int32_t type;
    if (!data.ReadInt32(type)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive restoreType");
    }
    RestoreTypeEnum restoreType = static_cast<RestoreTypeEnum>(type);
    int32_t userId;
    if (!data.ReadInt32(userId)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive userId");
    }

    int res = AppendBundlesRestoreSession(move(fd), bundleNames, detailInfos, restoreType, userId);
    if (!reply.WriteInt32(res)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to send the result ") + to_string(res));
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppendBundlesBackupSession(MessageParcel &data, MessageParcel &reply)
{
    vector<string> bundleNames;
    if (!data.ReadStringVector(&bundleNames)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleNames");
    }

    int32_t res = AppendBundlesBackupSession(bundleNames);
    if (!reply.WriteInt32(res)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to send the result ") + to_string(res));
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppendBundlesDetailsBackupSession(MessageParcel &data, MessageParcel &reply)
{
    vector<string> bundleNames;
    if (!data.ReadStringVector(&bundleNames)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleNames");
    }

    vector<string> detailInfos;
    if (!data.ReadStringVector(&detailInfos)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive detailInfos");
    }

    int32_t res = AppendBundlesDetailsBackupSession(bundleNames, detailInfos);
    if (!reply.WriteInt32(res)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to send the result ") + to_string(res));
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdFinish(MessageParcel &data, MessageParcel &reply)
{
    int res = Finish();
    if (!reply.WriteInt32(res)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to send the result ") + to_string(res));
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetBackupInfo(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("ServiceStub::CmdGetBackupInfo Begin.");
    int ret = ERR_OK;
    string bundleName;
    if (!data.ReadString(bundleName)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to recive bundleName"));
    }
    string result;
    ret = GetBackupInfo(bundleName, result);
    if (ret != ERR_OK) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to call GetBackupInfo"));
    }
    if (!reply.WriteString(result)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to write result"));
    }
    HILOGI("ServiceStub::CmdGetBackupInfo end.");
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdUpdateTimer(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("ServiceStub::CmdUpdateTimer Begin.");
    int ret = ERR_OK;
    string bundleName;
    if (!data.ReadString(bundleName)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to recive bundleName"));
    }
    uint32_t timeout;
    if (!data.ReadUint32(timeout)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to recive timeout"));
    }
    bool result;
    ret = UpdateTimer(bundleName, timeout, result);
    if (ret != ERR_OK) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to call UpdateTimer"));
    }
    if (!reply.WriteBool(result)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to write result"));
    }
    HILOGI("ServiceStub::CmdUpdateTimer end.");
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdUpdateSendRate(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("ServiceStub::CmdUpdateSendRate Begin.");
    int ret = ERR_OK;
    string bundleName;
    if (!data.ReadString(bundleName)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to recive bundleName"));
    }
    int32_t sendRate;
    if (!data.ReadInt32(sendRate)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to recive sendRate"));
    }
    bool result;
    ret = UpdateSendRate(bundleName, sendRate, result);
    if (ret != ERR_OK) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to call UpdateSendRate"));
    }
    if (!reply.WriteBool(result)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to write result"));
    }
    HILOGI("ServiceStub::CmdUpdateSendRate end.");
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdStartExtTimer(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("ServiceStub::CmdStartExtTimer Begin.");
    int ret = ERR_OK;
    bool isExtStart;
    ret = StartExtTimer(isExtStart);
    if (ret != ERR_OK) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to call UpdateTimer"));
    }
    if (!reply.WriteBool(isExtStart)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to write result"));
    }
    HILOGI("ServiceStub::CmdStartExtTimer end.");
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdStartFwkTimer(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("ServiceStub::CmdStartFwkTimer Begin.");
    int ret = ERR_OK;
    bool isFwkStart;
    ret = StartFwkTimer(isFwkStart);
    if (ret != ERR_OK) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to call UpdateTimer"));
    }
    if (!reply.WriteBool(isFwkStart)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to write result"));
    }
    HILOGI("ServiceStub::CmdStartFwkTimer end.");
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdRelease(MessageParcel &data, MessageParcel &reply)
{
    int res = Release();
    if (!reply.WriteInt32(res)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to send the result ") + to_string(res));
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetLocalCapabilitiesIncremental(MessageParcel &data, MessageParcel &reply)
{
    vector<BIncrementalData> bundleNames;
    if (!ReadParcelableVector(bundleNames, data)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleNames");
    }

    UniqueFd fd(GetLocalCapabilitiesIncremental(bundleNames));
    if (!reply.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SA_BROKEN_IPC, "Failed to send out the file");
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetAppLocalListAndDoIncrementalBackup(MessageParcel &data, MessageParcel &reply)
{
    int32_t res = GetAppLocalListAndDoIncrementalBackup();
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdInitIncrementalBackupSession(MessageParcel &data, MessageParcel &reply)
{
    auto remote = data.ReadRemoteObject();
    if (!remote) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive the reverse stub");
    }
    auto iremote = iface_cast<IServiceReverse>(remote);
    if (!iremote) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive the reverse stub");
    }

    int32_t res = InitIncrementalBackupSession(iremote);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppendBundlesIncrementalBackupSession(MessageParcel &data, MessageParcel &reply)
{
    vector<BIncrementalData> bundlesToBackup;
    if (!ReadParcelableVector(bundlesToBackup, data)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleNames");
    }

    int32_t res = AppendBundlesIncrementalBackupSession(bundlesToBackup);
    if (!reply.WriteInt32(res)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to send the result ") + to_string(res));
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppendBundlesDetailsIncrementalBackupSession(MessageParcel &data, MessageParcel &reply)
{
    vector<BIncrementalData> bundlesToBackup;
    if (!ReadParcelableVector(bundlesToBackup, data)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleNames");
    }
    std::vector<std::string> infos;
    if (!data.ReadStringVector(&infos)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive infos");
    }
    int32_t res = AppendBundlesIncrementalBackupSession(bundlesToBackup, infos);
    if (!reply.WriteInt32(res)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to send the result ") + to_string(res));
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdPublishIncrementalFile(MessageParcel &data, MessageParcel &reply)
{
    unique_ptr<BFileInfo> fileInfo(data.ReadParcelable<BFileInfo>());
    if (!fileInfo) {
        return BError(BError::Codes::SA_BROKEN_IPC, "Failed to receive fileInfo");
    }
    int res = PublishIncrementalFile(*fileInfo);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdPublishSAIncrementalFile(MessageParcel &data, MessageParcel &reply)
{
    unique_ptr<BFileInfo> fileInfo(data.ReadParcelable<BFileInfo>());
    if (!fileInfo) {
        return BError(BError::Codes::SA_BROKEN_IPC, "Failed to receive fileInfo");
    }
    UniqueFd fd(data.ReadFileDescriptor());
    int res = PublishSAIncrementalFile(*fileInfo, move(fd));
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppIncrementalFileReady(MessageParcel &data, MessageParcel &reply)
{
    string fileName;
    if (!data.ReadString(fileName)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fileName");
    }
    bool fdFlag = data.ReadBool();
    UniqueFd fd = UniqueFd(INVALID_FD);
    UniqueFd manifestFd = UniqueFd(INVALID_FD);
    if (fdFlag == true) {
        fd = UniqueFd(data.ReadFileDescriptor());
        if (fd < 0) {
            return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fd");
        }

        manifestFd = UniqueFd(data.ReadFileDescriptor());
        if (manifestFd < 0) {
            return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive manifestFd");
        }
    }
    int32_t errCode = data.ReadInt32();

    int res = AppIncrementalFileReady(fileName, move(fd), move(manifestFd), errCode);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppIncrementalDone(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret;
    if (!data.ReadInt32(ret)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bool flag");
    }
    int res = AppIncrementalDone(ret);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetIncrementalFileHandle(MessageParcel &data, MessageParcel &reply)
{
    string bundleName;
    if (!data.ReadString(bundleName)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleName").GetCode();
    }
    string fileName;
    if (!data.ReadString(fileName)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fileName").GetCode();
    }
    return GetIncrementalFileHandle(bundleName, fileName);
}

int32_t ServiceStub::CmdReportAppProcessInfo(MessageParcel &data, MessageParcel &reply)
{
    string processInfo;
    if (!data.ReadString(processInfo)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleName").GetCode();
    }
    int32_t scenario;
    if (!data.ReadInt32(scenario)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive errCode");
    }
    BackupRestoreScenario secenrioInfo = static_cast<BackupRestoreScenario>(scenario);
    return ReportAppProcessInfo(processInfo, secenrioInfo);
}

template <typename T>
bool ServiceStub::ReadParcelableVector(std::vector<T> &parcelableInfos, MessageParcel &data)
{
    int32_t infoSize = 0;
    if (!data.ReadInt32(infoSize)) {
        HILOGE("Failed to read Parcelable size.");
        return false;
    }

    parcelableInfos.clear();
    infoSize = (infoSize < BConstants::MAX_PARCELABLE_VECTOR_NUM) ? infoSize : BConstants::MAX_PARCELABLE_VECTOR_NUM;
    for (int32_t index = 0; index < infoSize; index++) {
        sptr<T> info = data.ReadParcelable<T>();
        if (info == nullptr) {
            HILOGE("Failed to read Parcelable infos.");
            return false;
        }
        parcelableInfos.emplace_back(move(*info));
    }

    return true;
}

int32_t ServiceStub::CmdCancel(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("ServiceStub::CmdCancel Begin.");
    string bundleName;
    if (!data.ReadString(bundleName)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleName").GetCode();
    }
    int result = BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK;
    ErrCode res = Cancel(bundleName, result);
    if (!reply.WriteInt32(result)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to send the result ") + to_string(res));
    }
    return res;
}
} // namespace OHOS::FileManagement::Backup
