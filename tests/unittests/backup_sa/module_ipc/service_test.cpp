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
#ifdef power_mgr_enabled
#include "power_mgr_client.h"
#include "running_lock.h"
#include "runninglock_mock.h"
#endif
#include "service_reverse_mock.h"
#include "test_common.h"
#include "test_manager.h"

#include "service.cpp"
#include "sub_service.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace PowerMgr;
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
        powerClientMock_ = std::make_shared<PowerMgrClientMock>();
        PowerMgrClientMock::powerMgrClient_ = powerClientMock_;
        runningLockMock_ = std::make_shared<RunningLockMock>();
        RunningLockMock::runninglock_ = runningLockMock_;
        servicePtr_->runningLockStatictic_ = std::make_shared<RadarRunningLockStatistic>(ERROR_OK);
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
        servicePtr_->runningLock_ = nullptr;
        servicePtr_->runningLockStatistic_ = nullptr;
        powerClientMock_ = nullptr;
        runningLockMock_ = nullptr;
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
    static inline shared_ptr<PowerMgrClientMock> powerClientMock_;
    static inline shared_ptr<RunningLockMock> runningLockMock_;
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
        EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _)).Times(2)
            .WillOnce(Return(nullptr);)
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
        EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _)).Times(2)
            .WillOnce(Return(nullptr);)
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
        EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _)).Times(2)
            .WillOnce(Return(nullptr);)
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
        EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _)).Times(2)
            .WillOnce(Return(nullptr);)
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
        EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _)).Times(2)
            .WillOnce(Return(nullptr);)
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
        EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _)).Times(4)
            .WillOnce(Return(nullptr);)
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
        EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _)).Times(2)
            .WillOnce(Return(nullptr);)
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
        EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _)).Times(2)
            .WillOnce(Return(nullptr);)
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
        EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _)).Times(4)
            .WillOnce(Return(nullptr);)
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
        EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _)).Times(4)
            .WillOnce(Return(nullptr);)
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
        EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _)).Times(4)
            .WillOnce(Return(nullptr);)
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
        EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _)).Times(6)
            .WillOnce(Return(nullptr);)
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
 * @tc.number: Service_Total_Start_Test
 * @tc.name: Service_Total_Start_Test
 * @tc.desc: 测试 TotalStatStart 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, Service_Total_Start_Test, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin Service_Total_Start_Test";
    servicePtr_->TotalStatStart(BizScene::BACKUP, BUNDLE_NAME, 1);
    EXPECT_GT(servicePtr_->totalStatistic_->totalSpendTime_.startMilli_, 0);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_Total_Start_Test";
}

