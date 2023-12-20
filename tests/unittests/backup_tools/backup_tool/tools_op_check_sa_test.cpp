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

#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <sstream>

#include "tools_op.h"
#include "tools_op_check_sa.h"
#include "utils_mock_global_variable.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
class ToolsOpCheckSaTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_tools_op_check_sa_0100
 * @tc.name: SUB_backup_tools_op_check_sa_0100
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpCheckSaTest, SUB_backup_tools_op_check_sa_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpCheckSaTest-begin SUB_backup_tools_op_check_sa_0100";
    try {
        GTEST_LOG_(INFO) << "ToolsOpCheckSaTest-info";
        map<string, vector<string>> mapArgToVal;
        vector<string_view> curOp;
        curOp.emplace_back("check");
        curOp.emplace_back("sa");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        int ret = 0;
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_EQ(ret, 0);
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpCheckSaTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpCheckSaTest-end SUB_backup_tools_op_check_sa_0100";
}

/**
 * @tc.number: SUB_backup_tools_op_check_sa_0200
 * @tc.name: SUB_backup_tools_op_check_sa_0200
 * @tc.desc: 测试Exec
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ToolsOpCheckSaTest, SUB_backup_tools_op_check_sa_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpCheckSaTest-begin SUB_backup_tools_op_check_sa_0200";
    try {
        GTEST_LOG_(INFO) << "ToolsOpCheckSaTest-info";
        map<string, vector<string>> mapArgToVal;
        SetMockGetInstance(true);
        vector<string_view> curOp;
        curOp.emplace_back("check");
        curOp.emplace_back("sa");
        CheckSaRegister();
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        int ret = 0;
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_EQ(ret, 0);
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpCheckSaTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpCheckSaTest-end SUB_backup_tools_op_check_sa_0200";
}
} // namespace OHOS::FileManagement::Backup