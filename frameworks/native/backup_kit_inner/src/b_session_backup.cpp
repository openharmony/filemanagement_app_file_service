/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include "service_client.h"
#include "service_reverse.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BSessionBackup::~BSessionBackup()
{
    HILOGE("BSessionBackup Destory");
    deathRecipient_ = nullptr;
}

unique_ptr<BSessionBackup> BSessionBackup::Init(Callbacks callbacks)
{
    try {
        HILOGI("Init BackupSession Begin");
        auto backup = make_unique<BSessionBackup>();
        ServiceClient::InvaildInstance();
        auto proxy = ServiceClient::GetInstance();
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

unique_ptr<BSessionBackup> BSessionBackup::Init(Callbacks callbacks,
                                                std::string &errMsg, ErrCode &errCode)
{
    try {
        HILOGI("Init BackupSession Begin");
        auto backup = make_unique<BSessionBackup>();
        ServiceClient::InvaildInstance();
        auto proxy = ServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOGI("Failed to get backup service");
            return nullptr;
        }
        proxy->InitBackupSessionWithErrMsg(sptr(new ServiceReverse(callbacks)), errCode, errMsg);
        if (errCode != ERR_OK) {
            HILOGE("Failed to Backup because of %{public}d, %{public}s", errCode, errMsg.c_str());
            AppRadar::Info info("", "", "");
            AppRadar::GetInstance().RecordBackupFuncRes(info, "BSessionBackup::Init",
                AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_CREATE_BACKUP_SESSION_FAIL, errCode);
            return nullptr;
        }
        backup->RegisterBackupServiceDied(callbacks.onBackupServiceDied);
        return backup;
    } catch (const exception &e) {
        HILOGE("Failed to Backup because of %{public}s", e.what());
        errCode = BError(BError::Codes::SDK_INVAL_ARG);
    }
    return nullptr;
}

void BSessionBackup::RegisterBackupServiceDied(std::function<void()> functor)
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
        ServiceClient::InvaildInstance();
        HILOGI("Backup service died");
        functor();
    };
    deathRecipient_ = sptr(new SvcDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient_);
}

ErrCode BSessionBackup::Start()
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->Start();
}

UniqueFd BSessionBackup::GetLocalCapabilities()
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

ErrCode BSessionBackup::GetBackupDataSize(bool isPreciseScan, vector<BIncrementalData> bundleNameList)
{
    HILOGI("GetBackupDataSize Begin");
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    ErrCode err = proxy->GetBackupDataSize(isPreciseScan, bundleNameList);
    if (err != ERR_OK) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to GetBackupDataSize").GetCode();
    }
    HILOGI("GetBackupDataSize end");
    return ERR_OK;
}

ErrCode BSessionBackup::AppendBundles(vector<BundleName> bundlesToBackup)
{
    auto proxy = ServiceClient::GetInstance();
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
    auto proxy = ServiceClient::GetInstance();
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
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->Finish();
}

ErrCode BSessionBackup::Release()
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->Release();
}

ErrCode BSessionBackup::Cancel(std::string bundleName)
{
    ErrCode result = BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK;
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Called Cancel, failed to get proxy.");
        return result;
    }

    proxy->CancelForResult(bundleName, result);
    return result;
}

ErrCode BSessionBackup::CleanBundleTempDir(const std::string &bundleName)
{
    HILOGI("BSessionBackup::CleanBundleTempDir");
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    return proxy->CleanBundleTempDir(bundleName);
}
} // namespace OHOS::FileManagement::Backup