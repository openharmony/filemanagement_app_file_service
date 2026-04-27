/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ext_extension.h"

#include "anco_scan_result.h"
#include "b_anony/b_anony.h"
#include "b_error/b_error.h"
#include "b_utils/scan_file_singleton.h"
#include "b_utils/string_utils.h"
#include "filemgmt_libhilog.h"
#include "hitrace_meter.h"
#include "service_client.h"

namespace OHOS::FileManagement::Backup {
constexpr size_t MAX_IPC_SEND_DATA_SIZE = static_cast<size_t>(1024 * 1024);
constexpr double IPC_SEND_DATA_SCALE_FACTOR = 0.75;
constexpr size_t SAFE_IPC_SEND_DATA_SIZE = static_cast<size_t>(MAX_IPC_SEND_DATA_SIZE * IPC_SEND_DATA_SCALE_FACTOR);
constexpr size_t VECTOR_MAX_SIZE = 102400;
constexpr size_t SET_MAX_SIZE = 102400;
constexpr size_t CHAR16_SIZE = 2;
constexpr size_t STRING_MEM_FACTOR_16 = CHAR16_SIZE;

ErrCode AncoBackupCallback::OnBigFileReadyCallback(
    const std::string &filePath, const std::string &restorePath, const StatInfo &statInfo)
{
    if (StringUtils::CheckOverLongPath(filePath) >= BConstants::MAX_PATH_LEN) {
        return ErrCode(BError::Codes::EXT_INVAL_ARG);
    }
    ScanFileSingleton::GetInstance().AddAncoBigFile(filePath, restorePath, statInfo.sta);
    return ErrCode(BError::Codes::OK);
}

ErrCode AncoBackupCallback::OnTarFileReadyCallback(
    const std::string &fileName, const std::string &filePath, const StatInfo &statInfo)
{
    auto extensionPtr = extension_.promote();
    if (!extensionPtr) {
        return ErrCode(BError::Codes::EXT_INVAL_ARG);
    }
    if (StringUtils::CheckOverLongPath(filePath) >= BConstants::MAX_PATH_LEN) {
        return ErrCode(BError::Codes::EXT_INVAL_ARG);
    }
    extensionPtr->appStatistic_->tarFileSize_ += TarFile::GetInstance().GetTarFileSize();
    extensionPtr->appStatistic_->tarFileCount_++;
    ScanFileSingleton::GetInstance().AddAncoTarFile(fileName, filePath, statInfo.sta);
    return ErrCode(BError::Codes::OK);
}

ErrCode AncoBackupCallback::WaitForPacketFlag()
{
    ScanFileSingleton::GetInstance().WaitForPacketFlag();
    return ErrCode(BError::Codes::OK);
}

ErrCode AncoBackupCallback::ReportErrFileByProc(const std::string &msg, int32_t err)
{
    auto extensionPtr = extension_.promote();
    if (!extensionPtr) {
        return ErrCode(BError::Codes::EXT_INVAL_ARG);
    }
    extensionPtr->ReportErrFileByProc(extension_, extensionPtr->curScenario_)(msg, err);
    return ErrCode(BError::Codes::OK);
}

ErrCode AncoBackupCallback::UpdateFileStat(const std::string &filePath, const StatInfo &statInfo)
{
    auto extensionPtr = extension_.promote();
    if (!extensionPtr) {
        return ErrCode(BError::Codes::EXT_INVAL_ARG);
    }
    extensionPtr->UpdateFileStat(filePath, statInfo.sta.st_size);
    return ErrCode(BError::Codes::OK);
}

ErrCode AncoRestoreCallback::ReportFileInfos(const std::map<std::string, int64_t> &endFileInfos,
    const std::map<std::string, std::vector<int32_t>> &errFileInfos)
{
    HILOGI("ReportFileInfos: endFileInfos size: %{public}zu, errFileInfos size: %{public}zu",
        endFileInfos.size(), errFileInfos.size());
    auto extensionPtr = extension_.promote();
    if (!extensionPtr) {
        return ErrCode(BError::Codes::EXT_INVAL_ARG);
    }
    for (const auto &[fileName, size] : endFileInfos) {
        extensionPtr->endFileInfos_.emplace(fileName, static_cast<off_t>(size));
    }
    for (const auto &[fileName, codes] : errFileInfos) {
        std::vector<ErrCode> convertCodes;
        for (auto code : codes) {
            convertCodes.push_back(code);
        }
        extensionPtr->errFileInfos_.emplace(fileName, convertCodes);
    }
    return ErrCode(BError::Codes::OK);
}

void AncoBackupHelper::CreateAncoBackupTask(wptr<BackupExtExtension> extension)
{
    auto callback = sptr<AncoBackupCallback>::MakeSptr(extension);
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return;
    }
    auto ret = proxy->CreateAncoBackupTask(callback);
    if (ret != ERR_OK) {
        HILOGE("Failed to CreateAncoBackupTask. err = %{public}d", ret);
    }
}

