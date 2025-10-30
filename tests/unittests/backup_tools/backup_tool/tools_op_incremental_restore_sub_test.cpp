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
 * @tc.number: SUB_backup_tools_op_incremental_restore_1001
 * @tc.name: SUB_backup_tools_op_incremental_restore_1001
 * @tc.desc: 测试RestoreApp session空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1001";
    try {
        shared_ptr<SessionRestore> restore = make_shared<SessionRestore>();
        restore->session_ = nullptr;
        RestoreApp(restore);
        EXPECT_EQ(restore->session_, nullptr);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1001";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_1002
 * @tc.name: SUB_backup_tools_op_incremental_restore_1002
 * @tc.desc: 测试RestoreApp 正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1002";
    try {
        shared_ptr<SessionRestore> restore = make_shared<SessionRestore>();
        restore->session_ = {};
        BIncrementalData data("text", 1);
        restore->lastIncrementalData = {data};
        RestoreApp(restore);
        EXPECT_TRUE(restore);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1002";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_1003
 * @tc.name: SUB_backup_tools_op_incremental_restore_1003
 * @tc.desc: 测试InitRestoreSession 正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1003";
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

        RestoreApp(ctx);
        EXPECT_TRUE(ctx);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1003";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_1100
 * @tc.name: SUB_backup_tools_op_incremental_restore_1100
 * @tc.desc: 测试Init pathCapFile参数为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1100";
    try {
        string pathCapFile = "";
        vector<string> bundleNames = {"com.example.app2backup/"};
        bool depMode = true;
        vector<string> times = {"10"};
        int32_t ret = Init(pathCapFile, bundleNames, depMode, times);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1100";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_1101
 * @tc.name: SUB_backup_tools_op_incremental_restore_1101
 * @tc.desc: 测试Init 正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1101";
    try {
        string pathCapFile = "/data/backup";
        vector<string> bundleNames = {"com.example.app2backup/"};
        bool depMode = true;
        vector<string> times = {"1"};
        int32_t ret = Init(pathCapFile, bundleNames, depMode, times);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1101";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_1102
 * @tc.name: SUB_backup_tools_op_incremental_restore_1102
 * @tc.desc: 测试Init 异常分支1
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1102";
    try {
        string pathCapFile = "";
        vector<string> bundleNames = {"com.example.app2backup/"};
        bool depMode = false;
        vector<string> times = {"10"};
        int32_t ret = Init(pathCapFile, bundleNames, depMode, times);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1102";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_1200
 * @tc.name: SUB_backup_tools_op_incremental_restore_1200
 * @tc.desc: 测试mapArgToVal
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1200";
    try {
        map<string, vector<string>> mapArgToVal;
        mapArgToVal["depMode"] = {"false"};
        g_exec(mapArgToVal);
        EXPECT_EQ(mapArgToVal["depMode"][0], "false");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1200";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_1201
 * @tc.name: SUB_backup_tools_op_incremental_restore_1201
 * @tc.desc: 测试g_exec 不同参数1
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1201, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1201";
    try {
        map<string, vector<string>> mapArgToVal = {
            {"pathCapFile", {"path"}},
            {"bundles", {"bundle1"}},
            {"incrementalTime", {"time"}}
        };
        int ret = g_exec(mapArgToVal);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1201";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_1202
 * @tc.name: SUB_backup_tools_op_incremental_restore_1202
 * @tc.desc: 测试g_exec 不同参数2
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1202, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1202";
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
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1202";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_1203
 * @tc.name: SUB_backup_tools_op_incremental_restore_1203
 * @tc.desc: 测试g_exec 不同参数3
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1203, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1203";
    try {
        map<string, vector<string>> mapArgToVal = {
            {"pathCapFile", {"path"}},
            {"bundles", {"bundle1"}},
            {"incrementalTime", {"time"}},
            {"depMode", {"true"}}
        };
        int ret = g_exec(mapArgToVal);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1203";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_1204
 * @tc.name: SUB_backup_tools_op_incremental_restore_1204
 * @tc.desc: 测试g_exec 不同参数4
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1204, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1204";
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
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1204";
}

/**
 * @tc.number: SUB_backup_tools_op_incremental_restore_1205
 * @tc.name: SUB_backup_tools_op_incremental_restore_1205
 * @tc.desc: 测试g_exec 不同参数5
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpIncrementalRestoreTest, SUB_backup_tools_op_incremental_restore_1205, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-begin SUB_backup_tools_op_incremental_restore_1205";
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
    GTEST_LOG_(INFO) << "ToolsOpIncrementalRestoreTest-end SUB_backup_tools_op_incremental_restore_1205";
}
} // namespace OHOS::FileManagement::Backup