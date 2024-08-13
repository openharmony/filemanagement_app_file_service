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

#include <iostream>
#include <map>
#include <sstream>

#include <gtest/gtest.h>

#include "b_resources/b_constants.h"
#include "tools_op.h"
#include "tools_op_incremental_backup.cpp"
#include "utils_mock_global_variable.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
class ToolsOpIncrementalBackupTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_tools_op_backup_0100
 * @tc.name: SUB_backup_tools_op_backup_0100
 * @tc.desc: 测试backup流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_backup_0100";
    try {
        SetMockGetInstance(true);
        GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-info";
        map<string, vector<string>> mapArgToVal;
        string localCap = string(BConstants::GetSaBundleBackupToolDir(BConstants::DEFAULT_USER_ID).data()) + "/tmp";
        vector<string> path = { localCap.data() };
        mapArgToVal.insert(make_pair("pathCapFile", path));
        vector<string> bundles = { "com.example.app2backup" };
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> incrementalTime = { "0" };
        mapArgToVal.insert(make_pair("incrementalTime", incrementalTime));

        // 尝试匹配当前命令，成功后执行
        GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-backup";
        vector<string_view> curOp;
        curOp.emplace_back("incrementalbackup");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        if (matchedOp != opeartions.end()) {
            auto ret = matchedOp->Execute(mapArgToVal);
            EXPECT_EQ(ret, 0);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_backup_0100";
}

/**
 * @tc.number: SUB_backup_tools_op_backup_0200
 * @tc.name: SUB_backup_tools_op_backup_0200
 * @tc.desc: 测试Exec分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_backup_0200";
    try {
        GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-The pathCapFile field is not contained.";
        map<string, vector<string>> mapArgToVal;
        vector<string> bundles = {"com.example.app2backup"};
        vector<string> path = {"/data/backup/tmp"};
        vector<string> incrementalTime = {"0"};
        mapArgToVal.insert(make_pair("bundles", bundles));
        mapArgToVal.insert(make_pair("incrementalTime", incrementalTime));

        vector<string_view> curOp;
        curOp.emplace_back("incrementalbackup");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        int ret = 0;
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }

        GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-The bundles field is noGt contained.";
        mapArgToVal.clear();
        mapArgToVal.insert(make_pair("pathCapFile", path));
        mapArgToVal.insert(make_pair("incrementalTime", incrementalTime));
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }

        GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-The incrementalTime field is noGt contained.";
        mapArgToVal.clear();
        mapArgToVal.insert(make_pair("pathCapFile", path));
        mapArgToVal.insert(make_pair("bundles", bundles));
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }

        GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-The all fields are not contained.";
        mapArgToVal.clear();
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_backup_0200";
}

/**
 * @tc.number: SUB_backup_tools_op_backup_0300
 * @tc.name: SUB_backup_tools_op_backup_0300
 * @tc.desc: 测试Exec分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_backup_0300";
    try {
        GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-info";
        map<string, vector<string>> mapArgToVal;
        vector<string> path = {"/data/backup/tmp"};
        mapArgToVal.insert(make_pair("pathCapFile", path));
        vector<string> bundles = {"com.example.app2backup"};
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> incrementalTime = {"0"};
        mapArgToVal.insert(make_pair("incrementalTime", incrementalTime));

        // 尝试匹配当前命令，成功后执行
        GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-backup";
        vector<string_view> curOp;
        curOp.emplace_back("incrementalbackup");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        if (matchedOp != opeartions.end()) {
            auto ret = matchedOp->Execute(mapArgToVal);
            EXPECT_EQ(ret, 0);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_backup_0300";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_0400
 * @tc.name: SUB_backup_tools_op_incremental_backup_0400
 * @tc.desc: 测试OnFileReady分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_restore_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_restore_0400";
    try {
        auto ctx = make_shared<SessionBckup>();
        BFileInfo fileInfo;
        fileInfo.owner = "test";
        fileInfo.fileName = "/manage.json";
        fileInfo.sn = 1;
        UniqueFd fd(open("textFile", O_RDONLY));
        UniqueFd manifestFd(open("textManifest", O_RDONLY));
        OnFileReady(ctx, fileInfo, move(fd), move(manifestFd));
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_restore_0400";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_0500
 * @tc.name: SUB_backup_tools_op_incremental_backup_0500
 * @tc.desc: 测试OnBundleStarted分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_0500";
    try {
        auto ctx = make_shared<SessionBckup>();
        ErrCode err = 0;
        BundleName name = "bundle";
        OnBundleStarted(ctx, err, name);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_0500";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_0501
 * @tc.name: SUB_backup_tools_op_incremental_backup_0501
 * @tc.desc: 测试OnBundleStarted分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_0501, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_0501";
    try {
        auto ctx = make_shared<SessionBckup>();
        ctx->SetBundleFinishedCount(1);
        ErrCode err = -1;
        BundleName name = "bundle";
        OnBundleStarted(ctx, err, name);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_0501";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_0600
 * @tc.name: SUB_backup_tools_op_incremental_backup_0600
 * @tc.desc: 测试OnBundleFinished分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_0600";
    try {
        auto ctx = make_shared<SessionBckup>();
        ctx->SetBundleFinishedCount(1);
        ErrCode err = 0;
        BundleName name = "bundle";
        OnBundleFinished(ctx, err, name);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_0600";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_0700
 * @tc.name: SUB_backup_tools_op_incremental_backup_0700
 * @tc.desc: 测试OnAllBundlesFinished分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_0700";
    try {
        auto ctx = make_shared<SessionBckup>();
        ErrCode err = 0;
        OnAllBundlesFinished(ctx, err);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_0700";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_0701
 * @tc.name: SUB_backup_tools_op_incremental_backup_0701
 * @tc.desc: 测试OnAllBundlesFinished分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_0701, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_0701";
    try {
        auto ctx = make_shared<SessionBckup>();
        ErrCode err = -1;
        OnAllBundlesFinished(ctx, err);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_0701";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_0800
 * @tc.name: SUB_backup_tools_op_incremental_backup_0800
 * @tc.desc: 测试OnBackupServiceDied分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_0800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_0800";
    try {
        auto ctx = make_shared<SessionBckup>();
        OnBackupServiceDied(ctx);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_0800";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_0900
 * @tc.name: SUB_backup_tools_op_incremental_backup_0900
 * @tc.desc: 测试BackupToolDirSoftlinkToBackupDir分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_0900";
    try {
        BackupToolDirSoftlinkToBackupDir();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_0900";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_1000
 * @tc.name: SUB_backup_tools_op_incremental_backup_1000
 * @tc.desc: 测试GetLocalCapabilitiesIncremental分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_1000";
    try {
        auto ctx = make_shared<SessionBckup>();
        string pathCapFile = "";
        vector<string> bundleNames;
        bundleNames.push_back("bundle1");
        bundleNames.push_back("bundle2");
        vector<string> times = {"100"};
        int32_t ret = GetLocalCapabilitiesIncremental(ctx, pathCapFile, bundleNames, times);
        EXPECT_EQ(-EPERM, ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_1000";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_1001
 * @tc.name: SUB_backup_tools_op_incremental_backup_1001
 * @tc.desc: 测试GetLocalCapabilitiesIncremental分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_1001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_1001";
    try {
        auto ctx = make_shared<SessionBckup>();
        string pathCapFile = "/data/backup";
        vector<string> bundleNames;
        bundleNames.push_back("bundle1");
        bundleNames.push_back("bundle2");
        vector<string> times;
        times.push_back("100");
        times.push_back("200");
        int32_t ret = GetLocalCapabilitiesIncremental(ctx, pathCapFile, bundleNames, times);
        EXPECT_EQ(-EPERM, ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_1001";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_1100
 * @tc.name: SUB_backup_tools_op_incremental_backup_1100
 * @tc.desc: 测试Init分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_1100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_1100";
    try {
        string pathCapFile = "/data/backup";
        vector<string> bundleNames;
        bundleNames.push_back("bundle1");
        bundleNames.push_back("bundle2");
        vector<string> times;
        times.push_back("100");
        times.push_back("200");
        int32_t ret = Init(pathCapFile, bundleNames, times);
        EXPECT_EQ(-EPERM, ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_1100";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_1200
 * @tc.name: SUB_backup_tools_op_incremental_backup_1200
 * @tc.desc: 测试Exec分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_1200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_1200";
    try {
        map<string, vector<string>> mapArgToVal = {
            {"pathCapFile", {"path"}},
            {"bundles", {"bundle1", "bundle2"}},
            {"incrementalTime", {"time"}}
        };
        int ret = g_exec(mapArgToVal);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_1200";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_1201
 * @tc.name: SUB_backup_tools_op_incremental_backup_1201
 * @tc.desc: 测试Exec分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_1201, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_1201";
    try {
        map<string, vector<string>> mapArgToVal = {
            {"bundles", {"bundle1"}},
            {"incrementalTime", {"time"}}
        };
        int ret = g_exec(mapArgToVal);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_1201";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_1202
 * @tc.name: SUB_backup_tools_op_incremental_backup_1202
 * @tc.desc: 测试Exec分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_1202, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_1202";
    try {
        map<string, vector<string>> mapArgToVal = {
            {"pathCapFile", {"path"}},
            {"incrementalTime", {"time"}}
        };
        int ret = g_exec(mapArgToVal);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_1202";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_backup_1203
 * @tc.name: SUB_backup_tools_op_incremental_backup_1203
 * @tc.desc: 测试Exec分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalBackupTest, SUB_backup_tools_op_incremental_backup_1203, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-begin SUB_backup_tools_op_incremental_backup_1203";
    try {
        map<string, vector<string>> mapArgToVal = {
            {"pathCapFile", {"path"}},
            {"bundles", {"bundle1"}}
        };
        int ret = g_exec(mapArgToVal);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalBackupTest-end SUB_backup_tools_op_incremental_backup_1203";
}
} // namespace OHOS::FileManagement::Backup