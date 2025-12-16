/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "backupsaappendbundlesrestoresession_fuzzer.h"

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

bool CmdAppendBundlesRestoreSessionFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    size_t len = 0;
    string paramString = "";
    if (size >= len + sizeof(int)) {
        int fd = *(reinterpret_cast<const int *>(data + len));
        datas.WriteFileDescriptor(UniqueFd(fd));
        len += sizeof(int);
    }
    int32_t type = 0;
    if (size >= len + sizeof(int32_t)) {
        type = *(reinterpret_cast<const int32_t *>(data + len));
        datas.WriteInt32(type);
        paramString += "type:" + to_string(type) + ',';
        len += sizeof(int32_t);
    }
    int32_t userId = 0;
    if (size >= len + sizeof(int32_t)) {
        userId = *(reinterpret_cast<const int32_t *>(data + len));
        paramString += "userId:" + to_string(userId) + ',';
        datas.WriteInt32(userId);
        len += sizeof(int32_t);
    }
    paramString += "bundleNames:";
    if (size > len) {
        vector<string> bundleNames;
        string baseStr(reinterpret_cast<const char*>(data + len), size - len);
        paramString += baseStr + ',';
        bundleNames.push_back(baseStr);
        datas.WriteStringVector(bundleNames);
    }
    HILOGI("CmdAppendBundlesRestoreSessionFuzzTest paramString:%{public}s", paramString.c_str());
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    sptr service(new Service(SERVICE_ID));
    service->OnRemoteRequest(
        static_cast<uint32_t>(IServiceIpcCode::COMMAND_APPEND_BUNDLES_RESTORE_SESSION_DATA),
        datas, reply, option);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    try {
        OHOS::CmdAppendBundlesRestoreSessionFuzzTest(data, size);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        HILOGE("BackupSaFuzzTest error");
    } catch (...) {
        HILOGE("BackupSaFuzzTest exception");
    }
    return 0;
}