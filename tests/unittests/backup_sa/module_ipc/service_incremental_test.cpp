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

#include "backup_para_mock.h"
#include "bms_adapter_mock.h"
#include "b_json/b_json_entity_caps.h"
#include "b_jsonutil_mock.h"
#include "ipc_skeleton_mock.h"
#include "sa_backup_connection_mock.h"
#include "service_reverse_proxy_mock.h"
#include "svc_backup_connection_mock.h"
#include "svc_extension_proxy_mock.h"
#include "svc_session_manager_mock.h"

#include "service_incremental.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BService {
public:
    virtual UniqueFd GetLocalCapabilities() = 0;
    virtual ErrCode VerifyCallerAndGetCallerName(std::string&) = 0;
    virtual ErrCode InitRestoreSession(sptr<IServiceReverse>) = 0;
    virtual ErrCode InitRestoreSession(sptr<IServiceReverse>, std::string&) = 0;
    virtual ErrCode InitBackupSession(sptr<IServiceReverse>) = 0;
    virtual ErrCode InitBackupSession(sptr<IServiceReverse>, std::string&) = 0;
    virtual ErrCode Start() = 0;
    virtual ErrCode PublishFile(const BFileInfo&) = 0;
    virtual ErrCode AppFileReady(const string&, UniqueFd, int32_t) = 0;
    virtual ErrCode AppDone(ErrCode) = 0;
    virtual ErrCode ServiceResultReport(const std::string, BackupRestoreScenario, ErrCode) = 0;
    virtual ErrCode AppendBundlesRestoreSession(UniqueFd, const std::vector<BundleName>&,
        const std::vector<std::string>&, RestoreTypeEnum, int32_t) = 0;
    virtual ErrCode AppendBundlesRestoreSession(UniqueFd, const std::vector<BundleName>&, RestoreTypeEnum,
        int32_t) = 0;
    virtual ErrCode AppendBundlesBackupSession(const std::vector<BundleName>&) = 0;
    virtual ErrCode AppendBundlesDetailsBackupSession(const std::vector<BundleName>&,
        const std::vector<std::string>&) = 0;
    virtual ErrCode Finish() = 0;
    virtual ErrCode LaunchBackupExtension(const BundleName&) = 0;
    virtual ErrCode LaunchBackupSAExtension(const BundleName&) = 0;
    virtual ErrCode GetFileHandle(const string&, const string&) = 0;
    virtual int Dump(int, const vector<u16string>&) = 0;
    virtual ErrCode VerifyCaller() = 0;
    virtual ErrCode VerifyCaller(IServiceReverse::Scenario) = 0;
    virtual int32_t GetUserIdDefault() = 0;
    virtual ErrCode GetBackupInfo(BundleName&, std::string&) = 0;
    virtual ErrCode UpdateTimer(BundleName&, uint32_t, bool&) = 0;
    virtual ErrCode UpdateSendRate(std::string&, int32_t, bool&) = 0;
    virtual ErrCode ReportAppProcessInfo(const std::string, BackupRestoreScenario) = 0;
    virtual ErrCode StartExtTimer(bool&) = 0;
    virtual ErrCode StartFwkTimer(bool&) = 0;
    virtual ErrCode StopExtTimer(bool&) = 0;
    virtual ErrCode RefreshDataSize(int64_t) = 0;
    virtual std::string GetCallerName() = 0;
    virtual bool IsReportBundleExecFail(const std::string&) = 0;
    virtual bool IsReportFileReadyFail(const std::string&) = 0;
    virtual std::vector<std::string> GetSupportBackupBundleNames(vector<BJsonEntityCaps::BundleInfo>&, bool,
        const vector<string>&) = 0;
    virtual ErrCode HandleCurBundleFileReady(const std::string&, const std::string&, bool) = 0;
    virtual ErrCode HandleCurAppDone(ErrCode, const std::string&, bool) = 0;
    virtual UniqueFd GetLocalCapabilitiesForBundleInfos() = 0;
    virtual ErrCode GetBackupDataSize(bool, vector<BIncrementalData>) = 0;
public:
    virtual bool UpdateToRestoreBundleMap(const string&, const string&) = 0;
public:
    BService() = default;
    virtual ~BService() = default;
public:
    static inline std::shared_ptr<BService> serviceMock = nullptr;
};

