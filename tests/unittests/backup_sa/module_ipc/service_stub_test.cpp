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

#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <message_parcel.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
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
    MOCK_METHOD2(AppFileReady, ErrCode(const string &fileName, UniqueFd fd));
    MOCK_METHOD1(AppDone, ErrCode(ErrCode errCode));
    MOCK_METHOD2(GetFileHandle, ErrCode(const string &bundleName, const string &fileName));
    MOCK_METHOD2(AppendBundlesRestoreSession, ErrCode(UniqueFd fd, const std::vector<BundleName> &bundleNames));
    MOCK_METHOD1(AppendBundlesBackupSession, ErrCode(const std::vector<BundleName> &bundleNames));
    MOCK_METHOD0(Finish, ErrCode());
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

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IService::SERVICE_CMD_INIT_RESTORE_SESSION, data, reply, option));
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
                  service.OnRemoteRequest(IService::SERVICE_CMD_INIT_BACKUP_SESSION, data, reply, option));
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
        EXPECT_EQ(BError(BError::Codes::OK), service.OnRemoteRequest(IService::SERVICE_CMD_START, data, reply, option));
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

        EXPECT_EQ(BError(BError::Codes::OK),
                  serviceSptr->OnRemoteRequest(IService::SERVICE_CMD_GET_LOCAL_CAPABILITIES, data, reply, option));
        UniqueFd fd(reply.ReadFileDescriptor());
        EXPECT_GT(fd, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "ServiceStubTest-CmdGetLocalCapabilities Brances";
        MessageParcel brances;
        EXPECT_TRUE(brances.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_NE(BError(BError::Codes::OK),
                  serviceSptr->OnRemoteRequest(IService::SERVICE_CMD_GET_LOCAL_CAPABILITIES, brances, reply, option));
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
                  service.OnRemoteRequest(IService::SERVICE_CMD_PUBLISH_FILE, data, reply, option));
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
        EXPECT_CALL(service, AppFileReady(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        TestManager tm("ServiceStub_GetFd_0200");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));

        EXPECT_TRUE(data.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(FILE_NAME));
        EXPECT_TRUE(data.WriteFileDescriptor(fd));
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IService::SERVICE_CMD_APP_FILE_READY, data, reply, option));
        GTEST_LOG_(INFO) << "ServiceStubTest-begin-CmdAppFileReady Brances";
        MessageParcel brances;
        EXPECT_TRUE(brances.WriteInterfaceToken(IService::GetDescriptor()));
        EXPECT_TRUE(brances.WriteString(FILE_NAME));
        EXPECT_NE(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IService::SERVICE_CMD_APP_FILE_READY, brances, reply, option));
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
                  service.OnRemoteRequest(IService::SERVICE_CMD_APP_DONE, data, reply, option));
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
                  service.OnRemoteRequest(IService::SERVICE_CMD_GET_FILE_NAME, data, reply, option));
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
        EXPECT_CALL(service, AppendBundlesRestoreSession(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
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
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IService::SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION, data, reply, option));
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
                  service.OnRemoteRequest(IService::SERVICE_CMD_APPEND_BUNDLES_BACKUP_SESSION, data, reply, option));
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
                  service.OnRemoteRequest(IService::SERVICE_CMD_FINISH, data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by Finish.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_Finish_0100";
}
} // namespace OHOS::FileManagement::Backup