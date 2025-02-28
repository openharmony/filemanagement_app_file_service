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
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return 0;
}