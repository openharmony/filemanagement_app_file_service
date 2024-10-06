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
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include "backup_para_mock.h"
#include "bms_adapter_mock.h"
#include "b_jsonutil_mock.h"
#include "ipc_skeleton_mock.h"
#include "sa_backup_connection_mock.h"
#include "service_reverse_proxy_mock.h"
#include "svc_backup_connection_mock.h"
#include "svc_extension_proxy_mock.h"
#include "svc_session_manager_mock.h"

#include "module_ipc/service.h"
#include "service.cpp"
#include "sub_service.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace testing::ext;

const string BUNDLE_NAME = "com.example.app2backup";
constexpr int32_t SERVICE_ID = 5203;

class ServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() {};
    void TearDown() {};

    static inline sptr<Service> servicePtr_ = nullptr;
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

void ServiceTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    servicePtr_ = sptr<Service>(new Service(SERVICE_ID));
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

void ServiceTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    servicePtr_ = nullptr;
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
 * @tc.number: SUB_Service_GetLocalCapabilities_0100
 * @tc.name: SUB_Service_GetLocalCapabilities_0100
 * @tc.desc: 测试 GetLocalCapabilities
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_GetLocalCapabilities_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetLocalCapabilities_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        servicePtr_->session_ = nullptr;
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_NE(fd, BError(BError::Codes::OK));

        servicePtr_->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(servicePtr_)));
        servicePtr_->isOccupyingSession_.store(true);
        fd = servicePtr_->GetLocalCapabilities();
        EXPECT_NE(fd, BError(BError::Codes::OK));
        servicePtr_->isOccupyingSession_.store(false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetLocalCapabilities_0100";
}

