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

#include <cstdio>
#include <gtest/gtest.h>
#include <string>

#include "b_error/b_error.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "ext_extension_mock.h"
#include "file_ex.h"
#include "module_ipc/service.h"
#include "module_ipc/svc_session_manager.h"
#include "service_reverse_mock.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace testing;
using namespace std;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string MANAGE_JSON = "manage.json";
const string FILE_NAME = "1.tar";
constexpr int32_t SERVICE_ID = 5203;
constexpr int32_t CLIENT_TOKEN_ID = 100;
} // namespace

class SvcSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override {};
    void TearDown() override {};
    void Init(IServiceReverse::Scenario scenario);

    static inline sptr<SvcSessionManager> sessionManagerPtr_ = nullptr;
    static inline sptr<ServiceReverseMock> remote_ = nullptr;
    static inline sptr<Service> servicePtr_ = nullptr;
};

void SvcSessionManagerTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    remote_ = sptr(new ServiceReverseMock());
    servicePtr_ = sptr(new Service(SERVICE_ID));
    sessionManagerPtr_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(servicePtr_)));
}

void SvcSessionManagerTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    sessionManagerPtr_ = nullptr;
    servicePtr_ = nullptr;
    remote_ = nullptr;
}

void SvcSessionManagerTest::Init(IServiceReverse::Scenario scenario)
{
    vector<string> bundleNames;
    map<string, BackupExtInfo> backupExtNameMap;
    bundleNames.emplace_back(BUNDLE_NAME);
    EXPECT_TRUE(sessionManagerPtr_ != nullptr);
    sessionManagerPtr_->Active(
        {.clientToken = CLIENT_TOKEN_ID, .scenario = scenario, .backupExtNameMap = {}, .clientProxy = remote_});
    sessionManagerPtr_->IsOnAllBundlesFinished();
    sessionManagerPtr_->AppendBundles(bundleNames);
    sessionManagerPtr_->Finish();
    sessionManagerPtr_->IsOnAllBundlesFinished();
}

