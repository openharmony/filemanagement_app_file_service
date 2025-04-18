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

#include "backupservicestub_fuzzer.h"

#include <cstring>

#include "b_incremental_data.h"
#include "b_session_backup.h"
#include "b_session_restore.h"
#include "message_parcel.h"
#include "refbase.h"
#include "service.h"
#include "service_reverse.h"

namespace OHOS {
using namespace FileManagement::Backup;

constexpr int32_t SERVICE_ID = 5203;

template<class T>
T TypeCast(const uint8_t *data, int *pos = nullptr)
{
    if (pos) {
        *pos += sizeof(T);
    }
    return *(reinterpret_cast<const T*>(data));
}

template <typename T>
void WriteParcelableVector(const std::vector<T> &parcelableVector, Parcel &data)
{
    if (!data.WriteUint32(parcelableVector.size())) {
        return;
    }

    for (const auto &parcelable : parcelableVector) {
        if (!data.WriteParcelable(&parcelable)) {
            return;
        }
    }

    return;
}

bool OnRemoteRequestFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(uint32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;

    int pos = 0;
    uint32_t code = TypeCast<uint32_t>(data, &pos);
    msg.WriteInterfaceToken(ServiceStub::GetDescriptor());

    service->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool CmdInitRestoreSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_INIT_RESTORE_SESSION);
    
    try {
        BSessionRestore::Callbacks callbacks;
        msg.WriteRemoteObject(new ServiceReverse(callbacks));
        msg.WriteBuffer(data, size);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdInitBackupSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_INIT_BACKUP_SESSION);

    try {
        BSessionBackup::Callbacks callbacks;
        msg.WriteRemoteObject(new ServiceReverse(callbacks));
        msg.WriteBuffer(data, size);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdStartFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_START);

    try {
        msg.WriteBuffer(data, size);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetLocalCapabilitiesFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_GET_LOCAL_CAPABILITIES);

    try {
        msg.WriteBuffer(data, size);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdPublishFileFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(TmpFileSN)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_PUBLISH_FILE);

