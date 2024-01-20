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
#include "file_permission_test.h"
#include <cassert>
#include <fcntl.h>

#include <singleton.h>
#include <string>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "access_token_error.h"
#include "accesstoken_kit.h"
#include "file_permission.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "parameter.h"
#include "sandbox_helper.h"
#include "uri.h"

#include "nativetoken_kit.h"
#include "permission_def.h"
#include "permission_state_full.h"
#include "token_setproc.h"

using namespace testing::ext;
namespace OHOS {
namespace AppFileService {
const char *g_fileManagerFullMountEnableParameter = "const.filemanager.full_mount.enable";
const std::string SET_POLICY_PERMISSION = "ohos.permission.SET_SANDBOX_POLICY";
const std::string ACCESS_PERSIST_PERMISSION = "ohos.permission.FILE_ACCESS_PERSIST";
const std::string BundleName = "com.example.filesharea";
uint64_t g_mockToken;
OHOS::Security::AccessToken::PermissionStateFull g_testState1 = {
    .permissionName = SET_POLICY_PERMISSION,
    .isGeneral = true,
    .resDeviceID = {"1"},
    .grantStatus = {0},
    .grantFlags = {0},
};
OHOS::Security::AccessToken::PermissionStateFull g_testState2 = {
    .permissionName = ACCESS_PERSIST_PERMISSION,
    .isGeneral = true,
    .resDeviceID = {"1"},
    .grantStatus = {0},
    .grantFlags = {0},
};
OHOS::Security::AccessToken::HapInfoParams g_testInfoParams = {
    .userID = 1,
    .bundleName = "full_mount_permission_test",
    .instIndex = 0,
    .appIDDesc = "test",
};
OHOS::Security::AccessToken::HapPolicyParams g_testPolicyParams = {
    .apl = OHOS::Security::AccessToken::APL_NORMAL,
    .domain = "test.domain",
    .permList = {},
    .permStateList = {g_testState1, g_testState2},
};

void NativeTokenGet()
{
    uint64_t fullTokenId;
    const char **perms = new const char *[1];
    perms[0] = "ohos.permission.DISTRIBUTED_DATASYNC";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_core",
    };
    infoInstance.processName = "TestCase";
    fullTokenId = GetAccessTokenId(&infoInstance);
    EXPECT_EQ(0, SetSelfTokenID(fullTokenId));
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
}
void FilePermissionTest::SetUpTestCase()
{
    NativeTokenGet();
    OHOS::Security::AccessToken::AccessTokenID tokenId =
        OHOS::Security::AccessToken::AccessTokenKit::GetNativeTokenId("foundation");
    EXPECT_EQ(0, tokenId);
    EXPECT_EQ(0, SetSelfTokenID(tokenId));
    OHOS::Security::AccessToken::AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = OHOS::Security::AccessToken::AccessTokenKit::AllocHapToken(g_testInfoParams, g_testPolicyParams);
    EXPECT_EQ(0, tokenIdEx.tokenIdExStruct.tokenID);
    g_mockToken = tokenIdEx.tokenIdExStruct.tokenID;
    EXPECT_EQ(0, SetSelfTokenID(tokenIdEx.tokenIdExStruct.tokenID));
}
void FilePermissionTest::TearDownTestCase()
{
    OHOS::Security::AccessToken::AccessTokenKit::DeleteToken(g_mockToken);
}

static bool CheckFileManagerFullMountEnable()
{
    char value[] = "false";
    int retSystem = GetParameter(g_fileManagerFullMountEnableParameter, "false", value, sizeof(value));
    if (retSystem > 0 && !std::strcmp(value, "true")) {
        return true;
    }
    LOGE("Not supporting all mounts");
    return false;
}
/**
 * @tc.name: PersistPermission_test_0000
 * @tc.desc: Test function of PersistPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, PersistPermission_test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin PersistPermission_test_0000";
    UriPolicyInfo infoA = {.uri = "file://" + BundleName + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    int32_t ret = FilePermission::PersistPermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "FileShareTest-end PersistPermission_test_0000";
}
/**
 * @tc.name: PersistPermission_test_0001
 * @tc.desc: Test function of PersistPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, PersistPermission_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin PersistPermission_test_0001";
    UriPolicyInfo infoA = {.uri = "file://" + BundleName + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    UriPolicyInfo infoB = {.uri = "file://" + BundleName + "/storage", .mode = -1};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    uriPolicies.emplace_back(infoB);
    deque<struct PolicyErrorResult> errorResults;
    int32_t ret = FilePermission::PersistPermission(uriPolicies, errorResults);
    if (CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, EPERM);
        EXPECT_EQ(INVALID_MODE, errorResults[0].code);
    } else {
        EXPECT_EQ(ret, 0);
    };
    GTEST_LOG_(INFO) << "FileShareTest-end PersistPermission_test_0001";
}
/**
 * @tc.name: PersistPermission_test_0002
 * @tc.desc: Test function of PersistPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, PersistPermission_test_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin PersistPermission_test_0002";
    UriPolicyInfo infoA = {.uri = "file://" + BundleName + "/storage/test01.test",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    int32_t ret = FilePermission::PersistPermission(uriPolicies, errorResults);
    if (CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, EPERM);
        ASSERT_EQ(uriPolicies.size(), errorResults.size());
        EXPECT_EQ(INVALID_PATH, errorResults[0].code);
    } else {
        EXPECT_EQ(ret, 0);
    };
    GTEST_LOG_(INFO) << "FileShareTest-end PersistPermission_test_0002";
}
/**
 * @tc.name: ActivatePermission_test_0000
 * @tc.desc: Test function of ActivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, ActivatePermission_test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin ActivatePermission_test_0000";
    UriPolicyInfo infoA = {.uri = "file://" + BundleName + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    int32_t ret = FilePermission::ActivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "FileShareTest-end ActivatePermission_test_0000";
}
/**
 * @tc.name: ActivatePermission_test_0001
 * @tc.desc: Test function of PersistPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, ActivatePermission_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin ActivatePermission_test_0001";
    UriPolicyInfo infoA = {.uri = "file://" + BundleName + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    UriPolicyInfo infoB = {.uri = "file://" + BundleName + "/storage", .mode = -1};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    uriPolicies.emplace_back(infoB);
    deque<struct PolicyErrorResult> errorResults;
    int32_t ret = FilePermission::ActivatePermission(uriPolicies, errorResults);
    if (CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, EPERM);
        EXPECT_EQ(INVALID_MODE, errorResults[0].code);
    } else {
        EXPECT_EQ(ret, 0);
    };
    GTEST_LOG_(INFO) << "FileShareTest-end ActivatePermission_test_0001";
}
/**
 * @tc.name: ActivatePermission_test_0002
 * @tc.desc: Test function of PersistPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, ActivatePermission_test_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin ActivatePermission_test_0002";
    UriPolicyInfo infoA = {.uri = "file://" + BundleName + "/storage/test01.test",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    int32_t ret = FilePermission::ActivatePermission(uriPolicies, errorResults);
    if (CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, EPERM);
        ASSERT_EQ(uriPolicies.size(), errorResults.size());
        EXPECT_EQ(INVALID_PATH, errorResults[0].code);
    } else {
        EXPECT_EQ(ret, 0);
    };
    GTEST_LOG_(INFO) << "FileShareTest-end ActivatePermission_test_0002";
}
/**
 * @tc.name: DeactivatePermission_test_0000
 * @tc.desc: Test function of DeactivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, DeactivatePermission_test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin DeactivatePermission_test_0000";
    UriPolicyInfo infoA = {.uri = "file://" + BundleName + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    int32_t ret = FilePermission::DeactivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "FileShareTest-end DeactivatePermission_test_0000";
}
/**
 * @tc.name: DeactivatePermission_test_0001
 * @tc.desc: Test function of DeactivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, DeactivatePermission_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin DeactivatePermission_test_0001";
    UriPolicyInfo infoA = {.uri = "file://" + BundleName + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    UriPolicyInfo infoB = {.uri = "file://" + BundleName + "/storage", .mode = -1};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    uriPolicies.emplace_back(infoB);
    deque<struct PolicyErrorResult> errorResults;
    int32_t ret = FilePermission::DeactivatePermission(uriPolicies, errorResults);
    if (CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, EPERM);
        EXPECT_EQ(INVALID_MODE, errorResults[0].code);
    } else {
        EXPECT_EQ(ret, 0);
    };
    GTEST_LOG_(INFO) << "FileShareTest-end DeactivatePermission_test_0001";
}
/**
 * @tc.name: DeactivatePermission_test_0002
 * @tc.desc: Test function of DeactivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, DeactivatePermission_test_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin DeactivatePermission_test_0002";
    UriPolicyInfo infoA = {.uri = "file://" + BundleName + "/storage/test01.test",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    int32_t ret = FilePermission::DeactivatePermission(uriPolicies, errorResults);
    if (CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, EPERM);
        ASSERT_EQ(uriPolicies.size(), errorResults.size());
        EXPECT_EQ(INVALID_PATH, errorResults[0].code);
    } else {
        EXPECT_EQ(ret, 0);
    };
    GTEST_LOG_(INFO) << "FileShareTest-end DeactivatePermission_test_0002";
}
/**
 * @tc.name: RevokePermission_test_0000
 * @tc.desc: Test function of RevokePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, RevokePermission_test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin RevokePermission_test_0000";
    UriPolicyInfo infoA = {.uri = "file://" + BundleName + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    int32_t ret = FilePermission::RevokePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "FileShareTest-end RevokePermission_test_0000";
}
/**
 * @tc.name: RevokePermission_test_0001
 * @tc.desc: Test function of RevokePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, RevokePermission_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin RevokePermission_test_0001";
    UriPolicyInfo infoA = {.uri = "file://" + BundleName + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    UriPolicyInfo infoB = {.uri = "file://" + BundleName + "/storage", .mode = -1};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    uriPolicies.emplace_back(infoB);
    deque<struct PolicyErrorResult> errorResults;
    int32_t ret = FilePermission::RevokePermission(uriPolicies, errorResults);
    if (CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, EPERM);
        EXPECT_EQ(INVALID_MODE, errorResults[0].code);
    } else {
        EXPECT_EQ(ret, 0);
    };
    GTEST_LOG_(INFO) << "FileShareTest-end RevokePermission_test_0001";
}
/**
 * @tc.name: RevokePermission_test_0002
 * @tc.desc: Test function of RevokePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, RevokePermission_test_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin RevokePermission_test_0002";
    UriPolicyInfo infoA = {.uri = "file://" + BundleName + "/storage/test01.test",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    int32_t ret = FilePermission::RevokePermission(uriPolicies, errorResults);
    if (CheckFileManagerFullMountEnable()) {
        EXPECT_EQ(ret, EPERM);
        ASSERT_EQ(uriPolicies.size(), errorResults.size());
        EXPECT_EQ(INVALID_PATH, errorResults[0].code);
    } else {
        EXPECT_EQ(ret, 0);
    };
    GTEST_LOG_(INFO) << "FileShareTest-end RevokePermission_test_0002";
}
} // namespace AppFileService
} // namespace OHOS