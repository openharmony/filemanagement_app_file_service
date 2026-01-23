/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifdef POWER_MANAGER_ENABLED
#include "power_mgr_client.h"
#include "running_lock.h"
#include "runninglock_mock.h"
#endif
#include "svc_session_manager_mock.h"

namespace OHOS::FileManagement::Backup {
ErrCode Service::AppendBundlesIncrementalBackupSessionWithBundleInfos(
    const std::vector<BIncrementalData> &bundlesToBackup,
    const std::vector<std::string> &bundleInfos)
{
    return BError(BError::Codes::OK);
}
ErrCode Service::PublishSAIncrementalFile(const BFileInfo &fileInfo, int fd)
{
    return BError(BError::Codes::OK);
}
ErrCode Service::AppIncrementalFileReady(const std::string &fileName, int fd,
    int manifestFd, int32_t appIncrementalFileReadyErrCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppIncrementalFileReadyWithoutFd(const std::string &fileName, int32_t appIncrementalFileReadyErrCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::Release()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::CancelForResult(const std::string& bundleName, int32_t &result)
{
    result = BError(BError::Codes::OK);
    return BError(BError::Codes::OK);
}

ErrCode Service::GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames, int &fd)
{
    return BError(BError::Codes::OK);
}

void Service::StartGetFdTask(std::string, wptr<Service>) {}

ErrCode Service::GetAppLocalListAndDoIncrementalBackup()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::InitIncrementalBackupSession(const sptr<IServiceReverse> &)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::InitIncrementalBackupSessionWithErrMsg(const sptr<IServiceReverse>&,
                                                        int32_t &errCodeForMsg, std::string &)
{
    errCodeForMsg = BError(BError::Codes::OK);
    return BError(BError::Codes::OK);
}

vector<string> Service::GetBundleNameByDetails(const std::vector<BIncrementalData>&)
{
    return {};
}

ErrCode Service::AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData>&)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData>&,
    const std::vector<std::string>&)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishIncrementalFile(const BFileInfo&)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishSAIncrementalFile(const BFileInfo&, UniqueFd)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppIncrementalFileReady(const std::string&, UniqueFd, UniqueFd, int32_t)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppIncrementalDone(ErrCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::GetIncrementalFileHandle(const std::string&, const std::string&)
{
    return BError(BError::Codes::OK);
}

bool Service::IncrementalBackup(const string&)
{
    return false;
}

ErrCode Service::IncrementalBackupSA(std::string bundleName)
{
    return BError(BError::Codes::OK);
}

void Service::NotifyCallerCurAppIncrementDone(ErrCode, const std::string&) {}

void Service::SendUserIdToApp(string&, int32_t) {}

void Service::SetCurrentBackupSessProperties(const vector<string> &,
                                             int32_t,
                                             std::vector<BJsonEntityCaps::BundleInfo> &,
                                             bool)
{
}

std::shared_ptr<ExtensionMutexInfo> Service::GetExtensionMutex(const BundleName &bundleName)
{
    return make_shared<ExtensionMutexInfo>(bundleName);
}

void Service::RemoveExtensionMutex(const BundleName&) {}

void Service::CreateDirIfNotExist(const std::string&) {}

void SvcSessionManager::UpdateDfxInfo(const std::string &bundleName, uint64_t uniqId) {}

void Service::ClearIncrementalStatFile(int32_t userId, const string &bundleName) {}

ErrCode Service::StartCleanData(int triggerType, unsigned int writeSize, unsigned int waitTime)
{
    return BError(BError::Codes::OK);
}
}

class BService {
public:
    virtual int VerifyDataClone() = 0;
public:
    BService() = default;
    virtual ~BService() = default;
public:
    static inline std::shared_ptr<BService> serviceMock = nullptr;
};

class ServiceMock : public BService {
public:
    MOCK_METHOD(int, VerifyDataClone, ());
};