/**
 * @tc.number: SUB_Service_AppendBundlesRestoreSession_0100
 * @tc.name: SUB_Service_AppendBundlesRestoreSession_0100
 * @tc.desc: 测试 AppendBundlesRestoreSession
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_AppendBundlesRestoreSession_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppendBundlesRestoreSession_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(false, 0)))
            .WillOnce(Return(make_pair<bool, int32_t>(false, 0)));
        EXPECT_CALL(*skeleton, GetCallingUid())
            .WillOnce(Return(BConstants::SYSTEM_UID))
            .WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        std::vector<BundleName> bundleNames;
        std::vector<std::string> bundleInfos;
        RestoreTypeEnum restoreType = RESTORE_DATA_READDY;
        int32_t userId = 100;

        servicePtr_->session_ = nullptr;
        auto ret = servicePtr_->AppendBundlesRestoreSession(move(fd), bundleNames, bundleInfos, restoreType, userId);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        servicePtr_->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(servicePtr_)));
        servicePtr_->isOccupyingSession_.store(true);
        ret = servicePtr_->AppendBundlesRestoreSession(move(fd), bundleNames, bundleInfos, restoreType, userId);
        EXPECT_NE(ret, BError(BError::Codes::OK));
        servicePtr_->isOccupyingSession_.store(false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppendBundlesRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppendBundlesRestoreSession_0100";
}

/**
 * @tc.number: SUB_Service_AppendBundlesRestoreSession_0200
 * @tc.name: SUB_Service_AppendBundlesRestoreSession_0200
 * @tc.desc: 测试 AppendBundlesRestoreSession
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_AppendBundlesRestoreSession_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppendBundlesRestoreSession_0200";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(false, 0)))
            .WillOnce(Return(make_pair<bool, int32_t>(false, 0)));
        EXPECT_CALL(*skeleton, GetCallingUid())
            .WillOnce(Return(BConstants::SYSTEM_UID))
            .WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        std::vector<BundleName> bundleNames;
        std::vector<std::string> bundleInfos;
        RestoreTypeEnum restoreType = RESTORE_DATA_READDY;
        int32_t userId = DEFAULT_INVAL_VALUE;

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount()).WillOnce(Return(make_pair<bool, int32_t>(false, 0)));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*session, IsOnOnStartSched()).WillOnce(Return(false));
        auto ret = servicePtr_->AppendBundlesRestoreSession(move(fd), bundleNames, bundleInfos, restoreType, userId);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppendBundlesRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppendBundlesRestoreSession_0200";
}

/**
 * @tc.number: SUB_Service_SetCurrentSessProperties_0100
 * @tc.name: SUB_Service_SetCurrentSessProperties_0100
 * @tc.desc: 测试 SetCurrentSessProperties
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_SetCurrentSessProperties_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SetCurrentSessProperties_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        BJsonEntityCaps::BundleInfo info;
        std::map<std::string, bool> isClearDataFlags;
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(info.name, info.appIndex);
        servicePtr_->session_ = nullptr;
        servicePtr_->SetCurrentSessProperties(info, isClearDataFlags, bundleNameIndexInfo);
        servicePtr_->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(servicePtr_)));
        servicePtr_->SetCurrentSessProperties(info, isClearDataFlags, bundleNameIndexInfo);

        info = BJsonEntityCaps::BundleInfo{BUNDLE_NAME, 0, {}, {}, 0, 0, true, false, BUNDLE_NAME};
        isClearDataFlags = {{BUNDLE_NAME, true}};
        servicePtr_->SetCurrentSessProperties(info, isClearDataFlags, bundleNameIndexInfo);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SetCurrentSessProperties.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SetCurrentSessProperties_0100";
}

/**
 * @tc.number: SUB_Service_AppendBundlesBackupSession_0100
 * @tc.name: SUB_Service_AppendBundlesBackupSession_0100
 * @tc.desc: 测试 AppendBundlesBackupSession
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_AppendBundlesBackupSession_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppendBundlesBackupSession_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        std::vector<BundleName> bundleNames;

        servicePtr_->session_ = nullptr;
        auto ret = servicePtr_->AppendBundlesBackupSession(bundleNames);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        servicePtr_->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(servicePtr_)));
        servicePtr_->isOccupyingSession_.store(true);
        ret = servicePtr_->AppendBundlesBackupSession(bundleNames);
        EXPECT_NE(ret, BError(BError::Codes::OK));
        servicePtr_->isOccupyingSession_.store(false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppendBundlesBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppendBundlesBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_AppendBundlesBackupSession_0200
 * @tc.name: SUB_Service_AppendBundlesBackupSession_0200
 * @tc.desc: 测试 AppendBundlesBackupSession
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_AppendBundlesBackupSession_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppendBundlesBackupSession_0200";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        std::vector<BundleName> bundleNames;

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*session, IsOnOnStartSched()).WillOnce(Return(false));
        auto ret = servicePtr_->AppendBundlesBackupSession(bundleNames);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppendBundlesBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppendBundlesBackupSession_0200";
}


/**
 * @tc.number: SUB_Service_AppendBundlesDetailsBackupSession_0100
 * @tc.name: SUB_Service_AppendBundlesDetailsBackupSession_0100
 * @tc.desc: 测试 AppendBundlesDetailsBackupSession
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_AppendBundlesDetailsBackupSession_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppendBundlesDetailsBackupSession_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        std::vector<BundleName> bundleNames;
        std::vector<std::string> bundleInfos;

        servicePtr_->session_ = nullptr;
        auto ret = servicePtr_->AppendBundlesDetailsBackupSession(bundleNames, bundleInfos);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        servicePtr_->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(servicePtr_)));
        servicePtr_->isOccupyingSession_.store(true);
        ret = servicePtr_->AppendBundlesDetailsBackupSession(bundleNames, bundleInfos);
        EXPECT_NE(ret, BError(BError::Codes::OK));
        servicePtr_->isOccupyingSession_.store(false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppendBundlesDetailsBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppendBundlesDetailsBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_AppendBundlesDetailsBackupSession_0200
 * @tc.name: SUB_Service_AppendBundlesDetailsBackupSession_0200
 * @tc.desc: 测试 AppendBundlesDetailsBackupSession
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_AppendBundlesDetailsBackupSession_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppendBundlesDetailsBackupSession_0200";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        std::vector<BundleName> bundleNames;
        std::vector<std::string> bundleInfos;

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*session, IsOnOnStartSched()).WillOnce(Return(false));
        auto ret = servicePtr_->AppendBundlesDetailsBackupSession(bundleNames, bundleInfos);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppendBundlesDetailsBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppendBundlesDetailsBackupSession_0200";
}

/**
 * @tc.number: SUB_Service_PublishFile_0100
 * @tc.name: SUB_Service_PublishFile_0100
 * @tc.desc: 测试 PublishFile
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_PublishFile_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_PublishFile_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        BFileInfo fileInfo {BUNDLE_NAME, "", 0};

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        auto ret = servicePtr_->PublishFile(fileInfo);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        ret = servicePtr_->PublishFile(fileInfo);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, PublishFile(_))
        .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = servicePtr_->PublishFile(fileInfo);
        EXPECT_NE(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by PublishFile.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_PublishFile_0100";
}

/**
 * @tc.number: SUB_Service_AppDone_0100
 * @tc.name: SUB_Service_AppDone_0100
 * @tc.desc: 测试 AppDone
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_AppDone_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppDone_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        ErrCode errCode = BError(BError::Codes::OK).GetCode();

        servicePtr_->session_ = nullptr;
        auto ret = servicePtr_->AppDone(errCode);
        EXPECT_NE(ret, BError(BError::Codes::OK));
        servicePtr_->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(servicePtr_)));

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false)).WillOnce(Return(false));
        ret = servicePtr_->AppDone(errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppDone.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppDone_0100";
}

/**
 * @tc.number: SUB_Service_AppDone_0200
 * @tc.name: SUB_Service_AppDone_0200
 * @tc.desc: 测试 AppDone
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_AppDone_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppDone_0200";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        ErrCode errCode = BError(BError::Codes::SA_INVAL_ARG).GetCode();

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        auto ret = servicePtr_->AppDone(errCode);
        EXPECT_NE(ret, BError(BError::Codes::OK));


        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        ret = servicePtr_->AppDone(errCode);
        EXPECT_NE(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppDone.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppDone_0200";
}

/**
 * @tc.number: SUB_Service_NotifyCloneBundleFinish_0100
 * @tc.name: SUB_Service_NotifyCloneBundleFinish_0100
 * @tc.desc: 测试 NotifyCloneBundleFinish
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_NotifyCloneBundleFinish_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_NotifyCloneBundleFinish_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        std::string bundleName = BUNDLE_NAME;
        BackupRestoreScenario senario = BackupRestoreScenario::INCREMENTAL_BACKUP;
        servicePtr_->NotifyCloneBundleFinish(bundleName, senario);

        senario = BackupRestoreScenario::FULL_RESTORE;
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false))
            .WillOnce(Return(false)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        servicePtr_->NotifyCloneBundleFinish(bundleName, senario);

        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        servicePtr_->NotifyCloneBundleFinish(bundleName, senario);

        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        servicePtr_->NotifyCloneBundleFinish(bundleName, senario);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by NotifyCloneBundleFinish.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_NotifyCloneBundleFinish_0100";
}

/**
 * @tc.number: SUB_Service_LaunchBackupExtension_0100
 * @tc.name: SUB_Service_LaunchBackupExtension_0100
 * @tc.desc: 测试 LaunchBackupExtension
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_LaunchBackupExtension_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_LaunchBackupExtension_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        std::string bundleName = "";
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        auto ret = servicePtr_->LaunchBackupExtension(bundleName);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetBackupExtName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionCode(_)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetBundleRestoreType(_)).WillOnce(Return(RestoreTypeEnum::RESTORE_DATA_READDY));
        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        ret = servicePtr_->LaunchBackupExtension(bundleName);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetBackupExtName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionCode(_)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetBundleRestoreType(_)).WillOnce(Return(RestoreTypeEnum::RESTORE_DATA_READDY));
        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, IsExtAbilityConnected()).WillOnce(Return(true));
        EXPECT_CALL(*connect, WaitDisconnectDone()).WillOnce(Return(false));
        ret = servicePtr_->LaunchBackupExtension(bundleName);
        EXPECT_NE(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by LaunchBackupExtension.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_LaunchBackupExtension_0100";
}

/**
 * @tc.number: SUB_Service_LaunchBackupExtension_0200
 * @tc.name: SUB_Service_LaunchBackupExtension_0100
 * @tc.desc: 测试 LaunchBackupExtension
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_LaunchBackupExtension_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_LaunchBackupExtension_0200";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        std::string bundleName = "";

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetBackupExtName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionCode(_)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetBundleRestoreType(_)).WillOnce(Return(RestoreTypeEnum::RESTORE_DATA_READDY));
        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, IsExtAbilityConnected()).WillOnce(Return(false));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*connect, ConnectBackupExtAbility(_, _))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        auto ret = servicePtr_->LaunchBackupExtension(bundleName);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetBackupExtName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionCode(_)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetBundleRestoreType(_)).WillOnce(Return(RestoreTypeEnum::RESTORE_DATA_READDY));
        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, IsExtAbilityConnected()).WillOnce(Return(true));
        EXPECT_CALL(*connect, WaitDisconnectDone()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*connect, ConnectBackupExtAbility(_, _))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = servicePtr_->LaunchBackupExtension(bundleName);
        EXPECT_NE(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by LaunchBackupExtension.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_LaunchBackupExtension_0200";
}

/**
 * @tc.number: SUB_Service_ClearResidualBundleData_0100
 * @tc.name: SUB_Service_ClearResidualBundleData_0100
 * @tc.desc: 测试 ClearResidualBundleData
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_ClearResidualBundleData_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ClearResidualBundleData_0100";
    try {
        EXPECT_TRUE(servicePtr_ != nullptr);
        std::string bundleName = BUNDLE_NAME;
        servicePtr_->session_ = nullptr;
        servicePtr_->ClearResidualBundleData(bundleName);
        servicePtr_->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(servicePtr_)));

        try {
            EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
            servicePtr_->ClearResidualBundleData(bundleName);
            EXPECT_TRUE(false);
        } catch (...) {
            EXPECT_TRUE(true);
        }

        try {
            EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
            EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
            servicePtr_->ClearResidualBundleData(bundleName);
            EXPECT_TRUE(false);
        } catch (...) {
            EXPECT_TRUE(true);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ClearResidualBundleData.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ClearResidualBundleData_0100";
}
}