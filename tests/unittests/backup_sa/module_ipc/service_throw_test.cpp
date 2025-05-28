/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <string>

#include "accesstoken_kit_mock.h"
#include "b_jsonutil_mock.h"
#include "backup_para_mock.h"
#include "ipc_skeleton_mock.h"
#include "module_ipc/service.h"
#include "svc_session_manager_throw_mock.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

constexpr int32_t SERVICE_ID = 5203;
constexpr int32_t DEBUG_ID = 100;

class ServiceThrowTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() {};
    void TearDown() {};

    static inline sptr<Service> service = nullptr;
    static inline shared_ptr<BackupParaMock> param = nullptr;
    static inline shared_ptr<SvcSessionManagerMock> sessionMock = nullptr;
    static inline shared_ptr<BJsonUtilMock> jsonUtil = nullptr;
    static inline shared_ptr<IPCSkeletonMock> skeleton = nullptr;
    static inline shared_ptr<AccessTokenKitMock> token = nullptr;
};

void ServiceThrowTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    service = sptr<Service>(new Service(SERVICE_ID));
    sessionMock = make_shared<SvcSessionManagerMock>();
    param = make_shared<BackupParaMock>();
    BackupParaMock::backupPara = param;
    SvcSessionManagerMock::session = sessionMock;
    jsonUtil = make_shared<BJsonUtilMock>();
    BJsonUtilMock::jsonUtil = jsonUtil;
    skeleton = make_shared<IPCSkeletonMock>();
    IPCSkeletonMock::skeleton = skeleton;
    token = make_shared<AccessTokenKitMock>();
    AccessTokenKitMock::token = token;
}

void ServiceThrowTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    service = nullptr;
    BackupParaMock::backupPara = nullptr;
    param = nullptr;
    SvcSessionManagerMock::session = nullptr;
    sessionMock = nullptr;
    BJsonUtilMock::jsonUtil = nullptr;
    jsonUtil = nullptr;
    IPCSkeletonMock::skeleton = nullptr;
    skeleton = nullptr;
    AccessTokenKitMock::token = nullptr;
    token = nullptr;
}

