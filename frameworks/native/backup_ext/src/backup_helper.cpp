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

void AncoBackupHelper::FilterAndSaveBackupPaths(std::set<std::string> &includes, std::set<std::string> &compatIncludes,
    const std::vector<std::string> &excludes)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return;
    }
    auto ret = proxy->FilterAndSaveBackupPaths(includes, compatIncludes, excludes);
    if (ret != ERR_OK) {
        HILOGE("Failed to FilterAndSaveBackupPaths. err = %{public}d", ret);
    }
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

void AncoIncrementalRestoreHelper::CreateAncoRestoreTask()
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return;
    }
    auto ret = proxy->CreateAncoRestoreTask();
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

ErrCode AncoIncrementalRestoreHelper::StartAncoUnPacket(const std::vector<string> &ancoTarFiles,
    const std::vector<int64_t> &ancoTarFileSizes, const std::vector<string> &ancoTarFileNames, const string &tempPath)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return BError(BError::Codes::EXT_INVAL_ARG);
    }
    return proxy->StartAncoUnPacket(ancoTarFiles, ancoTarFileSizes, ancoTarFileNames, tempPath);
}

void AncoIncrementalRestoreHelper::AddAncoMovePathsAndClean(std::vector<std::string> &ancoSourcePath,
    std::vector<std::string> &ancoTargetPath, std::vector<StatInfo> &ancoStats)
{
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get backup service");
        return;
    }
    auto ret = proxy->AddAncoMovePaths(ancoSourcePath, ancoTargetPath, ancoStats);
    if (ret != ERR_OK) {
        HILOGE("Failed to AddAncoMovePaths. err = %{public}d", ret);
    }
    ancoSourcePath.clear();
    ancoTargetPath.clear();
    ancoStats.clear();
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
