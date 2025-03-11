/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include "iservice_reverse.h"
#include "service_reverse_stub.h"
#include "test_manager.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string FILE_NAME = "1.tar";
const string FILE_NAME_MANIFEST = "1.fr";
} // namespace

class MockServiceReverse final : public ServiceReverseStub {
public:
    MOCK_METHOD4(BackupOnFileReady, void(string bundleName, string fileName, int fd, int32_t errCode));
    MOCK_METHOD2(BackupOnBundleStarted, void(int32_t errCode, string bundleName));
    MOCK_METHOD2(BackupOnResultReport, void(string result, std::string bundleName));
    MOCK_METHOD2(BackupOnBundleFinished, void(int32_t errCode, string bundleName));
    MOCK_METHOD1(BackupOnAllBundlesFinished, void(int32_t errCode));
    MOCK_METHOD2(BackupOnProcessInfo, void(std::string bundleName, std::string processInfo));
    MOCK_METHOD1(BackupOnScanningInfo, void(std::string scannedInfo));

    MOCK_METHOD2(RestoreOnBundleStarted, void(int32_t errCode, std::string bundleName));
    MOCK_METHOD2(RestoreOnBundleFinished, void(int32_t errCode, string bundleName));
    MOCK_METHOD1(RestoreOnAllBundlesFinished, void(int32_t errCode));
    MOCK_METHOD4(RestoreOnFileReady, void(string bundleName, string fileName, int fd, int32_t errCode));
    MOCK_METHOD3(RestoreOnResultReport, void(string result, string bundleName, ErrCode errCode));
    MOCK_METHOD2(RestoreOnProcessInfo, void(std::string bundleName, std::string processInfo));

    MOCK_METHOD5(IncrementalBackupOnFileReady,
        void(string bundleName, string fileName, int fd, int manifestFd, int32_t errCode));
    MOCK_METHOD2(IncrementalBackupOnBundleStarted, void(int32_t errCode, string bundleName));
    MOCK_METHOD2(IncrementalBackupOnResultReport, void(string result, std::string bundleName));
    MOCK_METHOD2(IncrementalBackupOnBundleFinished, void(int32_t errCode, string bundleName));
    MOCK_METHOD1(IncrementalBackupOnAllBundlesFinished, void(int32_t errCode));
    MOCK_METHOD2(IncrementalBackupOnProcessInfo, void(std::string bundleName, std::string processInfo));
    MOCK_METHOD1(IncrementalBackupOnScanningInfo, void(std::string scannedInfo));

    MOCK_METHOD2(IncrementalRestoreOnBundleStarted, void(int32_t errCode, std::string bundleName));
    MOCK_METHOD2(IncrementalRestoreOnBundleFinished, void(int32_t errCode, string bundleName));
    MOCK_METHOD1(IncrementalRestoreOnAllBundlesFinished, void(int32_t errCode));
    MOCK_METHOD5(IncrementalRestoreOnFileReady,
        void(string bundleName, string fileName, int fd, int manifestFd, int32_t errCode));
    MOCK_METHOD3(IncrementalRestoreOnResultReport, void(string result, string bundleName, ErrCode errCode));
    MOCK_METHOD2(IncrementalRestoreOnProcessInfo, void(std::string bundleName, std::string processInfo));
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
        EXPECT_CALL(service, BackupOnFileReady(_, _, _, _)).WillOnce(Return());
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
        data.WriteInt32(0);

