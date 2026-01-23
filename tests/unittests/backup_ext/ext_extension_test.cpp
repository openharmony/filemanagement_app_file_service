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
#include "b_filesystem/b_file.h"
#include "b_resources/b_constants.h"
#include "ext_extension.cpp"
#include "sub_ext_extension.cpp"

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
        EXPECT_TRUE(BDir::CheckAndCreateDirectory(filePath));
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
        EXPECT_TRUE(BDir::CheckAndCreateDirectory(filePath));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0101";
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
        off_t tarFileSize = 0;
        bool ret = IsUserTar(tarFile, extManageInfo, tarFileSize);
        EXPECT_FALSE(ret);
        EXPECT_TRUE(tarFileSize == 0);
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
        off_t tarFileSize = 0;
        bool ret = IsUserTar(tarFile, extManageInfo, tarFileSize);
        EXPECT_FALSE(ret);
        EXPECT_TRUE(tarFileSize == 0);
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
        info.sta.st_size = 1; // 1: test number;
        extManageInfo.push_back(info);
        off_t tarFileSize = 0;
        bool ret = IsUserTar(tarFile, extManageInfo, tarFileSize);
        EXPECT_TRUE(ret);
        EXPECT_TRUE(tarFileSize == 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0302";
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
        vector<struct ReportFileInfo> srcFiles;
        BFile::WriteFile(fileName, srcFiles);
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
        vector<struct ReportFileInfo> srcFiles;
        struct ReportFileInfo info;
        info.mode = "755";
        info.isDir = 0;
        info.size = 1024;
        info.mtime = 123456789;
        info.hash = "1234567890";
        info.userTar = 1;
        info.encodeFlag = true;
        srcFiles.push_back(info);
        BFile::WriteFile(fileName, srcFiles);
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
        vector<struct ReportFileInfo> srcFiles;
        struct ReportFileInfo info;
        info.mode = "755";
        info.isDir = 0;
        info.size = 1024;
        info.mtime = 123456789;
        info.hash = "1234567890";
        info.userTar = 1;
        info.encodeFlag = false;
        srcFiles.push_back(info);
        BFile::WriteFile(fileName, srcFiles);
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
 * @tc.number: SUB_Ext_Extension_0800
 * @tc.name: Ext_Extension_Test_0800
 * @tc.desc: 测试 GetIndexFileRestorePath
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0800";
    try {
        std::string bundleName = BConstants::BUNDLE_MEDIAL_DATA;
        auto ret = GetIndexFileRestorePath(bundleName);
        EXPECT_EQ(ret, string(BConstants::PATH_MEDIALDATA_BACKUP_HOME)
            .append(BConstants::SA_BUNDLE_BACKUP_RESTORE)
            .append(BConstants::EXT_BACKUP_MANAGE));

        bundleName = "test";
        ret = GetIndexFileRestorePath(bundleName);
        EXPECT_EQ(ret, INDEX_FILE_RESTORE);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0800";
}

/**
 * @tc.number: SUB_Ext_Extension_0900
 * @tc.name: Ext_Extension_Test_0900
 * @tc.desc: 测试 GetRestoreTempPath
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_0900";
    try {
        std::string bundleName = BConstants::BUNDLE_MEDIAL_DATA;
        auto ret = GetRestoreTempPath(bundleName);
        EXPECT_EQ(ret, string(BConstants::PATH_MEDIALDATA_BACKUP_HOME)
            .append(BConstants::SA_BUNDLE_BACKUP_RESTORE));

        bundleName = "test";
        ret = GetRestoreTempPath(bundleName);
        EXPECT_EQ(ret, string(BConstants::PATH_BUNDLE_BACKUP_HOME)
            .append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_0900";
}

/**
 * @tc.number: SUB_Ext_Extension_1200
 * @tc.name: Ext_Extension_Test_1200
 * @tc.desc: 测试 GetFileHandleForSpecialCloneCloud
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_Test_1200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_Test_1200";
    try {
        const string fileName = "1.txt";
        string tarName = "2.tar";
        auto ret = GetIncrementalFileHandlePath(fileName, BUNDLE_NAME, tarName);
        EXPECT_NE(ret, ERR_OK);
        ret = GetIncrementalFileHandlePath(fileName, BConstants::BUNDLE_FILE_MANAGER, tarName);
        EXPECT_NE(ret, ERR_OK);
        ret = GetIncrementalFileHandlePath(fileName, BConstants::BUNDLE_MEDIAL_DATA, tarName);
        EXPECT_NE(ret, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_Test_1200";
}

/**
 * @tc.number: SUB_Ext_Extension_FDSan_1301
 * @tc.name: Ext_Extension_FDSan_ReportAppFileReady_Test_1301
 * @tc.desc: 测试 ReportAppFileReady 函数的 FDSan 集成
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_FDSan_ReportAppFileReady_Test_1301, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_FDSan_ReportAppFileReady_Test_1301";

    try {
        string testFile = PATH + "fdsan_report_test.txt";
        int fd = open(testFile.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        EXPECT_GT(fd, 0) << "Failed to create test file";

        // 模拟 ReportAppFileReady 中的 FDSan 操作
        fdsan_exchange_owner_tag(fd, 0, BConstants::FDSAN_EXT_TAG);

        // 写入测试数据
        const char* testData = "FDSan test data";
        write(fd, testData, strlen(testData));

        // 验证文件可读
        lseek(fd, 0, SEEK_SET);
        char buffer[128] = {0};
        ssize_t readBytes = read(fd, buffer, sizeof(buffer) - 1);
        EXPECT_GT(readBytes, 0);
        EXPECT_STREQ(buffer, testData);

        int ret = fdsan_close_with_tag(fd, BConstants::FDSAN_EXT_TAG);
        EXPECT_EQ(ret, 0);

        // 清理
        remove(testFile.c_str());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_FDSan_ReportAppFileReady_Test_1301";
}

/**
 * @tc.number: SUB_Ext_Extension_FDSan_1302
 * @tc.name: Ext_Extension_FDSan_IncrementalTarFileReady_Test_1302
 * @tc.desc: 测试 IncrementalTarFileReady 函数的 FDSan 集成
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_FDSan_IncrementalTarFileReady_Test_1302, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_FDSan_IncrementalTarFileReady_Test_1302";

    try {
        string tarFile = PATH + "test.tar";
        string manifestFile = PATH + "manifest.txt";

        int fd = open(tarFile.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        EXPECT_GT(fd, 0);
        fdsan_exchange_owner_tag(fd, 0, BConstants::FDSAN_EXT_TAG);
        fdsan_close_with_tag(fd, BConstants::FDSAN_EXT_TAG);

        int manifestFd = open(manifestFile.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        EXPECT_GT(manifestFd, 0);
        fdsan_exchange_owner_tag(manifestFd, 0, BConstants::FDSAN_EXT_TAG);
        fdsan_close_with_tag(manifestFd, BConstants::FDSAN_EXT_TAG);

        int fdRead = open(tarFile.data(), O_RDONLY);
        ASSERT_GE(fdRead, 0);
        fdsan_exchange_owner_tag(fdRead, 0, BConstants::FDSAN_EXT_TAG);

        int manifestFdRead = open(manifestFile.data(), O_RDONLY);
        ASSERT_GE(manifestFdRead, 0);
        fdsan_exchange_owner_tag(manifestFdRead, 0, BConstants::FDSAN_EXT_TAG);

        int ret = fdsan_close_with_tag(fdRead, BConstants::FDSAN_EXT_TAG);
        EXPECT_EQ(ret, 0);

        ret = fdsan_close_with_tag(manifestFdRead, BConstants::FDSAN_EXT_TAG);
        EXPECT_EQ(ret, 0);

        remove(tarFile.c_str());
        remove(manifestFile.c_str());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_FDSan_IncrementalTarFileReady_Test_1302";
}

/**
 * @tc.number: SUB_Ext_Extension_FDSan_1303
 * @tc.name: Ext_Extension_FDSan_HandleIncrementalBackup_Test_1303
 * @tc.desc: 测试 HandleIncrementalBackup 函数的 FDSan 集成
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionTest, Ext_Extension_FDSan_HandleIncrementalBackup_Test_1303, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionTest-begin Ext_Extension_FDSan_HandleIncrementalBackup_Test_1303";

    try {
        string incrementalFile = PATH + "incremental.txt";
        string manifestFile = PATH + "incremental_manifest.txt";

        int incrementalFd = open(incrementalFile.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        EXPECT_GT(incrementalFd, 0);

        int manifestFd = open(manifestFile.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        EXPECT_GT(manifestFd, 0);

        fdsan_exchange_owner_tag(incrementalFd, 0, BConstants::FDSAN_EXT_TAG);
        fdsan_exchange_owner_tag(manifestFd, 0, BConstants::FDSAN_EXT_TAG);

        int ret = fdsan_close_with_tag(incrementalFd, BConstants::FDSAN_EXT_TAG);
        EXPECT_EQ(ret, 0);

        ret = fdsan_close_with_tag(manifestFd, BConstants::FDSAN_EXT_TAG);
        EXPECT_EQ(ret, 0);

        remove(incrementalFile.c_str());
        remove(manifestFile.c_str());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionTest-end Ext_Extension_FDSan_HandleIncrementalBackup_Test_1303";
}
} // namespace OHOS::FileManagement::Backup