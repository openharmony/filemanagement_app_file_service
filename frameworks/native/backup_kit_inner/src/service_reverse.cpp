/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ErrCode ServiceReverse::BackupOnFileReady(const std::string &bundleName,
                                          const std::string &fileName,
                                          int fd,
                                          int32_t errCode)
{
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksBackup_.onFileReady(bFileInfo, UniqueFd(fd), errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::BackupOnFileReadyWithoutFd(const std::string &bundleName,
                                                   const std::string &fileName,
                                                   int32_t errCode)
{
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksBackup_.onFileReady(bFileInfo, UniqueFd(INVALID_FD), errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::BackupOnBundleStarted(int32_t errCode, const std::string &bundleName)
{
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onBundleStarted) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksBackup_.onBundleStarted(errCode, bundleName);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::BackupOnResultReport(const std::string &result, const std::string &bundleName)
{
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onResultReport) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksBackup_.onResultReport(bundleName, result);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::BackupOnBundleFinished(int32_t errCode, const std::string &bundleName)
{
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onBundleFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    HILOGI("errCode = %{public}d, bundleName = %{public}s", errCode, bundleName.c_str());
    callbacksBackup_.onBundleFinished(errCode, bundleName);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::BackupOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onAllBundlesFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksBackup_.onAllBundlesFinished(errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::BackupOnProcessInfo(const std::string &bundleName, const std::string &processInfo)
{
    HILOGI("bundleName = %{public}s", bundleName.c_str());
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onProcess) {
        HILOGI("Error scenario or callback is nullptr");
        return BError(BError::Codes::OK);
    }
    callbacksBackup_.onProcess(bundleName, processInfo);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::BackupOnScanningInfo(const std::string &scannedInfo)
{
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onBackupSizeReport) {
        HILOGE("Error scenario or callback is nullptr");
        return BError(BError::Codes::OK);
    }
    callbacksBackup_.onBackupSizeReport(scannedInfo);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::RestoreOnBundleStarted(int32_t errCode, const std::string &bundleName)
{
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onBundleStarted) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksRestore_.onBundleStarted(errCode, bundleName);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::RestoreOnBundleFinished(int32_t errCode, const std::string &bundleName)
{
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onBundleFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    HILOGI("errCode = %{public}d, bundleName = %{public}s", errCode, bundleName.c_str());
    callbacksRestore_.onBundleFinished(errCode, bundleName);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::RestoreOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onAllBundlesFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksRestore_.onAllBundlesFinished(errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::RestoreOnFileReady(const std::string &bundleName,
                                           const std::string &fileName,
                                           int fd,
                                           int32_t errCode)
{
    HILOGD("begin, bundleName is:%{public}s", bundleName.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksRestore_.onFileReady(bFileInfo, UniqueFd(fd), errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::RestoreOnFileReadyWithoutFd(const std::string &bundleName,
                                                    const std::string &fileName,
                                                    int32_t errCode)
{
    HILOGD("begin, bundleName is:%{public}s", bundleName.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksRestore_.onFileReady(bFileInfo, UniqueFd(INVALID_FD), errCode);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::RestoreOnResultReport(const std::string &result, const std::string &bundleName, ErrCode errCode)
{
    HILOGI("ServiceReverse RestoreOnResultReport bundle %{public}s begin with result: %{public}s", bundleName.c_str(),
           result.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onResultReport) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksRestore_.onResultReport(bundleName, result);
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onBundleFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return BError(BError::Codes::OK);
    }
    callbacksRestore_.onBundleFinished(errCode, bundleName);
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverse::RestoreOnProcessInfo(const std::string &bundleName, const std::string &processInfo)
{
    HILOGI("bundleName = %{public}s", bundleName.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onProcess) {
        HILOGI("Error scenario or callback is nullptr");
        return BError(BError::Codes::OK);
    }
    callbacksRestore_.onProcess(bundleName, processInfo);
    return BError(BError::Codes::OK);
}

ServiceReverse::ServiceReverse(BSessionBackup::Callbacks callbacks)
    : scenario_(Scenario::BACKUP), callbacksBackup_(callbacks)
{
}

ServiceReverse::ServiceReverse(BSessionRestore::Callbacks callbacks)
    : scenario_(Scenario::RESTORE), callbacksRestore_(callbacks)
{
}
} // namespace OHOS::FileManagement::Backup