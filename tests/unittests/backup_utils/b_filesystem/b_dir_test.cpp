/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
 * @tc.number: SUB_backup_b_dir_GetBigFiles_0100
 * @tc.name: b_dir_GetBigFiles_0100
 * @tc.desc: 测试GetBigFiles接口是否能成功获取大文件
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_GetBigFiles_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetBigFiles_0100";
    try {
        TestManager tm("b_dir_GetBigFiles_0100");
        string rootDir = tm.GetRootDirCurTest();
        string filePath1 = rootDir + "a.txt";
        string filePath2 = rootDir + "b.txt";
        // 文件大小大于2M的文件属于大文件，因此这里创建大小为3MB文件和4MB的文件
        auto [bFatalErr, ret] =
            BProcess::ExecuteCmd({"dd", "if=/dev/urandom", ("of=" + filePath1).c_str(), "bs=1M", "count=3"});
        EXPECT_FALSE(bFatalErr);
        EXPECT_EQ(ret, 0);
        tie(bFatalErr, ret) =
            BProcess::ExecuteCmd({"dd", "if=/dev/urandom", ("of=" + filePath2).c_str(), "bs=1M", "count=4"});
        EXPECT_FALSE(bFatalErr);
        EXPECT_EQ(ret, 0);
        vector<string> includes = {rootDir};
        vector<string> excludes = {filePath2};
        auto [errCode, mpNameToStat, smallFiles] = BDir::GetBigFiles(includes, excludes);
        EXPECT_EQ(errCode, ERR_OK);
        EXPECT_EQ(mpNameToStat.at(filePath1).st_size, 1024 * 1024 * 3);
        EXPECT_EQ(mpNameToStat.find(filePath2), mpNameToStat.end());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetBigFiles_0100";
}