void AncoBackupHelper::DestroyAncoBackupTask()
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return;
    }
    auto ret = proxy->DestroyAncoBackupTask();
    if (ret != ERR_OK) {
        HILOGE("Failed to DestroyAncoBackupTask. err = %{public}d", ret);
    }
}

static bool FilterAndSaveBackupPathsInner(std::set<std::string> &includes, std::set<std::string> &compatIncludes,
    const std::vector<std::string> &excludes)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return false;
    }
    auto ret = proxy->FilterAndSaveBackupPaths(includes, compatIncludes, excludes);
    if (ret != ERR_OK) {
        HILOGE("Failed to FilterAndSaveBackupPaths. err = %{public}d", ret);
        return false;
    }
    return true;
}

static bool FilterAndSaveBackupPathsSendIncludes(std::set<std::string> &includes)
{
    std::set<std::string> sliceIncludes;
    std::set<std::string> compatIncludes;
    std::vector<std::string> excludes;
    std::set<std::string> filteredIncludes;
    size_t curIpcDataSize = 0;
    for (const auto &path : includes) {
        const size_t byteSize = path.size() * STRING_MEM_FACTOR_16;
        if (byteSize > MAX_IPC_SEND_DATA_SIZE) {
            HILOGW("path is too long");
            return false;
        }
        if (curIpcDataSize + byteSize > SAFE_IPC_SEND_DATA_SIZE || sliceIncludes.size() >= SET_MAX_SIZE) {
            if (!FilterAndSaveBackupPathsInner(sliceIncludes, compatIncludes, excludes)) {
                return false;
            }
            filteredIncludes.merge(sliceIncludes);
            sliceIncludes.clear();
            curIpcDataSize = 0;
        }
        sliceIncludes.insert(path);
        curIpcDataSize += byteSize;
    }
    if (curIpcDataSize > 0 && !FilterAndSaveBackupPathsInner(sliceIncludes, compatIncludes, excludes)) {
        return false;
    }
    filteredIncludes.merge(sliceIncludes);
    includes = move(filteredIncludes);
    return true;
}

static bool FilterAndSaveBackupPathsSendCompatIncludes(std::set<std::string> &compatIncludes)
{
    std::set<std::string> includes;
    std::set<std::string> sliceCompatIncludes;
    std::vector<std::string> excludes;
    std::set<std::string> filteredSliceCompatIncludes;
    size_t curIpcDataSize = 0;
    for (const auto &path : compatIncludes) {
        const size_t byteSize = path.size() * STRING_MEM_FACTOR_16;
        if (byteSize > MAX_IPC_SEND_DATA_SIZE) {
            HILOGW("path is too long");
            return false;
        }
        if (curIpcDataSize + byteSize > SAFE_IPC_SEND_DATA_SIZE || sliceCompatIncludes.size() >= SET_MAX_SIZE) {
            if (!FilterAndSaveBackupPathsInner(includes, sliceCompatIncludes, excludes)) {
                return false;
            }
            filteredSliceCompatIncludes.merge(sliceCompatIncludes);
            sliceCompatIncludes.clear();
            curIpcDataSize = 0;
        }
        sliceCompatIncludes.insert(path);
        curIpcDataSize += byteSize;
    }
    if (curIpcDataSize > 0 && !FilterAndSaveBackupPathsInner(includes, sliceCompatIncludes, excludes)) {
        return false;
    }
    filteredSliceCompatIncludes.merge(sliceCompatIncludes);
    compatIncludes = move(filteredSliceCompatIncludes);
    return true;
}

static bool FilterAndSaveBackupPathsSendExcludes(const std::vector<std::string> &excludes)
{
    std::set<std::string> includes;
    std::set<std::string> compatIncludes;
    std::vector<std::string> sliceExcludes;
    size_t curIpcDataSize = 0;
    for (const auto &path : excludes) {
        const size_t byteSize = path.size() * STRING_MEM_FACTOR_16;
        if (byteSize > MAX_IPC_SEND_DATA_SIZE) {
            HILOGW("path is too long");
            return false;
        }
        if (curIpcDataSize + byteSize > SAFE_IPC_SEND_DATA_SIZE || sliceExcludes.size() >= VECTOR_MAX_SIZE) {
            if (!FilterAndSaveBackupPathsInner(includes, compatIncludes, sliceExcludes)) {
                return false;
            }
            sliceExcludes.clear();
            curIpcDataSize = 0;
        }
        sliceExcludes.push_back(path);
        curIpcDataSize += byteSize;
    }
    if (curIpcDataSize > 0 && !FilterAndSaveBackupPathsInner(includes, compatIncludes, sliceExcludes)) {
        return false;
    }
    return true;
}

