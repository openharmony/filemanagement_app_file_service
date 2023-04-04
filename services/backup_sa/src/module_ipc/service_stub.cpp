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

/*
 * 注意：
 *     - 注意点1：本文件原则上只处理与IPC模块的IO，具体业务逻辑实现在service.cpp中
 *     - 注意点2：所有调用开头处打印 Begin 字样，通过BError返回正常结果/错误码，这是出于防抵赖的目的
 */

#include "module_ipc/service_stub.h"

#include <sstream>

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/service_reverse_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace BExcepUltils;

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
    const std::u16string descriptor = ServiceStub::GetDescriptor();
    const std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return BError(BError::Codes::SA_INVAL_ARG, "Invalid remote descriptor");
    }

    HILOGI("Begin to call procedure indexed %{public}u", code);
    auto interfaceIndex = opToInterfaceMap_.find(code);
    if (interfaceIndex == opToInterfaceMap_.end() || !interfaceIndex->second) {
        HILOGE("Cannot response request %{public}d : unknown procedure", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return ExceptionCatcherLocked([&]() { return ErrCode((this->*(interfaceIndex->second))(data, reply)); });
}

int32_t ServiceStub::CmdInitRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("Begin");
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
    HILOGI("Begin");
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
    HILOGI("Begin");
    UniqueFd fd(GetLocalCapabilities());
    if (!reply.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SA_BROKEN_IPC, "Failed to send out the file");
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdPublishFile(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("Begin");
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
    HILOGI("Begin");
    string fileName;
    if (!data.ReadString(fileName)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fileName");
    }
    UniqueFd fd(data.ReadFileDescriptor());
    if (fd < 0) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fd");
    }

    int res = AppFileReady(fileName, move(fd));
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppDone(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("Begin");
    bool success;
    if (!data.ReadBool(success)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bool flag");
    }
    int res = AppDone(success);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetFileHandle(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("Begin");
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
    HILOGI("Begin");
    UniqueFd fd(data.ReadFileDescriptor());
    if (fd < 0) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fd");
    }

    vector<string> bundleNames;
    if (!data.ReadStringVector(&bundleNames)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleNames");
    }

    int res = AppendBundlesRestoreSession(move(fd), bundleNames);
    if (!reply.WriteInt32(res)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to send the result ") + to_string(res));
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdAppendBundlesBackupSession(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("Begin");
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

int32_t ServiceStub::CmdFinish(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("Begin");
    int res = Finish();
    if (!reply.WriteInt32(res)) {
        return BError(BError::Codes::SA_BROKEN_IPC, string("Failed to send the result ") + to_string(res));
    }
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup