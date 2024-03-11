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

#include <cassert>
#include <fcntl.h>

#include <gtest/gtest.h>
#include <singleton.h>
#include <string>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "access_token_error.h"
#include "accesstoken_kit.h"
#include "common_func.h"
#include "error_code.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "oh_file_share.h"
#include "parameter.h"
#include "sandbox_helper.h"
#include "uri.h"

#include "nativetoken_kit.h"
#include "permission_def.h"
#include "permission_state_full.h"
#include "token_setproc.h"

using namespace testing::ext;
using namespace OHOS::AppFileService;
using namespace OHOS::Security::AccessToken;
namespace {
const std::string FILE_MANAGER_FULL_MOUNT_ENABLE_PARAMETER_STR = "const.filemanager.full_mount.enable";
const unsigned int SET_ZERO = 0;
} // namespace

namespace OHOS::AppFileService::ModuleFileSharePermission {

void GrantNativePermission()
{
    const char **perms = new const char *[1];
    perms[0] = "ohos.permission.FILE_ACCESS_PERSIST";
    TokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "app_file_service",
        .aplStr = "system_core",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
}

static bool CheckFileManagerFullMountEnable()
{
    char value[] = "false";
    int retSystem = GetParameter(FILE_MANAGER_FULL_MOUNT_ENABLE_PARAMETER_STR.c_str(), "false", value, sizeof(value));
    if (retSystem > 0 && !std::strcmp(value, "true")) {
        return true;
    }
    GTEST_LOG_(INFO) << "Not supporting all mounts";
    return false;
}

class NDKFileSharePermissionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void) {};
    void SetUp() {};
    void TearDown() {};
};

void NDKFileSharePermissionTest::SetUpTestCase()
{
    GrantNativePermission();
}

