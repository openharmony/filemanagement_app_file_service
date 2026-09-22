/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "tools_op_backup.cpp"
#include "utils_mock_global_variable.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
class ToolsOpBackupTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

static void PerformBackupOperation(map<string, vector<string>> &mapArgToVal)
{
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-backup";
    vector<string_view> curOp;
    curOp.emplace_back("backup");
    auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
    auto &&opeartions = ToolsOp::GetAllOperations();
    auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
    if (matchedOp != opeartions.end()) {
        auto ret = matchedOp->Execute(mapArgToVal);
        EXPECT_EQ(ret, 0);
    }
}

static map<string, vector<string>> PrepareBackupArguments()
{
    map<string, vector<string>> mapArgToVal;
    vector<string> path = {"/data/backup/tmp"};
    mapArgToVal.insert(make_pair("pathCapFile", path));
    vector<string> bundles = {"com.example.app2backup", "com.example.app3backup", "com.example.app4backup"};
    mapArgToVal.insert(make_pair("bundles", bundles));
    vector<string> local = {"false"};
    mapArgToVal.insert(make_pair("isLocal", local));

    return mapArgToVal;
}

/**
 * @tc.number: SUB_backup_tools_op_backup_0100
 * @tc.name: SUB_backup_tools_op_backup_0100
 * @tc.desc: 测试backup流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpBackupTest, SUB_backup_tools_op_backup_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-begin SUB_backup_tools_op_backup_0100";
    try {
        SetMockGetInstance(true);
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-info";
        map<string, vector<string>> mapArgToVal;
        string localCap = string(BConstants::GetSaBundleBackupToolDir(BConstants::DEFAULT_USER_ID).data()) + "/tmp";
        vector<string> path = {localCap.data()};
        mapArgToVal.insert(make_pair("pathCapFile", path));
        vector<string> bundles = {"com.example.app2backup"};
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> local = {"true"};
        mapArgToVal.insert(make_pair("isLocal", local));

        // 尝试匹配当前命令，成功后执行
        PerformBackupOperation(mapArgToVal);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-end SUB_backup_tools_op_backup_0100";
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
HWTEST_F(ToolsOpBackupTest, SUB_backup_tools_op_backup_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-begin SUB_backup_tools_op_backup_0200";
    try {
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-The pathCapFile field is not contained.";
        map<string, vector<string>> mapArgToVal;
        vector<string> bundles = {"com.example.app2backup"};
        vector<string> path = {"/data/backup/tmp"};
        vector<string> local = {"true"};
        mapArgToVal.insert(make_pair("bundles", bundles));
        mapArgToVal.insert(make_pair("isLocal", local));

        vector<string_view> curOp;
        curOp.emplace_back("backup");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        int ret = 0;
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }

        GTEST_LOG_(INFO) << "ToolsOpBackupTest-The bundles field is noGt contained.";
        mapArgToVal.clear();
        mapArgToVal.insert(make_pair("pathCapFile", path));
        mapArgToVal.insert(make_pair("isLocal", local));
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }

        GTEST_LOG_(INFO) << "ToolsOpBackupTest-The isLocal field is noGt contained.";
        mapArgToVal.clear();
        mapArgToVal.insert(make_pair("pathCapFile", path));
        mapArgToVal.insert(make_pair("bundles", bundles));
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }

        mapArgToVal.clear();
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-end SUB_backup_tools_op_backup_0200";
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
HWTEST_F(ToolsOpBackupTest, SUB_backup_tools_op_backup_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-begin SUB_backup_tools_op_backup_0300";
    try {
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-info";
        map<string, vector<string>> mapArgToVal = PrepareBackupArguments();

        // 尝试匹配当前命令，成功后执行
        PerformBackupOperation(mapArgToVal);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-end SUB_backup_tools_op_backup_0300";
}

/**
 * @tc.number: SUB_backup_tools_op_backup_0400
 * @tc.name: SUB_backup_tools_op_backup_0400
 * @tc.desc: 测试InitPathCapFile分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpBackupTest, SUB_backup_tools_op_backup_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-begin SUB_backup_tools_op_backup_0400";
    try {
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-info";
        BackUpRegister();
        map<string, vector<string>> mapArgToVal;
        vector<string> path = {"/data/backup/tmp"};
        mapArgToVal.insert(make_pair("pathCapFile", path));
        vector<string> bundles = {"com.example.app2backup", "com.example.app3backup", "com.example.app4backup"};
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> local = {"false"};
        mapArgToVal.insert(make_pair("isLocal", local));

        // 尝试匹配当前命令，成功后执行
        PerformBackupOperation(mapArgToVal);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-end SUB_backup_tools_op_backup_0400";
}

/**
 * @tc.number: SUB_backup_tools_op_backup_0500
 * @tc.name: SUB_backup_tools_op_backup_0500
 * @tc.desc: 测试OnAllBundlesFinished分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpBackupTest, SUB_backup_tools_op_backup_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-begin SUB_backup_tools_op_backup_0500";
    try {
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-info";
        BackUpRegister();
        map<string, vector<string>> mapArgToVal = PrepareBackupArguments();

        // 尝试匹配当前命令，成功后执行
        PerformBackupOperation(mapArgToVal);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-end SUB_backup_tools_op_backup_0500";
}

/**
 * @tc.number: SUB_backup_tools_op_backup_0600
 * @tc.name: SUB_backup_tools_op_backup_0600
 * @tc.desc: 测试InitPathCapFile分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpBackupTest, SUB_backup_tools_op_backup_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-begin SUB_backup_tools_op_backup_0600";
    try {
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-info";
        BackUpRegister();
        rmdir((BConstants::BACKUP_TOOL_LINK_DIR).data());
        mkdir((BConstants::BACKUP_TOOL_LINK_DIR).data(), S_IRWXO);
        map<string, vector<string>> mapArgToVal = PrepareBackupArguments();

        // 尝试匹配当前命令，成功后执行
        PerformBackupOperation(mapArgToVal);
        rmdir((BConstants::BACKUP_TOOL_LINK_DIR).data());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-end SUB_backup_tools_op_backup_0600";
}

/**
 * @tc.number: SUB_backup_tools_op_backup_0700
 * @tc.name: SUB_backup_tools_op_backup_0700
 * @tc.desc: 测试InitPathCapFile分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpBackupTest, SUB_backup_tools_op_backup_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-begin SUB_backup_tools_op_backup_0700";
    try {
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-info";
        BackUpRegister();
        map<string, vector<string>> mapArgToVal = PrepareBackupArguments();

        // 尝试匹配当前命令，成功后执行
        PerformBackupOperation(mapArgToVal);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-end SUB_backup_tools_op_backup_0700";
}

/**
 * @tc.number: SUB_backup_tools_op_backup_0900
 * @tc.name: SUB_backup_tools_op_backup_0900
 * @tc.desc: 测试BackupToolDirSoftlinkToBackupDir分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpBackupTest, SUB_backup_tools_op_backup_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-begin SUB_backup_tools_op_backup_0900";
    try {
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-info";
        BackUpRegister();
        rmdir(BConstants::BACKUP_TOOL_LINK_DIR.data());
        map<string, vector<string>> mapArgToVal = PrepareBackupArguments();

        // 尝试匹配当前命令，成功后执行
        PerformBackupOperation(mapArgToVal);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-end SUB_backup_tools_op_backup_0900";
}

/**
 * @tc.number: SUB_backup_tools_op_backup_1000
 * @tc.name: SUB_backup_tools_op_backup_1000
 * @tc.desc: 测试InitPathCapFile分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpBackupTest, SUB_backup_tools_op_backup_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-begin SUB_backup_tools_op_backup_1000";
    try {
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-info";
        BackUpRegister();
        string cmdusermod = string("usermod -u 1000") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data();
        system(cmdusermod.c_str());
        map<string, vector<string>> mapArgToVal = PrepareBackupArguments();

        // 尝试匹配当前命令，成功后执行
        PerformBackupOperation(mapArgToVal);
        rmdir((BConstants::BACKUP_TOOL_RECEIVE_DIR).data());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-end SUB_backup_tools_op_backup_1000";
}

/**
 * @tc.name: tools_op_backup_SessionBranches_1100
 * @tc.desc: Cover session state and callback branches without depending on backup SA.
 * @tc.type: FUNC
 */
