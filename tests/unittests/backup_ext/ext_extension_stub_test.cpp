/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "ext_extension_stub.h"
#include "message_parcel_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class ExtExtensionStubMock : public ExtExtensionStub {
public:
    MOCK_METHOD(UniqueFd, GetFileHandle, (const std::string &fileName, int32_t &errCode));
    MOCK_METHOD(ErrCode, HandleClear, ());
    MOCK_METHOD(ErrCode, HandleBackup, (bool isClearData));
    MOCK_METHOD(ErrCode, PublishFile, (const std::string &fileName));
    MOCK_METHOD(ErrCode, HandleRestore, (bool isClearData));
    MOCK_METHOD((std::tuple<ErrCode, UniqueFd, UniqueFd>), GetIncrementalFileHandle, (const std::string &fileName));
    MOCK_METHOD(ErrCode, PublishIncrementalFile, (const std::string &fileName));
    MOCK_METHOD(ErrCode, HandleIncrementalBackup, (UniqueFd incrementalFd, UniqueFd manifestFd));
    MOCK_METHOD(ErrCode, IncrementalOnBackup, (bool isClearData));
    MOCK_METHOD((std::tuple<UniqueFd, UniqueFd>), GetIncrementalBackupFileHandle, ());
    MOCK_METHOD(ErrCode, GetBackupInfo, (std::string &result));
    MOCK_METHOD(ErrCode, UpdateFdSendRate, (std::string &bundleName, int32_t sendRate));
    MOCK_METHOD(ErrCode, User0OnBackup, ());
};

class ExtExtensionStubTest : public testing::Test {
public:
    //所有测试用例执行之前执行
    static void SetUpTestCase(void);
    //所有测试用例执行之后执行
    static void TearDownTestCase(void);
    //每次测试用例执行之前执行
    void SetUp() {};
    //每次测试用例执行之后执行
    void TearDown() {};
public:
    static inline shared_ptr<ExtExtensionStubMock> stub = nullptr;
    static inline shared_ptr<MessageParcelMock> messageParcelMock = nullptr;
};

void ExtExtensionStubTest::SetUpTestCase()
{
    stub = make_shared<ExtExtensionStubMock>();
    messageParcelMock = make_shared<MessageParcelMock>();
    MessageParcelMock::messageParcel = messageParcelMock;
}

