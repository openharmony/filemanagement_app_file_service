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

#include "b_session_backup.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "service_proxy.h"
#include "service_reverse.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BSessionBackup::~BSessionBackup()
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

unique_ptr<BSessionBackup> BSessionBackup::Init(Callbacks callbacks)
{
    try {
        auto backup = make_unique<BSessionBackup>();
        ServiceProxy::InvaildInstance();
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            HILOGI("Failed to get backup service");
            return nullptr;
        }

        int32_t res = proxy->InitBackupSession(sptr(new ServiceReverse(callbacks)));
        if (res != 0) {
            HILOGE("Failed to Backup because of %{public}d", res);
            return nullptr;
        }

        backup->RegisterBackupServiceDied(callbacks.onBackupServiceDied);
        return backup;
    } catch (const exception &e) {
        HILOGE("Failed to Backup because of %{public}s", e.what());
    }
    return nullptr;
}

void BSessionBackup::RegisterBackupServiceDied(std::function<void()> functor)
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
        ServiceProxy::InvaildInstance();
        HILOGI("Backup service died");
        functor();
    };
    deathRecipient_ = sptr(new SvcDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient_);
}

ErrCode BSessionBackup::Start()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->Start();
}

ErrCode BSessionBackup::AppendBundles(vector<BundleName> bundlesToBackup)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->AppendBundlesBackupSession(bundlesToBackup);
}

ErrCode BSessionBackup::Finish()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->Finish();
}
} // namespace OHOS::FileManagement::Backup