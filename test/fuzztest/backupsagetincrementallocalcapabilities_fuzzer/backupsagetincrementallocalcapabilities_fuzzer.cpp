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

#include "backupsagetincrementallocalcapabilities_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <climits>
#include <fuzzer/FuzzedDataProvider.h>
#include <vector>

#include "directory_ex.h"
#include "message_parcel.h"
#include "module_external/storage_manager_service.h"
#include "sandbox_helper.h"
#include "service.h"
#include "service_proxy.h"
#include "service_reverse.h"
#include "service_stub.h"
#include "securec.h"
#include "system_ability.h"

using namespace std;
using namespace OHOS::FileManagement::Backup;

template <class T>
T TypeCast(const uint8_t *data, int *pos = nullptr)
{
    if (pos) {
        *pos += sizeof(T);
    }
    return *(reinterpret_cast<const T *>(data));
}

namespace OHOS {
constexpr int32_t SERVICE_ID = 5203;
constexpr int32_t NEED_CREATE_THREE_STRING_NUM = 3;

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

bool CmdGetLocalCapabilitiesIncrementalFuzzTest(const uint8_t *data, size_t size)
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
    uint32_t code = static_cast<uint32_t>(IServiceIpcCode::COMMAND_GET_LOCAL_CAPABILITIES_INCREMENTAL);
    service->OnRemoteRequest(code, datas, reply, option);
    return true;
}

bool GetStorageRecognizeSandboxPathEl1FuzzTest(const uint8_t *data, size_t size)
{
    uint32_t userId = 100;
    int len = size >> 1;
    string bundleName(reinterpret_cast<const char*>(data), len);
    string sandBoxSubpath(reinterpret_cast<const char*>(data + len), len);
    string fullPath = BASE_EL1 + DEFAULT_PATH_WITH_WILDCARD + sandBoxSubpath;
    std::vector<std::string> phyIncludes;
    std::map<std::string, std::string> pathMap;
    StorageManagerService::GetInstance().RecognizeSandboxWildCard(userId, bundleName, fullPath, phyIncludes, pathMap);
    return true;
}

bool CmdExcludeFilter(const uint8_t *data, size_t size)
{
    int len = size / NEED_CREATE_THREE_STRING_NUM;
    string dirPath(reinterpret_cast<const char*>(data), len);
    string pathVal1(reinterpret_cast<const char*>(data + len), len);
    string pathVal2(reinterpret_cast<const char*>(data + len * 2), len);
    std::map<std::string, bool> excludesMap;
    excludesMap.insert(std::make_pair(pathVal1, true));
    excludesMap.insert(std::make_pair(pathVal2, false));
    StorageManagerService::GetInstance().ExcludeFilter(excludesMap, dirPath);
    return true;
}

bool GetStorageRecognizeSandboxPathEl2FuzzTest(const uint8_t *data, size_t size)
{
    uint32_t userId = 100;
    int len = size >> 1;
    string bundleName(reinterpret_cast<const char*>(data), len);
    string sandBoxSubpath(reinterpret_cast<const char*>(data + len), len);
    string fullPath = BASE_EL2 + DEFAULT_PATH_WITH_WILDCARD + sandBoxSubpath;
    std::vector<std::string> phyIncludes;
    std::map<std::string, std::string> pathMap;
    StorageManagerService::GetInstance().RecognizeSandboxWildCard(userId, bundleName, fullPath, phyIncludes, pathMap);
    return true;
}

bool CmdGetMediaType(const uint8_t *data, size_t size)
{
    (void)data;
    std::shared_ptr<DataShare::DataShareResultSet> resultSet = std::make_shared<DataShare::DataShareResultSet>();
    StorageManager::StorageStats storageStats = {};
    StorageManagerService::GetInstance().GetMediaTypeAndSize(resultSet, storageStats);
    return true;
}

