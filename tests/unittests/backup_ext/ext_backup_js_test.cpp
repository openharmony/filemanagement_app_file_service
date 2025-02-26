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

#include "ability_info.h"
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

const int ARG_INDEX_FIRST = 1;
const int ARG_INDEX_SECOND = 2;
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
    MOCK_METHOD(void, DumpHeapSnapshot, (uint32_t tid, bool isFullGC, bool isBinary));
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
        sptr<AppExecFwk::AbilityInfo> info = sptr(new AppExecFwk::AbilityInfo());
        info->srcEntrance = "";
        auto ret = GetSrcPath(*info);
        EXPECT_TRUE(ret.empty());

        info->srcEntrance = "test";
        ret = GetSrcPath(*info);
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
        napi_value exception;
        EXPECT_CALL(*napiMock, napi_get_and_clear_last_exception(_, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = DealNapiException(env, exception, exceptionInfo);
        EXPECT_EQ(ret, napi_invalid_arg);

        EXPECT_CALL(*napiMock, napi_get_and_clear_last_exception(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = DealNapiException(env, exception, exceptionInfo);
        EXPECT_EQ(ret, napi_invalid_arg);

        EXPECT_CALL(*napiMock, napi_get_and_clear_last_exception(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok));
        ret = DealNapiException(env, exception, exceptionInfo);
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

        g_extBackupCount = 0;
        auto ret = PromiseCallback(env, info);
        EXPECT_TRUE(ret == nullptr);

        g_extBackupCount = 1;
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = PromiseCallback(env, info);
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
        EXPECT_CALL(*napiMock, napi_fatal_exception(_, _)).WillOnce(Return(napi_invalid_arg));
        ret = PromiseCatchCallback(env, info);
        EXPECT_TRUE(ret == nullptr);

        g_extBackupCount = 0;
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(&callback), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_fatal_exception(_, _)).WillOnce(Return(napi_ok));
        ret = PromiseCatchCallback(env, info);
        EXPECT_TRUE(ret == nullptr);

        g_extBackupCount = 1;
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(&callback), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_fatal_exception(_, _)).WillOnce(Return(napi_ok));
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

        g_extBackupCount = 0;
        auto ret = PromiseCallbackEx(env, info);
        EXPECT_TRUE(ret == nullptr);

        g_extBackupCount = 1;
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        ret = PromiseCallbackEx(env, info);
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
        EXPECT_CALL(*napiMock, napi_fatal_exception(_, _)).WillOnce(Return(napi_invalid_arg));
        ret = PromiseCatchCallbackEx(env, info);
        EXPECT_TRUE(ret == nullptr);

        g_extBackupCount = 0;
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(&callback), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_fatal_exception(_, _)).WillOnce(Return(napi_ok));
        ret = PromiseCatchCallbackEx(env, info);
        EXPECT_TRUE(ret == nullptr);

        g_extBackupCount = 1;
        EXPECT_CALL(*napiMock, napi_get_cb_info(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(&callback), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_fatal_exception(_, _)).WillOnce(Return(napi_ok));
        ret = PromiseCatchCallbackEx(env, info);
        EXPECT_TRUE(ret == nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by PromiseCatchCallbackEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_PromiseCatchCallbackEx_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_CheckPromise_0100
 * @tc.name: SUB_backup_ext_js_CheckPromise_0100
 * @tc.desc: 测试 CheckPromise 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CheckPromise_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CheckPromise_0100";
    try {
        napi_env env = nullptr;
        auto ret = CheckPromise(env, nullptr);
        EXPECT_FALSE(ret);

        int value = 0;
        EXPECT_CALL(*napiMock, napi_is_promise(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = CheckPromise(env, reinterpret_cast<napi_value>(&value));
        EXPECT_FALSE(ret);

        EXPECT_CALL(*napiMock, napi_is_promise(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        ret = CheckPromise(env, reinterpret_cast<napi_value>(&value));
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CheckPromise.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CheckPromise_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_CallCatchPromise_0100
 * @tc.name: SUB_backup_ext_js_CallCatchPromise_0100
 * @tc.desc: 测试 CallCatchPromise 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallCatchPromise_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallCatchPromise_0100";
    try {
        napi_value result = nullptr;
        struct CallbackInfo *callbackInfo = nullptr;
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallCatchPromise(*jsRuntime, result, callbackInfo));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallCatchPromise(*jsRuntime, result, callbackInfo));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallCatchPromise(*jsRuntime, result, callbackInfo));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallCatchPromise(*jsRuntime, result, callbackInfo));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallCatchPromise.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallCatchPromise_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_CallCatchPromise_0200
 * @tc.name: SUB_backup_ext_js_CallCatchPromise_0200
 * @tc.desc: 测试 CallCatchPromise 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallCatchPromise_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallCatchPromise_0200";
    try {
        napi_value result = nullptr;
        struct CallbackInfo *callbackInfo = nullptr;
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallCatchPromise(*jsRuntime, result, callbackInfo));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_TRUE(CallCatchPromise(*jsRuntime, result, callbackInfo));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallCatchPromise.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallCatchPromise_0200";
}

/**
 * @tc.number: SUB_backup_ext_js_CallPromise_0100
 * @tc.name: SUB_backup_ext_js_CallPromise_0100
 * @tc.desc: 测试 CallPromise 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallPromise_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallPromise_0100";
    try {
        napi_value result = nullptr;
        struct CallbackInfo *callbackInfo = nullptr;
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromise(*jsRuntime, result, callbackInfo));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromise(*jsRuntime, result, callbackInfo));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromise(*jsRuntime, result, callbackInfo));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromise(*jsRuntime, result, callbackInfo));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromise(*jsRuntime, result, callbackInfo));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallPromise.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallPromise_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_CallPromise_0200
 * @tc.name: SUB_backup_ext_js_CallPromise_0200
 * @tc.desc: 测试 CallPromise 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallPromise_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallPromise_0200";
    try {
        napi_value result = nullptr;
        struct CallbackInfo *callbackInfo = nullptr;
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromise(*jsRuntime, result, callbackInfo));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_TRUE(CallPromise(*jsRuntime, result, callbackInfo));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallPromise.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallPromise_0200";
}

/**
 * @tc.number: SUB_backup_ext_js_CallCatchPromiseEx_0100
 * @tc.name: SUB_backup_ext_js_CallCatchPromiseEx_0100
 * @tc.desc: 测试 CallCatchPromiseEx 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallCatchPromiseEx_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallCatchPromiseEx_0100";
    try {
        napi_value result = nullptr;
        struct CallbackInfoEx *callbackInfoEx = nullptr;
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallCatchPromiseEx(*jsRuntime, result, callbackInfoEx));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallCatchPromiseEx(*jsRuntime, result, callbackInfoEx));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallCatchPromiseEx(*jsRuntime, result, callbackInfoEx));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallCatchPromiseEx(*jsRuntime, result, callbackInfoEx));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallCatchPromiseEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallCatchPromiseEx_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_CallCatchPromiseEx_0200
 * @tc.name: SUB_backup_ext_js_CallCatchPromiseEx_0200
 * @tc.desc: 测试 CallCatchPromiseEx 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallCatchPromiseEx_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallCatchPromiseEx_0200";
    try {
        napi_value result = nullptr;
        struct CallbackInfoEx *callbackInfoEx = nullptr;
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallCatchPromiseEx(*jsRuntime, result, callbackInfoEx));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_TRUE(CallCatchPromiseEx(*jsRuntime, result, callbackInfoEx));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallCatchPromiseEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallCatchPromiseEx_0200";
}

/**
 * @tc.number: SUB_backup_ext_js_CallPromiseEx_0100
 * @tc.name: SUB_backup_ext_js_CallPromiseEx_0100
 * @tc.desc: 测试 CallPromiseEx 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallPromiseEx_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallPromiseEx_0100";
    try {
        napi_value result = nullptr;
        struct CallbackInfoEx *callbackInfoEx = nullptr;
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromiseEx(*jsRuntime, result, callbackInfoEx));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromiseEx(*jsRuntime, result, callbackInfoEx));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromiseEx(*jsRuntime, result, callbackInfoEx));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromiseEx(*jsRuntime, result, callbackInfoEx));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallPromiseEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallPromiseEx_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_CallPromiseEx_0200
 * @tc.name: SUB_backup_ext_js_CallPromiseEx_0200
 * @tc.desc: 测试 CallPromiseEx 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallPromiseEx_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallPromiseEx_0200";
    try {
        napi_value result = nullptr;
        struct CallbackInfoEx *callbackInfoEx = nullptr;
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _));
        EXPECT_FALSE(CallPromiseEx(*jsRuntime, result, callbackInfoEx));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_TRUE(CallPromiseEx(*jsRuntime, result, callbackInfoEx));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallPromiseEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallPromiseEx_0200";
}

/**
 * @tc.number: SUB_backup_ext_js_CallPromiseEx_0300
 * @tc.name: SUB_backup_ext_js_CallPromiseEx_0300
 * @tc.desc: 测试 CallPromiseEx 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallPromiseEx_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallPromiseEx_0300";
    try {
        napi_value result = nullptr;
        struct CallbackInfoBackup *callbackInfoBackup = nullptr;
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromiseEx(*jsRuntime, result, callbackInfoBackup));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromiseEx(*jsRuntime, result, callbackInfoBackup));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromiseEx(*jsRuntime, result, callbackInfoBackup));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromiseEx(*jsRuntime, result, callbackInfoBackup));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallPromiseEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallPromiseEx_0300";
}

/**
 * @tc.number: SUB_backup_ext_js_CallPromiseEx_0400
 * @tc.name: SUB_backup_ext_js_CallPromiseEx_0400
 * @tc.desc: 测试 CallPromiseEx 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallPromiseEx_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallPromiseEx_0400";
    try {
        napi_value result = nullptr;
        struct CallbackInfoBackup *callbackInfoBackup = nullptr;
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(CallPromiseEx(*jsRuntime, result, callbackInfoBackup));

        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_callable(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_TRUE(CallPromiseEx(*jsRuntime, result, callbackInfoBackup));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallPromiseEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallPromiseEx_0400";
}

/**
 * @tc.number: SUB_backup_ext_js_AttachBackupExtensionContext_0100
 * @tc.name: SUB_backup_ext_js_AttachBackupExtensionContext_0100
 * @tc.desc: 测试 AttachBackupExtensionContext 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_AttachBackupExtensionContext_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_AttachBackupExtensionContext_0100";
    try {
        auto ret = AttachBackupExtensionContext(nullptr, nullptr, nullptr);
        EXPECT_TRUE(ret == nullptr);

        auto value = make_shared<ExtBackupContext>();
        ret = AttachBackupExtensionContext(nullptr, value.get(), nullptr);
        EXPECT_TRUE(ret == nullptr);

        int env = 0;
        weak_ptr<ExtBackupContext> ptr = value;
        EXPECT_CALL(*extBackupMock, CreateExtBackupJsContext(_, _)).WillOnce(Return(nullptr));
        ret = AttachBackupExtensionContext(reinterpret_cast<napi_env>(&env), reinterpret_cast<void*>(&ptr), nullptr);
        EXPECT_TRUE(ret == nullptr);

        EXPECT_CALL(*extBackupMock, CreateExtBackupJsContext(_, _))
            .WillOnce(Return(reinterpret_cast<napi_value>(&env)));
        EXPECT_CALL(*extBackupMock, LoadSystemModuleByEngine(_, _, _, _)).WillOnce(Return(nullptr));
        ret = AttachBackupExtensionContext(reinterpret_cast<napi_env>(&env), reinterpret_cast<void*>(&ptr), nullptr);
        EXPECT_TRUE(ret == nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by AttachBackupExtensionContext.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_AttachBackupExtensionContext_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_ExportJsContext_0100
 * @tc.name: SUB_backup_ext_js_ExportJsContext_0100
 * @tc.desc: 测试 ExportJsContext 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_ExportJsContext_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_ExportJsContext_0100";
    try {
        extBackupJs->jsObj_ = nullptr;
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        extBackupJs->ExportJsContext();
        EXPECT_TRUE(true);

        extBackupJs->jsObj_ = make_unique<NativeReferenceMock>();
        auto refMock = static_cast<NativeReferenceMock*>(extBackupJs->jsObj_.get());
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*refMock, GetNapiValue()).WillOnce(Return(nullptr));
        extBackupJs->ExportJsContext();
        EXPECT_TRUE(extBackupJs->jsObj_ != nullptr);

        int value = 0;
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*refMock, GetNapiValue()).WillOnce(Return(reinterpret_cast<napi_value>(&value)));
        extBackupJs->ExportJsContext();
        EXPECT_TRUE(extBackupJs->jsObj_ != nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by ExportJsContext.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_ExportJsContext_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_CallJsMethod_0100
 * @tc.name: SUB_backup_ext_js_CallJsMethod_0100
 * @tc.desc: 测试 CallJsMethod 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallJsMethod_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallJsMethod_0100";
    try {
        extBackupJs->jsObj_ = make_unique<NativeReferenceMock>();
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->CallJsMethod("", *jsRuntime, extBackupJs->jsObj_.get(), nullptr, nullptr);
        EXPECT_EQ(ret, EINVAL);

        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->CallJsMethod("", *jsRuntime, extBackupJs->jsObj_.get(), nullptr, nullptr);
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallJsMethod.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallJsMethod_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_DoCallJsMethod_0100
 * @tc.name: SUB_backup_ext_js_DoCallJsMethod_0100
 * @tc.desc: 测试 DoCallJsMethod 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_DoCallJsMethod_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_DoCallJsMethod_0100";
    try {
        string funcName = "";
        InputArgsParser argParserIn = {};
        ResultValueParser retParserIn = {};
        auto param = make_shared<CallJsParam>(funcName, nullptr, nullptr, argParserIn, retParserIn);
        auto ret = DoCallJsMethod(param.get());
        EXPECT_EQ(ret, EINVAL);

        param->jsRuntime = jsRuntime.get();
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        ret = DoCallJsMethod(param.get());
        EXPECT_EQ(ret, EINVAL);

        int scope = 0;
        param->argParser = [](napi_env, std::vector<napi_value> &){ return false; };
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(
            DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_handle_scope>(&scope)), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        ret = DoCallJsMethod(param.get());
        EXPECT_EQ(ret, EINVAL);

        auto ref = make_shared<NativeReferenceMock>();
        param->argParser = [](napi_env, std::vector<napi_value> &){ return true; };
        param->jsObj = ref.get();
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(
            DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_handle_scope>(&scope)), Return(napi_ok)));
        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        ret = DoCallJsMethod(param.get());
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by DoCallJsMethod.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_DoCallJsMethod_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_DoCallJsMethod_0200
 * @tc.name: SUB_backup_ext_js_DoCallJsMethod_0200
 * @tc.desc: 测试 DoCallJsMethod 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_DoCallJsMethod_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_DoCallJsMethod_0200";
    try {
        string funcName = "";
        InputArgsParser argParserIn = {};
        ResultValueParser retParserIn = {};
        auto param = make_shared<CallJsParam>(funcName, nullptr, nullptr, argParserIn, retParserIn);
        auto ref = make_shared<NativeReferenceMock>();
        param->argParser = nullptr;
        param->retParser = nullptr;
        param->jsObj = ref.get();

        int scope = 0;
        napi_value value = nullptr;
        param->jsRuntime = jsRuntime.get();
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(
            DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_handle_scope>(&scope)), Return(napi_ok)));
        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(reinterpret_cast<napi_value>(&value)));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        auto ret = DoCallJsMethod(param.get());
        EXPECT_EQ(ret, EINVAL);

        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(
            DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_handle_scope>(&scope)), Return(napi_ok)));
        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(reinterpret_cast<napi_value>(&value)));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        ret = DoCallJsMethod(param.get());
        EXPECT_EQ(ret, EINVAL);

        param->retParser = [](napi_env, napi_value){ return false; };
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(
            DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_handle_scope>(&scope)), Return(napi_ok)));
        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(reinterpret_cast<napi_value>(&value)));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        ret = DoCallJsMethod(param.get());
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by DoCallJsMethod.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_DoCallJsMethod_0200";
}

/**
 * @tc.number: SUB_backup_ext_js_DoCallJsMethod_0300
 * @tc.name: SUB_backup_ext_js_DoCallJsMethod_0300
 * @tc.desc: 测试 DoCallJsMethod 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_DoCallJsMethod_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_DoCallJsMethod_0300";
    try {
        string funcName = "";
        InputArgsParser argParserIn = {};
        ResultValueParser retParserIn = {};
        auto param = make_shared<CallJsParam>(funcName, nullptr, nullptr, argParserIn, retParserIn);
        auto ref = make_shared<NativeReferenceMock>();
        param->argParser = nullptr;
        param->retParser = nullptr;
        param->jsObj = ref.get();

        int scope = 0;
        napi_value value = nullptr;
        param->jsRuntime = jsRuntime.get();
        param->retParser = [](napi_env, napi_value){ return true; };
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(
            DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_handle_scope>(&scope)), Return(napi_ok)));
        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(reinterpret_cast<napi_value>(&value)));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        auto ret = DoCallJsMethod(param.get());
        EXPECT_EQ(ret, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by DoCallJsMethod.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_DoCallJsMethod_0300";
}

/**
 * @tc.number: SUB_backup_ext_js_InvokeAppExtMethod_0100
 * @tc.name: SUB_backup_ext_js_InvokeAppExtMethod_0100
 * @tc.desc: 测试 InvokeAppExtMethod 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_InvokeAppExtMethod_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_InvokeAppExtMethod_0100";
    try {
        ErrCode errCode = BError(BError::Codes::OK);
        string result = "";
        auto ret = extBackupJs->InvokeAppExtMethod(errCode, result);
        EXPECT_EQ(ret, ERR_OK);

        result = "test";
        ret = extBackupJs->InvokeAppExtMethod(errCode, result);
        EXPECT_EQ(ret, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by InvokeAppExtMethod.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_InvokeAppExtMethod_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_InvokeAppExtMethod_0200
 * @tc.name: SUB_backup_ext_js_InvokeAppExtMethod_0200
 * @tc.desc: 测试 InvokeAppExtMethod 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_InvokeAppExtMethod_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_InvokeAppExtMethod_0200";
    try {
        ErrCode errCode = BError(BError::Codes::EXT_INVAL_ARG);
        string result = "";
        auto ret = extBackupJs->InvokeAppExtMethod(errCode, result);
        EXPECT_EQ(ret, ERR_OK);

        result = "test";
        ret = extBackupJs->InvokeAppExtMethod(errCode, result);
        EXPECT_EQ(ret, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by InvokeAppExtMethod.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_InvokeAppExtMethod_0200";
}

/**
 * @tc.number: SUB_backup_ext_js_CallJsOnBackupEx_0100
 * @tc.name: SUB_backup_ext_js_CallJsOnBackupEx_0100
 * @tc.desc: 测试 CallJsOnBackupEx 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallJsOnBackupEx_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallJsOnBackupEx_0100";
    try {
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->CallJsOnBackupEx();
        EXPECT_EQ(ret, EINVAL);

        extBackupJs->callbackInfoEx_ = std::make_shared<CallbackInfoEx>([](ErrCode, std::string){});
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->CallJsOnBackupEx();
        EXPECT_EQ(ret, EINVAL);

        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_get_and_clear_last_exception(_, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->CallJsOnBackupEx();
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallJsOnBackupEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallJsOnBackupEx_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_CallJsOnBackupEx_0200
 * @tc.name: SUB_backup_ext_js_CallJsOnBackupEx_0200
 * @tc.desc: 测试 CallJsOnBackupEx 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallJsOnBackupEx_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallJsOnBackupEx_0200";
    try {
        extBackupJs->callbackInfoEx_ = std::make_shared<CallbackInfoEx>([](ErrCode, std::string){});
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_promise(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->CallJsOnBackupEx();
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallJsOnBackupEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallJsOnBackupEx_0200";
}

/**
 * @tc.number: SUB_backup_ext_js_CallJsOnBackup_0100
 * @tc.name: SUB_backup_ext_js_CallJsOnBackup_0100
 * @tc.desc: 测试 CallJsOnBackup 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallJsOnBackup_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallJsOnBackup_0100";
    try {
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->CallJsOnBackup();
        EXPECT_EQ(ret, EINVAL);

        extBackupJs->callbackInfo_ = std::make_shared<CallbackInfo>([](ErrCode, std::string){});
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->CallJsOnBackup();
        EXPECT_EQ(ret, EINVAL);

        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_get_and_clear_last_exception(_, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->CallJsOnBackup();
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallJsOnBackup.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallJsOnBackup_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_CallJsOnBackup_0200
 * @tc.name: SUB_backup_ext_js_CallJsOnBackup_0200
 * @tc.desc: 测试 CallJsOnBackup 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallJsOnBackup_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallJsOnBackup_0200";
    try {
        extBackupJs->callbackInfo_ = std::make_shared<CallbackInfo>([](ErrCode, std::string){});
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_promise(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->CallJsOnBackup();
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallJsOnBackup.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallJsOnBackup_0200";
}

/**
 * @tc.number: SUB_backup_ext_js_CallJSRestoreEx_0100
 * @tc.name: SUB_backup_ext_js_CallJSRestoreEx_0100
 * @tc.desc: 测试 CallJSRestoreEx 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallJSRestoreEx_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallJSRestoreEx_0100";
    try {
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->CallJSRestoreEx();
        EXPECT_EQ(ret, EINVAL);

        extBackupJs->callbackInfoEx_ = std::make_shared<CallbackInfoEx>([](ErrCode, std::string){});
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->CallJSRestoreEx();
        EXPECT_EQ(ret, EINVAL);

        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_get_and_clear_last_exception(_, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->CallJSRestoreEx();
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallJSRestoreEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallJSRestoreEx_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_CallJSRestoreEx_0200
 * @tc.name: SUB_backup_ext_js_CallJSRestoreEx_0200
 * @tc.desc: 测试 CallJSRestoreEx 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallJSRestoreEx_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallJSRestoreEx_0200";
    try {
        extBackupJs->callbackInfoEx_ = std::make_shared<CallbackInfoEx>([](ErrCode, std::string){});
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_promise(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->CallJSRestoreEx();
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallJSRestoreEx.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallJSRestoreEx_0200";
}

/**
 * @tc.number: SUB_backup_ext_js_CallJSRestore_0100
 * @tc.name: SUB_backup_ext_js_CallJSRestore_0100
 * @tc.desc: 测试 CallJSRestore 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallJSRestore_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallJSRestore_0100";
    try {
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->CallJSRestore();
        EXPECT_EQ(ret, EINVAL);

        extBackupJs->callbackInfo_ = std::make_shared<CallbackInfo>([](ErrCode, std::string){});
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->CallJSRestore();
        EXPECT_EQ(ret, EINVAL);

        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_get_and_clear_last_exception(_, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->CallJSRestore();
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallJSRestore.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallJSRestore_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_CallJSRestore_0200
 * @tc.name: SUB_backup_ext_js_CallJSRestore_0200
 * @tc.desc: 测试 CallJSRestore 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_CallJSRestore_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_CallJSRestore_0200";
    try {
        extBackupJs->callbackInfo_ = std::make_shared<CallbackInfo>([](ErrCode, std::string){});
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_promise(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->CallJSRestore();
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by CallJSRestore.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_CallJSRestore_0200";
}

/**
 * @tc.number: SUB_backup_ext_js_GetBackupInfo_0100
 * @tc.name: SUB_backup_ext_js_GetBackupInfo_0100
 * @tc.desc: 测试 GetBackupInfo 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_GetBackupInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_GetBackupInfo_0100";
    try {
        extBackupJs->jsObj_ = make_unique<NativeReferenceMock>();
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->GetBackupInfo([](ErrCode, std::string){});
        EXPECT_EQ(ret, EINVAL);

        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->GetBackupInfo([](ErrCode, std::string){});
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by GetBackupInfo.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_GetBackupInfo_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_GetBackupInfo_0200
 * @tc.name: SUB_backup_ext_js_GetBackupInfo_0200
 * @tc.desc: 测试 GetBackupInfo 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_GetBackupInfo_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_GetBackupInfo_0200";
    try {
        extBackupJs->jsObj_ = make_unique<NativeReferenceMock>();
        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->GetBackupInfo([](ErrCode, std::string){});
        EXPECT_EQ(ret, EINVAL);

        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_promise(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(true), Return(napi_ok)));
        EXPECT_CALL(*napiMock, napi_open_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_close_handle_scope(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->GetBackupInfo([](ErrCode, std::string){});
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by GetBackupInfo.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_GetBackupInfo_0200";
}

/**
 * @tc.number: SUB_backup_ext_js_OnProcess_0100
 * @tc.name: SUB_backup_ext_js_OnProcess_0100
 * @tc.desc: 测试 OnProcess 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_OnProcess_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_OnProcess_0100";
    try {
        extBackupJs->jsObj_ = make_unique<NativeReferenceMock>();
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->OnProcess([](ErrCode, std::string){});
        EXPECT_EQ(ret, EINVAL);

        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->OnProcess([](ErrCode, std::string){});
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by OnProcess.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_OnProcess_0100";
}

/**
 * @tc.number: SUB_backup_ext_js_OnProcess_0200
 * @tc.name: SUB_backup_ext_js_OnProcess_0200
 * @tc.desc: 测试 OnProcess 各个分支成功与失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAFBOS
 */
HWTEST_F(ExtBackupJsTest, SUB_backup_ext_js_OnProcess_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtBackupJsTest-begin SUB_backup_ext_js_OnProcess_0200";
    try {
        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
        .WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto ret = extBackupJs->OnProcess([](ErrCode, std::string){});
        EXPECT_EQ(ret, EINVAL);

        EXPECT_CALL(*extBackupMock, GetNapiEnv()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*napiMock, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_is_exception_pending(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*napiMock, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = extBackupJs->OnProcess([](ErrCode, std::string){});
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtBackupJsTest-an exception occurred by OnProcess.";
    }
    GTEST_LOG_(INFO) << "ExtBackupJsTest-end SUB_backup_ext_js_OnProcess_0200";
}
} // namespace OHOS::FileManagement::Backup