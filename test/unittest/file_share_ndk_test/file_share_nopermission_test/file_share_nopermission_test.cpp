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

#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>
#include <string>

#include "common_func.h"
#include "error_code.h"
#include "oh_file_share.h"
#include "parameter.h"

using namespace testing::ext;
using namespace OHOS::AppFileService;
namespace {
const std::string FILE_MANAGER_FULL_MOUNT_ENABLE_PARAMETER_STR = "const.filemanager.full_mount.enable";
} // namespace

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

namespace OHOS::AppFileService::ModuleFileShareNoPermission {

class NDKFileShareNoPermissionTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.name: OH_FileShare_PersistPermission_test
 * @tc.desc: Test function of OH_FileShare_PersistPermission() interface for no permission.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileShareNoPermissionTest, OH_FileShare_PersistPermission_test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test start";
    char policyUriChar[] = "file://com.example.filesharea/storage";
    FileShare_PolicyInfo policy = {
        .uri = policyUriChar,
        .length = sizeof(policyUriChar) - 1,
        .operationMode = FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE
    };
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    if (!CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, E_DEVICE_NOT_SUPPORT);
    } else {
        EXPECT_EQ(ret, E_PERMISSION);
    }
    if (result != nullptr) {
        OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    }
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test end";
}

/**
 * @tc.name: OH_FileShare_RevokePermission_test
 * @tc.desc: Test function of OH_FileShare_RevokePermission() interface for no permission.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileShareNoPermissionTest, OH_FileShare_RevokePermission_test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_RevokePermission_test start";
    char policyUriChar[] = "file://com.example.filesharea/storage";
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
        EXPECT_EQ(ret, E_PERMISSION);
    }
    if (result != nullptr) {
        OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    }
    GTEST_LOG_(INFO) << "OH_FileShare_RevokePermission_test end";
}

/**
 * @tc.name: OH_FileShare_ActivatePermission_test
 * @tc.desc: Test function of OH_FileShare_ActivatePermission() interface for no permission.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileShareNoPermissionTest, OH_FileShare_ActivatePermission_test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_ActivatePermission_test start";
    char policyUriChar[] = "file://com.example.filesharea/storage";
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
        EXPECT_EQ(ret, E_PERMISSION);
    }
    if (result != nullptr) {
        OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    }
    GTEST_LOG_(INFO) << "OH_FileShare_ActivatePermission_test end";
}

/**
 * @tc.name: OH_FileShare_DeactivatePermission_test
 * @tc.desc: Test function of OH_FileShare_DeactivatePermission() interface for no permission.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileShareNoPermissionTest, OH_FileShare_DeactivatePermission_test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_DeactivatePermission_test start";
    char policyUriChar[] = "file://com.example.filesharea/storage";
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
        EXPECT_EQ(ret, E_PERMISSION);
    }
    if (result != nullptr) {
        OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    }
    GTEST_LOG_(INFO) << "OH_FileShare_DeactivatePermission_test end";
}

/**
 * @tc.name: OH_FileShare_CheckPersistentPermission_test
 * @tc.desc: Test function of OH_FileShare_CheckPersistentPermission() interface for no permission.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileShareNoPermissionTest, OH_FileShare_CheckPersistentPermission_test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test start";
    char policyUriChar[] = "file://com.example.filesharea/storage";
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
        EXPECT_EQ(ret, E_PERMISSION);
    }
    if (result != nullptr) {
        free(result);
    }
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test end";
}

} // namespace OHOS::AppFileService::ModuleFileShareNoPermission