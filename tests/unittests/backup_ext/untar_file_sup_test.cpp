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

#include "b_error/b_error.h"
#include "file_ex.h"
#include "library_func_mock.h"
#include "test_manager.h"
#include "untar_file.cpp"


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
    FileStatInfo info;
    info.fullPath = "realName";
    UntarFile::GetInstance().rootPath_ = "rootPath";
    try {
        UntarFile::GetInstance().ParseFileByTypeFlag(SYMTYPE, info);

        EXPECT_CALL(*funcMock, access(_, _)).WillOnce(Return(1)).WillOnce(Return(0))
            .WillOnce(Return(1)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, mkdir(_, _)).WillOnce(Return(0));
        UntarFile::GetInstance().ParseFileByTypeFlag(DIRTYPE, info);
        EXPECT_EQ(info.fullPath, "rootPath/realName");

        EXPECT_CALL(*funcMock, fseeko(_, _, _)).WillOnce(Return(0));
        UntarFile::GetInstance().ParseFileByTypeFlag('6', info);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by ParseFileByTypeFlag.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_CreateDir_0100";
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
    char buff[BLOCK_SIZE]  = {0};
    size_t buffLen = 0;
    TarHeader *header {};
    int ret = 0;
    try {
        EXPECT_EQ(UntarFile::GetInstance().CheckIfTarBlockValid(buff, buffLen, header, ret), false);
        for (int i = 0; i < BLOCK_SIZE; i++) {
            buff[i] = 'a' + i % 26;
        }
        EXPECT_EQ(UntarFile::GetInstance().CheckIfTarBlockValid(buff, buffLen, header, ret), false);
        buffLen = BLOCK_SIZE;
        EXPECT_EQ(UntarFile::GetInstance().CheckIfTarBlockValid(buff, buffLen, header, ret), false);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileSupTest-an exception occurred by CheckIfTarBlockValid.";
    }
    GTEST_LOG_(INFO) << "UntarFileSupTest-end SUB_Untar_File_CheckIfTarBlockValid_0100";
}
} // namespace OHOS::FileManagement::Backup