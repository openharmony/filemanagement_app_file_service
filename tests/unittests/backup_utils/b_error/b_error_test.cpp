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

#include <cstdio>
#include <gtest/gtest.h>

#include "b_error/b_error.h"

namespace OHOS::FileManagement::Backup {
class BErrorTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_b_error_construction_0100
 * @tc.name: b_error_construction_0100
 * @tc.desc: Test function of construction interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_construction_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_construction_0100";
    try {
        BError be(BError::Codes::OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BErrorTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_construction_0100";
}

/**
 * @tc.number: SUB_backup_b_error_construction_0300
 * @tc.name: b_error_construction_0300
 * @tc.desc: Test function of construction interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_construction_0300, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_construction_0300";
    try {
        std::string_view extraMsg;
        BError be(BError::Codes::OK, extraMsg);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BErrorTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_construction_0300";
}

/**
 * @tc.number: SUB_backup_b_error_construction_0500
 * @tc.name: b_error_construction_0500
 * @tc.desc: Test function of construction interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_construction_0500, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_construction_0500";
    try {
        throw BError(BError::Codes::UTILS_INVAL_JSON_ENTITY);
        EXPECT_TRUE(false);
    } catch (const BError &e) {
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BErrorTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_construction_0500";
}

/**
 * @tc.number: SUB_backup_b_error_GetCode_0100
 * @tc.name: b_error_GetCode_0100
 * @tc.desc: Test function of GetCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetCode_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetCode_0100";
    BError be(BError::Codes::OK);
    int result = be.GetCode();
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetCode_0100";
}

/**
 * @tc.number: SUB_backup_b_error_GetRawCode_0100
 * @tc.name: b_error_GetRawCode_0100
 * @tc.desc: Test function of GetRawCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetRawCode_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetRawCode_0100";
    BError be(BError::Codes::OK);
    BError::Codes result = be.GetRawCode();
    EXPECT_EQ(result, BError::Codes::OK);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetRawCode_0100";
}

/**
 * @tc.number: SUB_backup_b_error_what_0100
 * @tc.name: b_error_what_0100
 * @tc.desc: Test function of what interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_what_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_what_0100";
    BError be(BError::Codes::OK);
    auto result = be.what();
    EXPECT_NE(result, nullptr);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_what_0100";
}

/**
 * @tc.number: SUB_backup_b_error_bool_0100
 * @tc.name: b_error_bool_0100
 * @tc.desc: Test function of bool interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_bool_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_bool_0100";
    bool result = BError();
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_bool_0100";
}

/**
 * @tc.number: SUB_backup_b_error_int_0100
 * @tc.name: b_error_int_0100
 * @tc.desc: Test function of int interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_int_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_int_0100";
    int result = BError();
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_int_0100";
}
} // namespace OHOS::FileManagement::Backup