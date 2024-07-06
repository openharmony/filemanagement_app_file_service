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
#include "tools_op_incremental_backup.h"
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
        EXPECT_NE(matchedOp, nullptr);
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
        EXPECT_NE(matchedOp, nullptr);
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
        EXPECT_NE(matchedOp, nullptr);
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
} // namespace OHOS::FileManagement::Backup