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

#include "backupsaappincrementalfileready_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include "message_parcel.h"
#include "service.h"

using namespace std;
using namespace OHOS::FileManagement::Backup;

namespace OHOS {
constexpr int32_t SERVICE_ID = 5203;
constexpr size_t MAX_STRING_LENGTH = 256;

bool CmdAppIncrementalFileReadyFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    string fileName = fdp.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int32_t fd = fdp.ConsumeIntegral<int32_t>();
    int32_t manifestFd = fdp.ConsumeIntegral<int32_t>();

    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    datas.WriteString(fileName);
    datas.WriteFileDescriptor(UniqueFd(fd));
    datas.WriteFileDescriptor(UniqueFd(manifestFd));
    datas.RewindRead(0);

    MessageParcel reply;
    MessageOption option;
    sptr<Service> service(new Service(SERVICE_ID));
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_APP_INCREMENTAL_FILE_READY);
    service->OnRemoteRequest(code, datas, reply, option);
    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    try {
        OHOS::CmdAppIncrementalFileReadyFuzzTest(data, size);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        HILOGE("BackupSaFuzzTest error");
    } catch (...) {
        HILOGE("BackupSaFuzzTest exception");
    }
    return 0;
}