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

#include "module_ipc/service_reverse_proxy.h"

#include "b_error/b_error.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void ServiceReverseProxy::BackupOnFileReady(string bundleName, string fileName, int fd) {}

void ServiceReverseProxy::BackupOnBundleStarted(int32_t errCode, string bundleName) {}

void ServiceReverseProxy::BackupOnBundleFinished(int32_t errCode, string bundleName) {}

void ServiceReverseProxy::BackupOnAllBundlesFinished(int32_t errCode) {}

void ServiceReverseProxy::RestoreOnBundleStarted(int32_t errCode, string bundleName) {}

void ServiceReverseProxy::RestoreOnBundleFinished(int32_t errCode, string bundleName) {}

void ServiceReverseProxy::RestoreOnAllBundlesFinished(int32_t errCode) {}

void ServiceReverseProxy::RestoreOnFileReady(string bundleName, string fileName, int fd) {}
} // namespace OHOS::FileManagement::Backup