        EXPECT_EQ(
            BError(BError::Codes::OK),
            service.OnRemoteRequest(static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_BACKUP_ON_FILE_READY),
                                    data, reply, option));
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
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_BACKUP_ON_SUB_TASK_STARTED), data,
                      reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by BackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_BackupOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_BackupOnResultReport_0100
 * @tc.name: SUB_backup_ServiceReverseStub_BackupOnResultReport_0100
 * @tc.desc: Test function of BackupOnResultReport interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_BackupOnResultReport_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_BackupOnResultReport_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, BackupOnResultReport(_, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_BACKUP_ON_RESULT_REPORT), data,
                      reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by BackupOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_BackupOnResultReport_0100";
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
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_BACKUP_ON_SUB_TASK_FINISHED), data,
                      reply, option));
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
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_BACKUP_ON_TASK_FINISHED), data,
                      reply, option));
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
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_SUB_TASK_STARTED), data,
                      reply, option));
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
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_SUB_TASK_FINISHED), data,
                      reply, option));
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
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_TASK_FINISHED), data,
                      reply, option));
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
        EXPECT_CALL(service, RestoreOnFileReady(_, _, _, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        int32_t errCode = 0;
        bool fdFlag = true;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));
        EXPECT_TRUE(data.WriteString(FILE_NAME));

        TestManager tm("ServiceReverseStub_0200");
        string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        if (fd < 0) {
            errCode = BError::GetCodeByErrno(errno);
            fdFlag = false;
        }
        data.WriteBool(fdFlag);
        if (fdFlag == true) {
            data.WriteFileDescriptor(fd);
        }
        data.WriteInt32(errCode);

        EXPECT_EQ(
            BError(BError::Codes::OK),
            service.OnRemoteRequest(static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_FILE_READY),
                                    data, reply, option));
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
        EXPECT_NE(
            BError(BError::Codes::OK),
            service.OnRemoteRequest(static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_FILE_READY),
                                    data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_error_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_IncrementalBackupOnFileReady_0100
 * @tc.name: SUB_backup_ServiceReverseStub_IncrementalBackupOnFileReady_0100
 * @tc.desc: Test function of IncrementalBackupOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZZX
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_IncrementalBackupOnFileReady_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_IncrementalBackupOnFileReady_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));
        EXPECT_TRUE(data.WriteString(FILE_NAME));
        TestManager tm("ServiceReverseStub_0300");
        string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        data.WriteFileDescriptor(fd);
        TestManager tm2("ServiceReverseStub_0301");
        string filePathManifest = tm2.GetRootDirCurTest().append(FILE_NAME_MANIFEST);
        UniqueFd fdManifest(open(filePathManifest.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        data.WriteFileDescriptor(fdManifest);
        data.WriteInt32(0);

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_BACKUP_ON_FILE_READY),
                      data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by IncrementalBackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_IncrementalBackupOnFileReady_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_IncrementalBackupOnBundleStarted_0100
 * @tc.name: SUB_backup_ServiceReverseStub_IncrementalBackupOnBundleStarted_0100
 * @tc.desc: Test function of IncrementalBackupOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZZX
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_IncrementalBackupOnBundleStarted_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_IncrementalBackupOnBundleStarted_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, IncrementalBackupOnBundleStarted(_, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));

        EXPECT_EQ(
            BError(BError::Codes::OK),
            service.OnRemoteRequest(
                static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_BACKUP_ON_SUB_TASK_STARTED),
                data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by IncrementalBackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_IncrementalBackupOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_IncrementalBackupOnResultReport_0100
 * @tc.name: SUB_backup_ServiceReverseStub_IncrementalBackupOnResultReport_0100
 * @tc.desc: Test function of IncrementalBackupOnResultReport interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZZX
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_IncrementalBackupOnResultReport_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_IncrementalBackupOnResultReport_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, IncrementalBackupOnResultReport(_, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));

        EXPECT_EQ(
            BError(BError::Codes::OK),
            service.OnRemoteRequest(
                static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_BACKUP_ON_RESULT_REPORT),
                data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by IncrementalBackupOnResultReport.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_IncrementalBackupOnResultReport_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_IncrementalBackupOnBundleFinished_0100
 * @tc.name: SUB_backup_ServiceReverseStub_IncrementalBackupOnBundleFinished_0100
 * @tc.desc: Test function of IncrementalBackupOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZZX
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_IncrementalBackupOnBundleFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_IncrementalBackupOnBundleFinished_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, IncrementalBackupOnBundleFinished(_, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));

        EXPECT_EQ(
            BError(BError::Codes::OK),
            service.OnRemoteRequest(
                static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_BACKUP_ON_SUB_TASK_FINISHED),
                data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by IncrementalBackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_IncrementalBackupOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_IncrementalBackupOnAllBundlesFinished_0100
 * @tc.name: SUB_backup_ServiceReverseStub_IncrementalBackupOnAllBundlesFinished_0100
 * @tc.desc: Test function of IncrementalBackupOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZZX
 */

HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_IncrementalBackupOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_IncrementalBackupOnAllBundlesFinished_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, IncrementalBackupOnAllBundlesFinished(_)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_BACKUP_ON_TASK_FINISHED),
                      data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by IncrementalBackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_IncrementalBackupOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_IncrementalRestoreOnBundleStarted_0100
 * @tc.name: SUB_backup_ServiceReverseStub_IncrementalRestoreOnBundleStarted_0100
 * @tc.desc: Test function of IncrementalRestoreOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZZX
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_IncrementalRestoreOnBundleStarted_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_IncrementalRestoreOnBundleStarted_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, IncrementalRestoreOnBundleStarted(_, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));

        EXPECT_EQ(
            BError(BError::Codes::OK),
            service.OnRemoteRequest(
                static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_RESTORE_ON_SUB_TASK_STARTED),
                data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by IncrementalRestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_IncrementalRestoreOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_IncrementalRestoreOnBundleFinished_0100
 * @tc.name: SUB_backup_ServiceReverseStub_IncrementalRestoreOnBundleFinished_0100
 * @tc.desc: Test function of IncrementalRestoreOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZZX
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_IncrementalRestoreOnBundleFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_IncrementalRestoreOnBundleFinished_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, IncrementalRestoreOnBundleFinished(_, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));

        EXPECT_EQ(
            BError(BError::Codes::OK),
            service.OnRemoteRequest(
                static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_RESTORE_ON_SUB_TASK_FINISHED),
                data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by IncrementalRestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_IncrementalRestoreOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_IncrementalRestoreOnAllBundlesFinished_0100
 * @tc.name: SUB_backup_ServiceReverseStub_IncrementalRestoreOnAllBundlesFinished_0100
 * @tc.desc: Test function of IncrementalRestoreOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZZX
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_IncrementalRestoreOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_IncrementalRestoreOnAllBundlesFinished_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, IncrementalRestoreOnAllBundlesFinished(_)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteInt32(BError(BError::Codes::OK)));

        EXPECT_EQ(
            BError(BError::Codes::OK),
            service.OnRemoteRequest(
                static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_RESTORE_ON_TASK_FINISHED),
                data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by IncrementalRestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_IncrementalRestoreOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_IncrementalRestoreOnFileReady_0100
 * @tc.name: SUB_backup_ServiceReverseStub_IncrementalRestoreOnFileReady_0100
 * @tc.desc: Test function of IncrementalRestoreOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZZX
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_IncrementalRestoreOnFileReady_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_IncrementalRestoreOnFileReady_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, IncrementalRestoreOnFileReady(_, _, _, _, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));
        EXPECT_TRUE(data.WriteString(FILE_NAME));
        TestManager tm("ServiceReverseStub_0400");
        string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        data.WriteFileDescriptor(fd);
        TestManager tm2("ServiceReverseStub_0401");
        string filePathManifest = tm2.GetRootDirCurTest().append(FILE_NAME_MANIFEST);
        UniqueFd fdManifest(open(filePathManifest.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        data.WriteFileDescriptor(fdManifest);

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_RESTORE_ON_FILE_READY),
                      data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by IncrementalRestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_IncrementalRestoreOnFileReady_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_RestoreOnResultReport_0100
 * @tc.name: SUB_backup_ServiceReverseStub_RestoreOnResultReport_0100
 * @tc.desc: Test function of RestoreOnResultReport interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZZX
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_RestoreOnResultReport_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_RestoreOnResultReport_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, RestoreOnResultReport(_, _, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        std::string resultReport = "result_report";
        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(resultReport));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));
        EXPECT_TRUE(data.WriteInt32(0));

        EXPECT_EQ(
            BError(BError::Codes::OK),
            service.OnRemoteRequest(
                static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_RESTORE_ON_RESULT_REPORT),
                data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by RestoreOnResultReport.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_RestoreOnResultReport_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_IncrementalRestoreOnResultReport_0100
 * @tc.name: SUB_backup_ServiceReverseStub_IncrementalRestoreOnResultReport_0100
 * @tc.desc: Test function of IncrementalRestoreOnResultReport interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I90ZZX
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_IncrementalRestoreOnResultReport_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_RestoreOnResultReport_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, IncrementalRestoreOnResultReport(_, _, _)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        std::string resultReport = "result_report";
        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(resultReport));
        EXPECT_TRUE(data.WriteInt32(0));

        EXPECT_EQ(
            BError(BError::Codes::OK),
            service.OnRemoteRequest(
                static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_RESTORE_ON_RESULT_REPORT),
                data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by RestoreOnResultReport.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_IncrementalRestoreOnResultReport_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_BackupOnScanningInfo_0100
 * @tc.name: SUB_backup_ServiceReverseStub_BackupOnScanningInfo_0100
 * @tc.desc: Test function of BackupOnScanningInfo interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_BackupOnScanningInfo_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_BackupOnScanningInfo_0100";
    try {
        MockServiceReverse service;
        EXPECT_CALL(service, BackupOnScanningInfo(_)).WillOnce(Return());
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));

        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(IServiceReverseInterfaceCode::SERVICER_BACKUP_ON_SCANNED_INFO), data,
                      reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by BackupOnScanningInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_BackupOnScanningInfo_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverseStub_IncrementalBackupOnScanningInfo_0100
 * @tc.name: SUB_backup_ServiceReverseStub_IncrementalBackupOnScanningInfo_0100
 * @tc.desc: Test function of IncrementalBackupOnScanningInfo interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseStubTest,
         SUB_backup_ServiceReverseStub_IncrementalBackupOnScanningInfo_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) <<
        "ServiceReverseStubTest-begin SUB_backup_ServiceReverseStub_IncrementalBackupOnScanningInfo_0100";
    try {
        MockServiceReverse service;
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        EXPECT_TRUE(data.WriteInterfaceToken(IServiceReverse::GetDescriptor()));
        EXPECT_TRUE(data.WriteString(BUNDLE_NAME));

        EXPECT_CALL(service, IncrementalBackupOnScanningInfo(_)).WillOnce(Return());
        EXPECT_EQ(BError(BError::Codes::OK),
                  service.OnRemoteRequest(
                      static_cast<uint32_t>(
                        IServiceReverseInterfaceCode::SERVICER_INCREMENTAL_BACKUP_ON_SCANNED_INFO),
                        data, reply, option));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseStubTest-an exception occurred by BackupOnScanningInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseStubTest-end SUB_backup_ServiceReverseStub_IncrementalBackupOnScanningInfo_0100";
}
} // namespace OHOS::FileManagement::Backup