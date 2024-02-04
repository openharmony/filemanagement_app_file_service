/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <fcntl.h>
#include <file_ex.h>

#include "b_filesystem/b_file_hash.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BFileHashTest : public testing::Test {
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
 * @tc.number: SUB_backup_b_file_hash_HashWithSHA256_0100
 * @tc.name: b_file_hash_HashWithSHA256_0100
 * @tc.desc: Test function of HashWithSHA256 interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(BFileHashTest, b_file_hash_HashWithSHA256_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BFileHashTest-begin b_file_hash_HashWithSHA256_0100";
    try {
        TestManager tm(__func__);
        const auto [filePath, content] = GetTestFile(tm);

        auto [res, fileHash] = BFileHash::HashWithSHA256(filePath);

        EXPECT_EQ(res, 0);
    } catch (const exception &e) {
        GTEST_LOG_(INFO) << "BFileHashTest-an exception occurred by HashWithSHA256.";
        e.what();
    }
    GTEST_LOG_(INFO) << "BFileHashTest-end b_file_hash_HashWithSHA256_0100";
}
} // namespace OHOS::FileManagement::Backup