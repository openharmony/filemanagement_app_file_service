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

#include "b_incremental_restore_session.h"

#include "b_error/b_error.h"
#include "b_radar/b_radar.h"
#include "filemgmt_libhilog.h"
#include "service_proxy.h"
#include "service_reverse.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BIncrementalRestoreSession::~BIncrementalRestoreSession()
{
    if (!deathRecipient_) {
        HILOGI("Death Recipient is nullptr");
        return;
    }
    auto proxy = ServiceProxy::GetServiceProxyPointer();
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
        ServiceProxy::InvaildInstance();
        auto proxy = ServiceProxy::GetInstance();
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

ErrCode BIncrementalRestoreSession::PublishFile(BFileInfo fileInfo)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    return proxy->PublishIncrementalFile(fileInfo);
}

ErrCode BIncrementalRestoreSession::PublishSAFile(BFileInfo fileInfo, UniqueFd fd)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    return proxy->PublishSAIncrementalFile(fileInfo, move(fd));
}

ErrCode BIncrementalRestoreSession::GetFileHandle(const string &bundleName, const string &fileName)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->GetIncrementalFileHandle(bundleName, fileName);
}

ErrCode BIncrementalRestoreSession::AppendBundles(UniqueFd remoteCap, vector<BundleName> bundlesToRestore)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    ErrCode res = proxy->AppendBundlesRestoreSession(move(remoteCap), bundlesToRestore);
    if (res != ERR_OK) {
        std::string ss;
        for (const auto &bundle : bundlesToRestore) {
            ss += bundle + ", ";
        }
        AppRadar::Info info(ss.c_str(), "", "");
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "BIncrementalRestoreSession::AppendBundles",
            AppRadar::GetInstance().GetUserId(), BizStageRestore::BIZ_STAGE_APPEND_BUNDLES_FAIL, res);
    }
    return res;
}

ErrCode BIncrementalRestoreSession::AppendBundles(UniqueFd remoteCap, vector<BundleName> bundlesToRestore,
    std::vector<std::string> detailInfos)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    ErrCode res = proxy->AppendBundlesRestoreSession(move(remoteCap), bundlesToRestore, detailInfos);
    if (res != ERR_OK) {
        std::string ss;
        for (const auto &bundle : bundlesToRestore) {
            ss += bundle + ", ";
        }
        AppRadar::Info info(ss.c_str(), "", "AppendBundles with infos");
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "BIncrementalRestoreSession::AppendBundles",
            AppRadar::GetInstance().GetUserId(), BizStageRestore::BIZ_STAGE_APPEND_BUNDLES_FAIL, res);
    }
    return res;
}

ErrCode BIncrementalRestoreSession::Release()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->Release();
}

void BIncrementalRestoreSession::RegisterBackupServiceDied(function<void()> functor)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr || !functor) {
        return;
    }
    auto remoteObj = proxy->AsObject();
    if (!remoteObj) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Proxy's remote object can't be nullptr");
    }

    auto callback = [functor](const wptr<IRemoteObject> &obj) {
        HILOGI("Backup service died");
        ServiceProxy::InvaildInstance();
        functor();
    };
    deathRecipient_ = sptr(new SvcDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient_);
}

ErrCode BIncrementalRestoreSession::Cancel(std::string bundleName)
{
    ErrCode result = BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK;
    auto proxy = ServiceProxy::GetInstance();
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