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
#include "b_incremental_backup_session.h"
#include "b_incremental_restore_session.h"
#include "b_incremental_session_restore_async.h"
#include "service_proxy_mock.h"
#include "service_client.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class IncrementalSessionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
public:
    static inline sptr<ServiceProxyMock> proxy = nullptr;
    static inline shared_ptr<BIncrementalBackupSession> backupSession = nullptr;
    static inline shared_ptr<BIncrementalRestoreSession> restoreSession = nullptr;
    static inline shared_ptr<BIncrementalSessionRestoreAsync> restoreAsyncSession = nullptr;
};

void IncrementalSessionTest::SetUpTestCase()
{
    proxy = sptr<ServiceProxyMock>(new ServiceProxyMock(nullptr));
    backupSession = make_shared<BIncrementalBackupSession>();
    restoreSession = make_shared<BIncrementalRestoreSession>();
    BIncrementalSessionRestoreAsync::Callbacks callbacks;
    restoreAsyncSession = make_shared<BIncrementalSessionRestoreAsync>(callbacks);
    ServiceClient::serviceProxy_ = proxy;
}

void IncrementalSessionTest::TearDownTestCase()
{
    backupSession = nullptr;
    restoreSession = nullptr;
    restoreAsyncSession = nullptr;
    ServiceClient::serviceProxy_ = nullptr;
    proxy = nullptr;
}

