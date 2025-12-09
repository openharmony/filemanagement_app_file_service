/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <cstdlib>

#include <dirent.h>
#include <fcntl.h>

#include <errors.h>
#include <file_ex.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

#include "b_filesystem/b_dir.h"
#include "b_process/b_process.h"
#include "test_manager.h"
#include "library_func_mock.h"

#include "library_func_define.h"
#include "b_dir.cpp"
#include "library_func_undef.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace OHOS::AppFileService;

class BDirSubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp()
    {
        std::queue<std::shared_ptr<IFileInfo>> emptyQueue;
        ScanFileSingleton::GetInstance().pendingFileQueue_.swap(emptyQueue);
        ScanFileSingleton::GetInstance().smallFiles_.clear();
    };
    void TearDown() {};
    static inline shared_ptr<LibraryFuncMock> funcMock_ = nullptr;
};

void BDirSubTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    funcMock_ = make_shared<LibraryFuncMock>();
    LibraryFuncMock::libraryFunc_ = funcMock_;
}

void BDirSubTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    LibraryFuncMock::libraryFunc_ = nullptr;
    funcMock_ = nullptr;
}

/**
 * @tc.number: B_DIR_ProcessFile_001
 * @tc.name: B_DIR_ProcessFile_001
 * @tc.desc: Test function of ProcessFile interface for FAIL
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_ProcessFile_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_ProcessFile_001";
    GTEST_LOG_(INFO) << "1. test check fail";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1));
    std::string backupPath = "test1";
    std::string restorePath = "restore1";
    int64_t smallFileSize = 0;
    int64_t bigFileSize = 0;
    ProcessFile(backupPath, restorePath, 0, smallFileSize, bigFileSize);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_ProcessFile_001";
}


/**
 * @tc.number: B_DIR_ProcessFile_002
 * @tc.name: B_DIR_ProcessFile_002
 * @tc.desc: Test function of ProcessFile interface for ADD SMALL FILE
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_ProcessFile_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_ProcessFile_002";
    GTEST_LOG_(INFO) << "2. test add smallfile";
    struct stat sta = {.st_size = 5};
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Invoke([sta](const char *pathname, struct stat *statbuf) {
        *statbuf = sta;
        GTEST_LOG_(INFO) << "sta.size =" << statbuf->st_size;
        return 0;
    }));
    std::string backupPath = "test2";
    std::string restorePath = "restore2";
    int64_t smallFileSize = 0;
    int64_t bigFileSize = 0;
    ProcessFile(backupPath, restorePath, 10, bigFileSize, smallFileSize);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 1);
    EXPECT_EQ(smallFileSize, 5);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_ProcessFile_002";
}

/**
 * @tc.number: B_DIR_ProcessFile_003
 * @tc.name: B_DIR_ProcessFile_003
 * @tc.desc: Test function of ProcessFile interface for ADD BIG FILE
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BDirSubTest, B_DIR_ProcessFile_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_ProcessFile_003";
    GTEST_LOG_(INFO) << "3. test add big file";
    struct stat sta = {.st_size = 15};
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Invoke([sta](const char *pathname, struct stat *statbuf) {
        *statbuf = sta;
        GTEST_LOG_(INFO) << "sta.size =" << statbuf->st_size;
        return 0;
    }));
    std::string backupPath = "test3";
    std::string restorePath = "restore3";
    int64_t smallFileSize = 0;
    int64_t bigFileSize = 0;
    ProcessFile(backupPath, restorePath, 10, bigFileSize, smallFileSize);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 1);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 0);
    EXPECT_EQ(bigFileSize, 15);
    GTEST_LOG_(INFO) << "BDirSubTest-end B_DIR_ProcessFile_003";
}

/**
 * @tc.number: B_DIR_ProcessSingleFile_001
 * @tc.name: B_DIR_ProcessSingleFile_001
 * @tc.desc: Test function of ProcessSingleFile
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_ProcessSingleFile_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_ProcessSingleFile_001";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1));
    std::string backupPath = "test1";
    std::string restorePath = "restore1";
    off_t sizeBoundary = 10;
    auto [ret1, bigSize1, smallSize1] = ProcessSingleFile(backupPath, restorePath, sizeBoundary);
    EXPECT_EQ(ret1, 0);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 0);

    struct stat sta = {.st_size = 15};
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Invoke([sta](const char *pathname, struct stat *statbuf) {
        *statbuf = sta;
        GTEST_LOG_(INFO) << "sta.size =" << statbuf->st_size;
        return 0;
    }));
    auto [ret2, bigSize2, smallSize2] = ProcessSingleFile(backupPath, restorePath, sizeBoundary);
    EXPECT_EQ(ret2, 0);
    EXPECT_EQ(bigSize2, 15);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 1);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_ProcessSingleFile_001";
}

/**
 * @tc.number: B_DIR_DirScanner_ScanDir_001
 * @tc.name: B_DIR_DirScanner_ScanDir_001
 * @tc.desc: Test function of DirScanner::ScanDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_DirScanner_ScanDir_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_DirScanner_ScanDir_001";
    GTEST_LOG_(INFO) << "1. is not dir";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1));
    std::string backupPath = "test1";
    off_t sizeBoundary = 10;
    vector<string> excludes;
    DirScanner scanner;
    auto [ret1, bigSize1, smallSize1] = scanner.ScanDir(backupPath, excludes, sizeBoundary);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 0);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_DirScanner_ScanDir_001";
}

/**
 * @tc.number: B_DIR_DirScanner_ScanDir_002
 * @tc.name: B_DIR_DirScanner_ScanDir_002
 * @tc.desc: Test function of DirScanner::ScanDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_DirScanner_ScanDir_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_DirScanner_ScanDir_002";
    GTEST_LOG_(INFO) << "2. match excludes";
    // EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1));
    std::string backupPath = "/tmp";
    off_t sizeBoundary = 10;
    vector<string> excludes = {"/tmp"};
    DirScanner scanner;
    auto [ret1, bigSize1, smallSize1] = scanner.ScanDir(backupPath, excludes, sizeBoundary);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 0);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_DirScanner_ScanDir_002";
}

/**
 * @tc.number: B_DIR_DirScanner_ScanDir_003
 * @tc.name: B_DIR_DirScanner_ScanDir_003
 * @tc.desc: Test function of DirScanner::ScanDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_DirScanner_ScanDir_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_DirScanner_ScanDir_003";
    GTEST_LOG_(INFO) << "3. opendir fail";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1));
    std::string backupPath = "/abc";
    off_t sizeBoundary = 10;
    vector<string> excludes;
    DirScanner scanner;
    auto [ret1, bigSize1, smallSize1] = scanner.ScanDir(backupPath, excludes, sizeBoundary);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 0);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_DirScanner_ScanDir_003";
}

/**
 * @tc.number: B_DIR_DirScanner_ScanDir_004
 * @tc.name: B_DIR_DirScanner_ScanDir_004
 * @tc.desc: Test function of DirScanner::ScanDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_DirScanner_ScanDir_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_DirScanner_ScanDir_004";
    GTEST_LOG_(INFO) << "4. empty directory";
    // EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1));
    std::string backupPath = "/tmp";
    off_t sizeBoundary = 10;
    vector<string> excludes;
    DirScanner scanner;
    auto [ret1, bigSize1, smallSize1] = scanner.ScanDir(backupPath, excludes, sizeBoundary);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_DirScanner_ScanDir_004";
}

/**
 * @tc.number: B_DIR_DirScanner_ScanDir_005
 * @tc.name: B_DIR_DirScanner_ScanDir_005
 * @tc.desc: Test function of DirScanner::ScanDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_DirScanner_ScanDir_005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_DirScanner_ScanDir_005";
    GTEST_LOG_(INFO) << "5. not empty directory";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillRepeatedly(Return(-1));
    std::string backupPath = "/bin";
    off_t sizeBoundary = 10;
    vector<string> excludes;
    DirScanner scanner;
    auto [ret1, bigSize1, smallSize1] = scanner.ScanDir(backupPath, excludes, sizeBoundary);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_DirScanner_ScanDir_005";
}

/**
 * @tc.number: B_DIR_CompatibleDirScanner_ScanDir_001
 * @tc.name: B_DIR_CompatibleDirScanner_ScanDir_001
 * @tc.desc: Test function of CompatibleDirScanner::ScanDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_CompatibleDirScanner_ScanDir_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_CompatibleDirScanner_ScanDir_001";
    GTEST_LOG_(INFO) << "1. is not dir";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1));
    std::string backupPath = "test1||||/retstore||||";
    off_t sizeBoundary = 10;
    vector<string> excludes;
    CompatibleDirScanner scanner;
    auto [ret1, bigSize1, smallSize1] = scanner.ScanDir(backupPath, excludes, sizeBoundary);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 0);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_CompatibleDirScanner_ScanDir_001";
}

/**
 * @tc.number: B_DIR_CompatibleDirScanner_ScanDir_002
 * @tc.name: B_DIR_CompatibleDirScanner_ScanDir_002
 * @tc.desc: Test function of CompatibleDirScanner::ScanDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_CompatibleDirScanner_ScanDir_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_CompatibleDirScanner_ScanDir_002";
    GTEST_LOG_(INFO) << "2. match excludes";
    // EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1));
    std::string backupPath = "/tmp||||/retstore||||";
    off_t sizeBoundary = 10;
    vector<string> excludes = {"/tmp"};
    CompatibleDirScanner scanner;
    auto [ret1, bigSize1, smallSize1] = scanner.ScanDir(backupPath, excludes, sizeBoundary);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 0);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_CompatibleDirScanner_ScanDir_002";
}

/**
 * @tc.number: B_DIR_CompatibleDirScanner_ScanDir_003
 * @tc.name: B_DIR_CompatibleDirScanner_ScanDir_003
 * @tc.desc: Test function of CompatibleDirScanner::ScanDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_CompatibleDirScanner_ScanDir_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_CompatibleDirScanner_ScanDir_003";
    GTEST_LOG_(INFO) << "3. opendir fail";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1));
    std::string backupPath = "/abc||||/retstore||||";
    off_t sizeBoundary = 10;
    vector<string> excludes;
    CompatibleDirScanner scanner;
    auto [ret1, bigSize1, smallSize1] = scanner.ScanDir(backupPath, excludes, sizeBoundary);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 0);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_CompatibleDirScanner_ScanDir_003";
}

/**
 * @tc.number: B_DIR_CompatibleDirScanner_ScanDir_004
 * @tc.name: B_DIR_CompatibleDirScanner_ScanDir_004
 * @tc.desc: Test function of CompatibleDirScanner::ScanDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_CompatibleDirScanner_ScanDir_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_CompatibleDirScanner_ScanDir_004";
    GTEST_LOG_(INFO) << "4. empty directory";
    // EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1));
    std::string backupPath = "/tmp||||/retstore||||";
    off_t sizeBoundary = 10;
    vector<string> excludes;
    CompatibleDirScanner scanner;
    auto [ret1, bigSize1, smallSize1] = scanner.ScanDir(backupPath, excludes, sizeBoundary);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_CompatibleDirScanner_ScanDir_004";
}

/**
 * @tc.number: B_DIR_CompatibleDirScanner_ScanDir_005
 * @tc.name: B_DIR_CompatibleDirScanner_ScanDir_005
 * @tc.desc: Test function of CompatibleDirScanner::ScanDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_CompatibleDirScanner_ScanDir_005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_CompatibleDirScanner_ScanDir_005";
    GTEST_LOG_(INFO) << "5. not empty directory";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillRepeatedly(Return(-1));
    std::string backupPath = "/bin||||/retstore||||";
    off_t sizeBoundary = 10;
    vector<string> excludes;
    CompatibleDirScanner scanner;
    auto [ret1, bigSize1, smallSize1] = scanner.ScanDir(backupPath, excludes, sizeBoundary);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_CompatibleDirScanner_ScanDir_005";
}

/**
 * @tc.number: B_DIR_DirScanner_ScanAllDirs_001
 * @tc.name: B_DIR_DirScanner_ScanAllDirs_001
 * @tc.desc: Test function of DirScanner::ScanAllDirs
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_DirScanner_ScanAllDirs_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_DirScanner_ScanAllDirs_001";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1)).WillOnce(Return(-1));
    set<string> includes = {"test1", "test2"};
    vector<string> excludes;
    DirScanner scanner;
    auto [ret1, bigSize1, smallSize1] = scanner.ScanAllDirs(includes, excludes);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_DirScanner_ScanAllDirs_001";
}

/**
 * @tc.number: B_DIR_ScanAllDirs_001
 * @tc.name: B_DIR_ScanAllDirs_001
 * @tc.desc: Test function of BDir::ScanAllDirs
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_ScanAllDirs_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_ScanAllDirs_001";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillRepeatedly(Return(-1));
    set<string> includes = {"test1", "test2"};
    set<string> compatIncludes = {"test1", "test2"};
    vector<string> excludes;
    auto [ret1, bigSize1, smallSize1] = BDir::ScanAllDirs(includes, compatIncludes, excludes);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_ScanAllDirs_001";
}

/**
 * @tc.number: B_DIR_ScanAllDirs_002
 * @tc.name: B_DIR_ScanAllDirs_002
 * @tc.desc: Test function of BDir::ScanAllDirs
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_ScanAllDirs_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_ScanAllDirs_002";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1)).WillOnce(Return(-1));
    set<string> includes = {"test1", "test2"};
    set<string> compatIncludes = {};
    vector<string> excludes;
    auto [ret1, bigSize1, smallSize1] = BDir::ScanAllDirs(includes, compatIncludes, excludes);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_ScanAllDirs_002";
}

/**
 * @tc.number: B_DIR_ScanAllDirs_003
 * @tc.name: B_DIR_ScanAllDirs_003
 * @tc.desc: Test function of BDir::ScanAllDirs
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_ScanAllDirs_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_ScanAllDirs_003";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1)).WillOnce(Return(-1));
    set<string> includes = {};
    set<string> compatIncludes = {"test1", "test2"};
    vector<string> excludes;
    auto [ret1, bigSize1, smallSize1] = BDir::ScanAllDirs(includes, compatIncludes, excludes);
    EXPECT_EQ(ret1, 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_ScanAllDirs_003";
}
} // namespace OHOS::FileManagement::Backup