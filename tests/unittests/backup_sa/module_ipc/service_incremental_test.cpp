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

#include "backup_para_mock.h"
#include "bms_adapter_mock.h"
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

void Service::OnStart() {}

void Service::OnStop() {}

UniqueFd Service::GetLocalCapabilities()
{
    return UniqueFd(-1);
}

void Service::StopAll(const wptr<IRemoteObject> &obj, bool force) {}

string Service::VerifyCallerAndGetCallerName()
{
    return "";
}

ErrCode Service::InitRestoreSession(sptr<IServiceReverse> remote)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::InitBackupSession(sptr<IServiceReverse> remote)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::Start()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppFileReady(const string &fileName, UniqueFd fd, int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppDone(ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::ServiceResultReport(const std::string restoreRetInfo,
    BackupRestoreScenario sennario, ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd, const std::vector<BundleName> &bundleNames,
    const std::vector<std::string> &detailInfos, RestoreTypeEnum restoreType, int32_t userId)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd,
                                             const std::vector<BundleName> &bundleNames,
                                             RestoreTypeEnum restoreType,
                                             int32_t userId)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesBackupSession(const std::vector<BundleName> &bundleNames)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesDetailsBackupSession(const std::vector<BundleName> &bundleNames,
    const std::vector<std::string> &bundleInfos)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::Finish()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::LaunchBackupExtension(const BundleName &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::LaunchBackupSAExtension(const BundleName &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::GetFileHandle(const string &bundleName, const string &fileName)
{
    return BError(BError::Codes::OK);
}

void Service::OnBackupExtensionDied(const string &&bundleName, bool isSecondCalled) {}

void Service::ExtConnectDied(const string &callName) {}

void Service::ExtStart(const string &bundleName) {}

int Service::Dump(int fd, const vector<u16string> &args)
{
    return 0;
}

void Service::ExtConnectFailed(const string &bundleName, ErrCode ret)
{
    GTEST_LOG_(INFO) << "ExtConnectFailed is OK";
}

void Service::ExtConnectDone(string bundleName) {}

void Service::ClearSessionAndSchedInfo(const string &bundleName) {}

void Service::VerifyCaller() {}

void Service::VerifyCaller(IServiceReverse::Scenario scenario) {}

void Service::OnAllBundlesFinished(ErrCode errCode) {}

void Service::OnStartSched() {}

void Service::SendStartAppGalleryNotify(const BundleName &bundleName) {}

void Service::SessionDeactive() {}

ErrCode Service::GetBackupInfo(BundleName &bundleName, std::string &result)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::UpdateTimer(BundleName &bundleName, uint32_t timeOut, bool &result)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::UpdateSendRate(std::string &bundleName, int32_t sendRate, bool &result)
{
    return BError(BError::Codes::OK);
}

void Service::OnSABackup(const std::string &bundleName,
                         const int &fd,
                         const std::string &result,
                         const ErrCode &errCode)
{
}

void Service::OnSARestore(const std::string &bundleName, const std::string &result, const ErrCode &errCode) {}

void Service::SendEndAppGalleryNotify(const BundleName &bundleName)
{
}

void Service::NoticeClientFinish(const string &bundleName, ErrCode errCode)
{
}

ErrCode Service::ReportAppProcessInfo(const std::string processInfo, BackupRestoreScenario sennario)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::StartExtTimer(bool &isExtStart)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::StartFwkTimer(bool &isFwkStart)
{
    return BError(BError::Codes::OK);
}

void Service::SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo>&, std::vector<std::string>&,
    RestoreTypeEnum) {}

void Service::SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo>&, std::vector<std::string>&,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>>&, std::map<std::string, bool>&, RestoreTypeEnum) {}

void Service::SetCurrentSessProperties(BJsonEntityCaps::BundleInfo&, std::map<std::string, bool>&,
    const std::string&) {}

void Service::ReleaseOnException() {}

bool SvcRestoreDepsManager::UpdateToRestoreBundleMap(const string&, const string&)
{
    return true;
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
};

void ServiceIncrementalTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
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
}

