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
#include "message_parcel.h"
#include "service_stub.h"
#include "service.h"
#include "securec.h"
#include "system_ability.h"

using namespace OHOS::FileManagement::Backup;

namespace OHOS {
constexpr int32_t SERVICE_ID = 5203;

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
    datas.WriteBuffer(data, size);
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

    OHOS::CmdStartFuzzTest(data, size);
    OHOS::CmdFinishFuzzTest(data, size);
    OHOS::CmdAppendBundlesRestoreSessionFuzzTest(data, size);
    return 0;
}