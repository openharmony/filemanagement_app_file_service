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

#include "backupsaappend_fuzzer.h"

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

bool CmdInitRestoreSessionFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    BSessionRestore::Callbacks callbacks;
    std::shared_ptr<ServiceReverse> restorePtr =
        std::make_shared<ServiceReverse>(callbacks);
    datas.WriteRemoteObject(restorePtr->AsObject().GetRefPtr());
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    service->OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_RESTORE_SESSION),
        datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdInitBackupSessionFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    BSessionBackup::Callbacks callbacks;
    std::shared_ptr<ServiceReverse> backupPtr =
        std::make_shared<ServiceReverse>(callbacks);
    datas.WriteRemoteObject(backupPtr->AsObject().GetRefPtr());
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    service->OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_BACKUP_SESSION),
        datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdPublishFileFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    if (size > 0) {
        int pos = (size + 1) >> 1;
        std::string fileName((const char *)data, pos);
        std::string bundleName((const char *)data + pos, size - pos);
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
    service->OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_PUBLISH_FILE),
        datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdGetLocalCapabilitiesFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    uint32_t code = static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_LOCAL_CAPABILITIES);
    service->OnRemoteRequest(code, datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdAppFileReadyFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    std::string fileName((const char *)data, size);
    datas.WriteString(fileName);

    int fd = -1;
    if (size >= sizeof(int)) {
        fd = *(reinterpret_cast<const int *>(data));
    }
    datas.WriteFileDescriptor(UniqueFd(fd));

    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    uint32_t code = static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_FILE_READY);
    service->OnRemoteRequest(code, datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdAppDoneFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    if (size >= sizeof(bool)) {
        datas.WriteBool(*(reinterpret_cast<const bool *>(data)));
    }
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    service->OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_DONE),
        datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdStartFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    service->OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_START),
        datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdFinishFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    service->OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_FINISH),
        datas, reply, option);
    service = nullptr;
    return true;
}

bool CmdAppendBundlesRestoreSessionFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());

    size_t len = sizeof(int);
    if (size >= len) {
        int fd = *(reinterpret_cast<const int *>(data));
        datas.WriteFileDescriptor(UniqueFd(fd));
    }

    if (size > 0) {
        vector<string> bundleNames;
        for (size_t i = 0; i < size; i++) {
            string name = string(reinterpret_cast<const char*>(data), size) + to_string(i);
            bundleNames.push_back(name);
        }
        datas.WriteStringVector(bundleNames);
    }

    if (size >= len + sizeof(int32_t)) {
        int32_t type = static_cast<int32_t>(*(reinterpret_cast<const int32_t *>(data + len)));
        datas.WriteInt32(type);
        len += sizeof(int32_t);
    }

    if (size >= len + sizeof(int32_t)) {
        int32_t userId = static_cast<int32_t>(*(reinterpret_cast<const int32_t *>(data + len)));
        datas.WriteInt32(userId);
    }
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service = sptr(new Service(SERVICE_ID));
    service->OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION),
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
    
    OHOS::CmdInitRestoreSessionFuzzTest(data, size);
    OHOS::CmdInitBackupSessionFuzzTest(data, size);
    OHOS::CmdPublishFileFuzzTest(data, size);
    OHOS::CmdGetLocalCapabilitiesFuzzTest(data, size);
    OHOS::CmdAppFileReadyFuzzTest(data, size);
    OHOS::CmdAppDoneFuzzTest(data, size);
    OHOS::CmdStartFuzzTest(data, size);
    OHOS::CmdFinishFuzzTest(data, size);
    OHOS::CmdAppendBundlesRestoreSessionFuzzTest(data, size);
    return 0;
}