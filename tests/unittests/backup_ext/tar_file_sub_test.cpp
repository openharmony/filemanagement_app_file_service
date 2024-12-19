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

#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stack>
#include <sys/types.h>
#include <unistd.h>

#include "b_anony/b_anony.h"
#include "b_error/b_error.h"
#include "b_hiaudit/hi_audit.h"
#include "b_resources/b_constants.h"
#include "directory_ex.h"
#include "filemgmt_libhilog.h"
#include "library_func_mock.h"
#include "securec.h"
#include "tar_file.h"

#include "library_func_define.h"
#include "tar_file.cpp"
#include "library_func_undef.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace OHOS::AppFileService;

class TarFileSubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
    static inline shared_ptr<LibraryFuncMock> funcMock = nullptr;
};

void TarFileSubTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    funcMock = make_shared<LibraryFuncMock>();
    LibraryFuncMock::libraryFunc_ = funcMock;
}

void TarFileSubTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    LibraryFuncMock::libraryFunc_ = nullptr;
    funcMock = nullptr;
}

/**
 * @tc.number: SUB_Tar_File_SplitWriteAll_0100
 * @tc.name: SUB_Tar_File_SplitWriteAll_0100
 * @tc.desc: 测试 SplitWriteAll 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileSubTest, SUB_Tar_File_SplitWriteAll_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileSubTest-begin SUB_Tar_File_SplitWriteAll_0100";
    try {
        vector<uint8_t> ioBuffer {0, 0, 0, 0, 0};
        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(ioBuffer.size()));
        auto ret = TarFile::GetInstance().SplitWriteAll(ioBuffer, 5);
        EXPECT_EQ(ret, ioBuffer.size());

        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(0)).WillOnce(Return(ioBuffer.size()));
        ret = TarFile::GetInstance().SplitWriteAll(ioBuffer, 5);
        EXPECT_EQ(ret, ioBuffer.size());

        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(0)).WillOnce(Return(0));
        ret = TarFile::GetInstance().SplitWriteAll(ioBuffer, 5);
        EXPECT_EQ(ret, 0);

        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(4));
        ret = TarFile::GetInstance().SplitWriteAll(ioBuffer, 4);
        EXPECT_EQ(ret, 4);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileSubTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileSubTest-end SUB_Tar_File_SplitWriteAll_0100";
}

/**
 * @tc.number: SUB_Tar_File_CreateSplitTarFile_0100
 * @tc.name: SUB_Tar_File_CreateSplitTarFile_0100
 * @tc.desc: 测试 CreateSplitTarFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileSubTest, SUB_Tar_File_CreateSplitTarFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileSubTest-begin SUB_Tar_File_CreateSplitTarFile_0100";
    try {
        TarFile::GetInstance().currentTarFile_ = nullptr;
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr));
        EXPECT_THROW(TarFile::GetInstance().CreateSplitTarFile(), BError);

        int n = 0;
        TarFile::GetInstance().currentTarFile_ = reinterpret_cast<FILE*>(&n);
        EXPECT_CALL(*funcMock, fclose(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(reinterpret_cast<FILE*>(&n)));
        EXPECT_TRUE(TarFile::GetInstance().CreateSplitTarFile());
        TarFile::GetInstance().currentTarFile_ = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileSubTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileSubTest-end SUB_Tar_File_CreateSplitTarFile_0100";
}

/**
 * @tc.number: SUB_Tar_File_FillSplitTailBlocks_0100
 * @tc.name: SUB_Tar_File_FillSplitTailBlocks_0100
 * @tc.desc: 测试 FillSplitTailBlocks 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileSubTest, SUB_Tar_File_FillSplitTailBlocks_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileSubTest-begin SUB_Tar_File_FillSplitTailBlocks_0100";
    try {
        TarFile::GetInstance().currentTarFile_ = nullptr;
        EXPECT_THROW(TarFile::GetInstance().FillSplitTailBlocks(), BError);

        int n = 0;
        const int END_BLOCK_SIZE = 1024;
        TarFile::GetInstance().currentTarFile_ = reinterpret_cast<FILE*>(&n);
        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(END_BLOCK_SIZE));
        EXPECT_CALL(*funcMock, ferror(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fflush(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, stat(_, _)).WillOnce(Return(-1));
        EXPECT_THROW(TarFile::GetInstance().FillSplitTailBlocks(), BError);

        TarFile::GetInstance().tarFileCount_ = 1;
        TarFile::GetInstance().fileCount_ = 0;
        TarFile::GetInstance().currentTarFile_ = reinterpret_cast<FILE*>(&n);
        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(END_BLOCK_SIZE));
        EXPECT_CALL(*funcMock, ferror(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fflush(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, stat(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fclose(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, remove(_)).WillOnce(Return(0));
        EXPECT_TRUE(TarFile::GetInstance().FillSplitTailBlocks());

        TarFile::GetInstance().fileCount_ = 1;
        TarFile::GetInstance().isReset_ = true;
        TarFile::GetInstance().currentTarFile_ = reinterpret_cast<FILE*>(&n);
        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(END_BLOCK_SIZE));
        EXPECT_CALL(*funcMock, ferror(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fflush(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, stat(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fclose(_)).WillOnce(Return(0));
        EXPECT_TRUE(TarFile::GetInstance().FillSplitTailBlocks());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileSubTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileSubTest-end SUB_Tar_File_FillSplitTailBlocks_0100";
}

/**
 * @tc.number: SUB_Tar_File_FillSplitTailBlocks_0200
 * @tc.name: SUB_Tar_File_FillSplitTailBlocks_0200
 * @tc.desc: 测试 FillSplitTailBlocks 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileSubTest, SUB_Tar_File_FillSplitTailBlocks_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileSubTest-begin SUB_Tar_File_FillSplitTailBlocks_0200";
    try {
        TarFile::GetInstance().currentTarFile_ = nullptr;
        EXPECT_THROW(TarFile::GetInstance().FillSplitTailBlocks(), BError);

        int n = 0;
        const int END_BLOCK_SIZE = 1024;
        TarFile::GetInstance().fileCount_ = 0;
        TarFile::GetInstance().tarFileCount_ = 0;
        TarFile::GetInstance().isReset_ = false;
        TarFile::GetInstance().currentTarFile_ = reinterpret_cast<FILE*>(&n);
        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(END_BLOCK_SIZE));
        EXPECT_CALL(*funcMock, ferror(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fflush(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, stat(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fclose(_)).WillOnce(Return(0));
        EXPECT_TRUE(TarFile::GetInstance().FillSplitTailBlocks());

        struct stat sta { .st_size = 1 };
        TarFile::GetInstance().currentTarFile_ = reinterpret_cast<FILE*>(&n);
        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(END_BLOCK_SIZE));
        EXPECT_CALL(*funcMock, ferror(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, fflush(_)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, stat(_, _)).WillOnce(DoAll(SetArgPointee<1>(sta), Return(0)));
        EXPECT_CALL(*funcMock, fclose(_)).WillOnce(Return(0));
        EXPECT_TRUE(TarFile::GetInstance().FillSplitTailBlocks());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileSubTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileSubTest-end SUB_Tar_File_FillSplitTailBlocks_0200";
}

/**
 * @tc.number: SUB_Tar_File_WriteFileContent_0100
 * @tc.name: SUB_Tar_File_WriteFileContent_0100
 * @tc.desc: 测试 WriteFileContent 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileSubTest, SUB_Tar_File_WriteFileContent_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileSubTest-begin SUB_Tar_File_WriteFileContent_0100";
    try {
        string fileName = "fileName";
        off_t size = 0;
        int err = 0;
        EXPECT_CALL(*funcMock, open(_, _)).WillOnce(Return(-1));
        EXPECT_FALSE(TarFile::GetInstance().WriteFileContent(fileName, size, err));

        TarFile::GetInstance().ioBuffer_.resize(BLOCK_SIZE);
        EXPECT_CALL(*funcMock, open(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, read(_, _, _)).WillOnce(Return(BLOCK_SIZE));
        EXPECT_CALL(*funcMock, close(_)).WillOnce(Return(0));
        EXPECT_FALSE(TarFile::GetInstance().WriteFileContent(fileName, 1, err));

        TarFile::GetInstance().ioBuffer_.resize(BLOCK_SIZE);
        EXPECT_CALL(*funcMock, open(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, read(_, _, _)).WillOnce(Return(BLOCK_SIZE));
        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(BLOCK_SIZE + 1));
        EXPECT_CALL(*funcMock, close(_)).WillOnce(Return(0));
        EXPECT_FALSE(TarFile::GetInstance().WriteFileContent(fileName, BLOCK_SIZE, err));

        TarFile::GetInstance().ioBuffer_.resize(BLOCK_SIZE);
        EXPECT_CALL(*funcMock, open(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*funcMock, read(_, _, _)).WillOnce(Return(BLOCK_SIZE));
        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(BLOCK_SIZE));
        EXPECT_CALL(*funcMock, close(_)).WillOnce(Return(0));
        EXPECT_TRUE(TarFile::GetInstance().WriteFileContent(fileName, BLOCK_SIZE, err));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileSubTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileSubTest-end SUB_Tar_File_WriteFileContent_0100";
}
} // namespace OHOS::FileManagement::Backup