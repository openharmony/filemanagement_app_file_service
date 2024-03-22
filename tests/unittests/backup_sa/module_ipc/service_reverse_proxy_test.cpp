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
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "iservice_registry.h"
#include "module_ipc/service_reverse_proxy.h"
#include "service_reverse_mock.h"
#include "test_manager.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string FILE_NAME = "1.tar";
const string BUNDLE_NAME = "com.example.app2backup";
const string FILE_NAME_MANIFEST = "manifest.rp";
} // namespace

class ServiceReverseProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override;
    shared_ptr<ServiceReverseProxy> proxy_ = nullptr;
    sptr<ServiceReverseMock> mock_ = nullptr;
};

void ServiceReverseProxyTest::SetUp()
{
    mock_ = sptr(new ServiceReverseMock());
    proxy_ = make_shared<ServiceReverseProxy>(mock_);
}
void ServiceReverseProxyTest::TearDown()
{
    mock_ = nullptr;
    proxy_ = nullptr;
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnFileReady_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnFileReady_0100
 * @tc.desc: Test function of BackupOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnFileReady_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));

        TestManager tm("ServiceReverseProxyTest_GetFd_0100");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        proxy_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, fd);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnFileReady_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100
 * @tc.desc: Test function of BackupOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100
 * @tc.desc: Test function of BackupOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100
 * @tc.desc: Test function of BackupOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100
 * @tc.desc: Test function of RestoreOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100
 * @tc.desc: Test function of RestoreOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100
 * @tc.desc: Test function of RestoreOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnFileReady_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnFileReady_0100
 * @tc.desc: Test function of RestoreOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnFileReady_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));

        TestManager tm("ServiceReverseProxyTest_GetFd_0200");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        proxy_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, fd);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnFileReady_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0100
 * @tc.desc: Test function of IncrementalBackupOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));

        TestManager tm("ServiceReverseProxyTest_GetFd_0300");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        TestManager tm1("ServiceReverseProxyTest_GetFd_0301");
        std::string manifestFilePath = tm1.GetRootDirCurTest().append(FILE_NAME_MANIFEST);
        UniqueFd manifestFd(open(manifestFilePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        proxy_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, fd, manifestFd);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0100
 * @tc.desc: Test function of IncrementalBackupOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->IncrementalBackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0100
 * @tc.desc: Test function of IncrementalBackupOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0100
 * @tc.desc: Test function of IncrementalBackupOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->IncrementalBackupOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0100
 * @tc.desc: Test function of IncrementalRestoreOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0100
 * @tc.desc: Test function of IncrementalRestoreOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->IncrementalRestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0100
 * @tc.desc: Test function of IncrementalRestoreOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->IncrementalRestoreOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0100
 * @tc.desc: Test function of IncrementalRestoreOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));

        TestManager tm("ServiceReverseProxyTest_GetFd_0400");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        TestManager tm1("ServiceReverseProxyTest_GetFd_0401");
        std::string manifestFilePath = tm1.GetRootDirCurTest().append(FILE_NAME_MANIFEST);
        UniqueFd manifestFd(open(manifestFilePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        proxy_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, fd, manifestFd);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0100";
}
} // namespace OHOS::FileManagement::Backup