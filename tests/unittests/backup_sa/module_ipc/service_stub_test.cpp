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
#include <memory>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "ipc_types.h"
#include "i_service.h"
#include "message_parcel_mock.h"
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

class ServiceMock final : public ServiceStub {
public:
    MOCK_METHOD1(InitRestoreSession, ErrCode(sptr<IServiceReverse> remote));
    MOCK_METHOD1(InitBackupSession, ErrCode(sptr<IServiceReverse> remote));
    MOCK_METHOD0(Start, ErrCode());
    MOCK_METHOD0(GetLocalCapabilities, UniqueFd());
    MOCK_METHOD1(PublishFile, ErrCode(const BFileInfo &fileInfo));
    MOCK_METHOD3(AppFileReady, ErrCode(const string &fileName, UniqueFd fd, int32_t errCode));
    MOCK_METHOD1(AppDone, ErrCode(ErrCode errCode));
    MOCK_METHOD3(ServiceResultReport, ErrCode(const string restoreRetInfo,
        BackupRestoreScenario scenario, ErrCode errCode));
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
    MOCK_METHOD0(GetAppLocalListAndDoIncrementalBackup, ErrCode());
    MOCK_METHOD1(InitIncrementalBackupSession, ErrCode(sptr<IServiceReverse> remote));
    MOCK_METHOD1(AppendBundlesIncrementalBackupSession, ErrCode(const std::vector<BIncrementalData> &bundlesToBackup));
    MOCK_METHOD2(AppendBundlesIncrementalBackupSession,
        ErrCode(const std::vector<BIncrementalData> &bundlesToBackup, const std::vector<std::string> &infos));

    MOCK_METHOD1(PublishIncrementalFile, ErrCode(const BFileInfo &fileInfo));
    MOCK_METHOD2(PublishSAIncrementalFile, ErrCode(const BFileInfo &fileInfo, UniqueFd fd));
    MOCK_METHOD4(AppIncrementalFileReady, ErrCode(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd,
        int32_t errCode));
    MOCK_METHOD1(AppIncrementalDone, ErrCode(ErrCode errCode));
    MOCK_METHOD2(GetIncrementalFileHandle, ErrCode(const std::string &bundleName, const std::string &fileName));
    MOCK_METHOD2(GetBackupInfo, ErrCode(string &bundleName, string &result));
    MOCK_METHOD3(UpdateTimer, ErrCode(BundleName &bundleName, uint32_t timeout, bool &result));
    MOCK_METHOD1(StartExtTimer, ErrCode(bool &isExtStart));
    MOCK_METHOD1(StartFwkTimer, ErrCode(bool &isFwkStart));
    MOCK_METHOD3(UpdateSendRate, ErrCode(std::string &bundleName, int32_t sendRate, bool &result));
    MOCK_METHOD2(ReportAppProcessInfo, ErrCode(const std::string processInfo, BackupRestoreScenario sennario));
};

class ServiceStubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
public:
    static inline shared_ptr<MessageParcelMock> messageParcelMock = nullptr;
    static inline shared_ptr<ServiceMock> service = nullptr;
    static inline shared_ptr<IfaceCastMock> castMock = nullptr;
    static inline sptr<ServiceReverseMock> remote = nullptr;
};

