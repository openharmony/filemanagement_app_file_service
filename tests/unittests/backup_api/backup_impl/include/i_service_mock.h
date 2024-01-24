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

#ifndef MOCK_I_SERVICE_MOCK_H
#define MOCK_I_SERVICE_MOCK_H

#include <fcntl.h>
#include <gmock/gmock.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "i_service.h"
#include "iremote_stub.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
class IServiceMock : public IRemoteStub<IService> {
public:
    int code_ = 0;
    IServiceMock() : code_(0) {}
    virtual ~IServiceMock() {}

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));

    int32_t InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        reply.WriteInt32(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    }

    int32_t InvokeGetLocalSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        std::string fileName = "1.tar";
        TestManager tm("GetLocalCap_GetFd_0100");
        std::string filePath = tm.GetRootDirCurTest().append(fileName);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        reply.WriteFileDescriptor(fd);
        return BError(BError::Codes::OK);
    }

    ErrCode InitRestoreSession(sptr<IServiceReverse> remote) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode InitBackupSession(sptr<IServiceReverse> remote) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode Start() override
    {
        return BError(BError::Codes::OK);
    }

    UniqueFd GetLocalCapabilities() override
    {
        return UniqueFd(-1);
    }

    ErrCode PublishFile(const BFileInfo &fileInfo) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppFileReady(const std::string &fileName, UniqueFd fd) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppDone(ErrCode errCode) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode GetFileHandle(const std::string &bundleName, const std::string &fileName) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppendBundlesRestoreSession(UniqueFd fd,
                                        const std::vector<BundleName> &bundleNames,
                                        RestoreTypeEnum restoreType,
                                        int32_t userId) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppendBundlesBackupSession(const std::vector<BundleName> &bundleNames) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode Finish() override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode Release() override
    {
        return BError(BError::Codes::OK);
    }

    UniqueFd GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames) override
    {
        return UniqueFd(-1);
    }

    ErrCode InitIncrementalBackupSession(sptr<IServiceReverse> remote) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode PublishIncrementalFile(const BFileInfo &fileInfo)
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppIncrementalFileReady(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd)
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppIncrementalDone(ErrCode errCode)
    {
        return BError(BError::Codes::OK);
    }

    ErrCode GetIncrementalFileHandle(const std::string &bundleName, const std::string &fileName)
    {
        return BError(BError::Codes::OK);
    }
};
} // namespace OHOS::FileManagement::Backup
#endif // MOCK_I_SERVICE_MOCK_H