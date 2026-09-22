/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include <unistd.h>

#include <errors.h>
#include <file_ex.h>
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <memory>

#include "b_filesystem/b_dir.h"
#include "b_dir.cpp"
#include "b_process/b_process.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BDirTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_b_dir_GetDirFiles_0100
 * @tc.name: b_dir_GetDirFiles_0100
 * @tc.desc: Test function of GetDirFiles interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_GetDirFiles_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetDirFiles_0100";
    try {
        TestManager tm("b_dir_GetDirFiles_0100");

        string preparedDir = tm.GetRootDirCurTest();
        string touchFilePrefix = string("touch ") + preparedDir;
        system(touchFilePrefix.append("a.txt").c_str());
        system(touchFilePrefix.append("b.txt").c_str());
        system(touchFilePrefix.append("c.txt").c_str());

        vector<string> out;
        bool bSucc;
        tie(bSucc, out) = BDir::GetDirFiles(preparedDir);

        vector<string> expectedRes = {preparedDir.append("a.txt"), preparedDir.append("b.txt"),
                                      preparedDir.append("c.txt")};
        EXPECT_EQ(out, expectedRes);

        tie(bSucc, out) = BDir::GetDirFiles("dev");
        EXPECT_EQ(bSucc, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetDirFiles_0100";
}

/**
 * @tc.number: SUB_backup_b_dir_GetDirFiles_0104
 * @tc.name: b_dir_GetDirFiles_0104
 * @tc.desc: Test function of GetDirFiles interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_GetDirFiles_0104, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetDirFiles_0104";
    try {
        TestManager tm("b_dir_GetDirFiles_0104");

        string preparedDir = "/data/app/";
        string touchFilePrefix = string("touch ") + preparedDir;
        system(touchFilePrefix.append("d.txt").c_str());
        system(touchFilePrefix.append("e.txt").c_str());
        system(touchFilePrefix.append("f.txt").c_str());

        bool bSucc;
        vector<string> out;
        tie(bSucc, out) = BDir::GetDirFiles(preparedDir);

        vector<string> expectedRes = {preparedDir.append("d.txt"), preparedDir.append("e.txt"),
                                      preparedDir.append("f.txt")};
        EXPECT_EQ(out, expectedRes);

        tie(bSucc, out) = BDir::GetDirFiles("dev");
        EXPECT_EQ(bSucc, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetDirFiles_0104";
}

/**
 * @tc.number: SUB_backup_b_dir_IsFilePathValid_0100
 * @tc.name: b_dir_IsFilePathValid_0100
 * @tc.desc: Test function of IsFilePathValid interface for SUCCESS
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_IsFilePathValid_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_IsFilePathValid_0100";
    try {
        TestManager tm("b_dir_IsFilePathValid_0100");
        bool result = BDir::IsFilePathValid("../test../test1");
        EXPECT_FALSE(result);
        result = BDir::IsFilePathValid("/../test../test1");
        EXPECT_FALSE(result);
        result = BDir::IsFilePathValid("test../../test");
        EXPECT_FALSE(result);
        result = BDir::IsFilePathValid("test../../");
        EXPECT_FALSE(result);
        result = BDir::IsFilePathValid("test../test../..");
        EXPECT_FALSE(result);
        result = BDir::IsFilePathValid("/test/..test/..");
        EXPECT_FALSE(result);

        result = BDir::IsFilePathValid("test");
        EXPECT_TRUE(result);
        result = BDir::IsFilePathValid("/test/test../test");
        EXPECT_TRUE(result);
        result = BDir::IsFilePathValid("/test../test../test");
        EXPECT_TRUE(result);
        result = BDir::IsFilePathValid("/test../test../test../");
        EXPECT_TRUE(result);
        result = BDir::IsFilePathValid("/test../test../test../..test");
        EXPECT_TRUE(result);
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_IsFilePathValid_0100";
}

/**
 * @tc.number: SUB_backup_b_dir_ExpandPathWildcard_0100
 * @tc.name: b_dir_ExpandPathWildcard_0100
 * @tc.desc: Test function of ExpandPathWildcard interface for SUCCESS
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_ExpandPathWildcard_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_ExpandPathWildcard_0100";
    try {
        TestManager tmCurrentUser("", MakeDirType::CURRENTUSER);
        std::string dirCurrentUser = tmCurrentUser.GetRootDirCurTest();
        std::string cmdMkdirPre = std::string("mkdir -p ") + dirCurrentUser;
        std::string dirAppData = "appdata/test";
        std::string dirHaps = "haps/test";
        std::string cmdMkdir = cmdMkdirPre + dirAppData;
        system(cmdMkdir.c_str());
        cmdMkdir = cmdMkdirPre + dirHaps;
        system(cmdMkdir.c_str());
        std::string cmdTouchFile = std::string("touch ") + dirCurrentUser + dirAppData + FILE_SEPARATOR_CHAR + "1.txt";
        system(cmdTouchFile.c_str());
        cmdTouchFile = string("touch ") + dirCurrentUser + "2.txt";
        system(cmdTouchFile.c_str());

        std::vector<std::string> include = { dirCurrentUser };
        std::set<std::string> res = BDir::ExpandPathWildcard(include, true);
        EXPECT_EQ(res.count(dirCurrentUser), 0);
        EXPECT_TRUE(res.count(dirCurrentUser + "haps/") > 0);
        EXPECT_TRUE(res.count(dirCurrentUser + "2.txt") > 0);

        std::string testDir = dirCurrentUser + "appdata";
        include = { testDir };
        res = BDir::ExpandPathWildcard(include, true);
        EXPECT_EQ(res.size(), 0);

        testDir = dirCurrentUser + "*.txt";
        include = { testDir };

        res = BDir::ExpandPathWildcard(include, true);
        EXPECT_EQ(res.size(), 1); // 1: dirCurrentUser + "2.txt"
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an ExpandPathWildcard_0100 exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_ExpandPathWildcard_0100";
}

/**
 * @tc.number: SUB_backup_b_dir_ExpandPathWildcard_0200
 * @tc.name: b_dir_ExpandPathWildcard_0200
 * @tc.desc: Test function of ExpandPathWildcard interface for SUCCESS
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_ExpandPathWildcard_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_ExpandPathWildcard_0200";
    try {
        TestManager tmDefault("b_dir_ExpandPathWildcard_0200");
        std::string dirDefault = tmDefault.GetRootDirCurTest();
        std::string cmdMkdirPre = std::string("mkdir -p ") + dirDefault;
        std::string dirAppData = "appdata/test";
        std::string dirHaps = "haps/test";
        std::string cmdMkdir = cmdMkdirPre + dirAppData;
        system(cmdMkdir.c_str());
        cmdMkdir = cmdMkdirPre + dirHaps;
        system(cmdMkdir.c_str());
        std::string cmdTouchFile = std::string("touch ") + dirDefault + dirAppData + FILE_SEPARATOR_CHAR + "1.txt";
        system(cmdTouchFile.c_str());
        cmdTouchFile = string("touch ") + dirDefault + "2.txt";
        system(cmdTouchFile.c_str());

        std::vector<std::string> include = { dirDefault };
        std::set<std::string> res = BDir::ExpandPathWildcard(include, true);
        EXPECT_EQ(res.size(), 1); // 1: dirDefault

        std::string testDir = dirDefault + "*.txt";
        include = { testDir };
        res = BDir::ExpandPathWildcard(include, true);
        EXPECT_EQ(res.size(), 1); // 1: dirCurrentUser + "2.txt"
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an ExpandPathWildcard_0200 exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_ExpandPathWildcard_0200";
}

/**
 * @tc.number: SUB_backup_b_dir_RmForceExcludePath_0100
 * @tc.name: b_dir_RmForceExcludePath_0100
 * @tc.desc: Test function of RmForceExcludePath interface for SUCCESS
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_RmForceExcludePath_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_RmForceExcludePath_0100";
    try {
        TestManager tmCurrentUser("", MakeDirType::CURRENTUSER);
        std::string dirCurrentUser = tmCurrentUser.GetRootDirCurTest();
        std::string cmdMkdirPre = std::string("mkdir -p ") + dirCurrentUser;
        std::string dirAppData = "appdata/test";
        std::string dirHaps = "haps/test";
        std::string cmdMkdir = cmdMkdirPre + dirAppData;
        system(cmdMkdir.c_str());
        cmdMkdir = cmdMkdirPre + dirHaps;
        system(cmdMkdir.c_str());
        std::string cmdTouchFile = std::string("touch ") + dirCurrentUser + dirAppData + FILE_SEPARATOR_CHAR + "1.txt";
        system(cmdTouchFile.c_str());
        cmdTouchFile = string("touch ") + dirCurrentUser + "2.txt";
        system(cmdTouchFile.c_str());

        std::set<std::string> testPath = {
            dirCurrentUser
        };
        RmForceExcludePath(testPath);
        EXPECT_EQ(testPath.size(), 2); // 2: valid path number

        testPath = {
            dirCurrentUser + "appdata/"
        };
        RmForceExcludePath(testPath);
        EXPECT_EQ(testPath.size(), 0);

        testPath = {
            dirCurrentUser + "haps"
        };
        RmForceExcludePath(testPath);
        EXPECT_EQ(testPath.size(), 1); // 1: dirCurrentUser + "haps"
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an RmForceExcludePath exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_RmForceExcludePath_0100";
}

/**
 * @tc.number: SUB_backup_b_dir_GetSubDir_0100
 * @tc.name: b_dir_GetSubDir_0100
 * @tc.desc: Test function of GetSubDir interface for SUCCESS
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_GetSubDir_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetSubDir_0100";
    try {
        TestManager tmCurrentUser("", MakeDirType::CURRENTUSER);
        std::string dirCurrentUser = tmCurrentUser.GetRootDirCurTest();
        std::string cmdMkdirPre = std::string("mkdir -p ") + dirCurrentUser;
        std::string dirAppData = "appdata/test";
        std::string dirHaps = "haps/test";
        std::string cmdMkdir = cmdMkdirPre + dirAppData;
        system(cmdMkdir.c_str());
        cmdMkdir = cmdMkdirPre + dirHaps;
        system(cmdMkdir.c_str());
        std::string cmdTouchFile = std::string("touch ") + dirCurrentUser + dirAppData + FILE_SEPARATOR_CHAR + "1.txt";
        system(cmdTouchFile.c_str());
        cmdTouchFile = string("touch ") + dirCurrentUser + "2.txt";
        system(cmdTouchFile.c_str());

        std::set<std::string> result = GetSubDir("");
        EXPECT_EQ(result.size(), 0);

        result = GetSubDir("test");
        EXPECT_EQ(result.size(), 0);

        result = GetSubDir(dirCurrentUser);
        EXPECT_EQ(result.size(), 2); // 2: valid path number
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an GetSubDir exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetSubDir_0100";
}

/**
 * @tc.number: SUB_backup_b_dir_PreDealExcludes_0100
 * @tc.name: b_dir_PreDealExcludes_0100
 * @tc.desc: Test function of PreDealExcludes interface for SUCCESS
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir__PreDealExcludes_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_PreDealExcludes_0100";
    try {
        std::string firstEle = "test";
        std::string secEle = "";
        std::string thirdEle = "test/test1";
        std::string fourthEle = "/test/test1";
        std::string fifthEle = "/test/test1/";
        std::vector<std::string> excludes = {
            firstEle,
            secEle,
            thirdEle,
            fourthEle,
            fifthEle
        };
        BDir::PreDealExcludes(excludes);
        EXPECT_EQ(excludes.size(), 4); // 4: the size of excludes after preDeal
        EXPECT_EQ(excludes[0], firstEle); // 0: first idx
        EXPECT_EQ(excludes[1], fourthEle); // 1: second idx
        EXPECT_EQ(excludes[2], fourthEle); // 2: third idx
        EXPECT_EQ(excludes[3], fifthEle + "*"); // 3: firth idx
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an PreDealExcludes exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_PreDealExcludes_0100";
}

/**
* @tc.number: b_dir_IsNotPath_0100
* @tc.name: b_dir_IsNotPath_0100
* @tc.desc: Test function of IsNotPath interface for SUCCESS
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(BDirTest, b_dir_IsNotPath_0100, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_IsNotPath_0100";
    try {
        std::string longPath(BConstants::MAX_PATH_LEN + 1, 'a');
        std::vector<std::string> bigFiles, smallFiles;
        auto result = IsNotPath(longPath, bigFiles, smallFiles, 1024);
        
        EXPECT_TRUE(std::get<0>(result).empty());
        EXPECT_TRUE(std::get<1>(result).empty());
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an IsNotPath exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_IsNotPath_0100";
}

/**
* @tc.number: b_dir_IsNotPath_0200
* @tc.name: b_dir_IsNotPath_0200
* @tc.desc: Test function of IsNotPath interface for SUCCESS
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(BDirTest, b_dir_IsNotPath_0200, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_IsNotPath_0200";
    try {
        std::string invalidPath = "";
        std::vector<std::string> bigFiles, smallFiles;
        auto result = IsNotPath(invalidPath, bigFiles, smallFiles, 1024);
        
        EXPECT_TRUE(std::get<0>(result).empty());
        EXPECT_TRUE(std::get<1>(result).empty());
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an IsNotPath exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_IsNotPath_0200";
}

/**
* @tc.number: b_dir_IsNotPath_0300
* @tc.name: b_dir_IsNotPath_0300
* @tc.desc: Test function of IsNotPath interface for SUCCESS
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(BDirTest, b_dir_IsNotPath_0300, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_IsNotPath_0300";
    try {
        std::string longPath(BConstants::MAX_PATH_LEN - 1, 'a');
        std::vector<std::string> bigFiles, smallFiles;
        auto result = IsNotPath(longPath, bigFiles, smallFiles, 1024);
        
        EXPECT_TRUE(std::get<0>(result).empty());
        EXPECT_TRUE(std::get<1>(result).empty());
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an IsNotPath exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_IsNotPath_0300";
}

/**
* @tc.number: b_dir_GetUser0DirFilesDetail_0200
* @tc.name: b_dir_GetUser0DirFilesDetail_0200
* @tc.desc: Test function of GetUser0DirFilesDetail interface for SUCCESS
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(BDirTest, b_dir_GetUser0DirFilesDetail_0200, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetUser0DirFilesDetail_0200";
    try {
        std::string invalidPath = "";
        auto result = GetUser0DirFilesDetail(invalidPath);
        
        EXPECT_TRUE(std::get<0>(result).empty());
        EXPECT_TRUE(std::get<1>(result).empty());
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an GetUser0DirFilesDetail exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetUser0DirFilesDetail_0200";
}

/**
* @tc.number: b_dir_GetUser0DirFilesDetail_0300
* @tc.name: b_dir_GetUser0DirFilesDetail_0300
* @tc.desc: Test function of GetUser0DirFilesDetail interface for SUCCESS
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(BDirTest, b_dir_GetUser0DirFilesDetail_0300, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetUser0DirFilesDetail_0300";
    try {
        std::string longPath(BConstants::MAX_PATH_LEN - 1, 'a');
        auto result = GetUser0DirFilesDetail(longPath);
        
        EXPECT_TRUE(std::get<0>(result).empty());
        EXPECT_TRUE(std::get<1>(result).empty());
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an GetUser0DirFilesDetail exception occurred.";
        EXPECT_TRUE(true);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetUser0DirFilesDetail_0300";
}

/**
* @tc.number: PROCESS_FILE_TEST_001
* @tc.name: PROCESS_FILE_TEST_001
* @tc.desc: Test function of ProcessInfo interface for SUCCESS
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(BDirTest, PROCESS_FILE_TEST_001, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "BDirTest-begin: PROCESS_FILE_TEST_001";

    try {
        ProcessInfo info("", "path/to/backup", 1024);
        info.restorePath_ = "";
        info.backupPath_ = std::string(BConstants::MAX_PATH_LEN + 1, 'a');
        info.sizeBoundary_ = 1024;

        int64_t bigFileSize = 0;
        int64_t smallFileSize = 0;
        std::vector<std::string> excludes;

        ProcessFile(info, bigFileSize, smallFileSize, excludes);

        EXPECT_EQ(bigFileSize, 0);
        EXPECT_EQ(smallFileSize, 0);
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an ProcessFile exception occurred.";
        EXPECT_TRUE(false);
    }

    GTEST_LOG_(INFO) << "BDirTest-end: PROCESS_FILE_TEST_001";
}

/**
 * @tc.number: SUB_backup_b_dir_ClearDirectory_0100
 * @tc.name: b_dir_ClearDirectory_0100
 * @tc.desc: Test function of ClearDirectory interface for Invalid Path
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_ClearDirectory_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_ClearDirectory_0100";
    try {
        std::string invalidPath = "../test/../test1";
        BDir::ClearDirectory(invalidPath);
        EXPECT_TRUE(true);
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_ClearDirectory_0100";
}

/**
 * @tc.number: SUB_backup_b_dir_ClearDirectory_0200
 * @tc.name: b_dir_ClearDirectory_0200
 * @tc.desc: Test function of ClearDirectory interface for Non-existent Dir
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_ClearDirectory_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_ClearDirectory_0200";
    try {
        TestManager tm("b_dir_ClearDirectory_0200");
        std::string nonExistentPath = tm.GetRootDirCurTest() + "non_existent_dir";
        BDir::ClearDirectory(nonExistentPath);
        EXPECT_TRUE(true);
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_ClearDirectory_0200";
}

/**
 * @tc.number: SUB_backup_b_dir_ClearDirectory_0300
 * @tc.name: b_dir_ClearDirectory_0300
 * @tc.desc: Test function of ClearDirectory interface for Success
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_ClearDirectory_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_ClearDirectory_0300";
    try {
        TestManager tm("b_dir_ClearDirectory_0300");
        std::string targetDir = tm.GetRootDirCurTest();
        
        mkdir((targetDir + "sub").c_str(), 0755);
        int fd = open((targetDir + "file.txt").c_str(), O_CREAT | O_RDWR, 0644);
        close(fd);

        auto subs = GetSubDir(targetDir);
        EXPECT_EQ(subs.size(), 2);

        BDir::ClearDirectory(targetDir);

        subs = GetSubDir(targetDir);
        EXPECT_EQ(subs.size(), 0);

        EXPECT_EQ(access(targetDir.c_str(), F_OK), 0);
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_ClearDirectory_0300";
}

/**
 * @tc.number: SUB_backup_b_dir_ClearDirectory_0400
 * @tc.name: b_dir_ClearDirectory_0400
 * @tc.desc: Test function of ClearDirectory interface for Non-Directory Path
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_ClearDirectory_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_ClearDirectory_0400";
    try {
        TestManager tm("b_dir_ClearDirectory_0400");
        std::string targetDir = tm.GetRootDirCurTest();
        std::string filePath = targetDir + "test_file.txt";

        int fd = open(filePath.c_str(), O_CREAT | O_RDWR, 0644);
        close(fd);

        BDir::ClearDirectory(filePath);
        EXPECT_TRUE(true);
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_ClearDirectory_0400";
}

/**
 * @tc.name: b_dir_InternalHelpers_Branches_001
 * @tc.desc: Cover group, permission and empty-directory helper branches.
 * @tc.type: FUNC
 */
HWTEST_F(BDirTest, b_dir_InternalHelpers_Branches_001, testing::ext::TestSize.Level1)
{
    TestManager tm(__func__);
    std::filesystem::path root = tm.GetRootDirCurTest();
    std::filesystem::path emptyDir = root / "empty";
    std::filesystem::path nonEmptyDir = root / "non_empty";
    std::filesystem::create_directories(emptyDir);
    std::filesystem::create_directories(nonEmptyDir);
    {
        std::ofstream file(nonEmptyDir / "file");
        file << "data";
    }

    EXPECT_TRUE(IsInGroup(getegid()));
    EXPECT_FALSE(CheckPermission((root / "missing").string()));
    EXPECT_TRUE(CheckPermission((nonEmptyDir / "file").string()));
    EXPECT_TRUE(IsEmptyDirectory(emptyDir.string()));
    EXPECT_FALSE(IsEmptyDirectory(nonEmptyDir.string()));
    EXPECT_FALSE(IsEmptyDirectory((root / "missing").string()));
}

/**
 * @tc.name: b_dir_BasicPublicApis_Branches_001
 * @tc.desc: Cover directory creation, listing, wildcard and matching APIs.
 * @tc.type: FUNC
 */
HWTEST_F(BDirTest, b_dir_BasicPublicApis_Branches_001, testing::ext::TestSize.Level1)
{
    TestManager tm(__func__);
    std::filesystem::path root = tm.GetRootDirCurTest();
    std::filesystem::path subDir = root / "sub";
    std::filesystem::path nestedFile = root / "created/parent/file";
    std::filesystem::create_directories(subDir);
    {
        std::ofstream file(root / "file.txt");
        file << "data";
    }

    EXPECT_TRUE(BDir::CheckAndCreateDirectory("filename"));
    EXPECT_TRUE(BDir::CheckAndCreateDirectory(nestedFile.string()));
    EXPECT_TRUE(std::filesystem::is_directory(nestedFile.parent_path()));

    auto [invalidCode, invalidFiles] = BDir::GetDirFiles((root / "missing").string());
    EXPECT_NE(invalidCode, ERR_OK);
    EXPECT_TRUE(invalidFiles.empty());
    auto [code, files] = BDir::GetDirFiles(root.string());
    EXPECT_EQ(code, ERR_OK);
    EXPECT_NE(std::find(files.begin(), files.end(), (root / "file.txt").string()), files.end());
    EXPECT_EQ(std::find(files.begin(), files.end(), subDir.string()), files.end());

    EXPECT_FALSE(BDir::IsDirsMatch({"*excluded*"}, ""));
    EXPECT_TRUE(BDir::IsDirsMatch({"*excluded*"}, "/tmp/excluded/file"));
    EXPECT_FALSE(BDir::IsDirsMatch({"*excluded*"}, "/tmp/included/file"));

    std::string wildcard = root.string() + "/*";
    auto dirs = BDir::GetDirs({wildcard});
    EXPECT_FALSE(dirs.empty());
    auto allItems = BDir::ExpandPathWildcard({wildcard}, false);
    EXPECT_FALSE(allItems.empty());
}

/**
 * @tc.name: b_dir_GetUser0DirFilesDetail_Branches_001
 * @tc.desc: Cover empty directories, regular files and recursive directory traversal.
 * @tc.type: FUNC
 */
HWTEST_F(BDirTest, b_dir_GetUser0DirFilesDetail_Branches_001, testing::ext::TestSize.Level1)
{
    TestManager tm(__func__);
    std::filesystem::path root = tm.GetRootDirCurTest();
    std::filesystem::path emptyDir = root / "empty";
    std::filesystem::path subDir = root / "sub";
    std::filesystem::path smallFile = root / "small";
    std::filesystem::path bigFile = subDir / "big";
    std::filesystem::create_directories(emptyDir);
    std::filesystem::create_directories(subDir);
    {
        std::ofstream small(smallFile);
        small << "123";
        std::ofstream big(bigFile);
        big << "1234567890";
    }

    auto [emptyBig, emptySmall] = GetUser0DirFilesDetail(emptyDir.string(), 5);
    EXPECT_TRUE(emptyBig.empty());
    ASSERT_EQ(emptySmall.size(), 1);
    EXPECT_EQ(emptySmall[0].back(), BConstants::FILE_SEPARATOR_CHAR);

    auto [smallBig, smallSmall] = GetUser0DirFilesDetail(smallFile.string(), 5);
    EXPECT_TRUE(smallBig.empty());
    ASSERT_EQ(smallSmall.size(), 1);
    auto [fileBig, fileSmall] = GetUser0DirFilesDetail(bigFile.string(), 5);
    ASSERT_EQ(fileBig.size(), 1);
    EXPECT_TRUE(fileSmall.empty());

    auto [recursiveBig, recursiveSmall] = GetUser0DirFilesDetail(root.string(), 5);
    EXPECT_EQ(recursiveBig.size(), 1);
    EXPECT_GE(recursiveSmall.size(), 2);
}

/**
 * @tc.name: b_dir_ProcessFileWithResultManager_Branches_001
 * @tc.desc: Cover small and big file paths using the result-manager branch.
 * @tc.type: FUNC
 */
HWTEST_F(BDirTest, b_dir_ProcessFileWithResultManager_Branches_001, testing::ext::TestSize.Level1)
{
    TestManager tm(__func__);
    std::filesystem::path root = tm.GetRootDirCurTest();
    std::filesystem::path smallPath = root / "small";
    std::filesystem::path bigPath = root / "big";
    {
        std::ofstream small(smallPath);
        small << "123";
        std::ofstream big(bigPath);
        big << "1234567890";
    }

    auto resultManager = std::make_shared<ScanResultManager>();
    AdvancedScanOption option(false, "", resultManager);
    int64_t bigFileSize = 0;
    int64_t smallFileSize = 0;
    ProcessFile({smallPath.string(), "/restore/small", 5}, bigFileSize, smallFileSize, {}, option);
    ProcessFile({bigPath.string(), "/restore/big", 5}, bigFileSize, smallFileSize, {}, option);
    EXPECT_EQ(smallFileSize, 3);
    EXPECT_EQ(bigFileSize, 10);
    EXPECT_EQ(resultManager->GetAllSmallFiles().size(), 1);
    EXPECT_EQ(resultManager->GetAllFiles().size(), 1);
}

/**
 * @tc.name: b_dir_ScanAndBackupList_Branches_001
 * @tc.desc: Cover real directory scanning, empty directories and excludes.
 * @tc.type: FUNC
 */
HWTEST_F(BDirTest, b_dir_ScanAndBackupList_Branches_001, testing::ext::TestSize.Level1)
{
    TestManager tm(__func__);
    std::filesystem::path root = tm.GetRootDirCurTest();
    std::filesystem::path emptyDir = root / "empty";
    std::filesystem::path subDir = root / "sub";
    std::filesystem::path smallPath = root / "small";
    std::filesystem::path bigPath = subDir / "big";
    std::filesystem::create_directories(emptyDir);
    std::filesystem::create_directories(subDir);
    {
        std::ofstream small(smallPath);
        small << "123";
        std::ofstream big(bigPath);
        big << "1234567890";
    }

    DirScanner scanner;
    auto [code, bigSize, smallSize] = scanner.ScanDir(root.string(), {}, 5);
    EXPECT_EQ(code, ERR_OK);
    EXPECT_EQ(bigSize, 10);
    EXPECT_EQ(smallSize, 3);
    auto [excludedCode, excludedBig, excludedSmall] = scanner.ScanDir(
        root.string(), {root.string()}, 5);
    EXPECT_EQ(excludedCode, ERR_OK);
    EXPECT_EQ(excludedBig, 0);
    EXPECT_EQ(excludedSmall, 0);

    auto [backupBig, backupSmall] = BDir::GetBackupList(
        {root.string() + "/*"}, {smallPath.string()});
    EXPECT_EQ(std::find(backupSmall.begin(), backupSmall.end(), smallPath.string()), backupSmall.end());
    EXPECT_TRUE(backupBig.empty());
    EXPECT_FALSE(backupSmall.empty());
}

/**
 * @tc.name: b_dir_GetUser0FileStat_Branches_001
 * @tc.desc: Cover directory, small-file and big-file report generation.
 * @tc.type: FUNC
 */
HWTEST_F(BDirTest, b_dir_GetUser0FileStat_Branches_001, testing::ext::TestSize.Level1)
{
    TestManager tm(__func__);
    std::filesystem::path root = tm.GetRootDirCurTest();
    std::filesystem::path dirPath = root / "dir";
    std::filesystem::path smallPath = root / "small";
    std::filesystem::path bigPath = root / "big";
    std::filesystem::create_directories(dirPath);
    {
        std::ofstream small(smallPath);
        small << "small-data";
        std::ofstream big(bigPath);
        big << "big-data";
    }

    std::vector<ReportFileInfo> allFiles;
    std::vector<ReportFileInfo> smallFiles;
    std::vector<ReportFileInfo> bigFiles;
    BDir::GetUser0FileStat({bigPath.string()}, {dirPath.string(), smallPath.string()},
        allFiles, smallFiles, bigFiles);
    EXPECT_EQ(allFiles.size(), 3);
    EXPECT_EQ(smallFiles.size(), 2);
    EXPECT_EQ(bigFiles.size(), 1);
    EXPECT_TRUE(smallFiles[0].isDir);
    EXPECT_FALSE(smallFiles[1].hash.empty());
    EXPECT_FALSE(bigFiles[0].hash.empty());
}
} // namespace OHOS::FileManagement::Backup
