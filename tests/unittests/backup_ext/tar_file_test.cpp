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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "b_error/b_error.h"
#include "compress_strategy_mock.h"
#include "file_ex.h"
#include "tar_file.h"
#include "test_manager.h"

#if defined(BROTLI_ENABLED) || defined(LZ4_ENABLED)
#define COMPRESS_ENABLED true
#endif

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class TarFileTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override
    {
        compressMock_ = std::make_shared<CompressMock>();
    }
    void TearDown() override {};
protected:
    std::shared_ptr<CompressMock> compressMock_ = nullptr;
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


HWTEST_F(TarFileTest, WRITE_COMPRESS_DATA_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin WRITE_COMPRESS_DATA_TEST_001";
    UniqueFile fout("/tmp/test.txt", "wb");
    GTEST_LOG_(INFO) << "Test1. compressed bigger";
    Buffer origin(10);
    Buffer compress1(20);
    bool rs = TarFile::GetInstance().WriteCompressData(compress1, origin, fout);
    EXPECT_FALSE(rs);

    GTEST_LOG_(INFO) << "Test2. compressed smaller";
    Buffer compress2(9);
    rs = TarFile::GetInstance().WriteCompressData(compress2, origin, fout);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end WRITE_COMPRESS_DATA_TEST_001";
}


HWTEST_F(TarFileTest, WRITE_DECOMPRESS_DATA_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin WRITE_DECOMPRESS_DATA_TEST_001";
    TarFile::GetInstance().compressTool_ = compressMock_;
    EXPECT_CALL(*compressMock_, DecompressBuffer(_, _)).WillOnce(Return(true))
            .WillOnce(Return(false));
    UniqueFile fout("/tmp/test.txt", "wb");
    auto decompSpan = std::chrono::duration<double, std::milli>(std::chrono::seconds(0));
    GTEST_LOG_(INFO) << "Test1. compressed equal origin";
    Buffer origin(10);
    Buffer compress1(10);
    bool rs = TarFile::GetInstance().WriteDecompressData(compress1, origin, fout, decompSpan);
    EXPECT_FALSE(rs);

    GTEST_LOG_(INFO) << "Test2. compressed smaller , decompress success";
    Buffer compress2(8);
    rs = TarFile::GetInstance().WriteDecompressData(compress2, origin, fout, decompSpan);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "Test3. compressed smaller , decompress fail";
    rs = TarFile::GetInstance().WriteDecompressData(compress2, origin, fout, decompSpan);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end WRITE_DECOMPRESS_DATA_TEST_001";
}

HWTEST_F(TarFileTest, COMPRESS_FILE_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin COMPRESS_FILE_TEST_001";
    std::string srcFile = "/tmp/in.txt";
    std::string compressFile = "/tmp/out.txt";
#ifdef COMPRESS_ENABLED
    TarFile::GetInstance().compressTool_ = compressMock_;
    EXPECT_CALL(*compressMock_, GetMaxCompressedSize(_)).WillOnce(Return(0)).WillRepeatedly(Return(10));
    EXPECT_CALL(*compressMock_, CompressBuffer(_, _)).WillOnce(Return(false)).WillRepeatedly(Return(true));
    GTEST_LOG_(INFO) << "Test1. compressed file";
    bool rs = TarFile::GetInstance().CompressFile(srcFile, compressFile);
    EXPECT_FALSE(rs);

    GTEST_LOG_(INFO) << "Test2. compressed file2";
    Buffer compress2(8);
    rs = TarFile::GetInstance().WriteDecompressData(compress2, origin, fout, decompSpan);
    EXPECT_FALSE(rs);
#else
    EXPECT_TRUE(TarFile::GetInstance().CompressFile(srcFile, compressFile));
#endif
    GTEST_LOG_(INFO) << "TarFileTest-end COMPRESS_FILE_TEST_001";
}
} // namespace OHOS::FileManagement::Backup