class ServiceMock : public BService {
public:
    MOCK_METHOD(UniqueFd, GetLocalCapabilities, ());
    MOCK_METHOD(ErrCode, VerifyCallerAndGetCallerName, (std::string&));
    MOCK_METHOD(ErrCode, InitRestoreSession, (sptr<IServiceReverse>));
    MOCK_METHOD(ErrCode, InitRestoreSession, (sptr<IServiceReverse>, std::string&));
    MOCK_METHOD(ErrCode, InitBackupSession, (sptr<IServiceReverse>));
    MOCK_METHOD(ErrCode, InitBackupSession, ((sptr<IServiceReverse>), std::string&));
    MOCK_METHOD(ErrCode, Start, ());
    MOCK_METHOD(ErrCode, PublishFile, (const BFileInfo&));
    MOCK_METHOD(ErrCode, AppFileReady, (const string&, UniqueFd, int32_t));
    MOCK_METHOD(ErrCode, AppDone, (ErrCode));
    MOCK_METHOD(ErrCode, ServiceResultReport, (const std::string, BackupRestoreScenario, ErrCode));
    MOCK_METHOD(ErrCode, AppendBundlesRestoreSession, (UniqueFd, (const std::vector<BundleName>&),
        (const std::vector<std::string>&), RestoreTypeEnum, int32_t));
    MOCK_METHOD(ErrCode, AppendBundlesRestoreSession, (UniqueFd, (const std::vector<BundleName>&), RestoreTypeEnum,
        int32_t));
    MOCK_METHOD(ErrCode, AppendBundlesBackupSession, (const std::vector<BundleName>&));
    MOCK_METHOD(ErrCode, AppendBundlesDetailsBackupSession, ((const std::vector<BundleName>&),
        (const std::vector<std::string>&)));
    MOCK_METHOD(ErrCode, Finish, ());
    MOCK_METHOD(ErrCode, LaunchBackupExtension, (const BundleName&));
    MOCK_METHOD(ErrCode, LaunchBackupSAExtension, (const BundleName&));
    MOCK_METHOD(ErrCode, GetFileHandle, (const string&, const string&));
    MOCK_METHOD(int, Dump, (int, const vector<u16string>&));
    MOCK_METHOD(ErrCode, VerifyCaller, ());
    MOCK_METHOD(ErrCode, VerifyCaller, (IServiceReverse::Scenario));
    MOCK_METHOD(int32_t, GetUserIdDefault, ());
    MOCK_METHOD(ErrCode, GetBackupInfo, (BundleName&, std::string&));
    MOCK_METHOD(ErrCode, UpdateTimer, (BundleName&, uint32_t, bool&));
    MOCK_METHOD(ErrCode, UpdateSendRate, (std::string&, int32_t, bool&));
    MOCK_METHOD(ErrCode, ReportAppProcessInfo, (const std::string, BackupRestoreScenario));
    MOCK_METHOD(ErrCode, StartExtTimer, (bool&));
    MOCK_METHOD(ErrCode, StartFwkTimer, (bool&));
    MOCK_METHOD(ErrCode, StopExtTimer, (bool&));
    MOCK_METHOD(ErrCode, RefreshDataSize, (int64_t));
    MOCK_METHOD(std::string, GetCallerName, ());
    MOCK_METHOD(bool, IsReportBundleExecFail, (const std::string&));
    MOCK_METHOD(bool, IsReportFileReadyFail, (const std::string&));
    MOCK_METHOD(std::vector<std::string>, GetSupportBackupBundleNames, ((vector<BJsonEntityCaps::BundleInfo>&), bool,
        (const vector<string>&)));
    MOCK_METHOD(ErrCode, HandleCurBundleFileReady, (const std::string&, const std::string&, bool));
    MOCK_METHOD(ErrCode, HandleCurAppDone, (ErrCode, const std::string&, bool));
    MOCK_METHOD(UniqueFd, GetLocalCapabilitiesForBundleInfos, ());
    MOCK_METHOD(ErrCode, GetBackupDataSize, (bool, vector<BIncrementalData>));
public:
    MOCK_METHOD(bool, UpdateToRestoreBundleMap, (const string&, const string&));
};

void Service::OnStart() {}

void Service::OnStop() {}

UniqueFd Service::GetLocalCapabilities()
{
    return BService::serviceMock->GetLocalCapabilities();
}

void Service::StopAll(const wptr<IRemoteObject>&, bool) {}

ErrCode Service::VerifyCallerAndGetCallerName(std::string &bundleName)
{
    return BService::serviceMock->VerifyCallerAndGetCallerName(bundleName);
}

ErrCode Service::InitRestoreSession(sptr<IServiceReverse> remote)
{
    return BService::serviceMock->InitRestoreSession(remote);
}

ErrCode Service::InitRestoreSession(sptr<IServiceReverse> remote, std::string &errMsg)
{
    return BService::serviceMock->InitRestoreSession(remote, errMsg);
}

ErrCode Service::InitBackupSession(sptr<IServiceReverse> remote)
{
    return BService::serviceMock->InitBackupSession(remote);
}

ErrCode Service::InitBackupSession(sptr<IServiceReverse> remote, std::string &errMsg)
{
    return BService::serviceMock->InitBackupSession(remote, errMsg);
}

ErrCode Service::Start()
{
    return BService::serviceMock->Start();
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    return BService::serviceMock->PublishFile(fileInfo);
}

ErrCode Service::AppFileReady(const string &fileName, UniqueFd fd, int32_t errCode)
{
    return BService::serviceMock->AppFileReady(fileName, move(fd), errCode);
}

ErrCode Service::AppDone(ErrCode errCode)
{
    return BService::serviceMock->AppDone(errCode);
}

ErrCode Service::ServiceResultReport(const std::string restoreRetInfo, BackupRestoreScenario sennario, ErrCode errCode)
{
    return BService::serviceMock->ServiceResultReport(restoreRetInfo, sennario, errCode);
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd, const std::vector<BundleName> &bundleNames,
    const std::vector<std::string> &detailInfos, RestoreTypeEnum restoreType, int32_t userId)
{
    return BService::serviceMock->AppendBundlesRestoreSession(move(fd), bundleNames, detailInfos, restoreType, userId);
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd, const std::vector<BundleName> &bundleNames,
    RestoreTypeEnum restoreType, int32_t userId)
{
    return BService::serviceMock->AppendBundlesRestoreSession(move(fd), bundleNames, restoreType, userId);
}

ErrCode Service::AppendBundlesBackupSession(const std::vector<BundleName> &bundleNames)
{
    return BService::serviceMock->AppendBundlesBackupSession(bundleNames);
}

ErrCode Service::AppendBundlesDetailsBackupSession(const std::vector<BundleName> &bundleNames,
    const std::vector<std::string> &bundleInfos)
{
    return BService::serviceMock->AppendBundlesDetailsBackupSession(bundleNames, bundleInfos);
}

ErrCode Service::Finish()
{
    return BService::serviceMock->Finish();
}

ErrCode Service::LaunchBackupExtension(const BundleName &bundleName)
{
    return BService::serviceMock->LaunchBackupExtension(bundleName);
}

ErrCode Service::LaunchBackupSAExtension(const BundleName &bundleName)
{
    return BService::serviceMock->LaunchBackupSAExtension(bundleName);
}

ErrCode Service::GetFileHandle(const string &bundleName, const string &fileName)
{
    return BService::serviceMock->GetFileHandle(bundleName, fileName);
}

void Service::OnBackupExtensionDied(const string&&, bool) {}

void Service::ExtConnectDied(const string&) {}

void Service::ExtStart(const string&) {}

int Service::Dump(int fd, const vector<u16string> &args)
{
    return BService::serviceMock->Dump(fd, args);
}

void Service::ExtConnectFailed(const string&, ErrCode) {}

void Service::ExtConnectDone(string) {}

void Service::ClearSessionAndSchedInfo(const string&) {}

ErrCode Service::VerifyCaller()
{
    return BService::serviceMock->VerifyCaller();
}

