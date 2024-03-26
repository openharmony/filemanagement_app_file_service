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

#include <cstdio>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "ext_extension_mock.h"
#include "module_ipc/svc_extension_proxy.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class SvcExtensionProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override;
    sptr<SvcExtensionProxy> proxy_ = nullptr;
    sptr<BackupExtExtensionMock> mock_ = nullptr;
};

void SvcExtensionProxyTest::SetUp()
{
    mock_ = sptr(new BackupExtExtensionMock());
    proxy_ = sptr(new SvcExtensionProxy(mock_));
}
void SvcExtensionProxyTest::TearDown()
{
    mock_ = nullptr;
    proxy_ = nullptr;
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
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &BackupExtExtensionMock::InvokeGetFileHandleRequest))
        .WillOnce(Return(EPERM));
    string fileName = "1.tar";
    UniqueFd fd = proxy_->GetFileHandle(fileName);
    EXPECT_GT(fd, BError(BError::Codes::OK));

    UniqueFd fdErr = proxy_->GetFileHandle(fileName);
    EXPECT_LT(fdErr, BError(BError::Codes::OK));
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
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &BackupExtExtensionMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));
    ErrCode ret = proxy_->HandleClear();
    EXPECT_EQ(BError(BError::Codes::OK), ret);

    ret = proxy_->HandleClear();
    EXPECT_NE(BError(BError::Codes::OK), ret);
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
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &BackupExtExtensionMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));
    ErrCode ret = proxy_->HandleBackup();
    EXPECT_EQ(BError(BError::Codes::OK), ret);

    ret = proxy_->HandleBackup();
    EXPECT_NE(BError(BError::Codes::OK), ret);
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
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &BackupExtExtensionMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));
    string fileName = "1.tar";
    ErrCode ret = proxy_->PublishFile(fileName);
    EXPECT_EQ(BError(BError::Codes::OK), ret);

    ret = proxy_->PublishFile(fileName);
    EXPECT_NE(BError(BError::Codes::OK), ret);
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_PublishFile_0100";
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
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &BackupExtExtensionMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));
    string result = "result report";
    ErrCode ret = proxy_->GetBackupInfo(result);
    EXPECT_EQ(BError(BError::Codes::OK), ret);

    ret = proxy_->GetBackupInfo(result);
    EXPECT_NE(BError(BError::Codes::OK), ret);
    GTEST_LOG_(INFO) << "SvcExtensionProxyTest-end SUB_Ext_Extension_proxy_GetBackupInfo_0100";
}
} // namespace OHOS::FileManagement::Backup