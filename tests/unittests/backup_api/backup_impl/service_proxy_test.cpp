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
#include <cstdio>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "b_file_info.h"
#include "i_service_mock.h"
#include "iremote_object_mock.h"
#include "iservice_registry.h"
#include "service_proxy.h"
#include "service_reverse_mock.h"
#include "unique_fd.h"
#include "utils_mock_global_variable.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string FILE_NAME = "1.tar";
constexpr int32_t SERVICE_ID = 5203;
} // namespace

class ServiceProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override;

    shared_ptr<ServiceProxy> proxy_ = nullptr;
    sptr<IServiceMock> mock_ = nullptr;
    sptr<ServiceReverseMock> remote_ = nullptr;
};

void ServiceProxyTest::SetUp()
{
    mock_ = sptr(new IServiceMock());
    proxy_ = make_shared<ServiceProxy>(mock_);
    remote_ = sptr(new ServiceReverseMock());
}

void ServiceProxyTest::TearDown()
{
    proxy_ = nullptr;
    mock_ = nullptr;
    remote_ = nullptr;
}

/**
 * @tc.number: SUB_Service_proxy_InitRestoreSession_0100
 * @tc.name: SUB_Service_proxy_InitRestoreSession_0100
 * @tc.desc: 测试 InitRestoreSession 注册restore Session接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_InitRestoreSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_InitRestoreSession_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest));
    int32_t result = proxy_->InitRestoreSession(remote_);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    result = proxy_->InitRestoreSession(nullptr);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_InitRestoreSession_0100";
}

/**
 * @tc.number: SUB_Service_proxy_InitBackupSession_0100
 * @tc.name: SUB_Service_proxy_InitBackupSession_0100
 * @tc.desc: 测试 InitBackupSession 注册backup Session接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_InitBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_InitBackupSession_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest));

    int32_t result = proxy_->InitBackupSession(remote_);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    result = proxy_->InitBackupSession(nullptr);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_InitBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_proxy_Start_0100
 * @tc.name: SUB_Service_proxy_Start_0100
 * @tc.desc: 测试 Start 启动备份恢复流程接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_Start_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_Start_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));
    int32_t result = proxy_->Start();
    EXPECT_EQ(result, BError(BError::Codes::OK));

    result = proxy_->Start();
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_Start_0100";
}

/**
 * @tc.number: SUB_Service_proxy_GetLocalCapabilities_0100
 * @tc.name: SUB_Service_proxy_GetLocalCapabilities_0100
 * @tc.desc: 测试 GetLocalCapabilities 获取能力文件接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_GetLocalCapabilities_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_GetLocalCapabilities_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeGetLocalSendRequest))
        .WillOnce(Return(EPERM));
    UniqueFd fd = proxy_->GetLocalCapabilities();
    EXPECT_GT(fd, BError(BError::Codes::OK));

    UniqueFd fdErr = proxy_->GetLocalCapabilities();
    EXPECT_LT(fdErr, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_GetLocalCapabilities_0100";
}

/**
 * @tc.number: SUB_Service_proxy_PublishFile_0100
 * @tc.name: SUB_Service_proxy_PublishFile_0100
 * @tc.desc: 测试 PublishFile 推送接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_PublishFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_PublishFile_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));
    string bundleName = "com.example.app2backup";
    string fileName = "1.tar";
    BFileInfo fileInfo(bundleName, fileName, -1);
    int32_t result = proxy_->PublishFile(fileInfo);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    result = proxy_->PublishFile(fileInfo);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_PublishFile_0100";
}

/**
 * @tc.number: SUB_Service_proxy_AppFileReady_0100
 * @tc.name: SUB_Service_proxy_AppFileReady_0100
 * @tc.desc: 测试 AppFileReady 文件就绪调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_AppFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_AppFileReady_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));

    string bundleName = "com.example.app2backup";
    TestManager tm("AppFileReady_GetFd_0100");
    std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
    UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));

    int32_t result = proxy_->AppFileReady(bundleName, move(fd));
    EXPECT_EQ(result, BError(BError::Codes::OK));

    TestManager tmErr("AppFileReady_GetFd_0200");
    UniqueFd fdErr(open(tmErr.GetRootDirCurTest().append(FILE_NAME).data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
    result = proxy_->AppFileReady(bundleName, move(fdErr));
    EXPECT_NE(result, BError(BError::Codes::OK));
    result = proxy_->AppFileReady(bundleName, UniqueFd(-1));
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_AppFileReady_0100";
}

/**
 * @tc.number: SUB_Service_proxy_AppDone_0100
 * @tc.name: SUB_Service_proxy_AppDone_0100
 * @tc.desc: 测试 AppDone ext备份恢复流程结束调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_AppDone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_AppDone_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));
    int32_t result = proxy_->AppDone(BError(BError::Codes::OK));
    EXPECT_EQ(result, BError(BError::Codes::OK));

    result = proxy_->AppDone(BError(BError::Codes::OK));
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_AppDone_0100";
}

/**
 * @tc.number: SUB_Service_proxy_GetFileHandle_0100
 * @tc.name: SUB_Service_proxy_GetFileHandle_0100
 * @tc.desc: 测试 GetFileHandle 获取真实文件调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_GetFileHandle_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_GetFileHandle_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));
    string bundleName = "com.example.app2backup";
    string fileName = "1.tar";
    int32_t result = proxy_->GetFileHandle(bundleName, fileName);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    result = proxy_->GetFileHandle(bundleName, fileName);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_GetFileHandle_0100";
}

/**
 * @tc.number: SUB_Service_proxy_AppendBundlesRestoreSession_0100
 * @tc.name: SUB_Service_proxy_AppendBundlesRestoreSession_0100
 * @tc.desc: 测试 AppendBundlesRestoreSession 获取真实文件调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_AppendBundlesRestoreSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_AppendBundlesRestoreSession_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest));

    std::vector<string> bundleNames;
    TestManager tm("BackupSession_GetFd_0100");
    std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
    UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
    int32_t result = proxy_->AppendBundlesRestoreSession(move(fd), bundleNames);
    EXPECT_EQ(result, BError(BError::Codes::OK));
    result = proxy_->AppendBundlesRestoreSession(UniqueFd(-1), bundleNames);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_AppendBundlesRestoreSession_0100";
}

/**
 * @tc.number: SUB_Service_proxy_AppendBundlesRestoreSession_0101
 * @tc.name: SUB_Service_proxy_AppendBundlesRestoreSession_0101
 * @tc.desc: 测试 AppendBundlesRestoreSession 获取真实文件调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_AppendBundlesRestoreSession_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_AppendBundlesRestoreSession_0101";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest));

    std::vector<string> bundleNames;
    TestManager tm("BackupSession_GetFd_0100");
    std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
    UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
    std::vector<std::string> detailInfos;
    int32_t result = proxy_->AppendBundlesRestoreSession(move(fd), bundleNames, detailInfos);
    EXPECT_EQ(result, BError(BError::Codes::OK));
    result = proxy_->AppendBundlesRestoreSession(UniqueFd(-1), bundleNames, detailInfos);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_AppendBundlesRestoreSession_0101";
}

/**
 * @tc.number: SUB_Service_proxy_AppendBundlesBackupSession_0100
 * @tc.name: SUB_Service_proxy_AppendBundlesBackupSession_0100
 * @tc.desc: 测试 AppendBundlesBackupSession 获取真实文件调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_AppendBundlesBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_AppendBundlesBackupSession_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));

    std::vector<string> bundleNames;

    int32_t result = proxy_->AppendBundlesBackupSession(bundleNames);
    EXPECT_EQ(result, BError(BError::Codes::OK));
    result = proxy_->AppendBundlesBackupSession(bundleNames);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_AppendBundlesBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_proxy_Finish_0100
 * @tc.name: SUB_Service_proxy_Finish_0100
 * @tc.desc: 测试 Finish 获取真实文件调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6URNZ
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_Finish_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_Finish_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));

    int32_t result = proxy_->Finish();
    EXPECT_EQ(result, BError(BError::Codes::OK));
    result = proxy_->Finish();
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_Finish_0100";
}

/**
 * @tc.number: SUB_Service_proxy_Release_0100
 * @tc.name: SUB_Service_proxy_Release_0100
 * @tc.desc: 测试 Release 调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZV5
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_Release_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_Release_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));

    int32_t result = proxy_->Release();
    EXPECT_EQ(result, BError(BError::Codes::OK));
    result = proxy_->Release();
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_Release_0100";
}

/**
 * @tc.number: SUB_Service_proxy_GetLocalCapabilitiesIncremental_0100
 * @tc.name: SUB_Service_proxy_GetLocalCapabilitiesIncremental_0100
 * @tc.desc: 测试 GetLocalCapabilitiesIncremental 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZV5
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_GetLocalCapabilitiesIncremental_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_GetLocalCapabilitiesIncremental_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeGetLocalSendRequest))
        .WillOnce(Return(EPERM));

    std::vector<BIncrementalData> bundleNames;
    BIncrementalData data("com.example.app2backup", 0);
    bundleNames.push_back(data);
    UniqueFd fd = proxy_->GetLocalCapabilitiesIncremental(bundleNames);
    EXPECT_GT(fd, BError(BError::Codes::OK));

    UniqueFd fdErr = proxy_->GetLocalCapabilitiesIncremental(bundleNames);
    EXPECT_LT(fdErr, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_GetLocalCapabilitiesIncremental_0100";
}

/**
 * @tc.number: SUB_Service_proxy_InitIncrementalBackupSession_0100
 * @tc.name: SUB_Service_proxy_InitIncrementalBackupSession_0100
 * @tc.desc: 测试 InitIncrementalBackupSession 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZV5
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_InitIncrementalBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_InitIncrementalBackupSession_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest));

    int32_t result = proxy_->InitIncrementalBackupSession(remote_);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    result = proxy_->InitIncrementalBackupSession(nullptr);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_InitIncrementalBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_proxy_AppendBundlesIncrementalBackupSession_0100
 * @tc.name: SUB_Service_proxy_AppendBundlesIncrementalBackupSession_0100
 * @tc.desc: 测试 AppendBundlesIncrementalBackupSession 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZV5
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_AppendBundlesIncrementalBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_AppendBundlesIncrementalBackupSession_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));

    std::vector<BIncrementalData> bundleNames;
    int32_t result = proxy_->AppendBundlesIncrementalBackupSession(bundleNames);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    result = proxy_->AppendBundlesIncrementalBackupSession(bundleNames);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_AppendBundlesIncrementalBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_proxy_PublishIncrementalFile_0100
 * @tc.name: SUB_Service_proxy_PublishIncrementalFile_0100
 * @tc.desc: 测试 PublishIncrementalFile 接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZV5
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_PublishIncrementalFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_PublishIncrementalFile_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));

    string bundleName = "com.example.app2backup";
    string fileName = "";
    BFileInfo fileInfo(bundleName, fileName, -1);
    int32_t result = proxy_->PublishIncrementalFile(fileInfo);
    EXPECT_EQ(result, BError(BError::Codes::OK));
    fileName = "test";
    result = proxy_->PublishIncrementalFile(fileInfo);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_PublishIncrementalFile_0100";
}

/**
 * @tc.number: SUB_Service_proxy_AppIncrementalFileReady_0100
 * @tc.name: SUB_Service_proxy_AppIncrementalFileReady_0100
 * @tc.desc: 测试 AppIncrementalFileReady 接口成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZV5
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_AppIncrementalFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_AppIncrementalFileReady_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));

    string bundleName = "com.example.app2backup";
    TestManager tm("AppIncrementalFileReady_GetFd_0100");
    std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
    UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
    UniqueFd manifestFd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));

    int32_t result = proxy_->AppIncrementalFileReady(bundleName, move(fd), move(manifestFd));
    EXPECT_EQ(result, BError(BError::Codes::OK));

    TestManager tmErr("AppIncrementalFileReady_GetFd_0200");
    UniqueFd fdErr(open(tmErr.GetRootDirCurTest().append(FILE_NAME).data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
    UniqueFd manifestFdErr(
        open(tmErr.GetRootDirCurTest().append(FILE_NAME).data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
    result = proxy_->AppIncrementalFileReady(bundleName, move(fdErr), move(manifestFdErr));
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_AppIncrementalFileReady_0100";
}

/**
 * @tc.number: SUB_Service_proxy_AppIncrementalDone_0100
 * @tc.name: SUB_Service_proxy_AppIncrementalDone_0100
 * @tc.desc: 测试 AppIncrementalDone 接口成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZV5
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_AppIncrementalDone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_AppIncrementalDone_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));
    int32_t result = proxy_->AppIncrementalDone(BError(BError::Codes::OK));
    EXPECT_EQ(result, BError(BError::Codes::OK));

    result = proxy_->AppIncrementalDone(BError(BError::Codes::OK));
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_AppIncrementalDone_0100";
}

/**
 * @tc.number: SUB_Service_proxy_GetIncrementalFileHandle_0100
 * @tc.name: SUB_Service_proxy_GetIncrementalFileHandle_0100
 * @tc.desc: 测试 GetIncrementalFileHandle 接口成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZV5
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_GetIncrementalFileHandle_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_GetIncrementalFileHandle_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest))
        .WillOnce(Return(EPERM));
    string bundleName = "com.example.app2backup";
    string fileName = "1.tar";
    int32_t result = proxy_->GetIncrementalFileHandle(bundleName, fileName);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    result = proxy_->GetIncrementalFileHandle(bundleName, fileName);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_GetIncrementalFileHandle_0100";
}

/**
 * @tc.number: SUB_Service_proxy_OnLoadSystemAbilitySuccess_0100
 * @tc.name: SUB_Service_proxy_OnLoadSystemAbilitySuccess_0100
 * @tc.desc: 测试 OnLoadSystemAbilitySuccess 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_OnLoadSystemAbilitySuccess_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_OnLoadSystemAbilitySuccess_0100";
    sptr<ServiceProxy::ServiceProxyLoadCallback> loadCallback = new ServiceProxy::ServiceProxyLoadCallback();
    EXPECT_NE(loadCallback, nullptr);
    loadCallback->OnLoadSystemAbilitySuccess(SERVICE_ID, nullptr);
    loadCallback = nullptr;
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_OnLoadSystemAbilitySuccess_0100";
}

/**
 * @tc.number: SUB_Service_proxy_OnLoadSystemAbilitySuccess_0101
 * @tc.name: SUB_Service_proxy_OnLoadSystemAbilitySuccess_0101
 * @tc.desc: 测试 OnLoadSystemAbilitySuccess 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_OnLoadSystemAbilitySuccess_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_OnLoadSystemAbilitySuccess_0101";
    sptr<ServiceProxy::ServiceProxyLoadCallback> loadCallback = new ServiceProxy::ServiceProxyLoadCallback();
    EXPECT_NE(loadCallback, nullptr);
    int32_t systemAbilityId = 0;
    // const OHOS::sptr<IRemoteObject> &remoteObject = make_shared<IRemoteObject>();
    // shared_ptr<MockIRemoteObject> remoteObject = make_shared<MockIRemoteObject>();
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    loadCallback->OnLoadSystemAbilitySuccess(systemAbilityId, remoteObject);
    loadCallback = nullptr;
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_OnLoadSystemAbilitySuccess_0101";
}

/**
 * @tc.number: SUB_Service_proxy_OnLoadSystemAbilitySuccess_0102
 * @tc.name: SUB_Service_proxy_OnLoadSystemAbilitySuccess_0102
 * @tc.desc: 测试 OnLoadSystemAbilitySuccess 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_OnLoadSystemAbilitySuccess_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_OnLoadSystemAbilitySuccess_0102";
    sptr<ServiceProxy::ServiceProxyLoadCallback> loadCallback = new ServiceProxy::ServiceProxyLoadCallback();
    EXPECT_NE(loadCallback, nullptr);
    int32_t systemAbilityId = 0;
    loadCallback->OnLoadSystemAbilitySuccess(systemAbilityId, nullptr);
    loadCallback = nullptr;
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_OnLoadSystemAbilitySuccess_0102";
}

/**
 * @tc.number: SUB_Service_proxy_OnLoadSystemAbilityFail_0100
 * @tc.name: SUB_Service_proxy_OnLoadSystemAbilityFail_0100
 * @tc.desc: 测试 OnLoadSystemAbilityFail 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_OnLoadSystemAbilityFail_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_OnLoadSystemAbilityFail_0100";
    sptr<ServiceProxy::ServiceProxyLoadCallback> loadCallback = new ServiceProxy::ServiceProxyLoadCallback();
    EXPECT_NE(loadCallback, nullptr);
    loadCallback->OnLoadSystemAbilityFail(SERVICE_ID);
    loadCallback = nullptr;
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_OnLoadSystemAbilityFail_0100";
}

/**
 * @tc.number: SUB_Service_proxy_GetInstance_0100
 * @tc.name: SUB_Service_proxy_GetInstance_0100
 * @tc.desc: 测试 GetInstance 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_GetInstance_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_GetInstance_0100";
    GTEST_LOG_(INFO) << "MockLoadSystemAbility is false";
    SetMockLoadSystemAbility(false);
    auto proxy = ServiceProxy::GetInstance();
    EXPECT_EQ(proxy, nullptr);
    GTEST_LOG_(INFO) << "MockLoadSystemAbility is true";
    SetMockLoadSystemAbility(true);
    proxy = ServiceProxy::GetInstance();
    EXPECT_EQ(proxy, nullptr);
    sptr<ServiceProxy::ServiceProxyLoadCallback> loadCallback = new ServiceProxy::ServiceProxyLoadCallback();
    sptr<IRemoteObject> object = new MockIRemoteObject();
    loadCallback->OnLoadSystemAbilitySuccess(SERVICE_ID, object);
    GTEST_LOG_(INFO) << "GetInstance is ok";
    proxy = ServiceProxy::GetInstance();
    EXPECT_NE(proxy, nullptr);
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_GetInstance_0100";
}

/**
 * @tc.number: SUB_Service_proxy_InvaildInstance_0100
 * @tc.name: SUB_Service_proxy_InvaildInstance_0100
 * @tc.desc: 测试 InvaildInstance 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_InvaildInstance_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_InvaildInstance_0100";
    ServiceProxy::InvaildInstance();
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_InvaildInstance_0100";
}

/**
 * @tc.number: SUB_Service_proxy_GetBackupInfo_0100
 * @tc.name: SUB_Service_proxy_GetBackupInfo_0100
 * @tc.desc: 测试 GetBackupInfo 获取应用信息接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_GetBackupInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_GetBackupInfo_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &IServiceMock::InvokeSendRequest));
    std::string result;
    std::string bundleName = "com.example.app2backup";
    int32_t ret = proxy_->GetBackupInfo(bundleName, result);
    EXPECT_EQ(ret, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_GetBackupInfo_0100";
}
} // namespace OHOS::FileManagement::Backup