ErrCode Service::VerifyCaller(IServiceReverse::Scenario scenario)
{
    return BService::serviceMock->VerifyCaller(scenario);
}

int32_t Service::GetUserIdDefault()
{
    return BService::serviceMock->GetUserIdDefault();
}

void Service::OnAllBundlesFinished(ErrCode) {}

void Service::OnStartSched() {}

void Service::SendStartAppGalleryNotify(const BundleName&) {}

void Service::SessionDeactive() {}

ErrCode Service::GetBackupInfo(BundleName &bundleName, std::string &result)
{
    return BService::serviceMock->GetBackupInfo(bundleName, result);
}

ErrCode Service::UpdateTimer(BundleName &bundleName, uint32_t timeOut, bool &result)
{
    return BService::serviceMock->UpdateTimer(bundleName, timeOut, result);
}

ErrCode Service::UpdateSendRate(std::string &bundleName, int32_t sendRate, bool &result)
{
    return BService::serviceMock->UpdateSendRate(bundleName, sendRate,  result);
}

void Service::OnSABackup(const std::string&, const int&, const std::string&, const ErrCode&) {}

void Service::OnSARestore(const std::string&, const std::string&, const ErrCode&) {}

void Service::NotifyCallerCurAppDone(ErrCode, const std::string&) {}

void Service::SendEndAppGalleryNotify(const BundleName&) {}

void Service::NoticeClientFinish(const string&, ErrCode) {}

ErrCode Service::ReportAppProcessInfo(const std::string processInfo, BackupRestoreScenario sennario)
{
    return BService::serviceMock->ReportAppProcessInfo(processInfo, sennario);
}

ErrCode Service::StartExtTimer(bool &isExtStart)
{
    return BService::serviceMock->StartExtTimer(isExtStart);
}

ErrCode Service::StartFwkTimer(bool &isFwkStart)
{
    return BService::serviceMock->StartFwkTimer(isFwkStart);
}

ErrCode Service::StopExtTimer(bool &isExtStop)
{
    return BService::serviceMock->StopExtTimer(isExtStop);
}

ErrCode Service::RefreshDataSize(int64_t totalsize)
{
    return BService::serviceMock->RefreshDataSize(totalsize);
}

std::function<void()> Service::TimeOutCallback(wptr<Service> ptr, std::string bundleName)
{
    return []() {};
}

void Service::SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo>&, std::vector<std::string>&,
    RestoreTypeEnum, std::string &) {}

void Service::SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo>&, std::vector<std::string>&,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>>&, std::map<std::string, bool>&,
    RestoreTypeEnum, std::string &) {}

void Service::SetCurrentSessProperties(BJsonEntityCaps::BundleInfo&, std::map<std::string, bool>&,
    const std::string&) {}

void Service::ReleaseOnException() {}

bool SvcRestoreDepsManager::UpdateToRestoreBundleMap(const string &bundleName, const string &fileName)
{
    return BService::serviceMock->UpdateToRestoreBundleMap(bundleName, fileName);
}

void Service::OnBundleStarted(BError, sptr<SvcSessionManager>, const BundleName&) {}

void Service::HandleExceptionOnAppendBundles(sptr<SvcSessionManager>, const vector<BundleName>&,
    const vector<BundleName>&) {}

void Service::BundleBeginRadarReport(const std::string&, const ErrCode, const IServiceReverse::Scenario) {}

void Service::BundleEndRadarReport(const std::string&, const ErrCode, const IServiceReverse::Scenario) {}

void Service::FileReadyRadarReport(const std::string&, const std::string&, const ErrCode,
    const IServiceReverse::Scenario) {}

void Service::ExtensionConnectFailRadarReport(const std::string&, const ErrCode, const IServiceReverse::Scenario) {}

void Service::PermissionCheckFailRadar(const std::string&, const std::string&) {}

void Service::OnStartResRadarReport(const std::vector<std::string>&, int32_t) {}

std::string Service::GetCallerName()
{
    return BService::serviceMock->GetCallerName();
}

bool Service::IsReportBundleExecFail(const std::string &bundleName)
{
    return BService::serviceMock->IsReportBundleExecFail(bundleName);
}

void Service::ClearBundleRadarReport() {}

void Service::UpdateBundleRadarReport(const std::string&) {}

bool Service::IsReportFileReadyFail(const std::string &bundleName)
{
    return BService::serviceMock->IsReportFileReadyFail(bundleName);
}

void Service::ClearFileReadyRadarReport() {}

void Service::UpdateFailedBundles(const std::string&, BundleTaskInfo) {}

void Service::ClearFailedBundles() {}

void Service::StartRunningTimer(const std::string&) {}

std::vector<std::string> Service::GetSupportBackupBundleNames(vector<BJsonEntityCaps::BundleInfo> &bundleInfos,
    bool isIncBackup, const vector<std::string> &srcBundleNames)
{
    return BService::serviceMock->GetSupportBackupBundleNames(bundleInfos, isIncBackup, srcBundleNames);
}

ErrCode Service::HandleCurBundleFileReady(const std::string &bundleName, const std::string &fileName, bool isIncBackup)
{
    return BService::serviceMock->HandleCurBundleFileReady(bundleName, fileName, isIncBackup);
}

ErrCode Service::HandleCurAppDone(ErrCode errCode, const std::string &bundleName, bool isIncBackup)
{
    return BService::serviceMock->HandleCurAppDone(errCode, bundleName, isIncBackup);
}

void Service::StartCurBundleBackupOrRestore(const std::string&) {}

UniqueFd Service::GetLocalCapabilitiesForBundleInfos()
{
    return BService::serviceMock->GetLocalCapabilitiesForBundleInfos();
}

void Service::AppendBundles(const std::vector<std::string>&) {}

ErrCode Service::GetBackupDataSize(bool isPreciseScan, vector<BIncrementalData> bundleNameList)
{
    return BService::serviceMock->GetBackupDataSize(isPreciseScan, bundleNameList);
}
} // namespace OHOS::FileManagement::Backup

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace testing::ext;

constexpr int32_t SERVICE_ID = 5203;

class ServiceIncrementalTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() {};
    void TearDown() {};

    static inline sptr<Service> service = nullptr;
    static inline shared_ptr<BackupParaMock> param = nullptr;
    static inline shared_ptr<BJsonUtilMock> jsonUtil = nullptr;
    static inline shared_ptr<SvcSessionManagerMock> session = nullptr;
    static inline shared_ptr<BundleMgrAdapterMock> bms = nullptr;
    static inline sptr<SvcExtensionProxyMock> svcProxy = nullptr;
    static inline sptr<SvcBackupConnectionMock> connect = nullptr;
    static inline shared_ptr<SABackupConnectionMock> saConnect = nullptr;
    static inline shared_ptr<IPCSkeletonMock> skeleton = nullptr;
    static inline sptr<ServiceReverseProxyMock> srProxy = nullptr;
    static inline shared_ptr<ServiceMock> srvMock = nullptr;
};

void ServiceIncrementalTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    srvMock = make_shared<ServiceMock>();
    ServiceMock::serviceMock = srvMock;
    service = sptr(new Service(SERVICE_ID));
    param = make_shared<BackupParaMock>();
    BackupParaMock::backupPara = param;
    jsonUtil = make_shared<BJsonUtilMock>();
    BJsonUtilMock::jsonUtil = jsonUtil;
    session = make_shared<SvcSessionManagerMock>();
    SvcSessionManagerMock::sessionManager = session;
    svcProxy = sptr(new SvcExtensionProxyMock());
    bms = make_shared<BundleMgrAdapterMock>();
    BundleMgrAdapterMock::bms = bms;
    connect = sptr(new SvcBackupConnectionMock());
    SvcBackupConnectionMock::connect = connect;
    saConnect = make_shared<SABackupConnectionMock>();
    SABackupConnectionMock::saConnect = saConnect;
    skeleton = make_shared<IPCSkeletonMock>();
    IPCSkeletonMock::skeleton = skeleton;
    srProxy = sptr(new ServiceReverseProxyMock());
}

void ServiceIncrementalTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    service = nullptr;
    BackupParaMock::backupPara = nullptr;
    param = nullptr;
    BJsonUtilMock::jsonUtil = nullptr;
    jsonUtil = nullptr;
    SvcSessionManagerMock::sessionManager = nullptr;
    session = nullptr;
    svcProxy = nullptr;
    BundleMgrAdapterMock::bms = nullptr;
    bms = nullptr;
    SvcBackupConnectionMock::connect = nullptr;
    connect = nullptr;
    SABackupConnectionMock::saConnect = nullptr;
    saConnect = nullptr;
    IPCSkeletonMock::skeleton = nullptr;
    skeleton = nullptr;
    srProxy = nullptr;
    ServiceMock::serviceMock = nullptr;
    srvMock = nullptr;
}

