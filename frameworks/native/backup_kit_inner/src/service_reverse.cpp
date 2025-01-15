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

void ServiceReverse::BackupOnFileReady(string bundleName, string fileName, int fd, int32_t errCode)
{
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksBackup_.onFileReady(bFileInfo, UniqueFd(fd), errCode);
}

void ServiceReverse::BackupOnBundleStarted(int32_t errCode, string bundleName)
{
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onBundleStarted) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksBackup_.onBundleStarted(errCode, bundleName);
}

void ServiceReverse::BackupOnResultReport(std::string result, std::string bundleName)
{
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onResultReport) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksBackup_.onResultReport(bundleName, result);
}

void ServiceReverse::BackupOnBundleFinished(int32_t errCode, string bundleName)
{
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onBundleFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    HILOGI("errCode = %{public}d, bundleName = %{public}s", errCode, bundleName.c_str());
    callbacksBackup_.onBundleFinished(errCode, bundleName);
}

void ServiceReverse::BackupOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onAllBundlesFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksBackup_.onAllBundlesFinished(errCode);
}

void ServiceReverse::BackupOnProcessInfo(std::string bundleName, std::string processInfo)
{
    HILOGI("bundleName = %{public}s", bundleName.c_str());
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onProcess) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksBackup_.onProcess(bundleName, processInfo);
}

void ServiceReverse::BackupOnScanningInfo(std::string scannedInfo)
{
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onBackupSizeReport) {
        HILOGE("Error scenario or callback is nullptr");
        return;
    }
    callbacksBackup_.onBackupSizeReport(scannedInfo);
}

void ServiceReverse::RestoreOnBundleStarted(int32_t errCode, string bundleName)
{
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onBundleStarted) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksRestore_.onBundleStarted(errCode, bundleName);
}

void ServiceReverse::RestoreOnBundleFinished(int32_t errCode, string bundleName)
{
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onBundleFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    HILOGI("errCode = %{public}d, bundleName = %{public}s", errCode, bundleName.c_str());
    callbacksRestore_.onBundleFinished(errCode, bundleName);
}

void ServiceReverse::RestoreOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onAllBundlesFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksRestore_.onAllBundlesFinished(errCode);
}

void ServiceReverse::RestoreOnFileReady(string bundleName, string fileName, int fd, int32_t errCode)
{
    HILOGD("begin, bundleName is:%{public}s", bundleName.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksRestore_.onFileReady(bFileInfo, UniqueFd(fd), errCode);
}

void ServiceReverse::RestoreOnResultReport(string result, std::string bundleName, ErrCode errCode)
{
    HILOGI("ServiceReverse RestoreOnResultReport bundle %{public}s begin with result: %{public}s", bundleName.c_str(),
        result.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onResultReport) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksRestore_.onResultReport(bundleName, result);
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onBundleFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksRestore_.onBundleFinished(errCode, bundleName);
}

void ServiceReverse::RestoreOnProcessInfo(std::string bundleName, std::string processInfo)
{
    HILOGI("bundleName = %{public}s", bundleName.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onProcess) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksRestore_.onProcess(bundleName, processInfo);
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