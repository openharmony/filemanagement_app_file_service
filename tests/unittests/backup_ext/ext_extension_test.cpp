/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <fstream>

#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "ext_extension.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string FILE_NAME = "1.txt";
const string BUNDLE_NAME = "com.example.app2backup/";
const string MANAGE_JSON = "manage.json";
const string PATH = "/data/storage/el2/backup/test/";
const string TAR_FILE = "1.tar";
} // namespace

class ExtExtensionTest : public testing::Test {
public:
    //所有测试用例执行之前执行
    static void SetUpTestCase(void);
    //所有测试用例执行之后执行
    static void TearDownTestCase(void);
    //每次测试用例执行之前执行
    void SetUp() {};
    //每次测试用例执行之后执行
    void TearDown() {};
};

void ExtExtensionTest::SetUpTestCase(void)
{
    //创建测试路径
    string cmdMkdir = string("mkdir -p ") + PATH + BUNDLE_NAME;
    system(cmdMkdir.c_str());
    //创建测试文件
    string touchFile = string("touch ") + PATH + BUNDLE_NAME + FILE_NAME;
    system(touchFile.c_str());
    string touchFile2 = string("touch ") + PATH + BUNDLE_NAME + "2.txt";
    system(touchFile2.c_str());
};

void ExtExtensionTest::TearDownTestCase(void)
{
    //删除测试文件夹和文件
    string rmDir = string("rm -r") + PATH + BUNDLE_NAME;
    system(rmDir.c_str());
};

