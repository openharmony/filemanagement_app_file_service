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

#include "module_ipc/service_reverse_proxy.h"

#include "module_app_gallery/app_gallery_dispose_proxy.h"
#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void ServiceReverseProxy::BackupOnFileReady(string bundleName, string fileName, int fd, int32_t errCode)
{
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    bool fdFlag = fd < 0 ? false : true;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteString(bundleName) || !data.WriteString(fileName) ||
        !data.WriteBool(fdFlag) || (fdFlag == true && !data.WriteFileDescriptor(fd)) || !data.WriteInt32(errCode)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_BACKUP_ON_FILE_READY), data, reply, option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::BackupOnBundleStarted(int32_t errCode, string bundleName)
{
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteInt32(errCode) || !data.WriteString(bundleName)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    };

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_BACKUP_ON_SUB_TASK_STARTED), data, reply,
        option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::BackupOnResultReport(std::string result, std::string bundleName)
{
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteString(result) || !data.WriteString(bundleName)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    };

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_BACKUP_ON_RESULT_REPORT), data, reply,
        option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::BackupOnBundleFinished(int32_t errCode, string bundleName)
{
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteInt32(errCode) || !data.WriteString(bundleName)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_BACKUP_ON_SUB_TASK_FINISHED), data, reply,
        option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::BackupOnAllBundlesFinished(int32_t errCode)
{
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteInt32(errCode)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_BACKUP_ON_TASK_FINISHED), data, reply, option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::RestoreOnBundleStarted(int32_t errCode, string bundleName)
{
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteInt32(errCode) || !data.WriteString(bundleName)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_SUB_TASK_STARTED), data, reply,
        option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::RestoreOnBundleFinished(int32_t errCode, string bundleName)
{
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteInt32(errCode) || !data.WriteString(bundleName)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_SUB_TASK_FINISHED), data, reply,
        option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }

    DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->EndRestore(bundleName);
    HILOGI("RestoreOnBundleFinished EndRestore, code=%{public}d, bundleName=%{public}s", disposeErr,
        bundleName.c_str());
}

void ServiceReverseProxy::RestoreOnAllBundlesFinished(int32_t errCode)
{
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteInt32(errCode)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_TASK_FINISHED), data, reply,
        option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::RestoreOnFileReady(string bundleName, string fileName, int fd, int32_t errCode)
{
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    bool fdFlag = fd < 0 ? false : true;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteString(bundleName) || !data.WriteString(fileName) ||
        !data.WriteBool(fdFlag) || (fdFlag == true && !data.WriteFileDescriptor(fd)) || !data.WriteInt32(errCode)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_FILE_READY), data, reply, option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::RestoreOnResultReport(string result, std::string bundleName, ErrCode errCode)
{
    HILOGI("ServiceReverseProxy::RestoreOnResultReport Begin with result: %s", result.c_str());
    BExcepUltils::BAssert(Remote(), BError::Codes::SDK_INVAL_ARG, "Remote is nullptr");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteString(result) ||
        !data.WriteString(bundleName) || !data.WriteInt32(errCode)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(
        static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_RESULT_REPORT), data, reply,
        option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}
} // namespace OHOS::FileManagement::Backup