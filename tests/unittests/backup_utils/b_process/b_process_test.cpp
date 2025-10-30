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

#include <cstdlib>
#include <string>

#include <sys/stat.h>

#include <gtest/gtest.h>

#include "b_process/b_process.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BProcessTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

static bool DetectFatalLog(string_view output)
{
    GTEST_LOG_(INFO) << "DetectFatalLog " << output;
    if (output.find("empty archive") != string_view::npos) {
        return true;
    }
    return false;
}

/**
 * @tc.number: SUB_backup_tool_BProcess_0800
 * @tc.name: SUB_backup_tool_BProcess_0800
 * @tc.desc: 测试ExecuteCmd
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BProcessTest, SUB_backup_tool_BProcess_0800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BProcessTest-begin SUB_backup_tool_BProcess_0800";
    try {
        vector<string_view> argvCf = {
            "",
            "",
            "",
        };
        auto [bFatalError, ret] = BProcess::ExecuteCmd(argvCf, DetectFatalLog);
        EXPECT_EQ(ret, 2);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BProcessTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BProcessTest-end SUB_backup_tool_BProcess_0800";
}

/**
 * @tc.number: SUB_backup_tool_BProcess_0900
 * @tc.name: SUB_backup_tool_BProcess_0900
 * @tc.desc: 测试ExecuteCmd
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BProcessTest, SUB_backup_tool_BProcess_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BProcessTest-begin SUB_backup_tool_BProcess_0900";
    try {
        vector<string_view> argvCf = {
            "/system/bin/tar",
        };
        auto [bFatalError, ret] = BProcess::ExecuteCmd(argvCf, DetectFatalLog);
        EXPECT_EQ(ret, 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BProcessTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BProcessTest-end SUB_backup_tool_BProcess_0900";
}

/**
 * @tc.number: SUB_backup_tool_BProcess_1000
 * @tc.name: SUB_backup_tool_BProcess_1000
 * @tc.desc: 测试ExecuteCmd
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BProcessTest, SUB_backup_tool_BProcess_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BProcessTest-begin SUB_backup_tool_BProcess_1000";
    try {
        vector<string_view> argvCf = {
            "/system/bin/tar",
            "-cf",
        };
        auto [bFatalError, ret] = BProcess::ExecuteCmd(argvCf, DetectFatalLog);
        EXPECT_EQ(ret, 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BProcessTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BProcessTest-end SUB_backup_tool_BProcess_1000";
}
} // namespace OHOS::FileManagement::Backup
