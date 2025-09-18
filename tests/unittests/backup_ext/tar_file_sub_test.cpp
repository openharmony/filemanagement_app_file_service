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
#include <memory>

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

#if defined(BROTLI_ENABLED) || defined(LZ4_ENABLED)
#define COMPRESS_ENABLED true
#endif

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace OHOS::AppFileService;

class CompressMock : public ICompressStrategy {
public:
    MOCK_METHOD(bool, CompressBuffer, (Buffer& input, Buffer& output));
    MOCK_METHOD(bool, DecompressBuffer, (Buffer& compressed, Buffer& origin));
    MOCK_METHOD(std::string, GetFileSuffix, ());
    MOCK_METHOD(int32_t, GetMaxCompressedSizeInner, (uint32_t inputSize));
};

class TarFileSubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override
    {
        compressMock_ = std::make_shared<CompressMock>();
        TarFile::GetInstance().compressTool_ = compressMock_;
    }
    void TearDown() override
    {
        TarFile::GetInstance().compressTool_ = nullptr;
    }
    static inline shared_ptr<LibraryFuncMock> funcMock = nullptr;
    std::shared_ptr<CompressMock> compressMock_ = nullptr;
    static inline FILE* testFilePtr_ = nullptr;
};

void TarFileSubTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    testFilePtr_ = fopen("test.txt", "w");
    funcMock = make_shared<LibraryFuncMock>();
    LibraryFuncMock::libraryFunc_ = funcMock;
}

void TarFileSubTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    LibraryFuncMock::libraryFunc_ = nullptr;
    funcMock = nullptr;
    if (testFilePtr_ != nullptr) {
        fclose(testFilePtr_);
        testFilePtr_ = nullptr;
    }
    if (remove("test.txt")  != 0) {
        GTEST_LOG_(ERROR) << "remove test.txt fail";
    }
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
        int err = 0;
        vector<uint8_t> ioBuffer {0, 0, 0, 0, 0};
        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(ioBuffer.size()));
        auto ret = TarFile::GetInstance().SplitWriteAll(ioBuffer, 5, err);
        EXPECT_EQ(ret, ioBuffer.size());

        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(0)).WillOnce(Return(ioBuffer.size()));
        ret = TarFile::GetInstance().SplitWriteAll(ioBuffer, 5, err);
        EXPECT_EQ(ret, ioBuffer.size());

        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(0)).WillOnce(Return(0));
        ret = TarFile::GetInstance().SplitWriteAll(ioBuffer, 5, err);
        EXPECT_EQ(ret, 0);

        EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(4));
        ret = TarFile::GetInstance().SplitWriteAll(ioBuffer, 4, err);
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

