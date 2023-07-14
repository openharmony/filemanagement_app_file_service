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

#include "module_ipc/svc_extension_proxy.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

UniqueFd SvcExtensionProxy::GetFileHandle(const string &fileName)
{
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
    UniqueFd fd(reply.ReadFileDescriptor());
    return UniqueFd(fd.Release());
}

ErrCode SvcExtensionProxy::HandleClear()
{
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

    HILOGI("Successful");
    return reply.ReadInt32();
}
} // namespace OHOS::FileManagement::Backup