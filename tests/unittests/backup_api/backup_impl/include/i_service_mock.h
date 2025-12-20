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

#ifndef MOCK_I_SERVICE_MOCK_H
#define MOCK_I_SERVICE_MOCK_H

#include <fcntl.h>
#include <gmock/gmock.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "iservice.h"
#include "b_resources/b_constants.h"
#include "iremote_stub.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
class IServiceMock : public IRemoteStub<IService> {
public:
    int code_ = 0;
    IServiceMock() : code_(0) {}
    virtual ~IServiceMock() {}

    MOCK_METHOD(int, SendRequest, (uint32_t, MessageParcel &, MessageParcel &, MessageOption &));

    MOCK_METHOD(ErrCode, AppFileReady, (const std::string &fileName, int, int32_t errCode));
    MOCK_METHOD(ErrCode, AppFileReadyWithoutFd, (const std::string &fileName, int32_t errCode));
    MOCK_METHOD(ErrCode, AppDone, (int32_t errCode));
    MOCK_METHOD(ErrCode, AppIncrementalFileReady, (const std::string &, int, int, int32_t));
    MOCK_METHOD(ErrCode, AppIncrementalFileReadyWithoutFd, (const std::string &, int32_t));
    MOCK_METHOD(ErrCode, AppIncrementalDone, (ErrCode errCode));
    MOCK_METHOD(ErrCode, GetExtOnRelease, (bool&));
    MOCK_METHOD(ErrCode, ServiceResultReport, (const std::string&, BackupRestoreScenario, ErrCode));
    MOCK_METHOD(ErrCode, GetLocalCapabilities, (int& fd));
    MOCK_METHOD(ErrCode, GetLocalCapabilitiesForBundleInfos, (int& fd));

    int32_t InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        reply.WriteInt32(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    }

    int32_t InvokeMsgSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        reply.WriteString("");
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

    ErrCode InitRestoreSession(const sptr<IServiceReverse>& remote) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode InitRestoreSessionWithErrMsg(const sptr<IServiceReverse>& remote, int32_t& errCode,
        std::string &errMsg) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode InitBackupSession(const sptr<IServiceReverse>& remote) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode InitBackupSessionWithErrMsg(const sptr<IServiceReverse>& remote, int32_t& errCode,
        std::string &errMsg) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode Start() override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode PublishFile(const BFileInfo &fileInfo) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode GetFileHandle(const std::string &bundleName, const std::string &fileName) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppendBundlesRestoreSessionDataByDetail(int fd,
                                        const std::vector<BundleName> &bundleNames,
                                        const std::vector<std::string> &detailInfos,
                                        int32_t restoreType,
                                        int32_t userId) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppendBundlesRestoreSessionData(int fd,
                                        const std::vector<BundleName> &bundleNames,
                                        int32_t restoreType,
                                        int32_t userId) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppendBundlesBackupSession(const std::vector<BundleName> &bundleNames) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppendBundlesDetailsBackupSession(const std::vector<BundleName> &bundleNames,
                                              const std::vector<std::string> &bundleInfos) override
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

    ErrCode CancelForResult(const std::string& bundleName, int32_t& result) override
    {
        result = BError(BError::Codes::OK);
        return BError(BError::Codes::OK);
    }

    ErrCode GetAppLocalListAndDoIncrementalBackup() override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode GetBackupInfo(const std::string& bundleName, std::string& result) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode GetIncrementalFileHandle(const std::string &bundleName, const std::string &fileName)
    {
        return BError(BError::Codes::OK);
    }

    ErrCode UpdateTimer(const std::string &bundleName, uint32_t timeout, bool &result) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode UpdateSendRate(const std::string &bundleName, int32_t sendRate, bool &result) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode StartExtTimer(bool &isExtStart) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode StartFwkTimer(bool &isFwkStart) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode StopExtTimer(bool &isExtStop) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData>& bundleNames, int& fd) override
    {
        return UniqueFd(-1);
    }

    ErrCode InitIncrementalBackupSession(const sptr<IServiceReverse>& remote) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode InitIncrementalBackupSessionWithErrMsg(const sptr<IServiceReverse>& remote, int32_t& errCode,
        std::string &errMsg) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode AppendBundlesIncrementalBackupSessionWithBundleInfos(const std::vector<BIncrementalData> &bundlesToBackup,
        const std::vector<std::string> &infos) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode PublishIncrementalFile(const BFileInfo &fileInfo) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode PublishSAIncrementalFile(const BFileInfo &fileInfo, int fd) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode ReportAppProcessInfo(const std::string& processInfo, BackupRestoreScenario scenario)
    {
        return BError(BError::Codes::OK);
    }

    ErrCode RefreshDataSize(int64_t totalsize) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode GetBackupDataSize(bool isPreciseScan, const std::vector<BIncrementalData>& bundleNameList)
    {
        return BError(BError::Codes::OK);
    }

    ErrCode CleanBundleTempDir(const std::string &bundleName) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode HandleExtDisconnect(BackupRestoreScenario sennario, bool isAppResultReport, int32_t appErrCode) override
    {
        return BError(BError::Codes::OK);
    }

    ErrCode GetCompatibilityInfo(const std::string &bundleName, const std::string &extInfo, std::string &compatInfo)
    {
        return BError(BError::Codes::OK);
    }
    Errcode StartCleanData(int triggerType, unsigned int writeSize, unsigned int waitTime)
    {
        return BError(BError::Codes::OK); 
    }
};
} // namespace OHOS::FileManagement::Backup
#endif // MOCK_I_SERVICE_MOCK_H