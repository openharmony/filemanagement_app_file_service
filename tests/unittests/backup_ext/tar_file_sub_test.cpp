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
 * @tc.number: SUB_Tar_File_CopyData_0100
 * @tc.name: SUB_Tar_File_CopyData_0100
 * @tc.desc: 测试 CopyData 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(TarFileSubTest, SUB_Tar_File_CopyData_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TarFileSubTest-begin SUB_Tar_File_CopyData_0100";
    try {
        TarHeader hdr;
        string mode;
        string uid;
        string gid;
        string size;

        EXPECT_CALL(*funcMock, memcpy_s(_, _, _, _)).WillOnce(Return(-1));
        auto ret = CopyData(hdr, mode, uid, gid, size);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*funcMock, memcpy_s(_, _, _, _)).WillOnce(Return(EOK)).WillOnce(Return(-1));
        ret = CopyData(hdr, mode, uid, gid, size);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*funcMock, memcpy_s(_, _, _, _)).WillOnce(Return(EOK)).WillOnce(Return(EOK)).WillOnce(Return(-1));
        ret = CopyData(hdr, mode, uid, gid, size);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*funcMock, memcpy_s(_, _, _, _)).WillOnce(Return(EOK)).WillOnce(Return(EOK)).WillOnce(Return(EOK))
            .WillOnce(Return(-1));
        ret = CopyData(hdr, mode, uid, gid, size);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*funcMock, memcpy_s(_, _, _, _)).WillOnce(Return(EOK)).WillOnce(Return(EOK)).WillOnce(Return(EOK))
            .WillOnce(Return(0));
        ret = CopyData(hdr, mode, uid, gid, size);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "TarFileSubTest-an exception occurred by TarFile.";
    }
    GTEST_LOG_(INFO) << "TarFileSubTest-end SUB_Tar_File_CopyData_0100";
}
} // namespace OHOS::FileManagement::Backup