/**
 * @tc.number: SUB_b_incremental_session_test_0100
 * @tc.name: SUB_b_incremental_session_test_0100
 * @tc.desc: 测试 InitIncrementalBackupSession 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_0100";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        BIncrementalBackupSession::Callbacks callbacks;
        EXPECT_TRUE(backupSession != nullptr);
        auto err = backupSession->Init(callbacks);
        EXPECT_EQ(err, nullptr);

        EXPECT_CALL(*proxy, InitIncrementalBackupSession(_)).WillOnce(Return(-1)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = backupSession->Init(callbacks);
        EXPECT_EQ(err, nullptr);
        err = backupSession->Init(callbacks);
        EXPECT_NE(err, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_0100";
}

void BackupSeviceDied() {}
/**
 * @tc.number: SUB_b_incremental_session_test_0200
 * @tc.name: SUB_b_incremental_session_test_0200
 * @tc.desc: 测试 RegisterBackupServiceDied 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_0200";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        EXPECT_TRUE(backupSession != nullptr);
        backupSession->RegisterBackupServiceDied(nullptr);

        ServiceClient::serviceProxy_ = proxy;
        backupSession->RegisterBackupServiceDied(nullptr);

        EXPECT_CALL(*proxy, AsObject()).WillOnce(Return(nullptr));
        backupSession->RegisterBackupServiceDied(BackupSeviceDied);
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_0200";
}

/**
 * @tc.number: SUB_b_incremental_session_test_0300
 * @tc.name: SUB_b_incremental_session_test_0300
 * @tc.desc: 测试 AppendBundles 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_0300";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        vector<BIncrementalData> bundlesToBackup;
        EXPECT_TRUE(backupSession != nullptr);
        auto err = backupSession->AppendBundles(bundlesToBackup);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, AppendBundlesIncrementalBackupSession(_)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = backupSession->AppendBundles(bundlesToBackup);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_0300";
}

/**
 * @tc.number: SUB_b_incremental_session_test_0400
 * @tc.name: SUB_b_incremental_session_test_0400
 * @tc.desc: 测试 Release 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_0400";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        EXPECT_TRUE(backupSession != nullptr);
        auto err = backupSession->Release();
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, Release()).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = backupSession->Release();
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_0400";
}

/**
 * @tc.number: SUB_b_incremental_session_test_0500
 * @tc.name: SUB_b_incremental_session_test_0500
 * @tc.desc: 测试 Init 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_0500";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        BIncrementalRestoreSession::Callbacks callbacks;
        EXPECT_TRUE(restoreSession != nullptr);
        auto err = restoreSession->Init(callbacks);
        EXPECT_EQ(err, nullptr);

        EXPECT_CALL(*proxy, InitRestoreSession(_)).WillOnce(Return(-1)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreSession->Init(callbacks);
        EXPECT_EQ(err, nullptr);
        err = restoreSession->Init(callbacks);
        EXPECT_NE(err, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_0500";
}

void RestoreSeviceDied() {}
/**
 * @tc.number: SUB_b_incremental_session_test_0600
 * @tc.name: SUB_b_incremental_session_test_0600
 * @tc.desc: 测试 RegisterBackupServiceDied 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_0600";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        EXPECT_TRUE(restoreSession != nullptr);
        restoreSession->RegisterBackupServiceDied(nullptr);

        ServiceClient::serviceProxy_ = proxy;
        restoreSession->RegisterBackupServiceDied(nullptr);

        EXPECT_CALL(*proxy, AsObject()).WillOnce(Return(nullptr));
        restoreSession->RegisterBackupServiceDied(RestoreSeviceDied);
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_0600";
}

/**
 * @tc.number: SUB_b_incremental_session_test_0700
 * @tc.name: SUB_b_incremental_session_test_0700
 * @tc.desc: 测试 AppendBundles 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_0700";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        UniqueFd remoteCap1;
        vector<BundleName> bundlesToRestore;
        EXPECT_TRUE(restoreSession != nullptr);
        auto err = restoreSession->AppendBundles(move(remoteCap1), bundlesToRestore);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, AppendBundlesRestoreSessionData(_, _, _, _)).WillOnce(Return(0));
        UniqueFd remoteCap2;
        ServiceClient::serviceProxy_ = proxy;
        err = restoreSession->AppendBundles(move(remoteCap2), bundlesToRestore);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_0700";
}

/**
 * @tc.number: SUB_b_incremental_session_test_0800
 * @tc.name: SUB_b_incremental_session_test_0800
 * @tc.desc: 测试 Release 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_0800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_0800";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        EXPECT_TRUE(restoreSession != nullptr);
        auto err = restoreSession->Release();
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, Release()).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreSession->Release();
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_0800";
}

/**
 * @tc.number: SUB_b_incremental_session_test_0900
 * @tc.name: SUB_b_incremental_session_test_0900
 * @tc.desc: 测试 PublishFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_0900";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        BFileInfo fileInfo;
        EXPECT_TRUE(restoreSession != nullptr);
        auto err = restoreSession->PublishFile(fileInfo);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, PublishIncrementalFile(_)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreSession->PublishFile(fileInfo);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_0900";
}


/**
 * @tc.number: SUB_b_incremental_session_test_1000
 * @tc.name: SUB_b_incremental_session_test_1000
 * @tc.desc: 测试 GetFileHandle 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_1000";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        string bundleName;
        string fileName;
        EXPECT_TRUE(restoreSession != nullptr);
        auto err = restoreSession->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, GetIncrementalFileHandle(_, _)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreSession->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_1000";
}

/**
 * @tc.number: SUB_b_incremental_session_test_1100
 * @tc.name: SUB_b_incremental_session_test_1100
 * @tc.desc: 测试 AppendBundles 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_1100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_1100";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        UniqueFd remoteCap;
        vector<BundleName> bundlesToRestore;
        vector<std::string> detailInfos;
        EXPECT_TRUE(restoreSession != nullptr);
        auto err = restoreSession->AppendBundles(move(remoteCap), bundlesToRestore, detailInfos);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, AppendBundlesRestoreSessionDataByDetail(_, _, _, _, _)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreSession->AppendBundles(move(remoteCap), bundlesToRestore, detailInfos);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_1100";
}

/**
 * @tc.number: SUB_b_incremental_session_test_1200
 * @tc.name: SUB_b_incremental_session_test_1200
 * @tc.desc: 测试 Init 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_1200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_1200";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        BIncrementalSessionRestoreAsync::Callbacks callbacks;
        EXPECT_TRUE(restoreAsyncSession != nullptr);
        auto err = restoreAsyncSession->Init(callbacks);
        EXPECT_EQ(err, nullptr);

        EXPECT_CALL(*proxy, InitRestoreSession(_)).WillOnce(Return(-1)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreAsyncSession->Init(callbacks);
        EXPECT_EQ(err, nullptr);
        err = restoreAsyncSession->Init(callbacks);
        EXPECT_NE(err, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_1200";
}

void RestoreAsyncSeviceDied() {}
/**
 * @tc.number: SUB_b_incremental_session_test_1300
 * @tc.name: SUB_b_incremental_session_test_1300
 * @tc.desc: 测试 RegisterBackupServiceDied 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_1300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_1300";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        EXPECT_TRUE(restoreAsyncSession != nullptr);
        restoreAsyncSession->RegisterBackupServiceDied(nullptr);

        ServiceClient::serviceProxy_ = proxy;
        restoreAsyncSession->RegisterBackupServiceDied(nullptr);

        EXPECT_CALL(*proxy, AsObject()).WillOnce(Return(nullptr));
        restoreAsyncSession->RegisterBackupServiceDied(RestoreAsyncSeviceDied);
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_1300";
}

/**
 * @tc.number: SUB_b_incremental_session_test_1400
 * @tc.name: SUB_b_incremental_session_test_1400
 * @tc.desc: 测试 AppendBundles 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_1400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_1400";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        UniqueFd remoteCap1;
        vector<BundleName> bundlesToRestore;
        EXPECT_TRUE(restoreAsyncSession != nullptr);
        auto err = restoreAsyncSession->AppendBundles(move(remoteCap1), bundlesToRestore);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, AppendBundlesRestoreSessionData(_, _, _, _)).WillOnce(Return(0));
        UniqueFd remoteCap2;
        ServiceClient::serviceProxy_ = proxy;
        err = restoreAsyncSession->AppendBundles(move(remoteCap2), bundlesToRestore);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_1400";
}

/**
 * @tc.number: SUB_b_incremental_session_test_1500
 * @tc.name: SUB_b_incremental_session_test_1500
 * @tc.desc: 测试 PublishFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_1500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_1500";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        BFileInfo fileInfo;
        EXPECT_TRUE(restoreAsyncSession != nullptr);
        auto err = restoreAsyncSession->PublishFile(fileInfo);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, PublishIncrementalFile(_)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreAsyncSession->PublishFile(fileInfo);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_1500";
}


/**
 * @tc.number: SUB_b_incremental_session_test_1600
 * @tc.name: SUB_b_incremental_session_test_1600
 * @tc.desc: 测试 GetFileHandle 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_1600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_1600";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        string bundleName;
        string fileName;
        EXPECT_TRUE(restoreAsyncSession != nullptr);
        auto err = restoreAsyncSession->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, GetIncrementalFileHandle(_, _)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreAsyncSession->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_1600";
}

/**
 * @tc.number: SUB_b_incremental_session_test_1700
 * @tc.name: SUB_b_incremental_session_test_1700
 * @tc.desc: 测试 AppendBundles 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_1700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_1700";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        UniqueFd remoteCap;
        vector<BundleName> bundlesToRestore;
        vector<std::string> detailInfos;
        EXPECT_TRUE(restoreAsyncSession != nullptr);
        auto err = restoreAsyncSession->AppendBundles(move(remoteCap), bundlesToRestore, detailInfos);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, AppendBundlesRestoreSessionDataByDetail(_, _, _, _, _)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreAsyncSession->AppendBundles(move(remoteCap), bundlesToRestore, detailInfos);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_1700";
}

/**
 * @tc.number: SUB_b_incremental_session_test_1800
 * @tc.name: SUB_b_incremental_session_test_1800
 * @tc.desc: 测试 Release 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_1800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_1800";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        EXPECT_TRUE(restoreAsyncSession != nullptr);
        auto err = restoreAsyncSession->Release();
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, Release()).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreAsyncSession->Release();
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_1800";
}

/**
 * @tc.number: SUB_b_incremental_session_test_1900
 * @tc.name: SUB_b_incremental_session_test_1900
 * @tc.desc: 测试 AppendBundles 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_1900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_1900";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        vector<BIncrementalData> bundlesToBackup;
        vector<std::string> infos;
        EXPECT_TRUE(backupSession != nullptr);
        auto err = backupSession->AppendBundles(bundlesToBackup, infos);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, AppendBundlesIncrementalBackupSessionWithBundleInfos(_, _)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = backupSession->AppendBundles(bundlesToBackup, infos);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_1900";
}

/**
 * @tc.number: SUB_b_incremental_session_test_2000
 * @tc.name: SUB_b_incremental_session_test_2000
 * @tc.desc: 测试 PublishSAFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_2000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_2000";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        BFileInfo fileInfo;
        UniqueFd fd;
        EXPECT_TRUE(restoreSession != nullptr);
        auto err = restoreSession->PublishSAFile(fileInfo, move(fd));
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, PublishSAIncrementalFile(_, _)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreSession->PublishSAFile(fileInfo, move(fd));
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_2000";
}

/**
 * @tc.number: SUB_b_incremental_session_test_2100
 * @tc.name: SUB_b_incremental_session_test_2100
 * @tc.desc: 测试 Cancel 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_2100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_2100";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        std::string bundleName;
        EXPECT_TRUE(backupSession != nullptr);
        auto err = backupSession->Cancel(bundleName);
        EXPECT_EQ(err, BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK).GetCode());

        EXPECT_CALL(*proxy, Cancel(_, _)).WillOnce(DoAll(SetArgReferee<1>(0), Return(0)));
        ServiceClient::serviceProxy_ = proxy;
        err = backupSession->Cancel(bundleName);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by Cancel.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_2100";
}

/**
 * @tc.number: SUB_b_incremental_session_test_2200
 * @tc.name: SUB_b_incremental_session_test_2200
 * @tc.desc: 测试 Cancel 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_2200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_2200";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        std::string bundleName;
        EXPECT_TRUE(restoreSession != nullptr);
        auto err = restoreSession->Cancel(bundleName);
        EXPECT_EQ(err, BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK).GetCode());

        EXPECT_CALL(*proxy, Cancel(_, _)).WillOnce(DoAll(SetArgReferee<1>(0), Return(0)));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreSession->Cancel(bundleName);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by Cancel.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_2200";
}

/**
 * @tc.number: SUB_b_incremental_session_test_2300
 * @tc.name: SUB_b_incremental_session_test_2300
 * @tc.desc: 测试 Cancel 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_2300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_2300";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        std::string bundleName;
        EXPECT_TRUE(restoreAsyncSession != nullptr);
        auto err = restoreAsyncSession->Cancel(bundleName);
        EXPECT_EQ(err, BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK).GetCode());

        EXPECT_CALL(*proxy, Cancel(_, _)).WillOnce(DoAll(SetArgReferee<1>(0), Return(0)));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreAsyncSession->Cancel(bundleName);
        EXPECT_EQ(err, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by Cancel.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_2300";
}

/**
 * @tc.number: SUB_b_incremental_session_test_2400
 * @tc.name: SUB_b_incremental_session_test_2400
 * @tc.desc: 测试 InitIncrementalBackupSession 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_2400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_2400";
    try {
        std::string errMsg;
        ErrCode errCode;
        ServiceClient::serviceProxy_ = nullptr;
        BIncrementalBackupSession::Callbacks callbacks;
        EXPECT_TRUE(backupSession != nullptr);
        auto err = backupSession->Init(callbacks, errMsg, errCode);
        EXPECT_EQ(err, nullptr);

        EXPECT_CALL(*proxy, InitIncrementalBackupSessionWithErrMsg(_, _, _))
                    .WillOnce(DoAll(SetArgReferee<1>(-1), Return(0)))
                    .WillOnce(DoAll(SetArgReferee<1>(0), Return(0)));
        ServiceClient::serviceProxy_ = proxy;
        err = backupSession->Init(callbacks, errMsg, errCode);
        EXPECT_EQ(err, nullptr);
        err = backupSession->Init(callbacks, errMsg, errCode);
        EXPECT_NE(err, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_2400";
}

/**
 * @tc.number: SUB_b_incremental_session_test_2500
 * @tc.name: SUB_b_incremental_session_test_2500
 * @tc.desc: 测试 Init 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_2500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_2500";
    try {
        std::string errMsg;
        ErrCode errCode;
        ServiceClient::serviceProxy_ = nullptr;
        BIncrementalRestoreSession::Callbacks callbacks;
        EXPECT_TRUE(restoreSession != nullptr);
        auto err = restoreSession->Init(callbacks, errMsg, errCode);
        EXPECT_EQ(err, nullptr);

        EXPECT_CALL(*proxy, InitRestoreSessionWithErrMsg(_, _, _)).WillOnce(Return(-1)).WillOnce(Return(0));
        ServiceClient::serviceProxy_ = proxy;
        err = restoreSession->Init(callbacks, errMsg, errCode);
        EXPECT_EQ(err, nullptr);
        err = restoreSession->Init(callbacks, errMsg, errCode);
        EXPECT_NE(err, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_2500";
}

/**
 * @tc.number: SUB_b_incremental_session_test_2600
 * @tc.name: SUB_b_incremental_session_test_2600
 * @tc.desc: 测试 GetLocalCapabilities 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_2600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_2600";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        EXPECT_TRUE(backupSession != nullptr);
        auto err = backupSession->GetLocalCapabilities();
        EXPECT_EQ(err.Get(), -EPERM);

        ServiceClient::serviceProxy_ = proxy;
        EXPECT_CALL(*proxy, GetLocalCapabilitiesForBundleInfos(_)).WillOnce(Return(-1));
        err = backupSession->GetLocalCapabilities();
        EXPECT_EQ(err.Get(), -EPERM);

        EXPECT_CALL(*proxy, GetLocalCapabilitiesForBundleInfos(_))
            .WillOnce(DoAll(testing::SetArgReferee<0>(0), Return(0)));
        err = backupSession->GetLocalCapabilities();
        EXPECT_EQ(err.Get(), 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_2600";
}

/**
 * @tc.number: SUB_b_incremental_session_test_2700
 * @tc.name: SUB_b_incremental_session_test_2700
 * @tc.desc: 测试 GetLocalCapabilities 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_2700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_2700";
    try {
        ServiceClient::serviceProxy_ = nullptr;
        EXPECT_TRUE(restoreSession != nullptr);
        auto err = restoreSession->GetLocalCapabilities();
        EXPECT_EQ(err.Get(), -EPERM);

        ServiceClient::serviceProxy_ = proxy;
        EXPECT_CALL(*proxy, GetLocalCapabilitiesForBundleInfos(_)).WillOnce(Return(-1));
        err = restoreSession->GetLocalCapabilities();
        EXPECT_EQ(err.Get(), -EPERM);

        EXPECT_CALL(*proxy, GetLocalCapabilitiesForBundleInfos(_))
            .WillOnce(DoAll(testing::SetArgReferee<0>(0), Return(0)));
        err = restoreSession->GetLocalCapabilities();
        EXPECT_EQ(err.Get(), 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_2700";
}

/**
 * @tc.number: SUB_b_incremental_session_test_2800
 * @tc.name: SUB_b_incremental_session_test_2800
 * @tc.desc: 测试 GetBackupDataSize 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_2800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_2800";
    try {
        bool isPreciseScan = false;
        vector<BIncrementalData> bundleNameList;
        ServiceClient::serviceProxy_ = nullptr;
        ASSERT_TRUE(backupSession != nullptr);
        auto err = backupSession->GetBackupDataSize(isPreciseScan, bundleNameList);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        ServiceClient::serviceProxy_ = proxy;
        EXPECT_CALL(*proxy, GetBackupDataSize(_, _)).WillOnce(Return(BError(BError::Codes::SDK_BROKEN_IPC).GetCode()));
        err = backupSession->GetBackupDataSize(isPreciseScan, bundleNameList);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, GetBackupDataSize(_, _)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        err = backupSession->GetBackupDataSize(isPreciseScan, bundleNameList);
        EXPECT_EQ(err, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by GetBackupDataSize.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_2800";
}

/**
 * @tc.number: SUB_b_incremental_session_test_2900
 * @tc.name: SUB_b_incremental_session_test_2900
 * @tc.desc: 测试 CleanBundleTempDir 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IC7RHQ
 */
HWTEST_F(IncrementalSessionTest, SUB_b_incremental_session_test_2900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IncrementalSessionTest-begin SUB_b_incremental_session_test_2900";
    try {
        std::string bundleName;
        ServiceClient::serviceProxy_ = nullptr;
        ASSERT_TRUE(backupSession != nullptr);
        auto err = backupSession->CleanBundleTempDir(bundleName);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        ServiceClient::serviceProxy_ = proxy;
        EXPECT_CALL(*proxy, CleanBundleTempDir(_)).WillOnce(Return(BError(BError::Codes::SDK_BROKEN_IPC).GetCode()));
        err = backupSession->CleanBundleTempDir(bundleName);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        EXPECT_CALL(*proxy, CleanBundleTempDir(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        err = backupSession->CleanBundleTempDir(bundleName);
        EXPECT_EQ(err, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "IncrementalSessionTest-an exception occurred by CleanBundleTempDir.";
    }
    GTEST_LOG_(INFO) << "IncrementalSessionTest-end SUB_b_incremental_session_test_2900";
}
} // namespace OHOS::FileManagement::Backup