void AncoBackupHelper::FilterAndSaveBackupPaths(std::set<std::string> &includes, std::set<std::string> &compatIncludes,
    const std::vector<std::string> &excludes)
{
    HILOGI("<before> includes: %{public}zu, compatIncludes: %{public}zu, excludes: %{public}zu",
        includes.size(), compatIncludes.size(), excludes.size());
    if (!FilterAndSaveBackupPathsSendIncludes(includes)) {
        HILOGE("Failed to FilterAndSaveBackupPaths includes.");
        return;
    }
    if (!FilterAndSaveBackupPathsSendCompatIncludes(compatIncludes)) {
        HILOGE("Failed to FilterAndSaveBackupPaths compatIncludes.");
        return;
    }
    if (!FilterAndSaveBackupPathsSendExcludes(excludes)) {
        HILOGE("Failed to FilterAndSaveBackupPaths excludes.");
        return;
    }
    HILOGI("<after> includes: %{public}zu, compatIncludes: %{public}zu, excludes: %{public}zu",
        includes.size(), compatIncludes.size(), excludes.size());
}

std::tuple<ErrCode, int64_t, int64_t> AncoBackupHelper::StartAncoScanAllDirs()
{
    AncoScanResult scanResult;
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return std::make_tuple(ERR_INVALID_OPERATION, scanResult.bigFileSize, scanResult.smallFileSize);
    }
    auto ret = proxy->StartAncoScanAllDirs(scanResult);
    if (ret != ERR_OK) {
        HILOGE("Failed to StartAncoScanAllDirs. err = %{public}d", ret);
        return std::make_tuple(ERR_INVALID_VALUE, scanResult.bigFileSize, scanResult.smallFileSize);
    }
    return std::make_tuple(ERR_OK, scanResult.bigFileSize, scanResult.smallFileSize);
}

void AncoBackupHelper::StartAncoPacket(uint64_t &ancoSmallFileCount)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return;
    }
    auto ret = proxy->StartAncoPacket(ancoSmallFileCount);
    if (ret != ERR_OK) {
        HILOGE("Failed to StartAncoPacket. err = %{public}d", ret);
    }
}

void AncoIncrementalRestoreHelper::CreateAncoRestoreTask(wptr<BackupExtExtension> extension)
{
    auto callback = sptr<AncoRestoreCallback>::MakeSptr(extension);
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return;
    }
    auto ret = proxy->CreateAncoRestoreTask(callback);
    if (ret != ERR_OK) {
        HILOGE("Failed to CreateAncoRestoreTask. err = %{public}d", ret);
    }
}

void AncoIncrementalRestoreHelper::DestroyAncoRestoreTask()
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return;
    }
    auto ret = proxy->DestroyAncoRestoreTask();
    if (ret != ERR_OK) {
        HILOGE("Failed to DestroyAncoRestoreTask. err = %{public}d", ret);
    }
}

