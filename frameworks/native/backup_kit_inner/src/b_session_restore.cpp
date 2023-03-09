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

#include "b_session_restore.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "service_proxy.h"
#include "service_reverse.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BSessionRestore::~BSessionRestore()
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

unique_ptr<BSessionRestore> BSessionRestore::Init(std::vector<BundleName> bundlesToRestore, Callbacks callbacks)
{
    try {
        auto restore = make_unique<BSessionRestore>();
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            HILOGI("Failed to get backup service");
            return nullptr;
        }
        int32_t res = proxy->InitRestoreSession(new ServiceReverse(callbacks), bundlesToRestore);
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

UniqueFd BSessionRestore::GetLocalCapabilities()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        HILOGI("Failed to proxy because of is empty");
        return UniqueFd(-1);
    }
    return UniqueFd(proxy->GetLocalCapabilities());
}

ErrCode BSessionRestore::PublishFile(BFileInfo fileInfo)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to proxy because of is empty").GetCode();
    }
    return proxy->PublishFile(fileInfo);
}

ErrCode BSessionRestore::Start()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to proxy because of is empty").GetCode();
    }

    return proxy->Start();
}

ErrCode BSessionRestore::GetExtFileName(string &bundleName, string &fileName)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to proxy because of is empty").GetCode();
    }

    return proxy->GetExtFileName(bundleName, fileName);
}

void BSessionRestore::RegisterBackupServiceDied(std::function<void()> functor)
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
        functor();
    };
    deathRecipient_ = sptr(new SvcDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient_);
}
} // namespace OHOS::FileManagement::Backup