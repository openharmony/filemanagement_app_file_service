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
#include <map>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "b_json_clear_data_config_mock.h"
#include "b_json_service_disposal_config_mock.h"
#include "module_ipc/service.h"
#include "service_reverse_mock.h"
#include "test_common.h"
#include "test_manager.h"

#include "service.cpp"
#include "sub_service.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class ServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp()
    {
        totalStat_ = std::make_shared<RadarTotalStatistic>(BizScene::BACKUP, "UT.caller", Mode::FULL);
        session_ = sptr<SvcSessionManager>(new SvcSessionManager(servicePtr_));
        clearRecorder_ = std::make_shared<BJsonClearDataConfig>();
        disposal_ = std::make_shared<BJsonDisposalConfig>();
        clearRecorderMock_ = make_shared<BJsonClearDataConfigMock>();
        BJsonClearDataConfigMock::config = clearRecorderMock_;
        disposalMock_ = make_shared<BJsonDisposalConfigMock>();
        BBJsonDisposalConfig::config = disposalMock_;
    };
    void TearDown()
    {
        totalStat_ = nullptr;
        session_ = nullptr;
        clearRecorder_ = nullptr;
        disposal_ = nullptr;
        clearRecorderMock_ = nullptr;
        BJsonClearDataConfigMock::config = nullptr;
        disposalMock_ = nullptr;
        BBJsonDisposalConfig::config = nullptr;
    };

    ErrCode Init(IServiceReverseType::Scenario scenario);

    static inline sptr<Service> servicePtr_ = nullptr;
    static inline sptr<ServiceReverseMock> remote_ = nullptr;
    static inline std::shared_ptr<RadarTotalStatistic> totalStat_ = nullptr;
    static inline sptr<SvcSessionManager> session_ = nullptr;
    static inline std::shared_ptr<BJsonClearDataConfig> clearRecorder_ = nullptr;
    static inline shared_ptr<BJsonClearDataConfigMock> clearRecorderMock_ = nullptr;
    static inline std::shared_ptr<BJsonDisposalConfig> disposal_ = nullptr;
    static inline shared_ptr<BJsonDisposalConfigMock> disposalMock_ = nullptr;
    static inline bool boolVal_ = false;
    static inline int intVal_ = 0;
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

