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

#include <gtest/gtest.h>
#include <string>

#include "module_ipc/service.h"
#include "svc_session_manager_throw_mock.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

constexpr int32_t SERVICE_ID = 5203;

class ServiceThrowTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() {};
    void TearDown() {};

    static inline sptr<Service> service = nullptr;
    static inline shared_ptr<SvcSessionManagerMock> sessionMock = nullptr;
};

void ServiceThrowTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    service = sptr<Service>(new Service(SERVICE_ID));
    sessionMock = make_shared<SvcSessionManagerMock>();
    SvcSessionManagerMock::session = sessionMock;
}

void ServiceThrowTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    service = nullptr;
    SvcSessionManagerMock::session = nullptr;
    sessionMock = nullptr;
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
    try {
        EXPECT_NE(service, nullptr);
        EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
            return 0;
        }));
        EXPECT_CALL(*sessionMock, Deactive(_, _)).WillOnce(Return());
        auto ret = service->InitRestoreSession(nullptr);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
            return 0;
        }));
        ret = service->InitRestoreSession(nullptr);
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Invoke([]() {
            throw "未知错误";
            return 0;
        }));
        ret = service->InitRestoreSession(nullptr);
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by InitRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_InitRestoreSession_0100";
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
    try {
        EXPECT_NE(service, nullptr);
        EXPECT_CALL(*sessionMock, SetMemParaCurSize(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, Deactive(_, _)).WillOnce(Return());
        auto ret = service->InitBackupSession(nullptr);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by InitBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_InitBackupSession_0100";
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
    try {
        EXPECT_NE(service, nullptr);
        BFileInfo fileInfo;
        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        auto ret = service->PublishFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        ret = service->PublishFile(fileInfo);
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        ret = service->PublishFile(fileInfo);
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by PublishFile.";
    }
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
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        auto ret = service->AppFileReady(fileName, UniqueFd(-1), 0);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        ret = service->AppFileReady(fileName, UniqueFd(-1), 0);
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        ret = service->AppFileReady(fileName, UniqueFd(-1), 0);
        EXPECT_EQ(ret, EPERM);
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
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        auto ret = service->AppDone(0);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        ret = service->AppDone(0);
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        ret = service->AppDone(0);
        EXPECT_EQ(ret, EPERM);
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
        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
            return IServiceReverse::Scenario::UNDEFINED;
        }));
        auto ret = service->LaunchBackupExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
            return IServiceReverse::Scenario::UNDEFINED;
        }));
        ret = service->LaunchBackupExtension(bundleName);
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Invoke([]() {
            throw "未知错误";
            return IServiceReverse::Scenario::UNDEFINED;
        }));
        ret = service->LaunchBackupExtension(bundleName);
        EXPECT_EQ(ret, EPERM);
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
        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        auto ret = service->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        ret = service->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        ret = service->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, EPERM);
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
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, StopFwkTimer(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
            return true;
        }));
        EXPECT_CALL(*sessionMock, RemoveExtInfo(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
            return IServiceReverse::Scenario::UNDEFINED;
        }));
        service->OnBackupExtensionDied("bundleName");
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by OnBackupExtensionDied.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_OnBackupExtensionDied_0100";
}

/**
 * @tc.number: SUB_Service_throw_ExtStart_0100
 * @tc.name: SUB_Service_throw_ExtStart_0100
 * @tc.desc: 测试 ExtStart 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_ExtStart_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_ExtStart_0100";
    try {
        EXPECT_NE(service, nullptr);
        string bundleName;
        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
            return IServiceReverse::Scenario::UNDEFINED;
        })).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
            return IServiceReverse::Scenario::UNDEFINED;
        }));
        EXPECT_CALL(*sessionMock, RemoveExtInfo(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        })).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        })).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        service->ExtStart(bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by ExtStart.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_ExtStart_0100";
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
            return IServiceReverse::Scenario::UNDEFINED;
        })).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
            return IServiceReverse::Scenario::UNDEFINED;
        }));
        service->ExtConnectFailed(bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
            return IServiceReverse::Scenario::UNDEFINED;
        })).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
            return IServiceReverse::Scenario::UNDEFINED;
        }));
        service->ExtConnectFailed(bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Invoke([]() {
            throw "未知错误";
            return IServiceReverse::Scenario::UNDEFINED;
        })).WillOnce(Invoke([]() {
            throw "未知错误";
            return IServiceReverse::Scenario::UNDEFINED;
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
        EXPECT_CALL(*sessionMock, GetScenario()).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
            return IServiceReverse::Scenario::UNDEFINED;
        }));
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
        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        auto ret = service->GetBackupInfo(bundleName, result);
        EXPECT_EQ(ret, EPERM);
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
        auto ret = service->SADone(errCode, bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, OnBundleFileReady(_, _)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
            return false;
        }));
        ret = service->SADone(errCode, bundleName);
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, OnBundleFileReady(_, _)).WillOnce(Invoke([]() {
            throw "未知错误";
            return false;
        }));
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
        auto ret = service->GetLocalCapabilitiesIncremental(bundleNames);
        EXPECT_EQ(-ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->GetLocalCapabilitiesIncremental(bundleNames);
        EXPECT_EQ(-ret, EPERM);

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->GetLocalCapabilitiesIncremental(bundleNames);
        EXPECT_EQ(-ret, EPERM);
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
    try {
        EXPECT_NE(service, nullptr);
        EXPECT_CALL(*sessionMock, Active(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
            return 0;
        }));
        EXPECT_CALL(*sessionMock, Deactive(_, _)).WillOnce(Return());
        auto ret = service->InitIncrementalBackupSession(nullptr);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by InitIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_InitIncrementalBackupSession_0100";
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
        auto ret = service->AppendBundlesIncrementalBackupSession(bundlesToBackup, infos);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt(_)).WillOnce(Return());
        ret = service->AppendBundlesIncrementalBackupSession(bundlesToBackup, infos);
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
        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        auto ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, EPERM);
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
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        auto ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), 0);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), 0);
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), 0);
        EXPECT_EQ(ret, EPERM);
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
        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        auto ret = service->AppIncrementalDone(0);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, VerifyBundleName(_)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        ret = service->AppIncrementalDone(0);
        EXPECT_EQ(ret, EPERM);
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
        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        auto ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, EPERM);

        EXPECT_CALL(*sessionMock, VerifyCallerAndScenario(_, _)).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by GetIncrementalFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_GetIncrementalFileHandle_0100";
}
} // namespace OHOS::FileManagement::Backup