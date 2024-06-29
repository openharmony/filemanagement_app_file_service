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

#include "module_ipc/svc_extension_proxy.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
const int INVALID_FD = -1;

UniqueFd SvcExtensionProxy::GetFileHandle(const string &fileName, int32_t &errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Start");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    if (!data.WriteString(fileName)) {
        BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fileName");
        return UniqueFd(-1);
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(IExtensionInterfaceCode::CMD_GET_FILE_HANDLE), data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return UniqueFd(-ret);
    }

    HILOGI("Successful");
    bool fdFlag = reply.ReadBool();
    errCode = reply.ReadInt32();
    UniqueFd fd = UniqueFd(INVALID_FD);
    if (fdFlag == true) {
        fd = UniqueFd(reply.ReadFileDescriptor());
    }
    return UniqueFd(fd.Release());
}

ErrCode SvcExtensionProxy::HandleClear()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Start");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(IExtensionInterfaceCode::CMD_HANDLE_CLAER), data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ErrCode(ret);
    }

    HILOGI("Successful");
    return reply.ReadInt32();
}

ErrCode SvcExtensionProxy::HandleBackup()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Start");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(IExtensionInterfaceCode::CMD_HANDLE_BACKUP), data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ErrCode(ret);
    }

    HILOGI("Successful");
    return reply.ReadInt32();
}

ErrCode SvcExtensionProxy::PublishFile(const string &fileName)
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
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(IExtensionInterfaceCode::CMD_PUBLISH_FILE), data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ErrCode(ret);
    }

    HILOGD("Successful");
    return reply.ReadInt32();
}

ErrCode SvcExtensionProxy::HandleRestore()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Start");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(IExtensionInterfaceCode::CMD_HANDLE_RESTORE), data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ErrCode(ret);
    }

    HILOGI("Successful");
    return reply.ReadInt32();
}

ErrCode SvcExtensionProxy::GetBackupInfo(std::string &result)
{
    HILOGD("SvcExtensionProxy::GetBackupInfo begin.");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(IExtensionInterfaceCode::CMD_GET_BACKUP_INFO), data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ErrCode(ret);
    }
    if (!reply.ReadInt32(ret)) {
        HILOGE("fail to ReadInt32 ret");
        return ErrCode(ret);
    }
    if (ret != NO_ERROR) {
        HILOGE("ret is not NO_ERROR. ret = %d", ret);
        return ErrCode(ret);
    }
    if (!reply.ReadString(result)) {
        HILOGE("fail to ReadInt32 ret");
        return ErrCode(ret);
    }
    HILOGI("SvcExtensionProxy::GetBackupInfo end. result: %s", result.c_str());
    return ret;
}

ErrCode SvcExtensionProxy::UpdateFdSendRate(std::string &bundleName, int32_t sendRate)
{
    HILOGD("SvcExtensionProxy::UpdateFdSendRate begin.");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());
    if (!data.WriteString(bundleName)) {
        BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleName");
        return ErrCode(EPERM);
    }
    if (!data.WriteInt32(sendRate)) {
        BError(BError::Codes::SDK_INVAL_ARG, "Failed to send sendRate");
        return ErrCode(EPERM);
    }
    MessageParcel reply;
    MessageOption option;
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(IExtensionInterfaceCode::CMD_UPDATE_FD_SENDRATE), data, reply,
            option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ErrCode(ret);
    }
    if (!reply.ReadInt32(ret)) {
        HILOGE("fail to read ret, ret is %{public}d", ret);
        return ErrCode(ret);
    }
    HILOGI("SvcExtensionProxy::UpdateFdSendRate end.");
    return ret;
}
} // namespace OHOS::FileManagement::Backup