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

#include "b_error/b_error.h"
#include "file_ex.h"
#include "test_manager.h"
#include "untar_file.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class UntarFileTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
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
        int ret = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
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
        if (system(cmd.c_str()) != 0) {
            GTEST_LOG_(INFO) << " execute tar failure, errno :" << errno;
            throw BError(errno);
        }

        string rootPath(root);
        int ret = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
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
        int ret = UntarFile::GetInstance().UnPacket(aFile, rootPath);
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
        int ret = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_UnPacket_0400";
}
} // namespace OHOS::FileManagement::Backup