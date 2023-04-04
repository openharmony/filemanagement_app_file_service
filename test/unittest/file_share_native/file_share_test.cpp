/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <cassert>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <singleton.h>
#include <string>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "accesstoken_kit.h"
#include "file_share.h"
#include "ipc_skeleton.h"

namespace {
    using namespace std;
    using namespace OHOS::AppFileService;
    using namespace OHOS::Security::AccessToken;

    const int E_OK = 0;
    const int E_INVALID_ARGUMENT = 12100002;

    class FileShareTest : public testing::Test {
    public:
        static void SetUpTestCase(void) {};
        static void TearDownTestCase() {};
        void SetUp() {};
        void TearDown() {};
    };

    /**
     * @tc.name: file_share_test_0000
     * @tc.desc: Test function of CreateShareFile() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(FileShareTest, File_share_CreateShareFile_0000, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileShareTest-begin File_share_CreateShareFile_0000";

        int32_t uid = -1;
        uid = OHOS::IPCSkeleton::GetCallingUid();

        string bundleNameA = "com.ohos.settingsdata";
        string fileStr = "/data/app/el2/" + to_string(uid) + "/base/" + bundleNameA + "/files/test.txt";
        int32_t fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
        ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";
        string uri = "file://" + bundleNameA + "/data/storage/el2/base/files/test.txt";

        string bundleNameB = "com.ohos.systemui";
        int32_t tokenId = AccessTokenKit::GetHapTokenID(uid, bundleNameB, 0);

        int32_t flag = 3;
        int32_t ret = FileShare::CreateShareFile(uri, tokenId, flag);
        EXPECT_EQ(ret, E_OK);
        GTEST_LOG_(INFO) << "FileShareTest-end File_share_CreateShareFile_0000";
    }

    /**
     * @tc.name: file_share_test_0001
     * @tc.desc: Test function of CreateShareFile() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(FileShareTest, File_share_CreateShareFile_0001, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileShareTest-begin File_share_CreateShareFile_0001";

        int32_t uid = -1;
        uid = OHOS::IPCSkeleton::GetCallingUid();

        string bundleNameA = "com.ohos.settingsdata";
        string fileStr = "/data/app/el2/" + to_string(uid) + "/base/" + bundleNameA + "/files/test.txt";
        int32_t fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
        ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";
        string uri = "file://" + bundleNameA + "/data/test/el2/base/files/test.txt";

        string bundleNameB = "com.ohos.systemui";
        int32_t tokenId = AccessTokenKit::GetHapTokenID(uid, bundleNameB, 0);

        int32_t flag = 3;
        int32_t ret = FileShare::CreateShareFile(uri, tokenId, flag);
        EXPECT_EQ(ret, -EINVAL);
        GTEST_LOG_(INFO) << "FileShareTest-end File_share_CreateShareFile_0001";
    }

    /**
     * @tc.name: file_share_test_0002
     * @tc.desc: Test function of CreateShareFile() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(FileShareTest, File_share_CreateShareFile_0002, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileShareTest-begin File_share_CreateShareFile_0002";
        int32_t uid = -1;
        uid = OHOS::IPCSkeleton::GetCallingUid();

        string bundleNameA = "com.ohos.settingsdata";
        string fileStr = "/data/app/el2/" + to_string(uid) + "/base/" + bundleNameA + "/files/test.txt";
        int32_t fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
        ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";
        string uri = "file://" + bundleNameA + "/data/storage/el2/base/files/test.txt";
        int32_t tokenId = 100;

        int32_t flag = 3;
        int32_t ret = FileShare::CreateShareFile(uri, tokenId, flag);
        EXPECT_EQ(ret, E_INVALID_ARGUMENT);
        GTEST_LOG_(INFO) << "FileShareTest-end File_share_CreateShareFile_0002";
    }

    /**
     * @tc.name: file_share_test_0003
     * @tc.desc: Test function of CreateShareFile() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(FileShareTest, File_share_CreateShareFile_0003, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileShareTest-begin File_share_CreateShareFile_0003";
        int32_t uid = -1;
        uid = OHOS::IPCSkeleton::GetCallingUid();

        string bundleNameA = "com.ohos.settingsdata";
        string fileStr = "/data/app/el2/" + to_string(uid) + "/base/" + bundleNameA + "/files/test.txt";
        int32_t fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
        ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";
        string uri = "";

        string bundleNameB = "com.ohos.systemui";
        int32_t tokenId = AccessTokenKit::GetHapTokenID(uid, bundleNameB, 0);

        int32_t flag = 3;
        int32_t ret = FileShare::CreateShareFile(uri, tokenId, flag);
        EXPECT_EQ(ret, -EINVAL);
        GTEST_LOG_(INFO) << "FileShareTest-end File_share_CreateShareFile_0003";
    }

    /**
     * @tc.name: file_share_test_0004
     * @tc.desc: Test function of CreateShareFile() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(FileShareTest, File_share_CreateShareFile_0004, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileShareTest-begin File_share_CreateShareFile_0004";
        int32_t uid = -1;
        uid = OHOS::IPCSkeleton::GetCallingUid();

        string bundleNameA = "com.ohos.settingsdata";
        string fileStr = "/data/app/el2/" + to_string(uid) + "/base/" + bundleNameA + "/files/test.txt";
        int32_t fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
        ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";
        string uri = "";

        string bundleNameB = "com.ohos.systemui";
        int32_t tokenId = AccessTokenKit::GetHapTokenID(uid, bundleNameB, 0);

        int32_t flag = 4;
        int32_t ret = FileShare::CreateShareFile(uri, tokenId, flag);
        EXPECT_EQ(ret, -EINVAL);
        GTEST_LOG_(INFO) << "FileShareTest-end File_share_CreateShareFile_0004";
    }

    /**
     * @tc.name: file_share_test_0005
     * @tc.desc: Test function of DeleteShareFile() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(FileShareTest, File_share_DeleteShareFile_0005, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileShareTest-begin File_share_DeleteShareFile_0005";
        int32_t uid = -1;
        uid = OHOS::IPCSkeleton::GetCallingUid();

        string bundleNameB = "com.ohos.systemui";
        int32_t tokenId = AccessTokenKit::GetHapTokenID(uid, bundleNameB, 0);
        vector<string> sharePathList;
        string bundleNameA = "com.ohos.settingsdata";
        string uri = "file://" + bundleNameA + "/data/storage/el2/base/files/test.txt";
        sharePathList.push_back(uri);
        int32_t ret = FileShare::DeleteShareFile(tokenId, sharePathList);
        EXPECT_EQ(ret, E_OK);
        GTEST_LOG_(INFO) << "FileShareTest-end File_share_DeleteShareFile_0005";
    }

    /**
     * @tc.name: file_share_test_0006
     * @tc.desc: Test function of DeleteShareFile() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(FileShareTest, File_share_DeleteShareFile_0006, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileShareTest-begin File_share_DeleteShareFile_0006";
        int32_t tokenId = 104;
        vector<string> sharePathList;
        string bundleNameA = "com.ohos.settingsdata";
        string uri = "file://" + bundleNameA + "/data/storage/el2/base/files/test.txt";
        sharePathList.push_back(uri);
        int32_t ret = FileShare::DeleteShareFile(tokenId, sharePathList);
        EXPECT_EQ(ret, -EINVAL);
        GTEST_LOG_(INFO) << "FileShareTest-end File_share_DeleteShareFile_0006";
    }
}