HWTEST_F(ToolsOpBackupTest, tools_op_backup_SessionBranches_1100, testing::ext::TestSize.Level1)
{
    auto ctx = make_shared<Session>();
    ctx->TryNotify();
    EXPECT_FALSE(ctx->ready_);

    ctx->bundleStatusMap_["bundle"].indexFile.insert("file");
    ctx->UpdateBundleReceivedFiles("bundle", "other");
    EXPECT_EQ(ctx->bundleStatusMap_.count("bundle"), 1);

    auto clearCtx = make_shared<Session>();
    clearCtx->bundleStatusMap_["bundle"].indexFile.insert("file");
    clearCtx->UpdateBundleReceivedFiles("bundle", "file");
    EXPECT_TRUE(clearCtx->bundleStatusMap_.empty());

    ctx->SetBundleFinishedCount(1);
    ctx->isAllBundelsFinished.store(true);
    ctx->TryNotify();
    EXPECT_FALSE(ctx->ready_);
    ctx->UpdateBundleFinishedCount();
    ctx->bundleStatusMap_.clear();
    ctx->TryNotify();
    EXPECT_TRUE(ctx->ready_);

    auto forceCtx = make_shared<Session>();
    forceCtx->TryNotify(true);
    EXPECT_TRUE(forceCtx->ready_);
}

