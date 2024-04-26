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

#include "backupsaanother_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <climits>
#include <vector>

#include "message_parcel.h"
#include "service.h"
#include "service_proxy.h"
#include "service_reverse.h"
#include "service_stub.h"
#include "securec.h"
#include "system_ability.h"

using namespace std;
using namespace OHOS::FileManagement::Backup;

namespace OHOS {
constexpr int32_t SERVICE_ID = 5203;

bool CmdGetFileHandleFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    int pos = (size + 1) >> 1;
    if (size > 0) {
        datas.WriteString(string(reinterpret_cast<const char*>(data), pos));
    }

    if (size > 1) {
        datas.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
    }
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    service->OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_FILE_NAME),
        datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdAppendBundlesBackupSessionFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    if (size > 0) {
        vector<string> bundleNames;
        for (size_t i = 0; i < size; i++) {
            string name = string(reinterpret_cast<const char*>(data), size) + to_string(i);
            bundleNames.push_back(name);
        }
        datas.WriteStringVector(bundleNames);
    }

    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    uint32_t code = static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_BACKUP_SESSION);
    service->OnRemoteRequest(code, datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdReleaseFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    uint32_t code = static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_RELSEASE_SESSION);
    service->OnRemoteRequest(code, datas, reply, option);
    service = nullptr;
    return true;
}

void GetBundleNamesData(const uint8_t *data, size_t size, vector<BIncrementalData> &bundleNames)
{
    for (size_t i = 0; i < size; i++) {
        string name = string(reinterpret_cast<const char*>(data), size) + to_string(i);
        if (size < sizeof(int64_t)) {
            BIncrementalData data(name, 0);
            bundleNames.push_back(data);
            continue;
        }

        int64_t nTime = *(reinterpret_cast<const int64_t*>(data));
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t priority = *(reinterpret_cast<const int32_t*>(data + sizeof(int32_t)));
        string parameters = string(reinterpret_cast<const char*>(data), size) + to_string(size - i);
        BIncrementalData incrementaData(name, nTime, fd, parameters, priority);
        bundleNames.push_back(incrementaData);
    }
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

bool CmdGetLocalCapabilitiesIncrementalFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    if (size > 0) {
        vector<BIncrementalData> bundleNames;
        GetBundleNamesData(data, size, bundleNames);
        WriteParcelableVector(bundleNames, datas);
    }

    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    uint32_t code = static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_LOCAL_CAPABILITIES_INCREMENTAL);
    service->OnRemoteRequest(code, datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdInitIncrementalBackupSessionFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    BIncrementalBackupSession::Callbacks callbacks;
    std::shared_ptr<ServiceReverse> backupPtr =
        std::make_shared<ServiceReverse>(callbacks);
    datas.WriteRemoteObject(backupPtr->AsObject().GetRefPtr());

    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    uint32_t code = static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_INCREMENTAL_BACKUP_SESSION);
    service->OnRemoteRequest(code, datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdAppendBundlesIncrementalBackupSessionFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    if (size > 0) {
        vector<BIncrementalData> bundleNames;
        GetBundleNamesData(data, size, bundleNames);
        WriteParcelableVector(bundleNames, datas);
    }
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    uint32_t code = static_cast<uint32_t>(
        IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION);
    service->OnRemoteRequest(code, datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdPublishIncrementalFileFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    if (size > 0) {
        int pos = (size + 1) >> 1;
        std::string fileName(reinterpret_cast<const char *>(data), pos);
        std::string bundleName(reinterpret_cast<const char *>(data) + pos, size - pos);
        uint32_t sn = 0;
        if (size > sizeof(uint32_t)) {
            sn = *(reinterpret_cast<const uint32_t *>(data));
        }
        BFileInfo fileInfo(fileName, bundleName, sn);
        datas.WriteParcelable(&fileInfo);
    }
    
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    uint32_t code = static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_PUBLISH_INCREMENTAL_FILE);
    service->OnRemoteRequest(code, datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdAppIncrementalFileReadyFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    std::string fileName(reinterpret_cast<const char *>(data), size);
    datas.WriteString(fileName);
    if (size >= sizeof(int)) {
        int fd = *(reinterpret_cast<const int *>(data));
        datas.WriteFileDescriptor(UniqueFd(fd));
    }

    if (size >= sizeof(int) + sizeof(int))  {
        int manifestId = *(reinterpret_cast<const int *>(data + sizeof(int)));
        datas.WriteFileDescriptor(UniqueFd(manifestId));
    }
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    uint32_t code = static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_INCREMENTAL_FILE_READY);
    service->OnRemoteRequest(code, datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdAppIncrementalDoneFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    if (size >= sizeof(int32_t)) {
        datas.WriteInt32(*(reinterpret_cast<const int32_t *>(data)));
    }

    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    service->OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_INCREMENTAL_DONE),
        datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdGetIncrementalFileHandleFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    int pos = (size + 1) >> 1;
    if (size > 0) {
        datas.WriteString(string(reinterpret_cast<const char*>(data), pos));
    }

    if (size > 1) {
        datas.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));
    }

    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    service->OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_INCREMENTAL_FILE_NAME),
        datas, reply, option);
    service = nullptr;
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }
    
    OHOS::CmdGetFileHandleFuzzTest(data, size);
    OHOS::CmdAppendBundlesBackupSessionFuzzTest(data, size);
    OHOS::CmdReleaseFuzzTest(data, size);
    OHOS::CmdGetLocalCapabilitiesIncrementalFuzzTest(data, size);
    OHOS::CmdInitIncrementalBackupSessionFuzzTest(data, size);
    OHOS::CmdPublishIncrementalFileFuzzTest(data, size);
    OHOS::CmdAppIncrementalFileReadyFuzzTest(data, size);
    OHOS::CmdAppIncrementalDoneFuzzTest(data, size);
    OHOS::CmdGetIncrementalFileHandleFuzzTest(data, size);
    OHOS::CmdAppendBundlesIncrementalBackupSessionFuzzTest(data, size);
    return 0;
}