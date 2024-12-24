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

#include "accesstoken_kit_mock.h"
#include "app_gallery_dispose_proxy_mock.h"
#include "backup_para_mock.h"
#include "bms_adapter_mock.h"
#include "b_json_clear_data_config_mock.h"
#include "b_json_service_disposal_config_mock.h"
#include "b_json/b_json_entity_caps.h"
#include "b_jsonutil_mock.h"
#include "b_sa_utils_mock.h"
#include "ipc_skeleton_mock.h"
#include "notify_work_service_mock.h"
#include "sa_backup_connection_mock.h"
#include "service_reverse_proxy_mock.h"
#include "svc_backup_connection_mock.h"
#include "svc_extension_proxy_mock.h"
#include "svc_restore_deps_manager_mock.h"
#include "svc_session_manager_mock.h"
#include "system_ability_mock.h"

#include "service.cpp"
#include "sub_service.cpp"

namespace OHOS::FileManagement::Backup {
ErrCode Service::Release()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::Cancel(std::string bundleName, int32_t &result)
{
    result = BError(BError::Codes::OK);
    return BError(BError::Codes::OK);
}

UniqueFd Service::GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData>&)
{
    return UniqueFd(-1);
}

void Service::StartGetFdTask(std::string, wptr<Service>) {}

ErrCode Service::GetAppLocalListAndDoIncrementalBackup()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::InitIncrementalBackupSession(sptr<IServiceReverse>)
{
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

void Service::NotifyCallerCurAppIncrementDone(ErrCode, const std::string&) {}

void Service::SendUserIdToApp(string&, int32_t) {}

void Service::SetCurrentBackupSessProperties(const vector<string>&, int32_t,
    std::vector<BJsonEntityCaps::BundleInfo>&, bool) {}

std::shared_ptr<ExtensionMutexInfo> Service::GetExtensionMutex(const BundleName &bundleName)
{
    return make_shared<ExtensionMutexInfo>(bundleName);
}

void Service::RemoveExtensionMutex(const BundleName&) {}

void Service::CreateDirIfNotExist(const std::string&) {}
}

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
    static inline shared_ptr<BJsonClearDataConfigMock> cdConfig = nullptr;
    static inline shared_ptr<SAUtilsMock> saUtils = nullptr;
    static inline shared_ptr<AccessTokenKitMock> token = nullptr;
    static inline shared_ptr<BJsonDisposalConfigMock> jdConfig = nullptr;
    static inline shared_ptr<SystemAbilityMock> ability = nullptr;
    static inline shared_ptr<SvcRestoreDepsManagerMock> depManager = nullptr;
    static inline shared_ptr<NotifyWorkServiceMock> notify = nullptr;
    static inline shared_ptr<AppGalleryDisposeProxyMock> gallery = nullptr;
};

void ServiceTest::SetUpTestCase(void)
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
    cdConfig = make_shared<BJsonClearDataConfigMock>();
    BJsonClearDataConfigMock::config = cdConfig;
    saUtils = make_shared<SAUtilsMock>();
    SAUtilsMock::utils = saUtils;
    token = make_shared<AccessTokenKitMock>();
    AccessTokenKitMock::token = token;
    jdConfig = make_shared<BJsonDisposalConfigMock>();
    BJsonDisposalConfigMock::config = jdConfig;
    ability = make_shared<SystemAbilityMock>();
    SystemAbilityMock::ability = ability;
    depManager = make_shared<SvcRestoreDepsManagerMock>();
    SvcRestoreDepsManagerMock::manager = depManager;
    notify = make_shared<NotifyWorkServiceMock>();
    NotifyWorkServiceMock::notify = notify;
    gallery = make_shared<AppGalleryDisposeProxyMock>();
    AppGalleryDisposeProxyMock::proxy = gallery;
}

void ServiceTest::TearDownTestCase()
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
    BJsonClearDataConfigMock::config = nullptr;
    cdConfig = nullptr;
    SAUtilsMock::utils = nullptr;
    saUtils = nullptr;
    AccessTokenKitMock::token = nullptr;
    token = nullptr;
    BJsonDisposalConfigMock::config = nullptr;
    jdConfig = nullptr;
    SystemAbilityMock::ability = nullptr;
    ability = nullptr;
    SvcRestoreDepsManagerMock::manager = nullptr;
    depManager = nullptr;
    NotifyWorkServiceMock::notify = nullptr;
    notify = nullptr;
    AppGalleryDisposeProxyMock::proxy = nullptr;
    gallery = nullptr;
}

#include "sub_service_test.cpp"

