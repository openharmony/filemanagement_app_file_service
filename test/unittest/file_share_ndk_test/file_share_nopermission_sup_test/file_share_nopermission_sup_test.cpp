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
#include <memory>
#include <string>

#include "securec.h"

#include "common_func.h"
#include "error_code.h"
#include "oh_file_share.h"
#include "parameter_mock.h"

using namespace testing::ext;
using namespace testing;
using namespace OHOS::AppFileService;
using namespace std;

namespace OHOS::AppFileService::ModuleFileShareNoPermission {

class NDKFileShareNoPermissionSupTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() {};
    void TearDown() {};
    static inline shared_ptr<ParamMoc> paramMoc_ = nullptr;
};

void NDKFileShareNoPermissionSupTest::SetUpTestCase()
{
    paramMoc_ = make_shared<ParamMoc>();
    ParamMoc::paramMoc = paramMoc_;
}

void NDKFileShareNoPermissionSupTest::TearDownTestCase()
{
    ParamMoc::paramMoc = nullptr;
    paramMoc_ = nullptr;
}

/**
 * @tc.name: OH_FileShare_PersistPermission_test
 * @tc.desc: Test function of OH_FileShare_PersistPermission() interface for no permission.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(NDKFileShareNoPermissionSupTest, OH_FileShare_PersistPermission_test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_PersistPermission_test start";
    char policyUriChar[] = "file://com.example.filesharea/storage";
    FileShare_PolicyInfo policy = {
        .uri = policyUriChar,
        .length = strlen(policyUriChar),
        .operationMode = FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE
    };
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;

    EXPECT_CALL(*paramMoc_, GetParameter(_, _, _, _)).WillOnce(DoAll(WithArgs<2>(Invoke([](char *value) {
        memcpy_s(value, sizeof("false"), "true", sizeof("true"));
    })), Return(1)));
    FileManagement_ErrCode ret = OH_FileShare_PersistPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, E_PERMISSION);
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
HWTEST_F(NDKFileShareNoPermissionSupTest, OH_FileShare_RevokePermission_test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_RevokePermission_test start";
    char policyUriChar[] = "file://com.example.filesharea/storage";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = strlen(policyUriChar),
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;

    EXPECT_CALL(*paramMoc_, GetParameter(_, _, _, _)).WillOnce(DoAll(WithArgs<2>(Invoke([](char *value) {
        memcpy_s(value, sizeof("false"), "true", sizeof("true"));
    })), Return(1)));
    FileManagement_ErrCode ret = OH_FileShare_RevokePermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, E_PERMISSION);
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
HWTEST_F(NDKFileShareNoPermissionSupTest, OH_FileShare_ActivatePermission_test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_ActivatePermission_test start";
    char policyUriChar[] = "file://com.example.filesharea/storage";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = strlen(policyUriChar),
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;

    EXPECT_CALL(*paramMoc_, GetParameter(_, _, _, _)).WillOnce(DoAll(WithArgs<2>(Invoke([](char *value) {
        memcpy_s(value, sizeof("false"), "true", sizeof("true"));
    })), Return(1)));
    FileManagement_ErrCode ret = OH_FileShare_ActivatePermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, E_PERMISSION);
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
HWTEST_F(NDKFileShareNoPermissionSupTest, OH_FileShare_DeactivatePermission_test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_DeactivatePermission_test start";
    char policyUriChar[] = "file://com.example.filesharea/storage";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = strlen(policyUriChar),
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    FileShare_PolicyErrorResult *result = nullptr;
    unsigned int resultNum;

    EXPECT_CALL(*paramMoc_, GetParameter(_, _, _, _)).WillOnce(DoAll(WithArgs<2>(Invoke([](char *value) {
        memcpy_s(value, sizeof("false"), "true", sizeof("true"));
    })), Return(1)));
    FileManagement_ErrCode ret = OH_FileShare_DeactivatePermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, E_PERMISSION);
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
HWTEST_F(NDKFileShareNoPermissionSupTest, OH_FileShare_CheckPersistentPermission_test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test start";
    char policyUriChar[] = "file://com.example.filesharea/storage";
    FileShare_PolicyInfo policy = {.uri = policyUriChar,
                                   .length = strlen(policyUriChar),
                                   .operationMode =
                                       FileShare_OperationMode::READ_MODE | FileShare_OperationMode::WRITE_MODE};
    FileShare_PolicyInfo policies[] = {policy};
    unsigned int policiesNum = sizeof(policies) / sizeof(policies[0]);
    bool *result = nullptr;
    unsigned int resultNum;

    EXPECT_CALL(*paramMoc_, GetParameter(_, _, _, _)).WillOnce(DoAll(WithArgs<2>(Invoke([](char *value) {
        memcpy_s(value, sizeof("false"), "true", sizeof("true"));
    })), Return(1)));
    FileManagement_ErrCode ret = OH_FileShare_CheckPersistentPermission(policies, policiesNum, &result, &resultNum);
    EXPECT_EQ(ret, E_PERMISSION);
    if (result != nullptr) {
        free(result);
    }
    GTEST_LOG_(INFO) << "OH_FileShare_CheckPersistentPermission_test end";
}

} // namespace OHOS::AppFileService::ModuleFileShareNoPermission