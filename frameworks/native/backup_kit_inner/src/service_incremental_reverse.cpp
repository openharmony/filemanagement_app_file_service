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
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void ServiceReverse::IncrementalBackupOnFileReady(string bundleName, string fileName, int fd, int manifestFd,
    int32_t errCode)
{
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksIncrementalBackup_.onFileReady(bFileInfo, UniqueFd(fd), UniqueFd(manifestFd), errCode);
}

void ServiceReverse::IncrementalBackupOnBundleStarted(int32_t errCode, string bundleName)
{
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onBundleStarted) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksIncrementalBackup_.onBundleStarted(errCode, bundleName);
}

void ServiceReverse::IncrementalBackupOnResultReport(std::string result, std::string bundleName)
{
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onResultReport) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksIncrementalBackup_.onResultReport(bundleName, result);
}

void ServiceReverse::IncrementalBackupOnBundleFinished(int32_t errCode, string bundleName)
{
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onBundleFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    HILOGI("errCode = %{public}d, bundleName = %{public}s", errCode, bundleName.c_str());
    callbacksIncrementalBackup_.onBundleFinished(errCode, bundleName);
}

void ServiceReverse::IncrementalBackupOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onAllBundlesFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksIncrementalBackup_.onAllBundlesFinished(errCode);
}

void ServiceReverse::IncrementalBackupOnProcessInfo(std::string bundleName, std::string processInfo)
{
    HILOGI("bundleName = %{public}s", bundleName.c_str());
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onProcess) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksIncrementalBackup_.onProcess(bundleName, processInfo);
}

void ServiceReverse::IncrementalRestoreOnBundleStarted(int32_t errCode, string bundleName)
{
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onBundleStarted) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksIncrementalRestore_.onBundleStarted(errCode, bundleName);
}

void ServiceReverse::IncrementalRestoreOnBundleFinished(int32_t errCode, string bundleName)
{
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onBundleFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    HILOGI("errCode = %{public}d, bundleName = %{public}s", errCode, bundleName.c_str());
    callbacksIncrementalRestore_.onBundleFinished(errCode, bundleName);
}

void ServiceReverse::IncrementalRestoreOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onAllBundlesFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksIncrementalRestore_.onAllBundlesFinished(errCode);
}

void ServiceReverse::IncrementalRestoreOnFileReady(string bundleName, string fileName, int fd, int manifestFd,
    int32_t errCode)
{
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onFileReady) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksIncrementalRestore_.onFileReady(bFileInfo, UniqueFd(fd), UniqueFd(manifestFd), errCode);
}

void ServiceReverse::IncrementalRestoreOnResultReport(std::string result, std::string bundleName, ErrCode errCode)
{
    HILOGI("begin incremental restore on result report, bundleName:%{public}s", bundleName.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onResultReport) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksIncrementalRestore_.onResultReport(bundleName, result);
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onBundleFinished) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksIncrementalRestore_.onBundleFinished(errCode, bundleName);
}

void ServiceReverse::IncrementalRestoreOnProcessInfo(std::string bundleName, std::string processInfo)
{
    HILOGI("begin incremental report processInfo, bundleName:%{public}s", bundleName.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onProcess) {
        HILOGE("Error scenario or callback is nullptr, scenario = %{public}d", scenario_);
        return;
    }
    callbacksIncrementalRestore_.onProcess(bundleName, processInfo);
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