void ExtExtensionStubTest::TearDownTestCase()
{
    stub = nullptr;
    MessageParcelMock::messageParcel = nullptr;
    messageParcelMock = nullptr;
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_OnRemoteRequest_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_OnRemoteRequest_0100
 * @tc.desc: 测试 OnRemoteRequest 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_OnRemoteRequest_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_OnRemoteRequest_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        uint32_t code = 0;
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        EXPECT_CALL(*messageParcelMock, ReadInterfaceToken()).WillOnce(Return(u16string()));
        auto err = stub->OnRemoteRequest(code, data, reply, option);
        EXPECT_EQ(err, BError(BError::Codes::EXT_INVAL_ARG));

        const std::u16string descriptor = ExtExtensionStub::GetDescriptor();
        EXPECT_CALL(*messageParcelMock, ReadInterfaceToken()).WillOnce(Return(descriptor));
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        err = stub->OnRemoteRequest(static_cast<uint32_t>(IExtensionInterfaceCode::CMD_GET_FILE_HANDLE),
            data, reply, option);
        EXPECT_EQ(err, BError(BError::Codes::EXT_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadInterfaceToken()).WillOnce(Return(descriptor));
        auto ret = stub->OnRemoteRequest(-1, data, reply, option);
        EXPECT_EQ(ret, IPC_STUB_UNKNOW_TRANS_ERR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by OnRemoteRequest.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_OnRemoteRequest_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdGetFileHandle_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdGetFileHandle_0100
 * @tc.desc: 测试 CmdGetFileHandle 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdGetFileHandle_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdGetFileHandle_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        auto err = stub->CmdGetFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*stub, GetFileHandle(_, _)).WillOnce(Return(UniqueFd(-1)));
        EXPECT_CALL(*messageParcelMock, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        err = stub->CmdGetFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*stub, GetFileHandle(_, _)).WillOnce(Return(UniqueFd(0)));
        EXPECT_CALL(*messageParcelMock, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(false));
        err = stub->CmdGetFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*stub, GetFileHandle(_, _)).WillOnce(Return(UniqueFd(0)));
        EXPECT_CALL(*messageParcelMock, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(true));
        err = stub->CmdGetFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdGetFileHandle.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdGetFileHandle_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdHandleClear_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdHandleClear_0100
 * @tc.desc: 测试 CmdHandleClear 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdHandleClear_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdHandleClear_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*stub, HandleClear()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        auto err = stub->CmdHandleClear(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*stub, HandleClear()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        err = stub->CmdHandleClear(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdHandleClear.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdHandleClear_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdHandleBackup_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdHandleBackup_0100
 * @tc.desc: 测试 CmdHandleBackup 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdHandleBackup_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdHandleBackup_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
        EXPECT_CALL(*stub, HandleBackup(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        auto err = stub->CmdHandleBackup(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
        EXPECT_CALL(*stub, HandleBackup(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        err = stub->CmdHandleBackup(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdHandleBackup.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdHandleBackup_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdPublishFile_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdPublishFile_0100
 * @tc.desc: 测试 CmdPublishFile 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdPublishFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdPublishFile_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        auto err = stub->CmdPublishFile(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*stub, PublishFile(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = stub->CmdPublishFile(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*stub, PublishFile(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        err = stub->CmdPublishFile(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdPublishFile.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdPublishFile_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdHandleRestore_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdHandleRestore_0100
 * @tc.desc: 测试 CmdHandleRestore 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdHandleRestore_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdHandleRestore_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
        EXPECT_CALL(*stub, HandleRestore(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        auto err = stub->CmdHandleRestore(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
        EXPECT_CALL(*stub, HandleRestore(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        err = stub->CmdHandleRestore(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdHandleRestore.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdHandleRestore_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdGetIncrementalFileHandle_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdGetIncrementalFileHandle_0100
 * @tc.desc: 测试 CmdGetIncrementalFileHandle 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdGetIncrementalFileHandle_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdGetIncrementalFileHandle_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        auto err = stub->CmdGetIncrementalFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*stub, GetIncrementalFileHandle(_)).WillOnce(Return(make_tuple(0, UniqueFd(-1), UniqueFd(-1))));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = stub->CmdGetIncrementalFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*stub, GetIncrementalFileHandle(_)).WillOnce(Return(make_tuple(0, UniqueFd(-1), UniqueFd(-1))));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(true));
        err = stub->CmdGetIncrementalFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdGetIncrementalFileHandle.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdGetIncrementalFileHandle_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdPublishIncrementalFile_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdPublishIncrementalFile_0100
 * @tc.desc: 测试 CmdPublishIncrementalFile 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdPublishIncrementalFile_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdPublishIncrementalFile_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        auto err = stub->CmdPublishIncrementalFile(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*stub, PublishIncrementalFile(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = stub->CmdPublishIncrementalFile(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*stub, PublishIncrementalFile(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        err = stub->CmdPublishIncrementalFile(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdPublishIncrementalFile.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdPublishIncrementalFile_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdHandleIncrementalBackup_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdHandleIncrementalBackup_0100
 * @tc.desc: 测试 CmdHandleIncrementalBackup 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdHandleIncrementalBackup_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdHandleIncrementalBackup_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*stub, HandleIncrementalBackup(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        auto err = stub->CmdHandleIncrementalBackup(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadFileDescriptor()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*stub, HandleIncrementalBackup(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        err = stub->CmdHandleIncrementalBackup(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdHandleIncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdHandleIncrementalBackup_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdIncrementalOnBackup_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdIncrementalOnBackup_0100
 * @tc.desc: 测试 CmdIncrementalOnBackup 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdIncrementalOnBackup_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdIncrementalOnBackup_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
        EXPECT_CALL(*stub, IncrementalOnBackup(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        auto err = stub->CmdIncrementalOnBackup(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadBool()).WillOnce(Return(true));
        EXPECT_CALL(*stub, IncrementalOnBackup(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        err = stub->CmdIncrementalOnBackup(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdIncrementalOnBackup.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdIncrementalOnBackup_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdGetIncrementalBackupFileHandle_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdGetIncrementalBackupFileHandle_0100
 * @tc.desc: 测试 CmdGetIncrementalBackupFileHandle 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdGetIncrementalBackupFileHandle_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) <<
        "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdGetIncrementalBackupFileHandle_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*stub, GetIncrementalBackupFileHandle())
            .WillOnce(Return(std::tuple<UniqueFd, UniqueFd>(UniqueFd(-1), UniqueFd(-1))));
        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(false));
        auto err = stub->CmdGetIncrementalBackupFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*stub, GetIncrementalBackupFileHandle())
            .WillOnce(Return(std::tuple<UniqueFd, UniqueFd>(UniqueFd(-1), UniqueFd(-1))));
        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(false));
        err = stub->CmdGetIncrementalBackupFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*stub, GetIncrementalBackupFileHandle())
            .WillOnce(Return(std::tuple<UniqueFd, UniqueFd>(UniqueFd(-1), UniqueFd(-1))));
        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(true));
        err = stub->CmdGetIncrementalBackupFileHandle(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdGetIncrementalBackupFileHandle.";
    }
    GTEST_LOG_(INFO) <<
        "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdGetIncrementalBackupFileHandle_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdGetBackupInfo_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdGetBackupInfo_0100
 * @tc.desc: 测试 CmdGetBackupInfo 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdGetBackupInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdGetBackupInfo_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*stub, GetBackupInfo(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        auto err = stub->CmdGetBackupInfo(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*stub, GetBackupInfo(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, WriteString(_)).WillOnce(Return(false));
        err = stub->CmdGetBackupInfo(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*stub, GetBackupInfo(_)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, WriteString(_)).WillOnce(Return(true));
        err = stub->CmdGetBackupInfo(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdGetBackupInfo.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdGetBackupInfo_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdUpdateSendRate_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdUpdateSendRate_0100
 * @tc.desc: 测试 CmdUpdateSendRate 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9QWK5
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdUpdateSendRate_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdUpdateSendRate_0100";
    try {
        ASSERT_TRUE(stub != nullptr);
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(false));
        auto err = stub->CmdUpdateFdSendRate(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(false));
        err = stub->CmdUpdateFdSendRate(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_INVAL_ARG));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*stub, UpdateFdSendRate(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        err = stub->CmdUpdateFdSendRate(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*messageParcelMock, ReadString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*stub, UpdateFdSendRate(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        err = stub->CmdUpdateFdSendRate(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdUpdateSendRate.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdUpdateSendRate_0100";
}

/**
 * @tc.number: SUB_backup_ext_ExtExtensionStub_CmdUser0_0100
 * @tc.name: SUB_backup_ext_ExtExtensionStub_CmdUser0_0100
 * @tc.desc: 测试 CmdHandleUser0Backup 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issues
 */
HWTEST_F(ExtExtensionStubTest, SUB_backup_ext_ExtExtensionStub_CmdUser0_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-begin SUB_backup_ext_ExtExtensionStub_CmdUser0_0100";
    try {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_CALL(*stub, User0OnBackup()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(false));
        EXPECT_TRUE(stub != nullptr);
        auto err = stub->CmdHandleUser0Backup(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::EXT_BROKEN_IPC));

        EXPECT_CALL(*stub, User0OnBackup()).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock, WriteInt32(_)).WillOnce(Return(true));
        err = stub->CmdHandleUser0Backup(data, reply);
        EXPECT_EQ(err, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionStubTest-an exception occurred by CmdHandleClear.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionStubTest-end SUB_backup_ext_ExtExtensionStub_CmdUser0_0100";
}
} // namespace OHOS::FileManagement::Backup