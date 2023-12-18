/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <gtest/gtest.h>
#include <memory>
#include <shared_mutex>
#include <vector>

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "b_session_restore_async.h"
#include "backup_kit_inner.h"
#include "unique_fd.h"
#include "utils_mock_global_variable.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static void OnFileReady(const BFileInfo &fileInfo, UniqueFd fd)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest OnFileReady OK";
}

static void OnBundleStarted(ErrCode err, const BundleName name)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest OnBundleStarted OK";
}

static void OnBundleFinished(ErrCode err, const BundleName name)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest OnBundleFinished OK";
}

static void OnAllBundlesFinished(ErrCode err)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest OnAllBundlesFinished OK";
}

static void OnBackupServiceDied()
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest OnBackupServiceDied OK";
}

class BSessionRestoreAsyncTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override;

    void Init();

    shared_ptr<BSessionRestoreAsync> restorePtr_;
    BSessionRestoreAsync::Callbacks callbacks_;
};

void BSessionRestoreAsyncTest::SetUp()
{
    SetMockInitBackupOrRestoreSession(true);
    SetMockGetInstance(true);
    SetMockLoadSystemAbility(true);
    restorePtr_ = make_shared<BSessionRestoreAsync>(callbacks_);
}

void BSessionRestoreAsyncTest::TearDown()
{
    restorePtr_ = nullptr;
}

void BSessionRestoreAsyncTest::Init()
{
    callbacks_.onFileReady = OnFileReady;
    callbacks_.onBundleStarted = OnBundleStarted;
    callbacks_.onBundleFinished = OnBundleFinished;
    callbacks_.onAllBundlesFinished = OnAllBundlesFinished;
    callbacks_.onBackupServiceDied = OnBackupServiceDied;
}

