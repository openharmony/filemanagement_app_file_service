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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "file_ex.h"
#include "test_manager.h"
#include "untar_file.h"

#include <cstdio>
#include <fstream>

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class UntarFileTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
    tuple<string, string> TouchTestFile(int num, TestManager &tm);
};

void UntarFileTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
}

void UntarFileTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
}

static void ClearCache()
{
    UntarFile::GetInstance().rootPath_.clear();
    UntarFile::GetInstance().tarFileSize_ = 0;
    UntarFile::GetInstance().tarFileBlockCnt_ = 0;
    UntarFile::GetInstance().pos_ = 0;
    UntarFile::GetInstance().readCnt_ = 0;
    if (UntarFile::GetInstance().tarFilePtr_ != nullptr) {
        fclose(UntarFile::GetInstance().tarFilePtr_);
        UntarFile::GetInstance().tarFilePtr_ = nullptr;
    }
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

tuple<string, string> UntarFileTest::TouchTestFile(int num, TestManager &tm)
{
    string root = tm.GetRootDirCurTest();
    string testDir = root + "testdir/";
    if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
        GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
        throw BError(errno);
    }
    string aFile = "";
    string bFile = "";
    for (int i = 0; i < num; i++) {
        aFile += "test001/test002/test003/test004/test005/";
        bFile += "ab";
    }
    bFile += ".txt";
    aFile += bFile;

    string pax = " path=";
    size_t length = aFile.length() + pax.length();
    length += std::to_string(length).length();
    string str = std::to_string(length) + pax + aFile;

    double result = (static_cast<double>(length) + OTHER_HEADER) / BLOCK_SIZE;
    auto ret = static_cast<uint32_t>(std::ceil(result));
    auto curSize = ret * BLOCK_SIZE;

    std::vector<char> data(BLOCK_SIZE, '0');
    std::vector<char> data2;
    data2.reserve(curSize);
    data2.insert(data2.end(), str.begin(), str.end());
    for (size_t i = str.size(); i < curSize; ++i) {
        data2.push_back(0);
    }
    string path = testDir + "test.tar";
    std::ofstream file(path, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!file) {
        throw BError(errno);
    }
    file.write(data.data(), data.size());
    file.write(data2.data(), data2.size());
    file.close();
    return {path, aFile};
}