/**
 * @tc.name: OH_FileShare_PersistPermission_test_001
 * @tc.desc: Test function of OH_FileShare_PersistPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_PersistPermission_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_001 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/el2/base/PersistPermission.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = sizeof(policyUriChar) - 1,
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_001 end";
}

/**
 * @tc.name: OH_FileShare_PersistPermission_test_002
 * @tc.desc: Test function of OH_FileShare_PersistPermission() interface for invalid path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_PersistPermission_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_002 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/fileShare02.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = sizeof(policyUriChar) - 1,
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_EPERM);
        if (result != nullptr) {
            EXPECT_EQ(INVALID_PATH, result[0].code);
        } else {
            EXPECT_FALSE(true);
        }
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_002 end";
}

/**
 * @tc.name: OH_FileShare_PersistPermission_test_003
 * @tc.desc: Test function of OH_FileShare_PersistPermission() interface for err params.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_PersistPermission_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_003 start";
    FileShare_PolicyInfo *policies = nullptr;
    unsigned int policiesNum = SET_ZERO;
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, E_PARAMS);
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_003 end";
}

/**
 * @tc.name: OH_FileShare_PersistPermission_test_004
 * @tc.desc: Test function of OH_FileShare_PersistPermission() interface for eperm.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_PersistPermission_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_004 start";
    char policyUriCharA[] = "file://com.example.filesharea/data/storage/el2/base/PersistPermission.txt";
    char policyUriCharB[] = "file://com.example.filesharea/data/storage/fileShare04.txt";
    FileShare_PolicyInfo policyA = {.uri = policyUriCharA,
                                    .length = sizeof(policyUriCharA) - 1,
                                    .operationMode =
                                        FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policyB = {.uri = policyUriCharB,
                                    .length = sizeof(policyUriCharB) - 1,
                                    .operationMode =
                                        FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policyA, policyB};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_EPERM);
        if (result != nullptr) {
            EXPECT_EQ(INVALID_PATH, result[0].code);
        } else {
            EXPECT_FALSE(true);
        }
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_004 end";
}

/**
 * @tc.name: OH_FileShare_RevokePermission_test_001
 * @tc.desc: Test function of OH_FileShare_RevokePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_RevokePermission_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_RevokePermission_test_001 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/el2/base/RevokePermission.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = sizeof(policyUriChar) - 1,
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode retPersist = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(retPersist, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(retPersist, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    FileManagement_ErrCode ret = OH_FileShare_RevokePermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_RevokePermission_test_001 end";
}

/**
 * @tc.name: OH_FileShare_RevokePermission_test_002
 * @tc.desc: Test function of OH_FileShare_RevokePermission() interface for invalid path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_RevokePermission_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_RevokePermission_test_002 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/fileShare02.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = sizeof(policyUriChar) - 1,
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_RevokePermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_EPERM);
        if (result != nullptr) {
            EXPECT_EQ(INVALID_PATH, result[0].code);
        } else {
            EXPECT_FALSE(true);
        }
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_RevokePermission_test_002 end";
}

/**
 * @tc.name: OH_FileShare_RevokePermission_test_003
 * @tc.desc: Test function of OH_FileShare_RevokePermission() interface for err params.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_RevokePermission_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_RevokePermission_test_003 start";
    FileShare_PolicyInfo *policies = nullptr;
    unsigned int policiesNum = SET_ZERO;
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_RevokePermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, E_PARAMS);
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_RevokePermission_test_003 end";
}

/**
 * @tc.name: OH_FileShare_RevokePermission_test_004
 * @tc.desc: Test function of OH_FileShare_RevokePermission() interface for eperm.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_RevokePermission_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_RevokePermission_test_004 start";
    char policyUriCharA[] = "file://com.example.filesharea/data/storage/el2/base/RevokePermission.txt";
    FileShare_PolicyInfo policyA = {.uri = policyUriCharA,
                                    .length = sizeof(policyUriCharA) - 1,
                                    .operationMode =
                                        FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policiesA[] = {policyA};
    unsigned int policiesNumA = sizeof(policiesA) / sizeof(policiesA[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode retPersist = OH_FileShare_PersistPermission(policiesA, policiesNumA, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(retPersist, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(retPersist, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    char policyUriCharB[] = "file://com.example.filesharea/data/storage/fileShare04.txt";
    FileShare_PolicyInfo policyB = {.uri = policyUriCharB,
                                    .length = sizeof(policyUriCharB) - 1,
                                    .operationMode =
                                        FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policyA, policyB};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileManagement_ErrCode ret = OH_FileShare_RevokePermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_EPERM);
        if (result != nullptr) {
            EXPECT_EQ(INVALID_PATH, result[0].code);
        } else {
            EXPECT_FALSE(true);
        }
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_RevokePermission_test_004 end";
}

/**
 * @tc.name: OH_FileShare_ActivatePermission_test_001
 * @tc.desc: Test function of OH_FileShare_ActivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_ActivatePermission_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_ActivatePermission_test_001 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/el2/base/ActivatePermission.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = sizeof(policyUriChar) - 1,
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode retPersist = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(retPersist, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(retPersist, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    FileManagement_ErrCode ret = OH_FileShare_ActivatePermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_ActivatePermission_test_001 end";
}

/**
 * @tc.name: OH_FileShare_ActivatePermission_test_002
 * @tc.desc: Test function of OH_FileShare_ActivatePermission() interface for invalid path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_ActivatePermission_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_ActivatePermission_test_002 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/fileShare02.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = sizeof(policyUriChar) - 1,
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_ActivatePermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_EPERM);
        if (result != nullptr) {
            EXPECT_EQ(INVALID_PATH, result[0].code);
        } else {
            EXPECT_FALSE(true);
        }
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_ActivatePermission_test_002 end";
}

/**
 * @tc.name: OH_FileShare_ActivatePermission_test_003
 * @tc.desc: Test function of OH_FileShare_ActivatePermission() interface for err params.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_ActivatePermission_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_ActivatePermission_test_003 start";
    FileShare_PolicyInfo *policies = nullptr;
    unsigned int policiesNum = SET_ZERO;
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_ActivatePermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, E_PARAMS);
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_ActivatePermission_test_003 end";
}

/**
 * @tc.name: OH_FileShare_ActivatePermission_test_004
 * @tc.desc: Test function of OH_FileShare_ActivatePermission() interface for eperm.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_ActivatePermission_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_ActivatePermission_test_004 start";
    char policyUriCharA[] = "file://com.example.filesharea/data/storage/el2/base/ActivatePermission.txt";
    FileShare_PolicyInfo policyA = {.uri = policyUriCharA,
                                    .length = sizeof(policyUriCharA) - 1,
                                    .operationMode =
                                        FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policiesA[] = {policyA};
    unsigned int policiesNumA = sizeof(policiesA) / sizeof(policiesA[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode retPersist = OH_FileShare_PersistPermission(policiesA, policiesNumA, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(retPersist, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(retPersist, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    char policyUriCharB[] = "file://com.example.filesharea/data/storage/fileShare04.txt";
    FileShare_PolicyInfo policyB = {.uri = policyUriCharB,
                                    .length = sizeof(policyUriCharB) - 1,
                                    .operationMode =
                                        FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policyA, policyB};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileManagement_ErrCode ret = OH_FileShare_ActivatePermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_EPERM);
        if (result != nullptr) {
            EXPECT_EQ(INVALID_PATH, result[0].code);
        } else {
            EXPECT_FALSE(true);
        }
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_ActivatePermission_test_004 end";
}

/**
 * @tc.name: OH_FileShare_DeactivatePermission_test_001
 * @tc.desc: Test function of OH_FileShare_DeactivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_DeactivatePermission_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_DeactivatePermission_test_001 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/el2/base/DeactivatePermission.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = sizeof(policyUriChar) - 1,
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode retPersist = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(retPersist, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(retPersist, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    FileManagement_ErrCode retActivate = OH_FileShare_ActivatePermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(retActivate, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(retActivate, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    FileManagement_ErrCode ret = OH_FileShare_DeactivatePermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_DeactivatePermission_test_001 end";
}

/**
 * @tc.name: OH_FileShare_DeactivatePermission_test_002
 * @tc.desc: Test function of OH_FileShare_DeactivatePermission() interface for invalid path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_DeactivatePermission_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_DeactivatePermission_test_002 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/fileShare02.txt";

    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = sizeof(policyUriChar) - 1,
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_DeactivatePermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_EPERM);
        if (result != nullptr) {
            EXPECT_EQ(INVALID_PATH, result[0].code);
        } else {
            EXPECT_FALSE(true);
        }
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_DeactivatePermission_test_002 end";
}

/**
 * @tc.name: OH_FileShare_DeactivatePermission_test_003
 * @tc.desc: Test function of OH_FileShare_DeactivatePermission() interface for err params.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_DeactivatePermission_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_DeactivatePermission_test_003 start";
    FileShare_PolicyInfo *policies = nullptr;
    unsigned int policiesNum = SET_ZERO;
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_DeactivatePermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, E_PARAMS);
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_DeactivatePermission_test_003 end";
}

/**
 * @tc.name: OH_FileShare_DeactivatePermission_test_004
 * @tc.desc: Test function of OH_FileShare_DeactivatePermission() interface for eperm.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_DeactivatePermission_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_DeactivatePermission_test_004 start";
    char policyUriCharA[] = "file://com.example.filesharea/data/storage/el2/base/DeactivatePermission.txt";
    FileShare_PolicyInfo policyA = {.uri = policyUriCharA,
                                    .length = sizeof(policyUriCharA) - 1,
                                    .operationMode =
                                        FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policiesA[] = {policyA};
    unsigned int policiesNumA = sizeof(policiesA) / sizeof(policiesA[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode retPersist = OH_FileShare_PersistPermission(policiesA, policiesNumA, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(retPersist, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(retPersist, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    FileManagement_ErrCode retActivate = OH_FileShare_ActivatePermission(policiesA, policiesNumA, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(retActivate, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(retActivate, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    char policyUriCharB[] = "file://com.example.filesharea/data/storage/fileShare04.txt";
    FileShare_PolicyInfo policyB = {.uri = policyUriCharB,
                                    .length = sizeof(policyUriCharB) - 1,
                                    .operationMode =
                                        FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policyA, policyB};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileManagement_ErrCode ret = OH_FileShare_DeactivatePermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_EPERM);
        if (result != nullptr) {
            EXPECT_EQ(INVALID_PATH, result[0].code);
        } else {
            EXPECT_FALSE(true);
        }
    }
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_DeactivatePermission_test_004 end";
}

/**
 * @tc.name: OH_FileShare_CheckPersistentPermission_test_001
 * @tc.desc: Test function of OH_FileShare_CheckPersistentPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_CheckPersistentPermission_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_001 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/el2/base/CheckPersistentPermission.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = sizeof(policyUriChar) - 1,
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *resultPersist = nullptr;
    unsigned int resultPersistNum;
    FileManagement_ErrCode retPersist =
        OH_FileShare_PersistPermission(policies, policiesNum, &resultPersist, &resultPersistNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(retPersist, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(retPersist, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(resultPersist, resultPersistNum);
    bool *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_CheckPersistentPermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_NO_ERROR);
        if (result != nullptr) {
            EXPECT_EQ(result[0], true);
        } else {
            EXPECT_FALSE(true);
        }
    }
    if (result != nullptr) {
        free(result);
    }
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_001 end";
}

/**
 * @tc.name: OH_FileShare_CheckPersistentPermission_test_002
 * @tc.desc: Test function of OH_FileShare_CheckPersistentPermission() interface for invalid path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_CheckPersistentPermission_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_002 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/fileShare02.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = sizeof(policyUriChar) - 1,
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    bool *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_CheckPersistentPermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_NO_ERROR);
        if (result != nullptr) {
            EXPECT_EQ(result[0], false);
        } else {
            EXPECT_FALSE(true);
        }
    }
    if (result != nullptr) {
        free(result);
    }
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_002 end";
}

/**
 * @tc.name: OH_FileShare_CheckPersistentPermission_test_003
 * @tc.desc: Test function of OH_FileShare_CheckPersistentPermission() interface for err params.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_CheckPersistentPermission_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_003 start";
    FileShare_PolicyInfo *policies = nullptr;
    unsigned int policiesNum = SET_ZERO;
    bool *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_CheckPersistentPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, E_PARAMS);
    if (result != nullptr) {
        free(result);
    }
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_003 end";
}

/**
 * @tc.name: OH_FileShare_CheckPersistentPermission_test_004
 * @tc.desc: Test function of OH_FileShare_CheckPersistentPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionTest, OH_FileShare_CheckPersistentPermission_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_004 start";
    char policyUriCharA[] = "file://com.example.filesharea/data/storage/el2/base/CheckPersistentPermission.txt";
    FileShare_PolicyInfo policyA = {.uri = policyUriCharA,
                                    .length = sizeof(policyUriCharA) - 1,
                                    .operationMode =
                                        FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policiesA[] = {policyA};
    unsigned int policiesNumA = sizeof(policiesA) / sizeof(policiesA[0]);
    FileShare_PolicyErrorResult *resultPersist = nullptr;
    unsigned int resultPersistNum;
    FileManagement_ErrCode retPersist =
        OH_FileShare_PersistPermission(policiesA, policiesNumA, &resultPersist, &resultPersistNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(retPersist, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(retPersist, E_NO_ERROR);
    }
    OH_FileShare_ReleasePolicyErrorResult(resultPersist, resultPersistNum);
    char policyUriCharB[] = "file://com.example.filesharea/data/storage/fileShare04.txt";
    FileShare_PolicyInfo policyB = {.uri = policyUriCharB,
                                    .length = sizeof(policyUriCharB) - 1,
                                    .operationMode =
                                        FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policyA, policyB};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    bool *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_CheckPersistentPermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_NO_ERROR);
        if (result != nullptr && resultNum == 2) {
            EXPECT_EQ(result[0], true);
            EXPECT_EQ(result[1], false);
        } else {
            EXPECT_FALSE(true);
        }
    }
    if (result != nullptr) {
        free(result);
    }
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_004 end";
}
} // namespace OHOS::AppFileService::ModuleFileSharePermission