/**
 * @tc.number: WRITE_COMPRESS_DATA_TEST_001
 * @tc.name: WRITE_COMPRESS_DATA_TEST_001
 * @tc.desc: 测试 WriteCompressData 接口fopen文件失败的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, WRITE_COMPRESS_DATA_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin WRITE_COMPRESS_DATA_TEST_001";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    UniqueFile fout("/tmp/test1_1.txt", "wb");

    GTEST_LOG_(INFO) << "Test1. file null";
    Buffer origin(10);
    Buffer compress1(20);
    bool rs = TarFile::GetInstance().WriteCompressData(compress1, origin, fout);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end WRITE_COMPRESS_DATA_TEST_001";
}

/**
 * @tc.number: WRITE_COMPRESS_DATA_TEST_002
 * @tc.name: WRITE_COMPRESS_DATA_TEST_002
 * @tc.desc: 测试 WriteCompressData 压缩后更大且写入失败的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, WRITE_COMPRESS_DATA_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin WRITE_COMPRESS_DATA_TEST_002";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    UniqueFile fout2("/tmp/test2_2.txt", "wb");

    Buffer origin(10);
    Buffer compress1(20);
    GTEST_LOG_(INFO) << "Test2. compressed bigger, write fail";
    EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(0));
    bool rs = TarFile::GetInstance().WriteCompressData(compress1, origin, fout2);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end WRITE_COMPRESS_DATA_TEST_002";
}

/**
 * @tc.number: WRITE_COMPRESS_DATA_TEST_003
 * @tc.name: WRITE_COMPRESS_DATA_TEST_003
 * @tc.desc: 测试 WriteCompressData 压缩后更大且写入成功的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, WRITE_COMPRESS_DATA_TEST_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin WRITE_COMPRESS_DATA_TEST_003";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    UniqueFile fout2("/tmp/test3_2.txt", "wb");

    Buffer origin(10);
    Buffer compress1(20);
    GTEST_LOG_(INFO) << "Test3. compressed bigger, write success";
    size_t sizeCount = 1;
    EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(sizeCount)).WillOnce(Return(sizeCount))
        .WillOnce(Return(10));
    bool rs = TarFile::GetInstance().WriteCompressData(compress1, origin, fout2);
    EXPECT_TRUE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end WRITE_COMPRESS_DATA_TEST_003";
}

/**
 * @tc.number: WRITE_COMPRESS_DATA_TEST_004
 * @tc.name: WRITE_COMPRESS_DATA_TEST_004
 * @tc.desc: 测试 WriteCompressData 压缩后更新且写入成功的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, WRITE_COMPRESS_DATA_TEST_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin WRITE_COMPRESS_DATA_TEST_004";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    UniqueFile fout2("/tmp/test4.txt", "wb");

    GTEST_LOG_(INFO) << "Test4. compressed smaller, write success";
    Buffer origin(10);
    Buffer compress2(9);
    size_t sizeCount = 1;
    EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(sizeCount)).WillOnce(Return(sizeCount))
        .WillOnce(Return(9));
    bool rs = TarFile::GetInstance().WriteCompressData(compress2, origin, fout2);
    EXPECT_TRUE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end WRITE_COMPRESS_DATA_TEST_004";
}

/**
 * @tc.number: WRITE_DECOMPRESS_DATA_TEST_001
 * @tc.name: WRITE_DECOMPRESS_DATA_TEST_001
 * @tc.desc: 测试 WriteDecompressData 接口 fopen失败的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, WRITE_DECOMPRESS_DATA_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin WRITE_DECOMPRESS_DATA_TEST_001";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    UniqueFile fout("/tmp/test_decomp_1.txt", "wb");
    auto decompSpan = std::chrono::duration<double, std::milli>(std::chrono::seconds(0));

    GTEST_LOG_(INFO) << "Test1. file null";
    Buffer origin(10);
    Buffer compress1(10);
    bool rs = TarFile::GetInstance().WriteDecompressData(compress1, origin, fout, decompSpan);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end WRITE_DECOMPRESS_DATA_TEST_001";
}

/**
 * @tc.number: WRITE_DECOMPRESS_DATA_TEST_002
 * @tc.name: WRITE_DECOMPRESS_DATA_TEST_002
 * @tc.desc: 测试 WriteDecompressData 接口 压缩后大小等于压缩前且写入失败的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, WRITE_DECOMPRESS_DATA_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin WRITE_DECOMPRESS_DATA_TEST_002";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    UniqueFile fout("/tmp/test_decomp_2.txt", "wb");
    auto decompSpan = std::chrono::duration<double, std::milli>(std::chrono::seconds(0));
    Buffer origin(10);
    Buffer compress1(10);

    GTEST_LOG_(INFO) << "Test2. compressed equal origin, write fail";
    EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(0));
    bool rs = TarFile::GetInstance().WriteDecompressData(compress1, origin, fout, decompSpan);
    EXPECT_FALSE(rs);

    GTEST_LOG_(INFO) << "TarFileTest-end WRITE_DECOMPRESS_DATA_TEST_002";
}

/**
 * @tc.number: WRITE_DECOMPRESS_DATA_TEST_003
 * @tc.name: WRITE_DECOMPRESS_DATA_TEST_003
 * @tc.desc: 测试 WriteDecompressData 接口 压缩后大小等于压缩前且写入成功的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, WRITE_DECOMPRESS_DATA_TEST_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin WRITE_DECOMPRESS_DATA_TEST_003";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    UniqueFile fout("/tmp/test_decomp_3.txt", "wb");
    auto decompSpan = std::chrono::duration<double, std::milli>(std::chrono::seconds(0));
    Buffer origin(10);
    Buffer compress1(10);

    GTEST_LOG_(INFO) << "Test3. compressed equal origin, write success";
    EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(10));
    bool rs = TarFile::GetInstance().WriteDecompressData(compress1, origin, fout, decompSpan);
    EXPECT_TRUE(rs);

    GTEST_LOG_(INFO) << "TarFileTest-end WRITE_DECOMPRESS_DATA_TEST_003";
}

/**
 * @tc.number: WRITE_DECOMPRESS_DATA_TEST_004
 * @tc.name: WRITE_DECOMPRESS_DATA_TEST_004
 * @tc.desc: 测试 WriteDecompressData 接口 压缩后大小小于压缩前且解压成功的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, WRITE_DECOMPRESS_DATA_TEST_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin WRITE_DECOMPRESS_DATA_TEST_004";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    UniqueFile fout("/tmp/test_decomp_4.txt", "wb");
    auto decompSpan = std::chrono::duration<double, std::milli>(std::chrono::seconds(0));
    Buffer origin(10);

    GTEST_LOG_(INFO) << "Test4. compressed smaller , decompress success";
    EXPECT_CALL(*compressMock_, DecompressBuffer(_, _)).WillOnce(Return(true));
    EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillRepeatedly(Return(10));
    Buffer compress2(8);
    bool rs = TarFile::GetInstance().WriteDecompressData(compress2, origin, fout, decompSpan);
    EXPECT_TRUE(rs);

    GTEST_LOG_(INFO) << "TarFileTest-end WRITE_DECOMPRESS_DATA_TEST_004";
}

/**
 * @tc.number: WRITE_DECOMPRESS_DATA_TEST_005
 * @tc.name: WRITE_DECOMPRESS_DATA_TEST_005
 * @tc.desc: 测试 WriteDecompressData 接口 压缩后大小小于压缩前且解压失败的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, WRITE_DECOMPRESS_DATA_TEST_005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin WRITE_DECOMPRESS_DATA_TEST_005";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    UniqueFile fout("/tmp/test_decomp_5.txt", "wb");
    auto decompSpan = std::chrono::duration<double, std::milli>(std::chrono::seconds(0));
    Buffer compress2(8);
    Buffer origin(10);

    GTEST_LOG_(INFO) << "Test5. compressed smaller , decompress fail";
    EXPECT_CALL(*compressMock_, DecompressBuffer(_, _)).WillOnce(Return(false));
    EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillRepeatedly(Return(10));
    bool rs = TarFile::GetInstance().WriteDecompressData(compress2, origin, fout, decompSpan);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end WRITE_DECOMPRESS_DATA_TEST_005";
}

#ifdef COMPRESS_ENABLED
/**
 * @tc.number: COMPRESS_FILE_TEST_001
 * @tc.name: COMPRESS_FILE_TEST_001
 * @tc.desc: 测试 CompressFile 接口fopen失败场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, COMPRESS_FILE_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin COMPRESS_FILE_TEST_001";
    std::string srcFile = "/tmp/in_comp1.txt";
    std::string compressFile = "/tmp/out_comp1.txt";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    GTEST_LOG_(INFO) << "Test1. file null";
    bool rs = TarFile::GetInstance().CompressFile(srcFile, compressFile);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end COMPRESS_FILE_TEST_001";
}

/**
 * @tc.number: COMPRESS_FILE_TEST_002
 * @tc.name: COMPRESS_FILE_TEST_002
 * @tc.desc: 测试 CompressFile 接口获取maxSize失败场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, COMPRESS_FILE_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin COMPRESS_FILE_TEST_002";
    std::string srcFile = "/tmp/in_comp2.txt";
    std::string compressFile = "/tmp/out_comp2.txt";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillRepeatedly(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*compressMock_, GetMaxCompressedSizeInner(_)).WillOnce(Return(0));
    GTEST_LOG_(INFO) << "Test2. get max size fail";
    bool rs = TarFile::GetInstance().CompressFile(srcFile, compressFile);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end COMPRESS_FILE_TEST_002";
}

/**
 * @tc.number: COMPRESS_FILE_TEST_003
 * @tc.name: COMPRESS_FILE_TEST_003
 * @tc.desc: 测试 CompressFile 接口fread失败场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, COMPRESS_FILE_TEST_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin COMPRESS_FILE_TEST_003";
    std::string srcFile = "/tmp/in_comp3.txt";
    std::string compressFile = "/tmp/out_comp3.txt";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillRepeatedly(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    GTEST_LOG_(INFO) << "Test3. fread fail";
    compressMock_->maxSizeCache_[BLOCK_SIZE] = BLOCK_SIZE;
    EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(0));
    bool rs = TarFile::GetInstance().CompressFile(srcFile, compressFile);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end COMPRESS_FILE_TEST_003";
}

/**
 * @tc.number: COMPRESS_FILE_TEST_004
 * @tc.name: COMPRESS_FILE_TEST_004
 * @tc.desc: 测试 CompressFile 接口 调用CompressBuffer失败场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, COMPRESS_FILE_TEST_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin COMPRESS_FILE_TEST_004";
    std::string srcFile = "/tmp/in_comp4.txt";
    std::string compressFile = "/tmp/out_comp4.txt";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillRepeatedly(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    compressMock_->maxSizeCache_[BLOCK_SIZE] = BLOCK_SIZE;
    GTEST_LOG_(INFO) << "Test4. compress fail";
    EXPECT_CALL(*compressMock_, CompressBuffer(_, _)).WillOnce(Return(false));
    EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(BLOCK_SIZE));
    bool rs = TarFile::GetInstance().CompressFile(srcFile, compressFile);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end COMPRESS_FILE_TEST_004";
}

/**
 * @tc.number: COMPRESS_FILE_TEST_005
 * @tc.name: COMPRESS_FILE_TEST_005
 * @tc.desc: 测试 CompressFile 接口成功场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, COMPRESS_FILE_TEST_005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin COMPRESS_FILE_TEST_005";
    std::string srcFile = "/tmp/in_comp5.txt";
    std::string compressFile = "/tmp/out_comp5.txt";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillRepeatedly(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*compressMock_, CompressBuffer(_, _)).WillOnce(Return(true));
    compressMock_->maxSizeCache_[BLOCK_SIZE] = BLOCK_SIZE;
    GTEST_LOG_(INFO) << "Test5. compress success";
    EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(BLOCK_SIZE)).WillOnce(Return(0));
    size_t sizeCount = 1;
    EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(sizeCount)).WillOnce(Return(sizeCount))
        .WillOnce(Return(BLOCK_SIZE));
    bool rs = TarFile::GetInstance().CompressFile(srcFile, compressFile);
    EXPECT_TRUE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end COMPRESS_FILE_TEST_005";
}

/**
 * @tc.number: DECOMPRESS_FILE_TEST_001
 * @tc.name: DECOMPRESS_FILE_TEST_001
 * @tc.desc: 测试 DecompressFile 接口fopen失败的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, DECOMPRESS_FILE_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin DECOMPRESS_FILE_TEST_001";
    std::string srcFile = "/tmp/in_decomp1.txt";
    std::string compressFile = "/tmp/out_decomp1.txt";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    GTEST_LOG_(INFO) << "Test1. file null";
    bool rs = TarFile::GetInstance().DecompressFile(compressFile, srcFile);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end DECOMPRESS_FILE_TEST_001";
}

/**
 * @tc.number: DECOMPRESS_FILE_TEST_002
 * @tc.name: DECOMPRESS_FILE_TEST_002
 * @tc.desc: 测试 DecompressFile 接口fread失败的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, DECOMPRESS_FILE_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin DECOMPRESS_FILE_TEST_002";
    std::string srcFile = "/tmp/in_decomp2.txt";
    std::string compressFile = "/tmp/out_decomp2.txt";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillRepeatedly(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    GTEST_LOG_(INFO) << "Test2. fread fail";
    EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(0));
    bool rs = TarFile::GetInstance().DecompressFile(compressFile, srcFile);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end DECOMPRESS_FILE_TEST_002";
}

/**
 * @tc.number: DECOMPRESS_FILE_TEST_003
 * @tc.name: DECOMPRESS_FILE_TEST_003
 * @tc.desc: 测试 DecompressFile 接口fread size错误的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, DECOMPRESS_FILE_TEST_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin DECOMPRESS_FILE_TEST_003";
    std::string srcFile = "/tmp/in_decomp3.txt";
    std::string compressFile = "/tmp/out_decomp3.txt";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillRepeatedly(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    GTEST_LOG_(INFO) << "Test3. fread size wrong";
    EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(2));
    bool rs = TarFile::GetInstance().DecompressFile(compressFile, srcFile);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end DECOMPRESS_FILE_TEST_003";
}

/**
 * @tc.number: DECOMPRESS_FILE_TEST_004
 * @tc.name: DECOMPRESS_FILE_TEST_004
 * @tc.desc: 测试 DecompressFile 接口fread 第二次失败的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, DECOMPRESS_FILE_TEST_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin DECOMPRESS_FILE_TEST_004";
    std::string srcFile = "/tmp/in_decomp4.txt";
    std::string compressFile = "/tmp/out_decomp4.txt";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillRepeatedly(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    GTEST_LOG_(INFO) << "Test4. fread fail2";
    size_t sizeCount = 1;
    EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(sizeCount)).WillOnce(Return(0));
    bool rs = TarFile::GetInstance().DecompressFile(compressFile, srcFile);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end DECOMPRESS_FILE_TEST_004";
}

/**
 * @tc.number: DECOMPRESS_FILE_TEST_005
 * @tc.name: DECOMPRESS_FILE_TEST_005
 * @tc.desc: 测试 DecompressFile 接口decompress 失败的场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, DECOMPRESS_FILE_TEST_005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin DECOMPRESS_FILE_TEST_005";
    std::string srcFile = "/tmp/in_decomp5.txt";
    std::string compressFile = "/tmp/out_decomp5.txt";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillRepeatedly(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    GTEST_LOG_(INFO) << "Test5. decompress fail";
    size_t sizeCount = 1;
    EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(sizeCount)).WillOnce(Return(sizeCount))
        .WillOnce(Return(BLOCK_SIZE)).WillOnce(Return(0));
    EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(0));
    bool rs = TarFile::GetInstance().DecompressFile(compressFile, srcFile);
    EXPECT_FALSE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end DECOMPRESS_FILE_TEST_005";
}

/**
 * @tc.number: DECOMPRESS_FILE_TEST_006
 * @tc.name: DECOMPRESS_FILE_TEST_006
 * @tc.desc: 测试 DecompressFile 接口decompress成功场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, DECOMPRESS_FILE_TEST_006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin DECOMPRESS_FILE_TEST_006";
    std::string srcFile = "/tmp/in_decomp6.txt";
    std::string compressFile = "/tmp/out_decomp6.txt";
    EXPECT_CALL(*funcMock, fopen(_, _)).WillRepeatedly(Return(testFilePtr_));
    EXPECT_CALL(*funcMock, fclose(_)).WillRepeatedly(Return(0));
    GTEST_LOG_(INFO) << "Test6. decompress success";
    size_t sizeCount = 1;
    EXPECT_CALL(*funcMock, fread(_, _, _, _)).WillOnce(Return(sizeCount)).WillOnce(Return(sizeCount))
        .WillOnce(Return(BLOCK_SIZE)).WillOnce(Return(0));
    EXPECT_CALL(*funcMock, fwrite(_, _, _, _)).WillOnce(Return(BLOCK_SIZE));
    bool rs = TarFile::GetInstance().DecompressFile(compressFile, srcFile);
    EXPECT_TRUE(rs);
    GTEST_LOG_(INFO) << "TarFileTest-end DECOMPRESS_FILE_TEST_006";
}
#else
/**
 * @tc.number: COMPRESS_FILE_TEST_001
 * @tc.name: COMPRESS_FILE_TEST_001
 * @tc.desc: 测试 CompressFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, COMPRESS_FILE_TEST_DISABLE_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin COMPRESS_FILE_TEST_DISABLE_001";
    std::string srcFile = "/tmp/in_comp_disable.txt";
    std::string compressFile = "/tmp/out_comp_disable.txt";
    EXPECT_FALSE(TarFile::GetInstance().CompressFile(srcFile, compressFile));
    GTEST_LOG_(INFO) << "TarFileTest-end COMPRESS_FILE_TEST_DISABLE_001";
}

/**
 * @tc.number: DECOMPRESS_FILE_TEST_DISABLE_001
 * @tc.name: DECOMPRESS_FILE_TEST_DISABLE_001
 * @tc.desc: 测试 DecompressFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(TarFileSubTest, DECOMPRESS_FILE_TEST_DISABLE_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileTest-begin DECOMPRESS_FILE_TEST_DISABLE_001";
    std::string srcFile = "/tmp/in_decomp_disable.txt";
    std::string compressFile = "/tmp/out_decomp_disable.txt";
    EXPECT_FALSE(TarFile::GetInstance().DecompressFile(compressFile, srcFile));
    GTEST_LOG_(INFO) << "TarFileTest-end DECOMPRESS_FILE_TEST_DISABLE_001";
}
#endif
} // namespace OHOS::FileManagement::Backup