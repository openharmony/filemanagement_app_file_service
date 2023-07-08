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

#include "service_proxy.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ErrCode ServiceProxy::InitRestoreSession(sptr<IServiceReverse> remote)
{
    HILOGI("Begin");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    MessageParcel reply;
    MessageOption option;

    if (!remote) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Empty reverse stub").GetCode();
    }
    if (!data.WriteRemoteObject(remote->AsObject().GetRefPtr())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the reverse stub").GetCode();
    }

    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_RESTORE_SESSION),
                                        data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::InitBackupSession(sptr<IServiceReverse> remote)
{
    HILOGI("Begin");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    MessageParcel reply;
    MessageOption option;

    if (!remote) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Empty reverse stub").GetCode();
    }
    if (!data.WriteRemoteObject(remote->AsObject().GetRefPtr())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the reverse stub").GetCode();
    }

    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_BACKUP_SESSION),
                                        data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::Start()
{
    HILOGI("Begin");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_START), data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

UniqueFd ServiceProxy::GetLocalCapabilities()
{
    HILOGI("Begin");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        HILOGE("Failed to write descriptor");
        return UniqueFd(-EPERM);
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_LOCAL_CAPABILITIES), data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return UniqueFd(-ret);
    }
    UniqueFd fd(reply.ReadFileDescriptor());
    return UniqueFd(fd.Release());
}

ErrCode ServiceProxy::PublishFile(const BFileInfo &fileInfo)
{
    HILOGI("Begin");
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
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_PUBLISH_FILE), data,
                                        reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::AppFileReady(const string &fileName, UniqueFd fd)
{
    HILOGI("Begin");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    if (!data.WriteString(fileName)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the filename").GetCode();
    }
    if (!data.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fd").GetCode();
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_FILE_READY), data,
                                        reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::AppDone(ErrCode errCode)
{
    HILOGI("Begin");
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
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_DONE), data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::GetFileHandle(const string &bundleName, const string &fileName)
{
    HILOGI("Begin");
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
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_FILE_NAME), data,
                                        reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return ret;
}

ErrCode ServiceProxy::AppendBundlesRestoreSession(UniqueFd fd, const vector<BundleName> &bundleNames)
{
    HILOGI("Begin");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fd").GetCode();
    }
    if (!data.WriteStringVector(bundleNames)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleNames").GetCode();
    }

    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION), data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::AppendBundlesBackupSession(const vector<BundleName> &bundleNames)
{
    HILOGI("Begin");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteStringVector(bundleNames)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleNames").GetCode();
    }

    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_BACKUP_SESSION), data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::Finish()
{
    HILOGI("Begin");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_FINISH), data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

sptr<IService> ServiceProxy::GetInstance()
{
    unique_lock<mutex> lock(proxyMutex_);
    if (serviceProxy_ != nullptr) {
        return serviceProxy_;
    }

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOGE("Get an empty samgr");
        return nullptr;
    }
    sptr<ServiceProxyLoadCallback> loadCallback = new ServiceProxyLoadCallback();
    if (loadCallback == nullptr) {
        HILOGE("loadCallback is nullptr.");
        return nullptr;
    }
    int32_t ret = samgr->LoadSystemAbility(FILEMANAGEMENT_BACKUP_SERVICE_SA_ID, loadCallback);
    if (ret != ERR_OK) {
        HILOGE("Failed to Load systemAbility, systemAbilityId:%d, ret code:%d", FILEMANAGEMENT_BACKUP_SERVICE_SA_ID,
               ret);
        return nullptr;
    }

    auto waitStatus =
        loadCallback->proxyConVar_.wait_for(lock, std::chrono::milliseconds(BConstants::BACKUP_LOADSA_TIMEOUT_MS),
                                            [loadCallback]() { return loadCallback->isLoadSuccess_.load(); });
    if (!waitStatus) {
        HILOGE("Load backup sa timeout");
        return nullptr;
    }
    return serviceProxy_;
}

void ServiceProxy::InvaildInstance()
{
    HILOGI("invalid instance");
    unique_lock<mutex> lock(proxyMutex_);
    serviceProxy_ = nullptr;
}

void ServiceProxy::ServiceProxyLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
                                                                        const OHOS::sptr<IRemoteObject> &remoteObject)
{
    HILOGI("Load backup sa success, systemAbilityId:%d, remoteObject result:%s", systemAbilityId,
           (remoteObject != nullptr) ? "true" : "false");
    unique_lock<mutex> lock(proxyMutex_);
    serviceProxy_ = iface_cast<IService>(remoteObject);
    isLoadSuccess_.store(true);
    proxyConVar_.notify_one();
}

void ServiceProxy::ServiceProxyLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    HILOGE("Load backup sa failed, systemAbilityId:%d", systemAbilityId);
    unique_lock<mutex> lock(proxyMutex_);
    serviceProxy_ = nullptr;
    isLoadSuccess_.store(false);
    proxyConVar_.notify_one();
}
} // namespace OHOS::FileManagement::Backup