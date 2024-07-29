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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ext_backup_mock.h"
#include "napi_mock.h"
#include "native_reference_mock.h"

#include "ext_backup_js.cpp"

std::unique_ptr<NativeReference> OHOS::AbilityRuntime::JsRuntime::LoadSystemModuleByEngine(napi_env env,
    const std::string& moduleName, const napi_value* argv, size_t argc)
{
    return OHOS::FileManagement::Backup::BExtBackup::extBackup->LoadSystemModuleByEngine(env, moduleName, argv, argc);
}

napi_env OHOS::AbilityRuntime::JsRuntime::GetNapiEnv() const
{
    return OHOS::FileManagement::Backup::BExtBackup::extBackup->GetNapiEnv();
}

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

const int ARG_INDEX_FOURTH = 4;
const int ARG_INDEX_FIFTH = 5;

napi_value CreateExtBackupJsContext(napi_env env, std::shared_ptr<ExtBackupContext> context)
{
    return BExtBackup::extBackup->CreateExtBackupJsContext(env, context);
}

class JsRuntimeMock : public AbilityRuntime::JsRuntime {
public:
    MOCK_METHOD(void, StartDebugMode, (const DebugOption debugOption));
    MOCK_METHOD(void, DumpHeapSnapshot, (bool isPrivate));
    MOCK_METHOD(void, DumpCpuProfile, ());
    MOCK_METHOD(void, DestroyHeapProfiler, ());
    MOCK_METHOD(void, ForceFullGC, ());
    MOCK_METHOD(void, ForceFullGC, (uint32_t tid));
    MOCK_METHOD(void, DumpHeapSnapshot, (uint32_t tid, bool isFullGC));
    MOCK_METHOD(void, AllowCrossThreadExecution, ());
    MOCK_METHOD(void, GetHeapPrepare, ());
    MOCK_METHOD(void, NotifyApplicationState, (bool isBackground));
    MOCK_METHOD(bool, SuspendVM, (uint32_t tid));
    MOCK_METHOD(void, ResumeVM, (uint32_t tid));
    MOCK_METHOD(void, PreloadSystemModule, (const std::string& moduleName));
    MOCK_METHOD(void, FinishPreload, ());
    MOCK_METHOD(bool, LoadRepairPatch, (const std::string& patchFile, const std::string& baseFile));
    MOCK_METHOD(bool, NotifyHotReloadPage, ());
    MOCK_METHOD(bool, UnLoadRepairPatch, (const std::string& patchFile));
    MOCK_METHOD(void, RegisterQuickFixQueryFunc, ((const std::map<std::string, std::string>&) moduleAndPath));
    MOCK_METHOD(void, StartProfiler, (const DebugOption debugOption));
    MOCK_METHOD(void, DoCleanWorkAfterStageCleaned, ());
    MOCK_METHOD(void, SetModuleLoadChecker, (const std::shared_ptr<ModuleCheckerDelegate>), (const));
    MOCK_METHOD(void, SetDeviceDisconnectCallback, (const std::function<bool()> &cb));
    MOCK_METHOD(void, UpdatePkgContextInfoJson, (std::string moduleName, std::string hapPath, std::string packageName));
};

class ExtBackupJsTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
public:
    static inline unique_ptr<JsRuntimeMock> jsRuntime = nullptr;
    static inline shared_ptr<ExtBackupJs> extBackupJs = nullptr;
    static inline shared_ptr<ExtBackupMock> extBackupMock = nullptr;
    static inline shared_ptr<NapiMock> napiMock = nullptr;
};

void ExtBackupJsTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    jsRuntime = make_unique<JsRuntimeMock>();
    extBackupJs = make_shared<ExtBackupJs>(*jsRuntime);
    extBackupMock = make_shared<ExtBackupMock>();
    ExtBackupMock::extBackup = extBackupMock;
    napiMock = make_shared<NapiMock>();
    Napi::napi = napiMock;
}

void ExtBackupJsTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    extBackupJs = nullptr;
    jsRuntime = nullptr;
    ExtBackupMock::extBackup = nullptr;
    extBackupMock = nullptr;
    Napi::napi = nullptr;
    napiMock = nullptr;
}