    try {
        int pos = 0;
        BFileInfo info;
        info.sn = TypeCast<TmpFileSN>(data, &pos);
        int len = (size - pos) >> 1;
        info.owner = string(reinterpret_cast<const char*>(data + pos), len);
        info.fileName = string(reinterpret_cast<const char*>(data + pos + len), len);
        msg.WriteParcelable(&info);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppFileReadyFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t) + sizeof(bool) + sizeof(int)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_APP_FILE_READY);

    try {
        int pos = 0;
        int fd = TypeCast<int>(data, &pos);
        int32_t errCode = TypeCast<int32_t>(data + pos, &pos);
        bool fdFlag = TypeCast<bool>(data + pos, &pos);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        msg.WriteBool(fdFlag);
        if (fdFlag) {
            msg.WriteFileDescriptor(fd);
        }
        msg.WriteInt32(errCode);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppDoneFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_APP_DONE);

    try {
        int32_t errCode = TypeCast<int32_t>(data);
        msg.WriteInt32(errCode);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdResultReportFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t) + sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_SERVICE_RESULT_REPORT);

    try {
        int pos = 0;
        int32_t scenario = TypeCast<int32_t>(data, &pos);
        int32_t errCode = TypeCast<int32_t>(data + pos, &pos);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        msg.WriteInt32(scenario);
        msg.WriteInt32(errCode);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetFileHandleFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_GET_FILE_HANDLE);

    try {
        int len = size >> 1;
        msg.WriteString(string(reinterpret_cast<const char*>(data), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + len), size - len));
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppendBundlesRestoreSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int) + sizeof(int32_t) + sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_APPEND_BUNDLES_RESTORE_SESSION_DATA);

    try {
        int pos = 0;
        int fd = TypeCast<int>(data, &pos);
        int32_t type = TypeCast<int32_t>(data + pos, &pos);
        int32_t userId = TypeCast<int32_t>(data + pos, &pos);
        vector<string> bundleNames;
        bundleNames.emplace_back(string(reinterpret_cast<const char*>(data + pos), size - pos));
        msg.WriteFileDescriptor(fd);
        msg.WriteStringVector(bundleNames);
        msg.WriteInt32(type);
        msg.WriteInt32(userId);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppendBundlesDetailsRestoreSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int) + sizeof(int32_t) + sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_APPEND_BUNDLES_RESTORE_SESSION_DATA_BY_DETAIL);

    try {
        int pos = 0;
        int fd = TypeCast<int>(data, &pos);
        int32_t type = TypeCast<int32_t>(data + pos, &pos);
        int32_t userId = TypeCast<int32_t>(data + pos, &pos);
        int len = (size - pos) >> 1;
        vector<string> bundleNames;
        bundleNames.emplace_back(string(reinterpret_cast<const char*>(data + pos), len));
        vector<string> detailInfos;
        detailInfos.emplace_back(string(reinterpret_cast<const char*>(data + pos + len), len));
        msg.WriteFileDescriptor(fd);
        msg.WriteStringVector(bundleNames);
        msg.WriteStringVector(detailInfos);
        msg.WriteInt32(type);
        msg.WriteInt32(userId);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppendBundlesBackupSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_APPEND_BUNDLES_BACKUP_SESSION);

    try {
        vector<string> bundleNames;
        bundleNames.emplace_back(string(reinterpret_cast<const char*>(data), size));
        msg.WriteStringVector(bundleNames);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppendBundlesDetailsBackupSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_APPEND_BUNDLES_DETAILS_BACKUP_SESSION);

    try {
        int len = size >> 1;
        vector<string> bundleNames;
        bundleNames.emplace_back(string(reinterpret_cast<const char*>(data), len));
        vector<string> detailInfos;
        detailInfos.emplace_back(string(reinterpret_cast<const char*>(data + len), len));
        msg.WriteStringVector(bundleNames);
        msg.WriteStringVector(detailInfos);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdFinishFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_FINISH);
    try {
        msg.WriteBuffer(data, size);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

[[maybe_unused]] bool CmdReleaseFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_RELEASE);
    try {
        msg.WriteBuffer(data, size);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetLocalCapabilitiesIncrementalFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_GET_LOCAL_CAPABILITIES_INCREMENTAL);

    try {
        vector<BIncrementalData> bundleNames;
        bundleNames.emplace_back(string(reinterpret_cast<const char*>(data), size), 0);
        WriteParcelableVector(bundleNames, msg);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetAppLocalListAndDoIncrementalBackupFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_GET_APP_LOCAL_LIST_AND_DO_INCREMENTAL_BACKUP);
    try {
        msg.WriteBuffer(data, size);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdInitIncrementalBackupSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_INIT_INCREMENTAL_BACKUP_SESSION);

    try {
        BIncrementalBackupSession::Callbacks callbacks;
        msg.WriteRemoteObject(new ServiceReverse(callbacks));
        msg.WriteBuffer(data, size);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppendBundlesIncrementalBackupSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION);

    try {
        vector<BIncrementalData> bundlesToBackup;
        bundlesToBackup.emplace_back(string(reinterpret_cast<const char*>(data), size), 0);
        WriteParcelableVector(bundlesToBackup, msg);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppendBundlesDetailsIncrementalBackupSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        IServiceIpcCode::COMMAND_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION_WITH_BUNDLE_INFOS);

    try {
        int len = size >> 2;
        vector<BIncrementalData> bundlesToBackup;
        bundlesToBackup.emplace_back(string(reinterpret_cast<const char*>(data), len), 0);
        std::vector<std::string> infos;
        infos.emplace_back(string(reinterpret_cast<const char*>(data + len), len));
        infos.emplace_back(string(reinterpret_cast<const char*>(data + len + len), len));
        WriteParcelableVector(bundlesToBackup, msg);
        msg.WriteStringVector(infos);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdPublishIncrementalFileFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(TmpFileSN)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_PUBLISH_INCREMENTAL_FILE);

    try {
        int pos = 0;
        BFileInfo info;
        info.sn = TypeCast<TmpFileSN>(data, &pos);
        int len = (size - pos) >> 1;
        info.owner = string(reinterpret_cast<const char*>(data + pos), len);
        info.fileName = string(reinterpret_cast<const char*>(data + pos + len), len);
        msg.WriteParcelable(&info);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

[[maybe_unused]] bool CmdPublishSAIncrementalFileFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int) + sizeof(TmpFileSN)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_PUBLISH_S_A_INCREMENTAL_FILE);

    try {
        int pos = 0;
        BFileInfo info;
        int fd = TypeCast<int>(data, &pos);
        info.sn = TypeCast<TmpFileSN>(data + pos, &pos);
        int len = (size - pos) >> 1;
        info.owner = string(reinterpret_cast<const char*>(data + pos), len);
        info.fileName = string(reinterpret_cast<const char*>(data + pos + len), len);
        msg.WriteParcelable(&info);
        msg.WriteFileDescriptor(fd);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppIncrementalFileReadyFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t) + sizeof(bool) + sizeof(int) + sizeof(int)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_APP_INCREMENTAL_FILE_READY);

    try {
        int pos = 0;
        int fd1 = TypeCast<int>(data, &pos);
        int fd2 = TypeCast<int>(data + pos, &pos);
        int32_t errCode = TypeCast<int32_t>(data + pos, &pos);
        bool fdFlag = TypeCast<bool>(data + pos, &pos);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        msg.WriteBool(fdFlag);
        if (fdFlag) {
            msg.WriteFileDescriptor(fd1);
            msg.WriteFileDescriptor(fd2);
        }
        msg.WriteInt32(errCode);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppIncrementalDoneFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_APP_INCREMENTAL_DONE);

    try {
        int32_t errCode = TypeCast<int32_t>(data);
        msg.WriteInt32(errCode);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetIncrementalFileHandleFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_GET_INCREMENTAL_FILE_HANDLE);

    try {
        int len = size >> 1;
        msg.WriteString(string(reinterpret_cast<const char*>(data), len));
        msg.WriteString(string(reinterpret_cast<const char*>(data + len), size - len));
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetBackupInfoFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_GET_BACKUP_INFO);

    try {
        msg.WriteString(string(reinterpret_cast<const char*>(data), size));
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdUpdateTimerFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_UPDATE_TIMER);

    try {
        int pos = 0;
        int32_t timeout = TypeCast<int32_t>(data, &pos);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        msg.WriteInt32(timeout);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdUpdateSendRateFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_UPDATE_SEND_RATE);

    try {
        int pos = 0;
        int32_t sendRate = TypeCast<int32_t>(data, &pos);
        msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
        msg.WriteInt32(sendRate);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetBackupDataSizeFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t) + sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_GET_BACKUP_DATA_SIZE);

    try {
        msg.WriteBuffer(data, size);
        service->OnRemoteRequest(code, msg, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::sptr service(new OHOS::FileManagement::Backup::Service(OHOS::SERVICE_ID));
    if (service == nullptr) {
        return 0;
    }

    try {
        OHOS::OnRemoteRequestFuzzTest(service, data, size);
        CmdInitRestoreSessionFuzzTest(service, data, size);
        CmdInitBackupSessionFuzzTest(service, data, size);
        CmdStartFuzzTest(service, data, size);
        CmdGetLocalCapabilitiesFuzzTest(service, data, size);
        CmdPublishFileFuzzTest(service, data, size);
        CmdAppFileReadyFuzzTest(service, data, size);
        CmdAppDoneFuzzTest(service, data, size);
        CmdResultReportFuzzTest(service, data, size);
        CmdGetFileHandleFuzzTest(service, data, size);
        CmdAppendBundlesRestoreSessionFuzzTest(service, data, size);
        CmdAppendBundlesDetailsRestoreSessionFuzzTest(service, data, size);
        CmdAppendBundlesBackupSessionFuzzTest(service, data, size);
        CmdAppendBundlesDetailsBackupSessionFuzzTest(service, data, size);
        CmdFinishFuzzTest(service, data, size);
        CmdGetLocalCapabilitiesIncrementalFuzzTest(service, data, size);
        CmdGetAppLocalListAndDoIncrementalBackupFuzzTest(service, data, size);
        CmdInitIncrementalBackupSessionFuzzTest(service, data, size);
        CmdAppendBundlesIncrementalBackupSessionFuzzTest(service, data, size);
        CmdAppendBundlesDetailsIncrementalBackupSessionFuzzTest(service, data, size);
        CmdPublishIncrementalFileFuzzTest(service, data, size);
        CmdAppIncrementalFileReadyFuzzTest(service, data, size);
        CmdAppIncrementalDoneFuzzTest(service, data, size);
        CmdGetIncrementalFileHandleFuzzTest(service, data, size);
        CmdGetBackupInfoFuzzTest(service, data, size);
        CmdUpdateTimerFuzzTest(service, data, size);
        CmdUpdateSendRateFuzzTest(service, data, size);
        CmdGetBackupDataSizeFuzzTest(service, data, size);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return 0;
}