int OHOS::FileManagement::Backup::Service::VerifyDataClone()
{
    return ServiceMock::serviceMock->VerifyDataClone();
}

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace testing::ext;
using namespace PowerMgr;
constexpr int32_t SERVICE_ID = 5203;
class ServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp()
    {
        service_ = sptr(new Service(SERVICE_ID));
        session_ = sptr<SvcSessionManager>(new SvcSessionManager(service_));
        service_->session_ = session_;
        srvMock_ = make_shared<ServiceMock>();
        ServiceMock::serviceMock = srvMock_;
        sessionMock_ = make_shared<SvcSessionManagerMock>();
        SvcSessionManagerMock::sessionManager = session;
#ifdef POWER_MANAGER_ENABLED
        powerClientMock_ = std::make_shared<PowerMgrClientMock>();
        PowerMgrClientMock::powerMgrClient_ = powerClientMock_;
        runningLockMock_ = std::make_shared<RunningLockMock>();
        RunningLockMock::runninglock_ = RunningLockMock_;
        service_->runningLockStatistic_ = std::make_shared<RadarRunningLockStatistic>(ERROR_OK);
#endif
    };
    void TearDown()
    {
#ifdef POWER_MANAGER_ENABLED
        service_->runningLock_ = nullptr;
        service_->runningLockStatistic_ = nullptr;
        powerClientMock_ = nullptr;
        runningLockMock_ = nullptr;
#endif
        ServiceMock::serviceMock = nullptr;
        srvMock_ = nullptr;
        session_ = nullptr;
        service_->session_ = nullptr;
        service_ = nullptr;
        SvcSessionManagerMock::sessionManager = nullptr;
        sessionMock_ = nullptr;
    };
    static inline shared_ptr<SvcSessionManagerMock> sessionMock_ = nullptr;
    static inline sptr<Service> service_ = nullptr;
    static inline shared_ptr<ServiceMock> srvMock_ = nullptr;
    static inline sptr<SvcSessionManager> session_ = nullptr;
#ifdef POWER_MANAGER_ENABLED
    static inline shared_ptr<PowerMgrClientMock> powerClientMock_;
    static inline shared_ptr<RunningLockMock> runningLockMock_;
#endif
};

void ServiceTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
}

void ServiceTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
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
        RunningLockType::RUNNINGLOCK_BACKGROUND
    );
    EXPECT_CALL(*srvMock_, VerifyDataClone()).WillOnce(Return(true));
    EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _))
        .WillOnce(Return(testLock));

    EXPECT_CALL(*runningLockMock_, Lock(_)).WillOnce(Return(ERROR_OK));
    service_->CreateRunningLock();
    EXPECT_NE(service_->runningLock_, nullptr);
    EXPECT_EQ(service_->runningLockStatistic_->radarCode_, ERROR_OK);
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
    EXPECT_CALL(*srvMock_, VerifyDataClone()).WillOnce(Return(true));
    EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*sessionMock_, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
    service_->CreateRunningLock();
    EXPECT_EQ(service_->runningLock_, nullptr);
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
        RunningLockType::RUNNINGLOCK_BACKGROUND
    );
    EXPECT_CALL(*srvMock_, VerifyDataClone()).WillOnce(Return(true));
    EXPECT_CALL(*powerClientMock_, CreateRunningLock(_, _))
        .WillOnce(Return(testLock));
    EXPECT_CALL(*runningLockMock_, Lock(_)).WillOnce(Return(1));
    EXPECT_CALL(*sessionMock_, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
    service_->CreateRunningLock();
    EXPECT_EQ(service_->runningLock_, nullptr);
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
        RunningLockType::RUNNINGLOCK_BACKGROUND
    );
    service_->runningLock_ = testLock;
    EXPECT_CALL(*srvMock_, VerifyDataClone()).WillOnce(Return(true));
    EXPECT_CALL(*runningLockMock_, Lock(_)).WillOnce(Return(ERROR_OK));
    service_->CreateRunningLock();
    EXPECT_NE(service_->runningLock_, nullptr);
    EXPECT_EQ(service_->runningLockStatistic_->radarCode_, ERROR_OK);
    GTEST_LOG_(INFO) << "ServiceTest-end Service_CreateRunningLock_Test_0103";
}
#endif
}