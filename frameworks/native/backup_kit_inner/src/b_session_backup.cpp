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

#include "b_session_backup.h"

#include "b_error/b_error.h"
#include "b_radar/b_radar.h"
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

unique_ptr<BSessionBackup> BSessionBackup::Init(Callbacks callbacks)
{
    try {
        HILOGI("Init BackupSession Begin");
        auto backup = make_unique<BSessionBackup>();
        ServiceProxy::InvaildInstance();
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            HILOGI("Failed to get backup service");
            return nullptr;
        }

        int32_t res = proxy->InitBackupSession(sptr(new ServiceReverse(callbacks)));
        if (res != ERR_OK) {
            HILOGE("Failed to Backup because of %{public}d", res);
            AppRadar::Info info("", "", "");
            AppRadar::GetInstance().RecordBackupFuncRes(info, "BSessionBackup::Init",
                AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_CREATE_BACKUP_SESSION_FAIL, res);
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

    int32_t res = proxy->AppendBundlesBackupSession(bundlesToBackup);
    if (res != ERR_OK) {
        std::string ss;
        for (const auto &bundleName:bundlesToBackup) {
            ss += bundleName + ", ";
        }
        AppRadar::Info info(ss.c_str(), "", "");
        AppRadar::GetInstance().RecordBackupFuncRes(info, "BSessionBackup::AppendBundles",
            AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_APPEND_BUNDLES_FAIL, res);
    }
    return res;
}

ErrCode BSessionBackup::AppendBundles(vector<BundleName> bundlesToBackup, vector<std::string> detailInfos)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    int32_t res = proxy->AppendBundlesDetailsBackupSession(bundlesToBackup, detailInfos);
    if (res != ERR_OK) {
        std::string ss;
        for (const auto &bundleName:bundlesToBackup) {
            ss += bundleName + ", ";
        }
        AppRadar::Info info(ss.c_str(), "", "AppendBundles with infos");
        AppRadar::GetInstance().RecordBackupFuncRes(info, "BSessionBackup::AppendBundles",
            AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_APPEND_BUNDLES_FAIL, res);
    }
    return res;
}

ErrCode BSessionBackup::Finish()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->Finish();
}

ErrCode BSessionBackup::Release()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->Release();
}
} // namespace OHOS::FileManagement::Backup