/**
 * @tc.number: SUB_ServiceIncremental_GetUserIdDefault_0000
 * @tc.name: SUB_ServiceIncremental_GetUserIdDefault_0000
 * @tc.desc: 测试 GetUserIdDefault 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_GetUserIdDefault_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_GetUserIdDefault_0000";
    try {
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        auto ret = GetUserIdDefault();
        EXPECT_EQ(ret, DEBUG_ID + 1);

        EXPECT_CALL(*param, GetBackupDebugOverrideAccount()).WillOnce(Return(make_pair<bool, int32_t>(false, 0)));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        ret = GetUserIdDefault();
        EXPECT_EQ(ret, BConstants::DEFAULT_USER_ID);

        EXPECT_CALL(*param, GetBackupDebugOverrideAccount()).WillOnce(Return(make_pair<bool, int32_t>(true, 0)));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        ret = GetUserIdDefault();
        EXPECT_EQ(ret, BConstants::DEFAULT_USER_ID);

        EXPECT_CALL(*param, GetBackupDebugOverrideAccount()).WillOnce(Return(make_pair<bool, int32_t>(false, 0)));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::XTS_UID));
        ret = GetUserIdDefault();
        EXPECT_EQ(ret, BConstants::DEFAULT_USER_ID);

        EXPECT_CALL(*param, GetBackupDebugOverrideAccount()).WillOnce(Return(make_pair<bool, int32_t>(false, 0)));
        EXPECT_CALL(*skeleton, GetCallingUid())
            .WillOnce(Return(BConstants::SPAN_USERID_UID + BConstants::SPAN_USERID_UID));
        ret = GetUserIdDefault();
        EXPECT_EQ(ret, 2);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetUserIdDefault.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetUserIdDefault_0000";
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

        vector<BJsonEntityCaps::BundleInfo> info;
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetLastIncrementalTime(_)).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfosForIncremental(An<const vector<BIncrementalData>&>(), An<int32_t>()))
            .WillOnce(Return(info));
        EXPECT_THROW(service->StartGetFdTask("", service), BError);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by StartGetFdTask.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_StartGetFdTask_0000";
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
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr), BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        EXPECT_CALL(*param, GetBackupDebugOverrideAccount()).WillOnce(Return(make_pair<bool, int32_t>(false, 0)));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr), BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by InitIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_InitIncrementalBackupSession_0000";
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

        vector<BJsonEntityCaps::BundleInfo> bundleInfos {{ .allToBackup = true }};
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfosForAppend(_, _)).WillOnce(Return(bundleInfos));
        EXPECT_EQ(service->AppendBundlesIncrementalBackupSession({}), BError(BError::Codes::OK).GetCode());

        bundleInfos[0].allToBackup = false;
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfosForAppend(_, _)).WillOnce(Return(bundleInfos));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_EQ(service->AppendBundlesIncrementalBackupSession({}), BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppendBundlesIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0000";
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
        fileInfo.fileName = "test";
        auto ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, EPERM);

        fileInfo.fileName.clear();
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->session_ = session_;
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, PublishIncrementalFile(_))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

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
        auto ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::SA_EXT_ERR_CALL).GetCode());

        fileInfo.owner = "abc";
        ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::SA_EXT_ERR_CALL).GetCode());

        fileInfo.owner = "123";
        shared_ptr<SABackupConnection> sa = nullptr;
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        sa = make_shared<SABackupConnection>(nullptr, nullptr, nullptr, nullptr);
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
        string fileName;
        int32_t errCode = 0;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnFileReady(_, _, _, _, _)).WillOnce(Return());
        auto ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        fileName = BConstants::EXT_BACKUP_MANAGE;
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
        string fileName;
        int32_t errCode = 0;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        auto ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy)).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnFileReady(_, _, _, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, HandleClear()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*srProxy, IncrementalBackupOnBundleFinished(_, _)).WillOnce(Return());
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppIncrementalFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppIncrementalFileReady_0100";
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

        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalDone(errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        service->backupExtMutexMap_[""] = make_shared<extensionInfo>("");
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        ret = service->AppIncrementalDone(errCode);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        errCode = BError(BError::Codes::SA_INVAL_ARG).GetCode();
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        ret = service->AppIncrementalDone(errCode);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
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
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        auto ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalFileHandle(_)).WillOnce(Return(0));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalFileHandle(_)).WillOnce(Return(1));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::WAIT));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetIncrementalFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetIncrementalFileHandle_0000";
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
        EXPECT_THROW(service->IncrementalBackup(bundleName), BError);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        EXPECT_THROW(service->IncrementalBackup(bundleName), BError);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        auto ret = service->IncrementalBackup(bundleName);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, IncrementalOnBackup(_)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnBundleStarted(_, _)).WillOnce(Return());
        ret = service->IncrementalBackup(bundleName);
        EXPECT_TRUE(ret);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, IncrementalOnBackup(_)).WillOnce(Return(1));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnBundleStarted(_, _)).WillOnce(Return());
        ret = service->IncrementalBackup(bundleName);
        EXPECT_TRUE(ret);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(false));
        ret = service->IncrementalBackup(bundleName);
        EXPECT_FALSE(ret);
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
        auto ret = service->IncrementalBackup(bundleName);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(false));
        ret = service->IncrementalBackup(bundleName);
        EXPECT_FALSE(ret);

        set<string> fileNameVec { "fileName" };
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, HandleRestore(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_CALL(*session, GetExtFileNameRequest(_)).WillOnce(Return(fileNameVec));
        EXPECT_CALL(*svcProxy, GetIncrementalFileHandle(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->IncrementalBackup(bundleName);
        EXPECT_TRUE(ret);
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
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, HandleRestore(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_CALL(*session, GetExtFileNameRequest(_)).WillOnce(Return(fileNameVec));
        EXPECT_CALL(*svcProxy, GetIncrementalFileHandle(_))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        auto ret = service->IncrementalBackup(bundleName);
        EXPECT_TRUE(ret);
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
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        service->SetCurrentBackupSessProperties(bundleNames, userId);
        EXPECT_TRUE(true);

        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(true));
        service->SetCurrentBackupSessProperties(bundleNames, userId);
        EXPECT_TRUE(true);
        service->session_ = session_;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by SetCurrentBackupSessProperties.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_SetCurrentBackupSessProperties_0200";
}
}