/**
 * @tc.number: SUB_Untar_File_GetInstance_0100
 * @tc.name: SUB_Untar_File_GetInstance_0100
 * @tc.desc: 测试 GetInstance 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_GetInstance_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_GetInstance_0100";
    try {
        UntarFile::GetInstance();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_GetInstance_0100";
}

/**
 * @tc.number: SUB_Untar_File_UnPacket_0100
 * @tc.name: SUB_Untar_File_UnPacket_0100
 * @tc.desc: 测试 UnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_UnPacket_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_UnPacket_0100";
    try {
        string tarFile("");
        string rootPath("");
        auto [ret, fileInfos, errFileInfos] = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
        EXPECT_EQ(ret, ENOENT);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_UnPacket_0100";
}

/**
 * @tc.number: SUB_Untar_File_UnPacket_0200
 * @tc.name: SUB_Untar_File_UnPacket_0200
 * @tc.desc: 测试 UnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_UnPacket_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_UnPacket_0200";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_UnPacket_0200");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir + "/a.txt";
        string bFile = testDir + "/b.txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");

        string tarFile = root + "test.tar";
        string cmd = "tar -cvf " + tarFile + " " + testDir;
        string rootPath(root);
        if (system(cmd.c_str()) != 0) {
            GTEST_LOG_(INFO) << " execute tar failure, errno :" << errno;
            throw BError(errno);
        }
        auto [ret, fileInfos, errFileInfos] = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_UnPacket_0200";
}

/**
 * @tc.number: SUB_Untar_File_UnPacket_0300
 * @tc.name: SUB_Untar_File_UnPacket_0300
 * @tc.desc: 测试 UnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_UnPacket_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_UnPacket_0300";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_UnPacket_0300");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir + "/a.txt";
        SaveStringToFile(aFile, "hello");

        string rootPath(root);
        auto [ret, fileInfos, errFileInfos] = UntarFile::GetInstance().UnPacket(aFile, rootPath);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_UnPacket_0300";
}

/**
 * @tc.number: SUB_Untar_File_UnPacket_0400
 * @tc.name: SUB_Untar_File_UnPacket_0400
 * @tc.desc: 测试 UnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_UnPacket_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_UnPacket_0400";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_UnPacket_0400");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string tarFile = root + "test.tar";
        string cmd = "tar -cvf " + tarFile + " " + testDir;
        if (system(cmd.c_str()) != 0) {
            GTEST_LOG_(INFO) << " execute tar failure, errno :" << errno;
            throw BError(errno);
        }

        string rootPath(root);
        auto [ret, fileInfos, errFileInfos] = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_UnPacket_0400";
}

/**
 * @tc.number: SUB_Untar_File_UnPacket_0500
 * @tc.name: SUB_Untar_File_UnPacket_0500
 * @tc.desc: 测试 UnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_UnPacket_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_UnPacket_0500";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_UnPacket_0500");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir/";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir;
        string bFile = testDir;
        // 循环100次，用来构造超长路径和超长文件名
        for (int i = 0; i < 100; i++) {
            aFile += "test001/test002/test003/test004/test005/";
            bFile += "ab";
        }
        aFile += "a.txt";
        bFile += ".txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");

        string tarFile = root + "/test.0.tar";
        TarMap tarMap {};
        vector<string> smallFiles;
        smallFiles.emplace_back(aFile);
        smallFiles.emplace_back(bFile);
        auto reportCb = [](std::string msg, int err) {
            return;
        };
        TarFile::GetInstance().Packet(smallFiles, "test", root, tarMap, reportCb);

        string rootPath(root);
        auto [ret, fileInfos, errFileInfos] = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_UnPacket_0500";
}

/**
 * @tc.number: SUB_Untar_File_IncrementalUnPacket_0100
 * @tc.name: SUB_Untar_File_IncrementalUnPacket_0100
 * @tc.desc: 测试 IncrementalUnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_IncrementalUnPacket_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_IncrementalUnPacket_0100";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_IncrementalUnPacket_0100");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir/";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir;
        string bFile = testDir;
        // 循环100次，用来构造超长路径和超长文件名
        for (int i = 0; i < 100; i++) {
            aFile += "test001/test002/test003/test004/test005/";
            bFile += "ab";
        }
        aFile += "a.txt";
        bFile += ".txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");

        TarMap tarMap {};
        vector<string> smallFiles;
        smallFiles.emplace_back(aFile);
        smallFiles.emplace_back(bFile);
        auto reportCb = [](std::string msg, int err) {
            return;
        };
        TarFile::GetInstance().Packet(smallFiles, "test", root, tarMap, reportCb);

        string tarFile = root + "/test.0.tar";
        string rootPath(root);
        unordered_map<string, struct ReportFileInfo> cloudFiles;
        auto [ret, fileInfos, errFileInfos] =
            UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_IncrementalUnPacket_0100";
}

/**
 * @tc.number: SUB_Untar_File_IncrementalUnPacket_0200
 * @tc.name: SUB_Untar_File_IncrementalUnPacket_0200
 * @tc.desc: 测试 IncrementalUnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_IncrementalUnPacket_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_IncrementalUnPacket_0200";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_IncrementalUnPacket_0200");
        string root = tm.GetRootDirCurTest();
        // 构造tarFile不存在
        string tarFile = root + "/empty.0.tar";
        string rootPath(root);
        unordered_map<string, struct ReportFileInfo> cloudFiles;
        auto [ret, fileInfos, errFileInfos] =
            UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(ret, 2); // 错误码2表示找不到文件或路径
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_IncrementalUnPacket_0200";
}

/**
 * @tc.number: SUB_Untar_File_IncrementalUnPacket_0300
 * @tc.name: SUB_Untar_File_IncrementalUnPacket_0300
 * @tc.desc: 测试 IncrementalUnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_IncrementalUnPacket_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_IncrementalUnPacket_0300";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_IncrementalUnPacket_0300");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir/";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = "a.txt";
        SaveStringToFile(aFile, "hello");

        TarMap tarMap {};
        vector<string> smallFiles;
        smallFiles.emplace_back(aFile);
        auto reportCb = [](std::string msg, int err) {
            return;
        };
        TarFile::GetInstance().Packet(smallFiles, "test", root, tarMap, reportCb);

        string tarFile = root + "/test.0.tar";
        string rootPath(root);
        unordered_map<string, struct ReportFileInfo> cloudFiles;
        // 构造文件标头为空
        FILE *currentTarFile = fopen(tarFile.c_str(), "wb+");
        fwrite("\0", sizeof(uint8_t), 1, currentTarFile);
        fclose(currentTarFile);
        auto [ret, fileInfos, errFileInfos] =
            UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_IncrementalUnPacket_0300";
}

/**
 * @tc.number: SUB_Untar_File_IncrementalUnPacket_0400
 * @tc.name: SUB_Untar_File_IncrementalUnPacket_0400
 * @tc.desc: 测试 IncrementalUnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_IncrementalUnPacket_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_IncrementalUnPacket_0400";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_IncrementalUnPacket_0400");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir/";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir;
        string bFile = testDir;
        // 循环100次，用来构造超长路径和超长文件名
        for (int i = 0; i < 100; i++) {
            aFile += "test001/test002/test003/test004/test005/";
            bFile += "ab";
        }
        aFile += "a.txt";
        bFile += ".txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");

        TarMap tarMap {};
        vector<string> smallFiles;
        smallFiles.emplace_back(aFile);
        smallFiles.emplace_back(bFile);
        auto reportCb = [](std::string msg, int err) {
            return;
        };
        TarFile::GetInstance().Packet(smallFiles, "test", root, tarMap, reportCb);

        string tarFile = root + "/test.0.tar";
        string rootPath(root);
        unordered_map<string, struct ReportFileInfo> cloudFiles;
        // 构造文件标头为空
        FILE *currentTarFile = fopen(tarFile.c_str(), "wb+");
        fseeko(currentTarFile, 1L, SEEK_SET);
        fwrite("\0", sizeof(uint8_t), 1, currentTarFile);
        fclose(currentTarFile);
        auto [ret, fileInfos, errFileInfos] =
            UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_IncrementalUnPacket_0400";
}


/**
 * @tc.number: SUB_Untar_File_IncrementalUnPacket_0500
 * @tc.name: SUB_Untar_File_IncrementalUnPacket_0500
 * @tc.desc: 测试 IncrementalUnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_IncrementalUnPacket_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_IncrementalUnPacket_0500";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_IncrementalUnPacket_0500");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir/";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir + "a.txt";
        string bFile = testDir + "b.txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");
        // 打 tar
        TarMap tarMap {};
        vector<string> smallFiles;
        smallFiles.emplace_back(aFile);
        smallFiles.emplace_back(bFile);
        auto reportCb = [](std::string msg, int err) {
            return;
        };
        TarFile::GetInstance().Packet(smallFiles, "test", root, tarMap, reportCb);
        // 首次恢复
        string tarFile = root + "/test.0.tar";
        string rootPath(root);
        unordered_map<string, struct ReportFileInfo> cloudFiles;
        cloudFiles.emplace(make_pair(aFile.substr(1), ReportFileInfo())); // 1: the pos of /
        cloudFiles.emplace(make_pair(bFile.substr(1), ReportFileInfo())); // 1: the pos of /
        auto result = UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(std::get<1>(result).size(), 2); // 1: First ele, 2: 2 files, aFile and bFile
        EXPECT_EQ(std::get<0>(result), 0);
        // 增量恢复，aFile 有变化
        cloudFiles.erase(aFile.substr(1)); // 1: the pos of /
        result = UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(std::get<0>(result), 0);
        EXPECT_EQ(std::get<1>(result).size(), 1); // 1: the first ele, 1: result size -> bFile
        // 恢复归一后 clone等其他场景，rp文件没数据
        cloudFiles.erase(bFile.substr(1)); // 1: the pos of /
        result = UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(std::get<0>(result), 0);
        EXPECT_EQ(std::get<1>(result).size(), 2); // 1: First ele, 2: 2 files, aFile and bFile
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_IncrementalUnPacket_0500";
}
/**
 * @tc.number: SUB_Untar_File_CheckLongName_0100
 * @tc.name: SUB_Untar_File_CheckLongName_0100
 * @tc.desc: 测试 CheckLongName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_CheckLongName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_CheckLongName_0100";
    try {
        string longName = "";
        FileStatInfo info;
        UntarFile::GetInstance().CheckLongName(longName, info);
        EXPECT_TRUE(info.longName.empty());

        longName = "test\n";
        UntarFile::GetInstance().CheckLongName(longName, info);
        EXPECT_TRUE(info.longName == "test");

        longName = "test\n";
        UntarFile::GetInstance().CheckLongName(longName, info);
        EXPECT_TRUE(info.longName == "test");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by CheckLongName.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_CheckLongName_0100";
}

/**
 * @tc.number: SUB_Untar_File_ParsePaxBlock_0100
 * @tc.name: SUB_Untar_File_ParsePaxBlock_0100
 * @tc.desc: 测试 ParsePaxBlock 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_ParsePaxBlock_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_ParsePaxBlock_0100";
    try {
        TestManager tm("SUB_Untar_File_ParsePaxBlock_0100");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "testdir/";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        std::vector<char> data(BLOCK_SIZE, '0');
        std::vector<char> data2;
        data2.reserve(BLOCK_SIZE);
        string str = "20 atime=1739170159\n";
        data2.insert(data2.end(), str.begin(), str.end());
        for (size_t i = str.size(); i < BLOCK_SIZE; ++i) {
            data2.push_back(0);
        }
        string path = testDir + "test.tar";
        std::ofstream file(path, std::ios::out | std::ios::trunc | std::ios::binary);
        if (!file) {
            throw BError(errno);
        }
        file.write(data.data(), data.size());
        file.write(data2.data(), data2.size());
        file.close();
        UntarFile::GetInstance().tarFilePtr_ = fopen(path.c_str(), "rb");
        if (UntarFile::GetInstance().tarFilePtr_ == nullptr) {
            throw BError(errno);
        }
        char buff[BLOCK_SIZE] = {0};
        auto readCnt = fread(buff, 1, BLOCK_SIZE, UntarFile::GetInstance().tarFilePtr_);
        if (readCnt < BLOCK_SIZE) {
            throw BError(errno);
        }
        auto res = UntarFile::GetInstance().ParsePaxBlock();
        fclose(UntarFile::GetInstance().tarFilePtr_);
        UntarFile::GetInstance().tarFilePtr_ =nullptr;
        EXPECT_EQ(std::get<FIRST_PARAM>(res), 0);
        EXPECT_TRUE(std::get<SECOND_PARAM>(res) == "");
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by ParsePaxBlock.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_ParsePaxBlock_0100";
}

/**
 * @tc.number: SUB_Untar_File_ParsePaxBlock_0200
 * @tc.name: SUB_Untar_File_ParsePaxBlock_0200
 * @tc.desc: 测试 ParsePaxBlock 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_ParsePaxBlock_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_ParsePaxBlock_0200";
    try {
        TestManager tm("SUB_Untar_File_ParsePaxBlock_0200");
        int num = 30;
        auto [path, fileName] = TouchTestFile(num, tm);

        UntarFile::GetInstance().tarFilePtr_ = fopen(path.c_str(), "rb");
        if (UntarFile::GetInstance().tarFilePtr_ == nullptr) {
            throw BError(errno);
        }
        char buff[BLOCK_SIZE] = {0};
        auto readCnt = fread(buff, 1, BLOCK_SIZE, UntarFile::GetInstance().tarFilePtr_);
        if (readCnt < BLOCK_SIZE) {
            throw BError(errno);
        }
        auto [res, longName] = UntarFile::GetInstance().ParsePaxBlock();
        fclose(UntarFile::GetInstance().tarFilePtr_);
        UntarFile::GetInstance().tarFilePtr_ =nullptr;
        EXPECT_EQ(res, 0);
        EXPECT_TRUE(longName == fileName);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by ParsePaxBlock.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_ParsePaxBlock_0200";
}

/**
 * @tc.number: SUB_Untar_File_ParsePaxBlock_0300
 * @tc.name: SUB_Untar_File_ParsePaxBlock_0300
 * @tc.desc: 测试 ParsePaxBlock 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_ParsePaxBlock_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_ParsePaxBlock_0300";
    try {
        TestManager tm("SUB_Untar_File_ParsePaxBlock_0300");
        int num = 1;
        auto [path, fileName] = TouchTestFile(num, tm);

        UntarFile::GetInstance().tarFilePtr_ = fopen(path.c_str(), "rb");
        if (UntarFile::GetInstance().tarFilePtr_ == nullptr) {
            throw BError(errno);
        }
        char buff[BLOCK_SIZE] = {0};
        auto readCnt = fread(buff, 1, BLOCK_SIZE, UntarFile::GetInstance().tarFilePtr_);
        if (readCnt < BLOCK_SIZE) {
            throw BError(errno);
        }
        auto [res, longName] = UntarFile::GetInstance().ParsePaxBlock();
        fclose(UntarFile::GetInstance().tarFilePtr_);
        UntarFile::GetInstance().tarFilePtr_ =nullptr;
        EXPECT_EQ(res, 0);
        EXPECT_TRUE(longName == fileName);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by ParsePaxBlock.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_ParsePaxBlock_0300";
}
} // namespace OHOS::FileManagement::Backup