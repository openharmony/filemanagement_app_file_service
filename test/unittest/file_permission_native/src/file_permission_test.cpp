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

int fseeko(FILE *stream, off_t offset, int whence)
{
    if (OHOS::AppFileService::LibraryFunc::libraryFunc_ == nullptr) {
        return -1;
    }
    return OHOS::AppFileService::LibraryFunc::libraryFunc_->fseeko(stream, offset, whence);
}

int access(const char *pathname, int mode)
{
    if (OHOS::AppFileService::LibraryFunc::libraryFunc_ == nullptr) {
        return -1;
    }
    return OHOS::AppFileService::LibraryFunc::libraryFunc_->access(pathname, mode);
}

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace AppFileService {
const std::string SET_POLICY_PERMISSION = "ohos.permission.SET_SANDBOX_POLICY";
const std::string ACCESS_PERSIST_PERMISSION = "ohos.permission.FILE_ACCESS_PERSIST";
const std::string READ_WRITE_DOWNLOAD_PERMISSION = "ohos.permission.READ_WRITE_DOWNLOAD_DIRECTORY";
const std::string BUNDLE_NAME_A = "com.example.filesharea";
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
OHOS::Security::AccessToken::PermissionStateFull g_testState3 = {
    .permissionName = READ_WRITE_DOWNLOAD_PERMISSION,
    .isGeneral = true,
    .resDeviceID = {"1"},
    .grantStatus = {0},
    .grantFlags = {0},
};
OHOS::Security::AccessToken::HapInfoParams g_testInfoParams = {
    .userID = 1,
    .bundleName = BUNDLE_NAME_A,
    .instIndex = 0,
    .appIDDesc = "test",
};
OHOS::Security::AccessToken::HapPolicyParams g_testPolicyParams = {
    .apl = OHOS::Security::AccessToken::APL_NORMAL,
    .domain = "test.domain",
    .permList = {},
    .permStateList = {g_testState1, g_testState2, g_testState3},
};

void FilePermissionTest::SetUpTestCase()
{
    OHOS::Security::AccessToken::AccessTokenID tokenId =
        OHOS::Security::AccessToken::AccessTokenKit::GetNativeTokenId("foundation");
    EXPECT_EQ(0, SetSelfTokenID(tokenId));
    OHOS::Security::AccessToken::AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = OHOS::Security::AccessToken::AccessTokenKit::AllocHapToken(g_testInfoParams, g_testPolicyParams);
    g_mockToken = tokenIdEx.tokenIdExStruct.tokenID;
    EXPECT_EQ(0, SetSelfTokenID(tokenIdEx.tokenIdExStruct.tokenID));
#ifdef SANDBOX_MANAGER
    sandboxMock_ = make_shared<SandboxMock>();
    SandboxMock::sandboxManagerKitMock = sandboxMock_;
    funcMock = make_shared<LibraryFuncMock>();
    LibraryFuncMock::libraryFunc_ = funcMock;
    upmsMock_ = make_shared<UpmsMock>();
    UpmsMock::upmsManagerKitMock = upmsMock_;
#endif
}
void FilePermissionTest::TearDownTestCase()
{
    OHOS::Security::AccessToken::AccessTokenKit::DeleteToken(g_mockToken);
#ifdef SANDBOX_MANAGER
    SandboxMock::sandboxManagerKitMock = nullptr;
    sandboxMock_ = nullptr;
    LibraryFuncMock::libraryFunc_ = nullptr;
    funcMock = nullptr;
    UpmsMock::upmsManagerKitMock = nullptr;
    upmsMock_ = nullptr;
#endif
}

#ifdef SANDBOX_MANAGER

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
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.push_back(PolicyErrorCode::INVALID_PATH);
    // EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, PersistPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::PersistPermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
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
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    UriPolicyInfo infoB = {.uri = "file://" + BUNDLE_NAME_A + "/storage", .mode = -1};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    uriPolicies.emplace_back(infoB);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.push_back(PolicyErrorCode::INVALID_MODE);
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, PersistPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::PersistPermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
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
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/test01.test",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, PersistPolicy(_, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::PersistPermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
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
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*upmsMock_, Active(_, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::ActivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
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
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    UriPolicyInfo infoB = {.uri = "file://" + BUNDLE_NAME_A + "/storage", .mode = -1};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    uriPolicies.emplace_back(infoB);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.push_back(PolicyErrorCode::INVALID_MODE);
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*upmsMock_, Active(_, _)).WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::ActivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
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
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/test01.test",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*upmsMock_, Active(_, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::ActivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
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
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, StopAccessingPolicy(_, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::DeactivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
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
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    UriPolicyInfo infoB = {.uri = "file://" + BUNDLE_NAME_A + "/storage", .mode = -1};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    uriPolicies.emplace_back(infoB);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    resultCodes.push_back(PolicyErrorCode::INVALID_MODE);
    EXPECT_CALL(*sandboxMock_, StopAccessingPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::DeactivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
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
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/test01.test",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, StopAccessingPolicy(_, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::DeactivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
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
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, UnPersistPolicy(_, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::RevokePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
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
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage", .mode = -1};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.push_back(PolicyErrorCode::INVALID_MODE);
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, UnPersistPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::RevokePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
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
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/test01.test",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, UnPersistPolicy(_, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::RevokePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    GTEST_LOG_(INFO) << "FileShareTest-end RevokePermission_test_0002";
}
/**
 * @tc.name: GetPathByPermission_test_0001
 * @tc.desc: Test function of GetPathByPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, GetPathByPermission_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin GetPathByPermission_test_0001";
    std::string PermissionName = "ohos.permission.READ_WRITE_DOWNLOAD_DIRECTORY";
    std::string sandboxPath = "/storage/Users/currentUser/Download";
    std::string path = FilePermission::GetPathByPermission("", PermissionName);
    EXPECT_EQ(path, sandboxPath);
    GTEST_LOG_(INFO) << "FileShareTest-end GetPathByPermission_test_0001";
}
/**
 * @tc.name: GetPathByPermission_test_0002
 * @tc.desc: Test function of GetPathByPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, GetPathByPermission_test_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin GetPathByPermission_test_0002";
    std::string PermissionName = "ohos.permission.READ_WRITE_DESKTOP_DIRECTORY";
    std::string sandboxPath = "/storage/Users/currentUser/Desktop";
    std::string path = FilePermission::GetPathByPermission("", PermissionName);
    EXPECT_EQ(path, sandboxPath);
    GTEST_LOG_(INFO) << "FileShareTest-end GetPathByPermission_test_0002";
}
/**
 * @tc.name: GetPathByPermission_test_0003
 * @tc.desc: Test function of GetPathByPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, GetPathByPermission_test_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin GetPathByPermission_test_0003";
    std::string PermissionName = "ohos.permission.READ_WRITE_DOCUMENTS_DIRECTORY";
    std::string sandboxPath = "/storage/Users/currentUser/Documents";
    std::string path = FilePermission::GetPathByPermission("", PermissionName);
    EXPECT_EQ(path, sandboxPath);
    GTEST_LOG_(INFO) << "FileShareTest-end GetPathByPermission_test_0003";
}
/**
 * @tc.name: CheckPersistentPermission_test_0001
 * @tc.desc: Test function of CheckPersistentPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, CheckPersistentPermission_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin CheckPersistentPermission_test_0001";
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    vector<bool> errorResults;
    EXPECT_CALL(*sandboxMock_, CheckPersistPolicy(_, _, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::CheckPersistentPermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "FileShareTest-end CheckPersistentPermission_test_0001";
}
/**
 * @tc.name: CheckPersistentPermission_test_0002
 * @tc.desc: Test function of CheckPersistentPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, CheckPersistentPermission_test_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin CheckPersistentPermission_test_0002";
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    deque<struct PolicyErrorResult> errorResults;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, PersistPolicy(_, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    EXPECT_CALL(*sandboxMock_, CheckPersistPolicy(_, _, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::PersistPermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    vector<bool> errorResult;
    ret = FilePermission::CheckPersistentPermission(uriPolicies, errorResult);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "FileShareTest-end CheckPersistentPermission_test_0002";
}
/**
 * @tc.name: CheckPersistentPermission_test_0003
 * @tc.desc: Test function of CheckPersistentPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, CheckPersistentPermission_test_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin CheckPersistentPermission_test_0003";
    UriPolicyInfo infoA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/001.txt",
                           .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    vector<bool> errorResult;
    EXPECT_CALL(*sandboxMock_, CheckPersistPolicy(_, _, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::CheckPersistentPermission(uriPolicies, errorResult);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "FileShareTest-end CheckPersistentPermission_test_0003";
}

/**
 * @tc.name: GetPathPolicyInfoFromUriPolicyInfo_test_0001
 * @tc.desc: Test function of GetPathPolicyInfoFromUriPolicyInfo() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, GetPathPolicyInfoFromUriPolicyInfo_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin GetPathPolicyInfoFromUriPolicyInfo_test_0001";
    UriPolicyInfo infoB = {.uri = BUNDLE_NAME_A + "/storage/001.txt"};
    UriPolicyInfo infoC = {.uri = "file://media/Photo/10/1.jpeg"};
    UriPolicyInfo infoD = {.uri = "file://" + BUNDLE_NAME_A + "/storage/001.txt?networkid=5454541547878787878748748"};
    UriPolicyInfo infoE = {.uri = "file://" + BUNDLE_NAME_A + "/storage"};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoB);
    uriPolicies.emplace_back(infoC);
    uriPolicies.emplace_back(infoD);
    uriPolicies.emplace_back(infoE);
    vector<bool> errorResult;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    auto pathPolicies = FilePermission::GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResult);
    ASSERT_TRUE(errorResult.size() == 4);
    EXPECT_EQ(errorResult[3], true);
    GTEST_LOG_(INFO) << "FileShareTest-end GetPathPolicyInfoFromUriPolicyInfo_test_0001";
}

/**
 * @tc.name: ParseErrorResults_test_0001
 * @tc.desc: Test function of ParseErrorResults() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, ParseErrorResults_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin ParseErrorResults_test_0001";
    vector<bool> resultCodes;
    vector<bool> errorResults;
    FilePermission::ParseErrorResults(resultCodes, errorResults);
    EXPECT_EQ(errorResults.size(), 0);

    resultCodes.push_back(false);
    FilePermission::ParseErrorResults(resultCodes, errorResults);
    EXPECT_EQ(errorResults.size(), 0);

    resultCodes.push_back(false);
    errorResults.push_back(true);
    FilePermission::ParseErrorResults(resultCodes, errorResults);
    ASSERT_TRUE(errorResults.size() == 1);
    EXPECT_EQ(errorResults[0], false);

    errorResults.push_back(true);
    errorResults.push_back(true);
    FilePermission::ParseErrorResults(resultCodes, errorResults);
    ASSERT_TRUE(errorResults.size() == 3);
    EXPECT_EQ(errorResults[0], false);
    EXPECT_EQ(errorResults[1], false);
    EXPECT_EQ(errorResults[2], false);
    GTEST_LOG_(INFO) << "FileShareTest-end ParseErrorResults_test_0001";
}

/**
 * @tc.name: ParseErrorResults_test_002
 * @tc.desc: Test function of ParseErrorResults() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, ParseErrorResults_test_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin ParseErrorResults_test_002";
    PolicyInfo infoA = {.path = "file://" + BUNDLE_NAME_A + "/storage/001.txt"};
    PolicyInfo infoB = {.path = BUNDLE_NAME_A + "/storage/001.txt"};
    PolicyInfo infoC = {.path = "file://media/Photo/10/1.jpeg"};
    PolicyInfo infoD = {.path = "file://" + BUNDLE_NAME_A + "/storage/001.txt?networkid=5454541547878787878748748"};
    PolicyInfo infoE = {.path = "file://" + BUNDLE_NAME_A + "/storage"};
    std::vector<PolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoA);
    uriPolicies.emplace_back(infoB);
    uriPolicies.emplace_back(infoC);
    uriPolicies.emplace_back(infoD);
    uriPolicies.emplace_back(infoE);
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::PERSISTENCE_FORBIDDEN));
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_MODE));
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_PATH));
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::PERMISSION_NOT_PERSISTED));

    deque<struct PolicyErrorResult> errorResults;
    FilePermission::ParseErrorResults(resultCodes, uriPolicies, errorResults);
    ASSERT_TRUE(errorResults.size() == 0);

    resultCodes.emplace_back(5);
    FilePermission::ParseErrorResults(resultCodes, uriPolicies, errorResults);
    ASSERT_TRUE(errorResults.size() == 4);
    EXPECT_EQ(errorResults[0].uri, infoA.path);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::PERSISTENCE_FORBIDDEN);
    EXPECT_EQ(errorResults[1].uri, infoB.path);
    EXPECT_EQ(errorResults[1].code, PolicyErrorCode::INVALID_MODE);
    EXPECT_EQ(errorResults[2].uri, infoC.path);
    EXPECT_EQ(errorResults[2].code, PolicyErrorCode::INVALID_PATH);
    EXPECT_EQ(errorResults[3].uri, infoD.path);
    EXPECT_EQ(errorResults[3].code, PolicyErrorCode::PERMISSION_NOT_PERSISTED);
    GTEST_LOG_(INFO) << "FileShareTest-end ParseErrorResults_test_002";
}
/**
 * @tc.name: PersistPermission_test_1000
 * @tc.desc: Test function of PersistPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, PersistPermission_test_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin PersistPermission_test_1000";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, PersistPolicy(_, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::PersistPermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    GTEST_LOG_(INFO) << "FileShareTest-end PersistPermission_test_1000";
}
/**
 * @tc.name: PersistPermission_test_1001
 * @tc.desc: Test function of PersistPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, PersistPermission_test_1001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin PersistPermission_test_1001";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_PATH));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, PersistPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::PersistPermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::INVALID_PATH);
    GTEST_LOG_(INFO) << "FileShareTest-end PersistPermission_test_1001";
}

/**
 * @tc.name: PersistPermission_test_1002
 * @tc.desc: Test function of PersistPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, PersistPermission_test_1002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin PersistPermission_test_1002";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt", .mode = 5};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_MODE));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, PersistPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::PersistPermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::INVALID_MODE);
    GTEST_LOG_(INFO) << "FileShareTest-end PersistPermission_test_1002";
}

/**
 * @tc.name: PersistPermission_test_1003
 * @tc.desc: Test function of PersistPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, PersistPermission_test_1003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin PersistPermission_test_1003";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::PERSISTENCE_FORBIDDEN));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, PersistPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::PersistPermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::PERSISTENCE_FORBIDDEN);
    GTEST_LOG_(INFO) << "FileShareTest-end PersistPermission_test_1003";
}

/**
 * @tc.name: PersistPermission_test_1004
 * @tc.desc: Test function of PersistPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, PersistPermission_test_1004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin PersistPermission_test_1004";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_MODE));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, PersistPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::PersistPermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::INVALID_MODE);
    GTEST_LOG_(INFO) << "FileShareTest-end PersistPermission_test_1004";
}

/**
 * @tc.name: ActivatePermission_test_1000
 * @tc.desc: Test function of ActivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, ActivatePermission_test_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin ActivatePermission_test_1000";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*upmsMock_, Active(_, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::ActivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    GTEST_LOG_(INFO) << "FileShareTest-end ActivatePermission_test_1000";
}
/**
 * @tc.name: ActivatePermission_test_1001
 * @tc.desc: Test function of ActivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, ActivatePermission_test_1001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin ActivatePermission_test_1001";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_PATH));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*upmsMock_, Active(_, _)).WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::ActivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::INVALID_PATH);
    GTEST_LOG_(INFO) << "FileShareTest-end ActivatePermission_test_1001";
}

/**
 * @tc.name: ActivatePermission_test_1002
 * @tc.desc: Test function of ActivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, ActivatePermission_test_1002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin ActivatePermission_test_1002";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt", .mode = 5};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_MODE));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*upmsMock_, Active(_, _)).WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::ActivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::INVALID_MODE);
    GTEST_LOG_(INFO) << "FileShareTest-end ActivatePermission_test_1002";
}

/**
 * @tc.name: ActivatePermission_test_1003
 * @tc.desc: Test function of ActivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, ActivatePermission_test_1003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin ActivatePermission_test_1003";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::PERSISTENCE_FORBIDDEN));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*upmsMock_, Active(_, _)).WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::ActivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::PERSISTENCE_FORBIDDEN);
    GTEST_LOG_(INFO) << "FileShareTest-end ActivatePermission_test_1003";
}

/**
 * @tc.name: ActivatePermission_test_1004
 * @tc.desc: Test function of ActivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, ActivatePermission_test_1004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin ActivatePermission_test_1004";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_MODE));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*upmsMock_, Active(_, _)).WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::ActivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::INVALID_MODE);
    GTEST_LOG_(INFO) << "FileShareTest-end ActivatePermission_test_1004";
}

/**
 * @tc.name: DeactivatePermission_test_1000
 * @tc.desc: Test function of DeactivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, DeactivatePermission_test_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin DeactivatePermission_test_1000";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, StopAccessingPolicy(_, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::DeactivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    GTEST_LOG_(INFO) << "FileShareTest-end DeactivatePermission_test_1000";
}
/**
 * @tc.name: DeactivatePermission_test_1001
 * @tc.desc: Test function of DeactivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, DeactivatePermission_test_1001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin DeactivatePermission_test_1001";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_PATH));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, StopAccessingPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::DeactivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::INVALID_PATH);
    GTEST_LOG_(INFO) << "FileShareTest-end DeactivatePermission_test_1001";
}

/**
 * @tc.name: DeactivatePermission_test_1002
 * @tc.desc: Test function of DeactivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, DeactivatePermission_test_1002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin DeactivatePermission_test_1002";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt", .mode = 5};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_MODE));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, StopAccessingPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::DeactivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::INVALID_MODE);
    GTEST_LOG_(INFO) << "FileShareTest-end DeactivatePermission_test_1002";
}

/**
 * @tc.name: DeactivatePermission_test_1003
 * @tc.desc: Test function of DeactivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, DeactivatePermission_test_1003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin DeactivatePermission_test_1003";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::PERSISTENCE_FORBIDDEN));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, StopAccessingPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::DeactivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::PERSISTENCE_FORBIDDEN);
    GTEST_LOG_(INFO) << "FileShareTest-end DeactivatePermission_test_1003";
}

/**
 * @tc.name: DeactivatePermission_test_1004
 * @tc.desc: Test function of DeactivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, DeactivatePermission_test_1004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin DeactivatePermission_test_1004";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_MODE));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, StopAccessingPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::DeactivatePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::INVALID_MODE);
    GTEST_LOG_(INFO) << "FileShareTest-end DeactivatePermission_test_1004";
}

/**
 * @tc.name: RevokePermission_test_1000
 * @tc.desc: Test function of RevokePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, RevokePermission_test_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin RevokePermission_test_1000";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, UnPersistPolicy(_, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    int32_t ret = FilePermission::RevokePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    GTEST_LOG_(INFO) << "FileShareTest-end RevokePermission_test_1000";
}
/**
 * @tc.name: RevokePermission_test_1001
 * @tc.desc: Test function of RevokePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, RevokePermission_test_1001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin RevokePermission_test_1001";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_PATH));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, UnPersistPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::RevokePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::INVALID_PATH);
    GTEST_LOG_(INFO) << "FileShareTest-end RevokePermission_test_1001";
}

/**
 * @tc.name: RevokePermission_test_1002
 * @tc.desc: Test function of RevokePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, RevokePermission_test_1002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin RevokePermission_test_1002";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt", .mode = 5};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_MODE));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, UnPersistPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::RevokePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::INVALID_MODE);
    GTEST_LOG_(INFO) << "FileShareTest-end RevokePermission_test_1002";
}

/**
 * @tc.name: RevokePermission_test_1003
 * @tc.desc: Test function of RevokePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, RevokePermission_test_1003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin RevokePermission_test_1003";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::PERSISTENCE_FORBIDDEN));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, UnPersistPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::RevokePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::PERSISTENCE_FORBIDDEN);
    GTEST_LOG_(INFO) << "FileShareTest-end RevokePermission_test_1003";
}

/**
 * @tc.name: RevokePermission_test_1004
 * @tc.desc: Test function of RevokePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, RevokePermission_test_1004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin RevokePermission_test_1004";
    UriPolicyInfo infoTestA = {.uri = "file://" + BUNDLE_NAME_A + "/storage/PersistPermission01.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<UriPolicyInfo> uriPolicies;
    uriPolicies.emplace_back(infoTestA);
    deque<struct PolicyErrorResult> errorResults;
    vector<uint32_t> resultCodes;
    resultCodes.emplace_back(static_cast<uint32_t>(PolicyErrorCode::INVALID_MODE));
    EXPECT_CALL(*funcMock, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sandboxMock_, UnPersistPolicy(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(resultCodes), Return(SANDBOX_MANAGER_OK)));
    int32_t ret = FilePermission::RevokePermission(uriPolicies, errorResults);
    EXPECT_EQ(ret, EPERM);
    EXPECT_EQ(errorResults[0].code, PolicyErrorCode::INVALID_MODE);
    GTEST_LOG_(INFO) << "FileShareTest-end RevokePermission_test_1004";
}

/**
 * @tc.name: CheckPathPermission_test_1000
 * @tc.desc: Test function of CheckPathPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, CheckPathPermission_test_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin CheckPathPermission_test_1000";
    PathPolicyInfo infoTestA = {.path = "/storage/Users/currentUser/Documents/1.txt",
                               .mode = OperationMode::READ_MODE | OperationMode::WRITE_MODE};
    std::vector<PathPolicyInfo> pathPolicies;
    pathPolicies.emplace_back(infoTestA);
    vector<bool> errorResults;
    EXPECT_CALL(*sandboxMock_, CheckPersistPolicy(_, _, _)).WillOnce(Return(SANDBOX_MANAGER_OK));
    uint32_t tokenId = 537688848;
    int32_t policyType = PERSISTENT_TYPE;
    int32_t ret = FilePermission::CheckPathPermission(tokenId, pathPolicies, policyType, errorResults);
    EXPECT_EQ(ret, 0);

    ret = FilePermission::CheckPathPermission(tokenId, pathPolicies, 2, errorResults);
    EXPECT_EQ(ret, 401);
    GTEST_LOG_(INFO) << "FileShareTest-end CheckPathPermission_test_1000";
}
#endif
} // namespace AppFileService
} // namespace OHOS