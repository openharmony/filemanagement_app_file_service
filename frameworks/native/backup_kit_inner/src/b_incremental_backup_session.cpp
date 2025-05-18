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

#include "b_incremental_backup_session.h"

#include "b_error/b_error.h"
#include "b_radar/b_radar.h"
#include "filemgmt_libhilog.h"
#include "service_client.h"
#include "service_reverse.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BIncrementalBackupSession::~BIncrementalBackupSession()
{
    HILOGE("BIncrementalBackupSession Destory");
    deathRecipient_ = nullptr;
}

unique_ptr<BIncrementalBackupSession> BIncrementalBackupSession::Init(Callbacks callbacks)
{
    try {
        HILOGI("Init IncrementalBackupSession Begin");
        auto backup = make_unique<BIncrementalBackupSession>();
        ServiceClient::InvaildInstance();
        auto proxy = ServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOGI("Failed to get backup service");
            return nullptr;
        }

        int32_t res = proxy->InitIncrementalBackupSession(sptr(new ServiceReverse(callbacks)));
        if (res != ERR_OK) {
            HILOGE("Failed to Backup because of %{public}d", res);
            AppRadar::Info info("", "", "");
            AppRadar::GetInstance().RecordBackupFuncRes(info, "BIncrementalBackupSession::Init",
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

unique_ptr<BIncrementalBackupSession> BIncrementalBackupSession::Init(Callbacks callbacks,
                                                                      std::string &errMsg, ErrCode &errCode)
{
    try {
        HILOGI("Init IncrementalBackupSession Begin");
        auto backup = make_unique<BIncrementalBackupSession>();
        ServiceClient::InvaildInstance();
        auto proxy = ServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOGI("Failed to get backup service");
            return nullptr;
        }
        errCode = proxy->InitIncrementalBackupSessionWithErrMsg(sptr(new ServiceReverse(callbacks)), errMsg);
        if (errCode != ERR_OK) {
            HILOGE("Failed to Backup because of %{public}d", errCode);
            AppRadar::Info info("", "", "");
            AppRadar::GetInstance().RecordBackupFuncRes(info, "BIncrementalBackupSession::Init",
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

void BIncrementalBackupSession::RegisterBackupServiceDied(function<void()> functor)
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

UniqueFd BIncrementalBackupSession::GetLocalCapabilities()
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

ErrCode BIncrementalBackupSession::GetBackupDataSize(bool isPreciseScan, vector<BIncrementalData> bundleNameList)
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

ErrCode BIncrementalBackupSession::AppendBundles(vector<BIncrementalData> bundlesToBackup)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    ErrCode res = proxy->AppendBundlesIncrementalBackupSession(bundlesToBackup);
    if (res != ERR_OK) {
        std::string ss;
        for (const auto &bundle : bundlesToBackup) {
            ss += bundle.bundleName + ", ";
        }
        AppRadar::Info info(ss.c_str(), "", "");
        AppRadar::GetInstance().RecordBackupFuncRes(info, "BIncrementalBackupSession::AppendBundles",
            AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_APPEND_BUNDLES_FAIL, res);
    }
    return res;
}

ErrCode BIncrementalBackupSession::AppendBundles(vector<BIncrementalData> bundlesToBackup,
    std::vector<std::string> infos)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    int32_t res = proxy->AppendBundlesIncrementalBackupSessionWithBundleInfos(bundlesToBackup, infos);
    if (res != ERR_OK) {
        std::string ss;
        for (const auto &bundle : bundlesToBackup) {
            ss += bundle.bundleName + ", ";
        }
        AppRadar::Info info(ss.c_str(), "", "AppendBundles with infos");
        AppRadar::GetInstance().RecordBackupFuncRes(info, "BIncrementalBackupSession::AppendBundles",
            AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_APPEND_BUNDLES_FAIL, res);
    }
    return res;
}

ErrCode BIncrementalBackupSession::Release()
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }

    return proxy->Release();
}

ErrCode BIncrementalBackupSession::Cancel(std::string bundleName)
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

ErrCode BIncrementalBackupSession::CleanBundleTempDir(const std::string &bundleName)
{
    HILOGI("BIncrementalBackupSession::CleanBundleTempDir");
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to get backup service").GetCode();
    }
    return proxy->CleanBundleTempDir(bundleName);
}
} // namespace OHOS::FileManagement::Backup