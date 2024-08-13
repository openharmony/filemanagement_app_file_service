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

#include "service_proxy.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "svc_death_recipient.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ErrCode ServiceProxy::InitRestoreSession(sptr<IServiceReverse> remote)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
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
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
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
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
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
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        HILOGE("Failed to write descriptor");
        return UniqueFd(-EPERM);
    }

    MessageParcel reply;
    MessageOption option;
    option.SetWaitTime(BConstants::IPC_MAX_WAIT_TIME);
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
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_PUBLISH_FILE), data,
                                        reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::AppFileReady(const string &fileName, UniqueFd fd, int32_t errCode)
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
    bool fdFlag = fd < 0 ? false : true;
    data.WriteBool(fdFlag);
    if (fdFlag == true && !data.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fd").GetCode();
    }
    if (!data.WriteInt32(errCode)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the errCode").GetCode();
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
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_DONE), data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::ServiceResultReport(const std::string restoreRetInfo,
    BackupRestoreScenario scenario, ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    if (!data.WriteString(restoreRetInfo)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the restoreRetInfo").GetCode();
    }
    if (!data.WriteInt32(static_cast<int32_t>(scenario))) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the scenario").GetCode();
    }
    if (!data.WriteInt32(errCode)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the errCode").GetCode();
    }
    MessageParcel reply;
    MessageOption option;
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_RESULT_REPORT), data, reply,
            option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::GetFileHandle(const string &bundleName, const string &fileName)
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
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_FILE_NAME), data,
                                        reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return ret;
}

ErrCode ServiceProxy::AppendBundlesRestoreSession(UniqueFd fd, const vector<BundleName> &bundleNames,
    const std::vector<std::string> &detailInfos, RestoreTypeEnum restoreType, int32_t userId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    MessageParcel reply;
    MessageOption option;
    option.SetWaitTime(BConstants::IPC_MAX_WAIT_TIME);

    if (!data.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fd").GetCode();
    }
    if (!data.WriteStringVector(bundleNames)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleNames").GetCode();
    }
    if (!detailInfos.empty() && !data.WriteStringVector(detailInfos)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send detailInfos").GetCode();
    }
    if (!data.WriteInt32(static_cast<int32_t>(restoreType))) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send restoreType").GetCode();
    }
    if (!data.WriteInt32(userId)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send userId").GetCode();
    }
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION_DETAILS), data, reply,
        option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::AppendBundlesRestoreSession(UniqueFd fd,
                                                  const vector<BundleName> &bundleNames,
                                                  RestoreTypeEnum restoreType,
                                                  int32_t userId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    MessageParcel reply;
    MessageOption option;
    option.SetWaitTime(BConstants::IPC_MAX_WAIT_TIME);

    if (!data.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fd").GetCode();
    }
    if (!data.WriteStringVector(bundleNames)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleNames").GetCode();
    }
    if (!data.WriteInt32(static_cast<int32_t>(restoreType))) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send restoreType").GetCode();
    }
    if (!data.WriteInt32(userId)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send userId").GetCode();
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
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    MessageParcel reply;
    MessageOption option;
    option.SetWaitTime(BConstants::IPC_MAX_WAIT_TIME);

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

ErrCode ServiceProxy::AppendBundlesDetailsBackupSession(const vector<BundleName> &bundleNames,
                                                        const vector<std::string> &detailInfos)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    MessageParcel reply;
    MessageOption option;
    option.SetWaitTime(BConstants::IPC_MAX_WAIT_TIME);

    if (!data.WriteStringVector(bundleNames)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleNames").GetCode();
    }

    if (!data.WriteStringVector(detailInfos)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send detailInfos").GetCode();
    }

    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_BACKUP_SESSION_DETAILS),
        data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    return reply.ReadInt32();
}

ErrCode ServiceProxy::Finish()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
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
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    unique_lock<mutex> getInstanceLock(getInstanceMutex_);
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
        HILOGE("Failed to Load systemAbility, systemAbilityId:%{private}d, ret code:%{public}d",
               FILEMANAGEMENT_BACKUP_SERVICE_SA_ID, ret);
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
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGD("invalid instance");
    unique_lock<mutex> lock(proxyMutex_);
    serviceProxy_ = nullptr;
}

