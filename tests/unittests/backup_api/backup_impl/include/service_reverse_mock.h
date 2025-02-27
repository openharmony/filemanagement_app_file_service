/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef MOCK_SERVICE_REVERSE_MOCK_H
#define MOCK_SERVICE_REVERSE_MOCK_H

#include <gmock/gmock.h>

#include "b_error/b_error.h"
#include "iservice_reverse.h"
#include "iremote_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceReverseMock : public IRemoteStub<IServiceReverse> {
public:
    int code_ = 0;
    ServiceReverseMock() : code_(0) {}
    virtual ~ServiceReverseMock() {}

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));

    int32_t InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        return BError(BError::Codes::OK);
    }
    void BackupOnFileReady(std::string bundleName, std::string fileName, int fd, int32_t errCode) override {}
    void BackupOnBundleStarted(int32_t errCode, std::string bundleName) override {}
    void BackupOnResultReport(std::string result, std::string bundleName) override {};
    void BackupOnBundleFinished(int32_t errCode, std::string bundleName) override {}
    void BackupOnAllBundlesFinished(int32_t errCode) override {}
    void BackupOnProcessInfo(std::string bundleName, std::string processInfo) override {}
    void BackupOnScanningInfo(std::string scannedInfo) override {}

    void RestoreOnBundleStarted(int32_t errCode, std::string bundleName) override {}
    void RestoreOnBundleFinished(int32_t errCode, std::string bundleName) override {}
    void RestoreOnAllBundlesFinished(int32_t errCode) override {}
    void RestoreOnFileReady(std::string bundleName, std::string fileName, int fd, int32_t errCode) override {}
    void RestoreOnResultReport(std::string result, std::string bundleName, ErrCode errCode) override {}
    void RestoreOnProcessInfo(std::string bundleName, std::string processInfo) override {}

    void IncrementalBackupOnFileReady(std::string bundleName, std::string fileName, int fd, int manifestFd,
        int32_t errCode) override {}
    void IncrementalBackupOnBundleStarted(int32_t errCode, std::string bundleName) override {}
    void IncrementalBackupOnResultReport(std::string result, std::string bundleName) override {}
    void IncrementalBackupOnBundleFinished(int32_t errCode, std::string bundleName) override {}
    void IncrementalBackupOnAllBundlesFinished(int32_t errCode) override {}
    void IncrementalBackupOnProcessInfo(std::string bundleName, std::string processInfo) override {}
    void IncrementalBackupOnScanningInfo(std::string scannedInfo) override {}

    void IncrementalRestoreOnBundleStarted(int32_t errCode, std::string bundleName) override {}
    void IncrementalRestoreOnBundleFinished(int32_t errCode, std::string bundleName) override {}
    void IncrementalRestoreOnAllBundlesFinished(int32_t errCode) override {}
    void IncrementalRestoreOnFileReady(std::string bundleName, std::string fileName, int fd, int manifestFd,
        int32_t errCode) override {}
    void IncrementalRestoreOnResultReport(std::string result, std::string bundleName, ErrCode errCode) override {};
    void IncrementalRestoreOnProcessInfo(std::string bundleName, std::string processInfo) override {}
};
} // namespace OHOS::FileManagement::Backup
#endif // MOCK_SERVICE_REVERSE_MOCK_H