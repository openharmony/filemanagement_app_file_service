/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <message_parcel.h>
#include <string_ex.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "i_service_reverse.h"
#include "service_reverse_stub.h"
#include "test_manager.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string FILE_NAME = "1.tar";
} // namespace

class MockServiceReverse final : public ServiceReverseStub {
public:
    MOCK_METHOD3(BackupOnFileReady, void(string bundleName, string fileName, int fd));
    MOCK_METHOD2(BackupOnBundleStarted, void(int32_t errCode, string bundleName));
    MOCK_METHOD2(BackupOnBundleFinished, void(int32_t errCode, string bundleName));
    MOCK_METHOD1(BackupOnAllBundlesFinished, void(int32_t errCode));
    MOCK_METHOD2(RestoreOnBundleStarted, void(int32_t errCode, std::string bundleName));
    MOCK_METHOD2(RestoreOnBundleFinished, void(int32_t errCode, string bundleName));
    MOCK_METHOD1(RestoreOnAllBundlesFinished, void(int32_t errCode));
    MOCK_METHOD3(RestoreOnFileReady, void(string bundleName, string fileName, int fd));
};

class ServiceReverseStubTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.number: SUB_backup_ServiceReverseStub_BackupOnFileReady_0100
 * @tc.name: SUB_backup_ServiceReverseStub_BackupOnFileReady_0100
 * @tc.desc: Test function of BackupOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseStubTest, SUB_backup_ServiceReverseStub_BackupOnFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_BackupOnFileReady_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, BackupOnFileReady(_, _, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));
        EXPECT_TRUE(data.WriteString(FILE_NAME));
        TestManager tm("ServiceReverseStub_0100");
        string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        data.WriteFileDescriptor(fd);

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IServiceReverse::SERVICER_BACKUP_ON_FILE_READY, data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by BackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_BackupOnFileReady_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_BackupOnBundleStarted_0100
 * @tc.name: SUB_backup_ServiceReverseStub_BackupOnBundleStarted_0100
 * @tc.desc: Test function of BackupOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_BackupOnBundleStarted_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_BackupOnBundleStarted_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, BackupOnBundleStarted(_, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IServiceReverse::SERVICER_BACKUP_ON_SUB_TASK_STARTED, data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by BackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_BackupOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_BackupOnBundleFinished_0100
 * @tc.name: SUB_backup_ServiceReverseStub_BackupOnBundleFinished_0100
 * @tc.desc: Test function of BackupOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_BackupOnBundleFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_BackupOnBundleFinished_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, BackupOnBundleFinished(_, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IServiceReverse::SERVICER_BACKUP_ON_SUB_TASK_FINISHED, data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by BackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_BackupOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_BackupOnAllBundlesFinished_0100
 * @tc.name: SUB_backup_ServiceReverseStub_BackupOnAllBundlesFinished_0100
 * @tc.desc: Test function of BackupOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_BackupOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_BackupOnAllBundlesFinished_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, BackupOnAllBundlesFinished(_)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IServiceReverse::SERVICER_BACKUP_ON_TASK_FINISHED, data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by BackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_BackupOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_RestoreOnBundleStarted_0100
 * @tc.name: SUB_backup_ServiceReverseStub_RestoreOnBundleStarted_0100
 * @tc.desc: Test function of RestoreOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_RestoreOnBundleStarted_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_RestoreOnBundleStarted_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, RestoreOnBundleStarted(_, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IServiceReverse::SERVICER_RESTORE_ON_SUB_TASK_STARTED, data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by RestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_RestoreOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_RestoreOnBundleFinished_0100
 * @tc.name: SUB_backup_ServiceReverseStub_RestoreOnBundleFinished_0100
 * @tc.desc: Test function of RestoreOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_RestoreOnBundleFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_RestoreOnBundleFinished_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, RestoreOnBundleFinished(_, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IServiceReverse::SERVICER_RESTORE_ON_SUB_TASK_FINISHED, data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by RestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_RestoreOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_RestoreOnAllBundlesFinished_0100
 * @tc.name: SUB_backup_ServiceReverseStub_RestoreOnAllBundlesFinished_0100
 * @tc.desc: Test function of RestoreOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_RestoreOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_RestoreOnAllBundlesFinished_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, RestoreOnAllBundlesFinished(_)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IServiceReverse::SERVICER_RESTORE_ON_TASK_FINISHED, data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by RestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_RestoreOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_RestoreOnFileReady_0100
 * @tc.name: SUB_backup_ServiceReverseStub_RestoreOnFileReady_0100
 * @tc.desc: Test function of RestoreOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseStubTest, SUB_backup_ServiceReverseStub_RestoreOnFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_RestoreOnFileReady_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, RestoreOnFileReady(_, _, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));
        EXPECT_TRUE(data.WriteString(FILE_NAME));

        TestManager tm("ServiceReverseStub_0200");
        string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        data.WriteFileDescriptor(fd);

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IServiceReverse::SERVICER_RESTORE_ON_FILE_READY, data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by RestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_RestoreOnFileReady_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_error_0100
 * @tc.name: SUB_backup_ServiceReverseStub_error_0100
 * @tc.desc: Test function of RestoreOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseStubTest, SUB_backup_ServiceReverseStub_error_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_error_0100";
    try {
        MockServiceReverse service;
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        EXPECT_TRUE(data.WriteInterfaceToken(Str8ToStr16("test")));

        EXPECT_NE(BError(BError::Codes::OK), service.OnRemoteRequest(3333, data, reply, option));
        EXPECT_NE(BError(BError::Codes::OK),
                  service.OnRemoteRequest(IServiceReverse::SERVICER_RESTORE_ON_FILE_READY, data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_error_0100";
}
} // namespace OHOS::FileManagement::Backup