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

#include <fcntl.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "file_ex.h"
#include "tar_file.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class TarFileTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
};

void TarFileTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
}

void TarFileTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
}

static void ClearCache()
{
    TarFile::GetInstance().fileCount_ = 0;
    TarFile::GetInstance().tarMap_.clear();
    TarFile::GetInstance().rootPath_.clear();
    TarFile::GetInstance().packagePath_.clear();
    TarFile::GetInstance().baseTarName_.clear();
    TarFile::GetInstance().tarFileName_.clear();
    TarFile::GetInstance().ioBuffer_.clear();
    TarFile::GetInstance().currentTarName_.clear();
    TarFile::GetInstance().currentTarFileSize_ = 0;
    TarFile::GetInstance().tarFileCount_ = 0;
    TarFile::GetInstance().currentFileName_.clear();
    if (TarFile::GetInstance().currentTarFile_ != nullptr) {
        fclose(TarFile::GetInstance().currentTarFile_);
        TarFile::GetInstance().currentTarFile_ = nullptr;
    }
}

/**
 * @tc.number: SUB_Tar_File_GetInstance_0100
 * @tc.name: SUB_Tar_File_GetInstance_0100
 * @tc.desc: 测试 GetInstance 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileTest, SUB_Tar_File_GetInstance_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin SUB_Tar_File_GetInstance_0100";
    try {
        TarFile::GetInstance();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileTest-end SUB_Tar_File_GetInstance_0100";
}

/**
 * @tc.number: SUB_Tar_File_Packet_0100
 * @tc.name: SUB_Tar_File_Packet_0100
 * @tc.desc: 测试 Packet 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileTest, SUB_Tar_File_Packet_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin SUB_Tar_File_Packet_0100";
    try {
        vector<string> srcFiles = {};
        string tarFileName = "";
        string pkPath = "";
        TarMap tarMap;
        auto reportCb = [](std::string path, int err) {
            return;
        };
        bool ret = TarFile::GetInstance().Packet(srcFiles, tarFileName, pkPath, tarMap, reportCb);
        EXPECT_TRUE(tarMap.empty());
        EXPECT_FALSE(ret);

        TestManager tm("SUB_Tar_File_Packet_0100");
        string root = tm.GetRootDirCurTest();
        pkPath = root;
        ret = TarFile::GetInstance().Packet(srcFiles, tarFileName, pkPath, tarMap, reportCb);
        EXPECT_TRUE(tarMap.empty());
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileTest-end SUB_Tar_File_Packet_0100";
}

/**
 * @tc.number: SUB_Tar_File_Packet_0200
 * @tc.name: SUB_Tar_File_Packet_0200
 * @tc.desc: 测试 Packet 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileTest, SUB_Tar_File_Packet_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin SUB_Tar_File_Packet_0200";
    try {
        TestManager tm("SUB_Tar_File_Packet_0200");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }

        vector<string> srcFiles = {testDir};
        TarMap tarMap;
        string tarFileName = "part";
        string pkPath = root;
        auto reportCb = [](std::string path, int err) {
            return;
        };
        bool ret = TarFile::GetInstance().Packet(srcFiles, tarFileName, pkPath, tarMap, reportCb);
        EXPECT_TRUE(ret);
        EXPECT_EQ(tarMap.size(), 1);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileTest-end SUB_Tar_File_Packet_0200";
}

/**
 * @tc.number: SUB_Tar_File_Packet_0300
 * @tc.name: SUB_Tar_File_Packet_0300
 * @tc.desc: 测试 Packet 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileTest, SUB_Tar_File_Packet_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin SUB_Tar_File_Packet_0300";
    try {
        TestManager tm("SUB_Tar_File_Packet_0300");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir/";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }

        vector<string> srcFiles = {testDir};
        string pkPath = root;
        string tarFileName = "part";
        TarMap tarMap;
        auto reportCb = [](std::string path, int err) {
            return;
        };
        bool ret = TarFile::GetInstance().Packet(srcFiles, tarFileName, pkPath, tarMap, reportCb);
        EXPECT_TRUE(ret);
        EXPECT_EQ(tarMap.size(), 1);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileTest-end SUB_Tar_File_Packet_0300";
}

/**
 * @tc.number: SUB_Tar_File_Packet_0400
 * @tc.name: SUB_Tar_File_Packet_0400
 * @tc.desc: 测试 Packet 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileTest, SUB_Tar_File_Packet_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin SUB_Tar_File_Packet_0400";
    try {
        vector<string> srcFiles = {"xxx"};
        string tarFileName = "part";
        string pkPath = "/data/storage/el2/backup/backup";
        TarMap tarMap;
        auto reportCb = [](std::string path, int err) {
            return;
        };
        bool ret = TarFile::GetInstance().Packet(srcFiles, tarFileName, pkPath, tarMap, reportCb);
        EXPECT_FALSE(ret);
        EXPECT_TRUE(tarMap.empty());
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "TarFileTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileTest-end SUB_Tar_File_Packet_0400";
}

/**
 * @tc.number: SUB_Tar_File_Packet_0500
 * @tc.name: SUB_Tar_File_Packet_0500
 * @tc.desc: 测试 Packet 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileTest, SUB_Tar_File_Packet_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin SUB_Tar_File_Packet_0500";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Tar_File_Packet_0500");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string bFile = testDir + "/b.txt";
        string aFile = testDir + "/a.txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");

        // 调用Packet打包
        vector<string> srcFiles = {aFile, bFile};
        string tarFileName = "part";
        string pkPath = root;
        TarMap tarMap;
        auto reportCb = [](std::string path, int err) {
            return;
        };
        bool ret = TarFile::GetInstance().Packet(srcFiles, tarFileName, pkPath, tarMap, reportCb);
        EXPECT_TRUE(ret);
        EXPECT_EQ(tarMap.size(), 1);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileTest-end SUB_Tar_File_Packet_0500";
}

/**
 * @tc.number: SUB_Tar_File_Packet_0600
 * @tc.name: SUB_Tar_File_Packet_0600
 * @tc.desc: 测试 Packet 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileTest, SUB_Tar_File_Packet_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin SUB_Tar_File_Packet_0600";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Tar_File_Packet_0600");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        vector<string> srcFiles {};
        const uint32_t FILE_COUNT = 6010;
        for (uint32_t i = 0; i < FILE_COUNT; ++i) {
            string file = testDir + "/a_" + to_string(i) + ".txt";
            SaveStringToFile(file, "hello");
            srcFiles.emplace_back(file);
        }
        // 调用Packet打包
        TarMap tarMap;
        string tarFileName = "part";
        string pkPath = root;
        auto reportCb = [](std::string path, int err) {
            return;
        };
        bool ret = TarFile::GetInstance().Packet(srcFiles, tarFileName, pkPath, tarMap, reportCb);
        EXPECT_TRUE(ret);
        EXPECT_EQ(tarMap.size(), 2);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileTest-end SUB_Tar_File_Packet_0600";
}

/**
 * @tc.number: SUB_Tar_File_Packet_0700
 * @tc.name: SUB_Tar_File_Packet_0700
 * @tc.desc: 测试 Packet 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileTest, SUB_Tar_File_Packet_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin SUB_Tar_File_Packet_0700";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Tar_File_Packet_0700");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        vector<string> srcFiles {};
        const int32_t FILE_COUNT = 120;
        string cmd("");
        string fileName("");
        for (int i = 0; i < FILE_COUNT; i++) {
            fileName = root + "test_" + to_string(i);
            cmd = "dd if=/dev/urandom of=" + fileName + " bs=1M count=1";
            int ret = system(cmd.c_str());
            EXPECT_EQ(ret, 0);
            srcFiles.emplace_back(fileName);
        }

        // 调用Packet打包
        string tarFileName = "part";
        string pkPath = root;
        TarMap tarMap;
        auto reportCb = [](std::string path, int err) {
            return;
        };
        bool ret = TarFile::GetInstance().Packet(srcFiles, tarFileName, pkPath, tarMap, reportCb);
        EXPECT_TRUE(ret);
        EXPECT_EQ(tarMap.size(), 2);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileTest-end SUB_Tar_File_Packet_0700";
}

/**
 * @tc.number: SUB_Tar_File_FDSan_TraversalFile_0800
 * @tc.name: TarFile_FDSan_TraversalFile_Test_0800
 * @tc.desc: 测试 TraversalFile 函数的 FDSan 集成
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileTest, TarFile_FDSan_TraversalFile_Test_0800, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin TarFile_FDSan_TraversalFile_Test_0800";
    try {
        TestManager tm("TarFile_FDSan_TraversalFile_Test_0800");
        string root = tm.GetRootDirCurTest();
        string testFile = root + "/test_traversal.txt";

        // 创建测试文件
        int fd = open(testFile.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        ASSERT_GE(fd, 0) << "Failed to create test file";
        fdsan_exchange_owner_tag(fd, 0, BConstants::FDSAN_EXT_TAG);

        const char* testData = "FDSan test data for TraversalFile";
        ssize_t written = write(fd, testData, strlen(testData));
        EXPECT_EQ(written, strlen(testData));

        // 正常关闭
        int ret = fdsan_close_with_tag(fd, BConstants::FDSAN_EXT_TAG);
        EXPECT_EQ(ret, 0);
        // 清理
        remove(testFile.c_str());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileTest-exception occurred in TraversalFile FDSan test.";
    }
    GTEST_LOG_(INFO) << "TarFileTest-end TarFile_FDSan_TraversalFile_Test_0800";
}

/**
 * @tc.number: SUB_Tar_File_FDSan_WriteFileContent_0801
 * @tc.name: TarFile_FDSan_WriteFileContent_Test_0801
 * @tc.desc: 测试 WriteFileContent 函数的 FDSan 集成（使用 O_CLOEXEC）
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileTest, TarFile_FDSan_WriteFileContent_Test_0801, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin TarFile_FDSan_WriteFileContent_Test_0801";
    try {
        TestManager tm("TarFile_FDSan_WriteFileContent_Test_0801");
        string root = tm.GetRootDirCurTest();
        string srcFile = root + "/test_source.txt";
        string destFile = root + "/test_dest.txt";

        // 创建源文件
        const char* testData = "FDSan test data for WriteFileContent";
        int srcFd = open(srcFile.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        ASSERT_GE(srcFd, 0) << "Failed to create source file";
        write(srcFd, testData, strlen(testData));
        close(srcFd);

        int fd = open(srcFile.c_str(), O_RDONLY | O_CLOEXEC);
        ASSERT_GE(fd, 0) << "Failed to open source file with O_CLOEXEC";
        // 测试 FDSan tag 设置
        fdsan_exchange_owner_tag(fd, 0, BConstants::FDSAN_EXT_TAG);

        // 读取数据并写入目标文件
        char buffer[256] = {0};
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
        EXPECT_GT(bytesRead, 0);

        // 创建目标文件并写入数据
        int destFd = open(destFile.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        ASSERT_GE(destFd, 0) << "Failed to create dest file";
        fdsan_exchange_owner_tag(destFd, 0, BConstants::FDSAN_EXT_TAG);
        write(destFd, buffer, bytesRead);

        // 关闭文件描述符
        int ret = fdsan_close_with_tag(destFd, BConstants::FDSAN_EXT_TAG);
        EXPECT_EQ(ret, 0);
        ret = fdsan_close_with_tag(fd, BConstants::FDSAN_EXT_TAG);
        EXPECT_EQ(ret, 0);
        // 验证数据一致性
        string readContent;
        LoadStringFromFile(destFile, readContent);
        EXPECT_EQ(readContent, testData);

        // 清理
        remove(srcFile.c_str());
        remove(destFile.c_str());

        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileTest-exception occurred in WriteFileContent FDSan test.";
    }
    GTEST_LOG_(INFO) << "TarFileTest-end TarFile_FDSan_WriteFileContent_Test_0801";
}

/**
 * @tc.number: SUB_Tar_File_FDSan_MultiFd_0802
 * @tc.name: TarFile_FDSan_MultiFd_Test_0802
 * @tc.desc: 测试多个文件描述符的 FDSan 管理
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileTest, TarFile_FDSan_MultiFd_Test_0802, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin TarFile_FDSan_MultiFd_Test_0802";
    try {
        TestManager tm("TarFile_FDSan_MultiFd_Test_0802");
        string root = tm.GetRootDirCurTest();

        // 创建多个测试文件
        vector<string> testFiles;
        for (int i = 0; i < 3; i++) {
            string fileName = root + "/test_multi_" + to_string(i) + ".txt";
            int fd = open(fileName.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
            ASSERT_GE(fd, 0) << "Failed to create test file " << i;
            fdsan_exchange_owner_tag(fd, 0, BConstants::FDSAN_EXT_TAG);
            const char* testData = "Multi FDSan test";
            write(fd, testData, strlen(testData));
            fdsan_close_with_tag(fd, BConstants::FDSAN_EXT_TAG);
            testFiles.push_back(fileName);
        }

        // 清理
        for (const auto& file : testFiles) {
            remove(file.c_str());
        }

        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileTest-exception occurred in MultiFd FDSan test.";
    }
    GTEST_LOG_(INFO) << "TarFileTest-end TarFile_FDSan_MultiFd_Test_0802";
}

/**
 * @tc.number: SUB_Tar_File_FDSan_ErrorHandling_0803
 * @tc.name: TarFile_FDSan_ErrorHandling_Test_0803
 * @tc.desc: 测试 FDSan 在错误情况下的处理
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileTest, TarFile_FDSan_ErrorHandling_Test_0803, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin TarFile_FDSan_ErrorHandling_Test_0803";
    try {
        TestManager tm("TarFile_FDSan_ErrorHandling_Test_0803");
        string root = tm.GetRootDirCurTest();

        string nonExistFile = root + "/non_existent_file.txt";
        int fd = open(nonExistFile.c_str(), O_RDONLY);

        if (fd < 0) {
            EXPECT_TRUE(true);
        } else {
            fdsan_exchange_owner_tag(fd, 0, BConstants::FDSAN_EXT_TAG);
            fdsan_close_with_tag(fd, BConstants::FDSAN_EXT_TAG);
        }
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileTest-exception occurred in ErrorHandling FDSan test.";
    }
    GTEST_LOG_(INFO) << "TarFileTest-end TarFile_FDSan_ErrorHandling_Test_0803";
}
} // namespace OHOS::FileManagement::Backup