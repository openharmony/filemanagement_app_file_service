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
        try {
            EXPECT_TRUE(sessionManagerPtr_ != nullptr);
            sessionManagerPtr_->impl_.clientToken = 0;
            sessionManagerPtr_->GetLastIncrementalTime(BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_INVAL_ARG);
        }

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