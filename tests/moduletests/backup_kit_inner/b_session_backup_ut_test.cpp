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

#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "backup_kit_inner.h"
#include "unique_fd.h"
#include "utils_mock_global_variable.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static void OnFileReady(const BFileInfo &fileInfo, UniqueFd fd, ErrCode errCode)
{
    GTEST_LOG_(INFO) << "BSessionBackupUtTest OnFileReady OK";
}

static void OnBundleStarted(ErrCode err, const BundleName name)
{
    GTEST_LOG_(INFO) << "BSessionBackupUtTest OnBundleStarted OK";
}

static void OnBundleFinished(ErrCode err, const BundleName name)
{
    GTEST_LOG_(INFO) << "BSessionBackupUtTest OnBundleFinished OK";
}

static void OnAllBundlesFinished(ErrCode err)
{
    GTEST_LOG_(INFO) << "BSessionBackupUtTest OnAllBundlesFinished OK";
}

static void OnBackupServiceDied()
{
    GTEST_LOG_(INFO) << "BSessionBackupUtTest OnBackupServiceDied OK";
}

class BSessionBackupUtTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override;

    void Init();

    unique_ptr<BSessionBackup> backupPtr_ = nullptr;
    BSessionBackup::Callbacks callbacks_;
};

void BSessionBackupUtTest::SetUp()
{
    SetMockInitBackupOrRestoreSession(true);
    SetMockGetInstance(true);
    SetMockLoadSystemAbility(true);
    backupPtr_ = make_unique<BSessionBackup>();
}

void BSessionBackupUtTest::TearDown()
{
    backupPtr_ = nullptr;
}

void BSessionBackupUtTest::Init()
{
    callbacks_.onFileReady = OnFileReady;
    callbacks_.onBundleStarted = OnBundleStarted;
    callbacks_.onBundleFinished = OnBundleFinished;
    callbacks_.onAllBundlesFinished = OnAllBundlesFinished;
    callbacks_.onBackupServiceDied = OnBackupServiceDied;
}

/**
 * @tc.number: SUB_backup_b_session_backup_0100
 * @tc.name: SUB_backup_b_session_backup_0100
 * @tc.desc: 测试Callbacks接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionBackupUtTest, SUB_backup_b_session_backup_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupUtTest-begin SUB_backup_b_session_backup_0100";
    try {
        Init();
        BFileInfo backupFileInfo("", "", 0);
        callbacks_.onFileReady(backupFileInfo, UniqueFd(-1), 0);
        callbacks_.onBundleStarted(ErrCode(BError::Codes::OK), "");
        callbacks_.onBundleFinished(ErrCode(BError::Codes::OK), "");
        callbacks_.onAllBundlesFinished(ErrCode(BError::Codes::OK));
        callbacks_.onBackupServiceDied();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupUtTest-an exception occurred by Callbacks.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupUtTest-end SUB_backup_b_session_backup_0400";
}

/**
 * @tc.number: SUB_backup_b_session_backup_1000
 * @tc.name: SUB_backup_b_session_backup_1000
 * @tc.desc: 测试Cancel接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionBackupUtTest, SUB_backup_b_session_backup_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupUtTest-Cancel SUB_backup_b_session_backup_1000";
    try {
        if (backupPtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_backup_1000 backupPtr_ == nullptr";
            return;
        }
        BundleName bundleName;
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        auto ret = backupPtr_->Cancel(bundleName);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        ret = backupPtr_->Cancel(bundleName);
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupUtTest-an exception occurred by Cancel.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupUtTest-end SUB_backup_b_session_backup_1000";
}

/**
 * @tc.number: SUB_backup_b_session_backup_1100
 * @tc.name: SUB_backup_b_session_backup_1100
 * @tc.desc: 测试 GetLocalCapabilities 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(BSessionBackupUtTest, SUB_backup_b_session_backup_1100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupUtTest-begin SUB_backup_b_session_backup_1100";
    try {
        if (backupPtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_backup_1100 backupPtr_ == nullptr";
            return;
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        auto err = backupPtr_->GetLocalCapabilities();
        EXPECT_LT(err, 0);
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        err = backupPtr_->GetLocalCapabilities();
        EXPECT_LT(err, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupUtTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupUtTest-end SUB_backup_b_session_backup_1100";
}

/**
 * @tc.number: SUB_backup_b_session_backup_1200
 * @tc.name: SUB_backup_b_session_backup_1200
 * @tc.desc: 测试 GetBackupDataSize 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(BSessionBackupUtTest, SUB_backup_b_session_backup_1200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupUtTest-begin SUB_backup_b_session_backup_1200";
    try {
        if (backupPtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_backup_1200 backupPtr_ == nullptr";
            return;
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        bool isPreciseScan = true;
        vector<BIncrementalData> bundleNameList;
        SetMockGetInstance(false);
        auto err = backupPtr_->GetBackupDataSize(isPreciseScan, bundleNameList);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        err = backupPtr_->GetBackupDataSize(isPreciseScan, bundleNameList);
        EXPECT_EQ(err, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupUtTest-an exception occurred by GetBackupDataSize.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupUtTest-end SUB_backup_b_session_backup_1200";
}

/**
 * @tc.number: SUB_backup_b_session_backup_1300
 * @tc.name: SUB_backup_b_session_backup_1300
 * @tc.desc: 测试 CleanBundleTempDir 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IC7RHQ
 */
HWTEST_F(BSessionBackupUtTest, SUB_backup_b_session_backup_1300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupUtTest-begin SUB_backup_b_session_backup_1300";
    try {
        if (backupPtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_backup_1300 backupPtr_ == nullptr";
            return;
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        std::string bundleName;
        SetMockGetInstance(false);
        auto err = backupPtr_->CleanBundleTempDir(bundleName);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        err = backupPtr_->CleanBundleTempDir(bundleName);
        EXPECT_EQ(err, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupUtTest-an exception occurred by CleanBundleTempDir.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupUtTest-end SUB_backup_b_session_backup_1300";
}

/**
 * @tc.number: SUB_BSessionBackup_GetCompatibilityInfo_0000
 * @tc.name: SUB_BSessionBackup_GetCompatibilityInfo_0000
 * @tc.desc: 测试 GetCompatibilityInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BSessionBackupUtTest, SUB_BSessionBackup_GetCompatibilityInfo_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupUtTest-begin SUB_BSessionBackup_GetCompatibilityInfo_0000";
    try {
        ASSERT_TRUE(backupPtr_ != nullptr);
        std::string bundleName = "com.example.app";
        std::string extInfo = "";
        std::string compatInfo = "";
        SetMockGetInstance(false);
        auto err = backupPtr_->GetCompatibilityInfo(bundleName, extInfo, compatInfo);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        SetMockGetInstance(true);
        err = backupPtr_->GetCompatibilityInfo(bundleName, extInfo, compatInfo);
        EXPECT_EQ(err, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupUtTest-an exception occurred by GetCompatibilityInfo.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupUtTest-end SUB_BSessionBackup_GetCompatibilityInfo_0000";
}
}