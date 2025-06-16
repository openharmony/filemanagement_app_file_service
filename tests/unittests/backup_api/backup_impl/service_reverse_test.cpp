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

    void Init(IServiceReverseType::Scenario scenario, int nType = 0);
    void IncrementalInit(IServiceReverseType::Scenario scenario, int nType = 0);
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

static void OnResultReportTest(std::string bundleName, std::string result)
{
    EXPECT_EQ(result, "result_report");
    EXPECT_EQ(bundleName, "com.example.app2backup");
    GTEST_LOG_(INFO) << "ServiceReverseTest-OnResultReportTest SUCCESS";
}

void ServiceReverseTest::TearDown()
{
    service_ = nullptr;
}

void ServiceReverseTest::Init(IServiceReverseType::Scenario scenario, int nType)
{
    if (scenario == IServiceReverseType::Scenario::BACKUP) {
        if (nType) {
            service_ = new ServiceReverse(BSessionBackup::Callbacks {.onFileReady = nullptr,
                                                                     .onBundleStarted = nullptr,
                                                                     .onBundleFinished = nullptr,
                                                                     .onAllBundlesFinished = nullptr,
                                                                     .onResultReport = nullptr,
                                                                     .onBackupServiceDied = nullptr});
        } else {
            service_ = new ServiceReverse(BSessionBackup::Callbacks {
                .onFileReady = bind(OnFileReadyTest, placeholders::_1, placeholders::_2),
                .onBundleStarted = bind(OnBundleStartedTest, placeholders::_1, placeholders::_2),
                .onBundleFinished = bind(OnBundleFinishedTest, placeholders::_1, placeholders::_2),
                .onAllBundlesFinished = bind(OnAllBundlesFinishedTest, placeholders::_1),
                .onResultReport = bind(OnResultReportTest, placeholders::_1, placeholders::_2),
                .onBackupServiceDied = bind(OnBackupServiceDiedTest)});
        }
    } else {
        if (nType) {
            service_ = new ServiceReverse(BSessionRestore::Callbacks {.onFileReady = nullptr,
                                                                      .onBundleStarted = nullptr,
                                                                      .onBundleFinished = nullptr,
                                                                      .onAllBundlesFinished = nullptr,
                                                                      .onResultReport = nullptr,
                                                                      .onBackupServiceDied = nullptr});
        } else {
            service_ = new ServiceReverse(BSessionRestore::Callbacks {
                .onFileReady = bind(OnFileReadyTest, placeholders::_1, placeholders::_2),
                .onBundleStarted = bind(OnBundleStartedTest, placeholders::_1, placeholders::_2),
                .onBundleFinished = bind(OnBundleFinishedTest, placeholders::_1, placeholders::_2),
                .onAllBundlesFinished = bind(OnAllBundlesFinishedTest, placeholders::_1),
                .onResultReport = bind(OnResultReportTest, placeholders::_1, placeholders::_2),
                .onBackupServiceDied = bind(OnBackupServiceDiedTest)});
        }
    }
}