ErrCode AncoIncrementalRestoreHelper::AddAncoTars(const std::vector<string> &ancoTarFiles,
    const std::vector<int64_t> &ancoTarFileSizes, const std::vector<string> &ancoTarFileNames)
{
    HILOGI("paths: %{public}zu, names: %{public}zu, sizes: %{public}zu",
        ancoTarFiles.size(), ancoTarFileNames.size(), ancoTarFileSizes.size());
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return BError(BError::Codes::EXT_INVAL_ARG);
    }
    if (!(ancoTarFiles.size() == ancoTarFileSizes.size() && ancoTarFiles.size() == ancoTarFileNames.size())) {
        HILOGE("size is not equal");
        return BError(BError::Codes::EXT_INVAL_ARG);
    }

    std::vector<string> sliceTarFiles;
    std::vector<int64_t> sliceTarFileSizes;
    std::vector<string> sliceTarFileNames;
    size_t curIpcDataSize = 0;
    for (size_t i = 0; i < ancoTarFiles.size(); ++i) {
        const size_t byteSize =
            (ancoTarFiles[i].size() + ancoTarFileNames[i].size()) * STRING_MEM_FACTOR_16 + sizeof(int64_t);
        if (byteSize > MAX_IPC_SEND_DATA_SIZE) {
            HILOGW("path is too long");
            continue;
        }
        if (curIpcDataSize + byteSize > SAFE_IPC_SEND_DATA_SIZE || sliceTarFiles.size() >= VECTOR_MAX_SIZE) {
            auto ret = proxy->AddAncoTars(sliceTarFiles, sliceTarFileSizes, sliceTarFileNames);
            if (ret != ERR_OK) {
                HILOGE("Failed to AddAncoTars, err = %{public}d", ret);
                return ret;
            }
            sliceTarFiles.clear();
            sliceTarFileSizes.clear();
            sliceTarFileNames.clear();
            curIpcDataSize = 0;
        }
        sliceTarFiles.push_back(ancoTarFiles[i]);
        sliceTarFileSizes.push_back(ancoTarFileSizes[i]);
        sliceTarFileNames.push_back(ancoTarFileNames[i]);
        curIpcDataSize += byteSize;
    }
    if (curIpcDataSize == 0) {
        return BError(BError::Codes::OK);
    }
    auto ret = proxy->AddAncoTars(sliceTarFiles, sliceTarFileSizes, sliceTarFileNames);
    if (ret != ERR_OK) {
        HILOGE("Failed to AddAncoTars, err = %{public}d", ret);
        return ret;
    }
    return BError(BError::Codes::OK);
}

ErrCode AncoIncrementalRestoreHelper::StartAncoUnPacket(const string &tempPath)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return BError(BError::Codes::EXT_INVAL_ARG);
    }
    return proxy->StartAncoUnPacket(tempPath);
}

ErrCode AncoIncrementalRestoreHelper::AddAncoMovePaths(const std::vector<std::string> &ancoSourcePath,
    const std::vector<std::string> &ancoTargetPath, const std::vector<StatInfo> &ancoStats)
{
    HILOGI("srcPaths: %{public}zu, dstPaths: %{public}zu, stats: %{public}zu",
        ancoSourcePath.size(), ancoTargetPath.size(), ancoStats.size());
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return BError(BError::Codes::EXT_INVAL_ARG);
    }
    if (!(ancoSourcePath.size() == ancoTargetPath.size() && ancoSourcePath.size() == ancoStats.size())) {
        HILOGE("size is not equal");
        return BError(BError::Codes::EXT_INVAL_ARG);
    }

    std::vector<std::string> sliceSourcePath;
    std::vector<std::string> sliceTargetPath;
    std::vector<StatInfo> sliceStats;
    size_t curIpcDataSize = 0;
    for (size_t i = 0; i < ancoSourcePath.size(); ++i) {
        const size_t byteSize =
            (ancoSourcePath[i].size() + ancoTargetPath[i].size()) * STRING_MEM_FACTOR_16 + sizeof(StatInfo);
        if (byteSize > MAX_IPC_SEND_DATA_SIZE) {
            HILOGW("path is too long");
            continue;
        }
        if (curIpcDataSize + byteSize > SAFE_IPC_SEND_DATA_SIZE || sliceSourcePath.size() >= VECTOR_MAX_SIZE) {
            auto ret = proxy->AddAncoMovePaths(sliceSourcePath, sliceTargetPath, sliceStats);
            if (ret != ERR_OK) {
                HILOGE("Failed to AddAncoMovePaths, err = %{public}d", ret);
                return ret;
            }
            sliceSourcePath.clear();
            sliceTargetPath.clear();
            sliceStats.clear();
            curIpcDataSize = 0;
        }
        sliceSourcePath.push_back(ancoSourcePath[i]);
        sliceTargetPath.push_back(ancoTargetPath[i]);
        sliceStats.push_back(ancoStats[i]);
        curIpcDataSize += byteSize;
    }
    if (curIpcDataSize == 0) {
        return BError(BError::Codes::OK);
    }
    auto ret = proxy->AddAncoMovePaths(sliceSourcePath, sliceTargetPath, sliceStats);
    if (ret != ERR_OK) {
        HILOGE("Failed to AddAncoMovePaths, err = %{public}d", ret);
        return ret;
    }
    return BError(BError::Codes::OK);
}

AncoRestoreResult AncoIncrementalRestoreHelper::StartAncoMove()
{
    AncoRestoreResult ancoRestoreRes;
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return ancoRestoreRes;
    }
    auto ret = proxy->StartAncoMove(ancoRestoreRes);
    if (ret != ERR_OK) {
        HILOGE("Failed to StartAncoMove. err = %{public}d", ret);
    }
    return ancoRestoreRes;
}
} // namespace OHOS::FileManagement::Backup
