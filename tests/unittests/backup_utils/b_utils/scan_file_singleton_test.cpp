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

constexpr uint64_t ONE_HUNDRED_FIFTY_MB = 150ULL * 1024 * 1024; // 150MB

namespace OHOS::FileManagement::Backup {
using namespace std;

class ScanFileSingletonTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp()
    {
        std::queue<std::shared_ptr<IFileInfo>> emptyQueue;
        auto& instance = ScanFileSingleton::GetInstance();
        instance.pendingFileQueue_.swap(emptyQueue);
        instance.smallFiles_.clear();
        instance.stopPacket_.store(false);
        instance.currentTarSize_.store(0);
        instance.maxTarSize_.store(ONE_HUNDRED_FIFTY_MB);
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
* @tc.number: FILE_INFO_TEST_002
* @tc.name: FILE_INFO_TEST_002
* @tc.desc: Test function of FileInfo and CompatFileInfo
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: NA
*/
HWTEST_F(ScanFileSingletonTest, ANCO_FILE_INFO_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin ANCO_FILE_INFO_TEST_001";
    GTEST_LOG_(INFO) << "1. test AncoFileInfo";
    std::string filename = "test_anco";
    std::string filePath = "/tmp/test_anco";
    struct stat sta = {};
    std::shared_ptr<IFileInfo> fInfo1 = std::make_shared<AncoFileInfo>(filename, filePath, sta, false);
    EXPECT_EQ(fInfo1->GetRestorePath(), "");

    GTEST_LOG_(INFO) << "2. test AncoFileInfo";
    std::shared_ptr<IFileInfo> fInfo2 = std::make_shared<AncoFileInfo>(filename, filePath, sta, false);
    EXPECT_EQ(fInfo2->filename_, filename);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end ANCO_FILE_INFO_TEST_001";
}

/**
* @tc.number: FILE_INFO_TEST_002
* @tc.name: FILE_INFO_TEST_002
* @tc.desc: Test function of FileInfo and CompatFileInfo
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: NA
*/
HWTEST_F(ScanFileSingletonTest, ANCO_COMPATIBLEFILE_FILE_INFO_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin ANCO_COMPATIBLEFILE_FILE_INFO_TEST_001";
    GTEST_LOG_(INFO) << "1. test AncoCompatibleFileInfo";
    std::string filename = "test_anco_compatible";
    std::string filePath = "/tmp/test_anco_compatible";
    struct stat sta = {};
    std::string restorePath = "/tmp/restore/test_anco_compatible";
    std::shared_ptr<IFileInfo> fInfo1 = std::make_shared<AncoCompatibleFileInfo>(filename, filePath, sta, false,
        restorePath);
    EXPECT_EQ(fInfo1->GetRestorePath(), restorePath);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end ANCO_COMPATIBLEFILE_FILE_INFO_TEST_001";
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
 * @tc.desc: Test function of AddTarFile with maxTarSize check
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, ADD_TAR_FILE_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: ADD_TAR_FILE_TEST_001";
    GTEST_LOG_(INFO) << "1. test AddTarFile adds file to queue";
    std::string filename = "test";
    std::string filePath = "/tmp/test2";
    struct stat sta = {.st_size = 1024};
    EXPECT_FALSE(ScanFileSingleton::GetInstance().HasFileReady());
    auto& instance = ScanFileSingleton::GetInstance();
    instance.maxTarSize_.store(ONE_HUNDRED_FIFTY_MB);
    instance.stopPacket_.store(false);
    instance.AddTarFile(filename, filePath, sta);
    EXPECT_EQ(instance.pendingFileQueue_.size(), 1);
    EXPECT_TRUE(instance.HasFileReady());
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: ADD_TAR_FILE_TEST_001";
}

/**
 * @tc.number: ADD_TAR_FILE_TEST_002
 * @tc.name: ADD_TAR_FILE_TEST_002
 * @tc.desc: Test function of AddTarFile with currentTarSize exceeds maxTarSize - should stop packet
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(ScanFileSingletonTest, ADD_TAR_FILE_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: ADD_TAR_FILE_TEST_002";
    GTEST_LOG_(INFO) << "1. currentTarSize exceeds maxTarSize triggers stopPacket";
    std::string filename = "test";
    std::string filePath = "/tmp/test2";
    struct stat sta = {.st_size = 1024};
    auto& instance = ScanFileSingleton::GetInstance();
    instance.maxTarSize_.store(1000); // maxTarSize smaller than file size
    instance.currentTarSize_.store(0);
    instance.stopPacket_.store(false);
    instance.AddTarFile(filename, filePath, sta);
    EXPECT_EQ(instance.pendingFileQueue_.size(), 1);
    EXPECT_TRUE(instance.HasFileReady());
    EXPECT_TRUE(instance.stopPacket_.load());
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: ADD_TAR_FILE_TEST_002";
}

/**
 * @tc.number: GET_FILE_INFO_TEST_001
 * @tc.name: GET_FILE_INFO_TEST_001
 * @tc.desc: Test function of GetFileInfo with currentTarSize recovery
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

    GTEST_LOG_(INFO) << "2. queue not empty, get tar file and check currentTarSize recovery";
    std::string filename = "test";
    std::string filePath = "/tmp/test2";
    struct stat sta = {.st_size = 1024};
    auto& instance = ScanFileSingleton::GetInstance();
    instance.maxTarSize_.store(ONE_HUNDRED_FIFTY_MB);
    instance.currentTarSize_.store(ONE_HUNDRED_FIFTY_MB + 2048);
    instance.stopPacket_.store(true);
    instance.AddTarFile(filename, filePath, sta);
    auto fileInfo = instance.GetFileInfo();
    ASSERT_NE(fileInfo, nullptr);
    EXPECT_EQ(fileInfo->GetRestorePath(), "");
    EXPECT_TRUE(instance.stopPacket_.load());
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
* @tc.number: ADD_ANCO_BIGFILE_TEST_001
* @tc.name: ADD_ANCO_BIGFILE_TEST_001
* @tc.desc: Test function of AddAncoBigFile
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: NA
*/
HWTEST_F(ScanFileSingletonTest, ADD_ANCO_BIG_FILE_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: ADD_ANCO_BIG_FILE_TEST_001";
    std::string filePath = "/test/path/to/anco_file";
    std::string restorePath = "/restore/path/to/anco_file";
    struct stat sta = {};

    // 调用被测函数
    ScanFileSingleton::GetInstance().AddAncoBigFile(filePath, restorePath, sta);

    auto fileInfo = ScanFileSingleton::GetInstance().GetFileInfo();
    ASSERT_NE(fileInfo, nullptr);
    EXPECT_EQ(fileInfo->GetRestorePath(), "/restore/path/to/anco_file");

    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: ADD_ANCO_BIG_FILE_TEST_001";
}

/**
* @tc.number: ADD_ANCO_TARFILE_TEST_001
* @tc.name: ADD_ANCO_TARFILE_TEST_001
* @tc.desc: Test function of AddAncoTarFile
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: NA
*/
HWTEST_F(ScanFileSingletonTest, ADD_ANCO_TARFILE_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: ADD_ANCO_TARFILE_TEST_001";
    // 准备测试数据
    std::string filename = "test.tar";
    std::string filePath = "/data/test/test.tar";
    struct stat sta = {};
    sta.st_size = 1024 * 1024;

    // 执行测试操作
    ScanFileSingleton::GetInstance().AddAncoTarFile(filename, filePath, sta);

    // 验证结果
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 1);
    auto frontItem = ScanFileSingleton::GetInstance().pendingFileQueue_.front();
    EXPECT_NE(frontItem, nullptr);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: ADD_ANCO_TARFILE_TEST_001";
}

/**
* @tc.number: ADD_ANCO_BIGFILE_TEST_002
* @tc.name: ADD_ANCO_BIGFILE_TEST_002
* @tc.desc: Test function of AddAncoBigFile
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: NA
*/
HWTEST_F(ScanFileSingletonTest, ADD_ANCO_BIGFILE_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: ADD_ANCO_BIGFILE_TEST_002";
    std::string filePath = "/test/path/to/anco_file";
    std::string restorePath = "";
    struct stat sta = {};

    ScanFileSingleton::GetInstance().AddAncoBigFile(filePath, restorePath, sta);

    auto fileInfo = ScanFileSingleton::GetInstance().GetFileInfo();
    ASSERT_NE(fileInfo, nullptr);
    EXPECT_EQ(fileInfo->GetRestorePath(), "");

    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: ADD_ANCO_BIGFILE_TEST_002";
}

HWTEST_F(ScanFileSingletonTest, ADD_ANCO_TARFILE_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: ADD_ANCO_TARFILE_TEST_002";
    std::string filename = "test.tar";
    std::string filePath = "/data/test/test.tar";
    struct stat sta = {};
    sta.st_size = 1024 * 1024;

    ScanFileSingleton& instance = ScanFileSingleton::GetInstance();
    instance.currentTarSize_.store(0);
    instance.maxTarSize_.store(ONE_HUNDRED_FIFTY_MB);

    ScanFileSingleton::GetInstance().AddAncoTarFile(filename, filePath, sta);

    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 1);
    auto frontItem = ScanFileSingleton::GetInstance().pendingFileQueue_.front();
    EXPECT_NE(frontItem, nullptr);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: ADD_ANCO_TARFILE_TEST_002";
}