void ServiceProxy::ServiceProxyLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
                                                                        const OHOS::sptr<IRemoteObject> &remoteObject)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Load backup sa success, systemAbilityId: %{private}d, remoteObject result:%{private}s", systemAbilityId,
           (remoteObject != nullptr) ? "true" : "false");
    if (systemAbilityId != FILEMANAGEMENT_BACKUP_SERVICE_SA_ID || remoteObject == nullptr) {
        isLoadSuccess_.store(false);
        proxyConVar_.notify_one();
        return;
    }
    unique_lock<mutex> lock(proxyMutex_);
    serviceProxy_ = iface_cast<IService>(remoteObject);
    if (serviceProxy_ == nullptr) {
        HILOGD("serviceProxy_ is nullptr");
        return;
    }
    auto remoteObj = serviceProxy_->AsObject();
    if (!remoteObj) {
        HILOGE("Failed to get remote object");
        serviceProxy_ = nullptr;
        isLoadSuccess_.store(false);
        proxyConVar_.notify_one();
        return;
    }

    auto callback = [](const wptr<IRemoteObject> &obj) {
        ServiceProxy::InvaildInstance();
        HILOGE("Backup service died");
    };
    sptr<SvcDeathRecipient> deathRecipient = sptr(new SvcDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient);
    isLoadSuccess_.store(true);
    proxyConVar_.notify_one();
}

void ServiceProxy::ServiceProxyLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGE("Load backup sa failed, systemAbilityId:%{private}d", systemAbilityId);
    unique_lock<mutex> lock(proxyMutex_);
    serviceProxy_ = nullptr;
    isLoadSuccess_.store(false);
    proxyConVar_.notify_one();
}

ErrCode ServiceProxy::GetBackupInfo(BundleName &bundleName, std::string &result)
{
    HILOGI("ServiceProxy GetBackupInfo Begin.");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    if (!data.WriteString(bundleName)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleName").GetCode();
    }
    MessageParcel reply;
    MessageOption option;
    option.SetWaitTime(BConstants::IPC_MAX_WAIT_TIME);
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_BACKUP_INFO),
                                        data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    reply.ReadString(result);
    HILOGI("ServiceProxy GetBackupInfo end. result = %s", result.c_str());
    return BError(BError::Codes::OK, "success");
}

ErrCode ServiceProxy::UpdateTimer(BundleName &bundleName, uint32_t timeOut, bool &result)
{
    HILOGI("ServiceProxy UpdateTimer Begin.");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    if (!data.WriteString(bundleName)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleName").GetCode();
    }
    if (!data.WriteUint32(timeOut)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send timeOut").GetCode();
    }
    MessageParcel reply;
    MessageOption option;
    option.SetWaitTime(BConstants::IPC_MAX_WAIT_TIME);
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_UPDATE_TIMER),
                                        data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    reply.ReadBool(result);
    HILOGI("ServiceProxy UpdateTimer end. result = %d", result);
    return BError(BError::Codes::OK, "success");
}

ErrCode ServiceProxy::UpdateSendRate(std::string &bundleName, int32_t sendRate, bool &result)
{
    HILOGD("ServiceProxy UpdateSendRate Begin.");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    if (!data.WriteString(bundleName)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleName").GetCode();
    }
    if (!data.WriteInt32(sendRate)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send sendRate").GetCode();
    }
    MessageParcel reply;
    MessageOption option;
    option.SetWaitTime(BConstants::IPC_MAX_WAIT_TIME);
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_UPDATE_SENDRATE),
                                        data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    reply.ReadBool(result);
    HILOGI("ServiceProxy UpdateSendRate end. ret = %{public}d", ret);
    return BError(BError::Codes::OK, "success");
}

ErrCode ServiceProxy::ReportAppProcessInfo(const std::string processInfo, const BackupRestoreScenario sennario)
{
    HILOGD("ServiceProxy NotifyBundleProcessInfo Begin.");
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to write descriptor").GetCode();
    }
    if (!data.WriteString(processInfo)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleName").GetCode();
    }
    if (!data.WriteInt32(static_cast<int32_t>(scenario))) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the scenario").GetCode();
    }
    MessageParcel reply;
    MessageOption option;
    option.SetWaitTime(BConstants::IPC_MAX_WAIT_TIME);
    int32_t ret = Remote()-> SendRequest(static_cast<uint32_t>(
        IServiceInterfaceCode::SERVICE_CMD_NOTIFY_BUNDLE_PROCESS_INFO), data, reply, option);
    if (ret != NO_ERROR) {
        string str = "Failed to send out the request because of " + to_string(ret);
        return BError(BError::Codes::SDK_INVAL_ARG, str.data()).GetCode();
    }
    HILOGI("ServiceProxy NotifyBundleProcessInfo end. ret = %{public}d", ret);
    return BError(BError::Codes::OK, "success");
}
} // namespace OHOS::FileManagement::Backup
