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
 * @tc.number: SUB_backup_sa_session_GetLastIncrementalTime_0100
 * @tc.name: SUB_backup_sa_session_GetLastIncrementalTime_0100
 * @tc.desc: 测试 GetLastIncrementalTime
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetLastIncrementalTime_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetLastIncrementalTime_0100";
    try {
        EXPECT_TRUE(sessionManagerPtr_ != nullptr);
        sessionManagerPtr_->impl_.clientToken = 0;
        sessionManagerPtr_->GetLastIncrementalTime(BUNDLE_NAME);
        EXPECT_TRUE(true);

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->GetLastIncrementalTime(BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetLastIncrementalTime.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetLastIncrementalTime_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_DecreaseSessionCnt_0100
 * @tc.name: SUB_backup_sa_session_DecreaseSessionCnt_0100
 * @tc.desc: 测试 DecreaseSessionCnt
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_DecreaseSessionCnt_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_DecreaseSessionCnt_0100";
    try {
        sessionManagerPtr_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by DecreaseSessionCnt.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_DecreaseSessionCnt_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_DecreaseSessionCnt_0101
 * @tc.name: SUB_backup_sa_session_DecreaseSessionCnt_0101
 * @tc.desc: 测试 DecreaseSessionCnt
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_DecreaseSessionCnt_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_DecreaseSessionCnt_0101";
    try {
        sessionManagerPtr_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        sessionManagerPtr_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by DecreaseSessionCnt.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_DecreaseSessionCnt_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_GetServiceSchedAction_0104
 * @tc.name: SUB_backup_sa_session_GetServiceSchedAction_0104
 * @tc.desc: 测试 GetServiceSchedAction 获取状态
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetServiceSchedAction_0104, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetServiceSchedAction_0104";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetServiceSchedAction(BUNDLE_NAME);
            EXPECT_TRUE(true);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetServiceSchedAction.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetServiceSchedAction_0104";
}

/**
 * @tc.number: SUB_backup_sa_session_GetServiceSchedAction_0105
 * @tc.name: SUB_backup_sa_session_GetServiceSchedAction_0105
 * @tc.desc: 测试 GetServiceSchedAction 获取状态
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetServiceSchedAction_0105, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetServiceSchedAction_0105";
    try {
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::RUNNING);
            EXPECT_TRUE(true);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetServiceSchedAction.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetServiceSchedAction_0105";
}

/**
 * @tc.number: SUB_backup_sa_session_CleanAndCheckIfNeedWait_0100
 * @tc.name: SUB_backup_sa_session_CleanAndCheckIfNeedWait_0100
 * @tc.desc: 测试 CleanAndCheckIfNeedWait
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_CleanAndCheckIfNeedWait_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_CleanAndCheckIfNeedWait_0100";
    try {
        ErrCode err;
        std::vector<std::string> bundleNameList;
        bundleNameList.push_back(BUNDLE_NAME);

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();

        auto ret = sessionManagerPtr_->CleanAndCheckIfNeedWait(err, bundleNameList);
        EXPECT_FALSE(ret);

        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->impl_.restoreDataType = RestoreTypeEnum::RESTORE_DATA_READDY;
        ret = sessionManagerPtr_->CleanAndCheckIfNeedWait(err, bundleNameList);
        EXPECT_FALSE(ret);

        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = {};
        sessionManagerPtr_->impl_.restoreDataType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND;
        ret = sessionManagerPtr_->CleanAndCheckIfNeedWait(err, bundleNameList);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by CleanAndCheckIfNeedWait.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_CleanAndCheckIfNeedWait_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_CleanAndCheckIfNeedWait_0200
 * @tc.name: SUB_backup_sa_session_CleanAndCheckIfNeedWait_0200
 * @tc.desc: 测试 CleanAndCheckIfNeedWait
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_CleanAndCheckIfNeedWait_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_CleanAndCheckIfNeedWait_0200";
    try {
        ErrCode err;
        std::vector<std::string> bundleNameList;
        bundleNameList.push_back(BUNDLE_NAME);

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;

        BackupExtInfo extInfo {};
        extInfo.schedAction = BConstants::ServiceSchedAction::WAIT;
        extInfo.backUpConnection = nullptr;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        auto ret = sessionManagerPtr_->CleanAndCheckIfNeedWait(err, bundleNameList);
        EXPECT_FALSE(ret);

        BackupExtInfo extInfo2 {};
        extInfo2.schedAction = BConstants::ServiceSchedAction::RUNNING;
        extInfo2.isInPublishFile = true;
        extInfo2.backUpConnection = nullptr;
        extInfo2.fwkTimerStatus = true;
        extInfo2.extTimerStatus = false;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = extInfo2;
        ret = sessionManagerPtr_->CleanAndCheckIfNeedWait(err, bundleNameList);
        EXPECT_TRUE(ret);

        BackupExtInfo extInfo3 {};
        extInfo3.schedAction = BConstants::ServiceSchedAction::START;
        extInfo3.isInPublishFile = true;
        extInfo3.backUpConnection = nullptr;
        extInfo3.fwkTimerStatus = true;
        extInfo3.extTimerStatus = false;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = extInfo3;
        ret = sessionManagerPtr_->CleanAndCheckIfNeedWait(err, bundleNameList);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by CleanAndCheckIfNeedWait.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_CleanAndCheckIfNeedWait_0200";
}

/**
 * @tc.number: SUB_backup_sa_session_CleanAndCheckIfNeedWait_0300
 * @tc.name: SUB_backup_sa_session_CleanAndCheckIfNeedWait_0300
 * @tc.desc: 测试 CleanAndCheckIfNeedWait
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_CleanAndCheckIfNeedWait_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_CleanAndCheckIfNeedWait_0300";
    try {
        ErrCode err;
        std::vector<std::string> bundleNameList;
        bundleNameList.push_back(BUNDLE_NAME);

        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;

        BackupExtInfo extInfo {};
        extInfo.schedAction = BConstants::ServiceSchedAction::WAIT;
        extInfo.backUpConnection = sptr(new SvcBackupConnection(nullptr, nullptr, BUNDLE_NAME));;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = extInfo;
        auto ret = sessionManagerPtr_->CleanAndCheckIfNeedWait(err, bundleNameList);
        EXPECT_FALSE(ret);

        BackupExtInfo extInfo2 {};
        extInfo2.schedAction = BConstants::ServiceSchedAction::RUNNING;
        extInfo2.isInPublishFile = true;
        extInfo2.backUpConnection = sptr(new SvcBackupConnection(nullptr, nullptr, BUNDLE_NAME));;
        extInfo2.fwkTimerStatus = true;
        extInfo2.extTimerStatus = false;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = extInfo2;
        ret = sessionManagerPtr_->CleanAndCheckIfNeedWait(err, bundleNameList);
        EXPECT_TRUE(ret);

        BackupExtInfo extInfo3 {};
        extInfo3.schedAction = BConstants::ServiceSchedAction::START;
        extInfo3.isInPublishFile = true;
        extInfo3.backUpConnection = sptr(new SvcBackupConnection(nullptr, nullptr, BUNDLE_NAME));;
        extInfo3.fwkTimerStatus = true;
        extInfo3.extTimerStatus = false;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->impl_.backupExtNameMap[BUNDLE_NAME] = extInfo3;
        ret = sessionManagerPtr_->CleanAndCheckIfNeedWait(err, bundleNameList);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by CleanAndCheckIfNeedWait.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_CleanAndCheckIfNeedWait_0300";
}

/**
 * @tc.number: SUB_backup_sa_session_HandleOnRelease_0100
 * @tc.name: SUB_backup_sa_session_HandleOnRelease_0100
 * @tc.desc: 测试 HandleOnRelease
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_HandleOnRelease_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_HandleOnRelease_0100";
    try {
        sptr<SvcExtensionProxyMock> svcProxy = nullptr;
        sessionManagerPtr_->HandleOnRelease(svcProxy);

        svcProxy = sptr(new SvcExtensionProxyMock());
        EXPECT_NE(svcProxy, nullptr);
        sessionManagerPtr_->impl_.scenario = IServiceReverseType::Scenario::UNDEFINED;
        sessionManagerPtr_->HandleOnRelease(svcProxy);

        sessionManagerPtr_->impl_.scenario = IServiceReverseType::Scenario::CLEAN;
        sessionManagerPtr_->HandleOnRelease(svcProxy);

        sessionManagerPtr_->impl_.scenario = IServiceReverseType::Scenario::BACKUP;
        sessionManagerPtr_->HandleOnRelease(svcProxy);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by HandleOnRelease.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_HandleOnRelease_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_SetIsRestoreEnd_0100
 * @tc.name: SUB_backup_sa_session_SetIsRestoreEnd_0100
 * @tc.desc: 测试 SetIsRestoreEnd
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetIsRestoreEnd_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetIsRestoreEnd_0100";
    try {
        std::string bundleName = "com.test.bundleName";
        sessionManagerPtr_->impl_.clientToken = 0;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        sessionManagerPtr_->SetIsRestoreEnd(bundleName);
        EXPECT_EQ(sessionManagerPtr_->impl_.backupExtNameMap.find(bundleName),
            sessionManagerPtr_->impl_.backupExtNameMap.end());
        
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        sessionManagerPtr_->SetIsRestoreEnd(bundleName);
        EXPECT_EQ(sessionManagerPtr_->impl_.backupExtNameMap.find(bundleName),
            sessionManagerPtr_->impl_.backupExtNameMap.end());
        
        BackupExtInfo backupInfo {};
        sessionManagerPtr_->impl_.backupExtNameMap[bundleName] = backupInfo;
        EXPECT_NE(sessionManagerPtr_->impl_.backupExtNameMap.find(bundleName),
            sessionManagerPtr_->impl_.backupExtNameMap.end());
        sessionManagerPtr_->SetIsRestoreEnd(bundleName);
        bool ret = sessionManagerPtr_->impl_.backupExtNameMap[bundleName].isRestoreEnd;
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetIsRestoreEnd.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetIsRestoreEnd_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetIsRestoreEnd_0100
 * @tc.name: SUB_backup_sa_session_GetIsRestoreEnd_0100
 * @tc.desc: 测试 GetIsRestoreEnd
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetIsRestoreEnd_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetIsRestoreEnd_0100";
    try {
        std::string bundleName = "com.test.bundleName";
        sessionManagerPtr_->impl_.clientToken = 0;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        auto ret = sessionManagerPtr_->GetIsRestoreEnd(bundleName);
        EXPECT_FALSE(ret);
        
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        ret = sessionManagerPtr_->GetIsRestoreEnd(bundleName);
        EXPECT_FALSE(ret);
        
        BackupExtInfo backupInfo {};
        backupInfo.isRestoreEnd = true;
        sessionManagerPtr_->impl_.backupExtNameMap[bundleName] = backupInfo;
        EXPECT_NE(sessionManagerPtr_->impl_.backupExtNameMap.find(bundleName),
            sessionManagerPtr_->impl_.backupExtNameMap.end());
        ret = sessionManagerPtr_->GetIsRestoreEnd(bundleName);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetIsRestoreEnd.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetIsRestoreEnd_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetIsExisted_0100
 * @tc.name: SUB_backup_sa_session_GetIsExisted_0100
 * @tc.desc: 测试 GetIsExisted
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetIsExisted_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetIsExisted_0100";
    try {
        std::string bundleName = "com.test.bundleName";
        sessionManagerPtr_->impl_.clientToken = 0;
        sessionManagerPtr_->impl_.backupExtNameMap.clear();
        auto ret = sessionManagerPtr_->GetIsExisted(bundleName);
        EXPECT_TRUE(ret);
        
        sessionManagerPtr_->impl_.clientToken = CLIENT_TOKEN_ID;
        ret = sessionManagerPtr_->GetIsExisted(bundleName);
        EXPECT_FALSE(ret);
        
        BackupExtInfo backupInfo {};
        backupInfo.isRestoreEnd = true;
        sessionManagerPtr_->impl_.backupExtNameMap[bundleName] = backupInfo;
        EXPECT_NE(sessionManagerPtr_->impl_.backupExtNameMap.find(bundleName),
            sessionManagerPtr_->impl_.backupExtNameMap.end());
        ret = sessionManagerPtr_->GetIsExisted(bundleName);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetIsExisted.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetIsExisted_0100";
}