/**
 * @tc.number: SUB_Service_GetUserIdDefault_0000
 * @tc.name: SUB_Service_GetUserIdDefault_0000
 * @tc.desc: 测试 GetUserIdDefault 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_GetUserIdDefault_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetUserIdDefault_0000";
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
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetUserIdDefault.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetUserIdDefault_0000";
}

/**
 * @tc.number: SUB_Service_OnStart_0100
 * @tc.name: SUB_Service_OnStart_0100
 * @tc.desc: 测试 OnStart
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_OnStart_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnStart_0100";
    try {
        vector<string> bundleNames;
        auto disposal = service->disposal_;
        auto clearRecorder = service->clearRecorder_;
        auto sched_ = service->sched_;
        service->disposal_ = nullptr;
        service->clearRecorder_ = nullptr;
        service->sched_ = nullptr;
        service->isOccupyingSession_ = false;

        EXPECT_CALL(*ability, Publish(_)).WillOnce(Return(false));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNames));
        service->OnStart();
        EXPECT_TRUE(true);

        service->disposal_ = disposal;
        EXPECT_CALL(*ability, Publish(_)).WillOnce(Return(false));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNames))
            .WillOnce(Return(bundleNames));
        service->OnStart();
        EXPECT_TRUE(true);

        service->clearRecorder_ = clearRecorder;
        EXPECT_CALL(*ability, Publish(_)).WillOnce(Return(false));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNames))
            .WillOnce(Return(bundleNames));
        EXPECT_CALL(*cdConfig, GetAllClearBundleRecords()).WillOnce(Return(bundleNames));
        service->OnStart();
        EXPECT_TRUE(true);

        service->sched_ = sched_;
        EXPECT_CALL(*ability, Publish(_)).WillOnce(Return(false));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNames))
            .WillOnce(Return(bundleNames));
        EXPECT_CALL(*cdConfig, GetAllClearBundleRecords()).WillOnce(Return(bundleNames));
        service->OnStart();
        EXPECT_TRUE(true);

        service->isOccupyingSession_ = false;
        EXPECT_CALL(*ability, Publish(_)).WillOnce(Return(false));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNames))
            .WillOnce(Return(bundleNames));
        EXPECT_CALL(*cdConfig, GetAllClearBundleRecords()).WillOnce(Return(bundleNames));
        service->OnStart();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnStart_0100";
}

/**
 * @tc.number: SUB_Service_OnStart_0200
 * @tc.name: SUB_Service_OnStart_0200
 * @tc.desc: 测试 OnStart
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_OnStart_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnStart_0200";
    try {
        service->isOccupyingSession_ = true;
        EXPECT_CALL(*ability, Publish(_)).WillOnce(Return(false));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(vector<string>()))
            .WillOnce(Return(vector<string>()));
        EXPECT_CALL(*cdConfig, GetAllClearBundleRecords()).WillOnce(Return(vector<string>()));
        service->OnStart();
        EXPECT_TRUE(true);

        service->isOccupyingSession_ = false;
        EXPECT_CALL(*ability, Publish(_)).WillOnce(Return(false));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(vector<string>(1)))
            .WillOnce(Return(vector<string>()));
        EXPECT_CALL(*cdConfig, GetAllClearBundleRecords()).WillOnce(Return(vector<string>()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)))
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        service->OnStart();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnStart_0200";
}

/**
 * @tc.number: SUB_Service_OnStart_0300
 * @tc.name: SUB_Service_OnStart_0300
 * @tc.desc: 测试 OnStart
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_OnStart_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnStart_0300";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_CALL(*ability, Publish(_)).WillOnce(Return(false));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(vector<string>()))
            .WillOnce(Return(vector<string>()));
        EXPECT_CALL(*cdConfig, GetAllClearBundleRecords()).WillOnce(Return(vector<string>(1)));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)))
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        service->OnStart();
        EXPECT_TRUE(true);

        EXPECT_CALL(*ability, Publish(_)).WillOnce(Return(false));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(vector<string>(1)))
            .WillOnce(Return(vector<string>()));
        EXPECT_CALL(*cdConfig, GetAllClearBundleRecords()).WillOnce(Return(vector<string>(1)));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)))
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)))
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        service->OnStart();
        service->session_ = session_;
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnStart_0300";
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
        ASSERT_TRUE(service != nullptr);
        service->session_ = nullptr;
        UniqueFd fd = service->GetLocalCapabilities();
        EXPECT_NE(fd, BError(BError::Codes::OK));

        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        service->isOccupyingSession_.store(true);
        fd = service->GetLocalCapabilities();
        EXPECT_NE(fd, BError(BError::Codes::OK));
        service->isOccupyingSession_.store(false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetLocalCapabilities_0100";
}

/**
 * @tc.number: SUB_Service_VerifyCallerAndGetCallerName_0100
 * @tc.name: SUB_Service_VerifyCallerAndGetCallerName_0100
 * @tc.desc: 测试 VerifyCallerAndGetCallerName
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_VerifyCallerAndGetCallerName_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_VerifyCallerAndGetCallerName_0100";
    try {
        ASSERT_TRUE(service != nullptr);
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(-1));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_THROW(service->VerifyCallerAndGetCallerName(), BError);

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        auto ret = service->VerifyCallerAndGetCallerName();
        EXPECT_TRUE(ret.empty());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_THROW(service->VerifyCallerAndGetCallerName(), BError);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by VerifyCallerAndGetCallerName.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_VerifyCallerAndGetCallerName_0100";
}

/**
 * @tc.number: SUB_Service_VerifyCaller_0100
 * @tc.name: SUB_Service_VerifyCaller_0100
 * @tc.desc: 测试 VerifyCaller
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_VerifyCaller_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_VerifyCaller_0100";
    try {
        ASSERT_TRUE(service != nullptr);
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillOnce(Return(Security::AccessToken::PermissionState::PERMISSION_DENIED));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_THROW(service->VerifyCaller(), BError);

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillOnce(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        service->VerifyCaller();
        EXPECT_TRUE(true);

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillOnce(Return(Security::AccessToken::PermissionState::PERMISSION_DENIED));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_THROW(service->VerifyCaller(), BError);

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillOnce(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*token, IsSystemAppByFullTokenID(_)).WillOnce(Return(false));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_THROW(service->VerifyCaller(), BError);

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillOnce(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*token, IsSystemAppByFullTokenID(_)).WillOnce(Return(true));
        service->VerifyCaller();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by VerifyCaller.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_VerifyCaller_0100";
}

/**
 * @tc.number: SUB_Service_VerifyCaller_0200
 * @tc.name: SUB_Service_VerifyCaller_0200
 * @tc.desc: 测试 VerifyCaller
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_VerifyCaller_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_VerifyCaller_0200";
    try {
        ASSERT_TRUE(service != nullptr);
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::XTS_UID));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_THROW(service->VerifyCaller(), BError);

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        service->VerifyCaller();
        EXPECT_TRUE(true);

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_INVALID));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_THROW(service->VerifyCaller(), BError);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by VerifyCaller.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_VerifyCaller_0200";
}

/**
 * @tc.number: SUB_Service_SpecialVersion_0200
 * @tc.name: SUB_Service_SpecialVersion_0200
 * @tc.desc: 测试 SpecialVersion
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_SpecialVersion_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SpecialVersion_0200";
    try {
        string versionName(BConstants::DEFAULT_VERSION_NAME);
        auto ret = SpecialVersion(versionName);
        EXPECT_TRUE(ret);

        versionName.clear();
        ret = SpecialVersion(versionName);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SpecialVersion.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SpecialVersion_0200";
}

/**
 * @tc.number: SUB_Service_OnBundleStarted_0100
 * @tc.name: SUB_Service_OnBundleStarted_0100
 * @tc.desc: 测试 OnBundleStarted
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_OnBundleStarted_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnBundleStarted_0100";
    try {
        BError error(BError::Codes::OK);
        sptr<SvcSessionManager> session_ = service->session_;
        BundleName bundleName;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        service->OnBundleStarted(error, session_, bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnBundleStarted(_, _)).WillOnce(Return());
        service->OnBundleStarted(error, session_, bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnBundleStarted(_, _)).WillOnce(Return());
        service->OnBundleStarted(error, session_, bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleStarted(_, _)).WillOnce(Return());
        service->OnBundleStarted(error, session_, bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnBundleStarted_0100";
}

/**
 * @tc.number: SUB_Service_GetRestoreBundleNames_0100
 * @tc.name: SUB_Service_GetRestoreBundleNames_0100
 * @tc.desc: 测试 GetRestoreBundleNames
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_GetRestoreBundleNames_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetRestoreBundleNames_0100";
    try {
        vector<BundleName> bundleNames;
        vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        std::string backupVersion;
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfos(_, _)).WillOnce(Return(bundleInfos));
        EXPECT_THROW(GetRestoreBundleNames(UniqueFd(-1), service->session_, bundleNames, backupVersion), BError);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetRestoreBundleNames.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetRestoreBundleNames_0100";
}

/**
 * @tc.number: SUB_Service_HandleExceptionOnAppendBundles_0100
 * @tc.name: SUB_Service_HandleExceptionOnAppendBundles_0100
 * @tc.desc: 测试 HandleExceptionOnAppendBundles
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_HandleExceptionOnAppendBundles_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_HandleExceptionOnAppendBundles_0100";
    try {
        vector<BundleName> appendBundleNames { "bundleName" };
        vector<BundleName> restoreBundleNames;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        service->HandleExceptionOnAppendBundles(service->session_, appendBundleNames, restoreBundleNames);
        EXPECT_TRUE(true);

        restoreBundleNames.emplace_back("bundleName");
        restoreBundleNames.emplace_back("bundleName2");
        service->HandleExceptionOnAppendBundles(service->session_, appendBundleNames, restoreBundleNames);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by HandleExceptionOnAppendBundles.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_HandleExceptionOnAppendBundles_0100";
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
        ASSERT_TRUE(service != nullptr);
        std::vector<BundleName> bundleNames;
        std::vector<std::string> bundleInfos;
        RestoreTypeEnum restoreType = RESTORE_DATA_READDY;
        int32_t userId = 100;

        service->session_ = nullptr;
        auto ret = service->AppendBundlesRestoreSession(UniqueFd(-1), bundleNames, bundleInfos, restoreType, userId);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG));

        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        service->isOccupyingSession_.store(true);
        ret = service->AppendBundlesRestoreSession(UniqueFd(-1), bundleNames, bundleInfos, restoreType, userId);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG));
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
        ASSERT_TRUE(service != nullptr);
        vector<string> bundleInfos;
        vector<BundleName> bundleNames;
        vector<BJsonEntityCaps::BundleInfo> infos;
        RestoreTypeEnum restoreType = RESTORE_DATA_READDY;
        int32_t userId = DEFAULT_INVAL_VALUE;
        map<string, vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap;
        service->isOccupyingSession_.store(false);
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleInfos(_, _, _, _, _)).WillOnce(Return(bundleNameDetailMap));
        EXPECT_CALL(*bms, GetBundleInfos(_, _)).WillOnce(Return(infos));
        auto ret = service->AppendBundlesRestoreSession(UniqueFd(-1), bundleNames, bundleInfos, restoreType, userId);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG));
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
        ASSERT_TRUE(service != nullptr);
        BJsonEntityCaps::BundleInfo info;
        map<string, bool> isClearDataFlags;
        string bundleNameIndexInfo;

        service->session_ = nullptr;
        service->SetCurrentSessProperties(info, isClearDataFlags, bundleNameIndexInfo);
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        service->SetCurrentSessProperties(info, isClearDataFlags, bundleNameIndexInfo);

        info = BJsonEntityCaps::BundleInfo{BUNDLE_NAME, 0, {}, {}, 0, 0, true, false, BUNDLE_NAME};
        isClearDataFlags = {{BUNDLE_NAME, true}};
        service->SetCurrentSessProperties(info, isClearDataFlags, bundleNameIndexInfo);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SetCurrentSessProperties.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SetCurrentSessProperties_0100";
}

/**
 * @tc.number: SUB_Service_SetCurrentSessProperties_0200
 * @tc.name: SUB_Service_SetCurrentSessProperties_0200
 * @tc.desc: 测试 SetCurrentSessProperties
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_SetCurrentSessProperties_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SetCurrentSessProperties_0200";
    try {
        ASSERT_TRUE(service != nullptr);
        vector<BJsonEntityCaps::BundleInfo> restoreBundleInfos = {
            {.name = "bundleName", .appIndex = 0, .allToBackup = false, .versionName = ""} };
        vector<string> restoreBundleNames;
        map<string, vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap;
        map<string, bool> isClearDataFlags;
        RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND;
        std::string backupVersion;
        EXPECT_THROW(service->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, bundleNameDetailMap,
            isClearDataFlags, restoreType, backupVersion), BError);

        restoreBundleNames.emplace_back("bundleName");
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"))
            .WillOnce(Return("bundleName"));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        service->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, bundleNameDetailMap,
            isClearDataFlags, restoreType, backupVersion);
        EXPECT_TRUE(true);

        restoreBundleInfos[0].allToBackup = true;
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"))
            .WillOnce(Return("bundleName"));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        service->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, bundleNameDetailMap,
            isClearDataFlags, restoreType, backupVersion);
        EXPECT_TRUE(true);

        restoreBundleInfos[0].allToBackup = false;
        restoreBundleInfos[0].versionName = string(BConstants::DEFAULT_VERSION_NAME);
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"))
            .WillOnce(Return("bundleName"));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        service->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, bundleNameDetailMap,
            isClearDataFlags, restoreType, backupVersion);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SetCurrentSessProperties.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SetCurrentSessProperties_0200";
}

/**
 * @tc.number: SUB_Service_SetCurrentSessProperties_0300
 * @tc.name: SUB_Service_SetCurrentSessProperties_0300
 * @tc.desc: 测试 SetCurrentSessProperties
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_SetCurrentSessProperties_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SetCurrentSessProperties_0300";
    try {
        ASSERT_TRUE(service != nullptr);
        vector<BJsonEntityCaps::BundleInfo> restoreBundleInfos = {
            {.name = "bundleName", .appIndex = 0, .allToBackup = true, .extensionName = ""} };
        vector<string> restoreBundleNames { "bundleName" };
        map<string, vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap;
        map<string, bool> isClearDataFlags;
        RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND;
        std::string backupVersion;

        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"))
            .WillOnce(Return("bundleName"));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*jsonUtil, FindBundleInfoByName(_, _, _, _)).WillOnce(Return(false)).WillOnce(Return(false));
        service->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, bundleNameDetailMap,
            isClearDataFlags, restoreType, backupVersion);
        EXPECT_TRUE(true);

        restoreBundleInfos[0].extensionName = "extensionName";
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"))
            .WillOnce(Return("bundleName"));
        EXPECT_CALL(*jsonUtil, FindBundleInfoByName(_, _, _, _)).WillOnce(Return(false)).WillOnce(Return(false));
        service->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, bundleNameDetailMap,
            isClearDataFlags, restoreType, backupVersion);
        EXPECT_TRUE(true);

        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"))
            .WillOnce(Return("bundleName"));
        EXPECT_CALL(*jsonUtil, FindBundleInfoByName(_, _, _, _)).WillOnce(Return(false)).WillOnce(Return(false));
        service->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, bundleNameDetailMap,
            isClearDataFlags, restoreType, backupVersion);

        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"))
            .WillOnce(Return("bundleName"));
        EXPECT_CALL(*jsonUtil, FindBundleInfoByName(_, _, _, _)).WillOnce(Return(true)).WillOnce(Return(false));
        EXPECT_CALL(*notify, NotifyBundleDetail(_)).WillOnce(Return(true));
        service->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, bundleNameDetailMap,
            isClearDataFlags, restoreType, backupVersion);

        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"))
            .WillOnce(Return("bundleName"));
        EXPECT_CALL(*jsonUtil, FindBundleInfoByName(_, _, _, _)).WillOnce(Return(false)).WillOnce(Return(true));
        service->SetCurrentSessProperties(restoreBundleInfos, restoreBundleNames, bundleNameDetailMap,
            isClearDataFlags, restoreType, backupVersion);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SetCurrentSessProperties.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SetCurrentSessProperties_0300";
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
        ASSERT_TRUE(service != nullptr);
        std::vector<BundleName> bundleNames;

        service->session_ = nullptr;
        auto ret = service->AppendBundlesBackupSession(bundleNames);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        service->isOccupyingSession_.store(true);
        ret = service->AppendBundlesBackupSession(bundleNames);
        EXPECT_NE(ret, BError(BError::Codes::OK));
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
        ASSERT_TRUE(service != nullptr);
        vector<BundleName> bundleNames;
        vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        service->isOccupyingSession_.store(false);
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfosForAppend(_, _)).WillOnce(Return(bundleInfos));
        EXPECT_CALL(*session, IsOnOnStartSched()).WillOnce(Return(false));
        auto ret = service->AppendBundlesBackupSession(bundleNames);
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
        ASSERT_TRUE(service != nullptr);
        std::vector<BundleName> bundleNames;
        std::vector<std::string> bundleInfos;

        service->session_ = nullptr;
        auto ret = service->AppendBundlesDetailsBackupSession(bundleNames, bundleInfos);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        service->isOccupyingSession_.store(true);
        ret = service->AppendBundlesDetailsBackupSession(bundleNames, bundleInfos);
        EXPECT_NE(ret, BError(BError::Codes::OK));
        service->isOccupyingSession_.store(false);
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
        ASSERT_TRUE(service != nullptr);
        vector<BundleName> bundleNames;
        vector<string> bundleInfos;
        vector<BJsonEntityCaps::BundleInfo> infos;
        map<string, vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap;
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*jsonUtil, BuildBundleInfos(_, _, _, _, _)).WillOnce(Return(bundleNameDetailMap));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfosForAppend(_, _)).WillOnce(Return(infos));
        EXPECT_CALL(*session, IsOnOnStartSched()).WillOnce(Return(false));
        auto ret = service->AppendBundlesDetailsBackupSession(bundleNames, bundleInfos);
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
        ASSERT_TRUE(service != nullptr);
        BFileInfo fileInfo {BUNDLE_NAME, "", 0};

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        auto ret = service->PublishFile(fileInfo);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        ret = service->PublishFile(fileInfo);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, PublishFile(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = service->PublishFile(fileInfo);
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
        ASSERT_TRUE(service != nullptr);
        ErrCode errCode = BError(BError::Codes::OK).GetCode();

        service->session_ = nullptr;
        auto ret = service->AppDone(errCode);
        EXPECT_NE(ret, BError(BError::Codes::OK));
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        ret = service->AppDone(errCode);
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
        ASSERT_TRUE(service != nullptr);
        ErrCode errCode = BError(BError::Codes::SA_INVAL_ARG).GetCode();

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        auto ret = service->AppDone(errCode);
        EXPECT_NE(ret, BError(BError::Codes::OK));

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        ret = service->AppDone(errCode);
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
        ASSERT_TRUE(service != nullptr);
        std::string bundleName = BUNDLE_NAME;
        BackupRestoreScenario senario = BackupRestoreScenario::INCREMENTAL_BACKUP;
        service->NotifyCloneBundleFinish(bundleName, senario);
        EXPECT_TRUE(true);

        senario = BackupRestoreScenario::FULL_RESTORE;
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->NotifyCloneBundleFinish(bundleName, senario);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->NotifyCloneBundleFinish(bundleName, senario);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->NotifyCloneBundleFinish(bundleName, senario);
        EXPECT_TRUE(true);
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
        ASSERT_TRUE(service != nullptr);
        std::string bundleName = "";
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        auto ret = service->LaunchBackupExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG));

        BJsonUtil::BundleDetailInfo info;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(info));
        EXPECT_CALL(*session, GetBackupExtName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionCode(_)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetBundleRestoreType(_)).WillOnce(Return(RestoreTypeEnum::RESTORE_DATA_READDY));
        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        ret = service->LaunchBackupExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(info));
        EXPECT_CALL(*session, GetBackupExtName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionCode(_)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetBundleRestoreType(_)).WillOnce(Return(RestoreTypeEnum::RESTORE_DATA_READDY));
        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, IsExtAbilityConnected()).WillOnce(Return(true));
        EXPECT_CALL(*connect, WaitDisconnectDone()).WillOnce(Return(false));
        ret = service->LaunchBackupExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG));
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
        ASSERT_TRUE(service != nullptr);
        string bundleName = "";
        BJsonUtil::BundleDetailInfo info;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(info));
        EXPECT_CALL(*session, GetBackupExtName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionCode(_)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetBundleRestoreType(_)).WillOnce(Return(RestoreTypeEnum::RESTORE_DATA_READDY));
        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, IsExtAbilityConnected()).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::START));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*connect, ConnectBackupExtAbility(_, _, _))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        auto ret = service->LaunchBackupExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_BOOT_EXT_FAIL));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(info));
        EXPECT_CALL(*session, GetBackupExtName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionName(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetBundleVersionCode(_)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetBundleRestoreType(_)).WillOnce(Return(RestoreTypeEnum::RESTORE_DATA_READDY));
        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, IsExtAbilityConnected()).WillOnce(Return(true));
        EXPECT_CALL(*connect, WaitDisconnectDone()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::START));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*connect, ConnectBackupExtAbility(_, _, _))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        ret = service->LaunchBackupExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_BOOT_EXT_FAIL));
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
        ASSERT_TRUE(service != nullptr);
        std::string bundleName = BUNDLE_NAME;
        service->session_ = nullptr;
        service->ClearResidualBundleData(bundleName);
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));

        try {
            EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
            service->ClearResidualBundleData(bundleName);
            EXPECT_TRUE(false);
        } catch (...) {
            EXPECT_TRUE(true);
        }

        try {
            EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
            EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
            service->ClearResidualBundleData(bundleName);
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

/**
 * @tc.number: SUB_Service_AppendBundlesClearSession_0000
 * @tc.name: SUB_Service_AppendBundlesClearSession_0000
 * @tc.desc: 测试 AppendBundlesClearSession 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_AppendBundlesClearSession_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AppendBundlesClearSession_0000";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto ret = service->AppendBundlesClearSession({ "bundleNames" });
        service->session_ = session_;
        EXPECT_EQ(ret, EPERM);

        ret = service->AppendBundlesClearSession({});
        EXPECT_EQ(ret, EPERM);

        vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        BJsonEntityCaps::BundleInfo info;
        info.name = "bundleNames";
        info.appIndex = 0;
        bundleInfos.push_back(info);
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfos(_, _)).WillOnce(Return(bundleInfos));
        EXPECT_CALL(*session, IsOnOnStartSched()).WillOnce(Return(false));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"))
            .WillOnce(Return("bundleName"));
        ret = service->AppendBundlesClearSession({ "bundleNames" });
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AppendBundlesClearSession.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AppendBundlesClearSession_0000";
}

/**
 * @tc.number: SUB_Service_UpdateTimer_0000
 * @tc.name: SUB_Service_UpdateTimer_0000
 * @tc.desc: 测试 UpdateTimer 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_UpdateTimer_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_UpdateTimer_0000";
    try {
        string bundleName;
        bool result = 0;
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto ret = service->UpdateTimer(bundleName, 0, result);
        service->session_ = session_;
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->isOccupyingSession_ = true;
        ret = service->UpdateTimer(bundleName, 0, result);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->isOccupyingSession_ = false;
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*session, UpdateTimer(_, _, _)).WillOnce(Return(false));
        ret = service->UpdateTimer(bundleName, 0, result);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by UpdateTimer.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_UpdateTimer_0000";
}

/**
 * @tc.number: SUB_Service_UpdateSendRate_0000
 * @tc.name: SUB_Service_UpdateSendRate_0000
 * @tc.desc: 测试 UpdateSendRate 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_UpdateSendRate_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_UpdateSendRate_0000";
    try {
        string bundleName;
        bool result = 0;
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto ret = service->UpdateSendRate(bundleName, 0, result);
        service->session_ = session_;
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->isOccupyingSession_ = true;
        ret = service->UpdateSendRate(bundleName, 0, result);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->isOccupyingSession_ = false;
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        ret = service->UpdateSendRate(bundleName, 0, result);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        ret = service->UpdateSendRate(bundleName, 0, result);
        EXPECT_EQ(ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by UpdateSendRate.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_UpdateSendRate_0000";
}

/**
 * @tc.number: SUB_Service_UpdateSendRate_0100
 * @tc.name: SUB_Service_UpdateSendRate_0100
 * @tc.desc: 测试 UpdateSendRate 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_UpdateSendRate_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_UpdateSendRate_0100";
    try {
        string bundleName;
        bool result = 0;
        service->isOccupyingSession_ = false;
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, UpdateFdSendRate(_, _)).WillOnce(Return(EPERM));
        auto ret = service->UpdateSendRate(bundleName, 0, result);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_BROKEN_IPC).GetCode());

        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, UpdateFdSendRate(_, _)).WillOnce(Return(NO_ERROR));
        ret = service->UpdateSendRate(bundleName, 0, result);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by UpdateSendRate.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_UpdateSendRate_0100";
}

/**
 * @tc.number: SUB_Service_BackupSA_0000
 * @tc.name: SUB_Service_BackupSA_0000
 * @tc.desc: 测试 BackupSA 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_BackupSA_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_BackupSA_0000";
    try {
        string bundleName;
        shared_ptr<SABackupConnection> sa = nullptr;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        auto ret = service->BackupSA(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        sa = make_shared<SABackupConnection>(nullptr, nullptr, nullptr, nullptr);
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        ret = service->BackupSA(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        EXPECT_CALL(*saConnect, CallBackupSA()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, BackupOnBundleStarted(_, _)).WillOnce(Return());
        ret = service->BackupSA(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        EXPECT_CALL(*saConnect, CallBackupSA()).WillOnce(Return(BError(BError::Codes::SA_BOOT_EXT_FAIL).GetCode()));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, BackupOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(false));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        ret = service->BackupSA(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_BOOT_EXT_FAIL).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleStarted(_, _)).WillOnce(Return());
        ret = service->BackupSA(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by BackupSA.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_BackupSA_0000";
}


/**
 * @tc.number: SUB_Service_SADone_0000
 * @tc.name: SUB_Service_SADone_0000
 * @tc.desc: 测试 SADone 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_SADone_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SADone_0000";
    try {
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        auto ret = service->SADone(0, "");
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        shared_ptr<SABackupConnection> sa = nullptr;
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        ret = service->SADone(0, "");
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        sa = make_shared<SABackupConnection>(nullptr, nullptr, nullptr, nullptr);
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        EXPECT_CALL(*saConnect, DisconnectBackupSAExt()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        ret = service->SADone(0, "");
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SADone.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SADone_0000";
}

/**
 * @tc.number: SUB_Service_NotifyCallerCurAppDone_0000
 * @tc.name: SUB_Service_NotifyCallerCurAppDone_0000
 * @tc.desc: 测试 NotifyCallerCurAppDone 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_NotifyCallerCurAppDone_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_NotifyCallerCurAppDone_0000";
    try {
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        service->NotifyCallerCurAppDone(0, "");
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, BackupOnBundleFinished(_, _)).WillOnce(Return());
        service->NotifyCallerCurAppDone(0, "");
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnBundleFinished(_, _)).WillOnce(Return());
        service->NotifyCallerCurAppDone(0, "");
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by NotifyCallerCurAppDone.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_NotifyCallerCurAppDone_0000";
}

/**
 * @tc.number: SUB_Service_ReportAppProcessInfo_0000
 * @tc.name: SUB_Service_ReportAppProcessInfo_0000
 * @tc.desc: 测试 ReportAppProcessInfo 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_ReportAppProcessInfo_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ReportAppProcessInfo_0000";
    try {
        string processInfo;
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(OHOS::Security::AccessToken::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnProcessInfo(_, _)).WillOnce(Return());
        auto ret = service->ReportAppProcessInfo(processInfo, BackupRestoreScenario::FULL_RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(OHOS::Security::AccessToken::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnProcessInfo(_, _)).WillOnce(Return());
        ret = service->ReportAppProcessInfo(processInfo, BackupRestoreScenario::INCREMENTAL_RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(OHOS::Security::AccessToken::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, BackupOnProcessInfo(_, _)).WillOnce(Return());
        ret = service->ReportAppProcessInfo(processInfo, BackupRestoreScenario::FULL_BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(OHOS::Security::AccessToken::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnProcessInfo(_, _)).WillOnce(Return());
        ret = service->ReportAppProcessInfo(processInfo, BackupRestoreScenario::INCREMENTAL_BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ReportAppProcessInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ReportAppProcessInfo_0000";
}

/**
 * @tc.number: SUB_Service_TimeOutCallback_0000
 * @tc.name: SUB_Service_TimeOutCallback_0000
 * @tc.desc: 测试 TimeOutCallback 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_TimeOutCallback_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_TimeOutCallback_0000";
    try {
        string bundleName;
        auto fun = service->TimeOutCallback(nullptr, bundleName);
        fun();
        EXPECT_TRUE(true);

        auto session_ = service->session_;
        service->session_ = nullptr;
        fun = service->TimeOutCallback(service, bundleName);
        fun();
        service->session_ = session_;
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by TimeOutCallback.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_TimeOutCallback_0000";
}

/**
 * @tc.number: SUB_Service_DoTimeout_0000
 * @tc.name: SUB_Service_DoTimeout_0000
 * @tc.desc: 测试 DoTimeout 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_DoTimeout_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DoTimeout_0000";
    try {
        string bundleName;
        service->DoTimeout(nullptr, bundleName);
        EXPECT_TRUE(true);

        auto session_ = service->session_;
        service->session_ = nullptr;
        service->DoTimeout(service, bundleName);
        service->session_ = session_;
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(false));
        service->DoTimeout(service, bundleName);
        EXPECT_TRUE(true);

        shared_ptr<SABackupConnection> sa = nullptr;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        service->DoTimeout(service, bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by DoTimeout.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DoTimeout_0000";
}

/**
 * @tc.number: SUB_Service_DoTimeout_0100
 * @tc.name: SUB_Service_DoTimeout_0100
 * @tc.desc: 测试 DoTimeout 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_DoTimeout_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DoTimeout_0100";
    try {
        string bundleName;
        shared_ptr<SABackupConnection> sa = nullptr;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        service->DoTimeout(service, bundleName);
        EXPECT_TRUE(true);

        sa = nullptr;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        service->DoTimeout(service, bundleName);
        EXPECT_TRUE(true);

        sa = make_shared<SABackupConnection>(nullptr, nullptr, nullptr, nullptr);
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        EXPECT_CALL(*saConnect, DisconnectBackupSAExt()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(false));
        service->DoTimeout(service, bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by DoTimeout.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DoTimeout_0100";
}

/**
 * @tc.number: SUB_Service_AddClearBundleRecord_0000
 * @tc.name: SUB_Service_AddClearBundleRecord_0000
 * @tc.desc: 测试 AddClearBundleRecord 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_AddClearBundleRecord_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AddClearBundleRecord_0000";
    try {
        string bundleName;
        EXPECT_CALL(*cdConfig, InsertClearBundleRecord(_)).WillOnce(Return(false));
        service->AddClearBundleRecord(bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*cdConfig, InsertClearBundleRecord(_)).WillOnce(Return(true));
        service->AddClearBundleRecord(bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by AddClearBundleRecord.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AddClearBundleRecord_0000";
}

/**
 * @tc.number: SUB_Service_DelClearBundleRecord_0000
 * @tc.name: SUB_Service_DelClearBundleRecord_0000
 * @tc.desc: 测试 DelClearBundleRecord 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_DelClearBundleRecord_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DelClearBundleRecord_0000";
    try {
        vector<string> bundleNames { "bundleName" };
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(false));
        service->DelClearBundleRecord(bundleNames);
        EXPECT_TRUE(true);

        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        service->DelClearBundleRecord(bundleNames);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by DelClearBundleRecord.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DelClearBundleRecord_0000";
}

/**
 * @tc.number: SUB_Service_ReleaseOnException_0000
 * @tc.name: SUB_Service_ReleaseOnException_0000
 * @tc.desc: 测试 ReleaseOnException 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_ReleaseOnException_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ReleaseOnException_0000";
    try {
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->ReleaseOnException();
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        service->isInRelease_ = false;
        service->ReleaseOnException();
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        service->isInRelease_ = true;
        service->ReleaseOnException();
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        service->isInRelease_ = true;
        auto session_ = service->session_;
        service->session_ = nullptr;
        service->ReleaseOnException();
        service->session_ = session_;
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ReleaseOnException.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ReleaseOnException_0000";
}
}