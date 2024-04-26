/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "backupsaincremental_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <climits>
#include "message_parcel.h"
#include "service_stub.h"
#include "service.h"
#include "securec.h"
#include "system_ability.h"

#include "filemgmt_libhilog.h"

using namespace OHOS::FileManagement::Backup;


namespace OHOS {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
constexpr int32_t SERVICE_ID = 5203;

void GetLocalCapabilitiesIncrementalFuzzTest(const uint8_t *data, size_t size)
{
    sptr service = sptr(new Service(SERVICE_ID));
    BIncrementalData bundlenames;
    std::vector<BIncrementalData>bundleNames;
    bundlenames.bundleName = string(reinterpret_cast<const char *>(data), size);
    bundlenames.lastIncrementalTime = *(reinterpret_cast<const int64_t *>(data));
    bundlenames.manifestFd = *(reinterpret_cast<const int32_t *>(data));
    bundlenames.backupParameters = string(reinterpret_cast<const char *>(data), size);
    bundlenames.backupPriority = *(reinterpret_cast<const int32_t *>(data));
    bundleNames.push_back(bundlenames);
    service->GetLocalCapabilitiesIncremental(bundleNames);
    service = nullptr;
}

void AppendBundlesIncrementalBackupSessionFuzzTest(const uint8_t *data, size_t size)
{
    sptr service = sptr(new Service(SERVICE_ID));
    BIncrementalData bundlesToBackups;
    std::vector<BIncrementalData> bundlesToBackup;
    bundlesToBackups.bundleName = string(reinterpret_cast<const char *>(data), size);
    bundlesToBackups.lastIncrementalTime = *(reinterpret_cast<const int64_t *>(data));
    bundlesToBackups.manifestFd = *(reinterpret_cast<const int32_t *>(data));
    bundlesToBackups.backupParameters = string(reinterpret_cast<const char *>(data), size);
    bundlesToBackups.backupPriority = *(reinterpret_cast<const int32_t *>(data));
    bundlesToBackup.push_back(bundlesToBackups);
    service->AppendBundlesIncrementalBackupSession(bundlesToBackup);
    service = nullptr;
}

void PublishIncrementalFileFuzzTest(const uint8_t *data, size_t size)
{
    sptr service = sptr(new Service(SERVICE_ID));
    BFileInfo fileInfo;
    fileInfo.fileName = string(reinterpret_cast<const char *>(data), size);
    fileInfo.owner = string(reinterpret_cast<const char *>(data), size);
    fileInfo.sn = *(reinterpret_cast<const int32_t*>(data));
    service->PublishIncrementalFile(fileInfo);
    service = nullptr;
}

void InitIncrementalBackupSessionFuzzTest(const uint8_t *data, size_t size)
{
    sptr service = sptr(new Service(SERVICE_ID));
    sptr<IServiceReverse> remote = nullptr;
    service->InitIncrementalBackupSession(remote);
    service = nullptr;
}

void GetIncrementalFileHandleFuzzTest(const uint8_t *data, size_t size)
{
    sptr service = sptr(new Service(SERVICE_ID));
    const std::string bundleName(reinterpret_cast<const char*>(data), size);
    const std::string fileName(reinterpret_cast<const char*>(data), size);
    service->GetIncrementalFileHandle(bundleName, fileName);
    service = nullptr;
}
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t dataSize)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    /* Validate the length of size */
    if (dataSize < OHOS::U32_AT_SIZE || dataSize > OHOS::FOO_MAX_LEN) {
        return 0;
    }
    OHOS::GetLocalCapabilitiesIncrementalFuzzTest(data, dataSize);
    OHOS::AppendBundlesIncrementalBackupSessionFuzzTest(data, dataSize);
    OHOS::PublishIncrementalFileFuzzTest(data, dataSize);
    OHOS::InitIncrementalBackupSessionFuzzTest(data, dataSize);
    OHOS::GetIncrementalFileHandleFuzzTest(data, dataSize);
    return 0;
}