ErrCode ServiceTest::Init(IServiceReverseType::Scenario scenario)
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
    string errMsg;
    ErrCode ret = 0;
    if (scenario == IServiceReverseType::Scenario::RESTORE) {
        EXPECT_TRUE(servicePtr_ != nullptr);
        EXPECT_TRUE(remote_ != nullptr);
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_GE(fd, BError(BError::Codes::OK));
        sptr<IServiceReverse> srptr_=static_cast<sptr<IServiceReverse>>(remote_);
        ret = servicePtr_->InitRestoreSession(srptr_);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->InitRestoreSessionWithErrMsg(srptr_, ret, errMsg);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        ret = servicePtr_->AppendBundlesRestoreSession(move(fd), bundleNames, detailInfos);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        ret = servicePtr_->Finish();
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } else if (scenario == IServiceReverseType::Scenario::BACKUP) {
        sptr<IServiceReverse> srptr_ = static_cast<sptr<IServiceReverse>>(remote_);
        ret = servicePtr_->InitBackupSession(srptr_);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->InitBackupSessionWithErrMsg(srptr_, ret, errMsg);
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
 * @tc.require: mark
 */
HWTEST_F(ServiceTest, SUB_Service_GetLocalCapabilities_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetLocalCapabilities_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        int fd1 = 0;
        EXPECT_EQ(servicePtr_->GetLocalCapabilities(fd1), 0);
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_GT(fd, BError(BError::Codes::OK));
        servicePtr_->session_ = nullptr;
        EXPECT_EQ(servicePtr_->GetLocalCapabilities().Get(), UniqueFd(-EPERM).Get());
        servicePtr_->session_ = session_;
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
        servicePtr_->SetOccupySession(true);
        EXPECT_TRUE(servicePtr_->isOccupyingSession_.load());
        servicePtr_->OnStart();
        EXPECT_FALSE(servicePtr_->isOccupyingSession_.load());
        servicePtr_->disposal_ = disposal_;
        std::vector<std::string> bundleNames = { BUNDLE_NAME };
        EXPECT_CALL(*disposalMock_, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNames))
            .WillOnce(Return(bundleNames));
        servicePtr_->OnStart();
        EXPECT_FALSE(servicePtr_->isOccupyingSession_.load());
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
        GTEST_LOG_(INFO) << "1. Start ok";
        EXPECT_TRUE(servicePtr_ != nullptr);
        auto ret = servicePtr_->Start();
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        GTEST_LOG_(INFO) << "2. Start if2";
        servicePtr_->session_ = session_;
        session_->sessionCnt_.store(START_RETURN_FALSE);
        EXPECT_NE(servicePtr_->Start(), BError(BError::Codes::OK));
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
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
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
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
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
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
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
        auto ret = servicePtr_->AppFileReady(fileName, 1, 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "ServiceTest-AppFileReady Branches";
        fileName = "test";
        ret = servicePtr_->AppFileReady(fileName, 1, 0);
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
        auto ret = servicePtr_->AppFileReady(fileName, 1, 0);
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
        auto ret = servicePtr_->AppFileReady(fileName, 1, 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppFileReady_0102";
}

/**
 * @tc.number: SUB_Service_AppFileReady_0103
 * @tc.name: SUB_Service_AppFileReady_0103
 * @tc.desc: 测试 AppFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_AppFileReady_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppFileReady_0103";
    try {
        string fileName = "/manage.json";
        EXPECT_TRUE(servicePtr_ != nullptr);
        auto ret = servicePtr_->AppFileReady(fileName, UniqueFd(-1), 0);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppFileReady_0103";
}

/**
 * @tc.number: SUB_Service_AppFileReadyWithoutFd_0100
 * @tc.name: SUB_Service_AppFileReadyWithoutFd_0100
 * @tc.desc: 测试 AppFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_AppFileReadyWithoutFd_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppFileReady_0103";
    try {
        string fileName = "/manage.json";
        EXPECT_TRUE(servicePtr_ != nullptr);
        auto ret = servicePtr_->AppFileReadyWithoutFd(fileName, 0);
        EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppFileReady_0103";
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
        ret = Init(IServiceReverseType::Scenario::BACKUP);
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
        ret = Init(IServiceReverseType::Scenario::RESTORE);
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

        ret = servicePtr_->ServiceResultReport("test", BackupRestoreScenario::INCREMENTAL_BACKUP, 1);
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        ret = servicePtr_->ServiceResultReport("test", BackupRestoreScenario::FULL_RESTORE, 1);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ServiceResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ServiceResultReport_0000";
}

/**
 * @tc.number: SUB_Service_SAResultReport_0000
 * @tc.name: SUB_Service_SAResultReport_0000
 * @tc.desc: 测试 SAResultReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, SUB_Service_SAResultReport_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SAResultReport_0000";
    try {
        GTEST_LOG_(INFO) << "SUB_Service_SAResultReport Branches Start";
        string bundleName = BUNDLE_NAME;
        string restoreRetInfo;
        int errCode = 0;
        EXPECT_TRUE(servicePtr_ != nullptr);
        auto ret = servicePtr_->SAResultReport(BUNDLE_NAME, restoreRetInfo, errCode,
            BackupRestoreScenario::FULL_RESTORE);
        EXPECT_EQ(ret, 0);

        ret = servicePtr_->SAResultReport(BUNDLE_NAME, restoreRetInfo, errCode,
            BackupRestoreScenario::INCREMENTAL_RESTORE);
        EXPECT_EQ(ret, 0);

        ret = servicePtr_->SAResultReport(BUNDLE_NAME, restoreRetInfo, errCode, BackupRestoreScenario::FULL_BACKUP);
        EXPECT_EQ(ret, 0);

        ret = servicePtr_->SAResultReport(BUNDLE_NAME, restoreRetInfo, errCode,
            BackupRestoreScenario::INCREMENTAL_BACKUP);
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SAResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SAResultReport_0000";
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
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        ret = servicePtr_->LaunchBackupExtension(BUNDLE_NAME);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "SUB_Service_LaunchBackupExtension_0100 BACKUP";
        ret = Init(IServiceReverseType::Scenario::BACKUP);
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
        ErrCode ret = Init(IServiceReverseType::Scenario::UNDEFINED);
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
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
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
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        BackupExtInfo extInfo {};
        auto callDied = [](const string &&bundleName, bool isCleanCalled) {};
        auto callConnected = [](const string &&bundleName) {};
        extInfo.backUpConnection = sptr(new SvcBackupConnection(callDied, callConnected, BUNDLE_NAME));
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
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        string bundleName = BUNDLE_NAME;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->OnBackupExtensionDied(move(bundleName));
        GTEST_LOG_(INFO) << "SUB_Service_OnBackupExtensionDied_0100 BACKUP";
        ret = Init(IServiceReverseType::Scenario::BACKUP);
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
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        string bundleName = BUNDLE_NAME;
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        BackupExtInfo extInfo {};
        extInfo.backUpConnection = nullptr;
        extInfo.versionName = "0.0.0.0-0.0.0.0";
        impl_.restoreDataType = RESTORE_DATA_WAIT_SEND;
        impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        impl_.scenario = IServiceReverseType::Scenario::RESTORE;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->OnBackupExtensionDied(move(bundleName));
        GTEST_LOG_(INFO) << "SUB_Service_OnBackupExtensionDied_0101 BACKUP";

        ret = Init(IServiceReverseType::Scenario::BACKUP);
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
        GTEST_LOG_(INFO) << "1.ExtStart Incr BackupSA";
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->session_->impl_.callerName = BUNDLE_NAME;
        servicePtr_->ExtStart(SA_BUNDLE_NAME);

        GTEST_LOG_(INFO) << "2.ExtStart BackupSA";
        servicePtr_->session_->impl_.callerName = BUNDLE_NAME_FALSE;
        servicePtr_->session_->impl_.clientProxy = static_cast<sptr<IServiceReverse>>(remote_);
        servicePtr_->session_->impl_.scenario = IServiceReverseType::Scenario::RESTORE;
        servicePtr_->ExtStart(SA_BUNDLE_NAME);

        GTEST_LOG_(INFO) << "3.ExtStart IncrementalBackup";
        servicePtr_->ExtStart(BUNDLE_NAME);

        GTEST_LOG_(INFO) << "4.ExtStart StartCurBundleBackupOrRestore";
        servicePtr_->session_->impl_.callerName = BUNDLE_NAME_FALSE;
        servicePtr_->session_->impl_.scenario = IServiceReverseType::Scenario::BACKUP;
        servicePtr_->session_->impl_.clientProxy = static_cast<sptr<IServiceReverse>>(remote_);
        remote_->backupOnBundleStartedCalled_ = false;
        BackupExtInfo extInfo {};
        extInfo.backUpConnection = nullptr;
        extInfo.versionName = "0.0.0.0-0.0.0.0";
        servicePtr_->session_->impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        servicePtr_->ExtStart(BUNDLE_NAME);
        EXPECT_TRUE(remote_->backupOnBundleStartedCalled_);

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
        ErrCode ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->ExtStart(bundleName);

        GTEST_LOG_(INFO) << "SUB_Service_ExtStart_0101 RESTORE";
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        BackupExtInfo extInfo {};
        auto callDied = [](const string &&bundleName, bool isCleanCalled) {};
        auto callConnected = [](const string &&bundleName) {};
        extInfo.backUpConnection = sptr(new SvcBackupConnection(callDied, callConnected, BUNDLE_NAME));
        extInfo.backUpConnection->backupProxy_ = nullptr;
        impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        impl_.scenario = IServiceReverseType::Scenario::UNDEFINED;
        ret = Init(IServiceReverseType::Scenario::UNDEFINED);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ExtStart(BUNDLE_NAME);

        ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ExtStart(BUNDLE_NAME);

        ret = Init(IServiceReverseType::Scenario::UNDEFINED);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ExtStart(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtStart_0101";
}

/**
 * @tc.number: SUB_Service_StartCurBundleBackupOrRestore_0100
 * @tc.name: SUB_Service_StartCurBundleBackupOrRestore_0100
 * @tc.desc: 测试 StartCurBundleBackupOrRestore 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_StartCurBundleBackupOrRestore_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartCurBundleBackupOrRestore_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        GTEST_LOG_(INFO) << "1.StartCurBundleBackupOrRestore null1";
        servicePtr_->session_->impl_.backupExtNameMap.erase(BUNDLE_NAME);
        servicePtr_->StartCurBundleBackupOrRestore(BUNDLE_NAME);

        GTEST_LOG_(INFO) << "2.StartCurBundleBackupOrRestore null2";
        BackupExtInfo extInfo {};
        auto callDied = [](const string &&bundleName, bool isCleanCalled) {};
        auto callConnected = [](const string &&bundleName) {};
        string bundleNameIndexInfo = "123456789";
        auto connection = sptr(new SvcBackupConnection(callDied, callConnected, bundleNameIndexInfo));
        extInfo.backUpConnection = connection;
        connection->backupProxy_ = nullptr;
        servicePtr_->session_->impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        servicePtr_->session_->impl_.clientProxy = static_cast<sptr<IServiceReverse>>(remote_);
        servicePtr_->StartCurBundleBackupOrRestore(BUNDLE_NAME);

        GTEST_LOG_(INFO) << "3.StartCurBundleBackupOrRestore ok backup";
        servicePtr_->session_->impl_.backupExtNameMap[BUNDLE_NAME].backUpConnection = nullptr;
        servicePtr_->session_->impl_.scenario = IServiceReverseType::Scenario::BACKUP;
        servicePtr_->StartCurBundleBackupOrRestore(BUNDLE_NAME);

        GTEST_LOG_(INFO) << "4.StartCurBundleBackupOrRestore ok restore";
        servicePtr_->session_->impl_.callerName = BUNDLE_NAME_FALSE;
        servicePtr_->session_->impl_.scenario = IServiceReverseType::Scenario::RESTORE;
        remote_->restoreBundleStartCalled_ = false;
        servicePtr_->StartCurBundleBackupOrRestore(BUNDLE_NAME);
        EXPECT_TRUE(remote_->restoreBundleStartCalled_);

    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by StartCurBundleBackupOrRestore.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartCurBundleBackupOrRestore_0100";
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
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->ExtConnectFailed(BUNDLE_NAME, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "SUB_Service_ExtConnectFailed_0100 BACKUP";
        ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ExtConnectFailed(BUNDLE_NAME, BError(BError::Codes::OK));

        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        impl_.restoreDataType = RESTORE_DATA_READDY;
        ret = Init(IServiceReverseType::Scenario::RESTORE);
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
        std::vector<BundleName> bundles = { BUNDLE_NAME };
        session_->AppendBundles(bundles, bundles);
        servicePtr_->session_ = session_;
        session_->impl_.backupExtNameMap[BUNDLE_NAME].schedAction = BConstants::ServiceSchedAction::UNKNOWN;
        servicePtr_->ExtConnectDone(BUNDLE_NAME);
        session_->impl_.backupExtNameMap[BUNDLE_NAME].schedAction = BConstants::ServiceSchedAction::CLEAN;
        servicePtr_->ExtConnectDone(BUNDLE_NAME);
        session_->impl_.backupExtNameMap[BUNDLE_NAME].schedAction = BConstants::ServiceSchedAction::START;
        servicePtr_->clearRecorder_ = clearRecorder_;
        boolVal_ = true;
        EXPECT_CALL(*clearRecorderMock_, FindClearBundleRecord(_)).WillOnce(Return(true))
            .WillRepeatedly(ActionUpdateRetVal(&boolVal_, false));
        servicePtr_->ExtConnectDone(BUNDLE_NAME);
        servicePtr_->totalStatistic_ = nullptr;
        session_->impl_.backupExtNameMap[BUNDLE_NAME].schedAction = BConstants::ServiceSchedAction::START;
        servicePtr_->ExtConnectDone(BUNDLE_NAME);
        servicePtr_->totalStatistic_ = totalStat_;
        servicePtr_->ExtConnectDone(BUNDLE_NAME);
        servicePtr_->ExtConnectDone(BUNDLE_NAME_FALSE);
        EXPECT_FALSE(boolVal_);
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
        GTEST_LOG_(INFO) << "1. SendStartAppGalleryNotify null1";
        servicePtr_->SendStartAppGalleryNotify(SA_BUNDLE_NAME);

        GTEST_LOG_(INFO) << "2. SendStartAppGalleryNotify not restore";
        servicePtr_->session_->impl_.scenario = IServiceReverseType::Scenario::BACKUP;
        servicePtr_->SendStartAppGalleryNotify(BUNDLE_NAME);

        GTEST_LOG_(INFO) << "3. SendStartAppGalleryNotify ok";
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->SendStartAppGalleryNotify(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SendStartAppGalleryNotify.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SendStartAppGalleryNotify_0100";
}

/**
 * @tc.number: SUB_Service_SessionDeactive_0100
 * @tc.name: SUB_Service_SessionDeactive_0100
 * @tc.desc: 测试 SessionDeactive 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ todo
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
        auto ret = Init(IServiceReverseType::Scenario::BACKUP);
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
        auto ret = Init(IServiceReverseType::Scenario::BACKUP);
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
        session->impl_.clientProxy = static_cast<sptr<IServiceReverse>>(remote_);
        GTEST_LOG_(INFO) << "1. OnBundleStarted backup";
        session->impl_.scenario = IServiceReverseType::Scenario::BACKUP;
        servicePtr_->OnBundleStarted(BError(BError::Codes::SA_INVAL_ARG), session, BUNDLE_NAME);

        GTEST_LOG_(INFO) << "2. OnBundleStarted inc restore";
        session->impl_.scenario = IServiceReverseType::Scenario::RESTORE;
        servicePtr_->OnBundleStarted(BError(BError::Codes::SA_INVAL_ARG), session, BUNDLE_NAME);
        EXPECT_TRUE(remote_->incRestoreBundleStartCalled_);

        GTEST_LOG_(INFO) << "3. OnBundleStarted full restore";
        session->impl_.restoreDataType = RestoreTypeEnum::RESTORE_DATA_READDY;
        servicePtr_->OnBundleStarted(BError(BError::Codes::SA_INVAL_ARG), session, BUNDLE_NAME);
        EXPECT_TRUE(remote_->restoreBundleStartCalled_);
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
        std::string backupVersion = "";
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

        restoreBundleInfos.clear();
        aInfo.name = "123456";
        aInfo.appIndex = 2;
        restoreBundleInfos.push_back(aInfo);
        servicePtr_->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, restoreType, backupVersion);
        EXPECT_EQ(servicePtr_->session_->impl_.oldBackupVersion, backupVersion);
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
        servicePtr_->isOccupyingSession_.store(false);
        servicePtr_->session_ = session_;
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_GE(fd, BError(BError::Codes::OK));
        vector<BundleName> bundleNames {};
        RestoreTypeEnum restoreType = RESTORE_DATA_READDY;
        int32_t userId = 1;
        EXPECT_TRUE(servicePtr_ != nullptr);
        GTEST_LOG_(INFO) << "1. AppendBundlesRestoreSession fail nullptr";
        servicePtr_->session_ = nullptr;
        int ret = servicePtr_->AppendBundlesRestoreSession(move(fd), bundleNames, restoreType, userId);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        GTEST_LOG_(INFO) << "2. AppendBundlesRestoreSession fail false";
        servicePtr_->session_ = session_;
        servicePtr_->isOccupyingSession_.store(true);
        ret = servicePtr_->AppendBundlesRestoreSession(move(fd), bundleNames, restoreType, userId);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        GTEST_LOG_(INFO) << "3. AppendBundlesRestoreSession succ";
        servicePtr_->isOccupyingSession_.store(false);
        UniqueFd fd2 = servicePtr_->GetLocalCapabilities();
        ret = servicePtr_->AppendBundlesRestoreSession(move(fd2), bundleNames, restoreType, userId);
        EXPECT_EQ(ret, 0);

        GTEST_LOG_(INFO) << "4. AppendBundlesRestoreSessionData succ 1";
        UniqueFd fd3 = servicePtr_->GetLocalCapabilities();
        ret = servicePtr_->AppendBundlesRestoreSessionData(fd3.Get(), bundleNames, restoreType, userId);
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppendBundlesRestoreSession_01.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppendBundlesRestoreSession_0100";
}

/**
 * @tc.number: SUB_Service_AppendBundlesRestoreSession_0101
 * @tc.name: SUB_Service_AppendBundlesRestoreSession_0101
 * @tc.desc: 测试 AppendBundlesRestoreSession 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, SUB_Service_AppendBundlesRestoreSession_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppendBundlesRestoreSession_0101";
    try {
        servicePtr_->isOccupyingSession_.store(false);
        servicePtr_->session_ = session_;
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_GE(fd, BError(BError::Codes::OK));
        vector<BundleName> bundleNames {};
        RestoreTypeEnum restoreType = RESTORE_DATA_READDY;
        int32_t userId = 1;
        EXPECT_TRUE(servicePtr_ != nullptr);
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
        detailInfos.emplace_back(json);
        GTEST_LOG_(INFO) << "1. AppendBundlesRestoreSession fail nullptr";
        servicePtr_->session_ = nullptr;
        int ret = servicePtr_->AppendBundlesRestoreSession(move(fd), bundleNames, detailInfos, restoreType, userId);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        GTEST_LOG_(INFO) << "2. AppendBundlesRestoreSession fail false";
        servicePtr_->session_ = session_;
        servicePtr_->isOccupyingSession_.store(true);
        ret = servicePtr_->AppendBundlesRestoreSession(move(fd), bundleNames, detailInfos, restoreType, userId);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        GTEST_LOG_(INFO) << "3. AppendBundlesRestoreSession succ";
        servicePtr_->isOccupyingSession_.store(false);
        UniqueFd fd2 = servicePtr_->GetLocalCapabilities();
        ret = servicePtr_->AppendBundlesRestoreSession(move(fd2), bundleNames, detailInfos, restoreType, userId);
        EXPECT_EQ(ret, 0);

        GTEST_LOG_(INFO) << "4. AppendBundlesRestoreSessionDataByDetail succ";
        UniqueFd fd3 = servicePtr_->GetLocalCapabilities();
        ret = servicePtr_->AppendBundlesRestoreSessionDataByDetail(
            fd3.Get(), bundleNames, detailInfos, restoreType, userId
        );
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SUB_Service_AppendBundlesRestoreSession_0101.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppendBundlesRestoreSession_0101";
}

/**
 * @tc.number: SUB_Service_AppendBundlesBackupSession_0100
 * @tc.name: SUB_Service_AppendBundlesBackupSession_0100
 * @tc.desc: 测试 AppendBundlesBackupSession 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, SUB_Service_AppendBundlesBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppendBundlesBackupSession_0100";
    vector<BundleName> bundleNames { BUNDLE_NAME };
    EXPECT_TRUE(servicePtr_ != nullptr);
    GTEST_LOG_(INFO) << "1. AppendBundlesBackupSession fail nullptr";
    servicePtr_->session_ = nullptr;
    int ret = servicePtr_->AppendBundlesBackupSession(bundleNames);
    EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

    GTEST_LOG_(INFO) << "2. AppendBundlesBackupSession fail false";
    servicePtr_->session_ = session_;
    servicePtr_->isOccupyingSession_.store(true);
    ret = servicePtr_->AppendBundlesBackupSession(bundleNames);
    EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

    GTEST_LOG_(INFO) << "3. AppendBundlesBackupSession succ";
    servicePtr_->isOccupyingSession_.store(false);
    ret = servicePtr_->AppendBundlesBackupSession(bundleNames);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppendBundlesBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_AppendBundlesDetailsBackupSession_0100
 * @tc.name: SUB_Service_AppendBundlesDetailsBackupSession_0100
 * @tc.desc: 测试 AppendBundlesDetailsBackupSession 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, SUB_Service_AppendBundlesDetailsBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppendBundlesDetailsBackupSession_0100";
    std::vector<BundleName> bundleNames { BUNDLE_NAME };
    std::vector<std::string> bundleInfos;
    bundleInfos.push_back("");
    EXPECT_TRUE(servicePtr_ != nullptr);
    GTEST_LOG_(INFO) << "1. AppendBundlesDetailsBackupSession fail nullptr";
    servicePtr_->session_ = nullptr;
    int ret = servicePtr_->AppendBundlesDetailsBackupSession(bundleNames, bundleInfos);
    EXPECT_EQ(ret, 13900020);

    GTEST_LOG_(INFO) << "2. AppendBundlesDetailsBackupSession fail false";
    servicePtr_->session_ = session_;
    servicePtr_->isOccupyingSession_.store(true);
    ret = servicePtr_->AppendBundlesDetailsBackupSession(bundleNames, bundleInfos);
    EXPECT_EQ(ret, 13900020);

    GTEST_LOG_(INFO) << "3. AppendBundlesDetailsBackupSession succ";
    servicePtr_->isOccupyingSession_.store(false);
    ret = servicePtr_->AppendBundlesDetailsBackupSession(bundleNames, bundleInfos);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppendBundlesDetailsBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_HandleCurGroupBackupInfos_0100
 * @tc.name: SUB_Service_HandleCurGroupBackupInfos_0100
 * @tc.desc: 测试 HandleCurGroupBackupInfos 接口
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, SUB_Service_HandleCurGroupBackupInfos_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_HandleCurGroupBackupInfos_0100";
    std::vector<BJsonEntityCaps::BundleInfo> backupInfos;
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap;
    std::map<std::string, bool> isClearDataFlags;
    BJsonEntityCaps::BundleInfo bundleInfo;
    bundleInfo.name = BUNDLE_NAME;
    bundleInfo.appIndex = 0;
    bundleInfo.extensionName = "extensionNameTest";
    backupInfos.push_back(bundleInfo);
    EXPECT_TRUE(servicePtr_ != nullptr);
    GTEST_LOG_(INFO) << "1. HandleCurGroupBackupInfos";
    servicePtr_->session_ = session_;
    BackupExtInfo extInfo;
    session_->impl_.backupExtNameMap.emplace(BUNDLE_NAME, extInfo);
    servicePtr_->HandleCurGroupBackupInfos(backupInfos, bundleNameDetailMap, isClearDataFlags);
    EXPECT_EQ(session_->impl_.backupExtNameMap[BUNDLE_NAME].backupExtName, bundleInfo.extensionName);

    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_HandleCurGroupBackupInfos_0100";
}

/**
 * @tc.number: SUB_Service_HandleCurBundleEndWork_0100
 * @tc.name: SUB_Service_HandleCurBundleEndWork_0100
 * @tc.desc: 测试 HandleCurBundleEndWork 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_HandleCurBundleEndWork_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_HandleCurBundleEndWork_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        GTEST_LOG_(INFO) << "1. HandleCurBundleEndWork backup";
        servicePtr_->session_ = session_;
        servicePtr_->HandleCurBundleEndWork(BUNDLE_NAME, BackupRestoreScenario::FULL_BACKUP);
        
        GTEST_LOG_(INFO) << "2. HandleCurBundleEndWork nullptr1";
        servicePtr_->backupExtMutexMap_[BUNDLE_NAME] = nullptr;
        if (session_->OnBundleFileReady(BUNDLE_NAME)) {
            session_->OnBundleFileReady(BUNDLE_NAME);
        }
        servicePtr_->HandleCurBundleEndWork(BUNDLE_NAME, BackupRestoreScenario::FULL_RESTORE);

        GTEST_LOG_(INFO) << "3. HandleCurBundleEndWork nullptr2";
        servicePtr_->backupExtMutexMap_[BUNDLE_NAME] = std::make_shared<ExtensionMutexInfo>(BUNDLE_NAME);
        session_->impl_.backupExtNameMap.erase(BUNDLE_NAME);
        if (session_->OnBundleFileReady(BUNDLE_NAME)) {
            session_->OnBundleFileReady(BUNDLE_NAME);
        }
        servicePtr_->HandleCurBundleEndWork(BUNDLE_NAME, BackupRestoreScenario::FULL_RESTORE);

        GTEST_LOG_(INFO) << "4. HandleCurBundleEndWork nullptr3";
        auto callDied = [](const string &&bundleName, bool isCleanCalled) {};
        auto callConnected = [](const string &&bundleName) {};
        string bundleNameIndexInfo = "123456789";
        auto connection = sptr(new SvcBackupConnection(callDied, callConnected, bundleNameIndexInfo));
        BackupExtInfo extInfo {};
        extInfo.backUpConnection = connection;
        extInfo.backUpConnection->backupProxy_ = nullptr;
        session_->impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        if (session_->OnBundleFileReady(BUNDLE_NAME)) {
            session_->OnBundleFileReady(BUNDLE_NAME);
        }
        servicePtr_->HandleCurBundleEndWork(BUNDLE_NAME, BackupRestoreScenario::FULL_RESTORE);

        GTEST_LOG_(INFO) << "5. HandleCurBundleEndWork ok";
        servicePtr_->backupExtMutexMap_[BUNDLE_NAME] = std::make_shared<ExtensionMutexInfo>(BUNDLE_NAME);
        session_->impl_.backupExtNameMap[BUNDLE_NAME].backUpConnection = nullptr;
        if (session_->OnBundleFileReady(BUNDLE_NAME)) {
            session_->OnBundleFileReady(BUNDLE_NAME);
        }
        servicePtr_->HandleCurBundleEndWork(BUNDLE_NAME, BackupRestoreScenario::FULL_RESTORE);
        EXPECT_EQ(session_->impl_.clientToken, CLEARED_CLIENT_TOKEN);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by HandleCurBundleEndWork.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_HandleCurBundleEndWork_0100";
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
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
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
        GTEST_LOG_(INFO) << "1. LaunchBackupSAExtension other";
        ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        ret = servicePtr_->LaunchBackupSAExtension(bundleName);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        GTEST_LOG_(INFO) << "2. LaunchBackupSAExtension BACKUP";
        servicePtr_->session_->impl_.scenario = IServiceReverseType::Scenario::BACKUP;
        ret = servicePtr_->LaunchBackupSAExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        GTEST_LOG_(INFO) << "3. LaunchBackupSAExtension RESTORE";
        servicePtr_->session_->impl_.scenario = IServiceReverseType::Scenario::RESTORE;
        ret = servicePtr_->LaunchBackupSAExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
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
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        BackupExtInfo extInfo {};
        auto callDied = [](const string &&bundleName, bool isCleanCalled) {};
        auto callConnected = [](const string &&bundleName) {};
        extInfo.backUpConnection = sptr(new SvcBackupConnection(callDied, callConnected, BUNDLE_NAME));
        impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        impl_.scenario = IServiceReverseType::Scenario::RESTORE;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->ExtConnectDied(callName);
        extInfo.backUpConnection->isConnected_.store(true);
        servicePtr_->ExtConnectDied(callName);
        extInfo.isRestoreEnd = true;
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
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->NoticeClientFinish(BUNDLE_NAME, BError(BError::Codes::OK));
        GTEST_LOG_(INFO) << "SUB_Service_NoticeClientFinish_0100 BACKUP";
        ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->NoticeClientFinish(BUNDLE_NAME, BError(BError::Codes::OK));

        SvcSessionManager::Impl impl_;
        impl_.clientToken = 1;
        impl_.restoreDataType = RESTORE_DATA_READDY;
        ret = Init(IServiceReverseType::Scenario::RESTORE);
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
        ErrCode ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->session_ = sptr(new SvcSessionManager(servicePtr_));
        servicePtr_->OnAllBundlesFinished(BError(BError::Codes::OK));

        ret = Init(IServiceReverseType::Scenario::RESTORE);
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
        GTEST_LOG_(INFO) << "1. SendEndAppGalleryNotify backup";
        ErrCode ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->SendEndAppGalleryNotify(BUNDLE_NAME);

        GTEST_LOG_(INFO) << "2. SendEndAppGalleryNotify restore";
        ret = Init(IServiceReverseType::Scenario::RESTORE);
        servicePtr_->SendEndAppGalleryNotify(BUNDLE_NAME);

        GTEST_LOG_(INFO) << "3. SendEndAppGalleryNotify disposal false";
        servicePtr_->disposal_ = disposal_;
        boolVal_ = true;
        EXPECT_CALL(*disposalMock_, DeleteFromDisposalConfigFile(_))
            .WillRepeatedly(ActionUpdateRetVal(&boolVal_, false));
        servicePtr_->SendEndAppGalleryNotify(BUNDLE_NAME);
        EXPECT_FALSE(boolVal_);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SendEndAppGalleryNotify.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SendEndAppGalleryNotify_0100";
}

/**
 * @tc.number: SUB_Service_TryToClearDispose_0100
 * @tc.name: SUB_Service_TryToClearDispose_0100
 * @tc.desc: 测试 TryToClearDispose 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, SUB_Service_TryToClearDispose_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_TryToClearDispose_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        GTEST_LOG_(INFO) << "1. TryToClearDispose empty";
        servicePtr_->TryToClearDispose(EMPTY_BUNDLE_NAME);

        GTEST_LOG_(INFO) << "2. TryToClearDispose ok";
        servicePtr_->TryToClearDispose(BUNDLE_NAME);

        GTEST_LOG_(INFO) << "3. TryToClearDispose disposal false";
        servicePtr_->disposal_ = disposal_;
        boolVal_ = true;
        EXPECT_CALL(*disposalMock_, DeleteFromDisposalConfigFile(_))
            .WillRepeatedly(ActionUpdateRetVal(&boolVal_, false));
        servicePtr_->TryToClearDispose(BUNDLE_NAME);
        EXPECT_FALSE(boolVal_);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by TryToClearDispose.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_TryToClearDispose_0100";
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
        ErrCode ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->SendErrAppGalleryNotify();

        ret = Init(IServiceReverseType::Scenario::RESTORE);
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
        GTEST_LOG_(INFO) << "1. ClearDisposalOnSaStart backup";
        ErrCode ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->ClearDisposalOnSaStart();

        GTEST_LOG_(INFO) << "2. ClearDisposalOnSaStart disposal mock";
        servicePtr_->disposal_ = disposal_;
        std::vector<std::string> bundleNames = { BUNDLE_NAME };
        EXPECT_CALL(*disposalMock_, GetBundleNameFromConfigFile()).WillRepeatedly(Return(bundleNames));
        servicePtr_->ClearDisposalOnSaStart();
        EXPECT_TRUE(servicePtr_ != nullptr);
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
        GTEST_LOG_(INFO) << "1. DeleteDisConfigFile backup";
        servicePtr_->session_->impl_.scenario = IServiceReverseType::Scenario::BACKUP;
        servicePtr_->DeleteDisConfigFile();

        GTEST_LOG_(INFO) << "2. DeleteDisConfigFile restore if1";
        servicePtr_->session_->impl_.scenario = IServiceReverseType::Scenario::RESTORE;
        servicePtr_->disposal_ = disposal_;
        std::vector<std::string> bundleNames = { BUNDLE_NAME };
        std::vector<std::string> empty = {};
        EXPECT_CALL(*disposalMock_, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNames))
            .WillRepeatedly(Return(empty));
        boolVal_ = true;
        servicePtr_->DeleteDisConfigFile();

        GTEST_LOG_(INFO) << "3. DeleteDisConfigFile restore if2";
        EXPECT_CALL(*disposalMock_, DeleteConfigFile()).WillOnce(Return(true))
            .WillRepeatedly(ActionUpdateRetVal(&boolVal_, false));
        servicePtr_->DeleteDisConfigFile();

        GTEST_LOG_(INFO) << "4. DeleteDisConfigFile restore";
        servicePtr_->DeleteDisConfigFile();
        EXPECT_FALSE(boolVal_);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by DeleteDisConfigFile.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DeleteDisConfigFile_0100";
}

/**
 * @tc.number: SUB_Service_ExtensionConnectFailRadarReport_0100
 * @tc.name: SUB_Service_ExtensionConnectFailRadarReport_0100
 * @tc.desc: 测试 ExtensionConnectFailRadarReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_ExtensionConnectFailRadarReport_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtensionConnectFailRadarReport_0100";
    try {
        ErrCode ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->ClearBundleRadarReport();
        servicePtr_->ExtensionConnectFailRadarReport(BUNDLE_NAME, BError(BError::Codes::OK),
            IServiceReverseType::Scenario::BACKUP);
        EXPECT_TRUE(true);

        ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ExtensionConnectFailRadarReport(BUNDLE_NAME, BError(BError::Codes::OK),
            IServiceReverseType::Scenario::RESTORE);
        servicePtr_->UpdateBundleRadarReport(BUNDLE_NAME);
        servicePtr_->ExtensionConnectFailRadarReport(BUNDLE_NAME, BError(BError::Codes::OK),
            IServiceReverseType::Scenario::RESTORE);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtensionConnectFailRadarReport.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtensionConnectFailRadarReport_0100";
}

/**
 * @tc.number: SUB_Service_StartRunningTimer_0100
 * @tc.name: SUB_Service_StartRunningTimer_0100
 * @tc.desc: 测试 StartRunningTimer 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_StartRunningTimer_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartRunningTimer_0100";
    try {
        ErrCode ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->StartRunningTimer(BUNDLE_NAME);
        EXPECT_TRUE(true);

        ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->StartRunningTimer(BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by StartRunningTimer.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartRunningTimer_0100";
}

/**
 * @tc.number: SUB_Service_TimeoutRadarReport_0100
 * @tc.name: SUB_Service_TimeoutRadarReport_0100
 * @tc.desc: 测试 TimeoutRadarReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_TimeoutRadarReport_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_TimeoutRadarReport_0100";
    try {
        ErrCode ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        std::string bundleName = BUNDLE_NAME;
        servicePtr_->ClearBundleRadarReport();
        servicePtr_->TimeoutRadarReport(IServiceReverseType::Scenario::BACKUP, bundleName);
        EXPECT_TRUE(true);

        ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->TimeoutRadarReport(IServiceReverseType::Scenario::RESTORE, bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by TimeoutRadarReport.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_TimeoutRadarReport_0100";
}

/**
 * @tc.number: SUB_Service_ReportOnBundleStarted_0100
 * @tc.name: SUB_Service_ReportOnBundleStarted_0100
 * @tc.desc: 测试 ReportOnBundleStarted 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_ReportOnBundleStarted_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ReportOnBundleStarted_0100";
    try {
        ErrCode ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->ReportOnBundleStarted(IServiceReverseType::Scenario::BACKUP, BUNDLE_NAME);
        EXPECT_TRUE(true);

        ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        servicePtr_->ReportOnBundleStarted(IServiceReverseType::Scenario::RESTORE, BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ReportOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ReportOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_Service_HandleNotSupportBundleNames_0100
 * @tc.name: SUB_Service_HandleNotSupportBundleNames_0100
 * @tc.desc: 测试 HandleNotSupportBundleNames 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(ServiceTest, SUB_Service_HandleNotSupportBundleNames_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_HandleNotSupportBundleNames_0100";
    try {
        ErrCode ret = Init(IServiceReverseType::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        EXPECT_TRUE(servicePtr_ != nullptr);
        const std::vector<std::string> srcBundleNames = {"test0", "test1", "test2", "test3"};
        std::vector<std::string> supportBundleNames = {"test2", "test3", "test4", "test5"};

        servicePtr_->HandleNotSupportBundleNames(srcBundleNames, supportBundleNames, false);
        EXPECT_TRUE(true);
        servicePtr_->HandleNotSupportBundleNames(srcBundleNames, supportBundleNames, true);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by HandleNotSupportBundleNames.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_HandleNotSupportBundleNames_0100";
}

/**
 * @tc.number: SUB_Service_RefreshDataSize_0100
 * @tc.name: SUB_Service_RefreshDataSize_0100
 * @tc.desc: 测试 RefreshDataSize 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_RefreshDataSize_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_RefreshDataSize_0100";
    try {
        string fileName = MANAGE_JSON;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->session_ = nullptr;
        auto ret = servicePtr_->RefreshDataSize(0);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG));

        servicePtr_->session_ = sptr(new SvcSessionManager(servicePtr_));
        ret = servicePtr_->RefreshDataSize(0);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by RefreshDataSize.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_RefreshDataSize_0100";
}

/**
 * @tc.number: SUB_Service_StopExtTimer_0100
 * @tc.name: SUB_Service_StopExtTimer_0100
 * @tc.desc: 测试 StopExtTimer 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_StopExtTimer_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StopExtTimer_0100";
    try {
        string fileName = MANAGE_JSON;
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->session_ = nullptr;
        bool isExtStop = false;

        auto ret = servicePtr_->StopExtTimer(isExtStop);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG));

        servicePtr_->session_ = sptr(new SvcSessionManager(servicePtr_));
        ret = servicePtr_->StopExtTimer(isExtStop);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by StopExtTimer.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StopExtTimer_0100";
}

/**
 * @tc.number: SUB_Service_PublishFile_0103
 * @tc.name: SUB_Service_PublishFile_0103
 * @tc.desc: 测试 PublishFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_PublishFile_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_PublishFile_0103";
    try {
        ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        BFileInfo fileInfo {BUNDLE_NAME, "", 0};
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->session_ = nullptr;
        ret = servicePtr_->PublishFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG));
        servicePtr_->session_ = sptr(new SvcSessionManager(servicePtr_));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by PublishFile.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_PublishFile_0103";
}

/**
 * @tc.number: SUB_Service_AppFileReady_0104
 * @tc.name: SUB_Service_AppFileReady_0104
 * @tc.desc: 测试 AppFileReady 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceTest, SUB_Service_AppFileReady_0104, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppFileReady_0104";
    try {
        string fileName = "manage.json";
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->session_ = nullptr;
        auto ret = servicePtr_->AppFileReady(fileName, UniqueFd(-1), 0);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG));
        servicePtr_->session_ = sptr(new SvcSessionManager(servicePtr_));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppFileReady_0104";
}

HWTEST_F(ServiceTest, Service_Total_Start, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin Service_Total_Start";
    servicePtr_->totalStatistic_ = nullptr;
    servicePtr_->TotalStart();
    servicePtr_->totalStatistic_ = totalStat_;
    servicePtr_->TotalStart();
    EXPECT_GT(totalStat_->totalSpendTime_.startMilli_, 0);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_Total_Start";
}

HWTEST_F(ServiceTest, Service_Total_Stat_Report, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin Service_Total_Stat_Report";
    servicePtr_->totalStatistic_ = nullptr;
    servicePtr_->TotalStatReport(ERR_OK);
    servicePtr_->totalStatistic_ = totalStat_;
    servicePtr_->TotalStatReport(ERR_OK);
    EXPECT_GT(totalStat_->totalSpendTime_.endMilli_, 0);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_Total_Stat_Report";
}

} // namespace OHOS::FileManagement::Backup