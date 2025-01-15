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

#include "module_ipc/service_reverse_proxy.h"

#include "b_error/b_error.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void ServiceReverseProxy::BackupOnFileReady(string bundleName, string fileName, int fd, int32_t errCode) {}

void ServiceReverseProxy::BackupOnBundleStarted(int32_t errCode, string bundleName) {}

void ServiceReverseProxy::BackupOnResultReport(string result, std::string bundleName) {}

void ServiceReverseProxy::BackupOnBundleFinished(int32_t errCode, string bundleName) {}

void ServiceReverseProxy::BackupOnAllBundlesFinished(int32_t errCode) {}

void ServiceReverseProxy::BackupOnProcessInfo(std::string bundleName, std::string processInfo) {}

void ServiceReverseProxy::BackupOnScanningInfo(std::string scannedInfo) {}

void ServiceReverseProxy::RestoreOnBundleStarted(int32_t errCode, string bundleName) {}

void ServiceReverseProxy::RestoreOnBundleFinished(int32_t errCode, string bundleName) {}

void ServiceReverseProxy::RestoreOnAllBundlesFinished(int32_t errCode) {}

void ServiceReverseProxy::RestoreOnFileReady(string bundleName, string fileName, int fd, int32_t errCode) {}

void ServiceReverseProxy::RestoreOnResultReport(string result, string bundleName, ErrCode errCode) {}

void ServiceReverseProxy::RestoreOnProcessInfo(std::string bundleName, std::string processInfo) {}

void ServiceReverseProxy::IncrementalBackupOnFileReady(string bundleName, string fileName, int fd, int manifestFd,
    int32_t errCode) {}

void ServiceReverseProxy::IncrementalBackupOnBundleStarted(int32_t errCode, string bundleName) {}

void ServiceReverseProxy::IncrementalBackupOnResultReport(string result, std::string bundleName) {}

void ServiceReverseProxy::IncrementalBackupOnBundleFinished(int32_t errCode, string bundleName) {}

void ServiceReverseProxy::IncrementalBackupOnAllBundlesFinished(int32_t errCode) {}

void ServiceReverseProxy::IncrementalBackupOnProcessInfo(std::string bundleName, std::string processInfo) {}

void ServiceReverseProxy::IncrementalBackupOnScanningInfo(std::string scannedInfo) {}

void ServiceReverseProxy::IncrementalRestoreOnBundleStarted(int32_t errCode, string bundleName) {}

void ServiceReverseProxy::IncrementalRestoreOnBundleFinished(int32_t errCode, string bundleName) {}

void ServiceReverseProxy::IncrementalRestoreOnAllBundlesFinished(int32_t errCode) {}

void ServiceReverseProxy::IncrementalRestoreOnFileReady(string bundleName, string fileName, int fd, int manifestFd,
    int32_t errCode) {}

void ServiceReverseProxy::IncrementalRestoreOnResultReport(string result, string bundleName, ErrCode errCode) {}

void ServiceReverseProxy::IncrementalRestoreOnProcessInfo(std::string bundleName, std::string processInfo) {}
} // namespace OHOS::FileManagement::Backup