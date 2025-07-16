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

#include "backupsagetincrementalfilehandle_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <climits>
#include <fuzzer/FuzzedDataProvider.h>
#include <vector>

#include "message_parcel.h"
#include "sandbox_helper.h"
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

bool CmdGetIncrementalFileHandleFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    int len = size >> 1;
    string bundleName(reinterpret_cast<const char*>(data), len);
    datas.WriteString(bundleName);
    string filePath(reinterpret_cast<const char*>(data + len), len);
    datas.WriteString(filePath);

    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service(new Service(SERVICE_ID));
    service->OnRemoteRequest(static_cast<uint32_t>(IServiceIpcCode::COMMAND_GET_INCREMENTAL_FILE_HANDLE),
        datas, reply, option);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    try {
        OHOS::CmdGetIncrementalFileHandleFuzzTest(data, size);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        HILOGE("BackupSaFuzzTest error");
    } catch (...) {
        HILOGE("BackupSaFuzzTest exception");
    }
    return 0;
}