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

#include "backupsaappendbundlesincrementalbackupsession_fuzzer.h"

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

void GetBundleNamesData(const uint8_t *data, size_t size, vector<BIncrementalData> &bundleNames)
{
    int minLen = sizeof(int64_t) + sizeof(int) + sizeof(int32_t);
    if (size < minLen + 1) {
        return;
    }
    FuzzedDataProvider fdp(data, size);
    uint8_t loop = fdp.ConsumeIntegral<uint8_t>();
    size--;
    if (loop == 0 || (minLen * loop) > size) {
        return;
    }
    int blob = (size / loop);
    int len = (blob - minLen) >> 1;
    for (size_t i = 0, pos = 1; i < loop; i++, pos += blob) {
        int64_t nTime = fdp.ConsumeIntegral<int64_t>();
        int fd = fdp.ConsumeIntegral<int>();
        int32_t priority = fdp.ConsumeIntegral<int32_t>();
        string name(reinterpret_cast<const char*>(data + pos + minLen), len);
        string parameters(reinterpret_cast<const char*>(data + pos + len + minLen), len);
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

bool CmdAppendBundlesIncrementalBackupSessionFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    if (size >= sizeof(int32_t)) {
        vector<BIncrementalData> bundleNames;
        GetBundleNamesData(data, size, bundleNames);
        WriteParcelableVector(bundleNames, datas);
    }
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr service(new Service(SERVICE_ID));
    uint32_t code = static_cast<uint32_t>(
        IServiceIpcCode::COMMAND_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION);
    service->OnRemoteRequest(code, datas, reply, option);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    try {
        OHOS::CmdAppendBundlesIncrementalBackupSessionFuzzTest(data, size);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        HILOGE("BackupSaFuzzTest error");
    } catch (...) {
        HILOGE("BackupSaFuzzTest exception");
    }
    return 0;
}