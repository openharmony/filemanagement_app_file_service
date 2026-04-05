/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 * @tc.number: SUB_AncoBackupHelper_CreateAncoBackupTask_0000
 * @tc.name: SUB_AncoBackupHelper_CreateAncoBackupTask_0000
 * @tc.desc: 测试 CreateAncoBackupTask 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_CreateAncoBackupTask_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_CreateAncoBackupTask_0000";
    ServiceClient::serviceProxy_ = nullptr;
    AncoBackupHelper::CreateAncoBackupTask(nullptr);

    ServiceClient::serviceProxy_ = proxy;
    EXPECT_CALL(*proxy, CreateAncoBackupTask(_)).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));
    AncoBackupHelper::CreateAncoBackupTask(nullptr);

    EXPECT_CALL(*proxy, CreateAncoBackupTask(_)).WillOnce(Return(BError(BError::Codes::OK)));
    AncoBackupHelper::CreateAncoBackupTask(nullptr);
    EXPECT_TRUE(true);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_CreateAncoBackupTask_0000";
}

/**
 * @tc.number: SUB_AncoBackupHelper_DestroyAncoBackupTask_0000
 * @tc.name: SUB_AncoBackupHelper_DestroyAncoBackupTask_0000
 * @tc.desc: 测试 DestroyAncoBackupTask 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_DestroyAncoBackupTask_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_DestroyAncoBackupTask_0000";
    ServiceClient::serviceProxy_ = nullptr;
    AncoBackupHelper::DestroyAncoBackupTask();

    ServiceClient::serviceProxy_ = proxy;
    EXPECT_CALL(*proxy, DestroyAncoBackupTask()).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));
    AncoBackupHelper::DestroyAncoBackupTask();

    EXPECT_CALL(*proxy, DestroyAncoBackupTask()).WillOnce(Return(BError(BError::Codes::OK)));
    AncoBackupHelper::DestroyAncoBackupTask();
    EXPECT_TRUE(true);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_DestroyAncoBackupTask_0000";
}

/**
 * @tc.number: SUB_AncoBackupHelper_StartAncoScanAllDirs_0000
 * @tc.name: SUB_AncoBackupHelper_StartAncoScanAllDirs_0000
 * @tc.desc: 测试 StartAncoScanAllDirs 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_StartAncoScanAllDirs_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_StartAncoScanAllDirs_0000";
    ServiceClient::serviceProxy_ = nullptr;
    AncoBackupHelper::StartAncoScanAllDirs();

    ServiceClient::serviceProxy_ = proxy;
    EXPECT_CALL(*proxy, StartAncoScanAllDirs(_)).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));
    AncoBackupHelper::StartAncoScanAllDirs();

    EXPECT_CALL(*proxy, StartAncoScanAllDirs(_)).WillOnce(Return(BError(BError::Codes::OK)));
    AncoBackupHelper::StartAncoScanAllDirs();
    EXPECT_TRUE(true);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_StartAncoScanAllDirs_0000";
}

/**
 * @tc.number: SUB_AncoBackupHelper_StartAncoPacket_0000
 * @tc.name: SUB_AncoBackupHelper_StartAncoPacket_0000
 * @tc.desc: 测试 StartAncoPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_StartAncoPacket_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_StartAncoPacket_0000";
    uint64_t ancoSmallFileCount;

    ServiceClient::serviceProxy_ = nullptr;
    AncoBackupHelper::StartAncoPacket(ancoSmallFileCount);

    ServiceClient::serviceProxy_ = proxy;
    EXPECT_CALL(*proxy, StartAncoPacket(_)).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));
    AncoBackupHelper::StartAncoPacket(ancoSmallFileCount);

    EXPECT_CALL(*proxy, StartAncoPacket(_)).WillOnce(Return(BError(BError::Codes::OK)));
    AncoBackupHelper::StartAncoPacket(ancoSmallFileCount);
    EXPECT_TRUE(true);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_StartAncoPacket_0000";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_CreateAncoRestoreTask_0000
 * @tc.name: SUB_AncoIncrementalRestoreHelper_CreateAncoRestoreTask_0000
 * @tc.desc: 测试 CreateAncoRestoreTask 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_CreateAncoRestoreTask_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_CreateAncoRestoreTask_0000";
    ServiceClient::serviceProxy_ = nullptr;
    AncoIncrementalRestoreHelper::CreateAncoRestoreTask();

    ServiceClient::serviceProxy_ = proxy;
    EXPECT_CALL(*proxy, CreateAncoRestoreTask()).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));
    AncoIncrementalRestoreHelper::CreateAncoRestoreTask();

    EXPECT_CALL(*proxy, CreateAncoRestoreTask()).WillOnce(Return(BError(BError::Codes::OK)));
    AncoIncrementalRestoreHelper::CreateAncoRestoreTask();
    EXPECT_TRUE(true);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_CreateAncoRestoreTask_0000";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_DestroyAncoRestoreTask_0000
 * @tc.name: SUB_AncoIncrementalRestoreHelper_DestroyAncoRestoreTask_0000
 * @tc.desc: 测试 DestroyAncoRestoreTask 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_DestroyAncoRestoreTask_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_DestroyAncoRestoreTask_0000";
    ServiceClient::serviceProxy_ = nullptr;
    AncoIncrementalRestoreHelper::DestroyAncoRestoreTask();

    ServiceClient::serviceProxy_ = proxy;
    EXPECT_CALL(*proxy, DestroyAncoRestoreTask()).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));
    AncoIncrementalRestoreHelper::DestroyAncoRestoreTask();

    EXPECT_CALL(*proxy, DestroyAncoRestoreTask()).WillOnce(Return(BError(BError::Codes::OK)));
    AncoIncrementalRestoreHelper::DestroyAncoRestoreTask();
    EXPECT_TRUE(true);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_DestroyAncoRestoreTask_0000";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_StartAncoUnPacket_0000
 * @tc.name: SUB_AncoIncrementalRestoreHelper_StartAncoUnPacket_0000
 * @tc.desc: 测试 StartAncoUnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_StartAncoUnPacket_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_StartAncoUnPacket_0000";
    std::vector<string> ancoTarFiles;
    std::vector<int64_t> ancoTarFileSizes;
    std::vector<string> ancoTarFileNames;
    string tempPath;

    ServiceClient::serviceProxy_ = nullptr;
    AncoIncrementalRestoreHelper::StartAncoUnPacket(ancoTarFiles, ancoTarFileSizes, ancoTarFileNames, tempPath);

    ServiceClient::serviceProxy_ = proxy;
    EXPECT_CALL(*proxy, StartAncoUnPacket(_, _, _, _)).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));
    AncoIncrementalRestoreHelper::StartAncoUnPacket(ancoTarFiles, ancoTarFileSizes, ancoTarFileNames, tempPath);

    EXPECT_CALL(*proxy, StartAncoUnPacket(_, _, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
    AncoIncrementalRestoreHelper::StartAncoUnPacket(ancoTarFiles, ancoTarFileSizes, ancoTarFileNames, tempPath);
    EXPECT_TRUE(true);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_StartAncoUnPacket_0000";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_StartAncoMove_0000
 * @tc.name: SUB_AncoIncrementalRestoreHelper_StartAncoMove_0000
 * @tc.desc: 测试 StartAncoMove 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_StartAncoMove_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_StartAncoMove_0000";
    std::vector<string> ancoSourcePath;
    std::vector<string> ancoTargetPath;
    std::vector<StatInfo> ancoStats;

    ServiceClient::serviceProxy_ = nullptr;
    AncoIncrementalRestoreHelper::StartAncoMove(ancoSourcePath, ancoTargetPath, ancoStats);

    ServiceClient::serviceProxy_ = proxy;
    EXPECT_CALL(*proxy, StartAncoMove(_, _, _, _)).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));
    AncoIncrementalRestoreHelper::StartAncoMove(ancoSourcePath, ancoTargetPath, ancoStats);

    EXPECT_CALL(*proxy, StartAncoMove(_, _, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
    AncoIncrementalRestoreHelper::StartAncoMove(ancoSourcePath, ancoTargetPath, ancoStats);
    EXPECT_TRUE(true);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_StartAncoMove_0000";
}


/**
 * @tc.number: SUB_AncoBackupCallback_WaitForPacketFlag_0000
 * @tc.name: SUB_AncoBackupCallback_WaitForPacketFlag_0000
 * @tc.desc: 测试 WaitForPacketFlag 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupCallback_WaitForPacketFlag_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupCallback_WaitForPacketFlag_0000";
    ScanFileSingleton::GetInstance().SetCompletedFlag(true);
    auto callback = sptr<AncoBackupCallback>::MakeSptr(nullptr);
    callback->WaitForPacketFlag();
    EXPECT_TRUE(true);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupCallback_WaitForPacketFlag_0000";
}