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

#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <message_parcel.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "i_service.h"
#include "module_ipc/service_stub.h"
#include "service_reverse_mock.h"
#include "test_manager.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string FILE_NAME = "1.tar";
} // namespace

class MockService final : public ServiceStub {
public:
    MOCK_METHOD1(InitRestoreSession, ErrCode(sptr<IServiceReverse> remote));
    MOCK_METHOD1(InitBackupSession, ErrCode(sptr<IServiceReverse> remote));
    MOCK_METHOD0(Start, ErrCode());
    MOCK_METHOD0(GetLocalCapabilities, UniqueFd());
    MOCK_METHOD1(PublishFile, ErrCode(const BFileInfo &fileInfo));
    MOCK_METHOD3(AppFileReady, ErrCode(const string &fileName, UniqueFd fd, int32_t errCode));
    MOCK_METHOD1(AppDone, ErrCode(ErrCode errCode));
    MOCK_METHOD2(ServiceResultReport, ErrCode(const string restoreRetInfo, BackupRestoreScenario scenario));
    MOCK_METHOD2(GetFileHandle, ErrCode(const string &bundleName, const string &fileName));
    MOCK_METHOD5(
        AppendBundlesRestoreSession,
        ErrCode(UniqueFd fd, const std::vector<BundleName> &bundleNames, const std::vector<std::string> &detailInfos,
        RestoreTypeEnum restoreType, int32_t userId));
    MOCK_METHOD4(
        AppendBundlesRestoreSession,
        ErrCode(UniqueFd fd, const std::vector<BundleName> &bundleNames, RestoreTypeEnum restoreType, int32_t userId));
    MOCK_METHOD1(AppendBundlesBackupSession, ErrCode(const std::vector<BundleName> &bundleNames));
    MOCK_METHOD2(AppendBundlesDetailsBackupSession,
                 ErrCode(const std::vector<BundleName> &bundleNames, const std::vector<std::string> &bundleInfos));
    MOCK_METHOD0(Finish, ErrCode());
    MOCK_METHOD0(Release, ErrCode());
    MOCK_METHOD1(GetLocalCapabilitiesIncremental, UniqueFd(const std::vector<BIncrementalData> &bundleNames));
    MOCK_METHOD1(InitIncrementalBackupSession, ErrCode(sptr<IServiceReverse> remote));
    MOCK_METHOD1(AppendBundlesIncrementalBackupSession, ErrCode(const std::vector<BIncrementalData> &bundlesToBackup));

    MOCK_METHOD1(PublishIncrementalFile, ErrCode(const BFileInfo &fileInfo));
    MOCK_METHOD2(PublishSAIncrementalFile, ErrCode(const BFileInfo &fileInfo, UniqueFd fd));
    MOCK_METHOD4(AppIncrementalFileReady, ErrCode(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd,
        int32_t errCode));
    MOCK_METHOD1(AppIncrementalDone, ErrCode(ErrCode errCode));
    MOCK_METHOD2(GetIncrementalFileHandle, ErrCode(const std::string &bundleName, const std::string &fileName));
    MOCK_METHOD2(GetBackupInfo, ErrCode(string &bundleName, string &result));
    MOCK_METHOD3(UpdateTimer, ErrCode(BundleName &bundleName, uint32_t timeOut, bool &result));
    MOCK_METHOD0(GetAppLocalListAndDoIncrementalBackup, ErrCode());
    UniqueFd InvokeGetLocalCapabilities()
    {
        if (bCapabilities_) {
            return UniqueFd(-1);
        }
        TestManager tm("MockService_GetFd_0100");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        bCapabilities_ = true;
        return fd;
    }

private:
    bool bCapabilities_ = {false};
};

class ServiceStubTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.number: SUB_backup_sa_ServiceStub_InitRestoreSession_0100
 * @tc.name: SUB_backup_sa_ServiceStub_InitRestoreSession_0100
 * @tc.desc: Test function of InitRestoreSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_InitRestoreSession_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_InitRestoreSession_0100";
    try {
        MockService service;
        EXPECT_CALL(service, InitRestoreSession(_)).WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        sptr<ServiceReverseMock> remote = sptr(new ServiceReverseMock());
        EXPECT_TRUE(data.WriteRemoteObject(remote->AsObject().GetRefPtr()));

        EXPECT_EQ(
            BError(BError::Codes::OK),
            service.OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_RESTORE_SESSION),
                                    data, reply, option));
        remote = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by InitRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_InitRestoreSession_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_InitBackupSession_0100
 * @tc.name: SUB_backup_sa_ServiceStub_InitBackupSession_0100
 * @tc.desc: Test function of InitBackupSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_InitBackupSession_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_InitBackupSession_0100";
    try {
        MockService service;
        EXPECT_CALL(service, InitBackupSession(_)).WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        sptr<ServiceReverseMock> remote = sptr(new ServiceReverseMock());

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(data.WriteRemoteObject(remote->AsObject().GetRefPtr()));

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_INIT_BACKUP_SESSION),
                                          data, reply, option));
        remote = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by InitBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_InitBackupSession_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_Start_0100
 * @tc.name: SUB_backup_sa_ServiceStub_Start_0100
 * @tc.desc: Test function of Start interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_Start_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_Start_0100";
    try {
        MockService service;
        EXPECT_CALL(service, Start()).WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_START), data, reply,
                                          option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by Start.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_Start_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_GetLocalCapabilities_0100
 * @tc.name: SUB_backup_sa_ServiceStub_GetLocalCapabilities_0100
 * @tc.desc: Test function of GetLocalCapabilities interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_GetLocalCapabilities_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_GetLocalCapabilities_0100";
    try {
        sptr<MockService> serviceSptr = sptr(new MockService());
        EXPECT_CALL(*serviceSptr, GetLocalCapabilities())
            .Times(2)
            .WillOnce(Invoke(serviceSptr.GetRefPtr(), &MockService::InvokeGetLocalCapabilities))
            .WillOnce(Invoke(serviceSptr.GetRefPtr(), &MockService::InvokeGetLocalCapabilities));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));

        EXPECT_EQ(
            BError(BError::Codes::OK),
            serviceSptr->OnRemoteRequest(
                static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_LOCAL_CAPABILITIES), data, reply, option));
        UniqueFd fd(reply.ReadFileDescriptor());
        EXPECT_GT(fd, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "ServiceStubTest-CmdGetLocalCapabilities Brances";
        MessageParcel brances;
        EXPECT_TRUE(brances.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_NE(BError(BError::Codes::OK),
                  serviceSptr->OnRemoteRequest(
                      static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_LOCAL_CAPABILITIES), brances, reply,
                      option));
        serviceSptr = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_GetLocalCapabilities_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_PublishFile_0100
 * @tc.name: SUB_backup_sa_ServiceStub_PublishFile_0100
 * @tc.desc: Test function of PublishFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_PublishFile_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_PublishFile_0100";
    try {
        MockService service;
        EXPECT_CALL(service, PublishFile(_)).WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        BFileInfo fileInfo {BUNDLE_NAME, FILE_NAME, -1};
        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(data.WriteParcelable(&fileInfo));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_PUBLISH_FILE), data,
                                          reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by PublishFile.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_PublishFile_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppFileReady_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppFileReady_0100
 * @tc.desc: Test function of AppFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppFileReady_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppFileReady_0100";
    try {
        MockService service;
        EXPECT_CALL(service, AppFileReady(_, _, _))
            .WillOnce(Return(BError(BError::Codes::OK)))
            .WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        TestManager tm("ServiceStub_GetFd_0200");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(FILE_NAME));
        EXPECT_TRUE(data.WriteBool(true));
        EXPECT_TRUE(data.WriteFileDescriptor(fd));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_FILE_READY),
                                          data, reply, option));
        MessageParcel brances;
        EXPECT_TRUE(brances.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(brances.WriteString(FILE_NAME));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_FILE_READY),
                                          brances, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppFileReady_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppDone_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppDone_0100
 * @tc.desc: Test function of AppDone interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppDone_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppDone_0100";
    try {
        MockService service;
        EXPECT_CALL(service, AppDone(_)).WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APP_DONE), data,
                                          reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppDone.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppDone_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_GetFileHandle_0100
 * @tc.name: SUB_backup_sa_ServiceStub_GetFileHandle_0100
 * @tc.desc: Test function of GetFileHandle interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_GetFileHandle_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_GetFileHandle_0100";
    try {
        MockService service;
        EXPECT_CALL(service, GetFileHandle(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        option.SetFlags(MessageOption::TF_ASYNC);

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));
        EXPECT_TRUE(data.WriteString(FILE_NAME));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_FILE_NAME), data,
                                          reply, option));
        EXPECT_NE(BError(BError::Codes::OK), service.OnRemoteRequest(3333, data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_GetFileHandle_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0100
 * @tc.desc: Test function of AppendBundlesRestoreSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0100";
    try {
        MockService service;
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        vector<BundleName> bundleNames;
        bundleNames.push_back(BUNDLE_NAME);
        TestManager tm("ServiceStub_GetFd_0300");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(data.WriteFileDescriptor(fd));
        EXPECT_TRUE(data.WriteStringVector(bundleNames));
        EXPECT_TRUE(data.WriteInt32(0));
        EXPECT_TRUE(data.WriteInt32(-1));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION), data,
                      reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppendBundlesRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0100";
}


/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0101
 * @tc.name: SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0101
 * @tc.desc: Test function of AppendBundlesRestoreSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0101, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0100";
    try {
        MockService service;
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        vector<BundleName> bundleNames;
        bundleNames.push_back(BUNDLE_NAME);
        TestManager tm("ServiceStub_GetFd_0300");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(data.WriteFileDescriptor(fd));
        EXPECT_TRUE(data.WriteStringVector(bundleNames));
        EXPECT_TRUE(data.WriteInt32(0));
        EXPECT_TRUE(data.WriteInt32(-1));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION), data,
                      reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppendBundlesRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppendBundlesBackupSession_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppendBundlesBackupSession_0100
 * @tc.desc: Test function of AppendBundlesBackupSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppendBundlesBackupSession_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppendBundlesBackupSession_0100";
    try {
        MockService service;
        EXPECT_CALL(service, AppendBundlesBackupSession(_)).WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        vector<BundleName> bundleNames;
        bundleNames.push_back(BUNDLE_NAME);

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(data.WriteStringVector(bundleNames));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_APPEND_BUNDLES_BACKUP_SESSION), data,
                      reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppendBundlesBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppendBundlesBackupSession_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_Finish_0100
 * @tc.name: SUB_backup_sa_ServiceStub_Finish_0100
 * @tc.desc: Test function of AppendBundlesBackupSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_Finish_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_Finish_0100";
    try {
        MockService service;
        EXPECT_CALL(service, Finish()).WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_FINISH), data, reply,
                                          option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by Finish.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_Finish_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_Release_0100
 * @tc.name: SUB_backup_sa_ServiceStub_Release_0100
 * @tc.desc: Test function of Release interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_Release_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_Release_0100";
    try {
        MockService service;
        EXPECT_CALL(service, Release()).WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_RELSEASE_SESSION),
                                          data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by Release.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_Release_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_GetBackupInfo_0100
 * @tc.name: SUB_backup_sa_ServiceStub_GetBackupInfo_0100
 * @tc.desc: Test function of GetBackupInfo interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_GetBackupInfo_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_GetBackupInfo_0100";
    try {
        MockService service;
        EXPECT_CALL(service, GetBackupInfo(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        std::string bundleName = "com.example.app2backup";
        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(bundleName));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_BACKUP_INFO), data,
                      reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by GetBackupInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_GetBackupInfo_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_UpdateTimer_0100
 * @tc.name: SUB_backup_sa_ServiceStub_UpdateTimer_0100
 * @tc.desc: Test function of UpdateTimer interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_UpdateTimer_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_UpdateTimer_0100";
    try {
        MockService service;
        EXPECT_CALL(service, UpdateTimer(_, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        std::string bundleName = "com.example.app2backup";
        uint32_t timeOut = 30000;
        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(bundleName));
        EXPECT_TRUE(data.WriteUint32(timeOut));
        EXPECT_EQ(BError(BError::Codes::OK), service.OnRemoteRequest(
            static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_UPDATE_TIMER), data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by UpdateTimer.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_UpdateTimer_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppendBundlesDetailsRestoreSession_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppendBundlesDetailsRestoreSession_0100
 * @tc.desc: Test function of AppendBundlesDetailsRestoreSession interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppendBundlesDetailsRestoreSession_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppendBundlesDetailsRestoreSession_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        try {
            EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(-1));
            service->CmdAppendBundlesDetailsRestoreSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(false));
            service->CmdAppendBundlesDetailsRestoreSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(false));
            service->CmdAppendBundlesDetailsRestoreSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
            service->CmdAppendBundlesDetailsRestoreSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppendBundlesDetailsRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppendBundlesDetailsRestoreSession_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppendBundlesDetailsRestoreSession_0101
 * @tc.name: SUB_backup_sa_ServiceStub_AppendBundlesDetailsRestoreSession_0101
 * @tc.desc: Test function of AppendBundlesDetailsRestoreSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppendBundlesDetailsRestoreSession_0101,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppendBundlesDetailsRestoreSession_0101";
    try {
        MessageParcel data;
        MessageParcel reply;
        try {
            EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true)).WillOnce(Return(false));
            service->CmdAppendBundlesDetailsRestoreSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*service, AppendBundlesRestoreSession(_, _, _, _, _)).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
            service->CmdAppendBundlesDetailsRestoreSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppendBundlesRestoreSession(_, _, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdAppendBundlesDetailsRestoreSession(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppendBundlesDetailsRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppendBundlesDetailsRestoreSession_0101";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppendBundlesDetailsBackupSession_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppendBundlesDetailsBackupSession_0100
 * @tc.desc: Test function of AppendBundlesDetailsBackupSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppendBundlesDetailsBackupSession_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppendBundlesDetailsBackupSession_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        try {
            EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(false));
            service->CmdAppendBundlesDetailsBackupSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(false));
            service->CmdAppendBundlesDetailsBackupSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*service, AppendBundlesDetailsBackupSession(_, _)).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
            service->CmdAppendBundlesDetailsBackupSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppendBundlesDetailsBackupSession(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdAppendBundlesDetailsBackupSession(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppendBundlesDetailsBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppendBundlesDetailsBackupSession_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_GetLocalCapabilitiesIncremental_0100
 * @tc.name: SUB_backup_sa_ServiceStub_GetLocalCapabilitiesIncremental_0100
 * @tc.desc: Test function of GetLocalCapabilitiesIncremental interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_GetLocalCapabilitiesIncremental_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_GetLocalCapabilitiesIncremental_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        try {
            EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
            service->CmdGetLocalCapabilitiesIncremental(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(1), Return(true)));
            EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
            service->CmdGetLocalCapabilitiesIncremental(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*service, GetLocalCapabilitiesIncremental(_)).WillOnce(Return(UniqueFd(0)));
            EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(false));
            service->CmdGetLocalCapabilitiesIncremental(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, GetLocalCapabilitiesIncremental(_)).WillOnce(Return(UniqueFd(0)));
        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(true));
        auto ret = service->CmdGetLocalCapabilitiesIncremental(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by GetLocalCapabilitiesIncremental.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_GetLocalCapabilitiesIncremental_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_InitIncrementalBackupSession_0100
 * @tc.name: SUB_backup_sa_ServiceStub_InitIncrementalBackupSession_0100
 * @tc.desc: Test function of InitIncrementalBackupSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_InitIncrementalBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_InitIncrementalBackupSession_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        try {
            EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(nullptr));
            service->CmdInitIncrementalBackupSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(remote));
            EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(nullptr));
            service->CmdInitIncrementalBackupSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(remote));
            EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(remote));
            EXPECT_CALL(*service, InitIncrementalBackupSession(_)).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
            service->CmdInitIncrementalBackupSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(remote));
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(remote));
        EXPECT_CALL(*service, InitIncrementalBackupSession(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdInitIncrementalBackupSession(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by InitIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_InitIncrementalBackupSession_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppendBundlesIncrementalBackupSession_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppendBundlesIncrementalBackupSession_0100
 * @tc.desc: Test function of AppendBundlesIncrementalBackupSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppendBundlesIncrementalBackupSession_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppendBundlesIncrementalBackupSession_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        try {
            EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
            service->CmdAppendBundlesIncrementalBackupSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(1), Return(true)));
            EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
            service->CmdAppendBundlesIncrementalBackupSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*service, AppendBundlesIncrementalBackupSession(_)).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
            service->CmdAppendBundlesIncrementalBackupSession(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppendBundlesIncrementalBackupSession(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdAppendBundlesIncrementalBackupSession(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppendBundlesIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_InitIncrementalBackupSession_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_PublishIncrementalFile_0100
 * @tc.name: SUB_backup_sa_ServiceStub_PublishIncrementalFile_0100
 * @tc.desc: Test function of PublishIncrementalFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_PublishIncrementalFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_PublishIncrementalFile_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        try {
            EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
            service->CmdPublishIncrementalFile(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(1));
            EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock, ReadUint32(_)).WillOnce(Return(true));
            EXPECT_CALL(*service, PublishIncrementalFile(_)).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
            service->CmdPublishIncrementalFile(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(1));
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadUint32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, PublishIncrementalFile(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdPublishIncrementalFile(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by PublishIncrementalFile.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_PublishIncrementalFile_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_PublishSAIncrementalFile_0100
 * @tc.name: SUB_backup_sa_ServiceStub_PublishSAIncrementalFile_0100
 * @tc.desc: Test function of PublishSAIncrementalFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_PublishSAIncrementalFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_PublishSAIncrementalFile_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        try {
            EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
            service->CmdPublishSAIncrementalFile(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(1));
            EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock, ReadUint32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
            EXPECT_CALL(*service, PublishSAIncrementalFile(_, _)).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
            service->CmdPublishSAIncrementalFile(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(1));
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadUint32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*service, PublishSAIncrementalFile(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdPublishSAIncrementalFile(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by PublishSAIncrementalFile.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_PublishSAIncrementalFile_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppIncrementalFileReady_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppIncrementalFileReady_0100
 * @tc.desc: Test function of AppIncrementalFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppIncrementalFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppIncrementalFileReady_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        try {
            EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
            service->CmdAppIncrementalFileReady(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(-1));
            service->CmdAppIncrementalFileReady(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0)).WillOnce(Return(-1));
            service->CmdAppIncrementalFileReady(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0)).WillOnce(Return(1));
            EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
            EXPECT_CALL(*service, AppIncrementalFileReady(_, _, _, _)).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
            service->CmdAppIncrementalFileReady(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppIncrementalFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppIncrementalFileReady_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppIncrementalFileReady_0101
 * @tc.name: SUB_backup_sa_ServiceStub_AppIncrementalFileReady_0101
 * @tc.desc: Test function of AppIncrementalFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppIncrementalFileReady_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppIncrementalFileReady_0101";
    try {
        MessageParcel data;
        MessageParcel reply;

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(false));
        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
        EXPECT_CALL(*service, AppIncrementalFileReady(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdAppIncrementalFileReady(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppIncrementalFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppIncrementalFileReady_0101";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppIncrementalDone_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppIncrementalDone_0100
 * @tc.desc: Test function of AppIncrementalDone interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppIncrementalDone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppIncrementalDone_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        try {
            EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
            service->CmdAppIncrementalDone(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*service, AppIncrementalDone(_)).WillOnce(Return(0));
            EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
            service->CmdAppIncrementalDone(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppIncrementalDone(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdAppIncrementalDone(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppIncrementalDone.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppIncrementalDone_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_GetIncrementalFileHandle_0100
 * @tc.name: SUB_backup_sa_ServiceStub_GetIncrementalFileHandle_0100
 * @tc.desc: Test function of GetIncrementalFileHandle interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_GetIncrementalFileHandle_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_GetIncrementalFileHandle_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        try {
            EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
            service->CmdGetIncrementalFileHandle(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(false));
            service->CmdGetIncrementalFileHandle(data, reply);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*service, GetIncrementalFileHandle(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        auto ret = service->CmdGetIncrementalFileHandle(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by GetIncrementalFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_GetIncrementalFileHandle_0100";
}
} // namespace OHOS::FileManagement::Backup