/**
 * @tc.number: SUB_backup_ext_js_GetSrcPath_0100
 * @tc.name: SUB_backup_ext_js_GetSrcPath_0100
 * @tc.desc: 测试 GetSrcPath 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_GetSrcPath_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_GetSrcPath_0100";
    try {
        AppExecFwk::AbilityInfo info;
        info.srcEntrance = "";
        auto ret = GetSrcPath(info);
        EXPECT_TRUE(ret.empty());

        info.srcEntrance = "test";
        ret = GetSrcPath(info);
        EXPECT_FALSE(ret.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by GetSrcPath.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_GetSrcPath_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_DealNapiStrValue_0100
 * @tc.name: SUB_backup_ext_js_DealNapiStrValue_0100
 * @tc.desc: 测试 DealNapiStrValue 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_DealNapiStrValue_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_DealNapiStrValue_0100";
    try {
        napi_env env = nullptr;
        napi_value value = 0;
        string result = "";
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = DealNapiStrValue(env, value, result);
        EXPECT_EQ(ret, napi_invalid_arg);

        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok));
        ret = DealNapiStrValue(env, value, result);
        EXPECT_EQ(ret, napi_ok);

        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FOURTH>(1), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FOURTH>(1), Return(napi_invalid_arg)));
        ret = DealNapiStrValue(env, value, result);
        EXPECT_EQ(ret, napi_invalid_arg);

        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FOURTH>(1), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        ret = DealNapiStrValue(env, value, result);
        EXPECT_EQ(ret, napi_ok);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by DealNapiStrValue.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_DealNapiStrValue_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_DealNapiException_0100
 * @tc.name: SUB_backup_ext_js_DealNapiException_0100
 * @tc.desc: 测试 DealNapiException 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_DealNapiException_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_DealNapiException_0100";
    try {
        napi_env env = nullptr;
        string exceptionInfo = "";
        EXPECT_CALL(*napiMock, napi_get_and_clear_last_exception(_, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = DealNapiException(env, exceptionInfo);
        EXPECT_EQ(ret, napi_invalid_arg);

        EXPECT_CALL(*napiMock, napi_get_and_clear_last_exception(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = DealNapiException(env, exceptionInfo);
        EXPECT_EQ(ret, napi_invalid_arg);

        EXPECT_CALL(*napiMock, napi_get_and_clear_last_exception(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok));
        ret = DealNapiException(env, exceptionInfo);
        EXPECT_EQ(ret, napi_ok);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by DealNapiException.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_DealNapiException_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_PromiseCallback_0100
 * @tc.name: SUB_backup_ext_js_PromiseCallback_0100
 * @tc.desc: 测试 PromiseCallback 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_PromiseCallback_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_PromiseCallback_0100";
    try {
        napi_env env = nullptr;
        napi_callback_info info = nullptr;
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = PromiseCallback(env, info);
        EXPECT_TRUE(ret == nullptr);


        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        ret = PromiseCallback(env, info);
        EXPECT_TRUE(ret == nullptr);

        struct CallbackInfo callback([](ErrCode, std::string){});
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(&callback), Return(napi_ok)));
        ret = PromiseCallback(env, info);
        EXPECT_TRUE(ret == nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by PromiseCallback.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_PromiseCallback_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_PromiseCatchCallback_0100
 * @tc.name: SUB_backup_ext_js_PromiseCatchCallback_0100
 * @tc.desc: 测试 PromiseCatchCallback 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_PromiseCatchCallback_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_PromiseCatchCallback_0100";
    try {
        napi_env env = nullptr;
        napi_callback_info info = nullptr;
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = PromiseCatchCallback(env, info);
        EXPECT_TRUE(ret == nullptr);

        struct CallbackInfo callback([](ErrCode, std::string){});
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(&callback), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = PromiseCatchCallback(env, info);
        EXPECT_TRUE(ret == nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by PromiseCatchCallback.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_PromiseCatchCallback_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_PromiseCallbackEx_0100
 * @tc.name: SUB_backup_ext_js_PromiseCallbackEx_0100
 * @tc.desc: 测试 PromiseCallbackEx 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_PromiseCallbackEx_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_PromiseCallbackEx_0100";
    try {
        napi_env env = nullptr;
        napi_callback_info info = nullptr;
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        auto ret = PromiseCallbackEx(env, info);
        EXPECT_TRUE(ret == nullptr);

        struct CallbackInfoEx callback([](ErrCode, std::string){});
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(&callback), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = PromiseCallbackEx(env, info);
        EXPECT_TRUE(ret == nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by PromiseCallbackEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_PromiseCallbackEx_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_PromiseCatchCallbackEx_0100
 * @tc.name: SUB_backup_ext_js_PromiseCatchCallbackEx_0100
 * @tc.desc: 测试 PromiseCatchCallbackEx 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_PromiseCatchCallbackEx_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_PromiseCatchCallbackEx_0100";
    try {
        napi_env env = nullptr;
        napi_callback_info info = nullptr;
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = PromiseCatchCallbackEx(env, info);
        EXPECT_TRUE(ret == nullptr);

        struct CallbackInfoEx callback([](ErrCode, std::string){});
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(&callback), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = PromiseCatchCallbackEx(env, info);
        EXPECT_TRUE(ret == nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by PromiseCatchCallbackEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_PromiseCatchCallbackEx_0100";
}
} // namespace OHOS::FileManagement::Backup