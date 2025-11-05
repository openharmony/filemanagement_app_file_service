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

#include <cstdio>
#include <cstdlib>

#include <dirent.h>
#include <fcntl.h>

#include <errors.h>
#include <file_ex.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

#include "b_filesystem/b_dir.h"
#include "b_process/b_process.h"
#include "test_manager.h"
#include "library_func_mock.h"

#include "library_func_define.h"
#include "b_dir.cpp"
#include "library_func_undef.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace OHOS::AppFileService;

class BDirSubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp()
    {
        std::queue<std::shared_ptr<IFileInfo>> emptyQueue;
        ScanFileSingleton::GetInstance().pendingFileQueue_.swap(emptyQueue);
        ScanFileSingleton::GetInstance().smallFiles_.clear();
    };
    void TearDown() {};
    static inline shared_ptr<LibraryFuncMock> funcMock_ = nullptr;
};

void BDirSubTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    funcMock_ = make_shared<LibraryFuncMock>();
    LibraryFuncMock::libraryFunc_ = funcMock_;
}

void BDirSubTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    LibraryFuncMock::libraryFunc_ = nullptr;
    funcMock_ = nullptr;
}

/**
 * @tc.number: B_DIR_ProcessFile_001
 * @tc.name: B_DIR_ProcessFile_001
 * @tc.desc: Test function of ProcessFile interface for FAIL
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BDirSubTest, B_DIR_ProcessFile_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BDirSubTest-begin B_DIR_ProcessFile_001";
    GTEST_LOG_(INFO) << "1. test check fail";
    EXPECT_CALL(*funcMock_, stat(_, _)).WillOnce(Return(-1));
    std::string backupPath = "test1";
    std::string restorePath = "restore1";
    int64_t smallFileSize = 0;
    int64_t bigFileSize = 0;
    ProcessFile(backupPath, restorePath, 0, smallFileSize, bigFileSize);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    EXPECT_EQ(ScanFileSingleton::GetInstance().smallFiles_.size(), 0);
    GTEST_LOG_(INFO) << "BDirTest-end B_DIR_ProcessFile_001";
}
} // namespace OHOS::FileManagement::Backup