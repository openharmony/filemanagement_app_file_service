/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "b_session_restore_async.h"

#include "b_error/b_error.h"
#include "b_radar/b_radar.h"
#include "b_resources/b_constants.h"
#include "b_session_restore.h"
#include "filemgmt_libhilog.h"
#include "service_proxy.h"
#include "service_reverse.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BSessionRestoreAsync::~BSessionRestoreAsync()
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
    callbacks_ = {};
    deathRecipient_ = nullptr;
}

shared_ptr<BSessionRestoreAsync> BSessionRestoreAsync::Init(Callbacks callbacks)
{
    try {
        auto restore = make_shared<BSessionRestoreAsync>(callbacks);
        ServiceProxy::InvaildInstance();
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            HILOGI("Failed to get backup service");
            return nullptr;
        }
        BSessionRestore::Callbacks callbacksTmp {.onFileReady = callbacks.onFileReady,
                                                 .onBundleStarted = callbacks.onBundleStarted,
                                                 .onBundleFinished = callbacks.onBundleFinished,
                                                 .onAllBundlesFinished = callbacks.onAllBundlesFinished,
                                                 .onResultReport = callbacks.onResultReport,
                                                 .onBackupServiceDied = callbacks.onBackupServiceDied,
                                                 .onProcess = callbacks.onProcess};
        int32_t res = proxy->InitRestoreSession(sptr(new ServiceReverse(callbacksTmp)));
        if (res != ERR_OK) {
            HILOGE("Failed to Restore because of %{public}d", res);
            AppRadar::Info info ("", "", "create restore session failed");
            AppRadar::GetInstance().RecordRestoreFuncRes(info, "BSessionRestoreAsync::Init",
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

ErrCode BSessionRestoreAsync::PublishFile(BFileInfo fileInfo)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    return proxy->PublishFile(fileInfo);
}

ErrCode BSessionRestoreAsync::GetFileHandle(const string &bundleName, const string &fileName)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->GetFileHandle(bundleName, fileName);
}

ErrCode BSessionRestoreAsync::AppendBundles(UniqueFd remoteCap,
                                            vector<BundleName> bundlesToRestore,
                                            std::vector<std::string> detailInfos,
                                            RestoreTypeEnum restoreType,
                                            int32_t userId)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    ErrCode res = proxy->AppendBundlesRestoreSession(move(remoteCap), bundlesToRestore, detailInfos, restoreType,
        userId);
    if (res != ERR_OK) {
        std::string ss;
        for (const auto &bundle : bundlesToRestore) {
            ss += bundle + ", ";
        }
        AppRadar::Info info(ss.c_str(), "", "AppendBundles with infos");
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "BSessionRestoreAsync::AppendBundles",
            AppRadar::GetInstance().GetUserId(), BizStageRestore::BIZ_STAGE_APPEND_BUNDLES_FAIL, res);
    }
    return res;
}

ErrCode BSessionRestoreAsync::AppendBundles(UniqueFd remoteCap,
                                            vector<BundleName> bundlesToRestore,
                                            RestoreTypeEnum restoreType,
                                            int32_t userId)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    ErrCode res = proxy->AppendBundlesRestoreSession(move(remoteCap), bundlesToRestore, restoreType, userId);
    if (res != ERR_OK) {
        std::string ss;
        for (const auto &bundle : bundlesToRestore) {
            ss += bundle + ", ";
        }
        AppRadar::Info info(ss.c_str(), "", "");
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "BSessionRestoreAsync::AppendBundles",
            AppRadar::GetInstance().GetUserId(), BizStageRestore::BIZ_STAGE_APPEND_BUNDLES_FAIL, res);
    }
    return res;
}

ErrCode BSessionRestoreAsync::Release()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->Release();
}

void BSessionRestoreAsync::RegisterBackupServiceDied(std::function<void()> functor)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr || !functor) {
        return;
    }
    auto remoteObj = proxy->AsObject();
    if (!remoteObj) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Proxy's remote object can't be nullptr");
    }

    auto callback = [functor](const wptr<IRemoteObject> &obj) { functor(); };
    deathRecipient_ = sptr(new SvcDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient_);
}

ErrCode BSessionRestoreAsync::Cancel(std::string bundleName)
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