void ServiceStubTest::SetUpTestCase(void)
{
    remote = sptr(new ServiceReverseMock());
    service = make_shared<ServiceMock>();
    messageParcelMock = make_shared<MessageParcelMock>();
    MessageParcelMock::messageParcel = messageParcelMock;
    castMock = std::make_shared<IfaceCastMock>();
    IfaceCastMock::cast = castMock;
}
void ServiceStubTest::TearDownTestCase()
{
    remote = nullptr;
    service = nullptr;
    MessageParcelMock::messageParcel = nullptr;
    messageParcelMock = nullptr;
    IfaceCastMock::cast = nullptr;
    castMock = nullptr;
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_OnRemoteRequest_0100
 * @tc.name: SUB_backup_sa_ServiceStub_OnRemoteRequest_0100
 * @tc.desc: Test function of OnRemoteRequest interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_OnRemoteRequest_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_OnRemoteRequest_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        EXPECT_CALL(*messageParcelMock, ReadInterfaceToken()).WillOnce(Return(u16string()));
        EXPECT_TRUE(service != nullptr);
        auto err = service->OnRemoteRequest(0, data, reply, option);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        const std::u16string descriptor = ServiceStub::GetDescriptor();
        EXPECT_CALL(*messageParcelMock, ReadInterfaceToken()).WillOnce(Return(descriptor));
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        err = service->OnRemoteRequest(static_cast<uint32_t>(IServiceInterfaceCode::SERVICE_CMD_GET_FILE_NAME),
            data, reply, option);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadInterfaceToken()).WillOnce(Return(descriptor));
        auto ret = service->OnRemoteRequest(-1, data, reply, option);
        EXPECT_EQ(ret, IPC_STUB_UNKNOW_TRANS_ERR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by InitRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_OnRemoteRequest_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_InitRestoreSession_0100
 * @tc.name: SUB_backup_sa_ServiceStub_InitRestoreSession_0100
 * @tc.desc: Test function of InitRestoreSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_InitRestoreSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_InitRestoreSession_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(nullptr));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdInitRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(remote));
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(nullptr));
        err = service->CmdInitRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(remote));
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(remote));
        EXPECT_CALL(*service, InitRestoreSession(_)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdInitRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(remote));
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(remote));
        EXPECT_CALL(*service, InitRestoreSession(_)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdInitRestoreSession(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
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
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_InitBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_InitBackupSession_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(nullptr));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdInitBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(remote));
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(nullptr));
        err = service->CmdInitBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(remote));
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(remote));
        EXPECT_CALL(*service, InitBackupSession(_)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdInitBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(remote));
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(remote));
        EXPECT_CALL(*service, InitBackupSession(_)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdInitBackupSession(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
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
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_Start_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_Start_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*service, Start()).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdStart(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*service, Start()).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdStart(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
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
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_GetLocalCapabilities_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_GetLocalCapabilities_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*service, GetLocalCapabilities()).WillOnce(Return(UniqueFd(0)));
        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdGetLocalCapabilities(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*service, GetLocalCapabilities()).WillOnce(Return(UniqueFd(0)));
        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(true));
        auto ret = service->CmdGetLocalCapabilities(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
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
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_PublishFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_PublishFile_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdPublishFile(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(1));
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadUint32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, PublishFile(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdPublishFile(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(1));
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadUint32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, PublishFile(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdPublishFile(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by PublishFile.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_PublishFile_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppFileReady_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppFileReady_0100
 * @tc.desc: Test function of AppFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppFileReady_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdAppFileReady(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(-1));
        err = service->CmdAppFileReady(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
        EXPECT_CALL(*service, AppFileReady(_, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdAppFileReady(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(false));
        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
        EXPECT_CALL(*service, AppFileReady(_, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdAppFileReady(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppFileReady_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppFileReady_0101
 * @tc.name: SUB_backup_sa_ServiceStub_AppFileReady_0101
 * @tc.desc: Test function of AppFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppFileReady_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppFileReady_0101";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(false));
        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
        EXPECT_CALL(*service, AppFileReady(_, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_TRUE(service != nullptr);
        auto ret = service->CmdAppFileReady(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppFileReady_0101";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppDone_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppDone_0100
 * @tc.desc: Test function of AppDone interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppDone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppDone_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdAppDone(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppDone(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdAppDone(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppDone(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdAppDone(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
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
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_GetFileHandle_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_GetFileHandle_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdGetFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(false));
        err = service->CmdGetFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*service, GetFileHandle(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        auto ret = service->CmdGetFileHandle(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_GetFileHandle_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0100
 * @tc.desc: Test function of AppendBundlesRestoreSession interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(-1));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdAppendBundlesRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(false));
        err = service->CmdAppendBundlesRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
        err = service->CmdAppendBundlesRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppendBundlesRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0101
 * @tc.name: SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0101
 * @tc.desc: Test function of AppendBundlesRestoreSession interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0101";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdAppendBundlesRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppendBundlesRestoreSession(_, _, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdAppendBundlesRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppendBundlesRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0101";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0102
 * @tc.name: SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0102
 * @tc.desc: Test function of AppendBundlesRestoreSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0102";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_TRUE(service != nullptr);
        EXPECT_CALL(*service, AppendBundlesRestoreSession(_, _, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdAppendBundlesRestoreSession(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppendBundlesRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppendBundlesRestoreSession_0102";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_AppendBundlesBackupSession_0100
 * @tc.name: SUB_backup_sa_ServiceStub_AppendBundlesBackupSession_0100
 * @tc.desc: Test function of AppendBundlesBackupSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppendBundlesBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppendBundlesBackupSession_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdAppendBundlesBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppendBundlesBackupSession(_)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdAppendBundlesBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppendBundlesBackupSession(_)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdAppendBundlesBackupSession(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppendBundlesBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_AppendBundlesBackupSession_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_Finish_0100
 * @tc.name: SUB_backup_sa_ServiceStub_Finish_0100
 * @tc.desc: Test function of Finish interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_Finish_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_Finish_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_TRUE(service != nullptr);
        EXPECT_CALL(*service, Finish()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        auto err = service->CmdFinish(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*service, Finish()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdFinish(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
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
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_Release_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_Release_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_TRUE(service != nullptr);
        EXPECT_CALL(*service, Release()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        auto err = service->CmdRelease(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*service, Release()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdRelease(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
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
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_GetBackupInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_GetBackupInfo_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdGetBackupInfo(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, GetBackupInfo(_, _)).WillOnce(Return(-1));
        err = service->CmdGetBackupInfo(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, GetBackupInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteString(_)).WillOnce(Return(false));
        err = service->CmdGetBackupInfo(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, GetBackupInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteString(_)).WillOnce(Return(true));
        auto ret = service->CmdGetBackupInfo(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
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
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_UpdateTimer_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_UpdateTimer_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdUpdateTimer(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadUint32(_)).WillOnce(Return(false));
        err = service->CmdUpdateTimer(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadUint32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, UpdateTimer(_, _, _)).WillOnce(Return(-1));
        err = service->CmdUpdateTimer(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));
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
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(-1));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdAppendBundlesDetailsRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(false));
        err = service->CmdAppendBundlesDetailsRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(false));
        err = service->CmdAppendBundlesDetailsRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
        err = service->CmdAppendBundlesDetailsRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));
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
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdAppendBundlesDetailsRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppendBundlesRestoreSession(_, _, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdAppendBundlesDetailsRestoreSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

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
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdAppendBundlesDetailsBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(false));
        err = service->CmdAppendBundlesDetailsBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppendBundlesDetailsBackupSession(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdAppendBundlesDetailsBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

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
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdGetLocalCapabilitiesIncremental(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(1), Return(true)));
        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
        err = service->CmdGetLocalCapabilitiesIncremental(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, GetLocalCapabilitiesIncremental(_)).WillOnce(Return(UniqueFd(0)));
        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(false));
        err = service->CmdGetLocalCapabilitiesIncremental(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

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
        EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(nullptr));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdInitIncrementalBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(remote));
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(nullptr));
        err = service->CmdInitIncrementalBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadRemoteObject()).WillOnce(Return(remote));
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(remote));
        EXPECT_CALL(*service, InitIncrementalBackupSession(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdInitIncrementalBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

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
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdAppendBundlesIncrementalBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(1), Return(true)));
        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
        err = service->CmdAppendBundlesIncrementalBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppendBundlesIncrementalBackupSession(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdAppendBundlesIncrementalBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

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
 * @tc.number: SUB_backup_sa_ServiceStub_AppendBundlesIncrementalBackupSession_0101
 * @tc.name: SUB_backup_sa_ServiceStub_AppendBundlesIncrementalBackupSession_0101
 * @tc.desc: Test function of AppendBundlesIncrementalBackupSession interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_AppendBundlesIncrementalBackupSession_0101,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_AppendBundlesIncrementalBackupSession_0101";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdAppendBundlesDetailsIncrementalBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(1), Return(true)));
        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
        err = service->CmdAppendBundlesDetailsIncrementalBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppendBundlesIncrementalBackupSession(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdAppendBundlesDetailsIncrementalBackupSession(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadStringVector(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppendBundlesIncrementalBackupSession(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdAppendBundlesDetailsIncrementalBackupSession(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by AppendBundlesIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_InitIncrementalBackupSession_0101";
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
        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdPublishIncrementalFile(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(1));
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadUint32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, PublishIncrementalFile(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdPublishIncrementalFile(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

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
        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdPublishSAIncrementalFile(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(1));
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadUint32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0));
        EXPECT_CALL(*service, PublishSAIncrementalFile(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdPublishSAIncrementalFile(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

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
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdAppIncrementalFileReady(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(-1));
        err = service->CmdAppIncrementalFileReady(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0)).WillOnce(Return(-1));
        err = service->CmdAppIncrementalFileReady(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0)).WillOnce(Return(1));
        EXPECT_CALL(*messageParcelMock, ReadInt32()).WillOnce(Return(0));
        EXPECT_CALL(*service, AppIncrementalFileReady(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdAppIncrementalFileReady(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));
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
        EXPECT_TRUE(service != nullptr);
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
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdAppIncrementalDone(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, AppIncrementalDone(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdAppIncrementalDone(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

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
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdGetIncrementalFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(false));
        err = service->CmdGetIncrementalFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

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

/**
 * @tc.number: SUB_backup_sa_ServiceStub_CmdResultReport_0100
 * @tc.name: SUB_backup_sa_ServiceStub_CmdResultReport_0100
 * @tc.desc: Test function of CmdResultReport interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_CmdResultReport_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_CmdResultReport_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdResultReport(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
        err = service->CmdResultReport(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true)).WillOnce(Return(false));
        err = service->CmdResultReport(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*service, ServiceResultReport(_, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = service->CmdResultReport(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by CmdResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_CmdResultReport_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_CmdResultReport_0200
 * @tc.name: SUB_backup_sa_ServiceStub_CmdResultReport_0200
 * @tc.desc: Test function of GetIncrementalFileHandle interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_CmdResultReport_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_CmdResultReport_0200";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_TRUE(service != nullptr);
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*service, ServiceResultReport(_, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        auto ret = service->CmdResultReport(data, reply);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by CmdResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_CmdResultReport_0200";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_CmdUpdateSendRate_0100
 * @tc.name: SUB_backup_sa_ServiceStub_CmdUpdateSendRate_0100
 * @tc.desc: Test function of CmdUpdateSendRate interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_CmdUpdateSendRate_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_CmdUpdateSendRate_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        EXPECT_TRUE(service != nullptr);
        auto err = service->CmdUpdateSendRate(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
        err = service->CmdUpdateSendRate(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, UpdateSendRate(_, _, _)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        err = service->CmdUpdateSendRate(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, UpdateSendRate(_, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteBool(_)).WillOnce(Return(false));
        err = service->CmdUpdateSendRate(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*service, UpdateSendRate(_, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteBool(_)).WillOnce(Return(true));
        err = service->CmdUpdateSendRate(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by CmdUpdateSendRate.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_CmdUpdateSendRate_0100";
}

/**
 * @tc.number: SUB_backup_sa_ServiceStub_CmdGetAppLocalListAndDoIncrementalBackup_0100
 * @tc.name: SUB_backup_sa_ServiceStub_CmdGetAppLocalListAndDoIncrementalBackup_0100
 * @tc.desc: Test function of CmdGetAppLocalListAndDoIncrementalBackup interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceStubTest, SUB_backup_sa_ServiceStub_CmdGetAppLocalListAndDoIncrementalBackup_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceStubTest-begin SUB_backup_sa_ServiceStub_CmdGetAppLocalListAndDoIncrementalBackup_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_TRUE(service != nullptr);
        EXPECT_CALL(*service, GetAppLocalListAndDoIncrementalBackup()).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        auto err = service->CmdGetAppLocalListAndDoIncrementalBackup(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::SA_BROKEN_IPC));

        EXPECT_CALL(*service, GetAppLocalListAndDoIncrementalBackup()).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        err = service->CmdGetAppLocalListAndDoIncrementalBackup(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceStubTest-an exception occurred by CmdGetAppLocalListAndDoIncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "ServiceStubTest-end SUB_backup_sa_ServiceStub_CmdGetAppLocalListAndDoIncrementalBackup_0100";
}
} // namespace OHOS::FileManagement::Backup