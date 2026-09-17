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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "installd_un_tar_file.h"

#include <dirent.h>
#include <file_ex.h>
#include <filesystem>
#include <unistd.h>

#include "b_error/b_error.h"
#include "test_manager.h"

#include <sys/stat.h>

namespace installd {
uid_t FixUpOwnerDirFile(const uid_t uid, const gid_t gid, const uid_t owner, gid_t &newGid);
}

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace installd;

class InstalldUnTarFileTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
};

void InstalldUnTarFileTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest::SetUpTestCase enter";
}

void InstalldUnTarFileTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest::TearDownTestCase enter";
}

static string CreateUnSplitTar(const string &rootPath, const string &rootPathSubPath, int32_t innerFileCount)
{
    try {
        string testDir = rootPath + rootPathSubPath;
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        for (int i = 0; i < innerFileCount; ++i) {
            string file = testDir + "/" + to_string(i + 1) + ".txt";
            SaveStringToFile(file, "hello" + to_string(i));
            GTEST_LOG_(INFO) << "InstalldUnTarFileTest-an access: " << access(file.c_str(), F_OK);
        }

        string tarFile = rootPath + "testUnSplit.tar";
        string cmd = "tar -cvf " + tarFile + " " + testDir;
        if (system(cmd.c_str()) != 0) {
            GTEST_LOG_(INFO) << " execute tar failure, errno :" << errno;
            throw BError(errno);
        }
        return tarFile;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "InstalldUnTarFileTest-an exception occurred by CreateUnSplitTar.";
    }
    return "";
}

static vector<string> CreateSplitTar(const string& rootPath, const string& rootPathSubPath)
{
    try {
        const string testDir = rootPath + rootPathSubPath;
        vector<string> needCreateDir = {testDir, testDir + "/dir1", testDir + "/dir1/dir2", testDir + "/dir3",
                                        testDir + "/dir4"};
        for (const auto &dir : needCreateDir) {
            if (mkdir(dir.data(), S_IRWXU) && errno != EEXIST) {
                throw BError(errno);
            }
        }
        constexpr int32_t fileCount = 5;
        for (auto i = 0; i < needCreateDir.size(); ++i) {
            string file = needCreateDir[i] + "/" + to_string(i) + ".txt";
            string data = "";
            for (auto j = 0; j < fileCount * (i + 1); ++j) {
                data += "test data 123456789";
            }
            SaveStringToFile(file, data);
        }

        string tarDir = rootPath + "testTar";
        if (mkdir(tarDir.data(), S_IRWXU) && errno != EEXIST) {
            throw BError(errno);
        }

        string tarPath = tarDir + "/test.tar";
        string softlinkCmd = "cd " + testDir + "/dir4" + " && ln -s " + testDir + "/dir4/4.txt ./4_new.txt";
        string tarCmd = "tar -cvf " + tarPath + " " + testDir;
        string splitTarCmd = "cd " + tarDir + " && split -b 4k -a 2 test.tar test.tar.";
        vector<string> cmds = {softlinkCmd, tarCmd, splitTarCmd};

        for (const string& cmd : cmds) {
            if (system(cmd.c_str()) != 0) {
                throw BError(errno);
            }
        }
        vector<string> res;
        for (const auto& entry : std::filesystem::directory_iterator(tarDir)) {
            if (entry.path().filename() != "test.tar") {
                res.push_back(entry.path().string());
            }
        }
        return res;
    } catch (...) {
        GTEST_LOG_(INFO) << "InstalldUnTarFileTest-an exception occurred by CreateSplitTar.";
    }
    return {};
}

/**
 * @tc.number: Installd_Un_Tar_File_UnTarFile_0100
 * @tc.name: Installd_Un_Tar_File_UnTarFile_0100
 * @tc.desc: test File_UnTarFile gen
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IC15LE
 */
HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_UnTarFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-begin Installd_Un_Tar_File_UnTarFile_0100";
    try {
        char *tarPath = nullptr;
        installd::UnTarFile unTarFile(tarPath);

        const char *tarPath2 = string("/data/test/backup/test.tar").c_str();
        installd::UnTarFile unTarFile2(tarPath2);

        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "InstalldUnTarFileTest-an exception occurred by InstalldUnTarFile.";
    }
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-end Installd_Un_Tar_File_UnTarFile_0100";
}