/**
 * @tc.number: Service_Total_Stat_Report_Test
 * @tc.name: Service_Total_Stat_Report_Test
 * @tc.desc: 测试 TotalStatReport 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, Service_Total_Stat_Report_Test, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin Service_Total_Stat_Report_Test";
    servicePtr_->totalStatistic_ = nullptr;
    servicePtr_->TotalStatReport();
    servicePtr_->totalStatistic_ = totalStat_;
    servicePtr_->TotalStatReport();
    EXPECT_GT(totalStat_->uniqId_, 0);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_Total_Stat_Report_Test";
}

/**
 * @tc.number: Service_Total_Stat_End_Test
 * @tc.name: Service_Total_Stat_End_Test
 * @tc.desc: 测试 TotalStatEnd 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, Service_Total_Stat_End_Test, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin Service_Total_Stat_End_Test";
    servicePtr_->totalStatistic_ = nullptr;
    servicePtr_->TotalStatEnd(0);
    servicePtr_->totalStatistic_ = totalStat_;
    servicePtr_->TotalStatEnd(0);
    EXPECT_EQ(totalStat_->innerErr_, 0);
    servicePtr_->TotalStatEnd(1);
    EXPECT_EQ(totalStat_->innerErr_, 1);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_Total_Stat_End_Test";
}

/**
 * @tc.number: Service_Update_Handle_Count_Test
 * @tc.name: Service_Update_Handle_Count_Test
 * @tc.desc: 测试 UpdateHandleCnt 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, Service_Update_Handle_Count_Test, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin Service_Update_Handle_Count_Test";
    servicePtr_->totalStatistic_ = nullptr;
    servicePtr_->UpdateHandleCnt(0);
    servicePtr_->totalStatistic_ = totalStat_;
    servicePtr_->UpdateHandleCnt(0);
    EXPECT_EQ(totalStat_->succBundleCount_, 1);
    servicePtr_->UpdateHandleCnt(1);
    EXPECT_EQ(totalStat_->failBundleCount_, 1);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_Update_Handle_Count_Test";
}

#ifdef POWER_MANAGER_ENABLED
/**
 * @tc.number: Service_CreateRunningLock_Test_0100
 * @tc.name: Service_CreateRunningLock_Test_0100
 * @tc.desc: 测试 CreateRunningLock 接口 create success lock success
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, Service_CreateRunningLock_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin Service_CreateRunningLock_Test_0100";
    wptr<IPowerMgr> testProxy;
    auto testLock = std::make_shared<RunningLock>(
        testProxy,
        "testLock",
        RunningLockType::RUNNINGLLOCK_BACKGROUND
    );
    EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _))
        .WillOnce(Return(testLock);)

    EXPECT_CALL(*runningLockMock_, Lock(_))
        .WillOnce(Return(ERROR_OK);)
    servicePtr_->CreateRunningLock();
    EXPECT_NE(servicePtr_->runningLock_, nullptr);
    EXPECT_EQ(servicePtr_->runningLockStatistic_->radarCode_, ERROR_OK);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_CreateRunningLock_Test_0100";
}

/**
 * @tc.number: Service_CreateRunningLock_Test_0101
 * @tc.name: Service_CreateRunningLock_Test_0101
 * @tc.desc: 测试 CreateRunningLock 接口 create fail
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, Service_CreateRunningLock_Test_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin Service_CreateRunningLock_Test_0101";
    EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _))
        .WillOnce(Return(nullptr);)
    servicePtr_->CreateRunningLock();
    EXPECT_EQ(servicePtr_->runningLock_, nullptr);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_CreateRunningLock_Test_0101";
}

/**
 * @tc.number: Service_CreateRunningLock_Test_0102
 * @tc.name: Service_CreateRunningLock_Test_0102
 * @tc.desc: 测试 CreateRunningLock 接口 lock fail
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, Service_CreateRunningLock_Test_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin Service_CreateRunningLock_Test_0102";
    wptr<IPowerMgr> testProxy;
    auto testLock = std::make_shared<RunningLock>(
        testProxy,
        "testLock",
        RunningLockType::RUNNINGLLOCK_BACKGROUND
    );
    EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _))
        .WillOnce(Return(testLock);)
    EXPECT_CALL(*runningLockMock_, Lock(_)).WillOnce(Return(1);)
    servicePtr_->CreateRunningLock();
    EXPECT_EQ(servicePtr_->runningLock_, nullptr);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_CreateRunningLock_Test_0102";
}

/**
 * @tc.number: Service_CreateRunningLock_Test_0103
 * @tc.name: Service_CreateRunningLock_Test_0103
 * @tc.desc: 测试 CreateRunningLock 接口 no create lock success
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, Service_CreateRunningLock_Test_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin Service_CreateRunningLock_Test_0103";
    wptr<IPowerMgr> testProxy;
    auto testLock = std::make_shared<RunningLock>(
        testProxy,
        "testLock",
        RunningLockType::RUNNINGLLOCK_BACKGROUND
    );
    servicePtr_->runningLock_ = testLock;
    EXPECT_CALL(*runningLockMock_, Lock(_)).WillOnce(Return(ERROR_OK);)
    servicePtr_->CreateRunningLock();
    EXPECT_NE(servicePtr_->runningLock_, nullptr);
    EXPECT_EQ(servicePtr_->runningLockStatistic_->radarCode_, ERROR_OK);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_CreateRunningLock_Test_0103";
}

/**
 * @tc.number: Service_RunningLockRadarReport_Backup
 * @tc.name: Service_RunningLockRadarReport_Backup
 * @tc.desc: 测试 Service_RunningLockRadarReport_Backup
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, Service_RunningLockRadarReport_Backup, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin Service_RunningLockRadarReport_Backup";
    EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _))
        .WillOnce(Return(testLock);)
    ErrCode ret = Init(IServiceReverseType::Scenario::BACKUP);
    EXPECT_EQ(ret, BError(BError::Codes::OK));
    const std::string test = "test";
    const std::string ErrMsg = "ErrMsg";
    servicePtr_->runningLockStatistic = std::make_shared<RadarRunningLockStatistic>(ERROR_OK);
    int testCode = static_cast<int> (BError::Codes::SA_SESSION_RUNNINGLOCK_CREATE_FAIL);
    servicePtr_->RunningLockRadarReport(test, ErrMsg, testCode);
    EXPECT_NE(servicePtr_->runningLockStatistic->radarCode_, 0);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_RunningLockRadarReport_Backup";
}

/**
 * @tc.number: Service_RunningLockRadarReport_Restore
 * @tc.name: Service_RunningLockRadarReport_Restore
 * @tc.desc: 测试 Service_RunningLockRadarReport_Restore
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ServiceTest, Service_RunningLockRadarReport_Restore, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin Service_RunningLockRadarReport_Restore";
    EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _))
        .WillOnce(Return(testLock);)
    ErrCode ret = Init(IServiceReverseType::Scenario::RESTORE);
    EXPECT_EQ(ret, BError(BError::Codes::OK));
    const std::string test = "test";
    const std::string ErrMsg = "ErrMsg";
    servicePtr_->runningLockStatistic = std::make_shared<RadarRunningLockStatistic>(ERROR_OK);
    int testCode = static_cast<int> (BError::Codes::SA_SESSION_RUNNINGLOCK_CREATE_FAIL);
    servicePtr_->RunningLockRadarReport(test, ErrMsg, testCode);
    EXPECT_NE(servicePtr_->runningLockStatistic->radarCode_, 0);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_RunningLockRadarReport_Restore";
}
#endif
} // namespace OHOS::FileManagement::Backup