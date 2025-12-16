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

#include "backupsaappendbundlesbackupsession_fuzzer.h"

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

bool CmdAppendBundlesBackupSessionFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(ServiceStub::GetDescriptor());
    if (size > 0) {
        vector<string> bundleNames;
        string param(reinterpret_cast<const char*>(data), size);
        string paramString = "";
        for (size_t i = 0; i < size; i++) {
            string name = param + to_string(i);
            paramString += name + ',';
            bundleNames.push_back(name);
        }
        HILOGI("CmdAppendBundlesBackupSessionFuzzTest paramString:%{public}s", paramString.c_str());
        datas.WriteStringVector(bundleNames);
    }
    MessageParcel reply;
    MessageOption option;
    sptr service(new Service(SERVICE_ID));
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_APPEND_BUNDLES_BACKUP_SESSION);
    try {
        service->OnRemoteRequest(code, datas, reply, option);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        HILOGE("BackupSaFuzzTest error");
    } catch (const std::exception& ex) {
        HILOGE("BackupSaFuzzTest exception");
    } catch (...) {
        HILOGE("BackupSaFuzzTest unknown exception");
    }
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    try {
        OHOS::CmdAppendBundlesBackupSessionFuzzTest(data, size);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        HILOGE("BackupSaFuzzTest error");
    } catch (...) {
        HILOGE("BackupSaFuzzTest exception");
    }
    return 0;
}