void ServiceReverseTest::IncrementalInit(IServiceReverseType::Scenario scenario, int nType)
{
    if (scenario == IServiceReverseType::Scenario::BACKUP) {
        if (nType) {
            service_ = new ServiceReverse(BIncrementalBackupSession::Callbacks {.onFileReady = nullptr,
                                                                                .onBundleStarted = nullptr,
                                                                                .onBundleFinished = nullptr,
                                                                                .onAllBundlesFinished = nullptr,
                                                                                .onResultReport = nullptr,
                                                                                .onBackupServiceDied = nullptr});
        } else {
            service_ = new ServiceReverse(BIncrementalBackupSession::Callbacks {
                .onFileReady = bind(OnFileReadyTest, placeholders::_1, placeholders::_2),
                .onBundleStarted = bind(OnBundleStartedTest, placeholders::_1, placeholders::_2),
                .onBundleFinished = bind(OnBundleFinishedTest, placeholders::_1, placeholders::_2),
                .onAllBundlesFinished = bind(OnAllBundlesFinishedTest, placeholders::_1),
                .onResultReport = bind(OnResultReportTest, placeholders::_1, placeholders::_2),
                .onBackupServiceDied = bind(OnBackupServiceDiedTest)});
        }
    } else {
        if (nType) {
            service_ = new ServiceReverse(BIncrementalRestoreSession::Callbacks {.onFileReady = nullptr,
                                                                                 .onBundleStarted = nullptr,
                                                                                 .onBundleFinished = nullptr,
                                                                                 .onAllBundlesFinished = nullptr,
                                                                                 .onBackupServiceDied = nullptr});
        } else {
            service_ = new ServiceReverse(BIncrementalRestoreSession::Callbacks {
                .onFileReady = bind(OnFileReadyTest, placeholders::_1, placeholders::_2),
                .onBundleStarted = bind(OnBundleStartedTest, placeholders::_1, placeholders::_2),
                .onBundleFinished = bind(OnBundleFinishedTest, placeholders::_1, placeholders::_2),
                .onAllBundlesFinished = bind(OnAllBundlesFinishedTest, placeholders::_1),
                .onResultReport = bind(OnResultReportTest, placeholders::_1, placeholders::_2),
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
        Init(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnFileReady_0100 service_ == nullptr";
            return;
        }
        service_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
        service_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnFileReady_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnFileReady_0101
 * @tc.name: SUB_backup_ServiceReverse_BackupOnFileReady_0101
 * @tc.desc: 测试 BackupOnFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnFileReady_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnFileReady_0101";
    try {
        Init(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnFileReady_0101 service_ == nullptr";
            return;
        }
        service_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnFileReady_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnFileReady_0102
 * @tc.name: SUB_backup_ServiceReverse_BackupOnFileReady_0102
 * @tc.desc: 测试 BackupOnFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnFileReady_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnFileReady_0102";
    try {
        Init(IServiceReverseType::Scenario::BACKUP, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnFileReady_0102 service_ == nullptr";
            return;
        }
        service_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnFileReady_0102";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnFileReadyWithoutFd_0100
 * @tc.name: SUB_backup_ServiceReverse_BackupOnFileReadyWithoutFd_0100
 * @tc.desc: 测试 BackupOnFileReadyWithoutFd 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnFileReadyWithoutFd_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnFileReadyWithoutFd_0100";
    try {
        Init(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnFileReadyWithoutFd_0100 service_ == nullptr";
            return;
        }
        service_->BackupOnFileReadyWithoutFd(BUNDLE_NAME, FILE_NAME, 0);
        service_->RestoreOnFileReadyWithoutFd(BUNDLE_NAME, FILE_NAME, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnFileReadyWithoutFd_0100";
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
        Init(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnBundleStarted_0100 service_ == nullptr";
            return;
        }
        service_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnBundleStarted_0101
 * @tc.name: SUB_backup_ServiceReverse_BackupOnBundleStarted_0101
 * @tc.desc: 测试 BackupOnBundleStarted 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnBundleStarted_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnBundleStarted_0101";
    try {
        Init(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnBundleStarted_0101 service_ == nullptr";
            return;
        }
        service_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnBundleStarted_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnBundleStarted_0102
 * @tc.name: SUB_backup_ServiceReverse_BackupOnBundleStarted_0102
 * @tc.desc: 测试 BackupOnBundleStarted 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnBundleStarted_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnBundleStarted_0102";
    try {
        Init(IServiceReverseType::Scenario::BACKUP, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnBundleStarted_0102 service_ == nullptr";
            return;
        }
        service_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnBundleStarted_0102";
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
        Init(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnBundleFinished_0100 service_ == nullptr";
            return;
        }
        service_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnBundleFinished_0101
 * @tc.name: SUB_backup_ServiceReverse_BackupOnBundleFinished_0101
 * @tc.desc: 测试 BackupOnBundleFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnBundleFinished_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnBundleFinished_0101";
    try {
        Init(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnBundleFinished_0101 service_ == nullptr";
            return;
        }
        service_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnBundleFinished_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnBundleFinished_0102
 * @tc.name: SUB_backup_ServiceReverse_BackupOnBundleFinished_0102
 * @tc.desc: 测试 BackupOnBundleFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnBundleFinished_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnBundleFinished_0102";
    try {
        Init(IServiceReverseType::Scenario::BACKUP, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnBundleFinished_0102 service_ == nullptr";
            return;
        }
        service_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnBundleFinished_0102";
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
        Init(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0100 service_ == nullptr";
            return;
        }
        service_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
        service_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0101
 * @tc.name: SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0101
 * @tc.desc: 测试 BackupOnAllBundlesFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0101";
    try {
        Init(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0101 service_ == nullptr";
            return;
        }
        service_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0102
 * @tc.name: SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0102
 * @tc.desc: 测试 BackupOnAllBundlesFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0102";
    try {
        Init(IServiceReverseType::Scenario::BACKUP, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0102 service_ == nullptr";
            return;
        }
        service_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnAllBundlesFinished_0102";
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
        Init(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_RestoreOnFileReady_0100 service_ == nullptr";
            return;
        }
        service_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
        service_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnFileReady_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnFileReady_0101
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnFileReady_0101
 * @tc.desc: 测试 RestoreOnFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnFileReady_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnFileReady_0101";
    try {
        Init(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_RestoreOnFileReady_0101 service_ == nullptr";
            return;
        }
        service_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnFileReady_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnFileReady_0102
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnFileReady_0102
 * @tc.desc: 测试 RestoreOnFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnFileReady_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnFileReady_0102";
    try {
        Init(IServiceReverseType::Scenario::RESTORE, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_RestoreOnFileReady_0102 service_ == nullptr";
            return;
        }
        service_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnFileReady_0102";
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
        Init(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_RestoreOnBundleStarted_0100 service_ == nullptr";
            return;
        }
        service_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnBundleStarted_0101
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnBundleStarted_0101
 * @tc.desc: 测试 RestoreOnBundleStarted 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnBundleStarted_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnBundleStarted_0101";
    try {
        Init(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_RestoreOnBundleStarted_0101 service_ == nullptr";
            return;
        }
        service_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnBundleStarted_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnBundleStarted_0102
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnBundleStarted_0102
 * @tc.desc: 测试 RestoreOnBundleStarted 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnBundleStarted_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnBundleStarted_0102";
    try {
        Init(IServiceReverseType::Scenario::RESTORE, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_RestoreOnBundleStarted_0102 service_ == nullptr";
            return;
        }
        service_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnBundleStarted_0102";
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
        Init(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_RestoreOnBundleFinished_0100 service_ == nullptr";
            return;
        }
        service_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnBundleFinished_0101
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnBundleFinished_0101
 * @tc.desc: 测试 RestoreOnBundleFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnBundleFinished_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnBundleFinished_0101";
    try {
        Init(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_RestoreOnBundleFinished_0101 service_ == nullptr";
            return;
        }
        service_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnBundleFinished_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnBundleFinished_0102
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnBundleFinished_0102
 * @tc.desc: 测试 RestoreOnBundleFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnBundleFinished_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnBundleFinished_0102";
    try {
        Init(IServiceReverseType::Scenario::RESTORE, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_RestoreOnBundleFinished_0102 service_ == nullptr";
            return;
        }
        service_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnBundleFinished_0102";
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
        Init(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0100 service_ == nullptr";
            return;
        }
        service_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
        service_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0101
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0101
 * @tc.desc: 测试 RestoreOnAllBundlesFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0101";
    try {
        Init(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0101 service_ == nullptr";
            return;
        }
        service_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0102
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0102
 * @tc.desc: 测试 RestoreOnAllBundlesFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0102";
    try {
        Init(IServiceReverseType::Scenario::RESTORE, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0102 service_ == nullptr";
            return;
        }
        service_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnAllBundlesFinished_0102";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnProcessInfo_0100
 * @tc.name: SUB_backup_ServiceReverse_BackupOnProcessInfo_0100
 * @tc.desc: 测试 BackupOnProcessInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnProcessInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnProcessInfo_0100";
    try {
        Init(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnProcessInfo_0100 service_ == nullptr";
            return;
        }
        std::string bundleName = BUNDLE_NAME;
        std::string processInfo = "{\"timeInfo\": \"\", \"resultInfo\": \"\"}";
        service_->BackupOnProcessInfo(bundleName, processInfo);
        service_->RestoreOnProcessInfo(bundleName, processInfo);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnProcessInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnProcessInfo_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnProcessInfo_0101
 * @tc.name: SUB_backup_ServiceReverse_BackupOnProcessInfo_0101
 * @tc.desc: 测试 BackupOnProcessInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnProcessInfo_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnProcessInfo_0101";
    try {
        Init(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnProcessInfo_0101 service_ == nullptr";
            return;
        }
        std::string bundleName = BUNDLE_NAME;
        std::string processInfo = "{\"timeInfo\": \"\", \"resultInfo\": \"\"}";
        service_->BackupOnProcessInfo(bundleName, processInfo);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnProcessInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnProcessInfo_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnProcessInfo_0102
 * @tc.name: SUB_backup_ServiceReverse_BackupOnProcessInfo_0102
 * @tc.desc: 测试 BackupOnProcessInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnProcessInfo_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnProcessInfo_0102";
    try {
        Init(IServiceReverseType::Scenario::BACKUP, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnProcessInfo_0102 service_ == nullptr";
            return;
        }
        std::string bundleName = BUNDLE_NAME;
        std::string processInfo = "{\"timeInfo\": \"\", \"resultInfo\": \"\"}";
        service_->BackupOnProcessInfo(bundleName, processInfo);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnProcessInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnProcessInfo_0102";
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
        Init(IServiceReverseType::Scenario::RESTORE, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_0200 service_ == nullptr";
            return;
        }
        service_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
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
 * @tc.number: SUB_backup_ServiceReverse_0201
 * @tc.name: SUB_backup_ServiceReverse_0201
 * @tc.desc: 测试分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_0201, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_0201";
    try {
        Init(IServiceReverseType::Scenario::RESTORE, 0);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_0201 service_ == nullptr";
            return;
        }
        service_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
        service_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_0201";
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
        Init(IServiceReverseType::Scenario::BACKUP, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_0300 service_ == nullptr";
            return;
        }
        service_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
        service_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_0300";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_0301
 * @tc.name: SUB_backup_ServiceReverse_0301
 * @tc.desc: 测试分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_0301, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_0301";
    try {
        Init(IServiceReverseType::Scenario::BACKUP, 0);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_0301 service_ == nullptr";
            return;
        }
        service_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
        service_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_0301";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0100
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0100
 * @tc.desc: 测试 IncrementalBackupOnFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0100";
    try {
        IncrementalInit(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0100 service_ == nullptr";
            return;
        }
        service_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1, -1, 0);
        service_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1, -1, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0101
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0101
 * @tc.desc: 测试 IncrementalBackupOnFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0101";
    try {
        IncrementalInit(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0101 service_ == nullptr";
            return;
        }
        service_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1, -1, 0);

        IncrementalInit(IServiceReverseType::Scenario::BACKUP, 1);
        service_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1, -1, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalBackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnFileReadyWithoutFd_0100
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnFileReadyWithoutFd_0100
 * @tc.desc: 测试 IncrementalBackupOnFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_IncrementalBackupOnFileReadyWithoutFd_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) <<
        "ServiceReverseTestBegin SUB_backup_ServiceReverse_IncrementalBackupOnFileReadyWithoutFd_0100";
    try {
        IncrementalInit(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalBackupOnFileReady_0100 service_ == nullptr";
            return;
        }
        service_->IncrementalBackupOnFileReadyWithoutFd(BUNDLE_NAME, FILE_NAME, 0);
        service_->IncrementalRestoreOnFileReadyWithoutFd(BUNDLE_NAME, FILE_NAME, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnFileReadyWithoutFd_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnBundleStarted_0100
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnBundleStarted_0100
 * @tc.desc: 测试 IncrementalBackupOnBundleStarted 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalBackupOnBundleStarted_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalBackupOnBundleStarted_0100";
    try {
        IncrementalInit(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalBackupOnBundleStarted_0100 service_ == nullptr";
            return;
        }
        service_->IncrementalBackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalBackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnBundleStarted_0101
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnBundleStarted_0101
 * @tc.desc: 测试 IncrementalBackupOnBundleStarted 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalBackupOnBundleStarted_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalBackupOnBundleStarted_0101";
    try {
        IncrementalInit(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalBackupOnBundleStarted_0101 service_ == nullptr";
            return;
        }
        service_->IncrementalBackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);

        IncrementalInit(IServiceReverseType::Scenario::BACKUP, 1);
        service_->IncrementalBackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalBackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnBundleStarted_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnBundleFinished_0100
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnBundleFinished_0100
 * @tc.desc: 测试 IncrementalBackupOnBundleFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalBackupOnBundleFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalBackupOnBundleFinished_0100";
    try {
        IncrementalInit(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalBackupOnBundleFinished_0100 service_ == nullptr";
            return;
        }
        service_->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->IncrementalRestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalBackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnBundleFinished_0101
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnBundleFinished_0101
 * @tc.desc: 测试 IncrementalBackupOnBundleFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalBackupOnBundleFinished_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalBackupOnBundleFinished_0101";
    try {
        IncrementalInit(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalBackupOnBundleFinished_0101 service_ == nullptr";
            return;
        }
        service_->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);

        IncrementalInit(IServiceReverseType::Scenario::BACKUP, 1);
        service_->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalBackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnBundleFinished_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnAllBundlesFinished_0100
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnAllBundlesFinished_0100
 * @tc.desc: 测试 IncrementalBackupOnAllBundlesFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalBackupOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalBackupOnAllBundlesFinished_0100";
    try {
        IncrementalInit(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalBackupOnAllBundlesFinished_0100 service_ == nullptr";
            return;
        }
        service_->IncrementalBackupOnAllBundlesFinished(BError(BError::Codes::OK));
        service_->IncrementalRestoreOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalBackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnAllBundlesFinished_0101
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnAllBundlesFinished_0101
 * @tc.desc: 测试 IncrementalBackupOnAllBundlesFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalBackupOnAllBundlesFinished_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalBackupOnAllBundlesFinished_0101";
    try {
        IncrementalInit(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalBackupOnAllBundlesFinished_0101 service_ == nullptr";
            return;
        }
        service_->IncrementalBackupOnAllBundlesFinished(BError(BError::Codes::OK));

        IncrementalInit(IServiceReverseType::Scenario::BACKUP, 1);
        service_->IncrementalBackupOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalBackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnAllBundlesFinished_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalRestoreOnFileReady_0100
 * @tc.name: SUB_backup_ServiceReverse_IncrementalRestoreOnFileReady_0100
 * @tc.desc: 测试 IncrementalRestoreOnFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalRestoreOnFileReady_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalRestoreOnFileReady_0100";
    try {
        IncrementalInit(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalRestoreOnFileReady_0100 service_ == nullptr";
            return;
        }
        service_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1, -1, 0);
        service_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1, -1, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalRestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalRestoreOnFileReady_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalRestoreOnFileReady_0101
 * @tc.name: SUB_backup_ServiceReverse_IncrementalRestoreOnFileReady_0101
 * @tc.desc: 测试 IncrementalRestoreOnFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalRestoreOnFileReady_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalRestoreOnFileReady_0101";
    try {
        IncrementalInit(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalRestoreOnFileReady_0101 service_ == nullptr";
            return;
        }
        service_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1, -1, 0);

        IncrementalInit(IServiceReverseType::Scenario::RESTORE, 1);
        service_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1, -1, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalRestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalRestoreOnFileReady_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalRestoreOnBundleStarted_0100
 * @tc.name: SUB_backup_ServiceReverse_IncrementalRestoreOnBundleStarted_0100
 * @tc.desc: 测试 IncrementalRestoreOnBundleStarted 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalRestoreOnBundleStarted_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalRestoreOnBundleStarted_0100";
    try {
        IncrementalInit(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalRestoreOnBundleStarted_0100 service_ == nullptr";
            return;
        }
        service_->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->IncrementalBackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalRestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalRestoreOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalRestoreOnBundleStarted_0101
 * @tc.name: SUB_backup_ServiceReverse_IncrementalRestoreOnBundleStarted_0101
 * @tc.desc: 测试 IncrementalRestoreOnBundleStarted 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalRestoreOnBundleStarted_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalRestoreOnBundleStarted_0101";
    try {
        IncrementalInit(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalRestoreOnBundleStarted_0101 service_ == nullptr";
            return;
        }
        service_->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);

        IncrementalInit(IServiceReverseType::Scenario::RESTORE, 1);
        service_->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalRestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalRestoreOnBundleStarted_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalRestoreOnBundleFinished_0100
 * @tc.name: SUB_backup_ServiceReverse_IncrementalRestoreOnBundleFinished_0100
 * @tc.desc: 测试 IncrementalRestoreOnBundleFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalRestoreOnBundleFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalRestoreOnBundleFinished_0100";
    try {
        IncrementalInit(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalRestoreOnBundleFinished_0100 service_ == nullptr";
            return;
        }
        service_->IncrementalRestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        service_->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalRestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalRestoreOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalRestoreOnBundleFinished_0101
 * @tc.name: SUB_backup_ServiceReverse_IncrementalRestoreOnBundleFinished_0101
 * @tc.desc: 测试 IncrementalRestoreOnBundleFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalRestoreOnBundleFinished_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalRestoreOnBundleFinished_0101";
    try {
        IncrementalInit(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalRestoreOnBundleFinished_0101 service_ == nullptr";
            return;
        }
        service_->IncrementalRestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);

        IncrementalInit(IServiceReverseType::Scenario::RESTORE, 1);
        service_->IncrementalRestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalRestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalRestoreOnBundleFinished_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalRestoreOnAllBundlesFinished_0100
 * @tc.name: SUB_backup_ServiceReverse_IncrementalRestoreOnAllBundlesFinished_0100
 * @tc.desc: 测试 IncrementalRestoreOnAllBundlesFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalRestoreOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalRestoreOnAllBundlesFinished_0100";
    try {
        IncrementalInit(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalRestoreOnAllBundlesFinished_0100 service_ == nullptr";
            return;
        }
        service_->IncrementalRestoreOnAllBundlesFinished(BError(BError::Codes::OK));
        service_->IncrementalBackupOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalRestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalRestoreOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalRestoreOnAllBundlesFinished_0101
 * @tc.name: SUB_backup_ServiceReverse_IncrementalRestoreOnAllBundlesFinished_0101
 * @tc.desc: 测试 IncrementalRestoreOnAllBundlesFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9116W
 */
HWTEST_F(ServiceReverseTest,
         SUB_backup_ServiceReverse_IncrementalRestoreOnAllBundlesFinished_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalRestoreOnAllBundlesFinished_0101";
    try {
        IncrementalInit(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalRestoreOnAllBundlesFinished_0101 service_ == nullptr";
            return;
        }
        service_->IncrementalRestoreOnAllBundlesFinished(BError(BError::Codes::OK));

        IncrementalInit(IServiceReverseType::Scenario::RESTORE, 1);
        service_->IncrementalRestoreOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalRestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalRestoreOnAllBundlesFinished_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnResultReport_0100
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnResultReport_0100
 * @tc.desc: 测试 RestoreOnResultReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnResultReport_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnResultReport_0100";
    try {
        Init(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_ServiceReverse_RestoreOnResultReport_0100 service_ == nullptr";
            return;
        }
        std::string resultReport = "result_report";
        std::string bundleName = BUNDLE_NAME;
        service_->RestoreOnResultReport(resultReport, bundleName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnResultReport_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_RestoreOnResultReport_0101
 * @tc.name: SUB_backup_ServiceReverse_RestoreOnResultReport_0101
 * @tc.desc: 测试 RestoreOnResultReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_RestoreOnResultReport_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_RestoreOnResultReport_0101";
    try {
        Init(IServiceReverseType::Scenario::RESTORE, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_ServiceReverse_RestoreOnResultReport_0101 service_ == nullptr";
            return;
        }
        std::string resultReport = "result_report";
        std::string bundleName = BUNDLE_NAME;
        service_->RestoreOnResultReport(resultReport, bundleName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by RestoreOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_RestoreOnResultReport_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalRestoreOnResultReport_0100
 * @tc.name: SUB_backup_ServiceReverse_IncrementalRestoreOnResultReport_0100
 * @tc.desc: 测试 IncrementalRestoreOnResultReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_IncrementalRestoreOnResultReport_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalRestoreOnResultReport_0100";
    try {
        IncrementalInit(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_ServiceReverse_IncrementalRestoreOnResultReport_0100 service_ == nullptr";
            return;
        }
        std::string resultReport = "result_report";
        std::string bundleName = BUNDLE_NAME;
        service_->IncrementalRestoreOnResultReport(resultReport, bundleName, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalRestoreOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalRestoreOnResultReport_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalRestoreOnResultReport_0101
 * @tc.name: SUB_backup_ServiceReverse_IncrementalRestoreOnResultReport_0101
 * @tc.desc: 测试 IncrementalRestoreOnResultReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_IncrementalRestoreOnResultReport_0101,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalRestoreOnResultReport_0101";
    try {
        IncrementalInit(IServiceReverseType::Scenario::RESTORE, 1);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_ServiceReverse_IncrementalRestoreOnResultReport_0101 service_ == nullptr";
            return;
        }
        std::string resultReport = "result_report";
        std::string bundleName = BUNDLE_NAME;
        service_->IncrementalRestoreOnResultReport(resultReport, bundleName, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalRestoreOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalRestoreOnResultReport_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnResultReport_0100
 * @tc.name: SUB_backup_ServiceReverse_BackupOnResultReport_0100
 * @tc.desc: 测试 BackupOnResultReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnResultReport_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnResultReport_0100";
    try {
        Init(IServiceReverseType::Scenario::BACKUP);
        std::string resultReport = "result_report";
        std::string bundleName = BUNDLE_NAME;
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_ServiceReverse_BackupOnResultReport_0100 service_ == nullptr";
            return;
        }
        service_->BackupOnResultReport(resultReport, bundleName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnResultReport_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnResultReport_0101
 * @tc.name: SUB_backup_ServiceReverse_BackupOnResultReport_0101
 * @tc.desc: 测试 BackupOnResultReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnResultReport_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnResultReport_0101";
    try {
        Init(IServiceReverseType::Scenario::BACKUP, 1);
        std::string resultReport = "result_report";
        std::string bundleName = BUNDLE_NAME;
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_ServiceReverse_BackupOnResultReport_0101 service_ == nullptr";
            return;
        }
        service_->BackupOnResultReport(resultReport, bundleName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnResultReport_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnResultReport_0100
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnResultReport_0100
 * @tc.desc: 测试 IncrementalBackupOnResultReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_IncrementalBackupOnResultReport_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalBackupOnResultReport_0100";
    try {
        IncrementalInit(IServiceReverseType::Scenario::BACKUP);
        std::string resultReport = "result_report";
        std::string bundleName = BUNDLE_NAME;
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_ServiceReverse_IncrementalBackupOnResultReport_0100 service_ == nullptr";
            return;
        }
        service_->IncrementalBackupOnResultReport(resultReport, bundleName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalBackupOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnResultReport_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnResultReport_0101
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnResultReport_0101
 * @tc.desc: 测试 IncrementalBackupOnResultReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_IncrementalBackupOnResultReport_0101,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalBackupOnResultReport_0101";
    try {
        IncrementalInit(IServiceReverseType::Scenario::BACKUP, 1);
        std::string resultReport = "result_report";
        std::string bundleName = BUNDLE_NAME;
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_ServiceReverse_IncrementalBackupOnResultReport_0101 service_ == nullptr";
            return;
        }
        service_->IncrementalBackupOnResultReport(resultReport, bundleName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by IncrementalBackupOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnResultReport_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnScanningInfo_0100
 * @tc.name: SUB_backup_ServiceReverse_BackupOnScanningInfo_0100
 * @tc.desc: 测试 BackupOnScanningInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnScanningInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnScanningInfo_0100";
    try {
        Init(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnScanningInfo_0100 service_ == nullptr";
            return;
        }
        std::string scannedInfo = "";
        service_->BackupOnScanningInfo(scannedInfo);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnScanningInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnScanningInfo_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_BackupOnScanningInfo_0101
 * @tc.name: SUB_backup_ServiceReverse_BackupOnScanningInfo_0101
 * @tc.desc: 测试 BackupOnScanningInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest, SUB_backup_ServiceReverse_BackupOnScanningInfo_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_BackupOnScanningInfo_0101";
    try {
        Init(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_BackupOnScanningInfo_0101 service_ == nullptr";
            return;
        }
        std::string scannedInfo = "";
        service_->BackupOnScanningInfo(scannedInfo);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnScanningInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_BackupOnScanningInfo_0101";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnScanningInfo_0100
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnScanningInfo_0100
 * @tc.desc: 测试 IncrementalBackupOnScanningInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest,
    SUB_backup_ServiceReverse_IncrementalBackupOnScanningInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalBackupOnScanningInfo_0100";
    try {
        Init(IServiceReverseType::Scenario::RESTORE);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalBackupOnScanningInfo_0100 service_ == nullptr";
            return;
        }
        std::string scannedInfo = "";
        service_->BackupOnScanningInfo(scannedInfo);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnScanningInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnScanningInfo_0100";
}

/**
 * @tc.number: SUB_backup_ServiceReverse_IncrementalBackupOnScanningInfo_0101
 * @tc.name: SUB_backup_ServiceReverse_IncrementalBackupOnScanningInfo_0101
 * @tc.desc: 测试 IncrementalBackupOnScanningInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseTest,
    SUB_backup_ServiceReverse_IncrementalBackupOnScanningInfo_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseTest-begin SUB_backup_ServiceReverse_IncrementalBackupOnScanningInfo_0101";
    try {
        Init(IServiceReverseType::Scenario::BACKUP);
        if (service_ == nullptr) {
            GTEST_LOG_(INFO) <<
                "SUB_backup_ServiceReverse_IncrementalBackupOnScanningInfo_0101 service_ == nullptr";
            return;
        }
        std::string scannedInfo = "";
        service_->BackupOnScanningInfo(scannedInfo);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseTest-an exception occurred by BackupOnScanningInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseTest-end SUB_backup_ServiceReverse_IncrementalBackupOnScanningInfo_0101";
}
} // namespace OHOS::FileManagement::Backup