/**
 * @tc.number: SUB_ServiceIncremental_GetLocalCapabilitiesIncremental_0000
 * @tc.name: SUB_ServiceIncremental_GetLocalCapabilitiesIncremental_0000
 * @tc.desc: 测试 GetLocalCapabilitiesIncremental 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_GetLocalCapabilitiesIncremental_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_GetLocalCapabilitiesIncremental_0000";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto fd = service->GetLocalCapabilitiesIncremental({});
        service->session_ = session_;
        EXPECT_EQ(static_cast<int>(fd), -ENOENT);

        service->isOccupyingSession_ = true;
        fd = service->GetLocalCapabilitiesIncremental({});
        EXPECT_EQ(static_cast<int>(fd), -ENOENT);

        service->isOccupyingSession_ = false;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::EXT_INVAL_ARG).GetCode()));
        fd = service->GetLocalCapabilitiesIncremental({});
        EXPECT_EQ(static_cast<int>(fd), -ENOENT);

        vector<BJsonEntityCaps::BundleInfo> info;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfosForIncremental(An<int32_t>(), An<const vector<BIncrementalData>&>()))
            .WillOnce(Return(info));
        fd = service->GetLocalCapabilitiesIncremental({});
        EXPECT_TRUE(static_cast<int>(fd) > 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetLocalCapabilitiesIncremental.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetLocalCapabilitiesIncremental_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_StartGetFdTask_0000
 * @tc.name: SUB_ServiceIncremental_StartGetFdTask_0000
 * @tc.desc: 测试 StartGetFdTask 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_StartGetFdTask_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_StartGetFdTask_0000";
    try {
        service->StartGetFdTask("", nullptr);
        EXPECT_TRUE(true);

        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_THROW(service->StartGetFdTask("", service), BError);

        service->session_ = session_;
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        EXPECT_THROW(service->StartGetFdTask("", service), BError);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        EXPECT_THROW(service->StartGetFdTask("", service), BError);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*svcProxy, User0OnBackup()).WillOnce(Return(BError(BError::Codes::EXT_INVAL_ARG).GetCode()));
        service->StartGetFdTask("", service);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*svcProxy, User0OnBackup()).WillOnce(Return(ERR_OK));
        service->StartGetFdTask("", service);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by StartGetFdTask.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_StartGetFdTask_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_StartGetFdTask_0100
 * @tc.name: SUB_ServiceIncremental_StartGetFdTask_0100
 * @tc.desc: 测试 StartGetFdTask 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_StartGetFdTask_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_StartGetFdTask_0100";
    try {
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(false));
        EXPECT_THROW(service->StartGetFdTask("", service), BError);

        BJsonUtil::BundleDetailInfo bundleInfo;
        vector<BJsonEntityCaps::BundleInfo> info;
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetLastIncrementalTime(_)).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfosForIncremental(An<const vector<BIncrementalData>&>(), An<int32_t>()))
            .WillOnce(Return(info));
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(bundleInfo));
        EXPECT_THROW(service->StartGetFdTask("", service), BError);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by StartGetFdTask.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_StartGetFdTask_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_RefreshBundleDataSize_0000
 * @tc.name: SUB_ServiceIncremental_RefreshBundleDataSize_0000
 * @tc.desc: 测试 RefreshBundleDataSize 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_RefreshBundleDataSize_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_RefreshBundleDataSize_0000";
    try {
        service->RefreshBundleDataSize({}, "", nullptr);
        EXPECT_TRUE(true);

        auto session_ = service->session_;
        service->session_ = nullptr;
        service->RefreshBundleDataSize({}, "", service);
        EXPECT_TRUE(true);

        service->session_ = session_;
        BJsonUtil::BundleDetailInfo bundleInfo;
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(bundleInfo));
        service->RefreshBundleDataSize({}, "", service);
        EXPECT_TRUE(true);

        bundleInfo.bundleIndex = 0;
        bundleInfo.bundleName = "bundleName";
        vector<BJsonEntityCaps::BundleInfo> infos;
        infos.emplace_back(BJsonEntityCaps::BundleInfo{.name = "bundleName", .appIndex = 0});
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(bundleInfo));
        service->RefreshBundleDataSize(infos, "", service);
        EXPECT_TRUE(true);

        bundleInfo.bundleIndex = 1;
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(bundleInfo));
        service->RefreshBundleDataSize(infos, "", service);
        EXPECT_TRUE(true);

        bundleInfo.bundleName = "appName";
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(bundleInfo));
        service->RefreshBundleDataSize(infos, "", service);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by RefreshBundleDataSize.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_RefreshBundleDataSize_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_GetAppLocalListAndDoIncrementalBackup_0000
 * @tc.name: SUB_ServiceIncremental_GetAppLocalListAndDoIncrementalBackup_0000
 * @tc.desc: 测试 GetAppLocalListAndDoIncrementalBackup 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_GetAppLocalListAndDoIncrementalBackup_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) <<
        "ServiceIncrementalTest-begin SUB_ServiceIncremental_GetAppLocalListAndDoIncrementalBackup_0000";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_EQ(service->GetAppLocalListAndDoIncrementalBackup(), BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        service->isOccupyingSession_ = true;
        EXPECT_EQ(service->GetAppLocalListAndDoIncrementalBackup(), BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->isOccupyingSession_ = false;
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_EQ(service->GetAppLocalListAndDoIncrementalBackup(), BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        EXPECT_EQ(service->GetAppLocalListAndDoIncrementalBackup(), BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetAppLocalListAndDoIncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetAppLocalListAndDoIncrementalBackup_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_InitIncrementalBackupSession_0000
 * @tc.name: SUB_ServiceIncremental_InitIncrementalBackupSession_0000
 * @tc.desc: 测试 InitIncrementalBackupSession 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_InitIncrementalBackupSession_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_InitIncrementalBackupSession_0000";
    try {
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr), BError(BError::Codes::SA_INVAL_ARG).GetCode());

        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr), BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetCallerName()).WillOnce(Return(""));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr), BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetCallerName()).WillOnce(Return(""));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::SA_SESSION_CONFLICT)));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr), BError(BError::Codes::SA_SESSION_CONFLICT).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetCallerName()).WillOnce(Return(""));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr), BError(BError::Codes::SA_INVAL_ARG).GetCode());

    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by InitIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_InitIncrementalBackupSession_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_InitIncrementalBackupSession_0100
 * @tc.name: SUB_ServiceIncremental_InitIncrementalBackupSession_0100
 * @tc.desc: 测试 InitIncrementalBackupSession with errMsg的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_InitIncrementalBackupSession_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_InitIncrementalBackupSession_0100";
    try {
        std::string errMsg;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr, errMsg),
            BError(BError::Codes::SA_INVAL_ARG).GetCode());

        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr, errMsg),
            BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetCallerName()).WillOnce(Return(""));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr, errMsg), BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetCallerName()).WillOnce(Return(""));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::SA_SESSION_CONFLICT)));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*session, GetSessionCallerName()).WillOnce(Return(""));
        EXPECT_CALL(*session, GetSessionActiveTime()).WillOnce(Return(""));
        EXPECT_CALL(*jsonUtil, BuildInitSessionErrInfo(_, _, _)).WillOnce(Return(""));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr, errMsg),
            BError(BError::Codes::SA_SESSION_CONFLICT).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetCallerName()).WillOnce(Return(""));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr, errMsg),
            BError(BError::Codes::SA_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by InitIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_InitIncrementalBackupSession_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0000
 * @tc.name: SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0000
 * @tc.desc: 测试 AppendBundlesIncrementalBackupSession 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) <<
        "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0000";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_EQ(service->AppendBundlesIncrementalBackupSession({}), BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        service->isOccupyingSession_ = true;
        EXPECT_EQ(service->AppendBundlesIncrementalBackupSession({}), BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->isOccupyingSession_ = false;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_EQ(service->AppendBundlesIncrementalBackupSession({}), BError(BError::Codes::SA_INVAL_ARG).GetCode());

        std::vector<std::string> supportBackupNames;
        vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfosForAppendBundles(_, _)).WillOnce(Return(bundleInfos));
        EXPECT_CALL(*srvMock, GetSupportBackupBundleNames(_, _, _)).WillOnce(Return(supportBackupNames));
        EXPECT_EQ(service->AppendBundlesIncrementalBackupSession({}), BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppendBundlesIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0100
 * @tc.name: SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0100
 * @tc.desc: 测试 AppendBundlesIncrementalBackupSession 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) <<
        "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0100";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto ret = service->AppendBundlesIncrementalBackupSession({}, {});
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        service->isOccupyingSession_ = true;
        ret = service->AppendBundlesIncrementalBackupSession({}, {});
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->isOccupyingSession_ = false;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = service->AppendBundlesIncrementalBackupSession({}, {});
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        std::vector<std::string> supportBackupNames;
        vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleInfos(_, _, _, _, _)).WillOnce(Return(bundleNameDetailMap));
        EXPECT_CALL(*bms, GetBundleInfosForAppendBundles(_, _)).WillOnce(Return(bundleInfos));
        EXPECT_CALL(*srvMock, GetSupportBackupBundleNames(_, _, _)).WillOnce(Return(supportBackupNames));
        ret = service->AppendBundlesIncrementalBackupSession({}, {});
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppendBundlesIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_PublishIncrementalFile_0000
 * @tc.name: SUB_ServiceIncremental_PublishIncrementalFile_0000
 * @tc.desc: 测试 PublishIncrementalFile 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_PublishIncrementalFile_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_PublishIncrementalFile_0000";
    try {
        BFileInfo fileInfo;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        auto ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        fileInfo.fileName = "test";
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, EPERM);

        fileInfo.fileName.clear();
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->session_ = session_;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, PublishIncrementalFile(_))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, PublishIncrementalFile(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by PublishIncrementalFile.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_PublishIncrementalFile_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_PublishSAIncrementalFile_0000
 * @tc.name: SUB_ServiceIncremental_PublishSAIncrementalFile_0000
 * @tc.desc: 测试 PublishSAIncrementalFile 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_PublishSAIncrementalFile_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_PublishSAIncrementalFile_0000";
    try {
        BFileInfo fileInfo;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        auto ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::SA_EXT_ERR_CALL).GetCode());

        fileInfo.owner = "abc";
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::SA_EXT_ERR_CALL).GetCode());

        fileInfo.owner = "123";
        shared_ptr<SABackupConnection> sa = nullptr;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        sa = make_shared<SABackupConnection>(nullptr, nullptr, nullptr, nullptr);
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        EXPECT_CALL(*saConnect, CallRestoreSA(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by PublishSAIncrementalFile.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_PublishSAIncrementalFile_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppIncrementalFileReady_0000
 * @tc.name: SUB_ServiceIncremental_AppIncrementalFileReady_0000
 * @tc.desc: 测试 AppIncrementalFileReady 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppIncrementalFileReady_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppIncrementalFileReady_0000";
    try {
        string bundleName;
        string fileName;
        int32_t errCode = 0;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnFileReady(_, _, _, _, _)).WillOnce(Return());
        auto ret = service->AppIncrementalFileReady(bundleName, fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalFileReady(bundleName, fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        fileName = BConstants::EXT_BACKUP_MANAGE;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, OnBundleExtManageInfo(_, _)).WillOnce(Return(UniqueFd(-1)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalFileReady(bundleName, fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppIncrementalFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppIncrementalFileReady_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppIncrementalFileReady_0100
 * @tc.name: SUB_ServiceIncremental_AppIncrementalFileReady_0100
 * @tc.desc: 测试 AppIncrementalFileReady 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppIncrementalFileReady_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppIncrementalFileReady_0100";
    try {
        string bundleName;
        string fileName;
        int32_t errCode = 0;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*srvMock, HandleCurBundleFileReady(_, _, _))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        auto ret = service->AppIncrementalFileReady(bundleName, fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        fileName = BConstants::EXT_BACKUP_MANAGE;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, OnBundleExtManageInfo(_, _)).WillOnce(Return(UniqueFd(-1)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*srvMock, HandleCurBundleFileReady(_, _, _)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->AppIncrementalFileReady(bundleName, fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppIncrementalFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppIncrementalFileReady_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppIncrementalFileReady_0200
 * @tc.name: SUB_ServiceIncremental_AppIncrementalFileReady_0200
 * @tc.desc: 测试 AppIncrementalFileReady 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppIncrementalFileReady_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppIncrementalFileReady_0200";
    try {
        string fileName;
        int32_t errCode = 0;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        auto ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
            EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnFileReady(_, _, _, _, _)).WillOnce(Return());
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        fileName = BConstants::EXT_BACKUP_MANAGE;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, OnBundleExtManageInfo(_, _)).WillOnce(Return(UniqueFd(-1)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppIncrementalFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppIncrementalFileReady_0200";
}

/**
* @tc.number: SUB_ServiceIncremental_AppIncrementalFileReady_0300
* @tc.name: SUB_ServiceIncremental_AppIncrementalFileReady_0300
* @tc.desc: 测试 AppIncrementalFileReady 的正常/异常分支
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: issueIAKC3I
*/
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppIncrementalFileReady_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppIncrementalFileReady_0300";
    try {
        string fileName;
        int32_t errCode = 0;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*srvMock, HandleCurBundleFileReady(_, _, _))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        auto ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        fileName = BConstants::EXT_BACKUP_MANAGE;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, OnBundleExtManageInfo(_, _)).WillOnce(Return(UniqueFd(-1)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*srvMock, HandleCurBundleFileReady(_, _, _)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppIncrementalFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppIncrementalFileReady_0300";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppIncrementalDone_0000
 * @tc.name: SUB_ServiceIncremental_AppIncrementalDone_0000
 * @tc.desc: 测试 AppIncrementalDone 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppIncrementalDone_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppIncrementalDone_0000";
    try {
        int32_t errCode = BError(BError::Codes::OK).GetCode();
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto ret = service->AppIncrementalDone(errCode);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = service->AppIncrementalDone(errCode);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalDone(errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        service->backupExtMutexMap_.clear();
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*srvMock, HandleCurAppDone(_, _, _))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = service->AppIncrementalDone(BError(BError::Codes::SA_INVAL_ARG).GetCode());
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->backupExtMutexMap_.clear();
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*srvMock, HandleCurAppDone(_, _, _)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->AppIncrementalDone(BError(BError::Codes::SA_INVAL_ARG).GetCode());
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*srvMock, HandleCurAppDone(_, _, _)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->AppIncrementalDone(errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppIncrementalDone.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppIncrementalDone_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_GetIncrementalFileHandle_0000
 * @tc.name: SUB_ServiceIncremental_GetIncrementalFileHandle_0000
 * @tc.desc: 测试 GetIncrementalFileHandle 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_GetIncrementalFileHandle_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_GetIncrementalFileHandle_0000";
    try {
        string bundleName;
        string fileName;
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::UNKNOWN));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetIncrementalFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetIncrementalFileHandle_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_GetIncrementalFileHandle_0100
 * @tc.name: SUB_ServiceIncremental_GetIncrementalFileHandle_0100
 * @tc.desc: 测试 GetIncrementalFileHandle 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_GetIncrementalFileHandle_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_GetIncrementalFileHandle_0100";
    try {
        string bundleName;
        string fileName;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalFileHandle(_)).WillOnce(Return(make_tuple(0, UniqueFd(-1), UniqueFd(-1))));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*srvMock, HandleCurBundleFileReady(_, _, _))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        auto ret = service->GetIncrementalFileHandle(bundleName, fileName);
            EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalFileHandle(_)).WillOnce(Return(make_tuple(0, UniqueFd(-1), UniqueFd(-1))));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnFileReady(_, _, _, _, _)).WillOnce(Return());
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::WAIT));
        EXPECT_CALL(*srvMock, UpdateToRestoreBundleMap(_, _)).WillOnce(Return(true));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetIncrementalFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetIncrementalFileHandle_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_IncrementalBackup_0000
 * @tc.name: SUB_ServiceIncremental_IncrementalBackup_0000
 * @tc.desc: 测试 IncrementalBackup 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_IncrementalBackup_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_IncrementalBackup_0000";
    try {
        string bundleName;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        EXPECT_TRUE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        EXPECT_TRUE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_FALSE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, IncrementalOnBackup(_)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_TRUE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, IncrementalOnBackup(_)).WillOnce(Return(1));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_TRUE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(false));
        EXPECT_FALSE(service->IncrementalBackup(bundleName));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by IncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_IncrementalBackup_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_IncrementalBackup_0100
 * @tc.name: SUB_ServiceIncremental_IncrementalBackup_0100
 * @tc.desc: 测试 IncrementalBackup 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_IncrementalBackup_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_IncrementalBackup_0100";
    try {
        string bundleName;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(false));
        EXPECT_FALSE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(false));
        EXPECT_FALSE(service->IncrementalBackup(bundleName));

        set<string> fileNameVec { "fileName" };
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE))
        .WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, HandleRestore(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy)).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_CALL(*session, GetOldBackupVersion()).WillOnce(Return(""));
        EXPECT_CALL(*session, GetExtFileNameRequest(_)).WillOnce(Return(fileNameVec));
        EXPECT_CALL(*svcProxy, GetIncrementalFileHandle(_))
            .WillOnce(Return(make_tuple(BError(BError::Codes::OK).GetCode(), UniqueFd(-1), UniqueFd(-1))));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_TRUE(service->IncrementalBackup(bundleName));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by IncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_IncrementalBackup_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_IncrementalBackup_0200
 * @tc.name: SUB_ServiceIncremental_IncrementalBackup_0200
 * @tc.desc: 测试 IncrementalBackup 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_IncrementalBackup_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_IncrementalBackup_0200";
    try {
        string bundleName;
        set<string> fileNameVec { "fileName" };
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE))
        .WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, HandleRestore(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy)).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_CALL(*session, GetOldBackupVersion()).WillOnce(Return("1.0.0"));
        EXPECT_CALL(*session, GetExtFileNameRequest(_)).WillOnce(Return(fileNameVec));
        EXPECT_CALL(*svcProxy, GetIncrementalFileHandle(_))
            .WillOnce(Return(make_tuple(BError(BError::Codes::SA_INVAL_ARG).GetCode(), UniqueFd(-1), UniqueFd(-1))));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_TRUE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_FALSE(service->IncrementalBackup(bundleName));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by IncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_IncrementalBackup_0200";
}

