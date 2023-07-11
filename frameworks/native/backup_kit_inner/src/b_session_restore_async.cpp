/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
    auto proxy = ServiceProxy::GetInstance();
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
                                            RestoreTypeEnum restoreType,
                                            int32_t userId)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        workList_.push({move(remoteCap), move(bundlesToRestore), restoreType, userId});
    }

    if (isAppend_.exchange(true)) {
        return ERR_OK;
    } else {
        PopBundleInfo();
    }

    return ERR_OK;
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

void BSessionRestoreAsync::OnBackupServiceDied()
{
    HILOGE("Backup service died");
    if (callbacks_.onBackupServiceDied) {
        callbacks_.onBackupServiceDied();
    }
    deathRecipient_ = nullptr;
    ServiceProxy::InvaildInstance();
    PopBundleInfo();
}

void BSessionRestoreAsync::PopBundleInfo()
{
    AppendBundleInfo info;
    isAppend_.store(true);
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (workList_.empty()) {
            isAppend_.store(false);
            return;
        }
        info = move(workList_.front());
        workList_.pop();
    }
    AppendBundlesImpl(move(info));
}

void BSessionRestoreAsync::AppendBundlesImpl(AppendBundleInfo info)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    auto onBackupServiceDied = bind(&BSessionRestoreAsync::OnBackupServiceDied, shared_from_this());
    RegisterBackupServiceDied(onBackupServiceDied);

    BSessionRestore::Callbacks callbacksTmp {.onFileReady = callbacks_.onFileReady,
                                             .onBundleStarted = callbacks_.onBundleStarted,
                                             .onBundleFinished = callbacks_.onBundleFinished,
                                             .onAllBundlesFinished = callbacks_.onAllBundlesFinished,
                                             .onBackupServiceDied = onBackupServiceDied};
    int32_t res = proxy->InitRestoreSession(new ServiceReverse(callbacksTmp));
    if (res != 0) {
        HILOGE("Failed to Restore because of %{public}d", res);
        BError(BError::Codes::SDK_BROKEN_IPC, "Failed to int restore session").GetCode();
        return OnBundleStarted(res, info.bundlesToRestore);
    }
    res =
        proxy->AppendBundlesRestoreSession(move(info.remoteCap), info.bundlesToRestore, info.restoreType, info.userId);
    if (res != 0) {
        HILOGE("Failed to Restore because of %{public}d", res);
        BError(BError::Codes::SDK_BROKEN_IPC, "Failed to append bundles").GetCode();
        return OnBundleStarted(res, info.bundlesToRestore);
    }
}

void BSessionRestoreAsync::OnBundleStarted(ErrCode errCode, const vector<BundleName> &bundlesToRestore)
{
    for (auto &bundleName : bundlesToRestore) {
        if (callbacks_.onBundleStarted) {
            callbacks_.onBundleStarted(errCode, bundleName);
        }
    }
}
} // namespace OHOS::FileManagement::Backup