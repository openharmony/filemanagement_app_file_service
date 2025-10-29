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
#include <gtest/gtest.h>

#include "b_anony/b_anony.h"

namespace OHOS::FileManagement::Backup {
class BAnonyUtTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_b_anony_GetAnonyPath_0100
 * @tc.name: b_anony_GetAnonyPath_0100
 * @tc.desc: Test function of GetAnonyPath interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BAnonyUtTest, b_anony_GetAnonyPath_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BAnonyUtTest-begin b_anony_GetAnonyPath_0100";
    try {
        std::string path = "/";
        std::string result = "/";
        EXPECT_EQ(GetAnonyPath(path), result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BAnonyUtTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BAnonyUtTest-end b_anony_GetAnonyPath_0100";
}

/**
 * @tc.number: SUB_backup_b_anony_GetAnonyPath_0101
 * @tc.name: b_anony_GetAnonyPath_0101
 * @tc.desc: Test function of GetAnonyPath interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BAnonyUtTest, b_anony_GetAnonyPath_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BAnonyUtTest-begin b_anony_GetAnonyPath_0101";
    try {
        std::string path = "//";
        std::string result = "//";
        EXPECT_EQ(GetAnonyPath(path), result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BAnonyUtTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BAnonyUtTest-end b_anony_GetAnonyPath_0101";
}

/**
 * @tc.number: SUB_backup_b_anony_GetAnonyPath_0102
 * @tc.name: b_anony_GetAnonyPath_0102
 * @tc.desc: Test function of GetAnonyPath interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BAnonyUtTest, b_anony_GetAnonyPath_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BAnonyUtTest-begin b_anony_GetAnonyPath_0102";
    try {
        std::string path = "test.txt";
        std::string result = "t******t.txt";
        EXPECT_EQ(GetAnonyPath(path), result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BAnonyUtTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BAnonyUtTest-end b_anony_GetAnonyPath_0102";
}

/**
 * @tc.number: SUB_backup_b_anony_GetAnonyPath_0103
 * @tc.name: b_anony_GetAnonyPath_0103
 * @tc.desc: Test function of GetAnonyPath interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BAnonyUtTest, b_anony_GetAnonyPath_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BAnonyUtTest-begin b_anony_GetAnonyPath_0103";
    try {
        std::string path = "/*/*/shfkwam/xxf/x/xdf.db.xxx.xx";
        std::string result = "/******/******/s******m/x******f/******/x******f.db.xxx.xx";
        EXPECT_EQ(GetAnonyPath(path), result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BAnonyUtTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BAnonyUtTest-end b_anony_GetAnonyPath_0103";
}
}