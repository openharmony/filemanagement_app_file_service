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
 * @tc.number: SUB_Service_CreateAncoBackupTask_0000
 * @tc.name: SUB_Service_CreateAncoBackupTask_0000
 * @tc.desc: 测试 CreateAncoBackupTask 的正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_CreateAncoBackupTask_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_CreateAncoBackupTask_0000";
    EXPECT_CALL(*mockEnhanceService, CreateAncoBackupTask(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_EQ(service->CreateAncoBackupTask(nullptr), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_CreateAncoBackupTask_0000";
}
 
/**
 * @tc.number: SUB_Service_CreateAncoBackupTask_0001
 * @tc.name: SUB_Service_CreateAncoBackupTask_0001
 * @tc.desc: 测试 CreateAncoBackupTask 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_CreateAncoBackupTask_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_CreateAncoBackupTask_0001";
    EXPECT_CALL(*mockEnhanceService, CreateAncoBackupTask(_, _)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_NE(service->CreateAncoBackupTask(nullptr), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_CreateAncoBackupTask_0001";
}
 
/**
 * @tc.number: SUB_Service_CreateAncoBackupTask_0002
 * @tc.name: SUB_Service_CreateAncoBackupTask_0002
 * @tc.desc: 测试 CreateAncoBackupTask 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_CreateAncoBackupTask_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_CreateAncoBackupTask_0002";
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
 
    EXPECT_NE(service->CreateAncoBackupTask(nullptr), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_CreateAncoBackupTask_0002";
}
 
/**
 * @tc.number: SUB_Service_CreateAncoBackupTask_0003
 * @tc.name: SUB_Service_CreateAncoBackupTask_0003
 * @tc.desc: 测试 CreateAncoBackupTask 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_CreateAncoBackupTask_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_CreateAncoBackupTask_0003";
    auto backupService = EnhanceServiceManager().GetInstance().service_;
    EnhanceServiceManager().GetInstance().service_ = nullptr;
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_EQ(service->CreateAncoBackupTask(nullptr), BError(BError::Codes::OK));
    EnhanceServiceManager().GetInstance().service_ = backupService;
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_CreateAncoBackupTask_0003";
}
 
/**
 * @tc.number: SUB_Service_DestroyAncoBackupTask_0000
 * @tc.name: SUB_Service_DestroyAncoBackupTask_0000
 * @tc.desc: 测试 DestroyAncoBackupTask 的正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_DestroyAncoBackupTask_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DestroyAncoBackupTask_0000";
    EXPECT_CALL(*mockEnhanceService, DestroyAncoBackupTask(_)).WillOnce(Return(BError(BError::Codes::OK)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_EQ(service->DestroyAncoBackupTask(), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DestroyAncoBackupTask_0000";
}
 
/**
 * @tc.number: SUB_Service_DestroyAncoBackupTask_0001
 * @tc.name: SUB_Service_DestroyAncoBackupTask_0001
 * @tc.desc: 测试 DestroyAncoBackupTask 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_DestroyAncoBackupTask_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DestroyAncoBackupTask_0001";
    EXPECT_CALL(*mockEnhanceService, DestroyAncoBackupTask(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_NE(service->DestroyAncoBackupTask(), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DestroyAncoBackupTask_0001";
}
 
/**
 * @tc.number: SUB_Service_DestroyAncoBackupTask_0002
 * @tc.name: SUB_Service_DestroyAncoBackupTask_0002
 * @tc.desc: 测试 DestroyAncoBackupTask 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_DestroyAncoBackupTask_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DestroyAncoBackupTask_0002";
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
 
    EXPECT_NE(service->DestroyAncoBackupTask(), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DestroyAncoBackupTask_0002";
}
 
/**
 * @tc.number: SUB_Service_DestroyAncoBackupTask_0003
 * @tc.name: SUB_Service_DestroyAncoBackupTask_0003
 * @tc.desc: 测试 DestroyAncoBackupTask 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_DestroyAncoBackupTask_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DestroyAncoBackupTask_0003";
    auto backupService = EnhanceServiceManager().GetInstance().service_;
    EnhanceServiceManager().GetInstance().service_ = nullptr;
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_EQ(service->DestroyAncoBackupTask(), BError(BError::Codes::OK));
    EnhanceServiceManager().GetInstance().service_ = backupService;
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DestroyAncoBackupTask_0003";
}
 
/**
 * @tc.number: SUB_Service_StartAncoScanAllDirs_0000
 * @tc.name: SUB_Service_StartAncoScanAllDirs_0000
 * @tc.desc: 测试 StartAncoScanAllDirs 的正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoScanAllDirs_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoScanAllDirs_0000";
    AncoScanResult retResult;
    retResult.smallFileSize = 100;
    retResult.bigFileSize = 200;
    EXPECT_CALL(*mockEnhanceService, StartAncoScanAllDirs(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(retResult), Return(BError(BError::Codes::OK))));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    AncoScanResult scanResult;
    EXPECT_EQ(service->StartAncoScanAllDirs(scanResult),
        BError(BError::Codes::OK));
    EXPECT_EQ(scanResult.smallFileSize, retResult.smallFileSize);
    EXPECT_EQ(scanResult.bigFileSize, retResult.bigFileSize);
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoScanAllDirs_0000";
}
 
/**
 * @tc.number: SUB_Service_StartAncoScanAllDirs_0001
 * @tc.name: SUB_Service_StartAncoScanAllDirs_0001
 * @tc.desc: 测试 StartAncoScanAllDirs 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoScanAllDirs_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoScanAllDirs_0001";
    AncoScanResult retResult;
    retResult.smallFileSize = 100;
    retResult.bigFileSize = 200;
    EXPECT_CALL(*mockEnhanceService, StartAncoScanAllDirs(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(retResult), Return(BError(BError::Codes::SA_INVAL_ARG))));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    AncoScanResult scanResult;
    EXPECT_NE(service->StartAncoScanAllDirs(scanResult), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoScanAllDirs_0001";
}
 
/**
 * @tc.number: SUB_Service_StartAncoScanAllDirs_0002
 * @tc.name: SUB_Service_StartAncoScanAllDirs_0002
 * @tc.desc: 测试 StartAncoScanAllDirs 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoScanAllDirs_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoScanAllDirs_0002";
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
 
    AncoScanResult scanResult;
    EXPECT_NE(service->StartAncoScanAllDirs(scanResult), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoScanAllDirs_0002";
}
 
/**
 * @tc.number: SUB_Service_StartAncoScanAllDirs_0003
 * @tc.name: SUB_Service_StartAncoScanAllDirs_0003
 * @tc.desc: 测试 StartAncoScanAllDirs 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoScanAllDirs_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoScanAllDirs_0003";
    auto backupService = EnhanceServiceManager().GetInstance().service_;
    EnhanceServiceManager().GetInstance().service_ = nullptr;
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    AncoScanResult scanResult;
    EXPECT_EQ(service->StartAncoScanAllDirs(scanResult), BError(BError::Codes::OK));
    EnhanceServiceManager().GetInstance().service_ = backupService;
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoScanAllDirs_0003";
}
 
/**
 * @tc.number: SUB_Service_StartAncoPacket_0000
 * @tc.name: SUB_Service_StartAncoPacket_0000
 * @tc.desc: 测试 StartAncoPacket 的正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoPacket_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoPacket_0000";
    uint64_t retSmallFileCount = 300;
    EXPECT_CALL(*mockEnhanceService, StartAncoPacket(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(retSmallFileCount), Return(BError(BError::Codes::OK))));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    uint64_t smallFileCount;
    EXPECT_EQ(service->StartAncoPacket(smallFileCount), BError(BError::Codes::OK));
    EXPECT_EQ(smallFileCount, retSmallFileCount);
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoPacket_0000";
}
 
/**
 * @tc.number: SUB_Service_StartAncoPacket_0001
 * @tc.name: SUB_Service_StartAncoPacket_0001
 * @tc.desc: 测试 StartAncoPacket 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoPacket_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoPacket_0001";
    uint64_t retSmallFileCount = 300;
    EXPECT_CALL(*mockEnhanceService, StartAncoPacket(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(retSmallFileCount), Return(BError(BError::Codes::SA_INVAL_ARG))));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    uint64_t smallFileCount;
    EXPECT_NE(service->StartAncoPacket(smallFileCount), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoPacket_0001";
}
 
/**
 * @tc.number: SUB_Service_StartAncoPacket_0002
 * @tc.name: SUB_Service_StartAncoPacket_0002
 * @tc.desc: 测试 StartAncoPacket 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoPacket_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoPacket_0002";
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
 
    uint64_t smallFileCount;
    EXPECT_NE(service->StartAncoPacket(smallFileCount), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoPacket_0002";
}
 
/**
 * @tc.number: SUB_Service_StartAncoPacket_0003
 * @tc.name: SUB_Service_StartAncoPacket_0003
 * @tc.desc: 测试 StartAncoPacket 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoPacket_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoPacket_0003";
    auto backupService = EnhanceServiceManager().GetInstance().service_;
    EnhanceServiceManager().GetInstance().service_ = nullptr;
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    uint64_t smallFileCount;
    EXPECT_EQ(service->StartAncoPacket(smallFileCount), BError(BError::Codes::OK));
    EnhanceServiceManager().GetInstance().service_ = backupService;
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoPacket_0003";
}
 
/**
 * @tc.number: SUB_Service_CreateAncoRestoreTask_0000
 * @tc.name: SUB_Service_CreateAncoRestoreTask_0000
 * @tc.desc: 测试 CreateAncoRestoreTask 的正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_CreateAncoRestoreTask_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_CreateAncoRestoreTask_0000";
    EXPECT_CALL(*mockEnhanceService, CreateAncoRestoreTask(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_EQ(service->CreateAncoRestoreTask(nullptr), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_CreateAncoRestoreTask_0000";
}
 
/**
 * @tc.number: SUB_Service_CreateAncoRestoreTask_0001
 * @tc.name: SUB_Service_CreateAncoRestoreTask_0001
 * @tc.desc: 测试 CreateAncoRestoreTask 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_CreateAncoRestoreTask_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_CreateAncoRestoreTask_0001";
    EXPECT_CALL(*mockEnhanceService, CreateAncoRestoreTask(_, _)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_NE(service->CreateAncoRestoreTask(nullptr), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_CreateAncoRestoreTask_0001";
}
 
/**
 * @tc.number: SUB_Service_CreateAncoRestoreTask_0002
 * @tc.name: SUB_Service_CreateAncoRestoreTask_0002
 * @tc.desc: 测试 CreateAncoRestoreTask 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_CreateAncoRestoreTask_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_CreateAncoRestoreTask_0002";
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
 
    EXPECT_NE(service->CreateAncoRestoreTask(nullptr), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_CreateAncoRestoreTask_0002";
}
 
/**
 * @tc.number: SUB_Service_CreateAncoRestoreTask_0003
 * @tc.name: SUB_Service_CreateAncoRestoreTask_0003
 * @tc.desc: 测试 CreateAncoRestoreTask 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_CreateAncoRestoreTask_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_CreateAncoRestoreTask_0003";
    auto backupService = EnhanceServiceManager().GetInstance().service_;
    EnhanceServiceManager().GetInstance().service_ = nullptr;
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_EQ(service->CreateAncoRestoreTask(nullptr), BError(BError::Codes::OK));
    EnhanceServiceManager().GetInstance().service_ = backupService;
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_CreateAncoRestoreTask_0003";
}
 
/**
 * @tc.number: SUB_Service_DestroyAncoRestoreTask_0000
 * @tc.name: SUB_Service_DestroyAncoRestoreTask_0000
 * @tc.desc: 测试 DestroyAncoRestoreTask 的正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_DestroyAncoRestoreTask_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DestroyAncoRestoreTask_0000";
    EXPECT_CALL(*mockEnhanceService, DestroyAncoRestoreTask(_)).WillOnce(Return(BError(BError::Codes::OK)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_EQ(service->DestroyAncoRestoreTask(), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DestroyAncoRestoreTask_0000";
}
 
/**
 * @tc.number: SUB_Service_DestroyAncoRestoreTask_0001
 * @tc.name: SUB_Service_DestroyAncoRestoreTask_0001
 * @tc.desc: 测试 DestroyAncoRestoreTask 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_DestroyAncoRestoreTask_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DestroyAncoRestoreTask_0001";
    EXPECT_CALL(*mockEnhanceService, DestroyAncoRestoreTask(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_NE(service->DestroyAncoRestoreTask(), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DestroyAncoRestoreTask_0001";
}
 
/**
 * @tc.number: SUB_Service_DestroyAncoRestoreTask_0002
 * @tc.name: SUB_Service_DestroyAncoRestoreTask_0002
 * @tc.desc: 测试 DestroyAncoRestoreTask 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_DestroyAncoRestoreTask_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DestroyAncoRestoreTask_0002";
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
 
    EXPECT_NE(service->DestroyAncoRestoreTask(), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DestroyAncoRestoreTask_0002";
}
 
/**
 * @tc.number: SUB_Service_DestroyAncoRestoreTask_0003
 * @tc.name: SUB_Service_DestroyAncoRestoreTask_0003
 * @tc.desc: 测试 DestroyAncoRestoreTask 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_DestroyAncoRestoreTask_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DestroyAncoRestoreTask_0003";
    auto backupService = EnhanceServiceManager().GetInstance().service_;
    EnhanceServiceManager().GetInstance().service_ = nullptr;
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_EQ(service->DestroyAncoRestoreTask(), BError(BError::Codes::OK));
    EnhanceServiceManager().GetInstance().service_ = backupService;
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DestroyAncoRestoreTask_0003";
}
 
/**
 * @tc.number: SUB_Service_StartAncoUnPacket_0000
 * @tc.name: SUB_Service_StartAncoUnPacket_0000
 * @tc.desc: 测试 StartAncoUnPacket 的正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoUnPacket_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoUnPacket_0000";
    EXPECT_CALL(*mockEnhanceService, StartAncoUnPacket(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    std::string rootPath;
    EXPECT_EQ(service->StartAncoUnPacket(rootPath), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoUnPacket_0000";
}
 
/**
 * @tc.number: SUB_Service_StartAncoUnPacket_0001
 * @tc.name: SUB_Service_StartAncoUnPacket_0001
 * @tc.desc: 测试 StartAncoUnPacket 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoUnPacket_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoUnPacket_0001";
    EXPECT_CALL(*mockEnhanceService, StartAncoUnPacket(_, _))
        .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));

    std::string rootPath;
    EXPECT_NE(service->StartAncoUnPacket(rootPath), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoUnPacket_0001";
}
 
/**
 * @tc.number: SUB_Service_StartAncoUnPacket_0002
 * @tc.name: SUB_Service_StartAncoUnPacket_0002
 * @tc.desc: 测试 StartAncoUnPacket 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoUnPacket_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoUnPacket_0002";
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
 
    std::string rootPath;
    EXPECT_NE(service->StartAncoUnPacket(rootPath), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoUnPacket_0002";
}
 
/**
 * @tc.number: SUB_Service_StartAncoUnPacket_0003
 * @tc.name: SUB_Service_StartAncoUnPacket_0003
 * @tc.desc: 测试 StartAncoUnPacket 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoUnPacket_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoUnPacket_0003";
    auto backupService = EnhanceServiceManager().GetInstance().service_;
    EnhanceServiceManager().GetInstance().service_ = nullptr;
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    std::string rootPath;
    EXPECT_EQ(service->StartAncoUnPacket(rootPath),
        BError(BError::Codes::OK));
    EnhanceServiceManager().GetInstance().service_ = backupService;
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoUnPacket_0003";
}
 
/**
 * @tc.number: SUB_Service_StartAncoMove_0000
 * @tc.name: SUB_Service_StartAncoMove_0000
 * @tc.desc: 测试 StartAncoMove 的正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoMove_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoMove_0000";
    AncoRestoreResult retAncoRestoreRes;
    retAncoRestoreRes.successCount = 100;
    retAncoRestoreRes.duplicateCount = 200;
    retAncoRestoreRes.failedCount = 300;
    int fd = -1;
    EXPECT_CALL(*mockEnhanceService, StartAncoMove(_, _, _))
        .WillOnce(DoAll(SetArgReferee<1>(fd), SetArgReferee<2>(retAncoRestoreRes), Return(BError(BError::Codes::OK))));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    AncoRestoreResult ancoRestoreRes;
    int dbFd = -1;
    EXPECT_EQ(service->StartAncoMove(dbFd, ancoRestoreRes), BError(BError::Codes::OK));
    EXPECT_EQ(ancoRestoreRes.successCount, retAncoRestoreRes.successCount);
    EXPECT_EQ(ancoRestoreRes.duplicateCount, retAncoRestoreRes.duplicateCount);
    EXPECT_EQ(ancoRestoreRes.failedCount, retAncoRestoreRes.failedCount);
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoMove_0000";
}
 
/**
 * @tc.number: SUB_Service_StartAncoMove_0001
 * @tc.name: SUB_Service_StartAncoMove_0001
 * @tc.desc: 测试 StartAncoMove 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoMove_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoMove_0001";
    AncoRestoreResult retAncoRestoreRes;
    retAncoRestoreRes.successCount = 100;
    retAncoRestoreRes.duplicateCount = 200;
    retAncoRestoreRes.failedCount = 300;
    int fd = -1;
    EXPECT_CALL(*mockEnhanceService, StartAncoMove(_, _, _))
        .WillOnce(DoAll(SetArgReferee<1>(fd), SetArgReferee<2>(retAncoRestoreRes),
        Return(BError(BError::Codes::SA_INVAL_ARG))));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    AncoRestoreResult ancoRestoreRes;
    int dbFd = -1;
    EXPECT_NE(service->StartAncoMove(dbFd, ancoRestoreRes), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoMove_0001";
}
 
/**
 * @tc.number: SUB_Service_StartAncoMove_0002
 * @tc.name: SUB_Service_StartAncoMove_0002
 * @tc.desc: 测试 StartAncoMove 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoMove_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoMove_0002";
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));
 
    AncoRestoreResult ancoRestoreRes;
    int fd = -1;
    EXPECT_NE(service->StartAncoMove(fd, ancoRestoreRes), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoMove_0002";
}
 
/**
 * @tc.number: SUB_Service_StartAncoMove_0003
 * @tc.name: SUB_Service_StartAncoMove_0003
 * @tc.desc: 测试 StartAncoMove 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_StartAncoMove_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_StartAncoMove_0003";
    auto backupService = EnhanceServiceManager().GetInstance().service_;
    EnhanceServiceManager().GetInstance().service_ = nullptr;
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    AncoRestoreResult ancoRestoreRes;
    int fd = -1;
    EXPECT_EQ(service->StartAncoMove(fd, ancoRestoreRes), BError(BError::Codes::OK));
    EnhanceServiceManager().GetInstance().service_ = backupService;
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoMove_0003";
}

/**
 * @tc.number: SUB_Service_AddAncoMovePaths_0000
 * @tc.name: SUB_Service_AddAncoMovePaths_0000
 * @tc.desc: 测试 AddAncoMovePaths 的正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_AddAncoMovePaths_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AddAncoMovePaths_0000";
    EXPECT_CALL(*mockEnhanceService, AddAncoMovePaths(_, _, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));

    std::vector<std::string> ancoSourcePath;
    std::vector<std::string> ancoTargetPath;
    std::vector<StatInfo> ancoStats;
    EXPECT_EQ(service->AddAncoMovePaths(ancoSourcePath, ancoTargetPath, ancoStats), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AddAncoMovePaths_0000";
}

/**
 * @tc.number: SUB_Service_AddAncoMovePaths_0001
 * @tc.name: SUB_Service_AddAncoMovePaths_0001
 * @tc.desc: 测试 AddAncoMovePaths 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_AddAncoMovePaths_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AddAncoMovePaths_0001";
    AncoRestoreResult retAncoRestoreRes;
    retAncoRestoreRes.successCount = 100;
    retAncoRestoreRes.duplicateCount = 200;
    retAncoRestoreRes.failedCount = 300;
    EXPECT_CALL(*mockEnhanceService, AddAncoMovePaths(_, _, _, _))
        .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));

    std::vector<std::string> ancoSourcePath;
    std::vector<std::string> ancoTargetPath;
    std::vector<StatInfo> ancoStats;
    EXPECT_NE(service->AddAncoMovePaths(ancoSourcePath, ancoTargetPath, ancoStats), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AddAncoMovePaths_0001";
}

/**
 * @tc.number: SUB_Service_AddAncoMovePaths_0002
 * @tc.name: SUB_Service_AddAncoMovePaths_0002
 * @tc.desc: 测试 AddAncoMovePaths 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_AddAncoMovePaths_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AddAncoMovePaths_0002";
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));

    std::vector<std::string> ancoSourcePath;
    std::vector<std::string> ancoTargetPath;
    std::vector<StatInfo> ancoStats;
    EXPECT_NE(service->AddAncoMovePaths(ancoSourcePath, ancoTargetPath, ancoStats), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AddAncoMovePaths_0002";
}

/**
 * @tc.number: SUB_Service_AddAncoMovePaths_0003
 * @tc.name: SUB_Service_AddAncoMovePaths_0003
 * @tc.desc: 测试 AddAncoMovePaths 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_AddAncoMovePaths_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AddAncoMovePaths_0003";
    auto backupService = EnhanceServiceManager().GetInstance().service_;
    EnhanceServiceManager().GetInstance().service_ = nullptr;
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));

    std::vector<std::string> ancoSourcePath;
    std::vector<std::string> ancoTargetPath;
    std::vector<StatInfo> ancoStats;
    EXPECT_EQ(service->AddAncoMovePaths(ancoSourcePath, ancoTargetPath, ancoStats), BError(BError::Codes::OK));
    EnhanceServiceManager().GetInstance().service_ = backupService;
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AddAncoMovePaths_0003";
}

/**
 * @tc.number: SUB_Service_FilterAndSaveBackupPaths_0000
 * @tc.name: SUB_Service_FilterAndSaveBackupPaths_0000
 * @tc.desc: 测试 FilterAndSaveBackupPaths 的正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_FilterAndSaveBackupPaths_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_FilterAndSaveBackupPaths_0000";
    EXPECT_CALL(*mockEnhanceService, FilterAndSaveBackupPaths(_, _, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));

    std::set<std::string> includes;
    std::set<std::string> compatIncludes;
    std::vector<std::string> excludes;
    EXPECT_EQ(service->FilterAndSaveBackupPaths(includes, compatIncludes, excludes), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_FilterAndSaveBackupPaths_0000";
}

/**
 * @tc.number: SUB_Service_FilterAndSaveBackupPaths_0001
 * @tc.name: SUB_Service_FilterAndSaveBackupPaths_0001
 * @tc.desc: 测试 FilterAndSaveBackupPaths 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_FilterAndSaveBackupPaths_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_FilterAndSaveBackupPaths_0001";
    EXPECT_CALL(*mockEnhanceService, FilterAndSaveBackupPaths(_, _, _, _))
        .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));

    std::set<std::string> includes;
    std::set<std::string> compatIncludes;
    std::vector<std::string> excludes;
    EXPECT_NE(service->FilterAndSaveBackupPaths(includes, compatIncludes, excludes), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_FilterAndSaveBackupPaths_0001";
}

/**
 * @tc.number: SUB_Service_FilterAndSaveBackupPaths_0002
 * @tc.name: SUB_Service_FilterAndSaveBackupPaths_0002
 * @tc.desc: 测试 FilterAndSaveBackupPaths 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_FilterAndSaveBackupPaths_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_FilterAndSaveBackupPaths_0002";
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));

    std::set<std::string> includes;
    std::set<std::string> compatIncludes;
    std::vector<std::string> excludes;
    EXPECT_NE(service->FilterAndSaveBackupPaths(includes, compatIncludes, excludes), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_FilterAndSaveBackupPaths_0002";
}

/**
 * @tc.number: SUB_Service_FilterAndSaveBackupPaths_0003
 * @tc.name: SUB_Service_FilterAndSaveBackupPaths_0003
 * @tc.desc: 测试 FilterAndSaveBackupPaths 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_FilterAndSaveBackupPaths_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_FilterAndSaveBackupPaths_0003";
    auto backupService = EnhanceServiceManager().GetInstance().service_;
    EnhanceServiceManager().GetInstance().service_ = nullptr;
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));

    std::set<std::string> includes;
    std::set<std::string> compatIncludes;
    std::vector<std::string> excludes;
    EXPECT_EQ(service->FilterAndSaveBackupPaths(includes, compatIncludes, excludes), BError(BError::Codes::OK));
    EnhanceServiceManager().GetInstance().service_ = backupService;
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_FilterAndSaveBackupPaths_0003";
}

/**
 * @tc.number: SUB_Service_AddAncoTars_0000
 * @tc.name: SUB_Service_AddAncoTars_0000
 * @tc.desc: 测试 AddAncoTars 的正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_AddAncoTars_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AddAncoTars_0000";
    EXPECT_CALL(*mockEnhanceService, AddAncoTars(_, _, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));

    std::vector<string> tarFiles;
    std::vector<int64_t> tarFileSizes;
    std::vector<string> tarFileNames;
    EXPECT_EQ(service->AddAncoTars(tarFiles, tarFileSizes, tarFileNames), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AddAncoTars_0000";
}

/**
 * @tc.number: SUB_Service_AddAncoTars_0001
 * @tc.name: SUB_Service_AddAncoTars_0001
 * @tc.desc: 测试 AddAncoTars 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_AddAncoTars_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AddAncoTars_0001";
    EXPECT_CALL(*mockEnhanceService, AddAncoTars(_, _, _, _))
        .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));

    std::vector<string> tarFiles;
    std::vector<int64_t> tarFileSizes;
    std::vector<string> tarFileNames;
    EXPECT_NE(service->AddAncoTars(tarFiles, tarFileSizes, tarFileNames), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AddAncoTars_0001";
}

/**
 * @tc.number: SUB_Service_AddAncoTars_0002
 * @tc.name: SUB_Service_AddAncoTars_0002
 * @tc.desc: 测试 AddAncoTars 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_AddAncoTars_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AddAncoTars_0002";
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL));

    std::vector<string> tarFiles;
    std::vector<int64_t> tarFileSizes;
    std::vector<string> tarFileNames;
    EXPECT_NE(service->AddAncoTars(tarFiles, tarFileSizes, tarFileNames), BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AddAncoTars_0002";
}

/**
 * @tc.number: SUB_Service_AddAncoTars_0003
 * @tc.name: SUB_Service_AddAncoTars_0003
 * @tc.desc: 测试 AddAncoTars 的异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_AddAncoTars_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_AddAncoTars_0003";
    auto backupService = EnhanceServiceManager().GetInstance().service_;
    EnhanceServiceManager().GetInstance().service_ = nullptr;
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));

    std::vector<string> tarFiles;
    std::vector<int64_t> tarFileSizes;
    std::vector<string> tarFileNames;
    EXPECT_EQ(service->AddAncoTars(tarFiles, tarFileSizes, tarFileNames), BError(BError::Codes::OK));
    EnhanceServiceManager().GetInstance().service_ = backupService;
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_AddAncoTars_0003";
}

/**
 * @tc.number: SUB_Service_GetMigrateUidGid_0000
 * @tc.name: SUB_Service_GetMigrateUidGid_0000
 * @tc.desc: 测试 GetMigrateUidGid 的ANCO_DATA_PATH分支和GetUidGidForBundleName失败分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_GetMigrateUidGid_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetMigrateUidGid_0000";
    try {
        uid_t uid = 0;
        gid_t gid = 0;
        ErrCode ret = service->GetMigrateUidGid(BConstants::MIGRATE_ANCO_DATA_PATH, "bundleName", 100, uid, gid);
        EXPECT_EQ(ret, ERR_OK);
        EXPECT_EQ(uid, BConstants::SYSTEM_UID_GID);
        EXPECT_EQ(gid, BConstants::SYSTEM_UID_GID);

        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _)).WillOnce(Return(false));
        ret = service->GetMigrateUidGid("normal_path", "bundleName", 100, uid, gid);
        EXPECT_NE(ret, ERR_OK);

        uid = 0;
        gid = 0;
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillOnce(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        ret = service->GetMigrateUidGid("normal_path", "bundleName", 100, uid, gid);
        EXPECT_EQ(ret, ERR_OK);
        EXPECT_EQ(uid, 1001);
        EXPECT_EQ(gid, 1001);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetMigrateUidGid.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetMigrateUidGid_0000";
}

/**
 * @tc.number: SUB_Service_DoEnhanceMove_0000
 * @tc.name: SUB_Service_DoEnhanceMove_0000
 * @tc.desc: 测试 DoEnhanceMove enhanceService为空和正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_DoEnhanceMove_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DoEnhanceMove_0000";
    try {
        auto backupService = EnhanceServiceManager().GetInstance().service_;
        EnhanceServiceManager().GetInstance().service_ = nullptr;
        int32_t errCode = 0;
        ErrCode ret = service->DoEnhanceMove("/src", "/dest", 1000, 1000, errCode, false);
        EXPECT_NE(ret, ERR_OK);
        EnhanceServiceManager().GetInstance().service_ = backupService;

        EXPECT_CALL(*mockEnhanceService, MoveFiles(_, _)).WillOnce(Return(ERR_OK));
        ret = service->DoEnhanceMove("/src", "/dest", 1000, 1000, errCode, false);
        EXPECT_EQ(ret, ERR_OK);

        EXPECT_CALL(*mockEnhanceService, MoveDirectory(_, _)).WillOnce(Return(ERR_OK));
        ret = service->DoEnhanceMove("/src", "/dest", 1000, 1000, errCode, true);
        EXPECT_EQ(ret, ERR_OK);

        EXPECT_CALL(*mockEnhanceService, MoveFiles(_, _)).WillOnce(Return(1));
        ret = service->DoEnhanceMove("/src", "/dest", 1000, 1000, errCode, false);
        EXPECT_NE(ret, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by DoEnhanceMove.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DoEnhanceMove_0000";
}

/**
 * @tc.number: SUB_Service_DoEnhanceOpen_0000
 * @tc.name: SUB_Service_DoEnhanceOpen_0000
 * @tc.desc: 测试 DoEnhanceOpen enhanceService为空和正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_DoEnhanceOpen_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_DoEnhanceOpen_0000";
    try {
        auto backupService = EnhanceServiceManager().GetInstance().service_;
        EnhanceServiceManager().GetInstance().service_ = nullptr;
        int fd = -1;
        ErrCode ret = service->DoEnhanceOpen("/path/file", 1000, 1000, fd);
        EXPECT_NE(ret, ERR_OK);
        EnhanceServiceManager().GetInstance().service_ = backupService;

        FileBackupResultMsg resultMsg;
        ResultParam resParam;
        resParam.fd = 3;
        resultMsg.resInfo.push_back(resParam);
        EXPECT_CALL(*mockEnhanceService, GetApkFileHandle(_, _))
            .WillOnce(DoAll(SetArgReferee<1>(resultMsg), Return(ERR_OK)));
        ret = service->DoEnhanceOpen("/path/file", 1000, 1000, fd);
        EXPECT_EQ(ret, ERR_OK);
        EXPECT_EQ(fd, 3);

        EXPECT_CALL(*mockEnhanceService, GetApkFileHandle(_, _)).WillOnce(Return(1));
        ret = service->DoEnhanceOpen("/path/file", 1000, 1000, fd);
        EXPECT_NE(ret, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by DoEnhanceOpen.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_DoEnhanceOpen_0000";
}

/**
 * @tc.number: SUB_Service_MigrateFilePrecheck_0000
 * @tc.name: SUB_Service_MigrateFilePrecheck_0000
 * @tc.desc: 测试 MigrateFilePrecheck session为空和VerifyDataClone失败分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFilePrecheck_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFilePrecheck_0000";
    try {
        BPathInfo path("/data/src", "/data/dest");
        ErrCode ret = service->MigrateFilePrecheck("bundleName", path);
        EXPECT_NE(ret, ERR_OK);

        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        ret = service->MigrateFilePrecheck("bundleName", path);
        EXPECT_EQ(ret, ERR_OK);
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFilePrecheck.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFilePrecheck_0000";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0000
 * @tc.name: SUB_Service_MigrateFile_0000
 * @tc.desc: 测试 MigrateFile VerifyDataClone失败和正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0000";
    try {
        BPathInfo path("/data/src", "/data/dest");
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_NE(ret, ERR_OK);

        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*session, GetExtConnection(_)).WillRepeatedly(Return(connect));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        EXPECT_CALL(*mockEnhanceService, MoveFiles(_, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(0, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalRpFileHandle(_, _)).WillOnce(Return(ERR_OK));
        ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, ERR_OK);

        EXPECT_CALL(*mockEnhanceService, MoveDirectory(_, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(0, _))
            .Times(testing::AtLeast(1)).WillRepeatedly(Return(ERR_OK));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalRpFileHandle(_, _)).WillOnce(Return(ERR_OK));
        ret = service->MigrateFile(path, "bundleName", "");
        EXPECT_EQ(ret, ERR_OK);
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0000";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0001
 * @tc.name: SUB_Service_MigrateFile_0001
 * @tc.desc: 测试 MigrateFile 路径非法分支，返回 SA_INVAL_ARG
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0001";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/../src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0001.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0001";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0002
 * @tc.name: SUB_Service_MigrateFile_0002
 * @tc.desc: 测试 MigrateFile GetUidGidForBundleName 失败分支，返回 SA_INVAL_ARG
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0002";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(BError(BError::Codes::SA_INVAL_ARG).GetCode(), _))
            .WillOnce(Return(ERR_OK));
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0002.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0002";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0003
 * @tc.name: SUB_Service_MigrateFile_0003
 * @tc.desc: 测试 MigrateFile enhance service 不可用分支，返回 SA_INVAL_ARG
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0003";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(BError(BError::Codes::SA_INVAL_ARG).GetCode(), _))
            .WillOnce(Return(ERR_OK));
        auto backupService = EnhanceServiceManager::GetInstance().service_;
        EnhanceServiceManager::GetInstance().service_ = nullptr;
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        EnhanceServiceManager::GetInstance().service_ = backupService;
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0003.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0003";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0004
 * @tc.name: SUB_Service_MigrateFile_0004
 * @tc.desc: 测试 MigrateFile MoveFiles 失败分支，返回 SA_INVAL_ARG
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0004";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        EXPECT_CALL(*mockEnhanceService, MoveFiles(_, _)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(BError(BError::Codes::SA_INVAL_ARG).GetCode(), _))
            .WillOnce(Return(ERR_OK));
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0004.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0004";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0005
 * @tc.name: SUB_Service_MigrateFile_0005
 * @tc.desc: 测试 MigrateFile OpenIncrementalRpFile proxy为空分支，返回 SA_INVAL_ARG
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0005";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        EXPECT_CALL(*mockEnhanceService, MoveFiles(_, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(BError(BError::Codes::SA_INVAL_ARG).GetCode(), _))
            .WillOnce(Return(ERR_OK));
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0005.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0005";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0006
 * @tc.name: SUB_Service_MigrateFile_0006
 * @tc.desc: 测试 MigrateFile VerifyCaller 权限不足分支，返回 SA_REFUSED_ACT (13900001)
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0006";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_DENIED));
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0006.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0006";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0007
 * @tc.name: SUB_Service_MigrateFile_0007
 * @tc.desc: 测试 MigrateFile MoveFiles 返回 EIO 错误，覆盖 I/O error (13900005)
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0007";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        FileBackupResultMsg resultMsg;
        resultMsg.errorCode = EIO;
        EXPECT_CALL(*mockEnhanceService, MoveFiles(_, _))
            .WillOnce(DoAll(SetArgReferee<1>(resultMsg), Return(ERR_OK)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalRpFileHandle(_, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(EIO, _)).WillOnce(Return(ERR_OK));
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, ERR_OK);
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0007.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0007";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0008
 * @tc.name: SUB_Service_MigrateFile_0008
 * @tc.desc: 测试 MigrateFile MoveFiles 返回 ENOMEM 错误，覆盖 Out of memory (13900011)
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0008";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        FileBackupResultMsg resultMsg;
        resultMsg.errorCode = ENOMEM;
        EXPECT_CALL(*mockEnhanceService, MoveFiles(_, _))
            .WillOnce(DoAll(SetArgReferee<1>(resultMsg), Return(ERR_OK)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalRpFileHandle(_, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(ENOMEM, _)).WillOnce(Return(ERR_OK));
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, ERR_OK);
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0008.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0008";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0009
 * @tc.name: SUB_Service_MigrateFile_0009
 * @tc.desc: 测试 MigrateFile MoveFiles 返回 ENOSPC 错误，覆盖 No space left (13900025)
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0009";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        FileBackupResultMsg resultMsg;
        resultMsg.errorCode = ENOSPC;
        EXPECT_CALL(*mockEnhanceService, MoveFiles(_, _))
            .WillOnce(DoAll(SetArgReferee<1>(resultMsg), Return(ERR_OK)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalRpFileHandle(_, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(ENOSPC, _)).WillOnce(Return(ERR_OK));
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, ERR_OK);
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0009.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0009";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0010
 * @tc.name: SUB_Service_MigrateFile_0010
 * @tc.desc: 测试 MigrateFile MoveDirectory 失败分支，返回 SA_INVAL_ARG
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0010";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        EXPECT_CALL(*mockEnhanceService, MoveDirectory(_, _))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(BError(BError::Codes::SA_INVAL_ARG).GetCode(), _))
            .WillOnce(Return(ERR_OK));
        ErrCode ret = service->MigrateFile(path, "bundleName", "");
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0010.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0010";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0011
 * @tc.name: SUB_Service_MigrateFile_0011
 * @tc.desc: 测试 MigrateFile GetIncrementalRpFileHandle 返回错误分支，返回 SA_INVAL_ARG
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0011, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0011";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        EXPECT_CALL(*mockEnhanceService, MoveFiles(_, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalRpFileHandle(_, _))
            .WillOnce(DoAll(SetArgReferee<1>(EIO), Return(ERR_OK)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(BError(BError::Codes::SA_INVAL_ARG).GetCode(), _))
            .WillOnce(Return(ERR_OK));
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0011.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0011";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0012
 * @tc.name: SUB_Service_MigrateFile_0012
 * @tc.desc: 测试 MigrateFile MoveFiles 返回 EPERM 错误，覆盖 Operation not permitted (13900001)，
 *           onMigrateResult 回调传递非零 moveErrCode
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0012, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0012";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        FileBackupResultMsg resultMsg;
        resultMsg.errorCode = EPERM;
        EXPECT_CALL(*mockEnhanceService, MoveFiles(_, _))
            .WillOnce(DoAll(SetArgReferee<1>(resultMsg), Return(ERR_OK)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalRpFileHandle(_, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(EPERM, _)).WillOnce(Return(ERR_OK));
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, ERR_OK);
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0012.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0012";
}

/**
 * @tc.number: SUB_Service_MigrateFile_0013
 * @tc.name: SUB_Service_MigrateFile_0013
 * @tc.desc: 测试 MigrateFile OpenIncrementalRpFile 中 backUpConnection 为空分支，返回 ERR_OK
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_MigrateFile_0013, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_MigrateFile_0013";
    try {
        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        BPathInfo path("/data/src", "/data/dest");
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        EXPECT_CALL(*mockEnhanceService, MoveFiles(_, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnMigrateResult(0, _)).WillOnce(Return(ERR_OK));
        ErrCode ret = service->MigrateFile(path, "bundleName", "fileName");
        EXPECT_EQ(ret, ERR_OK);
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by MigrateFile_0013.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_MigrateFile_0013";
}

/**
 * @tc.number: SUB_Service_GetApkFileHandle_0000
 * @tc.name: SUB_Service_GetApkFileHandle_0000
 * @tc.desc: 测试 GetApkFileHandle VerifyCaller失败和正常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ServiceTest, SUB_Service_GetApkFileHandle_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetApkFileHandle_0000";
    try {
        int fd = -1;
        ErrCode ret = service->GetApkFileHandle("/path", "fileName", fd);
        EXPECT_NE(ret, ERR_OK);

        g_verifyDataCloneResult = true;
        service->session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(service)));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillRepeatedly(Return(0));
        EXPECT_CALL(*token, GetTokenType(_))
            .WillRepeatedly(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE));
        EXPECT_CALL(*token, VerifyAccessToken(_, _))
            .WillRepeatedly(Return(Security::AccessToken::PermissionState::PERMISSION_GRANTED));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*session, GetSessionUserId()).WillRepeatedly(Return(100));
        EXPECT_CALL(*bms, GetUidGidForBundleName(_, _, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<2>(1001), SetArgReferee<3>(1001), Return(true)));
        FileBackupResultMsg resultMsg;
        ResultParam resParam;
        resParam.fd = 5;
        resultMsg.resInfo.push_back(resParam);
        EXPECT_CALL(*mockEnhanceService, GetApkFileHandle(_, _))
            .WillOnce(DoAll(SetArgReferee<1>(resultMsg), Return(ERR_OK)));
        ret = service->GetApkFileHandle("/path", "fileName", fd);
        EXPECT_EQ(ret, ERR_OK);
        EXPECT_EQ(fd, 5);
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
    } catch (...) {
        service->session_ = nullptr;
        g_verifyDataCloneResult = false;
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetApkFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetApkFileHandle_0000";
}
