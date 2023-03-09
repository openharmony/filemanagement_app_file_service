/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

void ServiceReverse::BackupOnFileReady(string bundleName, string fileName, int fd)
{
    HILOGI("begin");
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onFileReady) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksBackup_.onFileReady(bFileInfo, UniqueFd(fd));
}

void ServiceReverse::BackupOnBundleStarted(int32_t errCode, string bundleName)
{
    HILOGI("begin");
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onBundleStarted) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksBackup_.onBundleStarted(errCode, bundleName);
}

void ServiceReverse::BackupOnBundleFinished(int32_t errCode, string bundleName)
{
    HILOGI("begin");
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onBundleFinished) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksBackup_.onBundleFinished(errCode, bundleName);
}

void ServiceReverse::BackupOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onAllBundlesFinished) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksBackup_.onAllBundlesFinished(errCode);
}

void ServiceReverse::RestoreOnBundleStarted(int32_t errCode, string bundleName)
{
    HILOGI("begin");
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onBundleStarted) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksRestore_.onBundleStarted(errCode, bundleName);
}

void ServiceReverse::RestoreOnBundleFinished(int32_t errCode, string bundleName)
{
    HILOGI("begin");
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onBundleFinished) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksRestore_.onBundleFinished(errCode, bundleName);
}

void ServiceReverse::RestoreOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onAllBundlesFinished) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    callbacksRestore_.onAllBundlesFinished(errCode);
}

void ServiceReverse::RestoreOnFileReady(string bundleName, string fileName, int fd)
{
    HILOGI("begin");
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onFileReady) {
        HILOGI("Error scenario or callback is nullptr");
        return;
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksRestore_.onFileReady(bFileInfo, UniqueFd(fd));
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