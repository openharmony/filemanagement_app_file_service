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

namespace OHOS::FileManagement::Backup {
using namespace std;

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

ServiceReverse::ServiceReverse(BIncrementalBackupSession::Callbacks callbacks)
    : scenario_(Scenario::BACKUP), callbacksIncrementalBackup_(callbacks)
{
}

ServiceReverse::ServiceReverse(BIncrementalRestoreSession::Callbacks callbacks)
    : scenario_(Scenario::RESTORE), callbacksIncrementalRestore_(callbacks)
{
}
} // namespace OHOS::FileManagement::Backup