/**
 * @tc.name: tools_op_backup_CallbackBranches_1200
 * @tc.desc: Cover success and failure callback branches.
 * @tc.type: FUNC
 */
HWTEST_F(ToolsOpBackupTest, tools_op_backup_CallbackBranches_1200, testing::ext::TestSize.Level1)
{
    auto started = make_shared<Session>();
    OnBundleStarted(started, 0, "bundle");
    EXPECT_FALSE(started->isAllBundelsFinished.load());

    started->SetBundleFinishedCount(1);
    OnBundleStarted(started, -1, "bundle");
    EXPECT_TRUE(started->isAllBundelsFinished.load());

    auto finished = make_shared<Session>();
    finished->SetBundleFinishedCount(1);
    OnBundleFinished(finished, 0, "bundle");
    OnAllBundlesFinished(finished, 0);
    EXPECT_TRUE(finished->isAllBundelsFinished.load());

    auto failed = make_shared<Session>();
    OnAllBundlesFinished(failed, -1);
    EXPECT_TRUE(failed->ready_);
    OnBackupServiceDied(failed);
    OnResultReport(failed, "bundle", "result");
    OnProcess(failed, "bundle", "process");
    EXPECT_FALSE(GenHelpMsg().empty());
}

/**
 * @tc.name: tools_op_backup_OnFileReadyBranches_1300
 * @tc.desc: Cover file-name validation and normal received-file handling.
 * @tc.type: FUNC
 */
HWTEST_F(ToolsOpBackupTest, tools_op_backup_OnFileReadyBranches_1300, testing::ext::TestSize.Level1)
{
    const string owner = "coverage.backup.bundle";
    const string bundleDir = string(BConstants::BACKUP_TOOL_RECEIVE_DIR) + owner;
    ASSERT_TRUE(ForceCreateDirectory(string(BConstants::BACKUP_TOOL_RECEIVE_DIR)));
    ForceRemoveDirectoryBMS(bundleDir);

    auto ctx = make_shared<Session>();
    BFileInfo invalidInfo(owner, "invalid/name", 0);
    UniqueFd invalidFd(open("/dev/null", O_RDONLY));
    EXPECT_THROW(OnFileReady(ctx, invalidInfo, move(invalidFd)), BError);

    BFileInfo normalInfo(owner, "data.tar", 0);
    UniqueFd normalFd(open("/dev/null", O_RDONLY));
    EXPECT_NO_THROW(OnFileReady(ctx, normalInfo, move(normalFd)));
    EXPECT_EQ(ctx->bundleStatusMap_.count(owner), 1);

    ForceRemoveDirectoryBMS(bundleDir);
}
} // namespace OHOS::FileManagement::Backup
