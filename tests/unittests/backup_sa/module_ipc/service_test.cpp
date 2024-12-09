/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "service.cpp"
#include "service_reverse_mock.h"
#include "sub_service.cpp"
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
    vector<string> detailInfos;
    string json = "[{\"infos\":[{"
                    "\"details\":[{"
                        "\"detail\":[{"
                            "\"source\":\"com.app.demo001\","
                            "\"target\":\"com.example.fileonrestoreex\""
                        "}],"
                        "\"type\":\"app_mapping_relation\""
                    "}],"
                    "\"type\":\"broadcast\""
                    "}]"
                "}]";
    bundleNames.emplace_back(BUNDLE_NAME);
    detailInfos.emplace_back(json);
    ErrCode ret = 0;
    if (scenario == IServiceReverse::Scenario::RESTORE) {
        EXPECT_TRUE(servicePtr_ != nullptr);
        EXPECT_TRUE(remote_ != nullptr);
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_GE(fd, BError(BError::Codes::OK));
        ret = servicePtr_->InitRestoreSession(remote_);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        ret = servicePtr_->AppendBundlesRestoreSession(move(fd), bundleNames, detailInfos);
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
        EXPECT_TRUE(servicePtr_ != nullptr);
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_GT(fd, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetLocalCapabilities_0100";
}

/**
 * @tc.number: SUB_Service_GetLocalCapabilities_0101
 * @tc.name: SUB_Service_GetLocalCapabilities_0101
 * @tc.desc: 测试 GetLocalCapabilities 获取本地能力文件
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_GetLocalCapabilities_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetLocalCapabilities_0101";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_GT(fd, -EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetLocalCapabilities_0101";
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
        EXPECT_TRUE(servicePtr_ != nullptr);
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
        EXPECT_TRUE(servicePtr_ != nullptr);
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
        BFileInfo fileInfo {BUNDLE_NAME, "", 0};
        EXPECT_TRUE(servicePtr_ != nullptr);
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
 * @tc.number: SUB_Service_PublishFile_0101
 * @tc.name: SUB_Service_PublishFile_0101
 * @tc.desc: 测试 PublishFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_PublishFile_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_PublishFile_0101";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        BFileInfo fileInfo {BUNDLE_NAME, "", 0};
        EXPECT_TRUE(servicePtr_ != nullptr);
        ret = servicePtr_->PublishFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "ServiceTest-PublishFile Branches";
        fileInfo.fileName = "/data/storage/el2/restore/bundle.hap";
        ret = servicePtr_->PublishFile(fileInfo);
        EXPECT_NE(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by PublishFile.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_PublishFile_0101";
}

/**
 * @tc.number: SUB_Service_PublishFile_0102
 * @tc.name: SUB_Service_PublishFile_0102
 * @tc.desc: 测试 PublishFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_PublishFile_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_PublishFile_0102";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        BFileInfo fileInfo {BUNDLE_NAME, "", 0};
        EXPECT_TRUE(servicePtr_ != nullptr);
        ret = servicePtr_->PublishFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by PublishFile.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_PublishFile_0102";
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
        EXPECT_TRUE(servicePtr_ != nullptr);
        auto ret = servicePtr_->AppFileReady(fileName, UniqueFd(-1), 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "ServiceTest-AppFileReady Branches";
        fileName = "test";
        ret = servicePtr_->AppFileReady(fileName, UniqueFd(-1), 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppFileReady_0100";
}

/**
 * @tc.number: SUB_Service_AppFileReady_0101
 * @tc.name: SUB_Service_AppFileReady_0101
 * @tc.desc: 测试 AppFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_AppFileReady_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppFileReady_0101";
    try {
        string fileName = "";
        EXPECT_TRUE(servicePtr_ != nullptr);
        auto ret = servicePtr_->AppFileReady(fileName, UniqueFd(-1), 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppFileReady_0101";
}

/**
 * @tc.number: SUB_Service_AppFileReady_0102
 * @tc.name: SUB_Service_AppFileReady_0102
 * @tc.desc: 测试 AppFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_AppFileReady_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppFileReady_0102";
    try {
        string fileName = "manage.json";
        EXPECT_TRUE(servicePtr_ != nullptr);
        auto ret = servicePtr_->AppFileReady(fileName, UniqueFd(-1), 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppFileReady_0102";
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
        GTEST_LOG_(INFO) << "SUB_Service_AppDone Branches Start";
        EXPECT_TRUE(servicePtr_ != nullptr);
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
 * @tc.number: SUB_Service_AppDone_0101
 * @tc.name: SUB_Service_AppDone_0101
 * @tc.desc: 测试 AppDone 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_AppDone_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppDone_0101";
    try {
        GTEST_LOG_(INFO) << "SUB_Service_AppDone Branches Start";
        EXPECT_TRUE(servicePtr_ != nullptr);
        auto ret = servicePtr_->AppDone(BError(BError::Codes::OK));
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "SUB_Service_AppDone_0101 RESTORE";
        ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        ret = servicePtr_->AppDone(BError(BError::Codes::OK));
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppDone.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppDone_0101";
}

/**
 * @tc.number: SUB_Service_AppDone_0102
 * @tc.name: SUB_Service_AppDone_0102
 * @tc.desc: 测试 AppDone 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_AppDone_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppDone_0102";
    try {
        GTEST_LOG_(INFO) << "SUB_Service_AppDone Branches Start";
        string bundleName = "";
        EXPECT_TRUE(servicePtr_ != nullptr);
        auto ret = servicePtr_->AppDone(BError(BError::Codes::OK));
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppDone.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppDone_0102";
}

/**
 * @tc.number: SUB_Service_ServiceResultReport_0000
 * @tc.name: SUB_Service_ServiceResultReport_0000
 * @tc.desc: 测试 ServiceResultReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_ServiceResultReport_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ServiceResultReport_0000";
    try {
        GTEST_LOG_(INFO) << "SUB_Service_ServiceResultReport Branches Start";
        string bundleName = "";
        EXPECT_TRUE(servicePtr_ != nullptr);
        auto ret = servicePtr_->ServiceResultReport("test", BackupRestoreScenario::FULL_RESTORE, 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        ret = servicePtr_->ServiceResultReport("test", BackupRestoreScenario::INCREMENTAL_RESTORE, 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        ret = servicePtr_->ServiceResultReport("test", BackupRestoreScenario::FULL_BACKUP, 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        ret = servicePtr_->ServiceResultReport("test", BackupRestoreScenario::INCREMENTAL_BACKUP, 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        ret = servicePtr_->ServiceResultReport("test", static_cast<BackupRestoreScenario>(1000), 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ServiceResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ServiceResultReport_0000";
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
        EXPECT_TRUE(servicePtr_ != nullptr);
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
 * @tc.number: SUB_Service_LaunchBackupExtension_0101
 * @tc.name: SUB_Service_LaunchBackupExtension_0101
 * @tc.desc: 测试 LaunchBackupExtension 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_LaunchBackupExtension_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_LaunchBackupExtension_0101";
    try {
        GTEST_LOG_(INFO) << "SUB_Service_LaunchBackupExtension_0100 UNDEFINED";
        ErrCode ret = Init(IServiceReverse::Scenario::UNDEFINED);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        ret = servicePtr_->LaunchBackupExtension(BUNDLE_NAME);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by LaunchBackupExtension.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_LaunchBackupExtension_0101";
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
        EXPECT_TRUE(servicePtr_ != nullptr);
        ret = servicePtr_->GetFileHandle(BUNDLE_NAME, FILE_NAME);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetFileHandle_0100";
}

/**
 * @tc.number: SUB_Service_GetFileHandle_0101
 * @tc.name: SUB_Service_GetFileHandle_0101
 * @tc.desc: 测试 GetFileHandle 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_GetFileHandle_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetFileHandle_0101";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        BackupExtInfo extInfo {};
        auto callDied = [](const string &&bundleName, bool isCleanCalled) {};
        auto callConnected = [](const string &&bundleName) {};
        string bundleNameIndexInfo = "123456";
        extInfo.backUpConnection = sptr(new SvcBackupConnection(callDied, callConnected, bundleNameIndexInfo));
        extInfo.schedAction = BConstants::ServiceSchedAction::RUNNING;
        impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        EXPECT_TRUE(servicePtr_ != nullptr);
        ret = servicePtr_->GetFileHandle(BUNDLE_NAME, FILE_NAME);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetFileHandle_0101";
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
        EXPECT_TRUE(servicePtr_ != nullptr);
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
 * @tc.number: SUB_Service_OnBackupExtensionDied_0101
 * @tc.name: SUB_Service_OnBackupExtensionDied_0101
 * @tc.desc: 测试 OnBackupExtensionDied 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_OnBackupExtensionDied_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnBackupExtensionDied_0101";
    try {
        GTEST_LOG_(INFO) << "SUB_Service_OnBackupExtensionDied_0101 RESTORE";
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        string bundleName = BUNDLE_NAME;
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        BackupExtInfo extInfo {};
        extInfo.backUpConnection = nullptr;
        extInfo.versionName = "0.0.0.0-0.0.0.0";
        impl_.restoreDataType = RESTORE_DATA_WAIT_SEND;
        impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        impl_.scenario = IServiceReverse::Scenario::RESTORE;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->OnBackupExtensionDied(move(bundleName));
        GTEST_LOG_(INFO) << "SUB_Service_OnBackupExtensionDied_0101 BACKUP";

        ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        impl_.restoreDataType = RESTORE_DATA_READDY;
        bundleName = "123456789";
        impl_.backupExtNameMap[bundleName] = extInfo;
        servicePtr_->OnBackupExtensionDied(move(bundleName));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnBackupExtensionDied_0101";
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
        EXPECT_TRUE(servicePtr_ != nullptr);
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
 * @tc.number: SUB_Service_ExtStart_0101
 * @tc.name: SUB_Service_ExtStart_0101
 * @tc.desc: 测试 ExtStart 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_ExtStart_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtStart_0101";
    try {
        GTEST_LOG_(INFO) << "SUB_Service_ExtStart_0101 BACKUP";
        std::string bundleName = "123456";
        ErrCode ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->ExtStart(bundleName);

        GTEST_LOG_(INFO) << "SUB_Service_ExtStart_0101 RESTORE";
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        BackupExtInfo extInfo {};
        auto callDied = [](const string &&bundleName, bool isCleanCalled) {};
        auto callConnected = [](const string &&bundleName) {};
        string bundleNameIndexInfo = "123456789";
        extInfo.backUpConnection = sptr(new SvcBackupConnection(callDied, callConnected, bundleNameIndexInfo));
        extInfo.backUpConnection->backupProxy_ = nullptr;
        impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        impl_.scenario = IServiceReverse::Scenario::UNDEFINED;
        ret = Init(IServiceReverse::Scenario::UNDEFINED);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ExtStart(BUNDLE_NAME);

        ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ExtStart(BUNDLE_NAME);

        ret = Init(IServiceReverse::Scenario::UNDEFINED);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ExtStart(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtStart_0101";
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
        EXPECT_TRUE(servicePtr_ != nullptr);
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
 * @tc.number: SUB_Service_Dump_0101
 * @tc.name: SUB_Service_Dump_0101
 * @tc.desc: 测试 Dump 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_Dump_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_Dump_0101";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        int ret = servicePtr_->Dump(1, {});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by Dump.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_Dump_0101";
}

/**
 * @tc.number: SUB_Service_Dump_0102
 * @tc.name: SUB_Service_Dump_0102
 * @tc.desc: 测试 Dump 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_Dump_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_Dump_0102";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        int ret = servicePtr_->Dump(-1, {});
        EXPECT_EQ(ret, -1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by Dump.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_Dump_0102";
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
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->ExtConnectFailed(BUNDLE_NAME, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "SUB_Service_ExtConnectFailed_0100 BACKUP";
        ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ExtConnectFailed(BUNDLE_NAME, BError(BError::Codes::OK));

        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        impl_.restoreDataType = RESTORE_DATA_READDY;
        ret = Init(IServiceReverse::Scenario::RESTORE);
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
        EXPECT_TRUE(servicePtr_ != nullptr);
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
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->StopAll(nullptr, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by StopAll.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StopAll_0100";
}

/**
 * @tc.number: SUB_Service_StopAll_0101
 * @tc.name: SUB_Service_StopAll_0101
 * @tc.desc: 测试 StopAll 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_StopAll_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StopAll_0101";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientProxy = nullptr;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->StopAll(nullptr, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by StopAll.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StopAll_0101";
}

/**
 * @tc.number: SUB_Service_StopAll_0102
 * @tc.name: SUB_Service_StopAll_0102
 * @tc.desc: 测试 StopAll 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_StopAll_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StopAll_0102";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->StopAll(nullptr, false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by StopAll.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StopAll_0102";
}

/**
 * @tc.number: SUB_Service_StopAll_0103
 * @tc.name: SUB_Service_StopAll_0103
 * @tc.desc: 测试 StopAll 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_StopAll_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StopAll_0103";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientProxy = nullptr;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->StopAll(nullptr, false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by StopAll.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StopAll_0103";
}

/**
 * @tc.number: SUB_Service_StopAll_0104
 * @tc.name: SUB_Service_StopAll_0104
 * @tc.desc: 测试 StopAll 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_StopAll_0104, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StopAll_0104";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientProxy = nullptr;
        const wptr<IRemoteObject> obj = nullptr;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->StopAll(obj, false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by StopAll.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StopAll_0104";
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
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->OnStop();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnStop.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnStop_0100";
}

/**
 * @tc.number: SUB_Service_SendStartAppGalleryNotify_0100
 * @tc.name: SUB_Service_SendStartAppGalleryNotify_0100
 * @tc.desc: 测试 SendStartAppGalleryNotify 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_SendStartAppGalleryNotify_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SendStartAppGalleryNotify_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        BundleName bundleName = "";
        servicePtr_->SendStartAppGalleryNotify(bundleName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SendStartAppGalleryNotify.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SendStartAppGalleryNotify_0100";
}

/**
 * @tc.number: SUB_Service_SendStartAppGalleryNotify_0101
 * @tc.name: SUB_Service_SendStartAppGalleryNotify_0101
 * @tc.desc: 测试 SendStartAppGalleryNotify 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_SendStartAppGalleryNotify_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SendStartAppGalleryNotify_0101";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->SendStartAppGalleryNotify(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SendStartAppGalleryNotify.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SendStartAppGalleryNotify_0101";
}

/**
 * @tc.number: SUB_Service_SessionDeactive_0100
 * @tc.name: SUB_Service_SessionDeactive_0100
 * @tc.desc: 测试 SessionDeactive 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_SessionDeactive_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SessionDeactive_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->SessionDeactive();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SessionDeactive.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SessionDeactive_0100";
}

/**
 * @tc.number: SUB_Service_GetBackupInfo_0100
 * @tc.name: SUB_Service_GetBackupInfo_0100
 * @tc.desc: 测试 SessionDeactive 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_GetBackupInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetBackupInfo_0100";
    try {
        std::string bundleName = "com.example.app2backup";
        std::string backupInfo = "backup info";
        auto ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->GetBackupInfo(bundleName, backupInfo);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetBackupInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetBackupInfo_0100";
}

/**
 * @tc.number: SUB_Service_GetBackupInfo_0101
 * @tc.name: SUB_Service_GetBackupInfo_0101
 * @tc.desc: 测试 SessionDeactive 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_GetBackupInfo_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetBackupInfo_0101";
    try {
        std::string bundleName = "com.example.app2backup";
        std::string result = "ok";
        auto ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->session_ = nullptr;
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        ret = servicePtr_->GetBackupInfo(bundleName, result);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        impl_.clientToken = 0;
        ret = servicePtr_->GetBackupInfo(bundleName, result);
        EXPECT_NE(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetBackupInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetBackupInfo_0101";
}

/**
 * @tc.number: SUB_Service_UpdateTimer_0100
 * @tc.name: SUB_Service_UpdateTimer_0100
 * @tc.desc: 测试 UpdateTimer 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_UpdateTimer_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_UpdateTimer_0100";
    try {
        std::string bundleName = "com.example.app2backup";
        bool result = true;
        uint32_t timeout = 30000;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->UpdateTimer(bundleName, timeout, result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by UpdateTimer.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_UpdateTimer_0100";
}

/**
 * @tc.number: SUB_Service_UpdateTimer_0101
 * @tc.name: SUB_Service_UpdateTimer_0101
 * @tc.desc: 测试 UpdateTimer 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_UpdateTimer_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_UpdateTimer_0101";
    try {
        std::string bundleName = "com.example.app2backup";
        bool result = true;
        uint32_t timeout = 30000;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->session_ = nullptr;
        servicePtr_->UpdateTimer(bundleName, timeout, result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by UpdateTimer.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_UpdateTimer_0101";
}

/**
 * @tc.number: SUB_Service_GetBackupInfoCmdHandle_0100
 * @tc.name: SUB_Service_GetBackupInfoCmdHandle_0100
 * @tc.desc: 测试 GetBackupInfoCmdHandle 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_GetBackupInfoCmdHandle_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetBackupInfoCmdHandle_0100";
    try {
        std::string bundleName = "com.example.app2backup";
        std::string result;
        EXPECT_TRUE(servicePtr_ != nullptr);
        auto ret = servicePtr_->GetBackupInfoCmdHandle(bundleName, result);
        EXPECT_TRUE(ret == BError::BackupErrorCode::E_INVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetBackupInfoCmdHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetBackupInfoCmdHandle_0100";
}

/**
 * @tc.number: SUB_Service_SpecialVersion_0100
 * @tc.name: SUB_Service_SpecialVersion_0100
 * @tc.desc: 测试 SpecialVersion 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_SpecialVersion_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SpecialVersion_0100";
    try {
        std::string versionName = "0.0.0.0-0.0.0.0";
        EXPECT_TRUE(servicePtr_ != nullptr);
        bool ret = SpecialVersion(versionName);
        EXPECT_EQ(ret, true);
        versionName = "1.1.1.1-1.1.1.1";
        ret = SpecialVersion(versionName);
        EXPECT_EQ(ret, false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SpecialVersion.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SpecialVersion_0100";
}

/**
 * @tc.number: SUB_Service_OnBundleStarted_0100
 * @tc.name: SUB_Service_OnBundleStarted_0100
 * @tc.desc: 测试 OnBundleStarted 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_OnBundleStarted_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnBundleStarted_0100";
    try {
        int32_t saID = 2503;
        wptr<Service> reversePtr(new Service(saID));
        sptr<SvcSessionManager> session(new SvcSessionManager(reversePtr));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->OnBundleStarted(BError(BError::Codes::SA_INVAL_ARG), session, BUNDLE_NAME);
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        impl_.scenario = IServiceReverse::Scenario::RESTORE;
        servicePtr_->OnBundleStarted(BError(BError::Codes::SA_INVAL_ARG), session, BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnBundleStarted_0100";
}

/**
 * @tc.number: SUB_Service_HandleExceptionOnAppendBundles_0100
 * @tc.name: SUB_Service_HandleExceptionOnAppendBundles_0100
 * @tc.desc: 测试 HandleExceptionOnAppendBundles 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_HandleExceptionOnAppendBundles_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_HandleExceptionOnAppendBundles_0100";
    try {
        int32_t saID = 4801;
        wptr<Service> reversePtr(new Service(saID));
        sptr<SvcSessionManager> session(new SvcSessionManager(reversePtr));
        vector<BundleName> appendBundleNames {"123456"};
        vector<BundleName> restoreBundleNames {"abcdef"};
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->HandleExceptionOnAppendBundles(session, appendBundleNames, restoreBundleNames);

        appendBundleNames.push_back("789");
        servicePtr_->HandleExceptionOnAppendBundles(session, appendBundleNames, restoreBundleNames);

        restoreBundleNames.push_back("123456");
        restoreBundleNames.push_back("123");
        servicePtr_->HandleExceptionOnAppendBundles(session, appendBundleNames, restoreBundleNames);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by HandleExceptionOnAppendBundles.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_HandleExceptionOnAppendBundles_0100";
}

/**
 * @tc.number: SUB_Service_SetCurrentSessProperties_0100
 * @tc.name: SUB_Service_SetCurrentSessProperties_0100
 * @tc.desc: 测试 SetCurrentSessProperties 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_SetCurrentSessProperties_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SetCurrentSessProperties_0100";
    try {
        BJsonEntityCaps::BundleInfo aInfo {};
        aInfo.name = "123456";
        aInfo.extensionName = "abcdef";
        aInfo.allToBackup = true;
        std::vector<BJsonEntityCaps::BundleInfo> restoreBundleInfos {aInfo};
        std::vector<std::string> restoreBundleNames {"12345678"};
        RestoreTypeEnum restoreType = RESTORE_DATA_READDY;
        std::string backupVersion;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->session_ = sptr(new SvcSessionManager(servicePtr_));
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);

        restoreBundleNames.push_back("123456");
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);

        restoreBundleInfos.clear();
        aInfo.allToBackup = true;
        aInfo.versionName = "0.0.0.0-0.0.0.0";
        aInfo.extensionName = "";
        restoreBundleInfos.push_back(aInfo);
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);

        restoreBundleInfos.clear();
        aInfo.name = "123456a";
        restoreBundleInfos.push_back(aInfo);
        restoreBundleNames.push_back("123456a");
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);

        restoreBundleInfos.clear();
        aInfo.allToBackup = false;
        aInfo.extensionName = "";
        restoreBundleInfos.push_back(aInfo);
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);

        restoreBundleInfos.clear();
        aInfo.allToBackup = false;
        aInfo.extensionName = "";
        restoreBundleInfos.push_back(aInfo);
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SetCurrentSessProperties.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SetCurrentSessProperties_0100";
}

/**
 * @tc.number: SUB_Service_SetCurrentSessProperties_0101
 * @tc.name: SUB_Service_SetCurrentSessProperties_0101
 * @tc.desc: 测试 SetCurrentSessProperties 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_SetCurrentSessProperties_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SetCurrentSessProperties_0101";
    try {
        BJsonEntityCaps::BundleInfo aInfo {};
        aInfo.name = "123456";
        aInfo.versionName = "0.0.0.0-0.0.0.0";
        aInfo.extensionName = "abcdef";
        aInfo.allToBackup = false;
        std::vector<BJsonEntityCaps::BundleInfo> restoreBundleInfos {aInfo};
        std::vector<std::string> restoreBundleNames {"123456"};
        RestoreTypeEnum restoreType = RESTORE_DATA_READDY;
        std::string backupVersion;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->session_ = sptr(new SvcSessionManager(servicePtr_));
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);

        restoreBundleInfos.clear();
        aInfo.extensionName = "";
        restoreBundleInfos.push_back(aInfo);
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);

        restoreBundleInfos.clear();
        aInfo.name = "123456a";
        restoreBundleInfos.push_back(aInfo);
        restoreBundleNames.push_back("123456a");
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SetCurrentSessProperties.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SetCurrentSessProperties_0101";
}

/**
 * @tc.number: SUB_Service_SetCurrentSessProperties_0102
 * @tc.name: SUB_Service_SetCurrentSessProperties_0102
 * @tc.desc: 测试 SetCurrentSessProperties 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_SetCurrentSessProperties_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SetCurrentSessProperties_0102";
    try {
        BJsonEntityCaps::BundleInfo aInfo {};
        aInfo.name = "123456";
        aInfo.versionName = "0.0.0.0-0.0.0.0";
        aInfo.extensionName = "abcdef";
        aInfo.allToBackup = false;
        std::vector<BJsonEntityCaps::BundleInfo> restoreBundleInfos {aInfo};
        std::vector<std::string> restoreBundleNames {"123456"};
        RestoreTypeEnum restoreType = RESTORE_DATA_READDY;
        std::string backupVersion;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);

        restoreBundleInfos.clear();
        aInfo.versionName = "1.1.1.1-1.1.1.1";
        aInfo.extensionName = "";
        aInfo.name = "123456";
        restoreBundleInfos.push_back(aInfo);
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);

        restoreBundleInfos.clear();
        aInfo.name = "123456a";
        restoreBundleInfos.push_back(aInfo);
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);

        restoreBundleInfos.clear();
        aInfo.extensionName = "abcdef";
        restoreBundleInfos.push_back(aInfo);
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);

        restoreBundleInfos.clear();
        aInfo.name = "123456";
        restoreBundleInfos.push_back(aInfo);
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SetCurrentSessProperties.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SetCurrentSessProperties_0102";
}

/**
 * @tc.number: SUB_Service_AppendBundlesRestoreSession_0100
 * @tc.name: SUB_Service_AppendBundlesRestoreSession_0100
 * @tc.desc: 测试 AppendBundlesRestoreSession 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_AppendBundlesRestoreSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppendBundlesRestoreSession_0100";
    try {
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_GE(fd, BError(BError::Codes::OK));
        vector<BundleName> bundleNames {};
        RestoreTypeEnum restoreType = RESTORE_DATA_READDY;
        int32_t userId = 1;
        EXPECT_TRUE(servicePtr_ != nullptr);
        auto ret = servicePtr_->AppendBundlesRestoreSession(move(fd), bundleNames, restoreType, userId);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppendBundlesRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppendBundlesRestoreSession_0100";
}

/**
 * @tc.number: SUB_Service_NotifyCloneBundleFinish_0100
 * @tc.name: SUB_Service_NotifyCloneBundleFinish_0100
 * @tc.desc: 测试 NotifyCloneBundleFinish 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_NotifyCloneBundleFinish_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_NotifyCloneBundleFinish_0100";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        BackupExtInfo extInfo {};
        extInfo.backUpConnection = nullptr;
        impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        impl_.scenario = IServiceReverse::Scenario::RESTORE;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->NotifyCloneBundleFinish(BUNDLE_NAME, BackupRestoreScenario::FULL_RESTORE);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by NotifyCloneBundleFinish.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_NotifyCloneBundleFinish_0100";
}

/**
 * @tc.number: SUB_Service_LaunchBackupSAExtension_0100
 * @tc.name: SUB_Service_LaunchBackupSAExtension_0100
 * @tc.desc: 测试 LaunchBackupSAExtension 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_LaunchBackupSAExtension_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_LaunchBackupSAExtension_0100";
    try {
        std::string bundleName = "123456";
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        ret = servicePtr_->LaunchBackupSAExtension(BUNDLE_NAME);
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        BackupExtInfo extInfo {};
        extInfo.backUpConnection = nullptr;

        auto callDied = [](const string &&bundleName) {};
        auto callConnected = [](const string &&bundleName) {};
        auto callBackup = [](const std::string &&bundleName, const int &&fd, const std::string &&result,
                             const ErrCode &&errCode) {};
        auto callRestore = [](const std::string &&bundleName, const std::string &&result, const ErrCode &&errCode) {};
        extInfo.saBackupConnection =
            std::make_shared<SABackupConnection>(callDied, callConnected, callBackup, callRestore);

        impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        ret = servicePtr_->LaunchBackupSAExtension(bundleName);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        ret = servicePtr_->LaunchBackupSAExtension(bundleName);
        EXPECT_NE(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by LaunchBackupSAExtension.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_LaunchBackupSAExtension_0100";
}

/**
 * @tc.number: SUB_Service_ExtConnectDied_0100
 * @tc.name: SUB_Service_ExtConnectDied_0100
 * @tc.desc: 测试 ExtConnectDied 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_ExtConnectDied_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtConnectDied_0100";
    try {
        std::string callName = "123456";
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        BackupExtInfo extInfo {};
        auto callDied = [](const string &&bundleName, bool isCleanCalled) {};
        auto callConnected = [](const string &&bundleName) {};
        string bundleNameIndexInfo = "123456789";
        extInfo.backUpConnection = sptr(new SvcBackupConnection(callDied, callConnected, bundleNameIndexInfo));
        impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        impl_.scenario = IServiceReverse::Scenario::RESTORE;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->ExtConnectDied(callName);
        extInfo.backUpConnection->isConnected_.store(true);
        servicePtr_->ExtConnectDied(callName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtConnectDied.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtConnectDied_0100";
}

/**
 * @tc.number: SUB_Service_NoticeClientFinish_0100
 * @tc.name: SUB_Service_NoticeClientFinish_0100
 * @tc.desc: 测试 NoticeClientFinish 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_NoticeClientFinish_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_NoticeClientFinish_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->NoticeClientFinish(BUNDLE_NAME, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "SUB_Service_NoticeClientFinish_0100 BACKUP";
        ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->NoticeClientFinish(BUNDLE_NAME, BError(BError::Codes::OK));

        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        impl_.restoreDataType = RESTORE_DATA_READDY;
        ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->NoticeClientFinish(BUNDLE_NAME, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by NoticeClientFinish.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_NoticeClientFinish_0100";
}

/**
 * @tc.number: SUB_Service_OnAllBundlesFinished_0100
 * @tc.name: SUB_Service_OnAllBundlesFinished_0100
 * @tc.desc: 测试 OnAllBundlesFinished 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_OnAllBundlesFinished_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnAllBundlesFinished_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->session_ = sptr(new SvcSessionManager(servicePtr_));
        servicePtr_->OnAllBundlesFinished(BError(BError::Codes::OK));

        ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->OnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_Service_SendEndAppGalleryNotify_0100
 * @tc.name: SUB_Service_SendEndAppGalleryNotify_0100
 * @tc.desc: 测试 SendEndAppGalleryNotify 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_SendEndAppGalleryNotify_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SendEndAppGalleryNotify_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->SendEndAppGalleryNotify(BUNDLE_NAME);

        ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->SendEndAppGalleryNotify(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SendEndAppGalleryNotify.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SendEndAppGalleryNotify_0100";
}

/**
 * @tc.number: SUB_Service_SendErrAppGalleryNotify_0100
 * @tc.name: SUB_Service_SendErrAppGalleryNotify_0100
 * @tc.desc: 测试 SendErrAppGalleryNotify 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_SendErrAppGalleryNotify_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SendErrAppGalleryNotify_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->SendErrAppGalleryNotify();

        ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->SendErrAppGalleryNotify();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SendErrAppGalleryNotify.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SendErrAppGalleryNotify_0100";
}

/**
 * @tc.number: SUB_Service_ClearDisposalOnSaStart_0100
 * @tc.name: SUB_Service_ClearDisposalOnSaStart_0100
 * @tc.desc: 测试 ClearDisposalOnSaStart 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_ClearDisposalOnSaStart_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ClearDisposalOnSaStart_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->ClearDisposalOnSaStart();

        ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ClearDisposalOnSaStart();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ClearDisposalOnSaStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ClearDisposalOnSaStart_0100";
}

/**
 * @tc.number: SUB_Service_DeleteDisConfigFile_0100
 * @tc.name: SUB_Service_DeleteDisConfigFile_0100
 * @tc.desc: 测试 DeleteDisConfigFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_DeleteDisConfigFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DeleteDisConfigFile_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->DeleteDisConfigFile();

        ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->DeleteDisConfigFile();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by DeleteDisConfigFile.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DeleteDisConfigFile_0100";
}
} // namespace OHOS::FileManagement::Backup