/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "b_incremental_restore_session.h"

#include "b_error/b_error.h"
#include "b_radar/b_radar.h"
#include "filemgmt_libhilog.h"
#include "service_client.h"
#include "service_reverse.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BIncrementalRestoreSession::~BIncrementalRestoreSession()
{
    if (!deathRecipient_) {
        HILOGI("Death Recipient is nullptr");
        return;
    }
    auto proxy = ServiceClient::GetServiceProxyPointer();
    if (proxy == nullptr) {
        return;
    }
    auto remoteObject = proxy->AsObject();
    if (remoteObject != nullptr) {
        remoteObject->RemoveDeathRecipient(deathRecipient_);
    }
    deathRecipient_ = nullptr;
}

unique_ptr<BIncrementalRestoreSession> BIncrementalRestoreSession::Init(Callbacks callbacks)
{
    try {
        HILOGI("Init IncrementalRestoreSession Begin");
        auto restore = make_unique<BIncrementalRestoreSession>();
        ServiceClient::InvaildInstance();
        auto proxy = ServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOGI("Failed to get backup service");
            return nullptr;
        }
        int32_t res = proxy->InitRestoreSession(sptr(new ServiceReverse(callbacks)));
        if (res != ERR_OK) {
            HILOGE("Failed to Restore because of %{public}d", res);
            AppRadar::Info info ("", "", "create restore session failed");
            AppRadar::GetInstance().RecordRestoreFuncRes(info, "BIncrementalRestoreSession::Init",
                AppRadar::GetInstance().GetUserId(), BizStageRestore::BIZ_STAGE_CREATE_RESTORE_SESSION_FAIL, res);
            return nullptr;
        }

        restore->RegisterBackupServiceDied(callbacks.onBackupServiceDied);
        return restore;
    } catch (const exception &e) {
        HILOGE("Failed to Restore because of %{public}s", e.what());
    }
    return nullptr;
}

unique_ptr<BIncrementalRestoreSession> BIncrementalRestoreSession::Init(Callbacks callbacks,
                                                                        std::string &errMsg, ErrCode &errCode)
{
    try {
        HILOGI("Init IncrementalRestoreSession Begin");
        auto restore = make_unique<BIncrementalRestoreSession>();
        ServiceClient::InvaildInstance();
        auto proxy = ServiceClient::GetInstance();
        if (proxy == nullptr) {
            errMsg = "Failed to get backup service";
            errCode = BError(BError::Codes::SDK_BROKEN_IPC);
            HILOGE("Init IncrementalRestoreSession failed, %{public}s", errMsg.c_str());
            return nullptr;
        }
        errCode = proxy->InitRestoreSessionWithErrMsg(sptr(new ServiceReverse(callbacks)), errMsg);
        if (errCode != ERR_OK) {
            HILOGE("Failed to Restore because of %{public}d", errCode);
            AppRadar::Info info ("", "", "create restore session failed");
            AppRadar::GetInstance().RecordRestoreFuncRes(info, "BIncrementalRestoreSession::Init",
                AppRadar::GetInstance().GetUserId(), BizStageRestore::BIZ_STAGE_CREATE_RESTORE_SESSION_FAIL, errCode);
            return nullptr;
        }

        restore->RegisterBackupServiceDied(callbacks.onBackupServiceDied);
        return restore;
    } catch (const exception &e) {
        HILOGE("Failed to Restore because of %{public}s", e.what());
        errCode = BError(BError::Codes::SDK_INVAL_ARG);
    }
    return nullptr;
}

UniqueFd BIncrementalRestoreSession::GetLocalCapabilities()
{
    HILOGI("GetLocalCapabilities begin");
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return UniqueFd(-EPERM);
    }
    int fdvalue = INVALID_FD;
    proxy->GetLocalCapabilitiesForBundleInfos(fdvalue);
    UniqueFd fd(fdvalue);
    if (fd < 0) {
        HILOGE("Failed to get local capabilities for bundleinfos");
        return UniqueFd(-EPERM);
    }
    return fd;
}

ErrCode BIncrementalRestoreSession::PublishFile(BFileInfo fileInfo)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    return proxy->PublishIncrementalFile(fileInfo);
}

ErrCode BIncrementalRestoreSession::PublishSAFile(BFileInfo fileInfo, UniqueFd fd)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    int fdValue = fd.Get();
    return proxy->PublishSAIncrementalFile(fileInfo, fdValue);
}

ErrCode BIncrementalRestoreSession::GetFileHandle(const string &bundleName, const string &fileName)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->GetIncrementalFileHandle(bundleName, fileName);
}

ErrCode BIncrementalRestoreSession::AppendBundles(UniqueFd remoteCap, vector<BundleName> bundlesToRestore)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    int32_t remoteCapInt = remoteCap.Get();
    ErrCode res = proxy->AppendBundlesRestoreSessionData(remoteCapInt, bundlesToRestore, 0, -1);
    if (res != ERR_OK) {
        std::string ss;
        for (const auto &bundle : bundlesToRestore) {
            ss += bundle + ", ";
        }
        AppRadar::Info info(ss.c_str(), "", "");
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "BIncrementalRestoreSession::AppendBundles",
                                                     AppRadar::GetInstance().GetUserId(),
                                                     BizStageRestore::BIZ_STAGE_APPEND_BUNDLES_FAIL, res);
    }
    return res;
}

ErrCode BIncrementalRestoreSession::AppendBundles(UniqueFd remoteCap,
                                                  vector<BundleName> bundlesToRestore,
                                                  std::vector<std::string> detailInfos)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    int32_t remoteCapInt = remoteCap.Get();
    ErrCode res = proxy->AppendBundlesRestoreSessionDataByDetail(remoteCapInt, bundlesToRestore, detailInfos,
                                                                 DEFAULT_RESTORE_TYPE, DEFAULT_USER_ID);
    if (res != ERR_OK) {
        std::string ss;
        for (const auto &bundle : bundlesToRestore) {
            ss += bundle + ", ";
        }
        AppRadar::Info info(ss.c_str(), "", "AppendBundles with infos");
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "BIncrementalRestoreSession::AppendBundles",
                                                     AppRadar::GetInstance().GetUserId(),
                                                     BizStageRestore::BIZ_STAGE_APPEND_BUNDLES_FAIL, res);
    }
    return res;
}

ErrCode BIncrementalRestoreSession::Release()
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->Release();
}

void BIncrementalRestoreSession::RegisterBackupServiceDied(function<void()> functor)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr || !functor) {
        return;
    }
    auto remoteObj = proxy->AsObject();
    if (!remoteObj) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Proxy's remote object can't be nullptr");
    }

    auto callback = [functor](const wptr<IRemoteObject> &obj) {
        HILOGI("Backup service died");
        ServiceClient::InvaildInstance();
        functor();
    };
    deathRecipient_ = sptr(new SvcDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient_);
}

ErrCode BIncrementalRestoreSession::Cancel(std::string bundleName)
{
    ErrCode result = BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK;
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Called Cancel, failed to get proxy.");
        return result;
    }

    ErrCode errCode = proxy->Cancel(bundleName, result);
    if (errCode != 0) {
        HILOGE("proxy->Cancel failed, errCode:%{public}d.", errCode);
        return result;
    }
    return result;
}
} // namespace OHOS::FileManagement::Backup