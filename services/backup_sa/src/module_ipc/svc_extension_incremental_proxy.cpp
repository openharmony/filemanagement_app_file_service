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

#include "module_ipc/svc_extension_proxy.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ErrCode SvcExtensionProxy::GetIncrementalFileHandle(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Start");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    if (!data.WriteString(fileName)) {
        BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fileName");
        return ErrCode(EPERM);
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IExtensionInterfaceCode::CMD_GET_INCREMENTAL_FILE_HANDLE),
                                        data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ErrCode(ret);
    }

    HILOGI("Successful");
    return reply.ReadInt32();
}

ErrCode SvcExtensionProxy::PublishIncrementalFile(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Start");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    if (!data.WriteString(fileName)) {
        BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fileName");
        return ErrCode(EPERM);
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IExtensionInterfaceCode::CMD_PUBLISH_INCREMENTAL_FILE),
                                        data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ErrCode(ret);
    }

    HILOGI("Successful");
    return reply.ReadInt32();
}

ErrCode SvcExtensionProxy::HandleIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Start");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    data.WriteFileDescriptor(incrementalFd);
    data.WriteFileDescriptor(manifestFd);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IExtensionInterfaceCode::CMD_HANDLE_INCREMENTAL_BACKUP),
                                        data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ErrCode(ret);
    }

    HILOGI("Successful");
    return reply.ReadInt32();
}

tuple<UniqueFd, UniqueFd> SvcExtensionProxy::GetIncrementalBackupFileHandle()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Start");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IExtensionInterfaceCode::CMD_GET_INCREMENTAL_BACKUP_FILE_HANDLE), data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return {UniqueFd(-1), UniqueFd(-1)};
    }

    HILOGI("Successful");
    UniqueFd incrementalFd(reply.ReadFileDescriptor());
    UniqueFd manifestFd(reply.ReadFileDescriptor());
    return {move(incrementalFd), move(manifestFd)};
}
} // namespace OHOS::FileManagement::Backup