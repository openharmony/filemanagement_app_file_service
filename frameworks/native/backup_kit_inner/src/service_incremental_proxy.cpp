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

#include "service_proxy.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "svc_death_recipient.h"
#include "unique_fd.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ErrCode ServiceProxy::Release()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_RELSEASE_SESSION),
                                        data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

UniqueFd ServiceProxy::GetLocalCapabilitiesIncremental(const vector<BIncrementalData> &bundleNames)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        HILOGE("Failed to write descriptor");
        return UniqueFd(-EPERM);
    }

    if (!WriteParcelableVector(bundleNames, data)) {
        HILOGE("Failed to send the bundleNames");
        return UniqueFd(-EPERM);
    }

    MessageParcel reply;
    MessageOption option;
    option.SetWaitTime(BConstants::IPC_MAX_WAIT_TIME);
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_LOCAL_CAPABILITIES_INCREMENTAL), data, reply,
        option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return UniqueFd(-ret);
    }
    UniqueFd fd(reply.ReadFileDescriptor());
    return UniqueFd(fd.Release());
}

ErrCode ServiceProxy::GetAppLocalListAndDoIncrementalBackup()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        HILOGE("Failed to write descriptor");
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    MessageParcel reply;
    MessageOption option;
    option.SetWaitTime(BConstants::IPC_MAX_WAIT_TIME);
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_APP_LOCAL_LIST_AND_DO_INCREMENTAL_BACKUP),
        data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return BError(BError::Codes::SDK_INVAL_ARG, "Received error when doing IPC").GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::InitIncrementalBackupSession(sptr<IServiceReverse> remote)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        HILOGE("Failed to write descriptor");
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    MessageParcel reply;
    MessageOption option;

    if (!remote) {
        HILOGE("Empty reverse stub");
        return BError(BError::Codes::SDK_INVAL_ARG, "Empty reverse stub").GetCode();
    }
    if (!data.WriteRemoteObject(remote->AsObject().GetRefPtr())) {
        HILOGE("Failed to send the reverse stub");
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the reverse stub").GetCode();
    }

    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_INCREMENTAL_BACKUP_SESSION), data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return BError(BError::Codes::SDK_INVAL_ARG, "Received error when doing IPC").GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::AppendBundlesIncrementalBackupSession(const vector<BIncrementalData> &bundlesToBackup)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        HILOGE("Failed to write descriptor");
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    if (!WriteParcelableVector(bundlesToBackup, data)) {
        HILOGE("Failed to send the bundleNames");
        return UniqueFd(-EPERM);
    }

    MessageParcel reply;
    MessageOption option;

    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION), data,
        reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return BError(BError::Codes::SDK_INVAL_ARG, "Received error when doing IPC").GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::AppendBundlesIncrementalBackupSession(const vector<BIncrementalData> &bundlesToBackup,
    const std::vector<std::string> &infos)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        HILOGE("Failed to write descriptor");
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    if (!WriteParcelableVector(bundlesToBackup, data)) {
        HILOGE("Failed to send the bundleNames");
        return UniqueFd(-EPERM);
    }

    if (!data.WriteStringVector(infos)) {
        HILOGE("Failed to write infos");
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleNames").GetCode();
    }
    MessageParcel reply;
    MessageOption option;

    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION_DETAILS),
        data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return BError(BError::Codes::SDK_INVAL_ARG, "Received error when doing IPC").GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::PublishIncrementalFile(const BFileInfo &fileInfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    if (!data.WriteParcelable(&fileInfo)) {
        HILOGE("Failed to send the fileInfo");
        return -EPIPE;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_PUBLISH_INCREMENTAL_FILE), data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::PublishSAIncrementalFile(const BFileInfo &fileInfo, UniqueFd fd)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    if (!data.WriteParcelable(&fileInfo)) {
        HILOGE("Failed to send the fileInfo");
        return -EPIPE;
    }

    if (!data.WriteFileDescriptor(fd)) {
        HILOGE("Failed to send the fd");
        return -EPIPE;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_PUBLISH_SA_INCREMENTAL_FILE), data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::AppIncrementalFileReady(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd,
    int32_t errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    if (!data.WriteString(fileName)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the filename").GetCode();
    }
    bool fdFlag = (fd < 0 || manifestFd < 0) ? false : true;
    data.WriteBool(fdFlag);
    if (fdFlag == true && !data.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fd").GetCode();
    }
    if (fdFlag == true && !data.WriteFileDescriptor(manifestFd)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fd").GetCode();
    }
    if (!data.WriteInt32(errCode)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the errCode").GetCode();
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_INCREMENTAL_FILE_READY), data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::AppIncrementalDone(ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    if (!data.WriteInt32(errCode)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the errCode").GetCode();
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_INCREMENTAL_DONE),
                                        data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::GetIncrementalFileHandle(const std::string &bundleName, const std::string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    if (!data.WriteString(bundleName)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the bundleName").GetCode();
    }
    if (!data.WriteString(fileName)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fileName").GetCode();
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_INCREMENTAL_FILE_NAME), data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return ret;
}

template <typename T>
bool ServiceProxy::WriteParcelableVector(const std::vector<T> &parcelableVector, Parcel &data)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (!data.WriteUint32(parcelableVector.size())) {
        HILOGE("failed to WriteInt32 for parcelableVector.size()");
        return false;
    }

    for (const auto &parcelable : parcelableVector) {
        if (!data.WriteParcelable(&parcelable)) {
            HILOGE("failed to WriteParcelable for parcelable");
            return false;
        }
    }

    return true;
}

ErrCode ServiceProxy::Cancel(std::string bundleName, int32_t &result)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (!Remote()) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Remote is nullptr").GetCode();
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    if (!data.WriteString(bundleName)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the bundleName").GetCode();
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_CANCEL_BUNDLE), data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    reply.ReadInt32(result);
    HILOGI("ServiceProxy Cancel end, result:%{public}d", result);

    return BError(BError::Codes::OK, "success");
}
} // namespace OHOS::FileManagement::Backup