/**
 * @tc.number: SUB_ServiceIncremental_NotifyCallerCurAppIncrementDone_0000
 * @tc.name: SUB_ServiceIncremental_NotifyCallerCurAppIncrementDone_0000
 * @tc.desc: 测试 NotifyCallerCurAppIncrementDone 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_NotifyCallerCurAppIncrementDone_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_NotifyCallerCurAppIncrementDone_0000";
    try {
        ErrCode errCode = 0;
        string callerName;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        service->NotifyCallerCurAppIncrementDone(errCode, callerName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnBundleFinished(_, _)).WillOnce(Return());
        service->NotifyCallerCurAppIncrementDone(errCode, callerName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleFinished(_, _)).WillOnce(Return());
        service->NotifyCallerCurAppIncrementDone(errCode, callerName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by NotifyCallerCurAppIncrementDone.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_NotifyCallerCurAppIncrementDone_0200";
}

/**
 * @tc.number: SUB_ServiceIncremental_SendUserIdToApp_0000
 * @tc.name: SUB_ServiceIncremental_SendUserIdToApp_0000
 * @tc.desc: 测试 SendUserIdToApp 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_SendUserIdToApp_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_SendUserIdToApp_0000";
    try {
        string bundleName;
        int32_t userId = 100;
        auto session_ = service->session_;
        service->session_ = nullptr;
        service->SendUserIdToApp(bundleName, userId);
        service->session_ = session_;
        EXPECT_TRUE(true);

        EXPECT_CALL(*jsonUtil, BuildBundleInfoJson(_, _)).WillOnce(Return(false));
        service->SendUserIdToApp(bundleName, userId);
        EXPECT_TRUE(true);

        EXPECT_CALL(*jsonUtil, BuildBundleInfoJson(_, _)).WillOnce(Return(true));
        service->SendUserIdToApp(bundleName, userId);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by SendUserIdToApp.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_SendUserIdToApp_0200";
}

/**
 * @tc.number: SUB_ServiceIncremental_SetCurrentBackupSessProperties_0000
 * @tc.name: SUB_ServiceIncremental_SetCurrentBackupSessProperties_0000
 * @tc.desc: 测试 SetCurrentBackupSessProperties 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_SetCurrentBackupSessProperties_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_SetCurrentBackupSessProperties_0000";
    try {
        vector<string> bundleNames { "bundleName" };
        int32_t userId = 100;
        vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        service->SetCurrentBackupSessProperties(bundleNames, userId, bundleInfos, true);
        EXPECT_TRUE(true);

        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(true));
        service->SetCurrentBackupSessProperties(bundleNames, userId, bundleInfos, true);
        EXPECT_TRUE(true);

        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        service->SetCurrentBackupSessProperties(bundleNames, userId, bundleInfos, false);
        EXPECT_TRUE(true);

        BJsonEntityCaps::BundleInfo bundleInfo;
        bundleInfo.name = "bundleName";
        bundleInfos.push_back(bundleInfo);
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        service->SetCurrentBackupSessProperties(bundleNames, userId, bundleInfos, false);
        EXPECT_TRUE(true);

        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        service->SetCurrentBackupSessProperties(bundleNames, userId, bundleInfos, true);
        EXPECT_TRUE(true);
        service->session_ = session_;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by SetCurrentBackupSessProperties.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_SetCurrentBackupSessProperties_0200";
}

/**
 * @tc.number: SUB_ServiceIncremental_Release_0000
 * @tc.name: SUB_ServiceIncremental_Release_0000
 * @tc.desc: 测试 Release 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_Release_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_Release_0000";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto ret = service->Release();
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->session_ = session_;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        ret = service->Release();
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        ret = service->Release();
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        ret = service->Release();
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by Release.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_Release_0200";
}

/**
 * @tc.number: SUB_ServiceIncremental_SetBundleIncDataInfo_0000
 * @tc.name: SUB_ServiceIncremental_SetBundleIncDataInfo_0000
 * @tc.desc: 测试 SetBundleIncDataInfo 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_SetBundleIncDataInfo_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_SetBundleIncDataInfo_0000";
    try {
        vector<BIncrementalData> bundlesToBackup;
        vector<string> supportBundleNames;
        service->SetBundleIncDataInfo(bundlesToBackup, supportBundleNames);
        EXPECT_TRUE(true);

        BIncrementalData data;
        data.bundleName = "bundleName";
        bundlesToBackup.push_back(data);
        service->SetBundleIncDataInfo(bundlesToBackup, supportBundleNames);
        EXPECT_TRUE(true);

        supportBundleNames.emplace_back("bundleName");
        service->SetBundleIncDataInfo(bundlesToBackup, supportBundleNames);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by SetBundleIncDataInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_SetBundleIncDataInfo_0200";
}

/**
 * @tc.number: SUB_ServiceIncremental_CancelTask_0000
 * @tc.name: SUB_ServiceIncremental_CancelTask_0000
 * @tc.desc: 测试 CancelTask 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_CancelTask_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_CancelTask_0000";
    try {
        service->CancelTask("", nullptr);
        EXPECT_TRUE(true);

        auto session_ = service->session_;
        service->session_ = nullptr;
        service->CancelTask("", service);
        service->session_ = session_;
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        service->CancelTask("", service);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        service->CancelTask("", service);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, HandleClear()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        service->CancelTask("", service);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, HandleClear()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(false));
        service->CancelTask("", service);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by CancelTask.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_CancelTask_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_CancelTask_0100
 * @tc.name: SUB_ServiceIncremental_CancelTask_0100
 * @tc.desc: 测试 CancelTask 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_CancelTask_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_CancelTask_0100";
    try {
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, HandleClear()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(true));
        service->CancelTask("", service);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, HandleClear()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(false));
        service->CancelTask("", service);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, HandleClear()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(true));
        service->CancelTask("", service);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by CancelTask.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_CancelTask_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_Cancel_0000
 * @tc.name: SUB_ServiceIncremental_Cancel_0000
 * @tc.desc: 测试 Cancel 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_Cancel_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_Cancel_0000";
    try {
        string bundleName = "com.example.app2backup";
        BackupExtInfo info {};
        info.backupExtName = "com.example.app2backup";
        SvcSessionManager::Impl impl;
        int32_t result;

        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_EQ(service->Cancel(bundleName, result),
            BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK).GetCode());
        service->session_ = session_;

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::EXT_INVAL_ARG).GetCode()));
        auto ret = service->Cancel(bundleName, result);
        EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetImpl()).WillOnce(Return(impl));
        service->Cancel(bundleName, result);
        EXPECT_EQ(result, BError(BError::BackupErrorCode::E_CANCEL_NO_TASK).GetCode());

        impl.backupExtNameMap.insert(make_pair(bundleName, info));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetImpl()).WillOnce(Return(impl));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::UNKNOWN));
        service->Cancel(bundleName, result);
        EXPECT_EQ(result, BError(BError::BackupErrorCode::E_CANCEL_NO_TASK).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetImpl()).WillOnce(Return(impl));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::CLEAN));
        service->Cancel(bundleName, result);
        EXPECT_EQ(result, BError(BError::BackupErrorCode::E_CANCEL_NO_TASK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by Cancel.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_Cancel_0000";
}

/**
* @tc.number: SUB_ServiceIncremental_Cancel_0100
* @tc.name: SUB_ServiceIncremental_Cancel_0100
* @tc.desc: 测试 Cancel 的正常/异常分支
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: issueIAKC3I
*/
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_Cancel_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_Cancel_0000";
    try {
        string bundleName = "com.example.app2backup";
        BackupExtInfo info {};
        info.backupExtName = "com.example.app2backup";
        SvcSessionManager::Impl impl;
        int32_t result;

        impl.backupExtNameMap.insert(make_pair(bundleName, info));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetImpl()).WillOnce(Return(impl));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::START));
        service->Cancel(bundleName, result);
        EXPECT_EQ(result, BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetImpl()).WillOnce(Return(impl));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        service->Cancel(bundleName, result);
        EXPECT_EQ(result, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by Cancel.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_Cancel_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_GetExtensionMutex_0000
 * @tc.name: SUB_ServiceIncremental_GetExtensionMutex_0000
 * @tc.desc: 测试 GetExtensionMutex 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_GetExtensionMutex_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_GetExtensionMutex_0000";
    try {
        BundleName bundleName = "bundleName";
        service->backupExtMutexMap_.clear();
        auto ret = service->GetExtensionMutex(bundleName);
        EXPECT_TRUE(ret != nullptr);

        ret = service->GetExtensionMutex(bundleName);
        EXPECT_TRUE(ret != nullptr);
        service->backupExtMutexMap_.clear();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetExtensionMutex.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetExtensionMutex_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_RemoveExtensionMutex_0000
 * @tc.name: SUB_ServiceIncremental_RemoveExtensionMutex_0000
 * @tc.desc: 测试 RemoveExtensionMutex 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_RemoveExtensionMutex_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_RemoveExtensionMutex_0000";
    try {
        BundleName bundleName = "bundleName";
        service->backupExtMutexMap_.clear();
        service->RemoveExtensionMutex(bundleName);
        EXPECT_TRUE(true);

        service->backupExtMutexMap_[bundleName] = nullptr;
        service->RemoveExtensionMutex(bundleName);
        EXPECT_TRUE(true);
        service->backupExtMutexMap_.clear();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by RemoveExtensionMutex.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_RemoveExtensionMutex_0000";
}
}