/**
 * @tc.number: MAX_TAR_SIZE_CHECK_INIT_001
 * @tc.name: MAX_TAR_SIZE_CHECK_INIT_001
 * @tc.desc: 测试GetInstance时初始化maxTarSize_（<4G场景）
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, MAX_TAR_SIZE_CHECK_INIT_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: MAX_TAR_SIZE_CHECK_INIT_001";
    auto& instance = ScanFileSingleton::GetInstance();
    // maxTarSize_应在GetInstance时根据剩余空间初始化
    // 由于GetInstance是单例，这里验证maxTarSize_已被设置且大于0
    EXPECT_GT(instance.maxTarSize_.load(), 0);
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: MAX_TAR_SIZE_CHECK_INIT_001";
}

/**
 * @tc.number: CURRENT_TAR_SIZE_EXCEED_001
 * @tc.name: CURRENT_TAR_SIZE_EXCEED_001
 * @tc.desc: 测试currentTarSize超过maxTarSize时停止打包
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, CURRENT_TAR_SIZE_EXCEED_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: CURRENT_TAR_SIZE_EXCEED_001";
    auto& instance = ScanFileSingleton::GetInstance();
    instance.maxTarSize_.store(1000); // 设置较小的maxTarSize_
    instance.currentTarSize_.store(0);
    instance.stopPacket_.store(false);

    std::string filename = "test.tar";
    std::string filePath = "/tmp/test.tar";
    struct stat sta = {.st_size = 2048}; // 文件大小超过maxTarSize_

    instance.AddTarFile(filename, filePath, sta);

    EXPECT_TRUE(instance.stopPacket_.load());
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: CURRENT_TAR_SIZE_EXCEED_001";
}

/**
 * @tc.number: CURRENT_TAR_SIZE_RECOVER_001
 * @tc.name: CURRENT_TAR_SIZE_RECOVER_001
 * @tc.desc: 测试获取文件后currentTarSize低于maxTarSize，重启打包
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, CURRENT_TAR_SIZE_RECOVER_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: CURRENT_TAR_SIZE_RECOVER_001";
    auto& instance = ScanFileSingleton::GetInstance();
    instance.maxTarSize_.store(ONE_HUNDRED_FIFTY_MB);
    instance.currentTarSize_.store(ONE_HUNDRED_FIFTY_MB + 2048);
    instance.stopPacket_.store(true);

    std::string filename = "test.tar";
    std::string filePath = "/tmp/test.tar";
    struct stat sta = {.st_size = 1024};

    instance.AddTarFile(filename, filePath, sta);

    auto fileInfo = instance.GetFileInfo();
    ASSERT_NE(fileInfo, nullptr);

    EXPECT_TRUE(instance.stopPacket_.load());
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: CURRENT_TAR_SIZE_RECOVER_001";
}

/**
 * @tc.number: CURRENT_TAR_SIZE_MULTIPLE_FILES_001
 * @tc.name: CURRENT_TAR_SIZE_MULTIPLE_FILES_001
 * @tc.desc: 测试添加多个文件时currentTarSize累加，取出后递减
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, CURRENT_TAR_SIZE_MULTIPLE_FILES_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScanFileSingletonTest-begin: CURRENT_TAR_SIZE_MULTIPLE_FILES_001";
    auto& instance = ScanFileSingleton::GetInstance();
    instance.maxTarSize_.store(ONE_HUNDRED_FIFTY_MB);
    instance.currentTarSize_.store(0);
    instance.stopPacket_.store(false);

    struct stat sta1 = {.st_size = 1024};
    struct stat sta2 = {.st_size = 2048};

    instance.AddTarFile("test1.tar", "/tmp/test1.tar", sta1);
    uint64_t currentAfterFirst = instance.currentTarSize_.load();

    instance.AddTarFile("test2.tar", "/tmp/test2.tar", sta2);
    uint64_t currentAfterSecond = instance.currentTarSize_.load();

    EXPECT_EQ(currentAfterFirst, 1024);
    EXPECT_EQ(currentAfterSecond, 1024 + 2048);

    // 取出文件，currentTarSize递减
    auto fileInfo1 = instance.GetFileInfo();
    auto fileInfo2 = instance.GetFileInfo();
    EXPECT_EQ(instance.currentTarSize_.load(), 0);

    GTEST_LOG_(INFO) << "ScanFileSingletonTest-end: CURRENT_TAR_SIZE_MULTIPLE_FILES_001";
}

} // namespace OHOS::FileManagement::Backup