/**
 * @tc.number: SUB_backup_b_dir_GetBigFiles_0200
 * @tc.name: b_dir_GetBigFiles_0200
 * @tc.desc: 测试GetBigFiles接口 分支逻辑
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_GetBigFiles_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetBigFiles_0200";
    try {
        vector<string> includes = {{}, {}};
        vector<string> excludes = {{}};
        auto [errCode, mpNameToStat, smallFiles] = BDir::GetBigFiles(includes, excludes);
        EXPECT_EQ(errCode, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetBigFiles_0200";
}

/**
 * @tc.number: SUB_backup_b_dir_GetBigFiles_0201
 * @tc.name: b_dir_GetBigFiles_0201
 * @tc.desc: 测试GetBigFiles接口 分支逻辑
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_GetBigFiles_0201, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetBigFiles_0201";
    try {
        vector<string> includes = {"/data/"};
        vector<string> excludes;
        auto [errCode, mpNameToStat, smallFiles] = BDir::GetBigFiles(includes, excludes);
        EXPECT_EQ(errCode, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetBigFiles_0201";
}

/**
 * @tc.number: SUB_backup_b_dir_GetBigFiles_0202
 * @tc.name: b_dir_GetBigFiles_0202
 * @tc.desc: 测试GetBigFiles接口 分支逻辑
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_GetBigFiles_0202, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetBigFiles_0202";
    try {
        vector<string> includes = {"/data/app/"};
        vector<string> excludes;
        auto [errCode, mpNameToStat, smallFiles] = BDir::GetBigFiles(includes, excludes);
        EXPECT_EQ(errCode, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetBigFiles_0202";
}

/**
 * @tc.number: SUB_backup_b_dir_GetBigFiles_0203
 * @tc.name: b_dir_GetBigFiles_0203
 * @tc.desc: 测试GetBigFiles接口 分支逻辑
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_GetBigFiles_0203, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetBigFiles_0203";
    try {
        vector<string> includes;
        vector<string> excludes;
        const string str = "";
        auto [errCode, mpNameToStat, smallFiles] = BDir::GetBigFiles(includes, excludes);
        EXPECT_EQ(errCode, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetBigFiles_0203";
}

/**
 * @tc.number: SUB_backup_b_dir_GetBigFiles_0300
 * @tc.name: b_dir_GetBigFiles_0300
 * @tc.desc: 测试GetBigFiles接口 分支逻辑
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_GetBigFiles_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetBigFiles_0300";
    try {
        TestManager tm("b_dir_GetBigFiles_0300");
        string preparedDir = tm.GetRootDirCurTest();
        string cmdMkdir = string("mkdir -p ") + preparedDir + string("test/test1/test2");
        system(cmdMkdir.c_str());
        string touchFilePrefix = string("touch ") + preparedDir;
        system(touchFilePrefix.append("a.txt").c_str());
        system(touchFilePrefix.append("b.txt").c_str());
        system(touchFilePrefix.append("c.txt").c_str());

        touchFilePrefix = string("touch ") + preparedDir + string("test/");
        system(touchFilePrefix.append("a.txt").c_str());
        system(touchFilePrefix.append("b.txt").c_str());
        system(touchFilePrefix.append("c.txt").c_str());
        touchFilePrefix = string("touch ") + preparedDir + string("test/test1/test2");
        system(touchFilePrefix.append("a.txt").c_str());
        system(touchFilePrefix.append("b.txt").c_str());
        system(touchFilePrefix.append("c.txt").c_str());
        vector<string> includes = {preparedDir + string("/*"), preparedDir + string("test")};
        vector<string> excludes = {preparedDir + string("/test/test1/test2"), {}};
        auto [errCode, mpNameToStat, smallFiles] = BDir::GetBigFiles(includes, excludes);
        EXPECT_EQ(errCode, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetBigFiles_0300";
}

/**
 * @tc.number: SUB_backup_b_dir_GetDirs_0100
 * @tc.name: b_dir_GetDirs_0100
 * @tc.desc: Test function of GetDirs interface for SUCCESS
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_GetDirs_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetDirs_0100";
    try {
        TestManager tm("b_dir_GetDirs_0100");
        vector<string_view> paths;
        vector<string> res = BDir::GetDirs(paths);
        set<string> inc(paths.begin(), paths.end());
        bool result = equal(inc.begin(), inc.end(), res.begin(), res.end());
        EXPECT_EQ(1, result);
        EXPECT_EQ(inc.size(), res.size());
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetDirs_0100";
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
 * @tc.number: SUB_backup_b_dir_GetFile_0100
 * @tc.name: b_dir_GetFile_0100
 * @tc.desc: Test function of GetFile interface for SUCCESS
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirTest, b_dir_GetFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetFile_0100";
    try {
        string path = "/";
        auto [errCode, subFiles, subSmallFiles] = GetFile(path);
        string pathData = "/data";
        auto [errCode1, subFiles1, subSmallFiles1] = GetFile(pathData, PATH_MAX_LEN);
        auto [errCode2, subFiles2, subSmallFiles2] = GetFile(pathData);
        EXPECT_EQ(errCode, 0);
    } catch (...) {
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetFile_0100";
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
        std::set<std::string> res = ExpandPathWildcard(include, true);
        EXPECT_EQ(res.size(), 2); // 2: valid path number
        EXPECT_EQ(res.count(dirCurrentUser), 0);

        std::string testDir = dirCurrentUser + "appdata";
        include = { testDir };
        res = ExpandPathWildcard(include, true);
        EXPECT_EQ(res.size(), 0);

        testDir = dirCurrentUser + "*.txt";
        include = { testDir };

        res = ExpandPathWildcard(include, true);
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
        std::set<std::string> res = ExpandPathWildcard(include, true);
        EXPECT_EQ(res.size(), 1); // 1: dirDefault

        std::string testDir = dirDefault + "*.txt";
        include = { testDir };
        res = ExpandPathWildcard(include, true);
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
        PreDealExcludes(excludes);
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
} // namespace OHOS::FileManagement::Backup