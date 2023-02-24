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

#include <cstddef>
#include <cstdio>
#include <fcntl.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <message_parcel.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "b_session_restore.h"
#include "refbase.h"
#include "service_reverse.h"
#include "test_manager.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string FILE_NAME = "1.tar";
} // namespace

class ServiceReverseTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override;

    void Init(IServiceReverse::Scenario scenario, int nType = 0);
    sptr<ServiceReverse> service_ = nullptr;
};

static void OnFileReadyTest(const BFileInfo &fileInfo, UniqueFd fd)
{
    EXPECT_EQ(fileInfo.owner, BUNDLE_NAME);
    EXPECT_EQ(fileInfo.fileName, FILE_NAME);
    GTEST_LOG_(INFO) << "ServiceReverseTest-OnFileReadyTest SUCCESS";
}

static void OnBundleStartedTest(ErrCode err, const BundleName name)
{
    EXPECT_EQ(name, BUNDLE_NAME);
    GTEST_LOG_(INFO) << "ServiceReverseTest-OnBundleStartedTest SUCCESS";
}

static void OnBundleFinishedTest(ErrCode err, const BundleName name)
{
    EXPECT_EQ(name, BUNDLE_NAME);
    GTEST_LOG_(INFO) << "ServiceReverseTest-OnBundleFinishedTest SUCCESS";
}

static void OnAllBundlesFinishedTest(ErrCode err)
{
    EXPECT_EQ(err, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceReverseTest-OnAllBundlesFinishedTest SUCCESS";
}

static void OnBackupServiceDiedTest() {}

void ServiceReverseTest::TearDown()
{
    service_ = nullptr;
}

void ServiceReverseTest::Init(IServiceReverse::Scenario scenario, int nType)
{
    if (scenario == IServiceReverse::Scenario::BACKUP) {
        if (nType) {
            service_ = new ServiceReverse(BSessionBackup::Callbacks {.onFileReady = nullptr,
                                                                     .onBundleStarted = nullptr,
                                                                     .onBundleFinished = nullptr,
                                                                     .onAllBundlesFinished = nullptr,
                                                                     .onBackupServiceDied = nullptr});
        } else {
            service_ = new ServiceReverse(BSessionBackup::Callbacks {
                .onFileReady = bind(OnFileReadyTest, placeholders::_1, placeholders::_2),
                .onBundleStarted = bind(OnBundleStartedTest, placeholders::_1, placeholders::_2),
                .onBundleFinished = bind(OnBundleFinishedTest, placeholders::_1, placeholders::_2),
                .onAllBundlesFinished = bind(OnAllBundlesFinishedTest, placeholders::_1),
                .onBackupServiceDied = bind(OnBackupServiceDiedTest)});
        }
    } else {
        if (nType) {
            service_ = new ServiceReverse(BSessionRestore::Callbacks {.onFileReady = nullptr,
                                                                      .onBundleStarted = nullptr,
                                                                      .onBundleFinished = nullptr,
                                                                      .onAllBundlesFinished = nullptr,
                                                                      .onBackupServiceDied = nullptr});
        } else {
            service_ = new ServiceReverse(BSessionRestore::Callbacks {
                .onFileReady = bind(OnFileReadyTest, placeholders::_1, placeholders::_2),
                .onBundleStarted = bind(OnBundleStartedTest, placeholders::_1, placeholders::_2),
                .onBundleFinished = bind(OnBundleFinishedTest, placeholders::_1, placeholders::_2),
                .onAllBundlesFinished = bind(OnAllBundlesFinishedTest, placeholders::_1),
                .onBackupServiceDied = bind(OnBackupServiceDiedTest)});
        }
    }
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnFileReady_0100
 * @tc.name: SUB_backup_ServiceReverse_BackupOnFileReady_0100
 * @tc.desc: 测试 BackupOnFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnFileReady_0100";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        service_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1);
        service_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnFileReady_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnBundleStarted_0100
 * @tc.name: SUB_backup_ServiceReverse_BackupOnBundleStarted_0100
 * @tc.desc: 测试 BackupOnBundleStarted 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnBundleStarted_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnBundleStarted_0100";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        service_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnBundleFinished_0100
 * @tc.name: SUB_backup_ServiceReverse_BackupOnBundleFinished_0100
 * @tc.desc: 测试 BackupOnBundleFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnBundleFinished_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnBundleFinished_0100";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        service_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0100
 * @tc.name: SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0100
 * @tc.desc: 测试 BackupOnAllBundlesFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0100";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        service_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
        service_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnFileReady_0100
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnFileReady_0100
 * @tc.desc: 测试 RestoreOnFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnFileReady_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        service_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1);
        service_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnFileReady_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnBundleStarted_0100
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnBundleStarted_0100
 * @tc.desc: 测试 RestoreOnBundleStarted 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnBundleStarted_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnBundleStarted_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        service_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnBundleFinished_0100
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnBundleFinished_0100
 * @tc.desc: 测试 RestoreOnBundleFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnBundleFinished_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnBundleFinished_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        service_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0100
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0100
 * @tc.desc: 测试 RestoreOnAllBundlesFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        service_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
        service_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_0200
 * @tc.name: SUB_backup_ServiceReverse_0200
 * @tc.desc: 测试分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_0200";
    try {
        Init(IServiceReverse::Scenario::RESTORE, 1);
        service_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1);
        service_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_0200";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_0300
 * @tc.name: SUB_backup_ServiceReverse_0300
 * @tc.desc: 测试分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_0300";
    try {
        Init(IServiceReverse::Scenario::BACKUP, 1);
        service_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1);
        service_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_0300";
}
} // namespace OHOS::FileManagement::Backup