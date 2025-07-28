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

#include "service_reverse_proxy.h"

#include "b_error/b_error.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
ErrCode ServiceReverseProxy::BackupOnFileReady(const std::string &bundleName,
                                               const std::string &fileName,
                                               int fd,
                                               int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::BackupOnBundleStarted(int32_t errCode, const std::string &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::BackupOnResultReport(const std::string &result, const std::string &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::BackupOnBundleFinished(int32_t errCode, const std::string &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::BackupOnAllBundlesFinished(int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::BackupOnProcessInfo(const std::string &bundleName, const std::string &processInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::BackupOnScanningInfo(const std::string &scannedInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::RestoreOnBundleStarted(int32_t errCode, const std::string &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::RestoreOnBundleFinished(int32_t errCode, const std::string &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::RestoreOnAllBundlesFinished(int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::RestoreOnFileReady(const std::string &bundleName,
                                                const std::string &fileName,
                                                int fd,
                                                int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::RestoreOnResultReport(const std::string &result,
                                                   const std::string &bundleName,
                                                   int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::RestoreOnProcessInfo(const std::string &bundleName, const std::string &processInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalBackupOnFileReady(const std::string &bundleName,
                                                          const std::string &fileName,
                                                          int fd,
                                                          int manifestFd,
                                                          int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalBackupOnBundleStarted(int32_t errCode, const std::string &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalBackupOnResultReport(const std::string &result, const std::string &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalBackupOnBundleFinished(int32_t errCode, const std::string &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalBackupOnAllBundlesFinished(int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalBackupOnProcessInfo(const std::string &bundleName,
                                                            const std::string &processInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalBackupOnScanningInfo(const std::string &scannedInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalRestoreOnBundleStarted(int32_t errCode, const std::string &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalRestoreOnBundleFinished(int32_t errCode, const std::string &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalRestoreOnAllBundlesFinished(int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalRestoreOnFileReady(const std::string &bundleName,
                                                           const std::string &fileName,
                                                           int fd,
                                                           int manifestFd,
                                                           int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalRestoreOnFileReadyWithoutFd(const std::string &bundleName,
                                                                    const std::string &fileName,
                                                                    int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalRestoreOnResultReport(const std::string &result,
                                                              const std::string &bundleName,
                                                              int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceReverseProxy::IncrementalRestoreOnProcessInfo(const std::string &bundleName,
                                                             const std::string &processInfo)
{
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup