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

#include <cstdio>
#include <tuple>

#include <fcntl.h>

#include <file_ex.h>
#include <gtest/gtest.h>

#include "b_filesystem/b_file.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BFileTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @brief 创建测试文件
 *
 * @return tuple<bool, string, string> 创建结果、文件路径、文件内容
 */
static tuple<string, string> GetTestFile(const TestManager &tm)
{
    string path = tm.GetRootDirCurTest();
    string filePath = path + "temp.txt";
    string content = "backup test";
    if (bool contentCreate = SaveStringToFile(filePath, content, true); !contentCreate) {
        throw system_error(errno, system_category());
    }
    return {filePath, content};
}

/**
 * @tc.number: SUB_backup_b_file_ReadFile_0100
 * @tc.name: b_file_ReadFile_0100
 * @tc.desc: Test function of ReadFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileTest, b_file_ReadFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileTest-begin b_file_ReadFile_0100";
    try {
        TestManager tm(__func__);
        const auto [filePath, content] = GetTestFile(tm);
        BFile bf;
        unique_ptr<char[]> result = bf.ReadFile(UniqueFd(open(filePath.data(), O_RDWR)));
        string readContent(result.get());
        EXPECT_EQ(readContent.compare(content), 0);
    } catch (const exception &e) {
        GTEST_LOG_(INFO) << "BFileTest-an exception occurred by ReadFile.";
        e.what();
    }
    GTEST_LOG_(INFO) << "BFileTest-end b_file_ReadFile_0100";
}

/**
 * @tc.number: SUB_backup_b_file_SendFile_0100
 * @tc.name: b_file_SendFile_0100
 * @tc.desc: 测试SendFile接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileTest, b_file_SendFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileTest-begin b_file_SendFile_0100";
    try {
        TestManager tm(__func__);
        const auto [filePath, content] = GetTestFile(tm);
        TestManager tmInFile("b_file_GetFd_0100");
        string fileInPath = tmInFile.GetRootDirCurTest().append("1.tar");
        BFile::SendFile(UniqueFd(open(filePath.data(), O_RDWR)),
                        UniqueFd(open(fileInPath.data(), O_RDWR | O_CREAT, S_IRWXU)));
    } catch (const exception &e) {
        GTEST_LOG_(INFO) << "BFileTest-an exception occurred by SendFile.";
        e.what();
    }
    GTEST_LOG_(INFO) << "BFileTest-end b_file_SendFile_0100";
}

/**
 * @tc.number: SUB_backup_b_file_CopyFile_0100
 * @tc.name: b_file_CopyFile_0100
 * @tc.desc: 测试CopyFile接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileTest, b_file_CopyFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileTest-begin b_file_CopyFile_0100";
    try {
        TestManager tm(__func__);
        const auto [filePath, content] = GetTestFile(tm);
        TestManager tmInFile("b_file_GetFd_0200");
        string fileInPath = tmInFile.GetRootDirCurTest().append("1.txt");
        auto ret = BFile::CopyFile(filePath, fileInPath);
        EXPECT_TRUE(ret);
        GTEST_LOG_(INFO) << "BFileTest-CopyFile Branches";
        ret = BFile::CopyFile(filePath, filePath);
        EXPECT_TRUE(ret);
    } catch (const exception &e) {
        GTEST_LOG_(INFO) << "BFileTest-an exception occurred by CopyFile.";
        e.what();
    }
    GTEST_LOG_(INFO) << "BFileTest-end b_file_CopyFile_0100";
}
} // namespace OHOS::FileManagement::Backup