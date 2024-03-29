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

#include <iostream>
#include <map>
#include <sstream>

#include <gtest/gtest.h>

#include "b_resources/b_constants.h"
#include "tools_op.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
const string FILE_NAME = "1.tar";
const string BUNDLE_NAME = "com.example.app2backup/";
const string MANAGE_JSON = "manage.json";
} // namespace

class ToolsOpRestoreAsyncTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_tools_op_restore_async_0100
 * @tc.name: SUB_backup_tools_op_restore_async_0100
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(ToolsOpRestoreAsyncTest, SUB_backup_tools_op_restore_async_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin SUB_backup_tools_op_restore_async_0100";
    try {
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-info";
        map<string, vector<string>> mapArgToVal;
        string localCap = string(BConstants::SA_BUNDLE_BACKUP_TMP_DIR.data()) + "/tmp";
        vector<string> path = {localCap.data()};
        mapArgToVal.insert(make_pair("pathCapFile", path));
        vector<string> bundles = {"com.example.app2backup"};
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> restoreType = {"true"};
        mapArgToVal.insert(make_pair("restoreType", restoreType));
        vector<string> userId = {"100"};
        mapArgToVal.insert(make_pair("userId", userId));

        // 创建测试路径以及测试环境
        string cmdMkdir = string("mkdir -p ") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data() + BUNDLE_NAME;
        system(cmdMkdir.c_str());
        string cmdTool = string("mkdir -p ") + BConstants::SA_BUNDLE_BACKUP_TMP_DIR.data();
        system(cmdTool.c_str());
        string touchTar = string("touch ") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data() + BUNDLE_NAME + FILE_NAME;
        system(touchTar.c_str());
        string touchManage = string("touch ") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data() + BUNDLE_NAME + MANAGE_JSON;
        system(touchManage.c_str());
        string touchTmp = string("touch ") + localCap;
        system(touchTmp.c_str());

        // 尝试匹配当前命令，成功后执行
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-restoreAsync";
        vector<string_view> curOp;
        curOp.emplace_back("restoreAsync");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        if (matchedOp != opeartions.end()) {
            auto ret = matchedOp->Execute(mapArgToVal);
            EXPECT_EQ(ret, 0);
        }

        mapArgToVal.clear();
        mapArgToVal.insert(make_pair("pathCapFile", path));
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> restoreTypeF = {"false"};
        mapArgToVal.insert(make_pair("restoreType", restoreTypeF));
        mapArgToVal.insert(make_pair("userId", userId));
        if (matchedOp != opeartions.end()) {
            auto ret = matchedOp->Execute(mapArgToVal);
            EXPECT_EQ(ret, 0);
        }

    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end SUB_backup_tools_op_restore_async_0100";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_0200
 * @tc.name: SUB_backup_tools_op_restore_async_0200
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(ToolsOpRestoreAsyncTest, SUB_backup_tools_op_restore_async_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin SUB_backup_tools_op_restore_async_0200";
    try {
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-The pathCapFile field is not contained.";
        map<string, vector<string>> mapArgToVal;
        vector<string> bundles = {"com.example.app2backup"};
        vector<string> path = {"/data/backup/tmp"};
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> restoreType = {"false"};
        mapArgToVal.insert(make_pair("restoreType", restoreType));
        vector<string> userId = {"100"};
        mapArgToVal.insert(make_pair("userId", userId));

        vector<string_view> curOp;
        curOp.emplace_back("restoreAsync");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        int ret = 0;
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-The bundles field is not contained.";
        mapArgToVal.clear();
        if (matchedOp != opeartions.end()) {
            mapArgToVal.insert(make_pair("pathCapFile", path));
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);

            mapArgToVal.clear();
            mapArgToVal.insert(make_pair("pathCapFile", path));
            mapArgToVal.insert(make_pair("bundles", bundles));
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);

            mapArgToVal.clear();
            mapArgToVal.insert(make_pair("pathCapFile", path));
            mapArgToVal.insert(make_pair("bundles", bundles));
            mapArgToVal.insert(make_pair("restoreType", restoreType));
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);

            mapArgToVal.clear();
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end SUB_backup_tools_op_restore_async_0200";
}
} // namespace OHOS::FileManagement::Backup