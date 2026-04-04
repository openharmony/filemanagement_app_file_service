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
 
    EXPECT_EQ(service->CreateAncoBackupTask(nullptr), BError(BError::Codes::ENHANCE_SERVICE_NOT_LOAD));
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
 
    EXPECT_EQ(service->DestroyAncoBackupTask(), BError(BError::Codes::ENHANCE_SERVICE_NOT_LOAD));
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
    EXPECT_EQ(service->StartAncoScanAllDirs(scanResult), BError(BError::Codes::ENHANCE_SERVICE_NOT_LOAD));
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
    EXPECT_EQ(service->StartAncoPacket(smallFileCount), BError(BError::Codes::ENHANCE_SERVICE_NOT_LOAD));
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
    EXPECT_CALL(*mockEnhanceService, CreateAncoRestoreTask(_)).WillOnce(Return(BError(BError::Codes::OK)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_EQ(service->CreateAncoRestoreTask(), BError(BError::Codes::OK));
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
    EXPECT_CALL(*mockEnhanceService, CreateAncoRestoreTask(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    EXPECT_NE(service->CreateAncoRestoreTask(), BError(BError::Codes::OK));
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
 
    EXPECT_NE(service->CreateAncoRestoreTask(), BError(BError::Codes::OK));
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
 
    EXPECT_EQ(service->CreateAncoRestoreTask(), BError(BError::Codes::ENHANCE_SERVICE_NOT_LOAD));
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
 
    EXPECT_EQ(service->DestroyAncoRestoreTask(), BError(BError::Codes::ENHANCE_SERVICE_NOT_LOAD));
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
    EXPECT_CALL(*mockEnhanceService, StartAncoUnPacket(_, _, _, _, _)).WillOnce(Return(BError(BError::Codes::OK)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    std::vector<std::string> tarFiles;
    std::vector<int64_t> tarFileSizes;
    std::vector<std::string> tarFileNames;
    std::string rootPath;
    EXPECT_EQ(service->StartAncoUnPacket(tarFiles, tarFileSizes, tarFileNames, rootPath),
        BError(BError::Codes::OK));
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
    EXPECT_CALL(*mockEnhanceService, StartAncoUnPacket(_, _, _, _, _))
        .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    std::vector<std::string> tarFiles;
    std::vector<int64_t> tarFileSizes;
    std::vector<std::string> tarFileNames;
    std::string rootPath;
    EXPECT_NE(service->StartAncoUnPacket(tarFiles, tarFileSizes, tarFileNames, rootPath),
        BError(BError::Codes::OK));
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
 
    std::vector<std::string> tarFiles;
    std::vector<int64_t> tarFileSizes;
    std::vector<std::string> tarFileNames;
    std::string rootPath;
    EXPECT_NE(service->StartAncoUnPacket(tarFiles, tarFileSizes, tarFileNames, rootPath),
        BError(BError::Codes::OK));
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
 
    std::vector<std::string> tarFiles;
    std::vector<int64_t> tarFileSizes;
    std::vector<std::string> tarFileNames;
    std::string rootPath;
    EXPECT_EQ(service->StartAncoUnPacket(tarFiles, tarFileSizes, tarFileNames, rootPath),
        BError(BError::Codes::ENHANCE_SERVICE_NOT_LOAD));
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
    EXPECT_CALL(*mockEnhanceService, StartAncoMove(_, _, _, _, _))
        .WillOnce(DoAll(SetArgReferee<4>(retAncoRestoreRes), Return(BError(BError::Codes::OK))));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    std::vector<std::string> ancoSourcePath;
    std::vector<std::string> ancoTargetPath;
    std::vector<StatInfo> ancoStats;
    AncoRestoreResult ancoRestoreRes;
    EXPECT_EQ(service->StartAncoMove(ancoSourcePath, ancoTargetPath, ancoStats, ancoRestoreRes),
        BError(BError::Codes::OK));
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
    EXPECT_CALL(*mockEnhanceService, StartAncoMove(_, _, _, _, _))
        .WillOnce(DoAll(SetArgReferee<4>(retAncoRestoreRes), Return(BError(BError::Codes::SA_INVAL_ARG))));
    EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
    EXPECT_CALL(*token, GetTokenType(_)).WillOnce(Return(Security::AccessToken::ATokenTypeEnum::TOKEN_HAP));
    EXPECT_CALL(*token, GetHapTokenInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
 
    std::vector<std::string> ancoSourcePath;
    std::vector<std::string> ancoTargetPath;
    std::vector<StatInfo> ancoStats;
    AncoRestoreResult ancoRestoreRes;
    EXPECT_NE(service->StartAncoMove(ancoSourcePath, ancoTargetPath, ancoStats, ancoRestoreRes),
        BError(BError::Codes::OK));
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
 
    std::vector<std::string> ancoSourcePath;
    std::vector<std::string> ancoTargetPath;
    std::vector<StatInfo> ancoStats;
    AncoRestoreResult ancoRestoreRes;
    EXPECT_NE(service->StartAncoMove(ancoSourcePath, ancoTargetPath, ancoStats, ancoRestoreRes),
        BError(BError::Codes::OK));
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
 
    std::vector<std::string> ancoSourcePath;
    std::vector<std::string> ancoTargetPath;
    std::vector<StatInfo> ancoStats;
    AncoRestoreResult ancoRestoreRes;
    EXPECT_EQ(service->StartAncoMove(ancoSourcePath, ancoTargetPath, ancoStats, ancoRestoreRes),
        BError(BError::Codes::ENHANCE_SERVICE_NOT_LOAD));
    EnhanceServiceManager().GetInstance().service_ = backupService;
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_StartAncoMove_0003";
}