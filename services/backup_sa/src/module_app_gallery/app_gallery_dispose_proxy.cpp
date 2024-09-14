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
mutex AppGalleryDisposeProxy::conditionMutex_;

string AppGalleryDisposeProxy::abilityName = "AppFoundationService";
sptr<IRemoteObject> AppGalleryDisposeProxy::appRemoteObj_;
condition_variable AppGalleryDisposeProxy::conditionVal_;
mutex AppGalleryDisposeProxy::appRemoteObjLock_;
mutex AppGalleryDisposeProxy::connectMutex;
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

DisposeErr AppGalleryDisposeProxy::StartBackup(const std::string &bundleName)
{
    HILOGI("StartBackup, app %{public}s", bundleName.c_str());
    return DoDispose(bundleName, DisposeOperation::START_BACKUP);
}

DisposeErr AppGalleryDisposeProxy::EndBackup(const std::string &bundleName)
{
    HILOGI("EndBackup, app %{public}s", bundleName.c_str());
    return DoDispose(bundleName, DisposeOperation::END_BACKUP);
}

DisposeErr AppGalleryDisposeProxy::StartRestore(const std::string &bundleName)
{
    if (SAUtils::IsSABundleName(bundleName)) {
        HILOGI("SA does not need to StartRestore");
        return DisposeErr::OK;
    }
    HILOGI("StartRestore, app %{public}s", bundleName.c_str());
    return DoDispose(bundleName, DisposeOperation::START_RESTORE);
}

DisposeErr AppGalleryDisposeProxy::EndRestore(const std::string &bundleName)
{
    if (SAUtils::IsSABundleName(bundleName)) {
        HILOGI("SA does not need to EndRestore");
        return DisposeErr::OK;
    }
    HILOGI("EndRestore, app %{public}s", bundleName.c_str());
    return DoDispose(bundleName, DisposeOperation::END_RESTORE);
}

void RecordDoDisposeRes(const std::string &bundleName,
                        AppGalleryDisposeProxy::DisposeOperation disposeOperation, int32_t err)
{
    AppRadar::Info info (bundleName, "", "");
    switch (disposeOperation) {
        case AppGalleryDisposeProxy::DisposeOperation::START_BACKUP:
            AppRadar::GetInstance().RecordBackupFuncRes(info, "StartBackup", AppRadar::GetInstance().GetUserId(),
                                                        BizStageBackup::BIZ_STAGE_START_DISPOSE_FAIL, err);
            break;
        case AppGalleryDisposeProxy::DisposeOperation::END_BACKUP:
            AppRadar::GetInstance().RecordBackupFuncRes(info, "EndBackup", AppRadar::GetInstance().GetUserId(),
                                                        BizStageBackup::BIZ_STAGE_END_DISPOSE_FAIL, err);
            break;
        case AppGalleryDisposeProxy::DisposeOperation::START_RESTORE:
            AppRadar::GetInstance().RecordRestoreFuncRes(info, "StartRestore", AppRadar::GetInstance().GetUserId(),
                                                         BizStageRestore::BIZ_STAGE_START_DISPOSE_FAIL, err);
            break;
        case AppGalleryDisposeProxy::DisposeOperation::END_RESTORE:
            AppRadar::GetInstance().RecordRestoreFuncRes(info, "EndRestore", AppRadar::GetInstance().GetUserId(),
                                                         BizStageRestore::BIZ_STAGE_END_DISPOSE_FAIL, err);
            break;
        default:
            break;
    }
}

DisposeErr AppGalleryDisposeProxy::DoDispose(const std::string &bundleName, DisposeOperation disposeOperation)
{
    try {
        HILOGI("DoDispose, app %{public}s, operation %{public}d", bundleName.c_str(), disposeOperation);
        if (!ConnectExtAbility<AppGalleryDisposeProxy>() || appRemoteObj_ == nullptr) {
            HILOGI("Can not connect to %{public}s", bundleName.c_str());
            return DisposeErr::CONN_FAIL;
        }

        BJsonUtil::BundleDetailInfo bundleDetailInfo = BJsonUtil::ParseBundleNameIndexStr(bundleName);
        MessageParcel data;
        const auto interfaceToken = APP_FOUNDATION_SERVICE;
        if (!data.WriteInterfaceToken(interfaceToken)) {
            HILOGI("write WriteInterfaceToken failed");
            return DisposeErr::IPC_FAIL;
        }
        if (!data.WriteString16(Str8ToStr16(bundleDetailInfo.bundleName))) {
            HILOGI("write bundleName failed");
            return DisposeErr::IPC_FAIL;
        }
        if (!data.WriteInt32(static_cast<int32_t>(bundleDetailInfo.bundleIndex))) {
            HILOGI("write bundleIndex failed");
            return DisposeErr::IPC_FAIL;
        }

        MessageParcel reply;
        MessageOption option(MessageOption::TF_ASYNC);
        int32_t ret = appRemoteObj_->SendRequest(static_cast<int>(disposeOperation), data, reply, option);
        if (ret != ERR_NONE) {
            HILOGI("SendRequest error, code=%{public}d, bundleName=%{public}s , appindex =%{public}d",
                ret, bundleDetailInfo.bundleName.c_str(), bundleDetailInfo.bundleIndex);
            RecordDoDisposeRes(bundleName, disposeOperation, ret);
            return DisposeErr::REQUEST_FAIL;
        }

        HILOGI("SendRequest success, dispose=%{public}d, bundleName=%{public}s, appindex =%{public}d",
            disposeOperation, bundleDetailInfo.bundleName.c_str(), bundleDetailInfo.bundleIndex);
        return DisposeErr::OK;
    } catch (const BError &e) {
        HILOGE("Catch exception, errCode = %{public}d", e.GetCode());
        return DisposeErr::IPC_FAIL;
    } catch (...) {
        HHILOGE("Unexpected exception");
        return DisposeErr::IPC_FAIL;
    }
}

} // namespace OHOS::FileManagement::Backup