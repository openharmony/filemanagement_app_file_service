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
    auto proxy = ServiceProxy::GetInstance();
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
        auto restore = make_unique<BIncrementalRestoreSession>();
        ServiceProxy::InvaildInstance();
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            HILOGI("Failed to get backup service");
            return nullptr;
        }
        int32_t res = proxy->InitRestoreSession(new ServiceReverse(callbacks));
        if (res != 0) {
            HILOGE("Failed to Restore because of %{public}d", res);
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

ErrCode BIncrementalRestoreSession::GetFileHandle(const string &bundleName, const string &fileName)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->GetIncrementalFileHandle(bundleName, fileName);
}

ErrCode BIncrementalRestoreSession::AppendBundlesDetails(UniqueFd remoteCap, vector<BundleName> bundlesToRestore,
    std::vector<std::string> detailInfos)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    return proxy->AppendBundlesDetailsRestoreSession(move(remoteCap), bundlesToRestore, detailInfos);
}

ErrCode BIncrementalRestoreSession::AppendBundles(UniqueFd remoteCap, vector<BundleName> bundlesToRestore)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->AppendBundlesRestoreSession(move(remoteCap), bundlesToRestore);
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
} // namespace OHOS::FileManagement::Backup