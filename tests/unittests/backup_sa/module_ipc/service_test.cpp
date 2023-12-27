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

#include <fcntl.h>
#include <gtest/gtest.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include "module_ipc/service.h"
#include "service_reverse_mock.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string MANAGE_JSON = "manage.json";
const string FILE_NAME = "1.tar";
constexpr int32_t SERVICE_ID = 5203;
} // namespace

class ServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() {};
    void TearDown() {};

    ErrCode Init(IServiceReverse::Scenario scenario);

    static inline sptr<Service> servicePtr_ = nullptr;
    static inline sptr<ServiceReverseMock> remote_ = nullptr;
};

void ServiceTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    servicePtr_ = sptr<Service>(new Service(SERVICE_ID));
    remote_ = sptr(new ServiceReverseMock());
}

void ServiceTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    servicePtr_ = nullptr;
    remote_ = nullptr;
}

ErrCode ServiceTest::Init(IServiceReverse::Scenario scenario)
{
    vector<string> bundleNames;
    bundleNames.emplace_back(BUNDLE_NAME);
    ErrCode ret = 0;
    if (scenario == IServiceReverse::Scenario::RESTORE) {
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_GT(fd, BError(BError::Codes::OK));
        ret = servicePtr_->InitRestoreSession(remote_);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        ret = servicePtr_->AppendBundlesRestoreSession(move(fd), bundleNames);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        ret = servicePtr_->Finish();
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } else if (scenario == IServiceReverse::Scenario::BACKUP) {
        ret = servicePtr_->InitBackupSession(remote_);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        ret = servicePtr_->AppendBundlesBackupSession(bundleNames);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        ret = servicePtr_->Finish();
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    }
    return ret;
}

