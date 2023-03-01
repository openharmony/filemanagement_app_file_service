/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <string>
#include <string_view>
#include <vector>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <file_ex.h>
#include <gtest/gtest.h>
#include <unique_fd.h>

#include "b_error/b_error.h"
#include "b_tarball/b_tarball_cmdline.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BTarballCmdlineTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_b_tarball_cmdline_0100
 * @tc.name: b_tarball_cmdline_0100
 * @tc.desc: 测试BTarballCmdline类构造函数是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BTarballCmdlineTest, b_tarball_cmdline_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTarballCmdlineTest-begin b_tarball_cmdline_0100";
    try {
        TestManager tm("b_tarball_cmdline_0100");
        string root = tm.GetRootDirCurTest();
        string_view tarballDir = root;
        string_view tarballName = "test.tar";

        BTarballCmdline tarballCmdline(tarballDir, tarballName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballCmdlineTest-an exception occurred by BTarballCmdline.";
    }
    GTEST_LOG_(INFO) << "BTarballCmdlineTest-end b_tarball_cmdline_0100";
}

/**
 * @tc.number: SUB_b_tarball_cmdline_0200
 * @tc.name: b_tarball_cmdline_0200
 * @tc.desc: 测试BTarballCmdline类Tar函数是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BTarballCmdlineTest, b_tarball_cmdline_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTarballCmdlineTest-begin b_tarball_cmdline_0200";
    try {
        // 预置文件和目录
        TestManager tm("b_tarball_cmdline_0200");
        string root = tm.GetRootDirCurTest();
        string_view tarballDir = root;
        string_view tarballName = "test.tar";
        string testDir = root + "/testdir";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string strFile = root + tarballName.data();
        UniqueFd fd(open(strFile.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        if (fd < 0) {
            GTEST_LOG_(INFO) << " invoked open failure, errno :" << errno;
            throw BError(errno);
        }

        string aFile = testDir + "/a.txt";
        string bFile = testDir + "/b.txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");
        vector<string_view> includes {testDir};
        vector<string_view> excludes {bFile};

        // 调用tar打包
        BTarballCmdline tarballCmdline(tarballDir, tarballName);
        tarballCmdline.Tar(root, includes, excludes);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballCmdlineTest-an exception occurred by BTarballCmdline.";
    }
    GTEST_LOG_(INFO) << "BTarballCmdlineTest-end b_tarball_cmdline_0200";
}

/**
 * @tc.number: SUB_b_tarball_cmdline_0300
 * @tc.name: b_tarball_cmdline_0300
 * @tc.desc: 测试BTarballCmdline类Untar函数是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BTarballCmdlineTest, b_tarball_cmdline_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTarballCmdlineTest-begin b_tarball_cmdline_0300";
    try {
        // 预置文件和目录
        TestManager tm("b_tarball_cmdline_0300");
        string root = tm.GetRootDirCurTest();
        string_view tarballDir = root;
        string_view tarballName = "test.tar";
        string testUntarDir = root + "/untardir";
        if (mkdir(testUntarDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        // 调用tar打包
        BTarballCmdline tarballCmdline(tarballDir, tarballName);
        tarballCmdline.Untar(testUntarDir);

    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballCmdlineTest-an exception occurred by BTarballCmdline.";
    }
    GTEST_LOG_(INFO) << "BTarballCmdlineTest-end b_tarball_cmdline_0300";
}
} // namespace OHOS::FileManagement::Backup