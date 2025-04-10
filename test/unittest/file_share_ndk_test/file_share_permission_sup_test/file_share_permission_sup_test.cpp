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
#include "oh_file_share.h"

#include <cassert>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <singleton.h>
#include <string>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "permission_def.h"
#include "permission_state_full.h"
#include "securec.h"
#include "token_setproc.h"
#include "uri.h"

#include "error_code.h"
#include "file_permission_mock.h"

using namespace testing::ext;
using namespace testing;
using namespace OHOS::AppFileService;
using namespace OHOS::Security::AccessToken;

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

class NDKFileSharePermissionSupTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() {};
    void TearDown() {};
    static inline shared_ptr<FilePermissionMock> filePermMoc_ = nullptr;
};

void NDKFileSharePermissionSupTest::SetUpTestCase()
{
    GrantNativePermission();

    filePermMoc_ = make_shared<FilePermissionMock>();
    FilePermissionMock::filePermissionMock = filePermMoc_;
}

void NDKFileSharePermissionSupTest::TearDownTestCase()
{
    FilePermissionMock::filePermissionMock = nullptr;
    filePermMoc_  = nullptr;
}

/**
 * @tc.name: OH_FileShare_PersistPermission_test_002
 * @tc.desc: Test function of OH_FileShare_PersistPermission() interface for invalid path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionSupTest, OH_FileShare_PersistPermission_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_002 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/fileShare02.txt";
    FileShare_PolicyInfo policy = {.uri = nullptr,
                                   .length = 0,
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_PARAMS);
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);

    policies[0].uri = policyUriChar;
    ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_PARAMS);
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);

    policies[0].length = sizeof(policyUriChar);
    ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_PARAMS);
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_002 end";
}

/**
 * @tc.name: OH_FileShare_PersistPermission_test_003
 * @tc.desc: Test function of OH_FileShare_PersistPermission() interface for invalid path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionSupTest, OH_FileShare_PersistPermission_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_003 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/fileShare02.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = strlen(policyUriChar),
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;
    
    EXPECT_CALL(*filePermMoc_, PersistPermission(_, _)).WillOnce(Return(ERR_PERMISSION_ERROR));
    FileManagement_ErrCode ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_ENOMEM);
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    
    std::deque<PolicyErrorResult> errorResults;
    for (int i = 0; i <= MAX_ARRAY_SIZE; i++) {
        PolicyErrorResult rlt;
        errorResults.push_back(rlt);
    }

    EXPECT_CALL(*filePermMoc_, PersistPermission(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(errorResults), Return(ERR_PARAMS)));
    ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_ENOMEM);
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_003 end";
}

/**
 * @tc.name: OH_FileShare_PersistPermission_test_004
 * @tc.desc: Test function of OH_FileShare_PersistPermission() interface for invalid path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionSupTest, OH_FileShare_PersistPermission_test_004, TestSize.Level1)
{
    char policyUriChar[] = "file://com.example.filesharea/data/storage/fileShare02.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = strlen(policyUriChar),
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;

    PolicyErrorResult rltOne = {.uri = string(policy.uri)};
    std::deque<PolicyErrorResult> errorResults;
    errorResults.push_back(rltOne);
    EXPECT_CALL(*filePermMoc_, PersistPermission(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(errorResults), Return(EPERM)));
    FileManagement_ErrCode ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_EPERM);
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);

    EXPECT_CALL(*filePermMoc_, PersistPermission(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(errorResults), Return(ERR_EPERM)));
    ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_UNKNOWN);
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_004 end";
}

/**
 * @tc.name: OH_FileShare_PersistPermission_test_005
 * @tc.desc: Test function of OH_FileShare_PersistPermission() interface for invalid path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionSupTest, OH_FileShare_PersistPermission_test_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_005 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/fileShare02.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = strlen(policyUriChar),
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;

    EXPECT_CALL(*filePermMoc_, PersistPermission(_, _)).WillOnce(Return(ERR_OK));
    FileManagement_ErrCode ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_OK);
    OH_FileShare_ReleasePolicyErrorResult(result, resultNum);
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test_005 end";
}

/**
 * @tc.name: OH_FileShare_CheckPersistentPermission_test_002
 * @tc.desc: Test function of OH_FileShare_CheckPersistentPermission() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionSupTest, OH_FileShare_CheckPersistentPermission_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_002 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/fileShare02.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = 0,
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    bool *result = nullptr;
    unsigned int resultNum;
    FileManagement_ErrCode ret = OH_FileShare_CheckPersistentPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_PARAMS);
    if (result != nullptr) {
        free(result);
    }

    policies[0].length = strlen(policyUriChar);
    EXPECT_CALL(*filePermMoc_, CheckPersistentPermission(_, _)).WillOnce(Return(EPERM));
    ret = OH_FileShare_CheckPersistentPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_EPERM);
    if (result != nullptr) {
        free(result);
    }
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_002 end";
}

/**
 * @tc.name: OH_FileShare_CheckPersistentPermission_test_003
 * @tc.desc: Test function of OH_FileShare_CheckPersistentPermission() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileSharePermissionSupTest, OH_FileShare_CheckPersistentPermission_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_003 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/fileShare02.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = strlen(policyUriChar),
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    bool *result = nullptr;
    unsigned int resultNum;
    EXPECT_CALL(*filePermMoc_, CheckPersistentPermission(_, _)).WillOnce(Return(ERR_OK));
    FileManagement_ErrCode ret = OH_FileShare_CheckPersistentPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_ENOMEM);
    if (result != nullptr) {
        free(result);
    }
    
    std::vector<bool> errorResults;
    for (int i = 0; i <= sizeof(FileShare_PolicyErrorResult) * MAX_ARRAY_SIZE; i++) {
        errorResults.push_back(false);
    }

    EXPECT_CALL(*filePermMoc_, CheckPersistentPermission(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(errorResults), Return(ERR_OK)));
    ret = OH_FileShare_CheckPersistentPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_ENOMEM);
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
HWTEST_F(NDKFileSharePermissionSupTest, OH_FileShare_CheckPersistentPermission_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_004 start";
    char policyUriChar[] = "file://com.example.filesharea/data/storage/fileShare02.txt";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = strlen(policyUriChar),
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    bool *result = nullptr;
    unsigned int resultNum;

    std::vector<bool> errorResults;
    errorResults.push_back(false);

    EXPECT_CALL(*filePermMoc_, CheckPersistentPermission(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(errorResults), Return(ERR_OK)));
    FileManagement_ErrCode ret = OH_FileShare_CheckPersistentPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, ERR_OK);

    if (result != nullptr) {
        free(result);
    }
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test_004 end";
}
} // namespace OHOS::AppFileService::ModuleFileSharePermission
