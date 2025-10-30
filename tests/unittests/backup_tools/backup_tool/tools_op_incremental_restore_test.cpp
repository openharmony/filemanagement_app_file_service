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
#include "tools_op_incremental_restore.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
const string BUNDLE_NAME = "com.example.app2backup/";
const string MANAGE_JSON = "manage.json";
const string FILE_NAME = "1.tar";
} // namespace

class ToolsOpIncrementalRestoreTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_tools_op_restore_0100
 * @tc.name: SUB_backup_tools_op_restore_0100
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_restore_0100";
    try {
        GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-info";
        map<string, vector<string>> mapArgToVal;
        string localCap = string(BConstants::GetSaBundleBackupToolDir(BConstants::DEFAULT_USER_ID).data()) + "/tmp";
        vector<string> path = { localCap.data() };
        mapArgToVal.insert(make_pair("pathCapFile", path));
        vector<string> bundles = { "com.example.app2backup" };
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> incrementalTime = { "0" };
        mapArgToVal.insert(make_pair("incrementalTime", incrementalTime));

        // 创建测试路径以及测试环境
        string cmdMkdir = string("mkdir -p ") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data() + BUNDLE_NAME;
        system(cmdMkdir.c_str());
        string cmdTool = string("mkdir -p ") + BConstants::GetSaBundleBackupToolDir(BConstants::DEFAULT_USER_ID).data();
        system(cmdTool.c_str());
        string touchTar = string("touch ") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data() + BUNDLE_NAME + FILE_NAME;
        system(touchTar.c_str());
        string touchManage = string("touch ") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data() + BUNDLE_NAME + MANAGE_JSON;
        system(touchManage.c_str());
        string touchTmp = string("touch ") + localCap;
        system(touchTmp.c_str());

        // 尝试匹配当前命令，成功后执行
        GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-restore";
        vector<string_view> curOp;
        curOp.emplace_back("incrementalrestore");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        if (matchedOp != opeartions.end()) {
            auto ret = matchedOp->Execute(mapArgToVal);
            EXPECT_EQ(ret, 0);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_restore_0100";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_0200
 * @tc.name: SUB_backup_tools_op_restore_0200
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_restore_0200";
    try {
        GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-The pathCapFile field is not contained.";
        map<string, vector<string>> mapArgToVal;
        vector<string> bundles = {"com.example.app2backup"};
        vector<string> path = {"/data/backup/tmp"};
        vector<string> incrementalTime = {"0"};
        mapArgToVal.insert(make_pair("bundles", bundles));
        mapArgToVal.insert(make_pair("incrementalTime", incrementalTime));

        vector<string_view> curOp;
        curOp.emplace_back("incrementalrestore");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        int ret = 0;
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }

        GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-The bundles field is not contained.";
        mapArgToVal.clear();
        mapArgToVal.insert(make_pair("pathCapFile", path));
        mapArgToVal.insert(make_pair("incrementalTime", incrementalTime));
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }

        GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-The incrementalTime field is not contained.";
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
        GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_restore_0200";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_0300
 * @tc.name: SUB_backup_tools_op_restore_0300
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_restore_0300";
    try {
        GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-The all required fields are contained.";
        map<string, vector<string>> mapArgToVal;

        vector<string> bundles = {"com.example.app2backup"};
        vector<string> path = {"/data/backup/tmp"};
        vector<string> incrementalTime = {"0"};
        mapArgToVal.insert(make_pair("pathCapFile", path));
        mapArgToVal.insert(make_pair("bundles", bundles));
        mapArgToVal.insert(make_pair("incrementalTime", incrementalTime));

        vector<string_view> curOp;
        curOp.emplace_back("incrementalrestore");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        int ret = 0;
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_EQ(ret, 0);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_restore_0300";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_0400
 * @tc.name: SUB_backup_tools_op_incremental_restore_0400
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_0400";
    try {
        auto ctx = make_shared<SessionRestore>();
        BFileInfo fileInfo;
        fileInfo.owner = "test";
        fileInfo.fileName = "/manage.json";
        fileInfo.sn = 1;
        UniqueFd fd(open("textFile", O_RDONLY));
        UniqueFd manifestFd(open("textManifest", O_RDONLY));
        OnFileReady(ctx, fileInfo, move(fd), move(manifestFd), 0);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_0400";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_0500
 * @tc.name: SUB_backup_tools_op_incremental_restore_0500
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_0500";
    try {
        auto ctx = make_shared<SessionRestore>();
        vector<BundleName> bundleNames;
        bundleNames.push_back("bundle1");
        bundleNames.push_back("bundle2");
        vector<string> times = {"100"};
        int32_t ret = InitRestoreSession(ctx, bundleNames, times);
        EXPECT_EQ(-EPERM, ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_0500";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_0501
 * @tc.name: SUB_backup_tools_op_incremental_restore_0501
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0501, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_0501";
    try {
        auto ctx = make_shared<SessionRestore>();
        vector<BundleName> bundleNames;
        bundleNames.push_back("bundle1");
        bundleNames.push_back("bundle2");
        vector<string> times;
        times.push_back("100");
        times.push_back("200");
        int32_t ret = InitRestoreSession(nullptr, bundleNames, times);
        EXPECT_EQ(-EPERM, ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_0501";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_0502
 * @tc.name: SUB_backup_tools_op_incremental_restore_0502
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0502, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_0502";
    try {
        auto ctx = make_shared<SessionRestore>();
        vector<BundleName> bundleNames;
        bundleNames.push_back("bundle1");
        bundleNames.push_back("bundle2");
        vector<string> times;
        times.push_back("10");
        times.push_back("20");
        int32_t ret = InitRestoreSession(ctx, bundleNames, times);
        EXPECT_EQ(0, ret);

        BFileInfo fileInfo;
        fileInfo.owner = "test";
        fileInfo.fileName = "manage.json";
        fileInfo.sn = 1;
        UniqueFd fd(open("textFile", O_RDONLY));
        UniqueFd manifestFd(open("textManifest", O_RDONLY));
        OnFileReady(ctx, fileInfo, move(fd), move(manifestFd), 0);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_0502";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_0600
 * @tc.name: SUB_backup_tools_op_incremental_restore_0600
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_0600";
    try {
        auto ctx = make_shared<SessionRestore>();
        ErrCode err = 0;
        BundleName name = "bundle";
        OnBundleStarted(ctx, err, name);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_0600";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_0601
 * @tc.name: SUB_backup_tools_op_incremental_restore_0601
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0601, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_0601";
    try {
        auto ctx = make_shared<SessionRestore>();
        ctx->cnt_ = 1;
        ErrCode err = -1;
        BundleName name = "bundle";
        OnBundleStarted(ctx, err, name);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_0601";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_0700
 * @tc.name: SUB_backup_tools_op_incremental_restore_0700
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_0700";
    try {
        auto ctx = make_shared<SessionRestore>();
        ctx->cnt_ = 1;
        ErrCode err = 0;
        BundleName name = "bundle";
        OnBundleFinished(ctx, err, name);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_0700";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_0701
 * @tc.name: SUB_backup_tools_op_incremental_restore_0701
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0701, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_0701";
    try {
        auto ctx = make_shared<SessionRestore>();
        ctx->cnt_ = 1;
        ErrCode err = -1;
        BundleName name = "bundle";
        OnBundleFinished(ctx, err, name);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_0701";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_0800
 * @tc.name: SUB_backup_tools_op_incremental_restore_0800
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_0800";
    try {
        auto ctx = make_shared<SessionRestore>();
        ErrCode err = 0;
        OnAllBundlesFinished(ctx, err);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_0800";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_0801
 * @tc.name: SUB_backup_tools_op_incremental_restore_0801
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0801, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_0801";
    try {
        auto ctx = make_shared<SessionRestore>();
        ErrCode err = -1;
        OnAllBundlesFinished(ctx, err);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_0801";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_0900
 * @tc.name: SUB_backup_tools_op_incremental_restore_0900
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_0900";
    try {
        auto ctx = make_shared<SessionRestore>();
        OnBackupServiceDied(ctx);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_0900";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_1000
 * @tc.name: SUB_backup_tools_op_incremental_restore_1000
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1000";
    try {
        shared_ptr<SessionRestore> restore = nullptr;
        RestoreApp(restore);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1000";
}
} // namespace OHOS::FileManagement::Backup