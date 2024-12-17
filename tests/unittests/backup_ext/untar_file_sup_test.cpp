/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <utime.h>

#include "b_anony/b_anony.h"
#include "b_error/b_error.h"
#include "b_filesystem/b_dir.h"
#include "directory_ex.h"
#include "file_ex.h"
#include "filemgmt_libhilog.h"
#include "library_func_mock.h"
#include "securec.h"
#include "test_manager.h"
#include "untar_file.h"

#include "library_func_define.h"
#include "untar_file.cpp"
#include "library_func_undef.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace OHOS::AppFileService;
class UntarFileSupTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
    static inline shared_ptr<LibraryFuncMock> funcMock = nullptr;
};

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

void UntarFileSupTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    funcMock = make_shared<LibraryFuncMock>();
    LibraryFuncMock::libraryFunc_ = funcMock;
}

void UntarFileSupTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    LibraryFuncMock::libraryFunc_ = nullptr;
    funcMock = nullptr;
    ClearCache();
}

/**
 * @tc.number: SUB_Untar_File_IsEmptyBlock_0100
 * @tc.name: SUB_Untar_File_IsEmptyBlock_0100
 * @tc.desc: 测试 IsEmptyBlock
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_IsEmptyBlock_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_IsEmptyBlock_0100";
    try {
        EXPECT_EQ(IsEmptyBlock(nullptr), true);
        char buff[BLOCK_SIZE] = {0};
        EXPECT_EQ(IsEmptyBlock(buff), true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by IsEmptyBlock.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_IsEmptyBlock_0100";
}

/**
 * @tc.number: SUB_Untar_File_ParseOctalStr_0100
 * @tc.name: SUB_Untar_File_ParseOctalStr_0100
 * @tc.desc: 测试 ParseOctalStr 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_ParseOctalStr_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_ParseOctalStr_0100";
    EXPECT_EQ(ParseOctalStr("0", 0), 0);
    EXPECT_EQ(ParseOctalStr("1234", 0), 0);
    EXPECT_EQ(ParseOctalStr("()-891234", 10), 668);
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_ParseOctalStr_0100";
}

/**
 * @tc.number: SUB_Untar_File_GenRealPath_0100
 * @tc.name: SUB_Untar_File_GenRealPath_0100
 * @tc.desc: 测试 GenRealPath 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_GenRealPath_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_GenRealPath_0100";
    string rootPath = "";
    string realName = "";
    EXPECT_EQ(UntarFile::GetInstance().GenRealPath(rootPath, realName), "");

    rootPath = "rootPath";
    EXPECT_EQ(UntarFile::GetInstance().GenRealPath(rootPath, realName), "");

    realName = "realName";
    EXPECT_EQ(UntarFile::GetInstance().GenRealPath(rootPath, realName), "rootPath/realName");

    rootPath = "/rootPath/";
    realName = "/realName";
    EXPECT_EQ(UntarFile::GetInstance().GenRealPath(rootPath, realName), "rootPath/realName");
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_GenRealPath_0100";
}

/**
 * @tc.number: SUB_Untar_File_ForceCreateDirectoryWithMode_0100
 * @tc.name: SUB_Untar_File_ForceCreateDirectoryWithMode_0100
 * @tc.desc: 测试 ForceCreateDirectoryWithMode 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_ForceCreateDirectoryWithMode_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_ForceCreateDirectoryWithMode_0100";
    string rootPath = "rootPath/realName";
    EXPECT_CALL(*funcMock, access(_, _)).WillOnce(Return(1));
    EXPECT_CALL(*funcMock, mkdir(_, _)).WillOnce(Return(1));
    EXPECT_EQ(ForceCreateDirectoryWithMode(rootPath, S_IRWXU), false);

    EXPECT_CALL(*funcMock, access(_, _)).WillOnce(Return(0)).WillOnce(Return(1)).WillOnce(Return(0));
    EXPECT_CALL(*funcMock, mkdir(_, _)).WillOnce(Return(0));
    EXPECT_EQ(ForceCreateDirectoryWithMode(rootPath, S_IRWXU), true);

    EXPECT_CALL(*funcMock, access(_, _)).WillOnce(Return(0)).WillOnce(Return(1)).WillOnce(Return(1));
    EXPECT_CALL(*funcMock, mkdir(_, _)).WillOnce(Return(0));
    EXPECT_EQ(ForceCreateDirectoryWithMode(rootPath, S_IRWXU), false);
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_ParseOctalStr_0100";
}

/**
 * @tc.number: SUB_Untar_File_ReadLongName_0100
 * @tc.name: SUB_Untar_File_ReadLongName_0100
 * @tc.desc: 测试 ReadLongName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_ReadLongName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_ReadLongName_0100";
    FileStatInfo info;
    info.fullPath = "test";
    UntarFile::GetInstance().tarFileSize_ = PATH_MAX_LEN + 1;
    auto [ret, fileInfo] = UntarFile::GetInstance().ReadLongName(info);
    EXPECT_EQ(ret, -1);

    UntarFile::GetInstance().tarFileSize_ = PATH_MAX_LEN;
    EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(1));
    tie(ret, fileInfo) = UntarFile::GetInstance().ReadLongName(info);
    EXPECT_EQ(ret, -1);

    UntarFile::GetInstance().tarFileSize_ = PATH_MAX_LEN;
    EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(PATH_MAX_LEN));
    EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(1));
    tie(ret, fileInfo) = UntarFile::GetInstance().ReadLongName(info);
    EXPECT_EQ(ret, -1);

    UntarFile::GetInstance().tarFileSize_ = PATH_MAX_LEN;
    EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(PATH_MAX_LEN));
    EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
    tie(ret, fileInfo) = UntarFile::GetInstance().ReadLongName(info);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_ParseOctalStr_0100";
}

/**
 * @tc.number: SUB_Untar_File_CreateDir_0100
 * @tc.name: SUB_Untar_File_CreateDir_0100
 * @tc.desc: 测试 CreateDir 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_CreateDir_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_CreateDir_0100";
    string rootPath = "";
    try {
        UntarFile::GetInstance().CreateDir(rootPath, S_IRWXU);
        rootPath = "rootPath/realName";
        EXPECT_CALL(*funcMock, access(_, _)).WillOnce(Return(0));
        UntarFile::GetInstance().CreateDir(rootPath, S_IRWXU);

        rootPath = "rootPath/realName/";
        EXPECT_CALL(*funcMock, access(_, _)).WillOnce(Return(0));
        UntarFile::GetInstance().CreateDir(rootPath, S_IRWXU);

        rootPath = "rootPath/realName";
        EXPECT_CALL(*funcMock, access(_, _)).WillOnce(Return(1)).WillOnce(Return(0))
            .WillOnce(Return(1)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, mkdir(_, _)).WillOnce(Return(0));
        UntarFile::GetInstance().CreateDir(rootPath, S_IRWXU);

        EXPECT_CALL(*funcMock, access(_, _)).WillOnce(Return(1)).WillOnce(Return(0))
            .WillOnce(Return(1)).WillOnce(Return(1));
        EXPECT_CALL(*funcMock, mkdir(_, _)).WillOnce(Return(0));
        UntarFile::GetInstance().CreateDir(rootPath, S_IRWXU);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by CreateDir.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_CreateDir_0100";
}

/**
 * @tc.number: SUB_Untar_File_ParseFileByTypeFlag_0100
 * @tc.name: SUB_Untar_File_ParseFileByTypeFlag_0100
 * @tc.desc: 测试 ParseFileByTypeFlag 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_ParseFileByTypeFlag_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_CreateDir_0100";
    UntarFile::GetInstance().rootPath_ = "rootPath";
    try {
        char c = '\0';
        FileStatInfo info;
        info.fullPath = "/test.txt";
        EXPECT_CALL(*funcMock, realpath(_, _)).WillOnce(Return(&c));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        auto [ret, flag, m] = UntarFile::GetInstance().ParseFileByTypeFlag(REGTYPE, info);
        EXPECT_EQ(ret, 0);
        EXPECT_FALSE(flag);

        info.fullPath = "/test/../test.txt";
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseFileByTypeFlag(REGTYPE, info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);

        info.fullPath = "/test.txt";
        EXPECT_CALL(*funcMock, realpath(_, _)).WillOnce(Return(&c));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseFileByTypeFlag(AREGTYPE, info);
        EXPECT_EQ(ret, 0);
        EXPECT_FALSE(flag);

        info.fullPath = "/test/../test.txt";
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseFileByTypeFlag(AREGTYPE, info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);

        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseFileByTypeFlag(SYMTYPE, info);
        EXPECT_EQ(ret, 0);
        EXPECT_TRUE(flag);

        info.fullPath = "/test/";
        EXPECT_CALL(*funcMock, access(_, _)).WillOnce(Return(0));
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseFileByTypeFlag(DIRTYPE, info);
        EXPECT_EQ(ret, 0);
        EXPECT_FALSE(flag);

        info.fullPath = "/test/../test/";
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseFileByTypeFlag(DIRTYPE, info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by ParseFileByTypeFlag.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_CreateDir_0100";
}

/**
 * @tc.number: SUB_Untar_File_ParseFileByTypeFlag_0200
 * @tc.name: SUB_Untar_File_ParseFileByTypeFlag_0200
 * @tc.desc: 测试 ParseFileByTypeFlag 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_ParseFileByTypeFlag_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_CreateDir_0100";
    UntarFile::GetInstance().rootPath_ = "rootPath";
    try {
        FileStatInfo info;
        UntarFile::GetInstance().tarFileSize_ = PATH_MAX_LEN + 1;
        info.fullPath = "/test/../test/";
        auto [ret, flag, m] = UntarFile::GetInstance().ParseFileByTypeFlag(GNUTYPE_LONGNAME, info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);

        info.fullPath = "/test";
        info.longName = "/test/../longName";
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseFileByTypeFlag(GNUTYPE_LONGNAME, info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);

        info.fullPath = "/test";
        info.longName = "longName";
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseFileByTypeFlag(GNUTYPE_LONGNAME, info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);

        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseFileByTypeFlag('6', info);
        EXPECT_EQ(ret, 0);
        EXPECT_TRUE(flag);

        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(-1));
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseFileByTypeFlag('6', info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by ParseFileByTypeFlag.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_CreateDir_0200";
}

/**
 * @tc.number: SUB_Untar_File_DealFileTag_0100
 * @tc.name: SUB_Untar_File_DealFileTag_0100
 * @tc.desc: 测试 DealFileTag 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_DealFileTag_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_DealFileTag_0100";
    try {
        char c = '\0';
        ErrFileInfo errFileInfo;
        FileStatInfo info;
        bool isFilter = false;
        std::string tmpFullPath;
        info.fullPath = "/test.txt";
        UntarFile::GetInstance().includes_.clear();
        EXPECT_CALL(*funcMock, realpath(_, _)).WillOnce(Return(&c));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        EXPECT_EQ(UntarFile::GetInstance().DealFileTag(errFileInfo, info, isFilter, tmpFullPath), true);

        info.fullPath = "/../test.txt";
        UntarFile::GetInstance().rootPath_ = "/root/";
        EXPECT_EQ(UntarFile::GetInstance().DealFileTag(errFileInfo, info, isFilter, tmpFullPath), false);

        ReportFileInfo report;
        UntarFile::GetInstance().includes_.emplace("/test/", report);
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        EXPECT_EQ(UntarFile::GetInstance().DealFileTag(errFileInfo, info, isFilter, tmpFullPath), true);

        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(-1));
        EXPECT_EQ(UntarFile::GetInstance().DealFileTag(errFileInfo, info, isFilter, tmpFullPath), false);

        tmpFullPath = "/test/";
        info.fullPath = "/test.txt";
        EXPECT_CALL(*funcMock, realpath(_, _)).WillOnce(Return(&c));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        EXPECT_EQ(UntarFile::GetInstance().DealFileTag(errFileInfo, info, isFilter, tmpFullPath), true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by DealFileTag.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_DealFileTag_0100";
}

/**
 * @tc.number: SUB_Untar_File_ParseIncrementalFileByTypeFlag_0100
 * @tc.name: SUB_Untar_File_ParseIncrementalFileByTypeFlag_0100
 * @tc.desc: 测试 ParseIncrementalFileByTypeFlag 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_ParseIncrementalFileByTypeFlag_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_CreateDir_0100";
    UntarFile::GetInstance().rootPath_ = "rootPath";
    try {
        char c = '\0';
        FileStatInfo info;
        info.fullPath = "/test.txt";
        UntarFile::GetInstance().includes_.clear();
        EXPECT_CALL(*funcMock, realpath(_, _)).WillOnce(Return(&c));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        auto [ret, flag, m] = UntarFile::GetInstance().ParseIncrementalFileByTypeFlag(REGTYPE, info);
        EXPECT_EQ(ret, 0);
        EXPECT_FALSE(flag);

        info.fullPath = "/test/../test.txt";
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseIncrementalFileByTypeFlag(REGTYPE, info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);

        info.fullPath = "/test.txt";
        EXPECT_CALL(*funcMock, realpath(_, _)).WillOnce(Return(&c));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseIncrementalFileByTypeFlag(AREGTYPE, info);
        EXPECT_EQ(ret, 0);
        EXPECT_FALSE(flag);

        info.fullPath = "/test/../test.txt";
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseIncrementalFileByTypeFlag(AREGTYPE, info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);

        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseIncrementalFileByTypeFlag(SYMTYPE, info);
        EXPECT_EQ(ret, 0);
        EXPECT_TRUE(flag);

        info.fullPath = "/test/";
        EXPECT_CALL(*funcMock, access(_, _)).WillOnce(Return(0));
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseIncrementalFileByTypeFlag(DIRTYPE, info);
        EXPECT_EQ(ret, 0);
        EXPECT_FALSE(flag);

        info.fullPath = "/test/../test/";
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseIncrementalFileByTypeFlag(DIRTYPE, info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by ParseIncrementalFileByTypeFlag.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_CreateDir_0100";
}

/**
 * @tc.number: SUB_Untar_File_ParseIncrementalFileByTypeFlag_0200
 * @tc.name: SUB_Untar_File_ParseIncrementalFileByTypeFlag_0200
 * @tc.desc: 测试 ParseIncrementalFileByTypeFlag 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_ParseIncrementalFileByTypeFlag_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_CreateDir_0100";
    UntarFile::GetInstance().rootPath_ = "rootPath";
    try {
        FileStatInfo info;
        UntarFile::GetInstance().tarFileSize_ = PATH_MAX_LEN + 1;
        info.fullPath = "/test/../test/";
        auto [ret, flag, m] = UntarFile::GetInstance().ParseIncrementalFileByTypeFlag(GNUTYPE_LONGNAME, info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);

        info.fullPath = "/test";
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseIncrementalFileByTypeFlag(GNUTYPE_LONGNAME, info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);

        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseIncrementalFileByTypeFlag('6', info);
        EXPECT_EQ(ret, 0);
        EXPECT_TRUE(flag);

        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(-1));
        tie(ret, flag, std::ignore) = UntarFile::GetInstance().ParseIncrementalFileByTypeFlag('6', info);
        EXPECT_EQ(ret, DEFAULT_ERR);
        EXPECT_TRUE(flag);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by ParseIncrementalFileByTypeFlag.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_CreateDir_0200";
}

/**
 * @tc.number: SUB_Untar_File_IsValidTarBlock_0100
 * @tc.name: SUB_Untar_File_IsValidTarBlock_0100
 * @tc.desc: 测试 IsValidTarBlock 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_IsValidTarBlock_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_IsValidTarBlock_0100";
    TarHeader header { .magic = "test" };
    try {
        EXPECT_EQ(UntarFile::GetInstance().IsValidTarBlock(header), false);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by IsValidTarBlock.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_IsValidTarBlock_0100";
}

/**
 * @tc.number: SUB_Untar_File_IsValidTarBlock_0200
 * @tc.name: SUB_Untar_File_IsValidTarBlock_0200
 * @tc.desc: 测试 IsValidTarBlock 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_IsValidTarBlock_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_IsValidTarBlock_0200";
    TarHeader header { .magic = "ustar" };
    try {
        EXPECT_EQ(UntarFile::GetInstance().IsValidTarBlock(header), false);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by IsValidTarBlock.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_IsValidTarBlock_0200";
}

/**
 * @tc.number: SUB_Untar_File_CheckIfTarBlockValid_0100
 * @tc.name: SUB_Untar_File_CheckIfTarBlockValid_0100
 * @tc.desc: 测试 CheckIfTarBlockValid 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_CheckIfTarBlockValid_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_CheckIfTarBlockValid_0100";
    try {
        char buff[BLOCK_SIZE]  = {0};
        size_t buffLen = 0;
        TarHeader header;
        int ret = 0;

        EXPECT_EQ(UntarFile::GetInstance().CheckIfTarBlockValid(nullptr, 0, nullptr, ret), false);

        EXPECT_EQ(UntarFile::GetInstance().CheckIfTarBlockValid(buff, 0, nullptr, ret), false);

        buffLen = BLOCK_SIZE;
        EXPECT_EQ(UntarFile::GetInstance().CheckIfTarBlockValid(buff, buffLen, nullptr, ret), false);

        buff[0] = '0';
        EXPECT_CALL(*funcMock, ftello(_)).WillOnce(Return(0));
        EXPECT_EQ(UntarFile::GetInstance().CheckIfTarBlockValid(buff, buffLen, &header, ret), false);

        buff[0] = '\0';
        header.typeFlag = GNUTYPE_LONGNAME;
        UntarFile::GetInstance().tarFileSize_ = 0;
        EXPECT_CALL(*funcMock, ftello(_)).WillOnce(Return(READ_BUFF_SIZE + 1));
        EXPECT_EQ(UntarFile::GetInstance().CheckIfTarBlockValid(buff, buffLen, &header, ret), false);

        header.typeFlag = REGTYPE;
        EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, ftello(_)).WillOnce(Return(0));
        EXPECT_EQ(UntarFile::GetInstance().CheckIfTarBlockValid(buff, buffLen, &header, ret), false);

        EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(BLOCK_SIZE));
        EXPECT_EQ(UntarFile::GetInstance().CheckIfTarBlockValid(buff, buffLen, &header, ret), false);

        EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(WithArgs<0>(Invoke([](void* buff) {
            (reinterpret_cast<char*>(buff))[0] = '0';
            return BLOCK_SIZE;
        })));
        EXPECT_CALL(*funcMock, ftello(_)).WillOnce(Return(0));
        EXPECT_EQ(UntarFile::GetInstance().CheckIfTarBlockValid(buff, buffLen, &header, ret), false);

        string sum = "01647";
        header.typeFlag = 'x';
        memcpy_s(&header.magic, sizeof(header.magic), TMAGIC.c_str(), TMAGIC.length());
        memcpy_s(&header.chksum, sizeof(header.chksum), sum.c_str(), sum.length());
        buff[0] = '0';
        EXPECT_EQ(UntarFile::GetInstance().CheckIfTarBlockValid(buff, buffLen, &header, ret), true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by CheckIfTarBlockValid.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_CheckIfTarBlockValid_0100";
}

/**
 * @tc.number: SUB_Untar_File_ParseRegularFile_0100
 * @tc.name: SUB_Untar_File_ParseRegularFile_0100
 * @tc.desc: 测试 ParseRegularFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_ParseRegularFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_ParseRegularFile_0100";
    try {
        char c = '\0';
        FileStatInfo info;
        info.fullPath = "/test.txt";
        EXPECT_CALL(*funcMock, realpath(_, _)).WillOnce(Return(&c));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        auto ret = UntarFile::GetInstance().ParseRegularFile(info);
        EXPECT_EQ(ret[info.fullPath].size(), 1);

        UntarFile::GetInstance().tarFileSize_ = READ_BUFF_SIZE + READ_BUFF_SIZE - 1;
        EXPECT_CALL(*funcMock, realpath(_, _)).WillOnce(Return(&c));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(reinterpret_cast<FILE*>(&c)));
        EXPECT_CALL(*funcMock, fread(_, _, _, _))
            .WillOnce(Return(READ_BUFF_SIZE))
            .WillOnce(Return(READ_BUFF_SIZE >> 1))
            .WillOnce(Return((READ_BUFF_SIZE >> 1) - 1));
        EXPECT_CALL(*funcMock, fwrite(_, _, _, _))
            .WillOnce(Return(READ_BUFF_SIZE))
            .WillOnce(Return(READ_BUFF_SIZE >> 1))
            .WillOnce(Return((READ_BUFF_SIZE >> 1) - 1));
        EXPECT_CALL(*funcMock, fclose(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, chmod(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, stat(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        ret = UntarFile::GetInstance().ParseRegularFile(info);
        EXPECT_EQ(ret[info.fullPath].size(), 0);

        UntarFile::GetInstance().tarFileSize_ = 0;
        EXPECT_CALL(*funcMock, realpath(_, _)).WillOnce(Return(&c));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(reinterpret_cast<FILE*>(&c)));
        EXPECT_CALL(*funcMock, fclose(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, chmod(_, _)).WillOnce(Return(1));
        EXPECT_CALL(*funcMock, stat(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        ret = UntarFile::GetInstance().ParseRegularFile(info);
        EXPECT_EQ(ret[info.fullPath].size(), 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by ParseRegularFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_ParseRegularFile_0100";
}

/**
 * @tc.number: SUB_Untar_File_ParseRegularFile_0200
 * @tc.name: SUB_Untar_File_ParseRegularFile_0200
 * @tc.desc: 测试 ParseRegularFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileSupTest, SUB_Untar_File_ParseRegularFile_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_ParseRegularFile_0200";
    try {
        char c = '\0';
        FileStatInfo info;
        info.fullPath = "/test.txt";
        UntarFile::GetInstance().tarFileSize_ = 0;
        EXPECT_CALL(*funcMock, realpath(_, _)).WillOnce(Return(&c));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(reinterpret_cast<FILE*>(&c)));
        EXPECT_CALL(*funcMock, fclose(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, chmod(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, stat(_, _)).WillOnce(Return(1));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        auto ret = UntarFile::GetInstance().ParseRegularFile(info);
        EXPECT_EQ(ret[info.fullPath].size(), 1);

        info.mtime = 1;
        EXPECT_CALL(*funcMock, realpath(_, _)).WillOnce(Return(&c));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(reinterpret_cast<FILE*>(&c)));
        EXPECT_CALL(*funcMock, fclose(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, chmod(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, stat(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, utime(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        ret = UntarFile::GetInstance().ParseRegularFile(info);
        EXPECT_EQ(ret[info.fullPath].size(), 0);

        EXPECT_CALL(*funcMock, realpath(_, _)).WillOnce(Return(&c));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(reinterpret_cast<FILE*>(&c)));
        EXPECT_CALL(*funcMock, fclose(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, chmod(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, stat(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, utime(_, _)).WillOnce(Return(1));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        ret = UntarFile::GetInstance().ParseRegularFile(info);
        EXPECT_EQ(ret[info.fullPath].size(), 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by ParseRegularFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_ParseRegularFile_0200";
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
HWTEST_F(UntarFileSupTest, SUB_Untar_File_UnPacket_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileSupTest-begin SUB_Untar_File_UnPacket_0100";
    try {
        string tarFile;
        string rootPath;
        errno = EPERM;
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr));
        auto [ret, info, err] = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
        EXPECT_EQ(ret, EPERM);

        char c = '\0';
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(reinterpret_cast<FILE*>(&c)));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0)).WillOnce(Return(1));
        EXPECT_CALL(*funcMock, ftello(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fclose(_)).WillOnce(Return(0));
        tie(ret, info, err) = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
        EXPECT_EQ(ret, 0);

        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(reinterpret_cast<FILE*>(&c)));
        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, ftello(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fclose(_)).WillOnce(Return(0));
        tie(ret, info, err) = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by UnPacket.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_UnPacket_0100";
}
} // namespace OHOS::FileManagement::Backup