/**
 * @tc.number: SUB_Service_throw_GetLocalCapabilities_0100
 * @tc.name: SUB_Service_throw_GetLocalCapabilities_0100
 * @tc.desc: 测试 GetLocalCapabilities 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_GetLocalCapabilities_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_GetLocalCapabilities_0100";
    try {
        EXPECT_NE(service, nullptr);
        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        auto ret = service->GetLocalCapabilities();
        EXPECT_EQ(-ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->GetLocalCapabilities();
        EXPECT_EQ(-ret, EPERM);

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->GetLocalCapabilities();
        EXPECT_EQ(-ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_GetLocalCapabilities_0100";
}

/**
 * @tc.number: SUB_Service_throw_InitRestoreSession_0100
 * @tc.name: SUB_Service_throw_InitRestoreSession_0100
 * @tc.desc: 测试 InitRestoreSession 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_InitRestoreSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_InitRestoreSession_0100";
    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::DEFAULT_USER_ID));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    sptr<IServiceReverse> reverse = nullptr;
    auto ret = service->InitRestoreSession(reverse);
    EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::SA_REFUSED_ACT)));
    ret = service->InitRestoreSession(reverse);
    EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::OK)));
    ret = service->InitRestoreSession(reverse);
    EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_InitRestoreSession_0100";
}

/**
 * @tc.number: SUB_Service_throw_InitRestoreSession_0200
 * @tc.name: SUB_Service_throw_InitRestoreSession_0200
 * @tc.desc: 测试 InitRestoreSession 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_InitRestoreSession_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_InitRestoreSession_0200";
    std::string errMsg;
    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::DEFAULT_USER_ID));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    sptr<IServiceReverse> reverseNUll = nullptr;
    ErrCode errCode;
    service->InitRestoreSessionWithErrMsg(reverseNUll, errCode, errMsg);
    EXPECT_EQ(errCode, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::SA_REFUSED_ACT)));
    service->InitRestoreSessionWithErrMsg(reverseNUll, errCode, errMsg);
    EXPECT_EQ(errCode, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::SA_SESSION_CONFLICT)));
    service->InitRestoreSessionWithErrMsg(reverseNUll, errCode, errMsg);
    EXPECT_EQ(errCode, BError(BError::Codes::SA_SESSION_CONFLICT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::OK)));
    service->InitRestoreSessionWithErrMsg(reverseNUll, errCode, errMsg);
    EXPECT_EQ(errCode, BError(BError::Codes::OK).GetCode());
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_InitRestoreSession_0200";
}

/**
 * @tc.number: SUB_Service_throw_InitBackupSession_0100
 * @tc.name: SUB_Service_throw_InitBackupSession_0100
 * @tc.desc: 测试 InitBackupSession 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_InitBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_InitBackupSession_0100";
    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::DEFAULT_USER_ID));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    sptr<IServiceReverse> reverse = nullptr;
    auto ret = service->InitBackupSession(reverse);
    EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::SA_REFUSED_ACT)));
    ret = service->InitBackupSession(reverse);
    EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::OK)));
    ret = service->InitBackupSession(reverse);
    EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_InitBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_throw_InitBackupSession_0200
 * @tc.name: SUB_Service_throw_InitBackupSession_0200
 * @tc.desc: 测试 InitBackupSession 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_InitBackupSession_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_InitBackupSession_0200";
    std::string errMsg;
    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::DEFAULT_USER_ID));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    sptr<IServiceReverse> reverseNUll = nullptr;
    ErrCode errCode;
    service->InitBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
    EXPECT_EQ(errCode, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::SA_REFUSED_ACT)));
    service->InitBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
    EXPECT_EQ(errCode, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::SA_SESSION_CONFLICT)));
    service->InitBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
    EXPECT_EQ(errCode, BError(BError::Codes::SA_SESSION_CONFLICT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::OK)));
    service->InitBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
    EXPECT_EQ(errCode, BError(BError::Codes::OK).GetCode());
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_InitBackupSession_0200";
}

/**
 * @tc.number: SUB_Service_throw_AppendBundlesRestoreSession_0100
 * @tc.name: SUB_Service_throw_AppendBundlesRestoreSession_0100
 * @tc.desc: 测试 AppendBundlesRestoreSession 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_AppendBundlesRestoreSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_AppendBundlesRestoreSession_0100";
    try {
        EXPECT_NE(service, nullptr);
        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        auto ret = service->AppendBundlesRestoreSession(UniqueFd(-1), {}, {}, RESTORE_DATA_WAIT_SEND, 0);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->AppendBundlesRestoreSession(UniqueFd(-1), {}, {}, RESTORE_DATA_WAIT_SEND, 0);
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by AppendBundlesRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_AppendBundlesRestoreSession_0100";
}

/**
 * @tc.number: SUB_Service_throw_AppendBundlesRestoreSession_0200
 * @tc.name: SUB_Service_throw_AppendBundlesRestoreSession_0200
 * @tc.desc: 测试 AppendBundlesRestoreSession 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_AppendBundlesRestoreSession_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_AppendBundlesRestoreSession_0200";
    try {
        EXPECT_NE(service, nullptr);
        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        auto ret = service->AppendBundlesRestoreSession(UniqueFd(-1), {}, RESTORE_DATA_WAIT_SEND, 0);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->AppendBundlesRestoreSession(UniqueFd(-1), {}, RESTORE_DATA_WAIT_SEND, 0);
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by AppendBundlesRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_AppendBundlesRestoreSession_0200";
}

/**
 * @tc.number: SUB_Service_throw_AppendBundlesBackupSession_0100
 * @tc.name: SUB_Service_throw_AppendBundlesBackupSession_0100
 * @tc.desc: 测试 AppendBundlesBackupSession 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_AppendBundlesBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_AppendBundlesBackupSession_0100";
    try {
        EXPECT_NE(service, nullptr);
        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        auto ret = service->AppendBundlesBackupSession({});
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->AppendBundlesBackupSession({});
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->AppendBundlesBackupSession({});
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by AppendBundlesBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_AppendBundlesBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_throw_AppendBundlesDetailsBackupSession_0100
 * @tc.name: SUB_Service_throw_AppendBundlesDetailsBackupSession_0100
 * @tc.desc: 测试 AppendBundlesDetailsBackupSession 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_AppendBundlesDetailsBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_AppendBundlesDetailsBackupSession_0100";
    try {
        EXPECT_NE(service, nullptr);
        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        auto ret = service->AppendBundlesDetailsBackupSession({}, {});
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->AppendBundlesDetailsBackupSession({}, {});
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->AppendBundlesDetailsBackupSession({}, {});
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by AppendBundlesDetailsBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_AppendBundlesDetailsBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_throw_PublishFile_0100
 * @tc.name: SUB_Service_throw_PublishFile_0100
 * @tc.desc: 测试 PublishFile 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_PublishFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_PublishFile_0100";
    EXPECT_NE(service, nullptr);
    BFileInfo fileInfo;
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
        .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _))
        .WillOnce(Return(BError(BError::Codes::SDK_MIXED_SCENARIO)));
    auto ret = service->PublishFile(fileInfo);
    EXPECT_EQ(ret, BError(BError::Codes::SDK_MIXED_SCENARIO).GetCode());

    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _))
        .WillOnce(Return(BError(BError(BError::Codes::SA_REFUSED_ACT))));
    ret = service->PublishFile(fileInfo);
    EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _))
        .WillOnce(Return(BError(BError(BError::Codes::OK))));
    EXPECT_CALL(*skeleton, GetCallingTokenID())
        .WillOnce(Return(0))
        .WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
    EXPECT_CALL(*token, VerifyAccessToken(_, _))
        .WillOnce(Return(Security::AccessToken::PermissionState::PERMISSION_DENIED));
    ret = service->PublishFile(fileInfo);
    EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_PublishFile_0100";
}

/**
 * @tc.number: SUB_Service_throw_AppFileReady_0100
 * @tc.name: SUB_Service_throw_AppFileReady_0100
 * @tc.desc: 测试 AppFileReady 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_AppFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_AppFileReady_0100";
    try {
        EXPECT_NE(service, nullptr);
        string fileName;
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillRepeatedly(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        int fd = 1;
        auto ret = service->AppFileReady(fileName, fd, 0);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Return(BError(BError::Codes::SA_REFUSED_ACT)));
        ret = service->AppFileReady(fileName, fd, 0);
        EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by AppFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_AppFileReady_0100";
}

/**
 * @tc.number: SUB_Service_throw_AppDone_0100
 * @tc.name: SUB_Service_throw_AppDone_0100
 * @tc.desc: 测试 AppDone 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_AppDone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_AppDone_0100";
    try {
        EXPECT_NE(service, nullptr);
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillRepeatedly(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        auto ret = service->AppDone(0);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by AppDone.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_AppDone_0100";
}

/**
 * @tc.number: SUB_Service_throw_LaunchBackupExtension_0100
 * @tc.name: SUB_Service_throw_LaunchBackupExtension_0100
 * @tc.desc: 测试 LaunchBackupExtension 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_LaunchBackupExtension_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_LaunchBackupExtension_0100";
    try {
        EXPECT_NE(service, nullptr);
        BundleName bundleName;
        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::UNDEFINED));
        auto ret = service->LaunchBackupExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by LaunchBackupExtension.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_LaunchBackupExtension_0100";
}

/**
 * @tc.number: SUB_Service_throw_GetFileHandle_0100
 * @tc.name: SUB_Service_throw_GetFileHandle_0100
 * @tc.desc: 测试 GetFileHandle 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_GetFileHandle_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_GetFileHandle_0100";
    try {
        EXPECT_NE(service, nullptr);
        string bundleName;
        string fileName;
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(
            Return(BError(BError::Codes::SDK_MIXED_SCENARIO)));
        auto ret = service->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(
            Return(BError(BError::Codes::SA_REFUSED_ACT)));
        ret = service->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_GetFileHandle_0100";
}

/**
 * @tc.number: SUB_Service_throw_OnBackupExtensionDied_0100
 * @tc.name: SUB_Service_throw_OnBackupExtensionDied_0100
 * @tc.desc: 测试 OnBackupExtensionDied 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_OnBackupExtensionDied_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_OnBackupExtensionDied_0100";
    try {
        EXPECT_NE(service, nullptr);
        string bundleName;
        EXPECT_CALL(*sessionMock, GetScenario())
            .WillOnce(Return(IServiceReverseType::Scenario::CLEAN))
            .WillOnce(Return(IServiceReverseType::Scenario::CLEAN));
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        EXPECT_CALL(*sessionMock, StopFwkTimer(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
            return true;
        }));
        EXPECT_CALL(*sessionMock, RemoveExtInfo(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->OnBackupExtensionDied("bundleName");
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by OnBackupExtensionDied.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_OnBackupExtensionDied_0100";
}

/**
 * @tc.number: SUB_Service_throw_ExtConnectFailed_0100
 * @tc.name: SUB_Service_throw_ExtConnectFailed_0100
 * @tc.desc: 测试 ExtConnectFailed 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_ExtConnectFailed_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_ExtConnectFailed_0100";
    try {
        EXPECT_NE(service, nullptr);
        BundleName bundleName;
        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
            return IServiceReverseType::Scenario::UNDEFINED;
        }));
        service->ExtConnectFailed(bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
            return IServiceReverseType::Scenario::UNDEFINED;
        }));
        service->ExtConnectFailed(bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Invoke([]() {
            throw "未知错误";
            return IServiceReverseType::Scenario::UNDEFINED;
        }));
        service->ExtConnectFailed(bundleName, 0);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by ExtConnectFailed.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_ExtConnectFailed_0100";
}

/**
 * @tc.number: SUB_Service_throw_NoticeClientFinish_0100
 * @tc.name: SUB_Service_throw_NoticeClientFinish_0100
 * @tc.desc: 测试 NoticeClientFinish 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_NoticeClientFinish_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_NoticeClientFinish_0100";
    try {
        EXPECT_NE(service, nullptr);
        string bundleName;
        ErrCode errCode = 0;
        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::UNDEFINED));
        EXPECT_CALL(*sessionMock, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->NoticeClientFinish(bundleName, errCode);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by NoticeClientFinish.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_NoticeClientFinish_0100";
}

/**
 * @tc.number: SUB_Service_throw_ClearSessionAndSchedInfo_0100
 * @tc.name: SUB_Service_throw_ClearSessionAndSchedInfo_0100
 * @tc.desc: 测试 ClearSessionAndSchedInfo 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_ClearSessionAndSchedInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_ClearSessionAndSchedInfo_0100";
    try {
        EXPECT_NE(service, nullptr);
        BundleName bundleName;
        EXPECT_CALL(*sessionMock, RemoveExtInfo(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        service->ClearSessionAndSchedInfo(bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*sessionMock, RemoveExtInfo(_)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        service->ClearSessionAndSchedInfo(bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*sessionMock, RemoveExtInfo(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        service->ClearSessionAndSchedInfo(bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by ClearSessionAndSchedInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_ClearSessionAndSchedInfo_0100";
}

/**
 * @tc.number: SUB_Service_throw_GetBackupInfo_0100
 * @tc.name: SUB_Service_throw_GetBackupInfo_0100
 * @tc.desc: 测试 GetBackupInfo 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_GetBackupInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_GetBackupInfo_0100";
    try {
        EXPECT_NE(service, nullptr);
        BundleName bundleName;
        string result;
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::DEFAULT_USER_ID));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        auto ret = service->GetBackupInfo(bundleName, result);
        EXPECT_NE(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by GetBackupInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_GetBackupInfo_0100";
}

/**
 * @tc.number: SUB_Service_throw_UpdateTimer_0100
 * @tc.name: SUB_Service_throw_UpdateTimer_0100
 * @tc.desc: 测试 UpdateTimer 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_UpdateTimer_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_UpdateTimer_0100";
    try {
        EXPECT_NE(service, nullptr);
        BundleName bundleName;
        bool result = false;
        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        auto ret = service->UpdateTimer(bundleName, 0, result);
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by UpdateTimer.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_UpdateTimer_0100";
}

/**
 * @tc.number: SUB_Service_throw_SADone_0100
 * @tc.name: SUB_Service_throw_SADone_0100
 * @tc.desc: 测试 SADone 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_SADone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_SADone_0100";
    try {
        EXPECT_NE(service, nullptr);
        ErrCode errCode = 0;
        string bundleName;
        EXPECT_CALL(*sessionMock, OnBundleFileReady(_, _)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
            return false;
        }));
        EXPECT_CALL(*sessionMock, IsOnAllBundlesFinished()).WillOnce(Return(false));
        auto ret = service->SADone(errCode, bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, OnBundleFileReady(_, _)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
            return false;
        }));
        EXPECT_CALL(*sessionMock, IsOnAllBundlesFinished()).WillOnce(Return(false));
        ret = service->SADone(errCode, bundleName);
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, OnBundleFileReady(_, _)).WillOnce(Invoke([]() {
            throw "未知错误";
            return false;
        }));
        EXPECT_CALL(*sessionMock, IsOnAllBundlesFinished()).WillOnce(Return(false));
        ret = service->SADone(errCode, bundleName);
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by SADone.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_SADone_0100";
}

/**
 * @tc.number: SUB_Service_throw_GetLocalCapabilitiesIncremental_0100
 * @tc.name: SUB_Service_throw_GetLocalCapabilitiesIncremental_0100
 * @tc.desc: 测试 GetLocalCapabilitiesIncremental 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_GetLocalCapabilitiesIncremental_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_GetLocalCapabilitiesIncremental_0100";
    try {
        EXPECT_NE(service, nullptr);
        vector<BIncrementalData> bundleNames;
        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        int fd = -1;
        ErrCode ret = service->GetLocalCapabilitiesIncremental(bundleNames, fd);
        EXPECT_EQ(-fd, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->GetLocalCapabilitiesIncremental(bundleNames, fd);
        EXPECT_EQ(-fd, EPERM);

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->GetLocalCapabilitiesIncremental(bundleNames, fd);
        EXPECT_EQ(-fd, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by GetLocalCapabilitiesIncremental.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_GetLocalCapabilitiesIncremental_0100";
}

/**
 * @tc.number: SUB_Service_throw_GetAppLocalListAndDoIncrementalBackup_0100
 * @tc.name: SUB_Service_throw_GetAppLocalListAndDoIncrementalBackup_0100
 * @tc.desc: 测试 GetAppLocalListAndDoIncrementalBackup 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_GetAppLocalListAndDoIncrementalBackup_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_GetAppLocalListAndDoIncrementalBackup_0100";
    try {
        EXPECT_NE(service, nullptr);
        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        auto ret = service->GetAppLocalListAndDoIncrementalBackup();
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->GetAppLocalListAndDoIncrementalBackup();
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->GetAppLocalListAndDoIncrementalBackup();
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by GetAppLocalListAndDoIncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_GetAppLocalListAndDoIncrementalBackup_0100";
}

/**
 * @tc.number: SUB_Service_throw_InitIncrementalBackupSession_0100
 * @tc.name: SUB_Service_throw_InitIncrementalBackupSession_0100
 * @tc.desc: 测试 InitIncrementalBackupSession 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_InitIncrementalBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_InitIncrementalBackupSession_0100";
    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::DEFAULT_USER_ID));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    sptr<IServiceReverse> reverse = nullptr;
    auto ret = service->InitIncrementalBackupSession(reverse);
    EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)))
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::SA_REFUSED_ACT)));
    EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::UNDEFINED));
    ret = service->InitIncrementalBackupSession(reverse);
    EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::OK)));
    ret = service->InitIncrementalBackupSession(reverse);
    EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_InitIncrementalBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_throw_InitIncrementalBackupSession_0200
 * @tc.name: SUB_Service_throw_InitIncrementalBackupSession_0200
 * @tc.desc: 测试 InitIncrementalBackupSession 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_InitIncrementalBackupSession_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_InitIncrementalBackupSession_0200";
    std::string errMsg;
    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::DEFAULT_USER_ID));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    sptr<IServiceReverse> reverseNUll = nullptr;
    ErrCode errCode;
    service->InitIncrementalBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
    EXPECT_EQ(errCode, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::SA_REFUSED_ACT)));
    service->InitIncrementalBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
    EXPECT_EQ(errCode, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::SA_SESSION_CONFLICT)));
    service->InitIncrementalBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
    EXPECT_EQ(errCode, BError(BError::Codes::SA_SESSION_CONFLICT).GetCode());

    EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
    EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
    EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Return(BError(BError::Codes::OK)));
    service->InitIncrementalBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
    EXPECT_EQ(errCode, BError(BError::Codes::OK).GetCode());
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_InitIncrementalBackupSession_0200";
}

/**
 * @tc.number: SUB_Service_throw_AppendBundlesIncrementalBackupSession_0100
 * @tc.name: SUB_Service_throw_AppendBundlesIncrementalBackupSession_0100
 * @tc.desc: 测试 AppendBundlesIncrementalBackupSession 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_AppendBundlesIncrementalBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_AppendBundlesIncrementalBackupSession_0100";
    try {
        EXPECT_NE(service, nullptr);
        vector<BIncrementalData> bundlesToBackup;
        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        auto ret = service->AppendBundlesIncrementalBackupSession(bundlesToBackup);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->AppendBundlesIncrementalBackupSession(bundlesToBackup);
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by AppendBundlesIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_AppendBundlesIncrementalBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_throw_AppendBundlesIncrementalBackupSession_0200
 * @tc.name: SUB_Service_throw_AppendBundlesIncrementalBackupSession_0200
 * @tc.desc: 测试 AppendBundlesIncrementalBackupSession 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_AppendBundlesIncrementalBackupSession_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_AppendBundlesIncrementalBackupSession_0200";
    try {
        EXPECT_NE(service, nullptr);
        vector<BIncrementalData> bundlesToBackup;
        vector<std::string> infos;
        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        auto ret = service->AppendBundlesIncrementalBackupSessionWithBundleInfos(bundlesToBackup, infos);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->AppendBundlesIncrementalBackupSessionWithBundleInfos(bundlesToBackup, infos);
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by AppendBundlesIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_AppendBundlesIncrementalBackupSession_0200";
}

/**
 * @tc.number: SUB_Service_throw_PublishIncrementalFile_0100
 * @tc.name: SUB_Service_throw_PublishIncrementalFile_0100
 * @tc.desc: 测试 PublishIncrementalFile 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_PublishIncrementalFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_PublishIncrementalFile_0100";
    try {
        EXPECT_NE(service, nullptr);
        BFileInfo fileInfo;
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(
            Return(BError(BError::Codes::SDK_MIXED_SCENARIO)));
        auto ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(
            Return(BError(BError::Codes::SA_REFUSED_ACT)));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by PublishIncrementalFile.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_PublishIncrementalFile_0100";
}

/**
 * @tc.number: SUB_Service_throw_AppIncrementalFileReady_0100
 * @tc.name: SUB_Service_throw_AppIncrementalFileReady_0100
 * @tc.desc: 测试 AppIncrementalFileReady 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_AppIncrementalFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_AppIncrementalFileReady_0100";
    try {
        EXPECT_NE(service, nullptr);
        string fileName;
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillRepeatedly(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        int fd = 1;
        auto ret = service->AppIncrementalFileReady(fileName, fd, fd, 0);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Return(BError(BError::Codes::SA_REFUSED_ACT)));
        ret = service->AppIncrementalFileReady(fileName, fd, fd, 0);
        EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by AppIncrementalFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_AppIncrementalFileReady_0100";
}

/**
 * @tc.number: SUB_Service_throw_AppIncrementalDone_0100
 * @tc.name: SUB_Service_throw_AppIncrementalDone_0100
 * @tc.desc: 测试 AppIncrementalDone 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_AppIncrementalDone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_AppIncrementalDone_0100";
    try {
        EXPECT_NE(service, nullptr);
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillRepeatedly(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        auto ret = service->AppIncrementalDone(0);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Return(BError(BError::Codes::SA_REFUSED_ACT)));
        ret = service->AppIncrementalDone(0);
        EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by AppIncrementalDone.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_AppIncrementalDone_0100";
}

/**
 * @tc.number: SUB_Service_throw_GetIncrementalFileHandle_0100
 * @tc.name: SUB_Service_throw_GetIncrementalFileHandle_0100
 * @tc.desc: 测试 GetIncrementalFileHandle 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_GetIncrementalFileHandle_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_GetIncrementalFileHandle_0100";
    try {
        EXPECT_NE(service, nullptr);
        string bundleName;
        string fileName;

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(
            Return(BError(BError::Codes::SDK_MIXED_SCENARIO)));
        auto ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SDK_MIXED_SCENARIO).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(
            Return(BError(BError::Codes::SA_REFUSED_ACT)));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by GetIncrementalFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_GetIncrementalFileHandle_0100";
}
} // namespace OHOS::FileManagement::Backup