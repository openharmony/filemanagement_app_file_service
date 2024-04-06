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

void ServiceReverse::IncrementalBackupOnFileReady(string bundleName, string fileName, int fd, int manifestFd)
{
    HILOGI("begin");
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onFileReady) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksIncrementalBackup_.onFileReady(bFileInfo, UniqueFd(fd), UniqueFd(manifestFd));
}

void ServiceReverse::IncrementalBackupOnBundleStarted(int32_t errCode, string bundleName)
{
    HILOGI("begin");
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onBundleStarted) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksIncrementalBackup_.onBundleStarted(errCode, bundleName);
}

void ServiceReverse::IncrementalBackupOnResultReport(std::string result)
{
    HILOGI("begin");
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onResultReport) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksIncrementalBackup_.onResultReport(result);
}

void ServiceReverse::IncrementalBackupOnBundleFinished(int32_t errCode, string bundleName)
{
    HILOGI("begin");
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onBundleFinished) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksIncrementalBackup_.onBundleFinished(errCode, bundleName);
}

void ServiceReverse::IncrementalBackupOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::BACKUP || !callbacksIncrementalBackup_.onAllBundlesFinished) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksIncrementalBackup_.onAllBundlesFinished(errCode);
}

void ServiceReverse::IncrementalRestoreOnBundleStarted(int32_t errCode, string bundleName)
{
    HILOGI("begin");
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onBundleStarted) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksIncrementalRestore_.onBundleStarted(errCode, bundleName);
}

void ServiceReverse::IncrementalRestoreOnBundleFinished(int32_t errCode, string bundleName)
{
    HILOGI("begin");
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onBundleFinished) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksIncrementalRestore_.onBundleFinished(errCode, bundleName);
}

void ServiceReverse::IncrementalRestoreOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onAllBundlesFinished) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksIncrementalRestore_.onAllBundlesFinished(errCode);
}

void ServiceReverse::IncrementalRestoreOnFileReady(string bundleName, string fileName, int fd, int manifestFd)
{
    HILOGI("begin");
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onFileReady) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksIncrementalRestore_.onFileReady(bFileInfo, UniqueFd(fd), UniqueFd(manifestFd));
}

void ServiceReverse::IncrementalRestoreOnResultReport(std::string result)
{
    HILOGI("begin");
    if (scenario_ != Scenario::RESTORE || !callbacksIncrementalRestore_.onResultReport) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksIncrementalRestore_.onResultReport(result);
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