bool CmdGetFileStorageStats(const uint8_t *data, size_t size)
{
    (void)data;
    int32_t userId = 100;
    StorageManager::StorageStats storageStats = {};
    StorageManagerService::GetInstance().GetFileStorageStats(userId, storageStats);
    return true;
}


bool CmdInitQuotaMounts(const uint8_t *data, size_t size)
{
    (void)data;
    StorageManagerService::GetInstance().InitialiseQuotaMounts();
    return true;
}
bool CmdExcludePathMap(const uint8_t *data, size_t size)
{
    string excludePath(reinterpret_cast<const char*>(data), size);
    std::map<std::string, bool> excludesMap;
    StorageManagerService::GetInstance().SetExcludePathMap(excludePath, excludesMap);
    return true;
}

bool CmdConvertSandbxRealPath(const uint8_t *data, size_t size)
{
    uint32_t userId = 100;
    int len = size >> 1;
    string bundleName(reinterpret_cast<const char*>(data), len);
    string sandBoxSubpath(reinterpret_cast<const char*>(data + len), len);
    string fullSandBoxPath = NORMAL_SAND_PREFIX + sandBoxSubpath;
    std::vector<std::string> realPaths;
    std::map<std::string, std::string> pathMap;
    StorageManagerService::GetInstance().ConvertSandboxRealPath(userId, bundleName, fullSandBoxPath,
        realPaths, pathMap);

    string fullSandBoxFilePath = FILE_SAND_PREFIX + sandBoxSubpath;
    StorageManagerService::GetInstance().ConvertSandboxRealPath(userId, bundleName, fullSandBoxFilePath,
        realPaths, pathMap);

    string fullSandBoxMediaPath = MEDIA_SAND_PREFIX + sandBoxSubpath;
    StorageManagerService::GetInstance().ConvertSandboxRealPath(userId, bundleName, fullSandBoxMediaPath,
        realPaths, pathMap);

    string fullSandBoxMediaCloudPath = MEDIA_CLOUD_SAND_PREFIX + sandBoxSubpath;
    StorageManagerService::GetInstance().ConvertSandboxRealPath(userId, bundleName, fullSandBoxMediaCloudPath,
        realPaths, pathMap);
    return true;
}

bool CmdAddQuterDirInfoFileStateFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int64_t)) {
        return false;
    }
    int64_t lastBackupTime = *(reinterpret_cast<const int64_t*>(data));
    int len = (size - sizeof(int64_t)) / NEED_CREATE_THREE_STRING_NUM;
    string dir(reinterpret_cast<const char*>(data), len);
    string bundleName(reinterpret_cast<const char*>(data + len), len);
    BundleStatsParas bundleStatsParas = {100, bundleName, lastBackupTime, 0, 0};
    string sanboxDir(reinterpret_cast<const char*>(data + len * 2), len);
    std::ofstream statFile;
    std::map<std::string, bool> excludesMap;
    StorageManagerService::GetInstance().AddOuterDirIntoFileStat(dir, bundleStatsParas, sanboxDir,
        statFile, excludesMap);
    return true;
}

bool CmdPhysicalToSanboxPath(const uint8_t *data, size_t size)
{
    int len = size / NEED_CREATE_THREE_STRING_NUM;
    string dir(reinterpret_cast<const char*>(data), len);
    string sandBodDir(reinterpret_cast<const char*>(data + len), len);
    string path(reinterpret_cast<const char*>(data + len * 2), len);
    StorageManagerService::GetInstance().PhysicalToSandboxPath(dir, sandBodDir, path);
    return true;
}

bool CmdAddPathMapForPathWildCard(const uint8_t *data, size_t size)
{
    uint32_t userId = 100;
    int len = size >> 1;
    string bundleName(reinterpret_cast<const char*>(data), len);
    string phyPath(reinterpret_cast<const char*>(data + len), len);
    std::map<std::string, std::string> pathMap;
    StorageManagerService::GetInstance().AddPathMapForPathWildCard(userId, bundleName, phyPath, pathMap);
    return true;
}

bool CheckIfDirForIncludesFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int64_t) * NEED_CREATE_THREE_STRING_NUM) {
        return false;
    }
    int pos = 0;
    string bundleName = "com.example.test";
    BundleStatsParas paras = {
        .userId = 100,
        .lastBackupTime = TypeCast<int64_t>(data, &pos),
        .fileSizeSum = TypeCast<int64_t>(data + pos, &pos),
        .incFileSizeSum = TypeCast<int64_t>(data + pos, &pos),
        .bundleName = bundleName
    };

    string path(reinterpret_cast<const char *>(data + pos), size - pos);
    map<string, string> pathMap;
    map<string, bool> execludePathMap;
    string dir = BACKUP_PATH_PREFIX + std::to_string(paras.userId) + BACKUP_PATH_SURFFIX + bundleName +
        FILE_SEPARATOR_CHAR;
    string filePath = dir + BACKUP_STAT_SYMBOL + std::to_string(0);
    ofstream statFile;
    ForceCreateDirectory(dir);
    statFile.open(filePath, std::ios::out | std::ios::trunc);
    if (!statFile.is_open()) {
        return false;
    }
    statFile << VER_10_LINE1 << std::endl;
    statFile << VER_10_LINE2 << std::endl;
    StorageManagerService::GetInstance().CheckIfDirForIncludes(path, paras, pathMap, statFile, execludePathMap);
    StorageManagerService::GetInstance().GetIncludesFileStats(path, paras, pathMap, statFile, execludePathMap);
    FileStat fileStat;
    fileStat.filePath = path;
    StorageManagerService::GetInstance().WriteFileList(statFile, fileStat, paras);
    StorageManagerService::GetInstance().AddOuterDirIntoFileStat("/data/test", paras, "/data/test", statFile,
        execludePathMap);
    StorageManagerService::GetInstance().InsertStatFile(path, fileStat, statFile, execludePathMap, paras);
    return true;
}

bool GetPathWildCardFuzzTest(const uint8_t *data, size_t size)
{
    uint32_t userId = 100;
    string bundleName(reinterpret_cast<const char*>(data), size);
    string includeWildCard = "/data/test/" + WILDCARD_DEFAULT_INCLUDE;
    std::vector<string> includePathList;
    std::map<std::string, std::string> pathMap;
    StorageManagerService::GetInstance().GetPathWildCard(userId, bundleName, includeWildCard, includePathList, pathMap);
    return true;
}

bool CmdCheckOverLongPath(const uint8_t *data, size_t size)
{
    string filePath(reinterpret_cast<const char*>(data), size);
    StorageManagerService::GetInstance().CheckOverLongPath(filePath);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    try {
        OHOS::CmdGetLocalCapabilitiesIncrementalFuzzTest(data, size);
        OHOS::GetStorageRecognizeSandboxPathEl1FuzzTest(data, size);
        OHOS::GetStorageRecognizeSandboxPathEl2FuzzTest(data, size);
        OHOS::CmdExcludeFilter(data, size);
        OHOS::CmdGetMediaType(data, size);
        OHOS::CmdGetFileStorageStats(data, size);
        OHOS::CmdInitQuotaMounts(data, size);
        OHOS::CmdExcludePathMap(data, size);
        OHOS::CmdConvertSandbxRealPath(data, size);
        OHOS::CmdAddQuterDirInfoFileStateFuzzTest(data, size);
        OHOS::CmdPhysicalToSanboxPath(data, size);
        OHOS::CmdAddPathMapForPathWildCard(data, size);
        OHOS::CheckIfDirForIncludesFuzzTest(data, size);
        OHOS::GetPathWildCardFuzzTest(data, size);
        OHOS::CmdCheckOverLongPath(data, size);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        HILOGE("BackupSaFuzzTest error");
    } catch (...) {
        HILOGE("BackupSaFuzzTest exception");
    }
    return 0;
}