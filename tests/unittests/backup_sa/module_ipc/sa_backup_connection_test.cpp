/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <cstdio>
#include <future>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "element_name.h"
#include "ext_extension_mock.h"
#include "message_parcel_mock.h"
#include "module_ipc/sa_backup_connection.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class SABackupConnectionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
public:
    static inline std::shared_ptr<SABackupConnection> backupCon_ = nullptr;
    static inline shared_ptr<MessageParcelMock> messageParcelMock = nullptr;
};

static void CallDied(const std::string &&name)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-CallDied SUCCESS";
}

static void CallConnect(const std::string &&name)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-CallConnect SUCCESS";
}

static void CallBackup(const std::string &&name, const int &&fd, const std::string &&result, const ErrCode &&err)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-CallBackup SUCCESS";
}

static void CallRestore(const std::string &&name, const std::string &&result, const ErrCode &&err)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-CallRestore SUCCESS";
}

void SABackupConnectionTest::SetUpTestCase()
{
    backupCon_ = std::make_shared<SABackupConnection>(CallDied, CallConnect, CallBackup, CallRestore);
    messageParcelMock = make_shared<MessageParcelMock>();
    MessageParcelMock::messageParcel = messageParcelMock;
}

void SABackupConnectionTest::TearDownTestCase()
{
    backupCon_ = nullptr;
    MessageParcelMock::messageParcel = nullptr;
    messageParcelMock = nullptr;
}

/**
 * @tc.number: SUB_BackupConnection_LoadBackupSAExt_0100
 * @tc.name: SUB_BackupConnection_LoadBackupSAExt_0100
 * @tc.desc: 测试 LoadBackupSAExt 链接回调接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SABackupConnectionTest, SUB_BackupConnection_LoadBackupSAExt_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SABackupConnectionTest-begin SUB_BackupConnection_LoadBackupSAExt_0100";
    try {
        EXPECT_TRUE(backupCon_->GetBackupSAExtProxy() == nullptr);
        auto res = backupCon_->LoadBackupSAExt();
        EXPECT_NE(res, BError(BError::Codes::OK).GetCode());

        backupCon_->reloadNum_.store(BConstants::BACKUP_SA_RELOAD_MAX);
        res = backupCon_->LoadBackupSAExt();
        EXPECT_EQ(res, BError(BError::Codes::SA_EXT_RELOAD_FAIL).GetCode());

        backupCon_->reloadNum_.store(0);
        res = backupCon_->LoadBackupSAExtInner();
        EXPECT_NE(res, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SABackupConnectionTest-an exception occurred by LoadBackupSAExt.";
    }
    GTEST_LOG_(INFO) << "SABackupConnectionTest-end SUB_BackupConnection_LoadBackupSAExt_0100";
}

/**
 * @tc.number: SUB_BackupConnection_Call_0100
 * @tc.name: SUB_BackupConnection_Call_0100
 * @tc.desc: 测试 Call 链接回调接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SABackupConnectionTest, SUB_BackupConnection_Call_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SABackupConnectionTest-begin SUB_BackupConnection_Call_0100";
    try {
        auto res = backupCon_->CallBackupSA();
        EXPECT_EQ(res, BError(BError::Codes::OK).GetCode());

        res = backupCon_->CallRestoreSA(UniqueFd(-1));
        EXPECT_EQ(res, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SABackupConnectionTest-an exception occurred by Call.";
    }
    GTEST_LOG_(INFO) << "SABackupConnectionTest-end SUB_BackupConnection_Call_0100";
}

/**
 * @tc.number: SUB_BackupConnection_InputParaSet_0100
 * @tc.name: SUB_BackupConnection_InputParaSet_0100
 * @tc.desc: 测试 InputParaSet 链接回调接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SABackupConnectionTest, SUB_BackupConnection_InputParaSet_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SABackupConnectionTest-begin SUB_BackupConnection_InputParaSet_0100";
    try {
        MessageParcel data;
        backupCon_->extension_ = BConstants::EXTENSION_BACKUP;
        EXPECT_CALL(*messageParcelMock, WriteString(_)).WillOnce(Return(false));
        auto res = backupCon_->InputParaSet(data);
        EXPECT_FALSE(res);

        EXPECT_CALL(*messageParcelMock, WriteString(_)).WillOnce(Return(true));
        res = backupCon_->InputParaSet(data);
        EXPECT_TRUE(res);

        backupCon_->extension_ = BConstants::EXTENSION_RESTORE;
        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(false));
        res = backupCon_->InputParaSet(data);
        EXPECT_FALSE(res);

        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, WriteString(_)).WillOnce(Return(false));
        res = backupCon_->InputParaSet(data);
        EXPECT_FALSE(res);

        EXPECT_CALL(*messageParcelMock, WriteFileDescriptor(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock, WriteString(_)).WillOnce(Return(true));
        res = backupCon_->InputParaSet(data);
        EXPECT_TRUE(res);

        backupCon_->extension_ = "";
        res = backupCon_->InputParaSet(data);
        EXPECT_FALSE(res);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SABackupConnectionTest-an exception occurred by InputParaSet.";
    }
    GTEST_LOG_(INFO) << "SABackupConnectionTest-end SUB_BackupConnection_InputParaSet_0100";
}

/**
 * @tc.number: SUB_BackupConnection_OutputParaGet_0100
 * @tc.name: SUB_BackupConnection_OutputParaGet_0100
 * @tc.desc: 测试 OutputParaGet 链接回调接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SABackupConnectionTest, SUB_BackupConnection_OutputParaGet_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SABackupConnectionTest-begin SUB_BackupConnection_OutputParaGet_0100";
    try {
        MessageParcel data;
        auto res = backupCon_->OutputParaGet(data);
        EXPECT_TRUE(res);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SABackupConnectionTest-an exception occurred by OutputParaGet.";
    }
    GTEST_LOG_(INFO) << "SABackupConnectionTest-end SUB_BackupConnection_OutputParaGet_0100";
}

/**
 * @tc.number: SUB_BackupConnection_DisconnectBackupSAExt_0100
 * @tc.name: SUB_BackupConnection_DisconnectBackupSAExt_0100
 * @tc.desc: 测试 DisconnectBackupSAExt 链接回调接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SABackupConnectionTest, SUB_BackupConnection_DisconnectBackupSAExt_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SABackupConnectionTest-begin SUB_BackupConnection_DisconnectBackupSAExt_0100";
    try {
        backupCon_->isLoaded_.store(false);
        auto res = backupCon_->DisconnectBackupSAExt();
        EXPECT_EQ(res, BError(BError::Codes::OK).GetCode());

        backupCon_->isLoaded_.store(true);
        backupCon_->isConnected_.store(false);
        res = backupCon_->DisconnectBackupSAExt();
        EXPECT_EQ(res, BError(BError::Codes::OK).GetCode());

        backupCon_->isConnected_.store(true);
        backupCon_->proxy_ = nullptr;
        res = backupCon_->DisconnectBackupSAExt();
        EXPECT_EQ(res, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SABackupConnectionTest-an exception occurred by DisconnectBackupSAExt.";
    }
    GTEST_LOG_(INFO) << "SABackupConnectionTest-end SUB_BackupConnection_DisconnectBackupSAExt_0100";
}
}