/**
 * @tc.number: SUB_Service_GetLocalCapabilities_0100
 * @tc.name: SUB_Service_GetLocalCapabilities_0100
 * @tc.desc: 测试 GetLocalCapabilities 获取本地能力文件
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_GetLocalCapabilities_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetLocalCapabilities_0100";
    try {
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_GT(fd, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetLocalCapabilities_0100";
}

/**
 * @tc.number: SUB_Service_OnStart_0100
 * @tc.name: SUB_Service_OnStart_0100
 * @tc.desc: 测试 OnStart 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_OnStart_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnStart_0100";
    try {
        servicePtr_->OnStart();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnStart_0100";
}

/**
 * @tc.number: SUB_Service_Start_0100
 * @tc.name: SUB_Service_Start_0100
 * @tc.desc: 测试 Start 备份恢复启动
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_Start_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_Start_0100";
    try {
        auto ret = servicePtr_->Start();
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by Start.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_Start_0100";
}

/**
 * @tc.number: SUB_Service_PublishFile_0100
 * @tc.name: SUB_Service_PublishFile_0100
 * @tc.desc: 测试 PublishFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_PublishFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_PublishFile_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        BFileInfo fileInfo {BUNDLE_NAME, FILE_NAME, 0};
        ret = servicePtr_->PublishFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "ServiceTest-PublishFile Branches";
        fileInfo.fileName = "test";
        ret = servicePtr_->PublishFile(fileInfo);
        EXPECT_NE(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by PublishFile.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_PublishFile_0100";
}

/**
 * @tc.number: SUB_Service_AppFileReady_0100
 * @tc.name: SUB_Service_AppFileReady_0100
 * @tc.desc: 测试 AppFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_AppFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppFileReady_0100";
    try {
        string fileName = MANAGE_JSON;
        auto ret = servicePtr_->AppFileReady(fileName, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "ServiceTest-AppFileReady Branches";
        fileName = "test";
        ret = servicePtr_->AppFileReady(fileName, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppFileReady_0100";
}

/**
 * @tc.number: SUB_Service_AppDone_0100
 * @tc.name: SUB_Service_AppDone_0100
 * @tc.desc: 测试 AppDone 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_AppDone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppDone_0100";
    try {
        GTEST_LOG_(INFO) << "SUB_Service_AppDone_0100 RESTORE";
        auto ret = servicePtr_->AppDone(BError(BError::Codes::OK));
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "SUB_Service_AppDone_0100 BACKUP";
        ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "ServiceTest-AppDone Branches";
        ret = servicePtr_->AppDone(1);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "ServiceTest-AppDone Branches End";
        ret = servicePtr_->AppDone(BError(BError::Codes::OK));
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppDone.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppDone_0100";
}

/**
 * @tc.number: SUB_Service_LaunchBackupExtension_0100
 * @tc.name: SUB_Service_LaunchBackupExtension_0100
 * @tc.desc: 测试 LaunchBackupExtension 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_LaunchBackupExtension_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_LaunchBackupExtension_0100";
    try {
        GTEST_LOG_(INFO) << "SUB_Service_LaunchBackupExtension_0100 RESTORE";
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        ret = servicePtr_->LaunchBackupExtension(BUNDLE_NAME);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "SUB_Service_LaunchBackupExtension_0100 BACKUP";
        ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        ret = servicePtr_->LaunchBackupExtension(BUNDLE_NAME);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by LaunchBackupExtension.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_LaunchBackupExtension_0100";
}

/**
 * @tc.number: SUB_Service_GetFileHandle_0100
 * @tc.name: SUB_Service_GetFileHandle_0100
 * @tc.desc: 测试 GetFileHandle 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_GetFileHandle_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetFileHandle_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        string bundleName = BUNDLE_NAME;
        string fileName = FILE_NAME;
        ret = servicePtr_->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetFileHandle_0100";
}

/**
 * @tc.number: SUB_Service_OnBackupExtensionDied_0100
 * @tc.name: SUB_Service_OnBackupExtensionDied_0100
 * @tc.desc: 测试 OnBackupExtensionDied 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_OnBackupExtensionDied_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnBackupExtensionDied_0100";
    try {
        GTEST_LOG_(INFO) << "SUB_Service_OnBackupExtensionDied_0100 RESTORE";
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        string bundleName = BUNDLE_NAME;
        servicePtr_->OnBackupExtensionDied(move(bundleName));
        GTEST_LOG_(INFO) << "SUB_Service_OnBackupExtensionDied_0100 BACKUP";
        ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        bundleName = BUNDLE_NAME;
        servicePtr_->OnBackupExtensionDied(move(bundleName));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnBackupExtensionDied_0100";
}

/**
 * @tc.number: SUB_Service_ExtStart_0100
 * @tc.name: SUB_Service_ExtStart_0100
 * @tc.desc: 测试 ExtStart 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_ExtStart_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtStart_0100";
    try {
        GTEST_LOG_(INFO) << "SUB_Service_ExtStart_0100 BACKUP";
        ErrCode ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ExtStart(BUNDLE_NAME);
        GTEST_LOG_(INFO) << "ServiceTest-ExtStart BACKUP Branches";
        servicePtr_->ExtStart(BUNDLE_NAME);
        GTEST_LOG_(INFO) << "SUB_Service_ExtStart_0100 RESTORE";
        ret = Init(IServiceReverse::Scenario::RESTORE);
        servicePtr_->ExtStart(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtStart_0100";
}

/**
 * @tc.number: SUB_Service_Dump_0100
 * @tc.name: SUB_Service_Dump_0100
 * @tc.desc: 测试 Dump 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_Dump_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_Dump_0100";
    try {
        servicePtr_->Dump(-1, {});
        TestManager tm("ServiceTest_GetFd_0100");
        string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        servicePtr_->Dump(move(fd), {});
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by Dump.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_Dump_0100";
}

/**
 * @tc.number: SUB_Service_ExtConnectFailed_0100
 * @tc.name: SUB_Service_ExtConnectFailed_0100
 * @tc.desc: 测试 ExtConnectFailed 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_ExtConnectFailed_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtConnectFailed_0100";
    try {
        GTEST_LOG_(INFO) << "SUB_Service_ExtConnectFailed_0100 RESTORE";
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ExtConnectFailed(BUNDLE_NAME, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "SUB_Service_ExtConnectFailed_0100 BACKUP";
        ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ExtConnectFailed(BUNDLE_NAME, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtConnectFailed.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtConnectFailed_0100";
}

/**
 * @tc.number: SUB_Service_ExtConnectDone_0100
 * @tc.name: SUB_Service_ExtConnectDone_0100
 * @tc.desc: 测试 ExtConnectDone 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_ExtConnectDone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtConnectDone_0100";
    try {
        servicePtr_->ExtConnectDone(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtConnectDone.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtConnectDone_0100";
}

/**
 * @tc.number: SUB_Service_StopAll_0100
 * @tc.name: SUB_Service_StopAll_0100
 * @tc.desc: 测试 StopAll 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_StopAll_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StopAll_0100";
    try {
        servicePtr_->StopAll(nullptr, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by StopAll.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StopAll_0100";
}

/**
 * @tc.number: SUB_Service_OnStop_0100
 * @tc.name: SUB_Service_OnStop_0100
 * @tc.desc: 测试 OnStop 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_OnStop_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnStop_0100";
    try {
        servicePtr_->OnStop();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnStop.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnStop_0100";
}
} // namespace OHOS::FileManagement::Backup