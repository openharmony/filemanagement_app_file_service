/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#include <chrono>
#include <tuple>

#include <fcntl.h>

#include <file_ex.h>
#include <gtest/gtest.h>

#include "b_utils/scan_file_singleton.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class ScanFileSingletonTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp()
    {
        std::queue<std::shared_ptr<IFileInfo>> emptyQueue;
        ScanFileSingleton::GetInstance().pendingFileQueue_.swap(emptyQueue);
        ScanFileSingleton::GetInstance().smallFiles_.clear();
    };
    void TearDown() {};
};

/**
 * @tc.number: FILE_INFO_TEST_001
 * @tc.name: FILE_INFO_TEST_001
 * @tc.desc: Test function of FileInfo and CompatFileInfo
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, FILE_INFO_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin FILE_INFO_TEST_001";
    GTEST_LOG_(INFO) << "1. test FileInfo";
    std::string filename = "test1";
    std::string filePath = "/tmp/test1";
    struct stat sta = {};
    std::shared_ptr<IFileInfo> fInfo1 = std::make_shared<FileInfo>(filename, filePath, sta, false);
    EXPECT_EQ(fInfo1->GetRestorePath(), "");

    GTEST_LOG_(INFO) << "2. test CompatFileInfo";
    std::string restorePath = "/tmp/restore/test1";
    std::shared_ptr<IFileInfo> fInfo2 = std::make_shared<CompatibleFileInfo>(filename, filePath, sta, false,
        restorePath);
    EXPECT_EQ(fInfo2->GetRestorePath(), restorePath);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end FILE_INFO_TEST_001";
}

/**
 * @tc.number: SMALL_FILE_INFO_TEST_001
 * @tc.name: SMALL_FILE_INFO_TEST_001
 * @tc.desc: Test function of SmallFileInfo and CompatSmallFileInfo
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, SMALL_FILE_INFO_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin SMALL_FILE_INFO_TEST_001";
    GTEST_LOG_(INFO) << "1. test SmallFileInfo";
    std::string filePath = "/tmp/test1";
    size_t fileSize = 100;
    std::shared_ptr<ISmallFileInfo> fInfo1 = std::make_shared<SmallFileInfo>(filePath, fileSize);
    EXPECT_EQ(fInfo1->GetRestorePath(), "");

    GTEST_LOG_(INFO) << "2. test CompatSmallFileInfo";
    std::string restorePath = "/tmp/restore/test1";
    std::shared_ptr<ISmallFileInfo> fInfo2 = std::make_shared<CompatibleSmallFileInfo>(filePath, fileSize, restorePath);
    EXPECT_EQ(fInfo2->GetRestorePath(), restorePath);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end SMALL_FILE_INFO_TEST_001";
}

/**
 * @brief 测试ScanFileSingleton的单例特性
 * @tc.number: SUB_scan_file_singleton_GetInstance_0100
 * @tc.name: scan_file_singleton_GetInstance_0100
 * @tc.desc: 测试ScanFileSingleton是否为单例，即多次获取实例返回相同对象
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, scan_file_singleton_GetInstance_0100, testing::ext::TestSize.Level1)
{
    auto& instance1 = ScanFileSingleton::GetInstance();
    auto& instance2 = ScanFileSingleton::GetInstance();
    EXPECT_EQ(&instance1, &instance2) << "ScanFileSingleton should be a singleton, but it's not.";
}

/**
 * @tc.number: ADD_BIG_FILE_TEST_001
 * @tc.name: ADD_BIG_FILE_TEST_001
 * @tc.desc: Test function of AddBigFile
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, ADD_BIG_FILE_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: ADD_BIG_FILE_TEST_001";
    GTEST_LOG_(INFO) << "1. test restorePath empty";
    std::string filePath = "test1";
    struct stat sta = {};
    ScanFileSingleton::GetInstance().AddBigFile(filePath, sta);
    auto fileInfo = ScanFileSingleton::GetInstance().GetFileInfo();
    ASSERT_NE(fileInfo, nullptr);
    EXPECT_EQ(fileInfo->GetRestorePath(), "");
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: ADD_BIG_FILE_TEST_001";
}

/**
 * @tc.number: ADD_BIG_FILE_TEST_002
 * @tc.name: ADD_BIG_FILE_TEST_002
 * @tc.desc: Test function of AddBigFile
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, ADD_BIG_FILE_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: ADD_BIG_FILE_TEST_002";
    GTEST_LOG_(INFO) << "2. test restorePath not empty";
    std::string filePath = "test2";
    std::string restorePath = "restore2";
    struct stat sta = {};
    ScanFileSingleton::GetInstance().AddBigFile(filePath, sta, restorePath);
    auto fileInfo = ScanFileSingleton::GetInstance().GetFileInfo();
    ASSERT_NE(fileInfo, nullptr);
    EXPECT_EQ(fileInfo->GetRestorePath(), restorePath);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: ADD_BIG_FILE_TEST_002";
}

/**
 * @tc.number: ADD_TAR_FILE_TEST_001
 * @tc.name: ADD_TAR_FILE_TEST_001
 * @tc.desc: Test function of AddBigFile
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, ADD_TAR_FILE_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: ADD_TAR_FILE_TEST_001";
    GTEST_LOG_(INFO) << "1. not over limit";
    std::string filename = "test";
    std::string filePath = "/tmp/test2";
    struct stat sta = {};
    EXPECT_FALSE(ScanFileSingleton::GetInstance().HasFileReady());
    ScanFileSingleton::GetInstance().percentSizeLimit_ = 10;
    ScanFileSingleton::GetInstance().currentTarSize_ = 1;
    ScanFileSingleton::GetInstance().stopPacket_ = false;
    ScanFileSingleton::GetInstance().AddTarFile(filename, filePath, sta);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 1);
    EXPECT_TRUE(ScanFileSingleton::GetInstance().HasFileReady());
    EXPECT_FALSE(ScanFileSingleton::GetInstance().stopPacket_);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: ADD_TAR_FILE_TEST_001";
}

/**
 * @tc.number: ADD_TAR_FILE_TEST_002
 * @tc.name: ADD_TAR_FILE_TEST_002
 * @tc.desc: Test function of AddBigFile
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, ADD_TAR_FILE_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: ADD_TAR_FILE_TEST_002";
    GTEST_LOG_(INFO) << "1. over limit";
    std::string filename = "test";
    std::string filePath = "/tmp/test2";
    struct stat sta = {};
    EXPECT_FALSE(ScanFileSingleton::GetInstance().HasFileReady());
    ScanFileSingleton::GetInstance().percentSizeLimit_ = 10;
    ScanFileSingleton::GetInstance().currentTarSize_ = 20;
    ScanFileSingleton::GetInstance().stopPacket_ = false;
    ScanFileSingleton::GetInstance().AddTarFile(filename, filePath, sta);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 1);
    EXPECT_TRUE(ScanFileSingleton::GetInstance().HasFileReady());
    EXPECT_TRUE(ScanFileSingleton::GetInstance().stopPacket_);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: ADD_TAR_FILE_TEST_002";
}

/**
 * @tc.number: GET_FILE_INFO_TEST_001
 * @tc.name: GET_FILE_INFO_TEST_001
 * @tc.desc: Test function of GetFileInfo
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, GET_FILE_INFO_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: GET_FILE_INFO_TEST_001";
    GTEST_LOG_(INFO) << "1. queue empty";
    EXPECT_EQ(ScanFileSingleton::GetInstance().GetFileInfo(), nullptr);

    GTEST_LOG_(INFO) << "1. queue not empty, get tar file";
    std::string filename = "test";
    std::string filePath = "/tmp/test2";
    struct stat sta = {.st_size = 10};
    ScanFileSingleton::GetInstance().AddTarFile(filename, filePath, sta);
    ScanFileSingleton::GetInstance().percentSizeLimit_ = 20;
    ScanFileSingleton::GetInstance().stopPacket_ = true;
    ScanFileSingleton::GetInstance().currentTarSize_ = 20;
    auto fileInfo = ScanFileSingleton::GetInstance().GetFileInfo();
    ASSERT_NE(fileInfo, nullptr);
    EXPECT_EQ(fileInfo->GetRestorePath(), "");
    EXPECT_FALSE(ScanFileSingleton::GetInstance().stopPacket_);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: GET_FILE_INFO_TEST_001";
}

/**
 * @tc.number: ADD_SMALL_FILE_TEST_001
 * @tc.name: ADD_SMALL_FILE_TEST_001
 * @tc.desc: Test function of AddSmallFile
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, ADD_SMALL_FILE_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: ADD_SMALL_FILE_TEST_001";
    GTEST_LOG_(INFO) << "1. test restorePath empty";
    std::string filePath = "/tmp/test2";
    size_t fileSize = 100;
    ScanFileSingleton::GetInstance().AddSmallFile(filePath, fileSize);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 1);
    auto allFiles = ScanFileSingleton::GetInstance().GetAllSmallFiles();
    ASSERT_EQ(allFiles.size(), 1);
    EXPECT_EQ(allFiles[0]->GetRestorePath(), "");
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: ADD_SMALL_FILE_TEST_001";
}

/**
 * @tc.number: ADD_SMALL_FILE_TEST_002
 * @tc.name: ADD_SMALL_FILE_TEST_002
 * @tc.desc: Test function of AddSmallFile
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, ADD_SMALL_FILE_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: ADD_SMALL_FILE_TEST_002";
    GTEST_LOG_(INFO) << "1. test restorePath not empty";
    std::string filePath = "/tmp/test2";
    size_t fileSize = 100;
    std::string restorePath = "restore2";
    ScanFileSingleton::GetInstance().AddSmallFile(filePath, fileSize, restorePath);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 1);
    auto allFiles = ScanFileSingleton::GetInstance().GetAllSmallFiles();
    ASSERT_EQ(allFiles.size(), 1);
    EXPECT_EQ(allFiles[0]->GetRestorePath(), restorePath);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: ADD_SMALL_FILE_TEST_002";
}

/**
 * @brief 测试设置和获取完成标志
 * @tc.number: SUB_scan_file_singleton_IsProcessCompleted_0100
 * @tc.name: scan_file_singleton_IsProcessCompleted_0100
 * @tc.desc: 测试ScanFileSingleton的SetCompletedFlag和GetCompletedFlag方法是否能正确设置和获取完成标志
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, scan_file_singleton_IsProcessCompleted_0100, testing::ext::TestSize.Level1)
{
    auto& instance = ScanFileSingleton::GetInstance();
    instance.SetCompletedFlag(true);
    EXPECT_TRUE(instance.IsProcessCompleted()) << "The completed flag should be true.";

    instance.SetCompletedFlag(false);
    EXPECT_FALSE(instance.IsProcessCompleted()) << "The completed flag should be false.";
}

/**
 * @brief 测试添加和获取小文件信息
 * @tc.number: SUB_scan_file_singleton_GetAllSmallFiles_0100
 * @tc.name: scan_file_singleton_GetAllSmallFiles_0100
 * @tc.desc: 测试ScanFileSingleton的GetAllSmallFiles方法是否能正确添加和获取小文件信息
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, scan_file_singleton_GetAllSmallFiles_0100, testing::ext::TestSize.Level1)
{
    auto& instance = ScanFileSingleton::GetInstance();

    instance.AddSmallFile("/path/to/small_file.txt", 512);
    auto allSmallFiles = instance.GetAllSmallFiles();

    EXPECT_EQ(allSmallFiles.size(), 1) << "There should be one small file in queue.";
    auto item = allSmallFiles[0];
    EXPECT_EQ(item->filePath_, "/path/to/small_file.txt") << "The file path should be /path/to/small_file.txt.";
    EXPECT_EQ(item->fileSize_, 512) << "The file size should be 512 bytes.";
}

/**
 * @tc.number: WAIT_FOR_FILES_TEST_001
 * @tc.name: WAIT_FOR_FILES_TEST_001
 * @tc.desc: Test function of WaitForFiles
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, WAIT_FOR_FILES_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: WAIT_FOR_FILES_TEST_001";
    GTEST_LOG_(INFO) << "1. test wait for files";
    std::string filePath = "test1";
    struct stat sta = {};
    ScanFileSingleton::GetInstance().AddBigFile(filePath, sta);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 1);

    auto start = std::chrono::steady_clock::now();
    ScanFileSingleton::GetInstance().WaitForFiles();
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 1000);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: WAIT_FOR_FILES_TEST_001";
}

/**
 * @tc.number: WAIT_FOR_PACKET_FLAG_TEST_001
 * @tc.name: WAIT_FOR_PACKET_FLAG_TEST_001
 * @tc.desc: Test function of WaitForFiles
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, WAIT_FOR_PACKET_FLAG_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: WAIT_FOR_PACKET_FLAG_TEST_001";
    GTEST_LOG_(INFO) << "1. test wait for packet";
    std::string filePath = "test1";
    struct stat sta = {};
    ScanFileSingleton::GetInstance().AddBigFile(filePath, sta);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 1);
    ScanFileSingleton::GetInstance().stopPacket_ = true;
    ScanFileSingleton::GetInstance().StartPacket();
    EXPECT_FALSE(ScanFileSingleton::GetInstance().stopPacket_);

    auto start = std::chrono::steady_clock::now();
    ScanFileSingleton::GetInstance().WaitForPacketFlag();
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 1000);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: WAIT_FOR_PACKET_FLAG_TEST_001";
}

/**
 * @tc.number: UPDATE_LIMIT_BY_TOTAL_SIZE_TEST_001
 * @tc.name: UPDATE_LIMIT_BY_TOTAL_SIZE_TEST_001
 * @tc.desc: Test function of UpdateLimitByTotalSize
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, UPDATE_LIMIT_BY_TOTAL_SIZE_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: UPDATE_LIMIT_BY_TOTAL_SIZE_TEST_001";
    ScanFileSingleton::GetInstance().UpdateLimitByTotalSize(100);
    EXPECT_EQ(ScanFileSingleton::GetInstance().percentSizeLimit_, 50);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: UPDATE_LIMIT_BY_TOTAL_SIZE_TEST_001";
}
} // namespace OHOS::FileManagement::Backup