/**
 * @tc.number: SUB_Ext_Extension_0100
 * @tc.name: Ext_Extension_Test_0100
 * @tc.desc: 测试路径为空返回TRUE
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0100";
    try {
        string filePath = " ";
        EXPECT_TRUE(CheckAndCreateDirectory(filePath));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0100";
}

/**
 * @tc.number: SUB_Ext_Extension_0101
 * @tc.name: Ext_Extension_Test_0101
 * @tc.desc: 测试路径非法
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0101";
    try {
        string filePath = PATH + "/tmp";
        EXPECT_TRUE(CheckAndCreateDirectory(filePath));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0101";
}

/**
 * @tc.number: SUB_Ext_Extension_0200
 * @tc.name: Ext_Extension_Test_0200
 * @tc.desc: 测试成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0200";
    try {
        string fileName = "filename";
        string reportName = GetReportFileName(fileName);
        EXPECT_EQ(reportName, "filename.rp");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0200";
}

/**
 * @tc.number: SUB_Ext_Extension_0300
 * @tc.name: Ext_Extension_Test_0300
 * @tc.desc: 测试tarfile为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0300";
    try {
        string tarFile = " ";
        vector<ExtManageInfo> extManageInfo;
        bool ret = IsUserTar(tarFile, extManageInfo);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0300";
}

/**
 * @tc.number: SUB_Ext_Extension_0301
 * @tc.name: Ext_Extension_Test_0301
 * @tc.desc: 测试tarfile不为空，extManageInfo不包含tarfile
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0301, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0301";
    try {
        string tarFile = TAR_FILE;
        vector<ExtManageInfo> extManageInfo;
        bool ret = IsUserTar(tarFile, extManageInfo);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0301";
}

/**
 * @tc.number: SUB_Ext_Extension_0302
 * @tc.name: Ext_Extension_Test_0302
 * @tc.desc: 测试tarfile不为空，extManageInfo包含tarfile且isusertar =true
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0302, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0302";
    try {
        string tarFile = TAR_FILE;
        vector<ExtManageInfo> extManageInfo;
        ExtManageInfo info;
        info.hashName = TAR_FILE;
        info.isUserTar = true;
        extManageInfo.push_back(info);
        bool ret = IsUserTar(tarFile, extManageInfo);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0302";
}

/**
 * @tc.number: SUB_Ext_Extension_0400
 * @tc.name: Ext_Extension_Test_0400
 * @tc.desc: 测试打开失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0400";
    try {
        string tarName = " ";
        auto ret = GetTarIncludes(tarName);
        size_t size = ret.size();
        EXPECT_EQ(size, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0400";
}

/**
 * @tc.number: SUB_Ext_Extension_0500
 * @tc.name: Ext_Extension_Test_0500
 * @tc.desc: 测试filepath为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0500";
    try {
        string filePath = " ";
        string fileName = FILE_NAME;
        string path = PATH;
        string hashName = "test.txt";
        bool ret = RestoreBigFilePrecheck(fileName, path, hashName, filePath);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0500";
}

/**
 * @tc.number: SUB_Ext_Extension_0501
 * @tc.name: Ext_Extension_Test_0501
 * @tc.desc: 测试filename为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0501, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0501";
    try {
        string filePath = PATH;
        string fileName = " ";
        string path = PATH;
        string hashName = "test.txt";
        bool ret = RestoreBigFilePrecheck(fileName, path, hashName, filePath);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0501";
}

/**
 * @tc.number: SUB_Ext_Extension_0502
 * @tc.name: Ext_Extension_Test_0502
 * @tc.desc: 测试filename为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0502, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0502";
    try {
        string filePath = "data/storage/el2/backup/test/";
        string fileName = PATH + BUNDLE_NAME + FILE_NAME;
        string path = PATH;
        string hashName = "test.txt";
        bool ret = RestoreBigFilePrecheck(fileName, path, hashName, filePath);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0502";
}

/**
 * @tc.number: SUB_Ext_Extension_0601
 * @tc.name: Ext_Extension_Test_0601
 * @tc.desc: 测试写入成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0601, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0601";
    try {
        string fileName = PATH + BUNDLE_NAME + "2.txt";
        int fd = open(fileName.data(), O_RDWR | O_TRUNC, S_IRWXU);
        EXPECT_GT(fd, 0);
        close(fd);
        map<string, struct ReportFileInfo> srcFiles;
        WriteFile(fileName, srcFiles);
        ifstream f(fileName);
        if (!f.is_open()) {
            throw BError(BError::Codes::EXT_INVAL_ARG, "open failed");
        }
        string line;
        getline(f, line);
        string buf1 = line;
        f.close();
        EXPECT_EQ(buf1, "version=1.0&attrNum=8");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0601";
}

/**
 * @tc.number: SUB_Ext_Extension_0602
 * @tc.name: Ext_Extension_Test_0602
 * @tc.desc: 测试写入成功 info.encodeFlag = true
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0602, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0602";
    try {
        string fileName = PATH + BUNDLE_NAME + "2.txt";
        int fd = open(fileName.data(), O_RDWR | O_TRUNC, S_IRWXU);
        EXPECT_GT(fd, 0);
        close(fd);
        map<string, struct ReportFileInfo> srcFiles;
        struct ReportFileInfo info;
        info.mode = "755";
        info.isDir = 0;
        info.size = 1024;
        info.mtime = 123456789;
        info.hash = "1234567890";
        info.userTar = 1;
        info.encodeFlag = true;
        srcFiles["/data/test.txt"] = info;
        WriteFile(fileName, srcFiles);
        ifstream f(fileName);
        if (!f.is_open()) {
            throw BError(BError::Codes::EXT_INVAL_ARG, "open failed");
        }
        f.seekg(-2, ios::end);
        string lastChar = to_string(f.get());
        f.close();
        EXPECT_EQ(lastChar, "49");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0602";
}

/**
 * @tc.number: SUB_Ext_Extension_0603
 * @tc.name: Ext_Extension_Test_0603
 * @tc.desc: 测试写入成功 info.encodeFlag = false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0603, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0603";
    try {
        string fileName = PATH + BUNDLE_NAME + FILE_NAME;
        map<string, struct ReportFileInfo> srcFiles;
        struct ReportFileInfo info;
        info.mode = "755";
        info.isDir = 0;
        info.size = 1024;
        info.mtime = 123456789;
        info.hash = "1234567890";
        info.userTar = 1;
        info.encodeFlag = false;
        srcFiles["/data/test.txt"] = info;
        WriteFile(fileName, srcFiles);
        ifstream f(fileName);
        if (!f.is_open()) {
            throw BError(BError::Codes::EXT_INVAL_ARG, "open failed");
        }
        f.seekg(-2, ios::end);
        string lastChar = to_string(f.get());
        f.close();
        EXPECT_EQ(lastChar, "48");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0603";
}

/**
 * @tc.number: SUB_Ext_Extension_0700
 * @tc.name: Ext_Extension_Test_0700
 * @tc.desc: 测试file为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0700";
    try {
        map<string, struct ReportFileInfo> files;
        TarMap result = GetIncrmentBigInfos(files);
        EXPECT_TRUE(result.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0700";
}

/**
 * @tc.number: SUB_Ext_Extension_0701
 * @tc.name: Ext_Extension_Test_0701
 * @tc.desc: 测试file只有一个元素
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0701, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0701";
    try {
        map<string, struct ReportFileInfo> files;
        files.emplace("test", (struct ReportFileInfo) {});
        TarMap result = GetIncrmentBigInfos(files);
        EXPECT_EQ(result.size(), 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0701";
}

/**
 * @tc.number: SUB_Ext_Extension_0702
 * @tc.name: Ext_Extension_Test_0702
 * @tc.desc: 测试file有重复元素
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0702, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0702";
    try {
        map<string, struct ReportFileInfo> files;
        files.emplace("test", (struct ReportFileInfo) {});
        files.emplace("test", (struct ReportFileInfo) {});
        TarMap result = GetIncrmentBigInfos(files);
        EXPECT_EQ(result.size(), 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0702";
}

/**
 * @tc.number: SUB_Ext_Extension_0703
 * @tc.name: Ext_Extension_Test_0703
 * @tc.desc: 测试file有2个元素
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0703, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0703";
    try {
        map<string, struct ReportFileInfo> files;
        files.emplace("test1", (struct ReportFileInfo) {});
        files.emplace("test2", (struct ReportFileInfo) {});
        TarMap result = GetIncrmentBigInfos(files);
        EXPECT_EQ(result.size(), 2);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0703";
}


} // namespace OHOS::FileManagement::Backup