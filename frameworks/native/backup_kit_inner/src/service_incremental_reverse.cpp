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

#include "service_reverse.h"

#include "b_error/b_error.h"
#include "b_radar/b_radar.h"
#include "filemgmt_libhilog.h"
#include "b_utils/string_utils.h"
#include <mutex>

namespace OHOS::FileManagement::Backup {
using namespace std;

void ServiceReverse::FlushPendingIncrementalFiles()
{
    std::vector<BackupFile> files;
    {
        std::lock_guard<std::mutex> lock(addIncrementalBatchLock_);
        files = std::move(pendingIncrementalFiles_);
        HILOGI("FlushPendingIncrementalFiles %{public}zu ok", files.size());
    }
    if (!files.empty() && callbacksIncrementalRestore_.onFileReadyBatch) {
        callbacksIncrementalRestore_.onFileReadyBatch(files);
    }
}
 
void ServiceReverse::AddIncrementalFileToBatch(const std::string &bundleName,
                                               const std::vector<std::string> &fileNames,
                                               const std::vector<FileOpenResult> &openResults)
{
    bool needFlush = false;
    {
        std::lock_guard<std::mutex> lock(addIncrementalBatchLock_);
        for (size_t i = 0; i < openResults.size(); ++i) {
            BackupFile file;
            file.bundleName = bundleName;
            file.uri = fileNames[i];
            file.fd = openResults[i].GetReleasedFd();
            file.manifestFd = openResults[i].GetReleasedManifestFd();
            file.errCode = openResults[i].errCode;
            pendingIncrementalFiles_.push_back(file);
        }
        needFlush = (pendingIncrementalFiles_.size() >= batchSize_);
        HILOGI("Current pending files: %{public}zu, batchSize: %{public}u, needFlush: %{public}d",
               pendingIncrementalFiles_.size(), batchSize_, needFlush);
    }
    if (needFlush) {
        FlushPendingIncrementalFiles();
    }
}

ErrCode ServiceReverse::IncrementalBackupOnFileReady(const std::string &bundleName,
                                                     const std::string &fileName,
                                                     int fd,
                                                     int manifestFd,
                                                     int32_t errCode)
{
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksIncrementalBackup_.onFileReady(bFileInfo, UniqueFd(fd), UniqueFd(manifestFd), errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalSaBackupOnFileReady(const std::string &bundleName, const std::string &fileName,
                                                       int fd, int32_t errCode)
{
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksIncrementalBackup_.onFileReady(bFileInfo, UniqueFd(fd), UniqueFd(INVALID_FD), errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalBackupOnFileReadyWithoutFd(const std::string &bundleName,
                                                              const std::string &fileName,
                                                              int32_t errCode)
{
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksIncrementalBackup_.onFileReady(bFileInfo, UniqueFd(INVALID_FD), UniqueFd(INVALID_FD), errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalBackupOnBundleStarted(int32_t errCode, const std::string &bundleName)
{
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onBundleStarted) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksIncrementalBackup_.onBundleStarted(errCode, bundleName);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalBackupOnResultReport(const std::string &result, const std::string &bundleName)
{
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onResultReport) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksIncrementalBackup_.onResultReport(bundleName, result);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalBackupOnBundleFinished(int32_t errCode, const std::string &bundleName)
{
    FlushPendingIncrementalFiles();
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onBundleFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    HILOGI("errCode = %{public}d, bundleName = %{public}s", errCode, bundleName.c_str());
    callbacksIncrementalBackup_.onBundleFinished(errCode, bundleName);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalBackupOnAllBundlesFinished(int32_t errCode)
{
    FlushPendingIncrementalFiles();
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onAllBundlesFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksIncrementalBackup_.onAllBundlesFinished(errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalBackupOnProcessInfo(const std::string &bundleName, const std::string &processInfo)
{
    HILOGI("bundleName = %{public}s", bundleName.c_str());
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onProcess) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksIncrementalBackup_.onProcess(bundleName, processInfo);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalBackupOnScanningInfo(const std::string &scannedInfo)
{
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onBackupSizeReport) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksIncrementalBackup_.onBackupSizeReport(scannedInfo);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalRestoreOnBundleStarted(int32_t errCode, const std::string &bundleName)
{
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onBundleStarted) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksIncrementalRestore_.onBundleStarted(errCode, bundleName);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalRestoreOnBundleFinished(int32_t errCode, const std::string &bundleName)
{
    FlushPendingIncrementalFiles();
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onBundleFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    HILOGI("errCode = %{public}d, bundleName = %{public}s", errCode, bundleName.c_str());
    callbacksIncrementalRestore_.onBundleFinished(errCode, bundleName);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalRestoreOnAllBundlesFinished(int32_t errCode)
{
    FlushPendingIncrementalFiles();
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onAllBundlesFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksIncrementalRestore_.onAllBundlesFinished(errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalRestoreOnFileReady(const std::string &bundleName,
                                                      const std::string &fileName,
                                                      int fd,
                                                      int manifestFd,
                                                      int32_t errCode)
{
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksIncrementalRestore_.onFileReady(bFileInfo, UniqueFd(fd), UniqueFd(manifestFd), errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalRestoreOnFileReadyWithoutFd(const std::string &bundleName,
                                                               const std::string &fileName,
                                                               int32_t errCode)
{
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksIncrementalRestore_.onFileReady(bFileInfo, UniqueFd(INVALID_FD), UniqueFd(INVALID_FD), errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalRestoreOnFileReadys(const std::string &bundleName,
                                                       const BStringRawData &fileNamesRD,
                                                       const std::vector<FileOpenResult> &openResults)
{
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onFileReadyBatch) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    std::string serializedData;
    fileNamesRD.Unmarshalling(serializedData);
    auto fileNames = StringUtils::StringVectorDeserialize(serializedData);
    AddIncrementalFileToBatch(bundleName, fileNames, openResults);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalRestoreOnResultReport(const std::string &result,
                                                         const std::string &bundleName,
                                                         ErrCode errCode)
{
    HILOGI("begin incremental restore on result report, bundleName:%{public}s", bundleName.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onResultReport) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksIncrementalRestore_.onResultReport(bundleName, result);
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onBundleFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksIncrementalRestore_.onBundleFinished(errCode, bundleName);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalRestoreOnProcessInfo(const std::string &bundleName, const std::string &processInfo)
{
    HILOGI("begin incremental report processInfo, bundleName:%{public}s", bundleName.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onProcess) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksIncrementalRestore_.onProcess(bundleName, processInfo);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::IncrementalRestoreOnMigrateResult(int32_t errCode, const std::string &bundleName)
{
    HILOGI("begin incremental migrate result, bundleName:%{public}s, errCode:%{public}d", bundleName.c_str(), errCode);
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onMigrateResult) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksIncrementalRestore_.onMigrateResult(errCode, bundleName);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::SetBatchSize(uint32_t size)
{
    std::lock_guard<std::mutex> lock(addBatchLock_);
    std::lock_guard<std::mutex> incrementalLock(addIncrementalBatchLock_);
    HILOGE("SetBatchSize = %{public}u", size);
    batchSize_ = size;
    return BError(BError::Codes::OK);
}

ServiceReverse::ServiceReverse(BIncrementalBackupSession::Callbacks callbacks)
    : scenario_(Scenario::BACKUP), callbacksIncrementalBackup_(callbacks)
{
}

ServiceReverse::ServiceReverse(BIncrementalRestoreSession::Callbacks callbacks)
    : scenario_(Scenario::RESTORE), callbacksIncrementalRestore_(callbacks)
{
}
} // namespace OHOS::FileManagement::Backup