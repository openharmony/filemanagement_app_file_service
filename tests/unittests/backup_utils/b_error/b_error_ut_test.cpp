/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
class BErrorUtTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_b_error_construction_0500
 * @tc.name: b_error_construction_0500
 * @tc.desc: Test function of construction interface for Throw SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorUtTest, b_error_construction_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorUtTest-begin b_error_construction_0500";
    try {
        throw BError(BError::Codes::UTILS_INVAL_JSON_ENTITY);
        EXPECT_TRUE(false);
    } catch (const BError &e) {
        EXPECT_EQ(e.GetCode(), BError::BackupErrorCode::E_INVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BErrorUtTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BErrorUtTest-end b_error_construction_0500";
}

/**
 * @tc.number: SUB_backup_b_error_construction_0700
 * @tc.name: b_error_construction_0700
 * @tc.desc: Test function of construction interface for Throw SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorUtTest, b_error_construction_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorUtTest-begin b_error_construction_0700";
    try {
        throw BError(BError::Codes::UTILS_INVAL_FILE_HANDLE);
        EXPECT_TRUE(false);
    } catch (const BError &e) {
        EXPECT_EQ(e.GetCode(), BError::BackupErrorCode::E_INVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BErrorUtTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BErrorUtTest-end b_error_construction_0700";
}

/**
 * @tc.number: SUB_backup_b_error_construction_0900
 * @tc.name: b_error_construction_0900
 * @tc.desc: Test function of construction interface for Throw SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorUtTest, b_error_construction_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorUtTest-begin b_error_construction_0900";
    try {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG);
        EXPECT_TRUE(false);
    } catch (const BError &e) {
        EXPECT_EQ(e.GetCode(), BError::BackupErrorCode::E_UKERR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BErrorUtTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BErrorUtTest-end b_error_construction_0900";
}

/**
 * @tc.number: SUB_backup_b_error_construction_0200
 * @tc.name: b_error_construction_0200
 * @tc.desc: Test function of construction interface for Throw SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorUtTest, b_error_construction_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorUtTest-begin b_error_construction_0200";
    try {
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG);
        EXPECT_TRUE(false);
    } catch (const BError &e) {
        EXPECT_EQ(e.GetCode(), BError::BackupErrorCode::E_UKERR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BErrorUtTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BErrorUtTest-end b_error_construction_0200";
}

/**
 * @tc.number: SUB_backup_b_error_construction_0400
 * @tc.name: b_error_construction_0400
 * @tc.desc: Test function of construction interface for Throw SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorUtTest, b_error_construction_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorUtTest-begin b_error_construction_0400";
    try {
        throw BError(BError::Codes::UTILS_INTERRUPTED_PROCESS);
        EXPECT_TRUE(false);
    } catch (const BError &e) {
        EXPECT_EQ(e.GetCode(), BError::BackupErrorCode::E_UKERR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BErrorUtTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BErrorUtTest-end b_error_construction_0400";
}
}