/**
 * @tc.number: Installd_Un_Tar_File_CheckIsSplitTar_0100
 * @tc.name: Installd_Un_Tar_File_CheckIsSplitTar_0100
 * @tc.desc: test File_CheckIsSplitTar method
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IC15LE
 */
HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_CheckIsSplitTar_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-begin Installd_Un_Tar_File_CheckIsSplitTar_0100";
    try {
        const TestManager tm("Installd_Un_Tar_File_CheckIsSplitTar_0100");
        const string rootPath = tm.GetRootDirCurTest();
        constexpr int32_t innerFileCount = 10;
        const string tarPath = CreateUnSplitTar(rootPath, "testTarPath", innerFileCount);
        EXPECT_FALSE(tarPath.empty());
        installd::UnTarFile unTarFile(rootPath.c_str());
        const bool ret = unTarFile.CheckIsSplitTar(tarPath, rootPath);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "InstalldUnTarFileTest-an exception occurred by InstalldUnTarFile.";
    }
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-end Installd_Un_Tar_File_CheckIsSplitTar_0100";
}

/**
 * @tc.number: Installd_Un_Tar_File_IsEmptyBlock_0100
 * @tc.name: Installd_Un_Tar_File_IsEmptyBlock_0100
 * @tc.desc: test File_IsEmptyBlock method
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IC15LE
 */
HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_IsEmptyBlock_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-begin Installd_Un_Tar_File_IsEmptyBlock_0100";
    try {
        const char *p = "\0";
        installd::UnTarFile unTarFile(nullptr);
        bool ret = unTarFile.IsEmptyBlock(p);
        EXPECT_TRUE(ret);

        const char *p1 = "test";
        installd::UnTarFile unTarFile1(nullptr);
        ret = unTarFile.IsEmptyBlock(p1);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "InstalldUnTarFileTest-an exception occurred by InstalldUnTarFile.";
    }
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-end Installd_Un_Tar_File_IsEmptyBlock_0100";
}

/**
 * @tc.number: Installd_Un_Tar_File_CreateDirWithRecursive_0100
 * @tc.name: Installd_Un_Tar_File_CreateDirWithRecursive_0100
 * @tc.desc: test File_CreateDirWithRecursive method
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IC15LE
 */
HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_CreateDirWithRecursive_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-begin Installd_Un_Tar_File_CreateDirWithRecursive_0100";
    try {
        installd::UnTarFile unTarFile(nullptr);
        mode_t mode = 448;
        bool ret = unTarFile.CreateDirWithRecursive("", mode);
        EXPECT_FALSE(ret);

        TestManager tm("Installd_Un_Tar_File_CreateDirWithRecursive_0100");
        string root = tm.GetRootDirCurTest();
        ret = unTarFile.CreateDirWithRecursive(root, mode);
        EXPECT_TRUE(ret);

        string destPath(root + "/testDir/test1/test2");
        ret = unTarFile.CreateDirWithRecursive(destPath, mode);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "InstalldUnTarFileTest-an exception occurred by InstalldUnTarFile.";
    }
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-end Installd_Un_Tar_File_CreateDirWithRecursive_0100";
}

/**
 * @tc.number: Installd_Un_Tar_File_UnSplitTar_0100
 * @tc.name: Installd_Un_Tar_File_UnSplitTar_0100
 * @tc.desc: test File_UnSplitTar method
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IC15LE
 */
HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_UnSplitTar_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-begin Installd_Un_Tar_File_UnSplitTar_0100";
    try {
        TestManager tm("Installd_Un_Tar_File_UnSplitTar_0100");
        string rootPath = tm.GetRootDirCurTest();
        vector<string> splitTarList = CreateSplitTar(rootPath, "testTarFile");
        EXPECT_FALSE(splitTarList.empty());

        installd::UnTarFile unTarFile(nullptr);
        for (auto& tarName : splitTarList) {
            GTEST_LOG_(INFO) << "InstalldUnTarFileTest-begin tarName: " << tarName;
            auto ret = unTarFile.UnSplitTar(tarName, rootPath);
            EXPECT_EQ(ret, 0);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "InstalldUnTarFileTest-an exception occurred by InstalldUnTarFile.";
    }
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-end Installd_Un_Tar_File_UnSplitTar_0100";
}

