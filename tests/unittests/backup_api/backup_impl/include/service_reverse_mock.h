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
    bool restoreBundleStartCalled_ = false;
    bool incRestoreBundleStartCalled_ = false;
    bool backupOnBundleStartedCalled_ = false;
    ServiceReverseMock() : code_(0) {}
    virtual ~ServiceReverseMock() {}

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));

    int32_t InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        return BError(BError::Codes::OK);
    }
    ErrCode BackupOnFileReady(const std::string &bundleName,
                              const std::string &fileName,
                              int fd,
                              int32_t errCode) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode BackupOnBundleStarted(int32_t errCode, const std::string &bundleName) override
    {
        backupOnBundleStartedCalled_ = true;
        return BError(BError::Codes::OK);
    }

    ErrCode BackupOnResultReport(const std::string &result, const std::string &bundleName) override
    {
        return BError(BError::Codes::OK);
    };

    ErrCode BackupOnBundleFinished(int32_t errCode, const std::string &bundleName) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode BackupOnAllBundlesFinished(int32_t errCode) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode BackupOnProcessInfo(const std::string &bundleName, const std::string &processInfo) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode BackupOnScanningInfo(const std::string &scannedInfo) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode RestoreOnBundleStarted(int32_t errCode, const std::string &bundleName) override
    {
        GTEST_LOG_(INFO) << "call RestoreOnBundleStarted";
        restoreBundleStartCalled_ = true;
        return BError(BError::Codes::OK);
    }

    ErrCode RestoreOnBundleFinished(int32_t errCode, const std::string &bundleName) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode RestoreOnAllBundlesFinished(int32_t errCode) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode RestoreOnFileReady(const std::string &bundleName,
                               const std::string &fileName,
                               int fd,
                               int32_t errCode) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode RestoreOnResultReport(const std::string &result, const std::string &bundleName, ErrCode errCode) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode RestoreOnProcessInfo(const std::string &bundleName, const std::string &processInfo) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalBackupOnFileReady(const std::string &bundleName,
                                         const std::string &fileName,
                                         int fd,
                                         int manifestFd,
                                         int32_t errCode) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalSaBackupOnFileReady(const std::string &bundleName,
                                           const std::string &fileName,
                                           int fd,
                                           int32_t errCode) override
    {
        return BError(BError::Codes::OK);
    }
    
    ErrCode IncrementalBackupOnBundleStarted(int32_t errCode, const std::string &bundleName) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalBackupOnResultReport(const std::string &result, const std::string &bundleName) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalBackupOnBundleFinished(int32_t errCode, const std::string &bundleName) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalBackupOnAllBundlesFinished(int32_t errCode) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalBackupOnProcessInfo(const std::string &bundleName, const std::string &processInfo) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalBackupOnScanningInfo(const std::string &scannedInfo) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalRestoreOnBundleStarted(int32_t errCode, const std::string &bundleName) override
    {
        GTEST_LOG_(INFO) << "call IncrementalRestoreOnBundleStarted";
        incRestoreBundleStartCalled_ = true;
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalRestoreOnBundleFinished(int32_t errCode, const std::string &bundleName) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalRestoreOnAllBundlesFinished(int32_t errCode) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalRestoreOnFileReady(const std::string &bundleName,
                                          const std::string &fileName,
                                          int fd,
                                          int manifestFd,
                                          int32_t errCode) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalRestoreOnResultReport(const std::string &result,
                                             const std::string &bundleName,
                                             int32_t errCode) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode IncrementalRestoreOnProcessInfo(const std::string &bundleName, const std::string &processInfo) override
    {
        return BError(BError::Codes::OK);
    }
};
} // namespace OHOS::FileManagement::Backup
#endif // MOCK_SERVICE_REVERSE_MOCK_H