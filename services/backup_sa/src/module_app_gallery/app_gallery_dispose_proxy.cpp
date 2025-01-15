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

#include <string>

#include "b_error/b_error.h"
#include "b_radar/b_radar.h"
#include "b_sa/b_sa_utils.h"
#include "b_jsonutil/b_jsonutil.h"
#include "filemgmt_libhilog.h"
#include "message_parcel.h"

#include "module_app_gallery/app_gallery_dispose_proxy.h"
#include "module_app_gallery/app_gallery_service_connection.h"
#include "want.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
    const auto APP_FOUNDATION_SERVICE = u"appgalleryservice.openapi.privacymanager.AppFoundationService";
}
mutex AppGalleryDisposeProxy::instanceLock_;
mutex AppGalleryDisposeProxy::appGalleryConnectionLock_;

string AppGalleryDisposeProxy::abilityName = "AppFoundationService";
std::map<int32_t, sptr<AppGalleryConnection>> AppGalleryDisposeProxy::appGalleryConnectionMap_;
sptr<AppGalleryDisposeProxy> AppGalleryDisposeProxy::appGalleryDisposeProxyInstance_;

AppGalleryDisposeProxy::AppGalleryDisposeProxy()
{
    HILOGI("AppGalleryDisposeProxy construct");
}

AppGalleryDisposeProxy::~AppGalleryDisposeProxy()
{
    appGalleryDisposeProxyInstance_ = nullptr;
}

sptr<AppGalleryDisposeProxy> AppGalleryDisposeProxy::GetInstance()
{
    if (appGalleryDisposeProxyInstance_ == nullptr) {
        lock_guard<mutex> autoLock(instanceLock_);
        if (appGalleryDisposeProxyInstance_ == nullptr) {
            appGalleryDisposeProxyInstance_ = new AppGalleryDisposeProxy;
        }
    }

    return appGalleryDisposeProxyInstance_;
}

DisposeErr AppGalleryDisposeProxy::StartBackup(const std::string &bundleName, const int32_t userId)
{
    HILOGI("StartBackup, app %{public}s, userId %{public}d", bundleName.c_str(), userId);
    DisposeErr res = DoDispose(bundleName, DisposeOperation::START_BACKUP, userId);
    if (res != DisposeErr::REQUEST_FAIL) {
        AppRadar::Info info(bundleName, "", "");
        AppRadar::GetInstance().RecordBackupFuncRes(info, "StartBackup", userId,
            BizStageBackup::BIZ_STAGE_START_DISPOSE, static_cast<int32_t>(res));
    }
    return res;
}

DisposeErr AppGalleryDisposeProxy::EndBackup(const std::string &bundleName, const int32_t userId)
{
    HILOGI("EndBackup, app %{public}s, userId %{public}d", bundleName.c_str(), userId);
    DisposeErr res = DoDispose(bundleName, DisposeOperation::END_BACKUP, userId);
    if (res != DisposeErr::REQUEST_FAIL) {
        AppRadar::Info info(bundleName, "", "");
        AppRadar::GetInstance().RecordBackupFuncRes(info, "EndBackup", userId,
            BizStageBackup::BIZ_STAGE_END_DISPOSE, static_cast<int32_t>(res));
    }
    return res;
}

DisposeErr AppGalleryDisposeProxy::StartRestore(const std::string &bundleName, const int32_t userId)
{
    if (SAUtils::IsSABundleName(bundleName)) {
        HILOGI("SA does not need to StartRestore");
        return DisposeErr::OK;
    }
    HILOGI("StartRestore, app %{public}s, userId %{public}d", bundleName.c_str(), userId);
    DisposeErr res = DoDispose(bundleName, DisposeOperation::START_RESTORE, userId);
    if (res != DisposeErr::REQUEST_FAIL) {
        AppRadar::Info info(bundleName, "", "");
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "StartRestore", userId,
            BizStageRestore::BIZ_STAGE_START_DISPOSE, static_cast<int32_t>(res));
    }
    return res;
}

DisposeErr AppGalleryDisposeProxy::EndRestore(const std::string &bundleName, const int32_t userId)
{
    if (SAUtils::IsSABundleName(bundleName)) {
        HILOGI("SA does not need to EndRestore");
        return DisposeErr::OK;
    }
    HILOGI("EndRestore, app %{public}s, userId %{public}d", bundleName.c_str(), userId);
    DisposeErr res = DoDispose(bundleName, DisposeOperation::END_RESTORE, userId);
    if (res != DisposeErr::REQUEST_FAIL) {
        AppRadar::Info info(bundleName, "", "");
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "EndRestore", userId,
            BizStageRestore::BIZ_STAGE_END_DISPOSE, static_cast<int32_t>(res));
    }
    return res;
}

void RecordDoDisposeRes(const std::string &bundleName,
                        AppGalleryDisposeProxy::DisposeOperation disposeOperation, int32_t userId, int32_t err)
{
    AppRadar::Info info (bundleName, "", "REQUEST FAIL");
    switch (disposeOperation) {
        case AppGalleryDisposeProxy::DisposeOperation::START_BACKUP:
            AppRadar::GetInstance().RecordBackupFuncRes(info, "StartBackup", userId,
                                                        BizStageBackup::BIZ_STAGE_START_DISPOSE, err);
            break;
        case AppGalleryDisposeProxy::DisposeOperation::END_BACKUP:
            AppRadar::GetInstance().RecordBackupFuncRes(info, "EndBackup", userId,
                                                        BizStageBackup::BIZ_STAGE_END_DISPOSE, err);
            break;
        case AppGalleryDisposeProxy::DisposeOperation::START_RESTORE:
            AppRadar::GetInstance().RecordRestoreFuncRes(info, "StartRestore", userId,
                                                         BizStageRestore::BIZ_STAGE_START_DISPOSE, err);
            break;
        case AppGalleryDisposeProxy::DisposeOperation::END_RESTORE:
            AppRadar::GetInstance().RecordRestoreFuncRes(info, "EndRestore", userId,
                                                         BizStageRestore::BIZ_STAGE_END_DISPOSE, err);
            break;
        default:
            break;
    }
}

sptr<AppGalleryConnection> AppGalleryDisposeProxy::GetAppGalleryConnection(const int32_t userId)
{
    auto it = appGalleryConnectionMap_.find(userId);
    if (it == appGalleryConnectionMap_.end()) {
        HILOGI("appGalleryConnectionMap not contain %{public}d, will register", userId);
        return nullptr;
    }
    HILOGI("appGalleryConnectionMap contain %{public}d", userId);
    return appGalleryConnectionMap_[userId];
}

sptr<AppGalleryConnection> AppGalleryDisposeProxy::ConnectAppGallery(const int32_t userId)
{
    std::unique_lock<std::mutex> lock(appGalleryConnectionLock_);
    sptr<AppGalleryConnection> connection = GetAppGalleryConnection(userId);
    if (connection == nullptr) {
        connection = sptr(new AppGalleryConnection(userId));
        appGalleryConnectionMap_[userId] = connection;
    }
    if (connection->GetRemoteObj() == nullptr) {
        HILOGI("AppGalleryConnection try to connect, userId = %{public}d", userId);
        if (!connection->ConnectExtAbility(abilityName) || connection->GetRemoteObj() == nullptr) {
            HILOGE("AppGalleryConnection failed to connect, userId = %{public}d", userId);
            return nullptr;
        }
    }
    return connection;
}

DisposeErr AppGalleryDisposeProxy::DoDispose(const std::string &bundleName, DisposeOperation disposeOperation,
    const int32_t userId)
{
    try {
        HILOGI("DoDispose, app %{public}s, operation %{public}d, userId %{public}d", bundleName.c_str(),
            disposeOperation, userId);
        sptr<AppGalleryConnection> connection = ConnectAppGallery(userId);
        if (connection == nullptr) {
            HILOGE("Get AppGalleryConnection failed, userId = %{public}d", userId);
            return DisposeErr::CONN_FAIL;
        }

        BJsonUtil::BundleDetailInfo bundleDetailInfo = BJsonUtil::ParseBundleNameIndexStr(bundleName);
        MessageParcel data;
        const auto interfaceToken = APP_FOUNDATION_SERVICE;
        if (!data.WriteInterfaceToken(interfaceToken)) {
            HILOGE("write WriteInterfaceToken failed");
            return DisposeErr::IPC_FAIL;
        }
        if (!data.WriteString16(Str8ToStr16(bundleDetailInfo.bundleName))) {
            HILOGE("write bundleName failed");
            return DisposeErr::IPC_FAIL;
        }
        if (!data.WriteInt32(static_cast<int32_t>(bundleDetailInfo.bundleIndex))) {
            HILOGE("write bundleIndex failed");
            return DisposeErr::IPC_FAIL;
        }

        MessageParcel reply;
        MessageOption option;
        auto remoteObj = connection->GetRemoteObj();
        if (remoteObj == nullptr) {
            HILOGE("remoteObj is nullptr, bundleName=%{public}s, appindex=%{public}d, userId=%{public}d",
                bundleDetailInfo.bundleName.c_str(), bundleDetailInfo.bundleIndex, userId);
            return DisposeErr::CONN_FAIL;
        }

        int32_t ret = remoteObj->SendRequest(static_cast<int>(disposeOperation), data, reply, option);
        if (ret != ERR_NONE) {
            HILOGE("SendRequest error, code=%{public}d, bundleName=%{public}s , appindex=%{public}d, userId=%{public}d",
                ret, bundleDetailInfo.bundleName.c_str(), bundleDetailInfo.bundleIndex, userId);
            RecordDoDisposeRes(bundleName, disposeOperation, userId, ret);
            return DisposeErr::REQUEST_FAIL;
        }

        HILOGI("SendRequest success, dispose=%{public}d, bundleName=%{public}s, appindex=%{public}d, userId=%{public}d",
            disposeOperation, bundleDetailInfo.bundleName.c_str(), bundleDetailInfo.bundleIndex, userId);
        return DisposeErr::OK;
    } catch (const BError &e) {
        HILOGE("Catch exception, errCode = %{public}d", e.GetCode());
        return DisposeErr::IPC_FAIL;
    } catch (...) {
        HILOGE("Unexpected exception");
        return DisposeErr::IPC_FAIL;
    }
}

} // namespace OHOS::FileManagement::Backup