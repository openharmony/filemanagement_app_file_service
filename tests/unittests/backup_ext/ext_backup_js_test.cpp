/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "ext_backup_js.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

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
    static inline unique_ptr<AbilityRuntime::JsRuntime> jsRuntime = nullptr;
    static inline shared_ptr<ExtBackupJs> extBackupJs = nullptr;
    static inline shared_ptr<ExtBackupMock> extBackupMock = nullptr;
};

void ExtBackupJsTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    jsRuntime = JsRuntimeMock::Create({});
    extBackupJs = make_shared<ExtBackupJs>(*jsRuntime);
    extBackupMock = make_shared<ExtBackupMock>();
    ExtBackupMock::extBackup = extBackupMock;
}

void ExtBackupJsTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    extBackupJs = nullptr;
    jsRuntime = nullptr;
    ExtBackupMock::extBackup = nullptr;
    extBackupMock = nullptr;
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
} // namespace OHOS::FileManagement::Backup