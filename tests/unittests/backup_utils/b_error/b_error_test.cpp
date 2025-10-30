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
HWTEST_F(BErrorTest, b_error_construction_0300, testing::ext::TestSize.Level1)
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
 * @tc.number: SUB_backup_b_error_GetCode_0100
 * @tc.name: b_error_GetCode_0100
 * @tc.desc: Test function of GetCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetCode_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetCode_0100";
    BError be(BError::Codes::OK);
    int result = be.GetCode();
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetCode_0100";
}

/**
 * @tc.number: SUB_backup_b_error_GetCode_0200
 * @tc.name: b_error_GetCode_0200
 * @tc.desc: Test function of GetCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetCode_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetCode_0200";
    BError be(BError::Codes::UTILS_INVAL_JSON_ENTITY);
    int ret = be.GetCode();
    EXPECT_EQ(ret, BError::BackupErrorCode::E_INVAL);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetCode_0200";
}

/**
 * @tc.number: SUB_backup_b_error_GetCode_0300
 * @tc.name: b_error_GetCode_0300
 * @tc.desc: Test function of GetCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetCode_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetCode_0300";
    BError be(BError::Codes::UTILS_INVAL_FILE_HANDLE);
    int ret = be.GetCode();
    EXPECT_EQ(ret, BError::BackupErrorCode::E_INVAL);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetCode_0300";
}

/**
 * @tc.number: SUB_backup_b_error_GetCode_0400
 * @tc.name: b_error_GetCode_0400
 * @tc.desc: Test function of GetCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetCode_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetCode_0400";
    BError be(BError::Codes::UTILS_INVAL_TARBALL_ARG);
    int ret = be.GetCode();
    EXPECT_EQ(ret, BError::BackupErrorCode::E_UKERR);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetCode_0400";
}

/**
 * @tc.number: SUB_backup_b_error_GetCode_0500
 * @tc.name: b_error_GetCode_0500
 * @tc.desc: Test function of GetCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetCode_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetCode_0500";
    BError be(BError::Codes::UTILS_INVAL_PROCESS_ARG);
    int ret = be.GetCode();
    EXPECT_EQ(ret, BError::BackupErrorCode::E_UKERR);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetCode_0500";
}

/**
 * @tc.number: SUB_backup_b_error_GetCode_0600
 * @tc.name: b_error_GetCode_0600
 * @tc.desc: Test function of GetCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetCode_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetCode_0600";
    BError be(BError::Codes::UTILS_INTERRUPTED_PROCESS);
    int ret = be.GetCode();
    EXPECT_EQ(ret, BError::BackupErrorCode::E_UKERR);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetCode_0600";
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
HWTEST_F(BErrorTest, b_error_GetRawCode_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetRawCode_0100";
    BError be(BError::Codes::OK);
    BError::Codes result = be.GetRawCode();
    EXPECT_EQ(result, BError::Codes::OK);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetRawCode_0100";
}

/**
 * @tc.number: SUB_backup_b_error_GetRawCode_0200
 * @tc.name: b_error_GetRawCode_0200
 * @tc.desc: Test function of GetRawCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetRawCode_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetRawCode_0200";
    BError be(BError::Codes::UTILS_INVAL_JSON_ENTITY);
    BError::Codes result = be.GetRawCode();
    EXPECT_EQ(result, BError::Codes::UTILS_INVAL_JSON_ENTITY);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetRawCode_0200";
}

/**
 * @tc.number: SUB_backup_b_error_GetRawCode_0300
 * @tc.name: b_error_GetRawCode_0300
 * @tc.desc: Test function of GetRawCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetRawCode_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetRawCode_0300";
    BError be(BError::Codes::UTILS_INVAL_FILE_HANDLE);
    BError::Codes result = be.GetRawCode();
    EXPECT_EQ(result, BError::Codes::UTILS_INVAL_FILE_HANDLE);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetRawCode_0300";
}

/**
 * @tc.number: SUB_backup_b_error_GetRawCode_0400
 * @tc.name: b_error_GetRawCode_0400
 * @tc.desc: Test function of GetRawCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetRawCode_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetRawCode_0400";
    BError be(BError::Codes::UTILS_INVAL_TARBALL_ARG);
    BError::Codes result = be.GetRawCode();
    EXPECT_EQ(result, BError::Codes::UTILS_INVAL_TARBALL_ARG);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetRawCode_0400";
}

/**
 * @tc.number: SUB_backup_b_error_GetRawCode_0500
 * @tc.name: b_error_GetRawCode_0500
 * @tc.desc: Test function of GetRawCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetRawCode_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetRawCode_0500";
    BError be(BError::Codes::UTILS_INVAL_PROCESS_ARG);
    BError::Codes result = be.GetRawCode();
    EXPECT_EQ(result, BError::Codes::UTILS_INVAL_PROCESS_ARG);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetRawCode_0500";
}

/**
 * @tc.number: SUB_backup_b_error_GetRawCode_0600
 * @tc.name: b_error_GetRawCode_0600
 * @tc.desc: Test function of GetRawCode interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetRawCode_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetRawCode_0600";
    BError be(BError::Codes::UTILS_INTERRUPTED_PROCESS);
    BError::Codes result = be.GetRawCode();
    EXPECT_EQ(result, BError::Codes::UTILS_INTERRUPTED_PROCESS);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetRawCode_0600";
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
HWTEST_F(BErrorTest, b_error_what_0100, testing::ext::TestSize.Level1)
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
HWTEST_F(BErrorTest, b_error_bool_0100, testing::ext::TestSize.Level1)
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
HWTEST_F(BErrorTest, b_error_int_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_int_0100";
    int result = BError();
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_int_0100";
}

/**
 * @tc.number: SUB_backup_b_error_GetCodeByErrno_0100
 * @tc.name: b_error_GetCodeByErrno_0100
 * @tc.desc: Test function of int interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetCodeByErrno_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetCodeByErrno_0100";
    int32_t errnoSys = 0;
    int result = BError::GetCodeByErrno(errnoSys);
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetCodeByErrno_0100";
}

/**
 * @tc.number: SUB_backup_b_error_GetCodeByErrno_0200
 * @tc.name: b_error_GetCodeByErrno_0200
 * @tc.desc: Test function of int interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetCodeByErrno_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetCodeByErrno_0200";
    int32_t errnoSys = EPERM;
    int result = BError::GetCodeByErrno(errnoSys);
    EXPECT_EQ(result, BError::BackupErrorCode::E_PERM);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetCodeByErrno_0200";
}

/**
 * @tc.number: SUB_backup_b_error_GetCodeByErrno_0300
 * @tc.name: b_error_GetCodeByErrno_0300
 * @tc.desc: Test function of int interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetCodeByErrno_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetCodeByErrno_0300";
    int32_t errnoSys = -EPERM;
    int result = BError::GetCodeByErrno(errnoSys);
    EXPECT_EQ(result, BError::BackupErrorCode::E_UKERR);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetCodeByErrno_0300";
}

/**
 * @tc.number: SUB_backup_b_error_GetBackupCodeByErrno_0100
 * @tc.name: b_error_GetBackupCodeByErrno_0100
 * @tc.desc: Test function of int interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetBackupCodeByErrno_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetBackupCodeByErrno_0100";
    int err = ERR_OK;
    int result = BError::GetBackupCodeByErrno(err);
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetBackupCodeByErrno_0100";
}

/**
 * @tc.number: SUB_backup_b_error_GetBackupCodeByErrno_0200
 * @tc.name: b_error_GetBackupCodeByErrno_0200
 * @tc.desc: Test function of int interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetBackupCodeByErrno_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetBackupCodeByErrno_0200";
    int err = BError(BError::Codes::SA_INVAL_ARG).GetCode();
    int result = BError::GetBackupCodeByErrno(err);
    EXPECT_EQ(result, BError::BackupErrorCode::E_INVAL);
    err = BError(BError::Codes::SA_BROKEN_IPC).GetCode();
    result = BError::GetBackupCodeByErrno(err);
    EXPECT_EQ(result, BError::BackupErrorCode::E_IPCSS);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetBackupCodeByErrno_0200";
}

/**
 * @tc.number: SUB_backup_b_error_GetBackupCodeByErrno_0300
 * @tc.name: b_error_GetBackupCodeByErrno_0300
 * @tc.desc: Test function of int interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetBackupCodeByErrno_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetBackupCodeByErrno_0300";
    int err = -EPERM;
    int result = BError::GetBackupCodeByErrno(err);
    EXPECT_EQ(result, BError::BackupErrorCode::E_UKERR);
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetBackupCodeByErrno_0300";
}

/**
 * @tc.number: SUB_backup_b_error_GetBackupMsgByErrno_0100
 * @tc.name: b_error_GetBackupMsgByErrno_0100
 * @tc.desc: Test function of int interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetBackupMsgByErrno_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetBackupMsgByErrno_0100";
    int err = BError(BError::BackupErrorCode::E_PERM).GetCode();
    std::string result = BError::GetBackupMsgByErrno(err);
    EXPECT_EQ(result, BError::backupErrorMsgTable_.at(err));
    err = BError(BError::BackupErrorCode::E_NOMEM).GetCode();
    result = BError::GetBackupMsgByErrno(err);
    EXPECT_EQ(result, BError::backupErrorMsgTable_.at(err));
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetBackupMsgByErrno_0100";
}

/**
 * @tc.number: SUB_backup_b_error_GetBackupMsgByErrno_0200
 * @tc.name: b_error_GetBackupMsgByErrno_0200
 * @tc.desc: Test function of int interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BErrorTest, b_error_GetBackupMsgByErrno_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BErrorTest-begin b_error_GetBackupMsgByErrno_0200";
    int err = -EPERM;
    std::string result = BError::GetBackupMsgByErrno(err);
    EXPECT_EQ(result, BError::backupErrorMsgTable_.at(BError::BackupErrorCode::E_UKERR));
    GTEST_LOG_(INFO) << "BErrorTest-end b_error_GetBackupMsgByErrno_0200";
}
} // namespace OHOS::FileManagement::Backup