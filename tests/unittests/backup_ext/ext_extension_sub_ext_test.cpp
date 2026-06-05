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
 * @tc.number: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0000
 * @tc.name: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0000
 * @tc.desc: 测试 FilterAndSaveBackupPaths 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0000";
    ServiceClient::serviceProxy_ = nullptr;
    std::set<std::string> includes;
    std::set<std::string> compatIncludes;
    std::vector<std::string> excludes;

    EXPECT_NO_THROW(AncoBackupHelper::FilterAndSaveBackupPaths(includes, compatIncludes, excludes));

    ServiceClient::serviceProxy_ = proxy;
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0000";
}

/**
 * @tc.number: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0001
 * @tc.name: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0001
 * @tc.desc: 测试 FilterAndSaveBackupPaths 接口 - 输入路径过长
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0001";
    ServiceClient::serviceProxy_ = proxy;
    std::set<std::string> includes = {std::string(MAX_IPC_SEND_DATA_SIZE, 'a')}; // 创建一个长路径
    std::set<std::string> compatIncludes;
    std::vector<std::string> excludes;

    EXPECT_NO_THROW(AncoBackupHelper::FilterAndSaveBackupPaths(includes, compatIncludes, excludes));
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0001";
}

/**
 * @tc.number: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0002
 * @tc.name: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0002
 * @tc.desc: 测试 FilterAndSaveBackupPaths 接口 - 输入路径数量超过限制
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0002";
    ServiceClient::serviceProxy_ = proxy;
    std::set<std::string> includes;
    std::set<std::string> compatIncludes;
    std::vector<std::string> excludes;

    // 添加超过限制的路径
    for (size_t i = 0; i < SET_MAX_SIZE + 1; ++i) {
        includes.insert("/data/path" + std::to_string(i));
    }

    EXPECT_NO_THROW(AncoBackupHelper::FilterAndSaveBackupPaths(includes, compatIncludes, excludes));
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0002";
}

/**
 * @tc.number: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0003
 * @tc.name: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0003
 * @tc.desc: 测试 FilterAndSaveBackupPaths 接口 - 输入路径总大小超过限制
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0003";
    ServiceClient::serviceProxy_ = proxy;
    std::set<std::string> includes;
    std::set<std::string> compatIncludes;
    std::vector<std::string> excludes;

    // 添加总大小超过限制的路径
    for (size_t i = 0; i < 1000; ++i) {
        includes.insert(std::string(10000, 'a')); // 每个路径长度为10000
    }

    EXPECT_NO_THROW(AncoBackupHelper::FilterAndSaveBackupPaths(includes, compatIncludes, excludes));
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0003";
}

/**
 * @tc.number: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0004
 * @tc.name: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0004
 * @tc.desc: 测试 FilterAndSaveBackupPaths 接口 - 输入路径为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0004";
    ServiceClient::serviceProxy_ = proxy;
    std::set<std::string> includes;
    std::set<std::string> compatIncludes;
    std::vector<std::string> excludes;

    EXPECT_NO_THROW(AncoBackupHelper::FilterAndSaveBackupPaths(includes, compatIncludes, excludes));
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0004";
}

/**
 * @tc.number: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0005
 * @tc.name: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0005
 * @tc.desc: 测试 FilterAndSaveBackupPaths 接口 - 输入路径正常
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0005";
    ServiceClient::serviceProxy_ = proxy;
    std::set<std::string> includes = {"/data/path1", "/data/path2"};
    std::set<std::string> compatIncludes = {"/data/compat1", "/data/compat2"};
    std::vector<std::string> excludes = {"/data/exclude1", "/data/exclude2"};

    EXPECT_NO_THROW(AncoBackupHelper::FilterAndSaveBackupPaths(includes, compatIncludes, excludes));
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0005";
}

/**
 * @tc.number: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0006
 * @tc.name: SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0006
 * @tc.desc: 测试 FilterAndSaveBackupPaths 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0006";
    ServiceClient::serviceProxy_ = proxy;
    std::set<std::string> includes;
    std::set<std::string> compatIncludes = {std::string(MAX_IPC_SEND_DATA_SIZE, 'a')}; // 创建一个长路径
    std::vector<std::string> excludes;

    EXPECT_NO_THROW(AncoBackupHelper::FilterAndSaveBackupPaths(includes, compatIncludes, excludes));

    compatIncludes.clear();
    excludes = {std::string(MAX_IPC_SEND_DATA_SIZE, 'a')}; // 创建一个长路径
    EXPECT_NO_THROW(AncoBackupHelper::FilterAndSaveBackupPaths(includes, compatIncludes, excludes));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_FilterAndSaveBackupPaths_0006";
}

/**
 * @tc.number: AncoBackupHelper_FilterAndSaveBackupPathsSendIncludes_0000
 * @tc.name: AncoBackupHelper_FilterAndSaveBackupPathsSendIncludes_0000
 * @tc.desc: 测试 FilterAndSaveBackupPathsSendIncludes 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, AncoBackupHelper_FilterAndSaveBackupPathsSendIncludes_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin AncoBackupHelper_FilterAndSaveBackupPathsSendIncludes_0000";
    ServiceClient::serviceProxy_ = proxy;
    std::set<std::string> includes = {std::string(MAX_IPC_SEND_DATA_SIZE + 1, 'a')}; // 创建一个长路径

    EXPECT_FALSE(FilterAndSaveBackupPathsSendIncludes(includes));

    ServiceClient::serviceProxy_ = nullptr;
    includes = {std::string(SAFE_IPC_SEND_DATA_SIZE / STRING_MEM_FACTOR_16  + 2, 'a')};
    EXPECT_FALSE(FilterAndSaveBackupPathsSendIncludes(includes));

    ServiceClient::serviceProxy_ = proxy;
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end AncoBackupHelper_FilterAndSaveBackupPathsSendIncludes_0000";
}

/**
 * @tc.number: SUB_AncoBackupHelper_FilterAndSaveBackupPathsSendCompatIncludes_0000
 * @tc.name: SUB_AncoBackupHelper_FilterAndSaveBackupPathsSendCompatIncludes_0000
 * @tc.desc: 测试 FilterAndSaveBackupPathsSendCompatIncludes 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_FilterAndSaveBackupPathsSendCompatIncludes_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_FilterAndSaveBackupPathsSendCompatIncludes_0000";
    ServiceClient::serviceProxy_ = proxy;
    std::set<std::string> compatIncludes = {std::string(MAX_IPC_SEND_DATA_SIZE + 1, 'a')}; // 创建一个长路径

    EXPECT_FALSE(FilterAndSaveBackupPathsSendCompatIncludes(compatIncludes));

    ServiceClient::serviceProxy_ = nullptr;
    compatIncludes = {std::string(SAFE_IPC_SEND_DATA_SIZE / STRING_MEM_FACTOR_16  + 2, 'a')};
    EXPECT_FALSE(FilterAndSaveBackupPathsSendCompatIncludes(compatIncludes));

    ServiceClient::serviceProxy_ = proxy;
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_FilterAndSaveBackupPathsSendCompatIncludes_0000";
}

/**
 * @tc.number: SUB_AncoBackupHelper_FilterAndSaveBackupPathsSendExcludes_0000
 * @tc.name: SUB_AncoBackupHelper_FilterAndSaveBackupPathsSendExcludes_0000
 * @tc.desc: 测试 FilterAndSaveBackupPathsSendExcludes 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupHelper_FilterAndSaveBackupPathsSendExcludes_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupHelper_FilterAndSaveBackupPathsSendExcludes_0000";
    ServiceClient::serviceProxy_ = proxy;
    std::vector<std::string> excludes = {std::string(MAX_IPC_SEND_DATA_SIZE + 1, 'a')}; // 创建一个长路径

    EXPECT_FALSE(FilterAndSaveBackupPathsSendExcludes(excludes));

    ServiceClient::serviceProxy_ = nullptr;
    excludes = {std::string(SAFE_IPC_SEND_DATA_SIZE / STRING_MEM_FACTOR_16  + 2, 'a')};
    EXPECT_FALSE(FilterAndSaveBackupPathsSendExcludes(excludes));

    ServiceClient::serviceProxy_ = proxy;
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupHelper_FilterAndSaveBackupPathsSendExcludes_0000";
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
    AncoIncrementalRestoreHelper::CreateAncoRestoreTask(nullptr);

    ServiceClient::serviceProxy_ = proxy;
    EXPECT_CALL(*proxy, CreateAncoRestoreTask(_)).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));
    AncoIncrementalRestoreHelper::CreateAncoRestoreTask(nullptr);

    EXPECT_CALL(*proxy, CreateAncoRestoreTask(_)).WillOnce(Return(BError(BError::Codes::OK)));
    AncoIncrementalRestoreHelper::CreateAncoRestoreTask(nullptr);
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
 * @tc.number: SUB_AncoIncrementalRestoreHelper_AddAncoTars_0000
 * @tc.name: SUB_AncoIncrementalRestoreHelper_AddAncoTars_0000
 * @tc.desc: 测试 AddAncoTars 接口 - 正常情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_AddAncoTars_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_AddAncoTars_0000";

    ServiceClient::serviceProxy_ = proxy;
    std::vector<std::string> ancoTarFiles = {"/data/test1.tar", "/data/test2.tar"};
    std::vector<int64_t> ancoTarFileSizes = {1024, 2048};
    std::vector<std::string> ancoTarFileNames = {"test1.tar", "test2.tar"};

    EXPECT_CALL(*proxy, AddAncoTars(_, _, _)).WillRepeatedly(Return(BError(BError::Codes::OK)));

    auto result = AncoIncrementalRestoreHelper::AddAncoTars(ancoTarFiles, ancoTarFileSizes, ancoTarFileNames);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    ancoTarFiles = {std::string(SAFE_IPC_SEND_DATA_SIZE / STRING_MEM_FACTOR_16 + 2, 'a')};
    ancoTarFileSizes = {1024};
    ancoTarFileNames = {"test1.tar"};
    result = AncoIncrementalRestoreHelper::AddAncoTars(ancoTarFiles, ancoTarFileSizes, ancoTarFileNames);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_AddAncoTars_0000";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_AddAncoTars_0001
 * @tc.name: SUB_AncoIncrementalRestoreHelper_AddAncoTars_0001
 * @tc.desc: 测试 AddAncoTars 接口 - 输入向量大小不一致
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_AddAncoTars_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_AddAncoTars_0001";

    ServiceClient::serviceProxy_ = proxy;
    std::vector<std::string> ancoTarFiles = {"/data/test1.tar"};
    std::vector<int64_t> ancoTarFileSizes = {1024, 2048};
    std::vector<std::string> ancoTarFileNames = {"test1.tar", "test2.tar"};

    auto result = AncoIncrementalRestoreHelper::AddAncoTars(ancoTarFiles, ancoTarFileSizes, ancoTarFileNames);
    EXPECT_EQ(result, BError(BError::Codes::EXT_INVAL_ARG));

    EXPECT_CALL(*proxy, AddAncoTars(_, _, _)).WillOnce(Return(BError(BError::Codes::EXT_INVAL_ARG)));
    ancoTarFiles = {std::string(SAFE_IPC_SEND_DATA_SIZE / STRING_MEM_FACTOR_16 + 2, 'a')};
    ancoTarFileSizes = {1024};
    ancoTarFileNames = {"test1.tar"};
    result = AncoIncrementalRestoreHelper::AddAncoTars(ancoTarFiles, ancoTarFileSizes, ancoTarFileNames);
    EXPECT_EQ(result, BError(BError::Codes::EXT_INVAL_ARG));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_AddAncoTars_0001";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_AddAncoTars_0002
 * @tc.name: SUB_AncoIncrementalRestoreHelper_AddAncoTars_0002
 * @tc.desc: 测试 AddAncoTars 接口 - 路径过长
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_AddAncoTars_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_AddAncoTars_0002";

    ServiceClient::serviceProxy_ = proxy;
    std::string longPath(MAX_IPC_SEND_DATA_SIZE + 1, 'a');  // 创建一个长路径
    std::vector<std::string> ancoTarFiles = {longPath};
    std::vector<int64_t> ancoTarFileSizes = {1024};
    std::vector<std::string> ancoTarFileNames = {"test1.tar"};

    auto result = AncoIncrementalRestoreHelper::AddAncoTars(ancoTarFiles, ancoTarFileSizes, ancoTarFileNames);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_AddAncoTars_0002";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_AddAncoTars_0003
 * @tc.name: SUB_AncoIncrementalRestoreHelper_AddAncoTars_0003
 * @tc.desc: 测试 AddAncoTars 接口 - 服务代理为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_AddAncoTars_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_AddAncoTars_0003";

    ServiceClient::serviceProxy_ = nullptr;
    std::vector<std::string> ancoTarFiles = {"/data/test1.tar"};
    std::vector<int64_t> ancoTarFileSizes = {1024};
    std::vector<std::string> ancoTarFileNames = {"test1.tar"};

    auto result = AncoIncrementalRestoreHelper::AddAncoTars(ancoTarFiles, ancoTarFileSizes, ancoTarFileNames);
    EXPECT_EQ(result, BError(BError::Codes::EXT_INVAL_ARG));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_AddAncoTars_0003";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_AddAncoTars_0004
 * @tc.name: SUB_AncoIncrementalRestoreHelper_AddAncoTars_0004
 * @tc.desc: 测试 AddAncoTars 接口 - 服务调用失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_AddAncoTars_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_AddAncoTars_0004";

    ServiceClient::serviceProxy_ = proxy;
    std::vector<std::string> ancoTarFiles = {"/data/test1.tar"};
    std::vector<int64_t> ancoTarFileSizes = {1024};
    std::vector<std::string> ancoTarFileNames = {"test1.tar"};

    EXPECT_CALL(*proxy, AddAncoTars(_, _, _)).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));

    auto result = AncoIncrementalRestoreHelper::AddAncoTars(ancoTarFiles, ancoTarFileSizes, ancoTarFileNames);
    EXPECT_EQ(result, BError(BError::Codes::SDK_INVAL_ARG));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_AddAncoTars_0004";
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
    string tempPath;

    ServiceClient::serviceProxy_ = nullptr;
    AncoIncrementalRestoreHelper::StartAncoUnPacket(tempPath);

    ServiceClient::serviceProxy_ = proxy;
    EXPECT_CALL(*proxy, StartAncoUnPacket(_)).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));
    AncoIncrementalRestoreHelper::StartAncoUnPacket(tempPath);

    EXPECT_CALL(*proxy, StartAncoUnPacket(_)).WillOnce(Return(BError(BError::Codes::OK)));
    AncoIncrementalRestoreHelper::StartAncoUnPacket(tempPath);
    EXPECT_TRUE(true);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_StartAncoUnPacket_0000";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0000
 * @tc.name: SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0000
 * @tc.desc: 测试 AddAncoMovePaths 接口 - 正常情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0000";

    ServiceClient::serviceProxy_ = proxy;
    std::vector<std::string> ancoSourcePath = {"/data/src1", "/data/src2"};
    std::vector<std::string> ancoTargetPath = {"/data/dst1", "/data/dst2"};
    std::vector<StatInfo> ancoStats = {{}, {}};

    EXPECT_CALL(*proxy, AddAncoMovePaths(_, _, _)).WillRepeatedly(Return(BError(BError::Codes::OK)));

    auto result = AncoIncrementalRestoreHelper::AddAncoMovePaths(ancoSourcePath, ancoTargetPath, ancoStats);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    ancoSourcePath = {std::string(SAFE_IPC_SEND_DATA_SIZE / STRING_MEM_FACTOR_16 + 2, 'a')};
    ancoTargetPath = {"/data/dst1"};
    ancoStats = {{}};
    result = AncoIncrementalRestoreHelper::AddAncoMovePaths(ancoSourcePath, ancoTargetPath, ancoStats);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0000";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0001
 * @tc.name: SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0001
 * @tc.desc: 测试 AddAncoMovePaths 接口 - 输入向量大小不一致
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0001";

    ServiceClient::serviceProxy_ = proxy;
    std::vector<std::string> ancoSourcePath = {"/data/src1"};
    std::vector<std::string> ancoTargetPath = {"/data/dst1", "/data/dst2"};
    std::vector<StatInfo> ancoStats = {{}, {}};

    auto result = AncoIncrementalRestoreHelper::AddAncoMovePaths(ancoSourcePath, ancoTargetPath, ancoStats);
    EXPECT_EQ(result, BError(BError::Codes::EXT_INVAL_ARG));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0001";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0002
 * @tc.name: SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0002
 * @tc.desc: 测试 AddAncoMovePaths 接口 - 路径过长
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0002";

    ServiceClient::serviceProxy_ = proxy;
    std::string longPath(MAX_IPC_SEND_DATA_SIZE + 1, 'a');  // 创建一个长路径
    std::vector<std::string> ancoSourcePath = {longPath};
    std::vector<std::string> ancoTargetPath = {"/data/dst1"};
    std::vector<StatInfo> ancoStats = {{}};

    auto result = AncoIncrementalRestoreHelper::AddAncoMovePaths(ancoSourcePath, ancoTargetPath, ancoStats);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0002";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0003
 * @tc.name: SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0003
 * @tc.desc: 测试 AddAncoMovePaths 接口 - 服务代理为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0003";

    ServiceClient::serviceProxy_ = nullptr;
    std::vector<std::string> ancoSourcePath = {"/data/src1"};
    std::vector<std::string> ancoTargetPath = {"/data/dst1"};
    std::vector<StatInfo> ancoStats = {{}};

    auto result = AncoIncrementalRestoreHelper::AddAncoMovePaths(ancoSourcePath, ancoTargetPath, ancoStats);
    EXPECT_EQ(result, BError(BError::Codes::EXT_INVAL_ARG));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0003";
}

/**
 * @tc.number: SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0004
 * @tc.name: SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0004
 * @tc.desc: 测试 AddAncoMovePaths 接口 - 服务调用失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0004";

    ServiceClient::serviceProxy_ = proxy;
    std::vector<std::string> ancoSourcePath = {"/data/src1"};
    std::vector<std::string> ancoTargetPath = {"/data/dst1"};
    std::vector<StatInfo> ancoStats = {{}};

    EXPECT_CALL(*proxy, AddAncoMovePaths(_, _, _)).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));

    auto result = AncoIncrementalRestoreHelper::AddAncoMovePaths(ancoSourcePath, ancoTargetPath, ancoStats);
    EXPECT_EQ(result, BError(BError::Codes::SDK_INVAL_ARG));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoIncrementalRestoreHelper_AddAncoMovePaths_0004";
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

    ServiceClient::serviceProxy_ = nullptr;
    UniqueFd fd(-1);
    auto res = AncoIncrementalRestoreHelper::StartAncoMove(fd);
    EXPECT_EQ(res.successCount, 0);

    ServiceClient::serviceProxy_ = proxy;
    EXPECT_CALL(*proxy, StartAncoMove(_, _)).WillOnce(Return(BError(BError::Codes::SDK_INVAL_ARG)));
    res = AncoIncrementalRestoreHelper::StartAncoMove(fd);
    EXPECT_EQ(res.successCount, 0);

    EXPECT_CALL(*proxy, StartAncoMove(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
    res = AncoIncrementalRestoreHelper::StartAncoMove(fd);
    EXPECT_EQ(res.successCount, 0);
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

/**
 * @tc.number: SUB_AncoBackupCallback_ReportErrFileByProc_0000
 * @tc.name: SUB_AncoBackupCallback_ReportErrFileByProc_0000
 * @tc.desc: 测试 ReportErrFileByProc 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupCallback_ReportErrFileByProc_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupCallback_ReportErrFileByProc_0000";
    ScanFileSingleton::GetInstance().SetCompletedFlag(true);
    auto callback = sptr<AncoBackupCallback>::MakeSptr(extExtension);
    std::string msg = "";
    int32_t err = 0;

    EXPECT_CALL(*extExtensionMock, ReportErrFileByProc(_, _)).WillRepeatedly(Return([](std::string, int) {}));
    EXPECT_EQ(callback->ReportErrFileByProc(msg, err), ErrCode(BError::Codes::OK));

    auto callback2 = sptr<AncoBackupCallback>::MakeSptr(nullptr);
    EXPECT_EQ(callback2->ReportErrFileByProc(msg, err), ErrCode(BError::Codes::EXT_INVAL_ARG));
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupCallback_ReportErrFileByProc_0000";
}

/**
 * @tc.number: SUB_AncoBackupCallback_OnBigFileReadyCallback_0000
 * @tc.name: SUB_AncoBackupCallback_OnBigFileReadyCallback_0000
 * @tc.desc: 测试 OnBigFileReadyCallback 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupCallback_OnBigFileReadyCallback_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupCallback_OnBigFileReadyCallback_0000";
    ScanFileSingleton::GetInstance().SetCompletedFlag(true);
    auto callback = sptr<AncoBackupCallback>::MakeSptr(extExtension);
    std::string filePath = "";
    std::string restorePath = "";
    StatInfo statInfo;

    EXPECT_EQ(callback->OnBigFileReadyCallback(filePath, restorePath, statInfo), ErrCode(BError::Codes::OK));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupCallback_OnBigFileReadyCallback_0000";
}

/**
 * @tc.number: SUB_AncoBackupCallback_OnTarFileReadyCallback_0000
 * @tc.name: SUB_AncoBackupCallback_OnTarFileReadyCallback_0000
 * @tc.desc: 测试 OnTarFileReadyCallback 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupCallback_OnTarFileReadyCallback_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupCallback_OnTarFileReadyCallback_0000";
    ScanFileSingleton::GetInstance().SetCompletedFlag(true);
    auto callback = sptr<AncoBackupCallback>::MakeSptr(nullptr);
    std::string fileName = "";
    std::string filePath = "";
    StatInfo statInfo;
    EXPECT_EQ(callback->OnTarFileReadyCallback(fileName, filePath, statInfo), ErrCode(BError::Codes::EXT_INVAL_ARG));

    auto callback2 = sptr<AncoBackupCallback>::MakeSptr(extExtension);
    EXPECT_EQ(callback2->OnTarFileReadyCallback(fileName, filePath, statInfo), ErrCode(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupCallback_OnTarFileReadyCallback_0000";
}

/**
 * @tc.number: SUB_AncoBackupCallback_UpdateFileStat_0000
 * @tc.name: SUB_AncoBackupCallback_UpdateFileStat_0000
 * @tc.desc: 测试 UpdateFileStat 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoBackupCallback_UpdateFileStat_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoBackupCallback_UpdateFileStat_0000";
    ScanFileSingleton::GetInstance().SetCompletedFlag(true);
    auto callback = sptr<AncoBackupCallback>::MakeSptr(nullptr);
    std::string filePath = "";
    StatInfo statInfo;
    EXPECT_EQ(callback->UpdateFileStat(filePath, statInfo), ErrCode(BError::Codes::EXT_INVAL_ARG));

    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoBackupCallback_UpdateFileStat_0000";
}

/**
 * @tc.number: SUB_AncoRestoreCallback_ReportFileInfos_0000
 * @tc.name: SUB_AncoRestoreCallback_ReportFileInfos_0000
 * @tc.desc: 测试 ReportFileInfos 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ExtExtensionSubTest, SUB_AncoRestoreCallback_ReportFileInfos_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin SUB_AncoRestoreCallback_ReportFileInfos_0000";
    ScanFileSingleton::GetInstance().SetCompletedFlag(true);
    auto callback = sptr<AncoRestoreCallback>::MakeSptr(nullptr);

    std::map<std::string, int64_t> endFileInfos;
    std::map<std::string, std::vector<int32_t>> errFileInfos;
    EXPECT_EQ(callback->ReportFileInfos(endFileInfos, errFileInfos), ErrCode(BError::Codes::EXT_INVAL_ARG));

    endFileInfos.emplace("1", 0);
    std::vector<int32_t> errCodes;
    errCodes.push_back(0);
    errFileInfos.emplace("2", errCodes);
    auto callback2 = sptr<AncoRestoreCallback>::MakeSptr(extExtension);
    EXPECT_EQ(callback2->ReportFileInfos(endFileInfos, errFileInfos), ErrCode(BError::Codes::OK));

    extExtension->endFileInfos_.clear();
    extExtension->errFileInfos_.clear();
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end SUB_AncoRestoreCallback_ReportFileInfos_0000";
}

/**
 * @tc.number: Ext_Extension_Sub_CallbackExit_Test_0100
 * @tc.name: Ext_Extension_Sub_CallbackExit_Test_0100
 * @tc.desc: 测试CallbackExit清空fdList
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_CallbackExit_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_CallbackExit_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);

        // 先填充 fdList_
        UniqueFd fd1(open("/dev/null", O_RDONLY));
        extExtension->fdList_.push_back(std::move(fd1));

        // 调用CallbackExit，传入COMMAND_GET_INCREMENTAL_FILE_HANDLES
        int32_t ret = extExtension->CallbackExit(
            static_cast<uint32_t>(IExtensionIpcCode::COMMAND_GET_INCREMENTAL_FILE_HANDLES), 0);

        // 验证返回值
        EXPECT_EQ(ret, ERR_NONE);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_CallbackExit_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_CallbackExit_Test_0200
 * @tc.name: Ext_Extension_Sub_CallbackExit_Test_0200
 * @tc.desc: 测试CallbackExit对其他命令不做处理
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_CallbackExit_Test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_CallbackExit_Test_0200";
    try {
        ASSERT_TRUE(extExtension != nullptr);

        // 先填充 fdList_
        UniqueFd fd1(open("/dev/null", O_RDONLY));
        extExtension->fdList_.push_back(std::move(fd1));
        size_t beforeSize = extExtension->fdList_.size();

        // 调用CallbackExit，传入其他命令码（如COMMAND_HANDLE_BACKUP）
        int32_t ret = extExtension->CallbackExit(
            static_cast<uint32_t>(IExtensionIpcCode::COMMAND_HANDLE_BACKUP), 0);

        // 验证返回值
        EXPECT_EQ(ret, ERR_NONE);
        // 验证fdList_大小不变
        EXPECT_EQ(extExtension->fdList_.size(), beforeSize);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_CallbackExit_Test_0200";
}