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

#include <cstdio>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "b_error/b_error.h"
#include "ext_extension_mock.h"
#include "message_parcel_mock.h"
#include "module_ipc/svc_extension_proxy.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class SvcExtensionProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
public:
    static inline sptr<SvcExtensionProxy> proxy_ = nullptr;
    static inline sptr<BackupExtExtensionMock> mock_ = nullptr;
    static inline shared_ptr<MessageParcelMock> messageParcelMock_ = nullptr;
};

void SvcExtensionProxyTest::SetUpTestCase()
{
    mock_ = sptr(new BackupExtExtensionMock());
    proxy_ = sptr(new SvcExtensionProxy(mock_));
    messageParcelMock_ = make_shared<MessageParcelMock>();
    MessageParcelMock::messageParcel = messageParcelMock_;
}
void SvcExtensionProxyTest::TearDownTestCase()
{
    mock_ = nullptr;
    proxy_ = nullptr;
    MessageParcelMock::messageParcel = nullptr;
    messageParcelMock_ = nullptr;
}

/**
 * @tc.number: SUB_Ext_Extension_proxy_GetFileHandle_0100
 * @tc.name: SUB_Ext_Extension_proxy_GetFileHandle_0100
 * @tc.desc: 测试 GetFileHandle 获取真实文件接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcExtensionProxyTest, SUB_Ext_Extension_proxy_GetFileHandle_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-begin SUB_Ext_Extension_proxy_GetFileHandle_0100";
    try {
        string fileName = "1.tar";
        int32_t errCode = 0;
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
        EXPECT_TRUE(proxy_ != nullptr);
        UniqueFd fd = proxy_->GetFileHandle(fileName, errCode);
        EXPECT_LT(fd, BError(BError::Codes::OK));

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(EPERM));
        fd = proxy_->GetFileHandle(fileName, errCode);
        EXPECT_LT(fd, BError(BError::Codes::OK));

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, ReadFileDescriptor()).WillOnce(Return(-1));
        fd = proxy_->GetFileHandle(fileName, errCode);
        EXPECT_LT(fd, BError(BError::Codes::OK));

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(false));
        fd = proxy_->GetFileHandle(fileName, errCode);
        EXPECT_LT(fd, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcExtensionProxyTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_GetFileHandle_0100";
}

/**
 * @tc.number: SUB_Ext_Extension_proxy_HandleClear_0100
 * @tc.name: SUB_Ext_Extension_proxy_HandleClear_0100
 * @tc.desc: 测试 HandleClear 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcExtensionProxyTest, SUB_Ext_Extension_proxy_HandleClear_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-begin SUB_Ext_Extension_proxy_HandleClear_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(EPERM));
        EXPECT_TRUE(proxy_ != nullptr);
        ErrCode ret = proxy_->HandleClear();
        EXPECT_EQ(EPERM, ret);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(0));
        ret = proxy_->HandleClear();
        EXPECT_EQ(BError(BError::Codes::OK), ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcExtensionProxyTest-an exception occurred by HandleClear.";
    }
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_HandleClear_0100";
}

/**
 * @tc.number: SUB_Ext_Extension_proxy_HandleBackup_0100
 * @tc.name: SUB_Ext_Extension_proxy_HandleBackup_0100
 * @tc.desc: 测试 HandleBackup 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcExtensionProxyTest, SUB_Ext_Extension_proxy_HandleBackup_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-begin SUB_Ext_Extension_proxy_HandleBackup_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(EPERM));
        EXPECT_TRUE(proxy_ != nullptr);
        ErrCode ret = proxy_->HandleBackup(true);
        EXPECT_EQ(EPERM, ret);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(0));
        ret = proxy_->HandleBackup(true);
        EXPECT_EQ(BError(BError::Codes::OK), ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcExtensionProxyTest-an exception occurred by HandleBackup.";
    }
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_HandleBackup_0100";
}

/**
 * @tc.number: SUB_Ext_Extension_proxy_PublishFile_0100
 * @tc.name: SUB_Ext_Extension_proxy_PublishFile_0100
 * @tc.desc: 测试 PublishFile 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcExtensionProxyTest, SUB_Ext_Extension_proxy_PublishFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-begin SUB_Ext_Extension_proxy_PublishFile_0100";
    try {
        string fileName = "1.tar";
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
        EXPECT_TRUE(proxy_ != nullptr);
        ErrCode ret = proxy_->PublishFile(fileName);
        EXPECT_EQ(EPERM, ret);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(EPERM));
        ret = proxy_->PublishFile(fileName);
        EXPECT_EQ(EPERM, ret);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(0));
        ret = proxy_->PublishFile(fileName);
        EXPECT_EQ(BError(BError::Codes::OK), ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcExtensionProxyTest-an exception occurred by PublishFile.";
    }
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_PublishFile_0100";
}

/**
 * @tc.number: SUB_Ext_Extension_proxy_HandleRestore_0100
 * @tc.name: SUB_Ext_Extension_proxy_HandleRestore_0100
 * @tc.desc: 测试 HandleRestore 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcExtensionProxyTest, SUB_Ext_Extension_proxy_HandleRestore_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-begin SUB_Ext_Extension_proxy_HandleRestore_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(EPERM));
        EXPECT_TRUE(proxy_ != nullptr);
        ErrCode ret = proxy_->HandleRestore(true);
        EXPECT_EQ(EPERM, ret);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(0));
        ret = proxy_->HandleRestore(true);
        EXPECT_EQ(BError(BError::Codes::OK), ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcExtensionProxyTest-an exception occurred by HandleRestore.";
    }
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_HandleRestore_0100";
}

/**
 * @tc.number: SUB_Ext_Extension_proxy_GetBackupInfo_0100
 * @tc.name: SUB_Ext_Extension_proxy_GetBackupInfo_0100
 * @tc.desc: 测试 GetBackupInfo 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcExtensionProxyTest, SUB_Ext_Extension_proxy_GetBackupInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-begin SUB_Ext_Extension_proxy_GetBackupInfo_0100";
    try {
        string result = "result_report";
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(EPERM));
        EXPECT_TRUE(proxy_ != nullptr);
        ErrCode ret = proxy_->GetBackupInfo(result);
        EXPECT_EQ(EPERM, ret);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(Return(false));
        ret = proxy_->GetBackupInfo(result);
        EXPECT_EQ(BError(BError::Codes::OK), ret);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(EPERM), Return(true)));
        ret = proxy_->GetBackupInfo(result);
        EXPECT_EQ(EPERM, ret);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(NO_ERROR), Return(true)));
        EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(false));
        ret = proxy_->GetBackupInfo(result);
        EXPECT_EQ(BError(BError::Codes::OK), ret);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(NO_ERROR), Return(true)));
        EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(true));
        ret = proxy_->GetBackupInfo(result);
        EXPECT_EQ(BError(BError::Codes::OK), ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcExtensionProxyTest-an exception occurred by GetBackupInfo.";
    }
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_GetBackupInfo_0100";
}

/**
 * @tc.number: SUB_Ext_Extension_proxy_UpdateFdSendRate_0100
 * @tc.name: SUB_Ext_Extension_proxy_UpdateFdSendRate_0100
 * @tc.desc: 测试 HandleRestore 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcExtensionProxyTest, SUB_Service_GetBackupInfoCmdHandle_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-begin SUB_Ext_Extension_proxy_UpdateFdSendRate_0100";
    try {
        std::string bundleName = "bundleName";
        int32_t sendRate = 0;
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
        EXPECT_TRUE(proxy_ != nullptr);
        ErrCode ret = proxy_->UpdateFdSendRate(bundleName, sendRate);
        EXPECT_EQ(EPERM, ret);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
        EXPECT_TRUE(proxy_ != nullptr);
        ret = proxy_->UpdateFdSendRate(bundleName, sendRate);
        EXPECT_EQ(EPERM, ret);
        
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(EPERM));
        EXPECT_TRUE(proxy_ != nullptr);
        ret = proxy_->UpdateFdSendRate(bundleName, sendRate);
        EXPECT_EQ(EPERM, ret);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(Return(false));
        EXPECT_TRUE(proxy_ != nullptr);
        ret = proxy_->UpdateFdSendRate(bundleName, sendRate);
        EXPECT_EQ(BError(BError::Codes::OK), ret);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(Return(true));
        EXPECT_TRUE(proxy_ != nullptr);
        ret = proxy_->UpdateFdSendRate(bundleName, sendRate);
        EXPECT_EQ(BError(BError::Codes::OK), ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcExtensionProxyTest-an exception occurred by UpdateFdSendRate.";
    }
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_UpdateFdSendRate_0100";
}

/**
 * @tc.number: SUB_Ext_Extension_proxy_GetIncrementalFileHandle_0100
 * @tc.name: SUB_Ext_Extension_proxy_GetIncrementalFileHandle_0100
 * @tc.desc: 测试 GetIncrementalFileHandle 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcExtensionProxyTest, SUB_Ext_Extension_proxy_GetIncrementalFileHandle_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-begin SUB_Ext_Extension_proxy_GetIncrementalFileHandle_0100";
    try {
        string fileName = "1.tar";
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
        EXPECT_TRUE(proxy_ != nullptr);
        ErrCode ret = proxy_->GetIncrementalFileHandle(fileName);
        EXPECT_EQ(ret, ErrCode(EPERM));

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(EPERM));
        ret = proxy_->GetIncrementalFileHandle(fileName);
        EXPECT_EQ(ret, ErrCode(EPERM));

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(NO_ERROR));
        ret = proxy_->GetIncrementalFileHandle(fileName);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcExtensionProxyTest-an exception occurred by GetIncrementalFileHandle.";
    }
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_GetIncrementalFileHandle_0100";
}

/**
 * @tc.number: SUB_Ext_Extension_proxy_PublishIncrementalFile_0100
 * @tc.name: SUB_Ext_Extension_proxy_PublishIncrementalFile_0100
 * @tc.desc: 测试 PublishIncrementalFile 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcExtensionProxyTest, SUB_Ext_Extension_proxy_PublishIncrementalFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-begin SUB_Ext_Extension_proxy_PublishIncrementalFile_0100";
    try {
        string fileName = "1.tar";
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
        EXPECT_TRUE(proxy_ != nullptr);
        ErrCode ret = proxy_->PublishIncrementalFile(fileName);
        EXPECT_EQ(ret, ErrCode(EPERM));

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(EPERM));
        ret = proxy_->PublishIncrementalFile(fileName);
        EXPECT_EQ(ret, ErrCode(EPERM));

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(NO_ERROR));
        ret = proxy_->PublishIncrementalFile(fileName);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcExtensionProxyTest-an exception occurred by PublishIncrementalFile.";
    }
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_PublishIncrementalFile_0100";
}

/**
 * @tc.number: SUB_Ext_Extension_proxy_HandleIncrementalBackup_0100
 * @tc.name: SUB_Ext_Extension_proxy_HandleIncrementalBackup_0100
 * @tc.desc: 测试 HandleIncrementalBackup 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcExtensionProxyTest, SUB_Ext_Extension_proxy_HandleIncrementalBackup_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-begin SUB_Ext_Extension_proxy_HandleIncrementalBackup_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(EPERM));
        EXPECT_TRUE(proxy_ != nullptr);
        ErrCode ret = proxy_->HandleIncrementalBackup(UniqueFd(-1), UniqueFd(-1));
        EXPECT_EQ(ret, ErrCode(EPERM));

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(NO_ERROR));
        ret = proxy_->HandleIncrementalBackup(UniqueFd(-1), UniqueFd(-1));
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcExtensionProxyTest-an exception occurred by HandleIncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_HandleIncrementalBackup_0100";
}

/**
 * @tc.number: SUB_Ext_Extension_proxy_IncrementalOnBackup_0100
 * @tc.name: SUB_Ext_Extension_proxy_IncrementalOnBackup_0100
 * @tc.desc: 测试 IncrementalOnBackup 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcExtensionProxyTest, SUB_Ext_Extension_proxy_IncrementalOnBackup_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-begin SUB_Ext_Extension_proxy_IncrementalOnBackup_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
        EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
        EXPECT_TRUE(proxy_ != nullptr);
        ErrCode ret = proxy_->IncrementalOnBackup(true);
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(false));
        EXPECT_TRUE(proxy_ != nullptr);
        ret = proxy_->IncrementalOnBackup(true);
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(EPERM));
        EXPECT_TRUE(proxy_ != nullptr);
        ret = proxy_->IncrementalOnBackup(true);
        EXPECT_EQ(ret, ErrCode(EPERM));
        
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_TRUE(proxy_ != nullptr);
        ret = proxy_->IncrementalOnBackup(true);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcExtensionProxyTest-an exception occurred by IncrementalOnBackup.";
    }
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_IncrementalOnBackup_0100";
}

/**
 * @tc.number: SUB_Ext_Extension_proxy_GetIncrementalBackupFileHandle_0100
 * @tc.name: SUB_Ext_Extension_proxy_GetIncrementalBackupFileHandle_0100
 * @tc.desc: 测试 GetIncrementalBackupFileHandle 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcExtensionProxyTest, SUB_Ext_Extension_proxy_GetIncrementalBackupFileHandle_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-begin SUB_Ext_Extension_proxy_GetIncrementalBackupFileHandle_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(EPERM));
        EXPECT_TRUE(proxy_ != nullptr);
        auto [incrementalFd, manifestFd] = proxy_->GetIncrementalBackupFileHandle();
        EXPECT_EQ(incrementalFd, -1);
        EXPECT_EQ(manifestFd, -1);

        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
        EXPECT_CALL(*messageParcelMock_, ReadFileDescriptor()).WillOnce(Return(-1)).WillOnce(Return(-1));
        tie(incrementalFd, manifestFd) = proxy_->GetIncrementalBackupFileHandle();
        EXPECT_EQ(incrementalFd, -1);
        EXPECT_EQ(manifestFd, -1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcExtensionProxyTest-an exception occurred by GetIncrementalBackupFileHandle.";
    }
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_GetIncrementalBackupFileHandle_0100";
}
} // namespace OHOS::FileManagement::Backup