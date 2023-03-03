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
 * @tc.number: SUB_backup_tool_BProcess_0100
 * @tc.name: SUB_backup_tool_BProcess_0100
 * @tc.desc: 测试ExecuteCmd
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BProcessTest, SUB_backup_tool_BProcess_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BProcessTest-begin SUB_backup_tool_BProcess_0100";
    try {
        vector<string_view> argvCf = {
            "/system/bin/tar",
            "-cf",
            "/data/backup/",
        };
        auto [bFatalError, ret] = BProcess::ExecuteCmd(argvCf, DetectFatalLog);
        EXPECT_NE(ret, 0);

        vector<string_view> argvTvf = {
            "/system/bin/tar",
            "-tvf",
            "/data/backup/",
        };
        auto [bFatalErro1r, retTvf] = BProcess::ExecuteCmd(argvTvf, DetectFatalLog);
        EXPECT_NE(retTvf, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BProcessTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BProcessTest-end SUB_backup_tool_BProcess_0100";
}
} // namespace OHOS::FileManagement::Backup