/**
 * @tc.number: SUB_backup_b_session_restore_async_0100
 * @tc.name: SUB_backup_b_session_restore_async_0100
 * @tc.desc: 测试Callbacks接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(BSessionRestoreAsyncTest, SUB_backup_b_session_restore_async_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-begin SUB_backup_b_session_restore_async_0100";
    try {
        Init();
        BFileInfo bFileInfo("", "", 0);
        callbacks_.onFileReady(bFileInfo, UniqueFd(-1));
        callbacks_.onBundleStarted(ErrCode(BError::Codes::OK), "");
        callbacks_.onBundleFinished(ErrCode(BError::Codes::OK), "");
        callbacks_.onAllBundlesFinished(ErrCode(BError::Codes::OK));
        callbacks_.onBackupServiceDied();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-an exception occurred by Callbacks.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-end SUB_backup_b_session_restore_async_0100";
}

/**
 * @tc.number: SUB_backup_b_session_restore_async_0200
 * @tc.name: SUB_backup_b_session_restore_async_0200
 * @tc.desc: 测试Init接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(BSessionRestoreAsyncTest, SUB_backup_b_session_restore_async_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-begin SUB_backup_b_session_restore_async_0200";
    try {
        auto restorePtr = BSessionRestoreAsync::Init(callbacks_);
        EXPECT_NE(restorePtr, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-an exception occurred by Init.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-end SUB_backup_b_session_restore_async_0200";
}

/**
 * @tc.number: SUB_backup_b_session_restore_async_0300
 * @tc.name: SUB_backup_b_session_restore_async_0300
 * @tc.desc: 测试PublishFile接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(BSessionRestoreAsyncTest, SUB_backup_b_session_restore_async_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-begin SUB_backup_b_session_restore_async_0300";
    try {
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        BFileInfo bFileInfo("", "", 0);
        auto ret = restorePtr_->PublishFile(bFileInfo);
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        ret = restorePtr_->PublishFile(bFileInfo);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-an exception occurred by PublishFile.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-end SUB_backup_b_session_restore_async_0300";
}

/**
 * @tc.number: SUB_backup_b_session_restore_async_0400
 * @tc.name: SUB_backup_b_session_restore_async_0400
 * @tc.desc: 测试GetFileHandle接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(BSessionRestoreAsyncTest, SUB_backup_b_session_restore_async_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-begin SUB_backup_b_session_restore_async_0400";
    try {
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        string bundleName = "";
        string fileName = "";
        auto ret = restorePtr_->GetFileHandle(bundleName, fileName);
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        ret = restorePtr_->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-end SUB_backup_b_session_restore_async_0400";
}

/**
 * @tc.number: SUB_backup_b_session_restore_async_0500
 * @tc.name: SUB_backup_b_session_restore_async_0500
 * @tc.desc: 测试AppendBundles接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(BSessionRestoreAsyncTest, SUB_backup_b_session_restore_async_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-begin SUB_backup_b_session_restore_async_0500";
    try {
        SetMockGetInstance(true);
        SetMockLoadSystemAbility(true);
        vector<string> bundleNames;
        ErrCode ret = restorePtr_->AppendBundles(UniqueFd(-1), bundleNames);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
        ret = restorePtr_->AppendBundles(UniqueFd(-1), bundleNames);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
        restorePtr_ = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-an exception occurred by ~BSessionRestoreAsync.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-end SUB_backup_b_session_restore_async_0500";
}

/**
 * @tc.number: SUB_backup_b_session_restore_async_0600
 * @tc.name: SUB_backup_b_session_restore_async_0600
 * @tc.desc: 测试RegisterBackupServiceDied接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(BSessionRestoreAsyncTest, SUB_backup_b_session_restore_async_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-begin SUB_backup_b_session_restore_async_0600";
    try {
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        restorePtr_->RegisterBackupServiceDied(nullptr);
        GTEST_LOG_(INFO) << "GetInstance is true but not equal to parameter";
        SetMockGetInstance(true);
        restorePtr_->RegisterBackupServiceDied(nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-an exception occurred by RegisterBackupServiceDied.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-end SUB_backup_b_session_restore_async_0600";
}

/**
 * @tc.number: SUB_backup_b_session_restore_async_0700
 * @tc.name: SUB_backup_b_session_restore_async_0700
 * @tc.desc: 测试OnBackupServiceDied接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(BSessionRestoreAsyncTest, SUB_backup_b_session_restore_async_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-begin SUB_backup_b_session_restore_async_0700";
    try {
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        GTEST_LOG_(INFO) << "callbacks_.onBackupServiceDied is nullptr";
        callbacks_.onBackupServiceDied = nullptr;
        restorePtr_->OnBackupServiceDied();
        GTEST_LOG_(INFO) << "callbacks_.onBackupServiceDied is not nullptr";
        Init();
        restorePtr_->OnBackupServiceDied();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-an exception occurred by OnBackupServiceDied.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-end SUB_backup_b_session_restore_async_0700";
}

/**
 * @tc.number: SUB_backup_b_session_restore_async_0800
 * @tc.name: SUB_backup_b_session_restore_async_0800
 * @tc.desc: 测试PopBundleInfo接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(BSessionRestoreAsyncTest, SUB_backup_b_session_restore_async_0800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-begin SUB_backup_b_session_restore_async_0800";
    try {
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        GTEST_LOG_(INFO) << "LoadSystemAbility is true";
        SetMockLoadSystemAbility(true);
        vector<string> bundleNames;
        restorePtr_->workList_.push({UniqueFd(-1), bundleNames, RESTORE_DATA_WAIT_SEND, -1});
        restorePtr_->PopBundleInfo();
        EXPECT_TRUE(restorePtr_->workList_.empty());
        restorePtr_->PopBundleInfo();
        EXPECT_TRUE(restorePtr_->workList_.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-an exception occurred by PopBundleInfo.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-end SUB_backup_b_session_restore_async_0800";
}

/**
 * @tc.number: SUB_backup_b_session_restore_async_0900
 * @tc.name: SUB_backup_b_session_restore_async_0900
 * @tc.desc: 测试AppendBundlesImpl接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(BSessionRestoreAsyncTest, SUB_backup_b_session_restore_async_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-begin SUB_backup_b_session_restore_async_0900";
    try {
        vector<string> bundleNames;
        BSessionRestoreAsync::AppendBundleInfo info;
        info.remoteCap = UniqueFd(-1);
        info.bundlesToRestore = bundleNames;
        info.restoreType = TypeRestoreTypeEnum::RESTORE_DATA_WAIT_SEND;
        info.userId = -1;
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        restorePtr_->AppendBundlesImpl(move(info));
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        restorePtr_->AppendBundlesImpl(move(info));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-an exception occurred by AppendBundlesImpl.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-end SUB_backup_b_session_restore_async_0900";
}

/**
 * @tc.number: SUB_backup_b_session_restore_async_1000
 * @tc.name: SUB_backup_b_session_restore_async_1000
 * @tc.desc: 测试OnBundleStarted接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(BSessionRestoreAsyncTest, SUB_backup_b_session_restore_async_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-begin SUB_backup_b_session_restore_async_1000";
    try {
        vector<string> bundleNames;
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        GTEST_LOG_(INFO) << "onBundleStarted is nullptr";
        callbacks_.onBundleStarted = nullptr;
        restorePtr_->OnBundleStarted(0x1000, bundleNames);
        GTEST_LOG_(INFO) << "onBundleStarted is func ptr";
        Init();
        restorePtr_->OnBundleStarted(0x1000, bundleNames);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-an exception occurred by OnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreAsyncTest-end SUB_backup_b_session_restore_async_1000";
}

} // namespace OHOS::FileManagement::Backup