/**
 * @tc.number: Installd_Un_Tar_File_FreePointer_0100
 * @tc.name: Installd_Un_Tar_File_FreePointer_0100
 * @tc.desc: test File_FreePointer method
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IC15LE
 */
HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_FreePointer_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-begin Installd_Un_Tar_File_FreePointer_0100";
    try {
        installd::ParseTarPath parseTarPath = {};
        parseTarPath.fullPath = (char *)malloc(sizeof(char));
        parseTarPath.longName = (char *)malloc(sizeof(char));
        parseTarPath.longLink = (char *)malloc(sizeof(char));
        installd::UnTarFile unTarFile(nullptr);
        unTarFile.FreePointer(&parseTarPath);
        EXPECT_TRUE(parseTarPath.fullPath == nullptr);
        EXPECT_TRUE(parseTarPath.longName == nullptr);
        EXPECT_TRUE(parseTarPath.longLink == nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "InstalldUnTarFileTest-an exception occurred by InstalldUnTarFile.";
    }
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-end Installd_Un_Tar_File_FreePointer_0100";
}

/**
 * @tc.number: Installd_Un_Tar_File_FreeLongTypePointer_0100
 * @tc.name: Installd_Un_Tar_File_FreeLongTypePointer_0100
 * @tc.desc: test File_FreeLongTypePointer method
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IC15LE
 */
HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_FreeLongTypePointer_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-begin Installd_Un_Tar_File_FreeLongTypePointer_0100";
    try {
        installd::ParseTarPath parseTarPath = {};
        parseTarPath.longName = (char *)malloc(sizeof(char));
        parseTarPath.longLink = (char *)malloc(sizeof(char));
        installd::UnTarFile unTarFile(nullptr);
        unTarFile.FreeLongTypePointer(&parseTarPath);
        EXPECT_TRUE(parseTarPath.longName == nullptr);
        EXPECT_TRUE(parseTarPath.longLink == nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "InstalldUnTarFileTest-an exception occurred by InstalldUnTarFile.";
    }
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-end Installd_Un_Tar_File_FreeLongTypePointer_0100";
}

/**
 * @tc.number: Installd_Un_Tar_File_ProcessTarBlock_0100
 * @tc.name: Installd_Un_Tar_File_ProcessTarBlock_0100
 * @tc.desc: test ProcessTarBlock method
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IC15LE
 */
HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_ProcessTarBlock_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-begin Installd_Un_Tar_File_ProcessTarBlock_0100";
    try {
        const TestManager tm("Installd_Un_Tar_File_ProcessTarBlock_0100");
        const string rootPath = tm.GetRootDirCurTest();
        UnTarFile unTarFile(rootPath.c_str());
        TarHeader headBuff = {};
        headBuff.size[0] = '2';
        headBuff.typeflag = DIRTYPE;
        ParseTarPath parseTarPath = {};
        char emptyPath[1] = {'\0'};
        parseTarPath.fullPath = emptyPath;
        bool isSkip = false;
        bool isSoftLink = false;
        unTarFile.ProcessTarBlock((char *)(&headBuff),
            UnTarFile::EParseType::eCheckSplit, &parseTarPath, isSkip, isSoftLink);
        EXPECT_FALSE(isSkip);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "InstalldUnTarFileTest-an exception occurred by ProcessTarBlock.";
    }
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-end Installd_Un_Tar_File_ProcessTarBlock_0100";
}

/**
 * @tc.number: Installd_Un_Tar_File_HandleSymType_0100
 * @tc.name: Installd_Un_Tar_File_HandleSymType_0100
 * @tc.desc: test HandleSymType rejects symlink with path traversal
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IC15LE
 */
HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_HandleSymType_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-begin Installd_Un_Tar_File_HandleSymType_0100";
    try {
        const TestManager tm("Installd_Un_Tar_File_HandleSymType_0100");
        const string rootPath = tm.GetRootDirCurTest();
        UnTarFile unTarFile(rootPath.c_str());
        TarHeader headBuff = {};
        headBuff.size[0] = '0';
        headBuff.typeflag = SYMTYPE;
        ParseTarPath parseTarPath = {};
        string fullPath = rootPath + "/../etc/passwd";
        string realLink = "/data/local/tmp/link";
        parseTarPath.fullPath = const_cast<char *>(fullPath.c_str());
        parseTarPath.realLink = const_cast<char *>(realLink.c_str());
        bool isSkip = false;
        bool isSoftLink = false;
        unTarFile.ProcessTarBlock((char *)(&headBuff),
            UnTarFile::EParseType::eUnpack, &parseTarPath, isSkip, isSoftLink);
        EXPECT_FALSE(isSoftLink);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "InstalldUnTarFileTest-an exception occurred by HandleSymType.";
    }
    GTEST_LOG_(INFO) << "InstalldUnTarFileTest-end Installd_Un_Tar_File_HandleSymType_0100";
}

HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_OwnerAndOctal_0200, testing::ext::TestSize.Level1)
{
    gid_t newGid = 0;
    EXPECT_EQ(FixUpOwnerDirFile(100, 200, 0, newGid), 100U);
    EXPECT_EQ(newGid, 200U);
    EXPECT_EQ(FixUpOwnerDirFile(APP_ID_START + 1, APP_ID_START + 1, 300, newGid), 300U);
    EXPECT_EQ(newGid, 300U);
    EXPECT_EQ(FixUpOwnerDirFile(APP_ID_START + 1, APP_ID_START + 1 + UID_GID_OFFSET, 400, newGid), 400U);
    EXPECT_EQ(newGid, 400U + UID_GID_OFFSET);
    EXPECT_EQ(FixUpOwnerDirFile(APP_ID_START + 1, APP_ID_START + 2, 500, newGid), 500U);
    EXPECT_EQ(newGid, APP_ID_START + 2);

    EXPECT_EQ(ParseOctalStr("  17x", 5), 15);
    EXPECT_EQ(ParseOctalStr("xyz", 3), 0);
    EXPECT_EQ(ParseOctalStr("777", 1), 7);
}

HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_HeaderValidation_0200, testing::ext::TestSize.Level1)
{
    UnTarFile unTarFile(nullptr);
    EXPECT_FALSE(unTarFile.VerifyChecksum(nullptr));
    EXPECT_FALSE(unTarFile.IsValidTarBlock(nullptr));

    TarHeader header = {};
    (void)strncpy(header.magic, TMAGIC, sizeof(header.magic));
    (void)memset(header.chksum, ' ', sizeof(header.chksum));
    unsigned int checksum = 0;
    const auto *bytes = reinterpret_cast<const unsigned char *>(&header);
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        checksum += bytes[i];
    }
    (void)snprintf(header.chksum, sizeof(header.chksum), "%06o", checksum);
    header.chksum[6] = '\0';
    header.chksum[7] = ' ';
    EXPECT_TRUE(unTarFile.VerifyChecksum(&header));
    EXPECT_TRUE(unTarFile.IsValidTarBlock(&header));
    header.magic[0] = 'x';
    EXPECT_FALSE(unTarFile.IsValidTarBlock(&header));
    header.magic[0] = TMAGIC[0];
    header.name[0] = 'x';
    EXPECT_FALSE(unTarFile.VerifyChecksum(&header));
}

HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_CheckFileAndReset_0200, testing::ext::TestSize.Level1)
{
    UnTarFile unTarFile(nullptr);
    ParseTarPath path = {};
    EXPECT_EQ(unTarFile.CheckFileAndInitPath("/data", &path), ERR_PARAM);

    unTarFile.FilePtr = tmpfile();
    ASSERT_NE(unTarFile.FilePtr, nullptr);
    EXPECT_EQ(unTarFile.CheckFileAndInitPath(nullptr, &path), ERR_NOEXIST);
    ASSERT_EQ(fwrite("x", 1, 1, unTarFile.FilePtr), 1U);
    EXPECT_EQ(unTarFile.CheckFileAndInitPath("/data", &path), ERR_FORMAT);
    unTarFile.Reset();
    EXPECT_EQ(unTarFile.FilePtr, nullptr);
    EXPECT_FALSE(unTarFile.isSplit);

    unTarFile.FilePtr = tmpfile();
    ASSERT_NE(unTarFile.FilePtr, nullptr);
    char block[BLOCK_SIZE] = {};
    ASSERT_EQ(fwrite(block, 1, sizeof(block), unTarFile.FilePtr), sizeof(block));
    EXPECT_EQ(unTarFile.CheckFileAndInitPath("/data", &path), 0);
    EXPECT_NE(path.fullPath, nullptr);
    unTarFile.FreePointer(&path);
}

HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_LongNameAndLink_0200, testing::ext::TestSize.Level1)
{
    UnTarFile unTarFile(nullptr);
    unTarFile.FilePtr = tmpfile();
    ASSERT_NE(unTarFile.FilePtr, nullptr);
    const std::string validName = "valid/path";
    ASSERT_EQ(fwrite(validName.data(), 1, validName.size(), unTarFile.FilePtr), validName.size());
    rewind(unTarFile.FilePtr);
    ParseTarPath path = {};
    TarFileInfo info = {static_cast<off_t>(validName.size()), 1, 0};
    bool isSkip = false;
    unTarFile.HandleGnuLongName(&path, isSkip, info);
    EXPECT_TRUE(isSkip);
    EXPECT_NE(path.longName, nullptr);
    unTarFile.FreeLongTypePointer(&path);

    fclose(unTarFile.FilePtr);
    unTarFile.FilePtr = tmpfile();
    ASSERT_NE(unTarFile.FilePtr, nullptr);
    const std::string invalidName = "../invalid";
    ASSERT_EQ(fwrite(invalidName.data(), 1, invalidName.size(), unTarFile.FilePtr), invalidName.size());
    rewind(unTarFile.FilePtr);
    info.fileSize = invalidName.size();
    unTarFile.HandleGnuLongLink(&path, isSkip, info);
    EXPECT_EQ(path.longLink, nullptr);

    info.fileSize = PATH_MAX_LEN;
    unTarFile.HandleGnuLongName(&path, isSkip, info);
    EXPECT_EQ(path.longName, nullptr);
}

HWTEST_F(InstalldUnTarFileTest, Installd_Un_Tar_File_ReadWriteAndSwitch_0200, testing::ext::TestSize.Level1)
{
    UnTarFile unTarFile(nullptr);
    unTarFile.FilePtr = tmpfile();
    FILE *dest = tmpfile();
    ASSERT_NE(unTarFile.FilePtr, nullptr);
    ASSERT_NE(dest, nullptr);
    ASSERT_EQ(fwrite("abc", 1, 3, unTarFile.FilePtr), 3U);
    rewind(unTarFile.FilePtr);
    char data[4] = {};
    EXPECT_TRUE(unTarFile.FileReadAndWrite(data, dest, 3));
    EXPECT_STREQ(data, "abc");
    EXPECT_FALSE(unTarFile.FileReadAndWrite(data, dest, 1));
    fclose(dest);

    TarHeader header = {};
    header.typeflag = SPLIT_START_TYPE;
    ParseTarPath path = {};
    char fullPath[] = "/data/local/tmp/untar_switch";
    path.fullPath = fullPath;
    bool isSkip = false;
    bool isSoftLink = false;
    rewind(unTarFile.FilePtr);
    EXPECT_FALSE(unTarFile.ProcessTarBlock(reinterpret_cast<char *>(&header), UnTarFile::eCheckSplit,
        &path, isSkip, isSoftLink));
    EXPECT_TRUE(unTarFile.isSplit);

    header.typeflag = 'z';
    rewind(unTarFile.FilePtr);
    EXPECT_TRUE(unTarFile.ProcessTarBlock(reinterpret_cast<char *>(&header), UnTarFile::eList,
        &path, isSkip, isSoftLink));
    EXPECT_FALSE(isSkip);

    header.typeflag = REGTYPE;
    path.realName = header.name;
    rewind(unTarFile.FilePtr);
    EXPECT_TRUE(unTarFile.ProcessTarBlock(reinterpret_cast<char *>(&header), UnTarFile::eList,
        &path, isSkip, isSoftLink));
    EXPECT_EQ(unTarFile.file_names.size(), 1U);
}
} // namespace OHOS::FileManagement::Backup