/**
 * @tc.number: SUB_backup_sa_session_VerifyCallerAndScenario_0100
 * @tc.name: SUB_backup_sa_session_VerifyCallerAndScenario_0100
 * @tc.desc: 测试 VerifyCallerAndScenario 是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_VerifyCallerAndScenario_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_VerifyCallerAndScenario_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::BACKUP;
            sessionManagerPtr_->VerifyCallerAndScenario(CLIENT_TOKEN_ID, IServiceReverse::Scenario::RESTORE);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SDK_MIXED_SCENARIO);
        }

        try {
            sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::BACKUP;
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->VerifyCallerAndScenario(CLIENT_TOKEN_ID, IServiceReverse::Scenario::BACKUP);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_REFUSED_ACT);
        }

        sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::BACKUP;
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->VerifyCallerAndScenario(CLIENT_TOKEN_ID, IServiceReverse::Scenario::BACKUP);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by VerifyCallerAndScenario.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_VerifyCallerAndScenario_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_Active_0100
 * @tc.name: SUB_backup_sa_session_Active_0100
 * @tc.desc: 测试 Active
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_Active_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_Active_0100";
    try {
        SvcSessionManager::Impl newImpl;
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
            auto res = sessionManagerPtr_->Active(newImpl);
            EXPECT_EQ(res, BError(BError::Codes::SA_REFUSED_ACT).GetCode());
        } catch (BError &err) {
            EXPECT_TRUE(false);
        }

        try {
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->Active(newImpl);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            newImpl.clientToken = CLIENT_TOKEN_ID;
            newImpl.scenario = IServiceReverse::Scenario::UNDEFINED;
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->Active(newImpl);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            newImpl.clientToken = CLIENT_TOKEN_ID;
            newImpl.scenario = IServiceReverse::Scenario::BACKUP;
            newImpl.clientProxy = nullptr;
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->Active(newImpl);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by Active.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_Active_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_Deactive_0100
 * @tc.name: SUB_backup_sa_session_Deactive_0100
 * @tc.desc: 测试 Deactive
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_Deactive_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_Deactive_0100";
    try {
        wptr<IRemoteObject> remoteInAction = nullptr;
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->impl_.clientToken = 0;
        sessionManagerPtr_->Deactive(remoteInAction, false);
        EXPECT_TRUE(true);

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.clientProxy = nullptr;
        sessionManagerPtr_->Deactive(remoteInAction, false);
        EXPECT_TRUE(true);

        try {
            sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
            sessionManagerPtr_->impl_.clientProxy = remote_;
            sessionManagerPtr_->Deactive(remoteInAction, false);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.clientProxy = remote_;
        sessionManagerPtr_->Deactive(remoteInAction, true);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by Deactive.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_Deactive_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_VerifyBundleName_0100
 * @tc.name: SUB_backup_sa_session_VerifyBundleName_0100
 * @tc.desc: 测试 VerifyBundleName 检验调用者给定的bundleName是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_VerifyBundleName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_VerifyBundleName_0100";
    try {
        string bundleName = BUNDLE_NAME;
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->VerifyBundleName(bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
            sessionManagerPtr_->impl_.backupExtNameMap.clear();
            sessionManagerPtr_->VerifyBundleName(bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_REFUSED_ACT);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->VerifyBundleName(bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by VerifyBundleName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_VerifyBundleName_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetServiceReverseProxy_0100
 * @tc.name: SUB_backup_sa_session_GetServiceReverseProxy_0100
 * @tc.desc: 测试 GetServiceReverseProxy
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetServiceReverseProxy_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetServiceReverseProxy_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientProxy = nullptr;
            sessionManagerPtr_->GetServiceReverseProxy();
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_REFUSED_ACT);
        }

        sessionManagerPtr_->impl_.clientProxy = remote_;
        auto proxy = sessionManagerPtr_->GetServiceReverseProxy();
        EXPECT_EQ(reinterpret_cast<long long>(proxy.GetRefPtr()),
            reinterpret_cast<long long>(sessionManagerPtr_->impl_.clientProxy.GetRefPtr()));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetServiceReverseProxy.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetServiceReverseProxy_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_getscenario_0100
 * @tc.name: SUB_backup_sa_session_getscenario_0100
 * @tc.desc: 测试 GetScenario 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_getscenario_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_getscenario_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetScenario();
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->GetScenario();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by getscenario.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_getscenario_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_onBundlefileready_0100
 * @tc.name: SUB_backup_sa_session_onBundlefileready_0100
 * @tc.desc: 测试 OnBundleFileReady 接口 restore流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onBundlefileready_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onBundlefileready_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
            sessionManagerPtr_->impl_.backupExtNameMap.clear();
            sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_REFUSED_ACT);
        }

        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::RESTORE;
        auto ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME);
        EXPECT_TRUE(ret);

        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap["123"] = {};
        sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::BACKUP;
        ret = sessionManagerPtr_->OnBundleFileReady("123");
        EXPECT_TRUE(ret);

        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::UNDEFINED;
        ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by onBundlefileready.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_onBundlefileready_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_onBundlefileready_0101
 * @tc.name: SUB_backup_sa_session_onBundlefileready_0101
 * @tc.desc: 测试 OnBundleFileReady 接口 restore流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onBundlefileready_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onBundlefileready_0101";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::BACKUP;
        auto ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME, "");
        EXPECT_FALSE(ret);

        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::BACKUP;
        ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME, "test");
        EXPECT_FALSE(ret);

        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::BACKUP;
        ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME, string(BConstants::EXT_BACKUP_MANAGE));
        EXPECT_FALSE(ret);

        BackupExtInfo info;
        info.receExtManageJson = true;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME, "");
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by onBundlefileready.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_onBundlefileready_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_OnBundleExtManageInfo_0100
 * @tc.name: SUB_backup_sa_session_OnBundleExtManageInfo_0100
 * @tc.desc: 测试 OnBundleExtManageInfo
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_OnBundleExtManageInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_OnBundleExtManageInfo_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->OnBundleExtManageInfo(BUNDLE_NAME, UniqueFd(-1));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
            sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::RESTORE;
            sessionManagerPtr_->OnBundleExtManageInfo(BUNDLE_NAME, UniqueFd(-1));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        TestManager tm("SvcSessionManagerTest_GetFd_0100");
        string filePath = tm.GetRootDirCurTest().append(MANAGE_JSON);
        SaveStringToFile(filePath, R"({"fileName" : "1.tar"})");
        UniqueFd fd(open(filePath.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR));
        sessionManagerPtr_->OnBundleExtManageInfo(BUNDLE_NAME, move(fd));
        auto ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME, MANAGE_JSON);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by OnBundleExtManageInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_OnBundleExtManageInfo_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBackupAbilityExt_0100
 * @tc.name: SUB_backup_sa_session_GetBackupAbilityExt_0100
 * @tc.desc: 测试 GetBackupAbilityExt 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBackupAbilityExt_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBackupAbilityExt_0100";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->reversePtr_ = nullptr;
        auto ret = sessionManagerPtr_->GetBackupAbilityExt(BUNDLE_NAME);
        ret->callDied_("", false);
        ret->callConnected_("");
        EXPECT_TRUE(true);

        sessionManagerPtr_->reversePtr_ = servicePtr_;
        ret = sessionManagerPtr_->GetBackupAbilityExt(BUNDLE_NAME);
        ret->callDied_("", false);
        ret->callConnected_("");
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBackupAbilityExt.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBackupAbilityExt_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBackupSAExt_0100
 * @tc.name: SUB_backup_sa_session_GetBackupSAExt_0100
 * @tc.desc: 测试 GetBackupSAExt 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBackupSAExt_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBackupSAExt_0100";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->reversePtr_ = nullptr;
        auto ret = sessionManagerPtr_->GetBackupSAExt(BUNDLE_NAME);
        ret->callDied_("");
        ret->callConnected_("");
        ret->callBackup_("", 0, "", BError(BError::Codes::OK));
        ret->callRestore_("", "", BError(BError::Codes::OK));
        EXPECT_TRUE(true);

        sessionManagerPtr_->reversePtr_ = servicePtr_;
        ret = sessionManagerPtr_->GetBackupSAExt(BUNDLE_NAME);
        ret->callDied_("");
        ret->callConnected_("");
        ret->callBackup_("", 0, "", BError(BError::Codes::OK));
        ret->callRestore_("", "", BError(BError::Codes::OK));
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBackupSAExt.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBackupSAExt_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_InitClient_0100
 * @tc.name: SUB_backup_sa_session_InitClient_0100
 * @tc.desc: 测试 InitClient 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_InitClient_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_InitClient_0100";
    try {
        SvcSessionManager::Impl newImpl;
        try {
            newImpl.clientProxy = nullptr;
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->InitClient(newImpl);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        newImpl.clientProxy = remote_;
        sessionManagerPtr_->InitClient(newImpl);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by InitClient.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_InitClient_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetExtFileNameRequest_0100
 * @tc.name: SUB_backup_sa_session_GetExtFileNameRequest_0100
 * @tc.desc: 测试 GetExtFileNameRequest 获取暂存真实文件请求
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtFileNameRequest_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtFileNameRequest_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->SetExtFileNameRequest(BUNDLE_NAME, FILE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->SetExtFileNameRequest(BUNDLE_NAME, FILE_NAME);
        EXPECT_EQ(sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME].fileNameInfo.size(), 1);
        EXPECT_EQ(*(sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME].fileNameInfo.begin()), FILE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetExtFileNameRequest.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetExtFileNameRequest_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetExtFileNameRequest_0101
 * @tc.name: SUB_backup_sa_session_GetExtFileNameRequest_0101
 * @tc.desc: 测试 GetExtFileNameRequest 获取暂存真实文件请求
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtFileNameRequest_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtFileNameRequest_0101";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetExtFileNameRequest(BUNDLE_NAME);
            EXPECT_TRUE(true);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetExtFileNameRequest.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetExtFileNameRequest_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_GetExtFileNameRequest_0102
 * @tc.name: SUB_backup_sa_session_GetExtFileNameRequest_0102
 * @tc.desc: 测试 GetExtFileNameRequest 获取暂存真实文件请求
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtFileNameRequest_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtFileNameRequest_0102";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
            sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::UNDEFINED;
            sessionManagerPtr_->GetExtFileNameRequest(BUNDLE_NAME);
            EXPECT_TRUE(true);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetExtFileNameRequest.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetExtFileNameRequest_0102";
}

/**
 * @tc.number: SUB_backup_sa_session_GetExtFileNameRequest_0103
 * @tc.name: SUB_backup_sa_session_GetExtFileNameRequest_0103
 * @tc.desc: 测试 GetExtFileNameRequest 获取暂存真实文件请求
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtFileNameRequest_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtFileNameRequest_0103";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
            sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::RESTORE;
            sessionManagerPtr_->GetExtFileNameRequest(BUNDLE_NAME);
            EXPECT_TRUE(true);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetExtFileNameRequest.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetExtFileNameRequest_0103";
}

/**
 * @tc.number: SUB_backup_sa_session_GetExtConnection_0100
 * @tc.name: SUB_backup_sa_session_GetExtConnection_0100
 * @tc.desc: 测试 GetExtConnection 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtConnection_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtConnection_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetExtConnection(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
            sessionManagerPtr_->impl_.backupExtNameMap.clear();
            sessionManagerPtr_->GetExtConnection(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_REFUSED_ACT);
        }

        try {
            sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
            sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
            sessionManagerPtr_->GetExtConnection(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        BackupExtInfo info;
        info.backUpConnection = sptr(new SvcBackupConnection(nullptr, nullptr, BUNDLE_NAME));
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        auto ret = sessionManagerPtr_->GetExtConnection(BUNDLE_NAME);
        EXPECT_EQ(reinterpret_cast<long long>(ret.GetRefPtr()),
            reinterpret_cast<long long>(info.backUpConnection.GetRefPtr()));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetExtConnection.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetExtConnection_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetSAExtConnection_0100
 * @tc.name: SUB_backup_sa_session_GetSAExtConnection_0100
 * @tc.desc: 测试 GetSAExtConnection 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetSAExtConnection_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetSAExtConnection_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetSAExtConnection(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
            sessionManagerPtr_->impl_.backupExtNameMap.clear();
            sessionManagerPtr_->GetSAExtConnection(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_REFUSED_ACT);
        }

        try {
            sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
            sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
            sessionManagerPtr_->GetSAExtConnection(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        BackupExtInfo info;
        info.saBackupConnection = make_shared<SABackupConnection>(nullptr, nullptr, nullptr, nullptr);
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        auto ret = sessionManagerPtr_->GetSAExtConnection(BUNDLE_NAME).lock();
        EXPECT_EQ(reinterpret_cast<long long>(ret.get()), reinterpret_cast<long long>(info.saBackupConnection.get()));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetSAExtConnection.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetSAExtConnection_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_DumpInfo_0100
 * @tc.name: SUB_backup_sa_session_DumpInfo_0100
 * @tc.desc: 测试 DumpInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_DumpInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_DumpInfo_0100";
    try {
        TestManager tm("SvcSessionManagerTest_GetFd_0100");
        string fileUri = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(fileUri.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->DumpInfo(fd, {});
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by DumpInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_DumpInfo_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetSchedBundleName_0100
 * @tc.name: SUB_backup_sa_session_GetSchedBundleName_0100
 * @tc.desc: 测试 GetSchedBundleName 调度器获取所需要的调度信息
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetSchedBundleName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetSchedBundleName_0100";
    try {
        string bundleName;
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->SetIsReadyLaunch(BUNDLE_NAME);
        bool condition = sessionManagerPtr_->GetSchedBundleName(bundleName);
        EXPECT_EQ(bundleName, BUNDLE_NAME);
        EXPECT_FALSE(condition);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-GetSchedBundleName Branches";
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        condition = sessionManagerPtr_->GetSchedBundleName(bundleName);
        EXPECT_FALSE(condition);
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        condition = sessionManagerPtr_->GetSchedBundleName(bundleName);
        EXPECT_FALSE(condition);
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::WAIT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetSchedBundleName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetSchedBundleName_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetServiceSchedAction_0100
 * @tc.name: SUB_backup_sa_session_GetServiceSchedAction_0100
 * @tc.desc: 测试 GetServiceSchedAction 获取状态
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetServiceSchedAction_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetServiceSchedAction_0100";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        auto action = sessionManagerPtr_->GetServiceSchedAction(BUNDLE_NAME);
        EXPECT_EQ(action, BConstants::ServiceSchedAction::WAIT);

        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        action = sessionManagerPtr_->GetServiceSchedAction(BUNDLE_NAME);
        EXPECT_EQ(action, BConstants::ServiceSchedAction::START);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-SetServiceSchedAction Branches";
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::FINISH);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetServiceSchedAction.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetServiceSchedAction_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetServiceSchedAction_0102
 * @tc.name: SUB_backup_sa_session_GetServiceSchedAction_0102
 * @tc.desc: 测试 GetServiceSchedAction 获取状态
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetServiceSchedAction_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetServiceSchedAction_0102";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        auto action = sessionManagerPtr_->GetServiceSchedAction(BUNDLE_NAME);
        EXPECT_NE(action, BConstants::ServiceSchedAction::WAIT);

        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        action = sessionManagerPtr_->GetServiceSchedAction(BUNDLE_NAME);
        EXPECT_EQ(action, BConstants::ServiceSchedAction::START);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-SetServiceSchedAction Branches";
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::FINISH);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetServiceSchedAction.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetServiceSchedAction_0102";
}

/**
 * @tc.number: SUB_backup_sa_session_GetServiceSchedAction_0103
 * @tc.name: SUB_backup_sa_session_GetServiceSchedAction_0103
 * @tc.desc: 测试 GetServiceSchedAction 获取状态
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetServiceSchedAction_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetServiceSchedAction_0103";
    try {
        string bundleName = "";
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        auto action = sessionManagerPtr_->GetServiceSchedAction(bundleName);
        EXPECT_EQ(action, BConstants::ServiceSchedAction::WAIT);

        sessionManagerPtr_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::START);
        action = sessionManagerPtr_->GetServiceSchedAction(bundleName);
        EXPECT_NE(action, BConstants::ServiceSchedAction::START);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-SetServiceSchedAction Branches";
        sessionManagerPtr_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::FINISH);

        try {
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetServiceSchedAction(bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::START);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetServiceSchedAction.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetServiceSchedAction_0103";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBackupExtName_0100
 * @tc.name: SUB_backup_sa_session_GetBackupExtName_0100
 * @tc.desc: 测试 GetBackupExtName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBackupExtName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBackupExtName_0100";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->SetBackupExtName(BUNDLE_NAME, BUNDLE_NAME);
        string extName = sessionManagerPtr_->GetBackupExtName(BUNDLE_NAME);
        EXPECT_EQ(extName, BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBackupExtName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBackupExtName_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBackupExtName_0101
 * @tc.name: SUB_backup_sa_session_GetBackupExtName_0101
 * @tc.desc: 测试 GetBackupExtName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBackupExtName_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBackupExtName_0101";
    try {
        string bundleName = "";
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->SetBackupExtName(bundleName, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        try {
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetBackupExtName(bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->SetBackupExtName(bundleName, bundleName);
        string extName = sessionManagerPtr_->GetBackupExtName(bundleName);
        EXPECT_NE(extName, bundleName);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBackupExtName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBackupExtName_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_SetBackupExtInfo_0100
 * @tc.name: SUB_backup_sa_session_SetBackupExtInfo_0100
 * @tc.desc: 测试 SetBackupExtInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetBackupExtInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetBackupExtInfo_0100";
    try {
        string bundleName = BUNDLE_NAME;
        string extInfo = "test";
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->SetBackupExtInfo(bundleName, extInfo);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[bundleName] = {};
        sessionManagerPtr_->SetBackupExtInfo(bundleName, extInfo);
        EXPECT_EQ(sessionManagerPtr_->impl_.backupExtNameMap.size(), 1);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetBackupExtInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetBackupExtInfo_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBackupExtInfo_0100
 * @tc.name: SUB_backup_sa_session_GetBackupExtInfo_0100
 * @tc.desc: 测试 GetBackupExtInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBackupExtInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBackupExtInfo_0100";
    try {
        string bundleName = BUNDLE_NAME;
        string extInfo = "test";
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetBackupExtInfo(bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        BackupExtInfo info;
        info.extInfo = "test";
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[bundleName] = info;
        auto ret = sessionManagerPtr_->GetBackupExtInfo(bundleName);
        EXPECT_EQ(ret, "test");
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBackupExtInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBackupExtInfo_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_AppendBundles_0100
 * @tc.name: SUB_backup_sa_session_AppendBundles_0100
 * @tc.desc: 测试 AppendBundles 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_AppendBundles_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_AppendBundles_0100";
    try {
        vector<BundleName> bundleNames;
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->AppendBundles(bundleNames);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        bundleNames.clear();
        bundleNames.emplace_back("app1");
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->AppendBundles(bundleNames);
        EXPECT_EQ(sessionManagerPtr_->impl_.backupExtNameMap.size(), 1);

        bundleNames.clear();
        bundleNames.emplace_back("123");
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->AppendBundles(bundleNames);
        EXPECT_EQ(sessionManagerPtr_->impl_.backupExtNameMap.size(), 1);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by AppendBundles.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_AppendBundles_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_NeedToUnloadService_0100
 * @tc.name: SUB_backup_sa_session_NeedToUnloadService_0100
 * @tc.desc: 测试 NeedToUnloadService 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_NeedToUnloadService_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_NeedToUnloadService_0100";
    try {

        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->impl_.restoreDataType = RestoreTypeEnum::RESTORE_DATA_READDY;
        auto ret = sessionManagerPtr_->NeedToUnloadService();
        EXPECT_FALSE(ret);

        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->impl_.restoreDataType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND;
        sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::RESTORE;
        ret = sessionManagerPtr_->NeedToUnloadService();
        EXPECT_FALSE(ret);

        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->impl_.restoreDataType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND;
        sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::BACKUP;
        ret = sessionManagerPtr_->NeedToUnloadService();
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by NeedToUnloadService.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_NeedToUnloadService_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_removeextinfo_0100
 * @tc.name: SUB_backup_sa_session_removeextinfo_0100
 * @tc.desc: 测试 RemoveExtInfo 移除bundleName是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_removeextinfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_removeextinfo_0100";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->RemoveExtInfo(BUNDLE_NAME);
        EXPECT_EQ(sessionManagerPtr_->impl_.backupExtNameMap.size(), 0);

        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->RemoveExtInfo(BUNDLE_NAME);
        EXPECT_EQ(sessionManagerPtr_->impl_.backupExtNameMap.size(), 0);

        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->extConnectNum_ = 0;
        sessionManagerPtr_->RemoveExtInfo(BUNDLE_NAME);
        EXPECT_EQ(sessionManagerPtr_->extConnectNum_, 0);

        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->extConnectNum_ = 1;
        sessionManagerPtr_->RemoveExtInfo(BUNDLE_NAME);
        EXPECT_EQ(sessionManagerPtr_->extConnectNum_, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by RemoveExtInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_removeextinfo_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_OnBundleFileReady_0200
 * @tc.name: SUB_backup_sa_session_OnBundleFileReady_0200
 * @tc.desc: 测试 OnBundleFileReady
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_OnBundleFileReady_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_OnBundleFileReady_0200";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->Deactive(nullptr, true);
        Init(IServiceReverse::Scenario::BACKUP);
        auto ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME, MANAGE_JSON);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBundleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by OnBundleFileReady.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_OnBundleFileReady_0200";
}

/**
 * @tc.number: SUB_backup_sa_session_SetSessionUserId_0100
 * @tc.name: SUB_backup_sa_session_SetSessionUserId_0100
 * @tc.desc: 测试 SetSessionUserId
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetSessionUserId_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_backup_sa_session_SetSessionUserId_0100";
    try {
        int32_t userId = 1;
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->SetSessionUserId(userId);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SetSessionUserId.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_backup_sa_session_SetSessionUserId_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetSessionUserId_0100
 * @tc.name: SUB_backup_sa_session_GetSessionUserId_0100
 * @tc.desc: 测试 GetSessionUserId
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetSessionUserId_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_backup_sa_session_GetSessionUserId_0100";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->GetSessionUserId();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetSessionUserId.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_backup_sa_session_GetSessionUserId_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_SetBundleRestoreType_0100
 * @tc.name: SUB_backup_sa_session_SetBundleRestoreType_0100
 * @tc.desc: 测试 SetBundleRestoreType 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetBundleRestoreType_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetBundleRestoreType_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->SetBundleRestoreType(BUNDLE_NAME, RESTORE_DATA_READDY);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->SetBundleRestoreType(BUNDLE_NAME, RESTORE_DATA_READDY);
        EXPECT_EQ(sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME].restoreType, RESTORE_DATA_READDY);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetBundleRestoreType.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetBundleRestoreType_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBundleRestoreType_0100
 * @tc.name: SUB_backup_sa_session_GetBundleRestoreType_0100
 * @tc.desc: 测试 GetBundleRestoreType 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBundleRestoreType_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBundleRestoreType_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetBundleRestoreType(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->GetBundleRestoreType(BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBundleRestoreType.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBundleRestoreType_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_SetBundleVersionCode_0100
 * @tc.name: SUB_backup_sa_session_SetBundleVersionCode_0100
 * @tc.desc: 测试 SetBundleVersionCode 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetBundleVersionCode_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetBundleVersionCode_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->SetBundleVersionCode(BUNDLE_NAME, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->SetBundleVersionCode(BUNDLE_NAME, 0);
        EXPECT_EQ(sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME].versionCode, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetBundleVersionCode.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetBundleVersionCode_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBundleVersionCode_0100
 * @tc.name: SUB_backup_sa_session_GetBundleVersionCode_0100
 * @tc.desc: 测试 GetBundleVersionCode 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBundleVersionCode_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBundleVersionCode_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetBundleVersionCode(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->GetBundleVersionCode(BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBundleVersionCode.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBundleVersionCode_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_SetBundleVersionName_0100
 * @tc.name: SUB_backup_sa_session_SetBundleVersionName_0100
 * @tc.desc: 测试 SetBundleVersionName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetBundleVersionName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetBundleVersionName_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->SetBundleVersionName(BUNDLE_NAME, "1.0.0");
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->SetBundleVersionName(BUNDLE_NAME, "1.0.0");
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetBundleVersionName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetBundleVersionName_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBundleVersionName_0100
 * @tc.name: SUB_backup_sa_session_GetBundleVersionName_0100
 * @tc.desc: 测试 GetBundleVersionName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBundleVersionName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBundleVersionName_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetBundleVersionName(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->GetBundleVersionName(BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBundleVersionName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBundleVersionName_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_Start_0100
 * @tc.name: SUB_backup_sa_session_Start_0100
 * @tc.desc: 测试 Start
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_Start_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_Start_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->Start();
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->Start();
        EXPECT_TRUE(sessionManagerPtr_->impl_.isBackupStart);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by Start.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_Start_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_Finish_0100
 * @tc.name: SUB_backup_sa_session_Finish_0100
 * @tc.desc: 测试 Finish
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_Finish_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_Finish_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->Finish();
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->Finish();
        EXPECT_TRUE(sessionManagerPtr_->impl_.isAppendFinish);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by Finish.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_Finish_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_IsOnAllBundlesFinished_0100
 * @tc.name: SUB_backup_sa_session_IsOnAllBundlesFinished_0100
 * @tc.desc: 测试 IsOnAllBundlesFinished
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_IsOnAllBundlesFinished_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_IsOnAllBundlesFinished_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->IsOnAllBundlesFinished();
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::RESTORE;
        sessionManagerPtr_->IsOnAllBundlesFinished();
        EXPECT_TRUE(true);

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.scenario = IServiceReverse::Scenario::UNDEFINED;
        sessionManagerPtr_->IsOnAllBundlesFinished();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by IsOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_IsOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_IsOnOnStartSched_0100
 * @tc.name: SUB_backup_sa_session_IsOnOnStartSched_0100
 * @tc.desc: 测试 IsOnOnStartSched
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_IsOnOnStartSched_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_IsOnOnStartSched_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->IsOnOnStartSched();
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.isBackupStart = false;
        auto ret = sessionManagerPtr_->IsOnOnStartSched();

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.isBackupStart = true;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        ret = sessionManagerPtr_->IsOnOnStartSched();
        EXPECT_FALSE(ret);

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.isBackupStart = true;
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        ret = sessionManagerPtr_->IsOnOnStartSched();
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by IsOnOnStartSched.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_IsOnOnStartSched_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_SetBundleDataSize_0100
 * @tc.name: SUB_backup_sa_session_SetBundleDataSize_0100
 * @tc.desc: 测试 SetBundleDataSize
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetBundleDataSize_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetBundleDataSize_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->SetBundleDataSize(BUNDLE_NAME, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->SetBundleDataSize(BUNDLE_NAME, 0);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetBundleDataSize.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetBundleDataSize_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_StartFwkTimer_0100
 * @tc.name: SUB_backup_sa_session_StartFwkTimer_0100
 * @tc.desc: 测试 StartFwkTimer
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_StartFwkTimer_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_StartFwkTimer_0100";
    try {
        auto callback = []() -> void {};
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->impl_.clientToken = 0;
        auto ret = sessionManagerPtr_->StartFwkTimer(BUNDLE_NAME, callback);
        EXPECT_FALSE(ret);

        BackupExtInfo info;
        info.fwkTimerStatus = false;
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        ret = sessionManagerPtr_->StartFwkTimer(BUNDLE_NAME, callback);
        EXPECT_TRUE(ret);
        ret = sessionManagerPtr_->StopFwkTimer(BUNDLE_NAME);
        EXPECT_TRUE(ret);

        info.fwkTimerStatus = true;
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        ret = sessionManagerPtr_->StartFwkTimer(BUNDLE_NAME, callback);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->StopFwkTimer(BUNDLE_NAME);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by StartFwkTimer.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_StartFwkTimer_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_UpdateTimer_0100
 * @tc.name: SUB_backup_sa_session_UpdateTimer_0100
 * @tc.desc: 测试 UpdateTimer
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_UpdateTimer_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_UpdateTimer_0100";
    try {
        auto callback = []() -> void {};
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->impl_.clientToken = 0;
        auto ret = sessionManagerPtr_->UpdateTimer(BUNDLE_NAME, BConstants::DEFAULT_TIMEOUT, callback);
        EXPECT_FALSE(ret);

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        EXPECT_THROW(sessionManagerPtr_->UpdateTimer(BUNDLE_NAME, BConstants::TIMEOUT_INVALID, callback), BError);

        BackupExtInfo info;
        info.extTimerStatus = false;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        ret = sessionManagerPtr_->UpdateTimer(BUNDLE_NAME, BConstants::DEFAULT_TIMEOUT, callback);
        EXPECT_TRUE(ret);

        info.extTimerStatus = true;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        ret = sessionManagerPtr_->UpdateTimer(BUNDLE_NAME, BConstants::DEFAULT_TIMEOUT, callback);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by UpdateTimer.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_UpdateTimer_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_StopFwkTimer_0100
 * @tc.name: SUB_backup_sa_session_StopFwkTimer_0100
 * @tc.desc: 测试 StopFwkTimer
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_StopFwkTimer_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_StopFwkTimer_0100";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->impl_.clientToken = 0;
        auto ret = sessionManagerPtr_->StopFwkTimer(BUNDLE_NAME);
        EXPECT_FALSE(ret);

        BackupExtInfo info;
        info.fwkTimerStatus = false;
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        ret = sessionManagerPtr_->StopFwkTimer(BUNDLE_NAME);
        EXPECT_TRUE(ret);

        info.fwkTimerStatus = true;
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        ret = sessionManagerPtr_->StopFwkTimer(BUNDLE_NAME);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by StopFwkTimer.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_StopFwkTimer_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_ClearSessionData_0100
 * @tc.name: SUB_backup_sa_session_ClearSessionData_0100
 * @tc.desc: 测试 ClearSessionData
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_ClearSessionData_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_ClearSessionData_0100";
    try {
        BackupExtInfo info;
        info.fwkTimerStatus = true;
        info.schedAction = BConstants::ServiceSchedAction::RUNNING;
        info.backUpConnection = sptr(new SvcBackupConnection(nullptr, nullptr, BUNDLE_NAME));
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        sessionManagerPtr_->ClearSessionData();
        EXPECT_TRUE(true);

        info.fwkTimerStatus = false;
        info.schedAction = BConstants::ServiceSchedAction::WAIT;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        sessionManagerPtr_->ClearSessionData();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by ClearSessionData.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_ClearSessionData_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetIsIncrementalBackup_0100
 * @tc.name: SUB_backup_sa_session_GetIsIncrementalBackup_0100
 * @tc.desc: 测试 GetIsIncrementalBackup
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetIsIncrementalBackup_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetIsIncrementalBackup_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetIsIncrementalBackup();
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->GetIsIncrementalBackup();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetIsIncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetIsIncrementalBackup_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_SetIncrementalData_0100
 * @tc.name: SUB_backup_sa_session_SetIncrementalData_0100
 * @tc.desc: 测试 SetIncrementalData
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetIncrementalData_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetIncrementalData_0100";
    try {
        BIncrementalData incrementalData;
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->SetIncrementalData(incrementalData);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        incrementalData.bundleName = BUNDLE_NAME;
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->SetIncrementalData(incrementalData);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetIncrementalData.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetIncrementalData_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetIncrementalManifestFd_0100
 * @tc.name: SUB_backup_sa_session_GetIncrementalManifestFd_0100
 * @tc.desc: 测试 GetIncrementalManifestFd
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetIncrementalManifestFd_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetIncrementalManifestFd_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetIncrementalManifestFd(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->GetIncrementalManifestFd(BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetIncrementalManifestFd.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetIncrementalManifestFd_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_StartExtTimer_0100
 * @tc.name: SUB_backup_sa_session_StartExtTimer_0100
 * @tc.desc: 测试 StartExtTimer 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_StartExtTimer_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_StartExtTimer_0100";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        auto callback = []() -> void {};
        sessionManagerPtr_->impl_.clientToken = 0;
        bool ret = sessionManagerPtr_->StartExtTimer(BUNDLE_NAME, callback);
        EXPECT_FALSE(ret);

        BackupExtInfo info;
        info.extTimerStatus = false;
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        ret = sessionManagerPtr_->StartExtTimer(BUNDLE_NAME, callback);
        EXPECT_TRUE(ret);
        ret = sessionManagerPtr_->StopExtTimer(BUNDLE_NAME);
        EXPECT_TRUE(ret);

        info.extTimerStatus = true;
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        ret = sessionManagerPtr_->StartExtTimer(BUNDLE_NAME, callback);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->StopExtTimer(BUNDLE_NAME);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by StartExtTimer.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_StartExtTimer_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_StopExtTimer_0100
 * @tc.name: SUB_backup_sa_session_StopExtTimer_0100
 * @tc.desc: 测试 StopExtTimer 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_StopExtTimer_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_StopExtTimer_0100";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->impl_.clientToken = 0;
        bool ret = sessionManagerPtr_->StopExtTimer(BUNDLE_NAME);
        EXPECT_FALSE(ret);

        BackupExtInfo info;
        info.extTimerStatus = false;
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        ret = sessionManagerPtr_->StopExtTimer(BUNDLE_NAME);
        EXPECT_TRUE(ret);

        info.extTimerStatus = true;
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = info;
        ret = sessionManagerPtr_->StopExtTimer(BUNDLE_NAME);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by StopExtTimer.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_StopExtTimer_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_SetClearDataFlag_0100
 * @tc.name: SUB_backup_sa_session_SetClearDataFlag_0100
 * @tc.desc: 测试 SetClearDataFlag
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetClearDataFlag_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetClearDataFlag_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->SetClearDataFlag(BUNDLE_NAME, false);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->SetClearDataFlag(BUNDLE_NAME, false);
        EXPECT_EQ(sessionManagerPtr_->GetClearDataFlag(BUNDLE_NAME), false);

        sessionManagerPtr_->SetClearDataFlag(BUNDLE_NAME, true);
        EXPECT_EQ(sessionManagerPtr_->GetClearDataFlag(BUNDLE_NAME), true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetClearDataFlag.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetClearDataFlag_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetClearDataFlag_0100
 * @tc.name: SUB_backup_sa_session_GetClearDataFlag_0100
 * @tc.desc: 测试 GetClearDataFlag
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetClearDataFlag_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetClearDataFlag_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetClearDataFlag(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->SetClearDataFlag(BUNDLE_NAME, false);
        EXPECT_EQ(sessionManagerPtr_->GetClearDataFlag(BUNDLE_NAME), false);

        sessionManagerPtr_->SetClearDataFlag(BUNDLE_NAME, true);
        EXPECT_EQ(sessionManagerPtr_->GetClearDataFlag(BUNDLE_NAME), true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetClearDataFlag.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetClearDataFlag_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_SetPublishFlag_0100
 * @tc.name: SUB_backup_sa_session_SetPublishFlag_0100
 * @tc.desc: 测试 SetPublishFlag
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetPublishFlag_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetPublishFlag_0100";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->SetPublishFlag(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->SetPublishFlag(BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetPublishFlag.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetPublishFlag_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_SetImplRestoreType_0100
 * @tc.name: SUB_backup_sa_session_SetImplRestoreType_0100
 * @tc.desc: 测试 SetImplRestoreType
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetImplRestoreType_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetImplRestoreType_0100";
    try {
        sessionManagerPtr_->SetImplRestoreType(RESTORE_DATA_WAIT_SEND);
        bool isValid = sessionManagerPtr_->ValidRestoreDataType(RESTORE_DATA_READDY);
        EXPECT_EQ(isValid, false);
        isValid = sessionManagerPtr_->ValidRestoreDataType(RESTORE_DATA_WAIT_SEND);
        EXPECT_EQ(isValid, true);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetImplRestoreType.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetImplRestoreType_0100";
}
#include "svc_session_manager_ex_test.cpp"
} // namespace OHOS::FileManagement::Backup