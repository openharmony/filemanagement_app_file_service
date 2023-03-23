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

#include "service_reverse_stub.h"

#include <cstdint>
#include <sstream>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

int32_t ServiceReverseStub::OnRemoteRequest(uint32_t code,
                                            MessageParcel &data,
                                            MessageParcel &reply,
                                            MessageOption &option)
{
    const std::u16string descriptor = ServiceReverseStub::GetDescriptor();
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
    return (this->*(interfaceIndex->second))(data, reply);
}

ServiceReverseStub::ServiceReverseStub()
{
    opToInterfaceMap_[SERVICER_BACKUP_ON_FILE_READY] = &ServiceReverseStub::CmdBackupOnFileReady;
    opToInterfaceMap_[SERVICER_BACKUP_ON_SUB_TASK_STARTED] = &ServiceReverseStub::CmdBackupOnBundleStarted;
    opToInterfaceMap_[SERVICER_BACKUP_ON_SUB_TASK_FINISHED] = &ServiceReverseStub::CmdBackupOnBundleFinished;
    opToInterfaceMap_[SERVICER_BACKUP_ON_TASK_FINISHED] = &ServiceReverseStub::CmdBackupOnAllBundlesFinished;

    opToInterfaceMap_[SERVICER_RESTORE_ON_SUB_TASK_STARTED] = &ServiceReverseStub::CmdRestoreOnBundleStarted;
    opToInterfaceMap_[SERVICER_RESTORE_ON_SUB_TASK_FINISHED] = &ServiceReverseStub::CmdRestoreOnBundleFinished;
    opToInterfaceMap_[SERVICER_RESTORE_ON_TASK_FINISHED] = &ServiceReverseStub::CmdRestoreOnAllBundlesFinished;
    opToInterfaceMap_[SERVICER_RESTORE_ON_FILE_READY] = &ServiceReverseStub::CmdRestoreOnFileReady;
}

int32_t ServiceReverseStub::CmdBackupOnFileReady(MessageParcel &data, MessageParcel &reply)
{
    auto bundleName = data.ReadString();
    auto fileName = data.ReadString();
    int fd = data.ReadFileDescriptor();
    BackupOnFileReady(bundleName, fileName, fd);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdBackupOnBundleStarted(MessageParcel &data, MessageParcel &reply)
{
    int32_t errCode = data.ReadInt32();
    auto bundleName = data.ReadString();
    BackupOnBundleStarted(errCode, bundleName);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdBackupOnBundleFinished(MessageParcel &data, MessageParcel &reply)
{
    int32_t errCode = data.ReadInt32();
    auto bundleName = data.ReadString();
    BackupOnBundleFinished(errCode, bundleName);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdBackupOnAllBundlesFinished(MessageParcel &data, MessageParcel &reply)
{
    int32_t errCode = data.ReadInt32();
    BackupOnAllBundlesFinished(errCode);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdRestoreOnBundleStarted(MessageParcel &data, MessageParcel &reply)
{
    int32_t errCode = data.ReadInt32();
    auto bundleName = data.ReadString();
    RestoreOnBundleStarted(errCode, bundleName);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdRestoreOnBundleFinished(MessageParcel &data, MessageParcel &reply)
{
    int32_t errCode = data.ReadInt32();
    auto bundleName = data.ReadString();
    RestoreOnBundleFinished(errCode, bundleName);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdRestoreOnAllBundlesFinished(MessageParcel &data, MessageParcel &reply)
{
    int32_t errCode = data.ReadInt32();
    RestoreOnAllBundlesFinished(errCode);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdRestoreOnFileReady(MessageParcel &data, MessageParcel &reply)
{
    auto bundleName = data.ReadString();
    auto fileName = data.ReadString();
    int fd = data.ReadFileDescriptor();
    RestoreOnFileReady(bundleName, fileName, fd);
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup