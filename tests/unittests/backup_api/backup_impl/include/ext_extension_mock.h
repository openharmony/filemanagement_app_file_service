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

#ifndef MOCK_EXTENSION_MOCK_H
#define MOCK_EXTENSION_MOCK_H

#include <fcntl.h>
#include <gmock/gmock.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "i_extension.h"
#include "iremote_stub.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
class BackupExtExtensionMock : public IRemoteStub<IExtension> {
public:
    int code_ = 0;
    BackupExtExtensionMock() : code_(0) {}
    virtual ~BackupExtExtensionMock() {}

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));

    int32_t InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        reply.WriteInt32(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    }

    int32_t InvokeGetFileHandleRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        std::string fileName = "1.tar";
        TestManager tm("GetFileHand_GetFd_0100");
        std::string filePath = tm.GetRootDirCurTest().append(fileName);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        reply.WriteBool(true);
        reply.WriteInt32(0);
        reply.WriteFileDescriptor(fd);
        return BError(BError::Codes::OK);
    }

    UniqueFd GetFileHandle(const std::string &fileName, int32_t &errCode) override
    {
        GTEST_LOG_(INFO) << "GetFileHandle" << fileName;
        if (fileName == "testName") {
            errCode = BError::BackupErrorCode::E_UKERR;
            return UniqueFd(-1);
        }

        if (fileName.empty()) {
            errCode = BError::BackupErrorCode::E_UKERR;
            return UniqueFd(-1);
        }
        TestManager tm("GetFileHand_GetFd_0200");
        std::string filePath = tm.GetRootDirCurTest().append(fileName);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        if (fd < 0) {
            errCode = BError::GetCodeByErrno(errno);
        }
        return fd;
    };

    ErrCode HandleClear() override
    {
        return BError(BError::Codes::OK);
    };

    ErrCode HandleBackup(bool isClearData) override
    {
        GTEST_LOG_(INFO) << "HandleBackup";
        if (nHandleBackupNum_ == 1) {
            GTEST_LOG_(INFO) << "HandleBackup is false";
            return 1;
        }
        nHandleBackupNum_++;
        return BError(BError::Codes::OK);
    };

    ErrCode PublishFile(const std::string &fileName) override
    {
        GTEST_LOG_(INFO) << "PublishFile " << fileName;
        if (fileName == "test") {
            return 1;
        }
        return BError(BError::Codes::OK);
    };

    ErrCode HandleRestore(bool isClearData) override
    {
        return BError(BError::Codes::OK);
    };

    std::tuple<ErrCode, UniqueFd, UniqueFd> GetIncrementalFileHandle(const std::string &fileName) override
    {
        return {BError(BError::Codes::OK), UniqueFd(-1), UniqueFd(-1)};
    };

    ErrCode PublishIncrementalFile(const std::string &fileName) override
    {
        return BError(BError::Codes::OK);
    };

    ErrCode HandleIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd) override
    {
        return BError(BError::Codes::OK);
    };

    ErrCode IncrementalOnBackup(bool isClearData) override
    {
        return BError(BError::Codes::OK);
    };

    std::tuple<UniqueFd, UniqueFd> GetIncrementalBackupFileHandle() override
    {
        return {UniqueFd(-1), UniqueFd(-1)};
    };

    ErrCode GetBackupInfo(std::string &result) override
    {
        return BError(BError::Codes::OK);
    };

    ErrCode UpdateFdSendRate(std::string &bundleName, int32_t sendRate) override
    {
        return BError(BError::Codes::OK);
    };

    ErrCode User0OnBackup() override
    {
        return BError(BError::Codes::OK);
    };

private:
    int32_t nHandleBackupNum_ = 0;
};
} // namespace OHOS::FileManagement::Backup
#endif // MOCK_EXTENSION_MOCK_H