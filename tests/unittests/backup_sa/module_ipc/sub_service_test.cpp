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

/**
 * @tc.number: SUB_Service_HandleCurGroupBackupInfos_0000
 * @tc.name: SUB_Service_HandleCurGroupBackupInfos_0000
 * @tc.desc: 测试 HandleCurGroupBackupInfos 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_HandleCurGroupBackupInfos_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_HandleCurGroupBackupInfos_0000";
    try {
        vector<BJsonEntityCaps::BundleInfo> backupInfos = {
            {.name = "bundleName", .appIndex = 0, .allToBackup = false, .versionName = ""} };
        map<string, vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap;
        map<string, bool> isClearDataFlags;
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, BackupOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_CALL(*jsonUtil, FindBundleInfoByName(_, _, _, _)).WillOnce(Return(false));
        service->HandleCurGroupBackupInfos(backupInfos, bundleNameDetailMap, isClearDataFlags);
        EXPECT_TRUE(true);

        backupInfos[0].allToBackup = true;
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*jsonUtil, FindBundleInfoByName(_, _, _, _)).WillOnce(Return(false));
        service->HandleCurGroupBackupInfos(backupInfos, bundleNameDetailMap, isClearDataFlags);
        EXPECT_TRUE(true);

        backupInfos[0].allToBackup = true;
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*jsonUtil, FindBundleInfoByName(_, _, _, _)).WillOnce(Return(true));
        service->HandleCurGroupBackupInfos(backupInfos, bundleNameDetailMap, isClearDataFlags);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by HandleCurGroupBackupInfos.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_HandleCurGroupBackupInfos_0000";
}

/**
 * @tc.number: SUB_Service_ServiceResultReport_0000
 * @tc.name: SUB_Service_ServiceResultReport_0000
 * @tc.desc: 测试 ServiceResultReport 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_ServiceResultReport_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ServiceResultReport_0000";
    try {
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnResultReport(_, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        auto ret = service->ServiceResultReport("", BackupRestoreScenario::FULL_RESTORE, 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnResultReport(_, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        ret = service->ServiceResultReport("", BackupRestoreScenario::INCREMENTAL_RESTORE, 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, BackupOnResultReport(_, _)).WillOnce(Return());
        ret = service->ServiceResultReport("", BackupRestoreScenario::FULL_BACKUP, 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnResultReport(_, _)).WillOnce(Return());
        ret = service->ServiceResultReport("", BackupRestoreScenario::INCREMENTAL_BACKUP, 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ServiceResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ServiceResultReport_0000";
}

/**
 * @tc.number: SUB_Service_SAResultReport_0000
 * @tc.name: SUB_Service_SAResultReport_0000
 * @tc.desc: 测试 SAResultReport 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_SAResultReport_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SAResultReport_0000";
    try {
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnResultReport(_, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        auto ret = service->SAResultReport("", "", 0, BackupRestoreScenario::FULL_RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnResultReport(_, _, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        ret = service->SAResultReport("", "", 0, BackupRestoreScenario::INCREMENTAL_RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy)).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, BackupOnResultReport(_, _)).WillOnce(Return());
        EXPECT_CALL(*srProxy, BackupOnBundleFinished(_, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        ret = service->SAResultReport("", "", 0, BackupRestoreScenario::FULL_BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnResultReport(_, _)).WillOnce(Return());
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        ret = service->SAResultReport("", "", 0, BackupRestoreScenario::INCREMENTAL_BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SAResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SAResultReport_0000";
}

/**
 * @tc.number: SUB_Service_LaunchBackupSAExtension_0000
 * @tc.name: SUB_Service_LaunchBackupSAExtension_0000
 * @tc.desc: 测试 LaunchBackupSAExtension 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_LaunchBackupSAExtension_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_LaunchBackupSAExtension_0000";
    try {
        BundleName bundleName;
        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        auto ret = service->LaunchBackupSAExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        shared_ptr<SABackupConnection> sa = nullptr;
        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        ret = service->LaunchBackupSAExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        sa = make_shared<SABackupConnection>(nullptr, nullptr, nullptr, nullptr);
        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        ret = service->LaunchBackupSAExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        EXPECT_CALL(*saConnect, ConnectBackupSAExt(_, _, _)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->LaunchBackupSAExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetBackupExtInfo(_)).WillOnce(Return(""));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        EXPECT_CALL(*saConnect, ConnectBackupSAExt(_, _, _)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->LaunchBackupSAExtension(bundleName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by LaunchBackupSAExtension.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_LaunchBackupSAExtension_0000";
}

/**
 * @tc.number: SUB_Service_GetFileHandle_0000
 * @tc.name: SUB_Service_GetFileHandle_0000
 * @tc.desc: 测试 GetFileHandle 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_GetFileHandle_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetFileHandle_0000";
    try {
        string bundleName;
        string fileName;
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*depManager, UpdateToRestoreBundleMap(_, _)).WillOnce(Return(true));
        auto ret = service->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*depManager, UpdateToRestoreBundleMap(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::WAIT));
        ret = service->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*depManager, UpdateToRestoreBundleMap(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        ret = service->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*depManager, UpdateToRestoreBundleMap(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        ret = service->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetFileHandle_0000";
}

/**
 * @tc.number: SUB_Service_GetFileHandle_0100
 * @tc.name: SUB_Service_GetFileHandle_0100
 * @tc.desc: 测试 GetFileHandle 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_GetFileHandle_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetFileHandle_0100";
    try {
        string bundleName;
        string fileName;
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*depManager, UpdateToRestoreBundleMap(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetFileHandle(_, _)).WillOnce(Return(UniqueFd(-1)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnFileReady(_, _, _, _)).WillOnce(Return());
        auto ret = service->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        EXPECT_CALL(*depManager, UpdateToRestoreBundleMap(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetFileHandle(_, _)).WillOnce(DoAll(SetArgReferee<1>(1), Return(UniqueFd(-1))));
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)))
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnFileReady(_, _, _, _)).WillOnce(Return());
        ret = service->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetFileHandle_0100";
}

/**
 * @tc.number: SUB_Service_OnBackupExtensionDied_0000
 * @tc.name: SUB_Service_OnBackupExtensionDied_0000
 * @tc.desc: 测试 OnBackupExtensionDied 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_OnBackupExtensionDied_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnBackupExtensionDied_0000";
    try {
        service->isOccupyingSession_ = false;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->OnBackupExtensionDied("", true);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        service->OnBackupExtensionDied("", false);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnBackupExtensionDied.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnBackupExtensionDied_0000";
}

/**
 * @tc.number: SUB_Service_ExtConnectDied_0000
 * @tc.name: SUB_Service_ExtConnectDied_0000
 * @tc.desc: 测试 ExtConnectDied 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_ExtConnectDied_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtConnectDied_0000";
    try {
        string callName;
        service->isOccupyingSession_ = false;
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->ExtConnectDied(callName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, IsExtAbilityConnected()).WillOnce(Return(false));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->ExtConnectDied(callName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, IsExtAbilityConnected()).WillOnce(Return(true));
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->ExtConnectDied(callName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtConnectDied.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtConnectDied_0000";
}

/**
 * @tc.number: SUB_Service_ExtStart_0000
 * @tc.name: SUB_Service_ExtStart_0000
 * @tc.desc: 测试 ExtStart 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_ExtStart_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtStart_0000";
    try {
        string callName;
        shared_ptr<SABackupConnection> sa = make_shared<SABackupConnection>(nullptr, nullptr, nullptr, nullptr);
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        service->ExtStart(callName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->ExtStart(callName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->ExtStart(callName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->ExtStart(callName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtStart_0000";
}

/**
 * @tc.number: SUB_Service_ExtStart_0100
 * @tc.name: SUB_Service_ExtStart_0100
 * @tc.desc: 测试 ExtStart 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_ExtStart_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtStart_0100";
    try {
        string callName;
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, HandleBackup(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, BackupOnBundleStarted(_, _)).WillOnce(Return());
        service->ExtStart(callName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, HandleBackup(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, BackupOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->ExtStart(callName);
        EXPECT_TRUE(true);

        set<string> fileNameVec;
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, HandleRestore(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        EXPECT_CALL(*session, GetExtFileNameRequest(_)).WillOnce(Return(fileNameVec));
        service->ExtStart(callName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtStart_0100";
}

/**
 * @tc.number: SUB_Service_Dump_0000
 * @tc.name: SUB_Service_Dump_0000
 * @tc.desc: 测试 Dump 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_Dump_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_Dump_0000";
    try {
        vector<u16string> args;
        auto ret = service->Dump(-1, args);
        EXPECT_EQ(ret, -1);

        ret = service->Dump(0, args);
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by Dump.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_Dump_0000";
}

/**
 * @tc.number: SUB_Service_ReportOnExtConnectFailed_0000
 * @tc.name: SUB_Service_ReportOnExtConnectFailed_0000
 * @tc.desc: 测试 ReportOnExtConnectFailed 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_ReportOnExtConnectFailed_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ReportOnExtConnectFailed_0000";
    try {
        string bundleName;
        auto session_ = service->session_;
        service->session_ = nullptr;
        service->ReportOnExtConnectFailed(IServiceReverse::Scenario::UNDEFINED, bundleName, 0);
        service->session_ = session_;
        EXPECT_TRUE(true);

        service->ReportOnExtConnectFailed(IServiceReverse::Scenario::UNDEFINED, bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnBundleStarted(_, _)).WillOnce(Return());
        service->ReportOnExtConnectFailed(IServiceReverse::Scenario::BACKUP, bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, BackupOnBundleStarted(_, _)).WillOnce(Return());
        service->ReportOnExtConnectFailed(IServiceReverse::Scenario::BACKUP, bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_CALL(*gallery, EndRestore(_, _)).WillOnce(Return(DisposeErr::OK));
        service->ReportOnExtConnectFailed(IServiceReverse::Scenario::RESTORE, bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_CALL(*gallery, EndRestore(_, _)).WillOnce(Return(DisposeErr::OK));
        service->ReportOnExtConnectFailed(IServiceReverse::Scenario::RESTORE, bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnBundleStarted(_, _)).WillOnce(Return());
        EXPECT_CALL(*gallery, EndRestore(_, _)).WillOnce(Return(DisposeErr::OK));
        service->ReportOnExtConnectFailed(IServiceReverse::Scenario::RESTORE, bundleName, 0);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ReportOnExtConnectFailed.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ReportOnExtConnectFailed_0000";
}

/**
 * @tc.number: SUB_Service_NoticeClientFinish_0000
 * @tc.name: SUB_Service_NoticeClientFinish_0000
 * @tc.desc: 测试 NoticeClientFinish 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_NoticeClientFinish_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_NoticeClientFinish_0000";
    try {
        string bundleName;
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->NoticeClientFinish(bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnBundleFinished(_, _)).WillOnce(Return());
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->NoticeClientFinish(bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleFinished(_, _)).WillOnce(Return());
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->NoticeClientFinish(bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, BackupOnBundleFinished(_, _)).WillOnce(Return());
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->NoticeClientFinish(bundleName, 0);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by NoticeClientFinish.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_NoticeClientFinish_0000";
}

/**
 * @tc.number: SUB_Service_NoticeClientFinish_0100
 * @tc.name: SUB_Service_NoticeClientFinish_0100
 * @tc.desc: 测试 NoticeClientFinish 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_NoticeClientFinish_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_NoticeClientFinish_0100";
    try {
        string bundleName;
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnBundleFinished(_, _)).WillOnce(Return());
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->NoticeClientFinish(bundleName, 0);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnBundleFinished(_, _)).WillOnce(Return());
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->NoticeClientFinish(bundleName, 0);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by NoticeClientFinish.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_NoticeClientFinish_0100";
}

/**
 * @tc.number: SUB_Service_ExtConnectDone_0000
 * @tc.name: SUB_Service_ExtConnectDone_0000
 * @tc.desc: 测试 ExtConnectDone 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_ExtConnectDone_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtConnectDone_0000";
    try {
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::WAIT));
        EXPECT_CALL(*cdConfig, InsertClearBundleRecord(_)).WillOnce(Return(true));
        service->ExtConnectDone("");
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::CLEAN));
        service->ExtConnectDone("");
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::START));
        EXPECT_CALL(*cdConfig, FindClearBundleRecord(_)).WillOnce(Return(true));
        service->ExtConnectDone("");
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::START));
        EXPECT_CALL(*cdConfig, FindClearBundleRecord(_)).WillOnce(Return(false));
        EXPECT_CALL(*cdConfig, InsertClearBundleRecord(_)).WillOnce(Return(true));
        service->ExtConnectDone("");
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtConnectDone.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtConnectDone_0000";
}

/**
 * @tc.number: SUB_Service_ClearSessionAndSchedInfo_0000
 * @tc.name: SUB_Service_ClearSessionAndSchedInfo_0000
 * @tc.desc: 测试 ClearSessionAndSchedInfo 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_ClearSessionAndSchedInfo_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ClearSessionAndSchedInfo_0000";
    try {
        string bundleName;
        service->isOccupyingSession_ = false;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        service->ClearSessionAndSchedInfo(bundleName);
        EXPECT_TRUE(true);

        service->isOccupyingSession_ = true;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->ClearSessionAndSchedInfo(bundleName);
        EXPECT_TRUE(true);

        service->failedBundles_.clear();
        service->successBundlesNum_ = 0;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        service->ClearSessionAndSchedInfo(bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ClearSessionAndSchedInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ClearSessionAndSchedInfo_0000";
}

/**
 * @tc.number: SUB_Service_HandleRestoreDepsBundle_0000
 * @tc.name: SUB_Service_HandleRestoreDepsBundle_0000
 * @tc.desc: 测试 HandleRestoreDepsBundle 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_HandleRestoreDepsBundle_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_HandleRestoreDepsBundle_0000";
    try {
        string bundleName;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        service->HandleRestoreDepsBundle(bundleName);
        EXPECT_TRUE(true);

        map<string, SvcRestoreDepsManager::RestoreInfo> bundleMap;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*depManager, GetRestoreBundleMap()).WillOnce(Return(bundleMap));
        service->HandleRestoreDepsBundle(bundleName);
        EXPECT_TRUE(true);

        bundleMap["bundleName"] = {.fileNames_ = {"name"}};
        vector<BJsonEntityCaps::BundleInfo> bundleInfos {{.name = "name"}};
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*depManager, GetRestoreBundleMap()).WillOnce(Return(bundleMap));
        EXPECT_CALL(*depManager, GetAllBundles()).WillOnce(Return(bundleInfos));
        service->HandleRestoreDepsBundle(bundleName);
        EXPECT_TRUE(true);

        bundleInfos.clear();
        bundleInfos = {{.name = "bundleName"}};
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*depManager, GetRestoreBundleMap()).WillOnce(Return(bundleMap));
        EXPECT_CALL(*depManager, GetAllBundles()).WillOnce(Return(bundleInfos));
        service->HandleRestoreDepsBundle(bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by HandleRestoreDepsBundle.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_HandleRestoreDepsBundle_0000";
}


/**
 * @tc.number: SUB_Service_OnAllBundlesFinished_0000
 * @tc.name: SUB_Service_OnAllBundlesFinished_0000
 * @tc.desc: 测试 OnAllBundlesFinished 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_OnAllBundlesFinished_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnAllBundlesFinished_0000";
    try {
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(false));
        service->OnAllBundlesFinished(0);
        EXPECT_TRUE(true);

        service->isInRelease_.store(false);
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*param, GetBackupOverrideBackupSARelease()).WillOnce(Return(true));
        service->OnAllBundlesFinished(0);
        EXPECT_TRUE(true);

        service->isInRelease_.store(true);
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*param, GetBackupOverrideBackupSARelease()).WillOnce(Return(true));
        service->OnAllBundlesFinished(0);
        EXPECT_TRUE(true);

        service->isInRelease_.store(true);
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE))
            .WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, CleanAndCheckIfNeedWait(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnAllBundlesFinished(_)).WillOnce(Return());
        EXPECT_CALL(*param, GetBackupOverrideBackupSARelease()).WillOnce(Return(true));
        service->OnAllBundlesFinished(0);
        EXPECT_TRUE(true);

        service->isInRelease_.store(false);
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnAllBundlesFinished(_)).WillOnce(Return());
        EXPECT_CALL(*param, GetBackupOverrideBackupSARelease()).WillOnce(Return(true));
        service->OnAllBundlesFinished(0);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnAllBundlesFinished_0000";
}

/**
 * @tc.number: SUB_Service_OnAllBundlesFinished_0100
 * @tc.name: SUB_Service_OnAllBundlesFinished_0100
 * @tc.desc: 测试 OnAllBundlesFinished 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_OnAllBundlesFinished_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnAllBundlesFinished_0100";
    try {
        service->isInRelease_.store(false);
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::BACKUP));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, BackupOnAllBundlesFinished(_)).WillOnce(Return());
        EXPECT_CALL(*param, GetBackupOverrideBackupSARelease()).WillOnce(Return(true));
        service->OnAllBundlesFinished(0);
        EXPECT_TRUE(true);

        service->isInRelease_.store(false);
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnAllBundlesFinished(_)).WillOnce(Return());
        EXPECT_CALL(*param, GetBackupOverrideBackupSARelease()).WillOnce(Return(true));
        service->OnAllBundlesFinished(0);
        EXPECT_TRUE(true);

        service->isInRelease_.store(false);
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnAllBundlesFinished(_)).WillOnce(Return());
        EXPECT_CALL(*param, GetBackupOverrideBackupSARelease()).WillOnce(Return(true));
        service->OnAllBundlesFinished(0);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_Service_OnAllBundlesFinished_0200
 * @tc.name: SUB_Service_OnAllBundlesFinished_0200
 * @tc.desc: 测试 OnAllBundlesFinished 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_OnAllBundlesFinished_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnAllBundlesFinished_0200";
    try {
        service->isInRelease_.store(false);
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, RestoreOnAllBundlesFinished(_)).WillOnce(Return());
        EXPECT_CALL(*param, GetBackupOverrideBackupSARelease()).WillOnce(Return(true));
        service->OnAllBundlesFinished(0);
        EXPECT_TRUE(true);

        service->isInRelease_.store(false);
        EXPECT_CALL(*session, IsOnAllBundlesFinished()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*param, GetBackupOverrideBackupSARelease()).WillOnce(Return(false));
        service->OnAllBundlesFinished(0);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnAllBundlesFinished_0200";
}

/**
 * @tc.number: SUB_Service_OnStartSched_0000
 * @tc.name: SUB_Service_OnStartSched_0000
 * @tc.desc: 测试 OnStartSched 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_OnStartSched_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_OnStartSched_0000";
    try {
        EXPECT_CALL(*session, IsOnOnStartSched()).WillOnce(Return(false));
        service->OnStartSched();
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, IsOnOnStartSched()).WillOnce(Return(true));
        service->OnStartSched();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by OnStartSched.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_OnStartSched_0000";
}

/**
 * @tc.number: SUB_Service_SendStartAppGalleryNotify_0000
 * @tc.name: SUB_Service_SendStartAppGalleryNotify_0000
 * @tc.desc: 测试 SendStartAppGalleryNotify 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_SendStartAppGalleryNotify_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SendStartAppGalleryNotify_0000";
    try {
        BundleName bundleName;
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        service->SendStartAppGalleryNotify(bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        service->SendStartAppGalleryNotify(bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*jdConfig, IfBundleNameInDisposalConfigFile(_)).WillOnce(Return(false));
        service->SendStartAppGalleryNotify(bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*jdConfig, IfBundleNameInDisposalConfigFile(_)).WillOnce(Return(true));
        EXPECT_CALL(*gallery, StartRestore(_, _)).WillOnce(Return(DisposeErr::OK));
        service->SendStartAppGalleryNotify(bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SendStartAppGalleryNotify.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SendStartAppGalleryNotify_0000";
}

/**
 * @tc.number: SUB_Service_SendEndAppGalleryNotify_0000
 * @tc.name: SUB_Service_SendEndAppGalleryNotify_0000
 * @tc.desc: 测试 SendEndAppGalleryNotify 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_SendEndAppGalleryNotify_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SendEndAppGalleryNotify_0000";
    try {
        BundleName bundleName;
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        service->SendEndAppGalleryNotify(bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        service->SendEndAppGalleryNotify(bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*gallery, EndRestore(_, _)).WillOnce(Return(DisposeErr::REQUEST_FAIL));
        service->SendEndAppGalleryNotify(bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*gallery, EndRestore(_, _)).WillOnce(Return(DisposeErr::OK));
        EXPECT_CALL(*jdConfig, DeleteFromDisposalConfigFile(_)).WillOnce(Return(false));
        service->SendEndAppGalleryNotify(bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*gallery, EndRestore(_, _)).WillOnce(Return(DisposeErr::OK));
        EXPECT_CALL(*jdConfig, DeleteFromDisposalConfigFile(_)).WillOnce(Return(true));
        service->SendEndAppGalleryNotify(bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SendEndAppGalleryNotify.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SendEndAppGalleryNotify_0000";
}

/**
 * @tc.number: SUB_Service_TryToClearDispose_0000
 * @tc.name: SUB_Service_TryToClearDispose_0000
 * @tc.desc: 测试 TryToClearDispose 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_TryToClearDispose_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_TryToClearDispose_0000";
    try {
        BundleName bundleName;
        EXPECT_CALL(*gallery, EndRestore(_, _)).WillOnce(Return(DisposeErr::OK));
        EXPECT_CALL(*jdConfig, DeleteFromDisposalConfigFile(_)).WillOnce(Return(true));
        service->TryToClearDispose(bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*gallery, EndRestore(_, _)).WillOnce(Return(DisposeErr::REQUEST_FAIL))
            .WillOnce(Return(DisposeErr::OK));
        EXPECT_CALL(*jdConfig, DeleteFromDisposalConfigFile(_)).WillOnce(Return(false));
        service->TryToClearDispose(bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by TryToClearDispose.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_TryToClearDispose_0000";
}

/**
 * @tc.number: SUB_Service_SendErrAppGalleryNotify_0000
 * @tc.name: SUB_Service_SendErrAppGalleryNotify_0000
 * @tc.desc: 测试 SendErrAppGalleryNotify 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_SendErrAppGalleryNotify_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SendErrAppGalleryNotify_0000";
    try {
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        service->SendErrAppGalleryNotify();
        EXPECT_TRUE(true);

        vector<string> bundleNameList;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNameList));
        service->SendErrAppGalleryNotify();
        EXPECT_TRUE(true);

        bundleNameList.emplace_back("bundleName");
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNameList));
        EXPECT_CALL(*gallery, EndRestore(_, _)).WillOnce(Return(DisposeErr::OK));
        EXPECT_CALL(*jdConfig, DeleteFromDisposalConfigFile(_)).WillOnce(Return(true));
        service->SendErrAppGalleryNotify();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SendErrAppGalleryNotify.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SendErrAppGalleryNotify_0000";
}

/**
 * @tc.number: SUB_Service_ClearDisposalOnSaStart_0000
 * @tc.name: SUB_Service_ClearDisposalOnSaStart_0000
 * @tc.desc: 测试 ClearDisposalOnSaStart 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_ClearDisposalOnSaStart_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ClearDisposalOnSaStart_0000";
    try {
        vector<string> bundleNameList;
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNameList));
        service->ClearDisposalOnSaStart();
        EXPECT_TRUE(true);

        bundleNameList.emplace_back("bundleName");
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNameList));
        EXPECT_CALL(*gallery, EndRestore(_, _)).WillOnce(Return(DisposeErr::OK));
        EXPECT_CALL(*jdConfig, DeleteFromDisposalConfigFile(_)).WillOnce(Return(true));
        service->ClearDisposalOnSaStart();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ClearDisposalOnSaStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ClearDisposalOnSaStart_0000";
}

/**
 * @tc.number: SUB_Service_DeleteDisConfigFile_0000
 * @tc.name: SUB_Service_DeleteDisConfigFile_0000
 * @tc.desc: 测试 DeleteDisConfigFile 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_DeleteDisConfigFile_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DeleteDisConfigFile_0000";
    try {
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        service->DeleteDisConfigFile();
        EXPECT_TRUE(true);

        vector<string> bundleNameList {"bundleName"};
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNameList));
        service->DeleteDisConfigFile();
        EXPECT_TRUE(true);

        bundleNameList.clear();
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNameList));
        EXPECT_CALL(*jdConfig, DeleteConfigFile()).WillOnce(Return(false));
        service->DeleteDisConfigFile();
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*jdConfig, GetBundleNameFromConfigFile()).WillOnce(Return(bundleNameList));
        EXPECT_CALL(*jdConfig, DeleteConfigFile()).WillOnce(Return(true));
        service->DeleteDisConfigFile();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by DeleteDisConfigFile.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DeleteDisConfigFile_0000";
}

/**
 * @tc.number: SUB_Service_SessionDeactive_0000
 * @tc.name: SUB_Service_SessionDeactive_0000
 * @tc.desc: 测试 SessionDeactive 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_SessionDeactive_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SessionDeactive_0000";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        service->SessionDeactive();
        service->session_ = session_;
        EXPECT_TRUE(true);

        auto sched = service->sched_;
        service->sched_ = nullptr;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        service->SessionDeactive();
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, CleanAndCheckIfNeedWait(_, _)).WillOnce(Return(false));
        service->SessionDeactive();
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, CleanAndCheckIfNeedWait(_, _))
            .WillOnce(DoAll(SetArgReferee<0>(BError(BError::Codes::SA_INVAL_ARG)), Return(true)));
        service->SessionDeactive();
        EXPECT_TRUE(true);

        vector<string> bundleNameList {"bundleName"};
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::RESTORE));
        EXPECT_CALL(*session, CleanAndCheckIfNeedWait(_, _))
            .WillOnce(DoAll(SetArgReferee<1>(bundleNameList), Return(true)));
        EXPECT_CALL(*cdConfig, DeleteClearBundleRecord(_)).WillOnce(Return(true));
        service->SessionDeactive();
        service->sched_ = sched;
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SessionDeactive.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SessionDeactive_0000";
}

/**
 * @tc.number: SUB_Service_SessionDeactive_0100
 * @tc.name: SUB_Service_SessionDeactive_0100
 * @tc.desc: 测试 SessionDeactive 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_SessionDeactive_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SessionDeactive_0100";
    try {
        service->failedBundles_.clear();
        service->successBundlesNum_ = 0;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, ClearSessionData()).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        EXPECT_CALL(*session, GetSessionCnt()).WillOnce(Return(0));
        service->SessionDeactive();
        EXPECT_TRUE(true);

        service->isRmConfigFile_ = true;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, ClearSessionData()).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*cdConfig, DeleteConfigFile()).WillOnce(Return(false));
        EXPECT_CALL(*session, GetSessionCnt()).WillOnce(Return(0));
        service->SessionDeactive();
        EXPECT_TRUE(true);

        service->isRmConfigFile_ = false;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, ClearSessionData()).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_CALL(*session, GetSessionCnt()).WillOnce(Return(0));
        service->SessionDeactive();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SessionDeactive.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SessionDeactive_0100";
}

/**
 * @tc.number: SUB_Service_SessionDeactive_0200
 * @tc.name: SUB_Service_SessionDeactive_0200
 * @tc.desc: 测试 SessionDeactive 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_SessionDeactive_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_SessionDeactive_0200";
    try {
        service->failedBundles_.clear();
        service->successBundlesNum_ = 0;
        service->isRmConfigFile_ = false;
        auto clearRecorder = service->clearRecorder_;
        service->clearRecorder_ = nullptr;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, ClearSessionData()).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        EXPECT_CALL(*session, GetSessionCnt()).WillOnce(Return(0));
        service->SessionDeactive();
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED))
            .WillOnce(Return(IServiceReverse::Scenario::UNDEFINED));
        EXPECT_CALL(*session, ClearSessionData()).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        EXPECT_CALL(*session, GetSessionCnt()).WillOnce(Return(-1));
        service->SessionDeactive();
        service->clearRecorder_ = clearRecorder;
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SessionDeactive.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_SessionDeactive_0200";
}

/**
 * @tc.number: SUB_Service_StartExtTimer_0000
 * @tc.name: SUB_Service_StartExtTimer_0000
 * @tc.desc: 测试 StartExtTimer 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_StartExtTimer_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartExtTimer_0000";
    try {
        bool isExtStart = false;
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto ret = service->StartExtTimer(isExtStart);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->session_ = session_;
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, StartExtTimer(_, _)).WillOnce(Return(true));
        ret = service->StartExtTimer(isExtStart);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by StartExtTimer.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartExtTimer_0000";
}

/**
 * @tc.number: SUB_Service_StartFwkTimer_0000
 * @tc.name: SUB_Service_StartFwkTimer_0000
 * @tc.desc: 测试 StartFwkTimer 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_StartFwkTimer_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartFwkTimer_0000";
    try {
        bool isFwkStart = false;
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto ret = service->StartFwkTimer(isFwkStart);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->session_ = session_;
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
        EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return(""));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, StartFwkTimer(_, _)).WillOnce(Return(true));
        ret = service->StartFwkTimer(isFwkStart);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by StartFwkTimer.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartFwkTimer_0000";
}

/**
 * @tc.number: SUB_Service_TimeoutRadarReport_0000
 * @tc.name: SUB_Service_TimeoutRadarReport_0000
 * @tc.desc: 测试 TimeoutRadarReport 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceTest, SUB_Service_TimeoutRadarReport_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_TimeoutRadarReport_0000";
    try {
        string bundleName;
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        service->TimeoutRadarReport(IServiceReverse::Scenario::BACKUP, bundleName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        service->TimeoutRadarReport(IServiceReverse::Scenario::RESTORE, bundleName);
        EXPECT_TRUE(true);

        service->TimeoutRadarReport(IServiceReverse::Scenario::UNDEFINED, bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by TimeoutRadarReport.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_TimeoutRadarReport_0000";
}