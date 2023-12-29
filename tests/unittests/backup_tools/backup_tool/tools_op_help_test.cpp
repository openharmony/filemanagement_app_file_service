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

#include "tools_op.h"
#include "tools_op_help.h"
#include <gtest/gtest.h>

namespace OHOS::FileManagement::Backup {
using namespace std;
class ToolsOpHelpTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_tools_op_help_0101
 * @tc.name: SUB_backup_tools_op_help_0101
 * @tc.desc: 测试Execute分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpHelpTest, SUB_backup_tools_op_help_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpHelpTest-begin SUB_backup_tools_op_help_0101";
    try {
        GTEST_LOG_(INFO) << "ToolsOpHelpTest-info";
        HelpRegister();
        map<string, vector<string>> mapArgToVal;
        vector<string> path = {"/data/backup/tmp"};
        mapArgToVal.insert(make_pair("pathCapFile", path));
        vector<string> bundles = {"com.example.app2backup"};
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> local = {"false"};
        mapArgToVal.insert(make_pair("isLocal", local));
        vector<string_view> curOp;
        curOp.emplace_back("help");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        if (matchedOp != opeartions.end()) {
            matchedOp->Execute(mapArgToVal);
        }
        curOp.clear();
        curOp.emplace_back("backup");
        if (matchedOp != opeartions.end()) {
            matchedOp->Execute(mapArgToVal);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpHelpTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpHelpTest-end SUB_backup_tools_op_help_0101";
}
} // namespace OHOS::FileManagement::Backup