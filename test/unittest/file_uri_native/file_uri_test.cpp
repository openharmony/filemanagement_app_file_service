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

#include "file_uri.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <fcntl.h>

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "parameter.h"
#include "uri.h"

#include "common_func.h"
#include "file_share.h"
#include "log.h"

using namespace std;
using namespace OHOS::Security::AccessToken;
using namespace OHOS::AppFileService;

namespace {
    const string BUNDLE_A = "com.example.filesharea";
    const string FULL_MOUNT_ENABLE_PARAMETER = "const.filemanager.full_mount.enable";
}

string CommonFunc::GetSelfBundleName()
{
    return BUNDLE_A;
}

static bool CheckFileManagerFullMountEnable()
{
    char value[] = "false";
    int retSystem = GetParameter(FULL_MOUNT_ENABLE_PARAMETER.c_str(), "false", value, sizeof(value));
    if ((retSystem > 0) && (string_view(value) == "true")) {
        return true;
    }
    LOGE("Not supporting all mounts");
    return false;
}

namespace OHOS::AppFileService::ModuleFileUri {
    const string PATH_SHARE = "/data/storage/el2/share";
    const string MODE_RW = "/rw/";
    const string MODE_R = "/r/";
    const int E_OK = 0;

    class FileUriTest : public testing::Test {
    public:
        static void SetUpTestCase(void) {};
        static void TearDownTestCase() {};
        void SetUp() {};
        void TearDown() {};
    };

    /**
     * @tc.name: file_uri_test_0000
     * @tc.desc: Test function of ToString() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7LW57
     */
    HWTEST_F(FileUriTest, File_uri_ToString_0000, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_ToString_0000";

        string fileStr = "/data/storage/el2/base/files/test.txt";
        string uri = "file://" + BUNDLE_A + fileStr;
        FileUri fileUri(fileStr);
        EXPECT_EQ(fileUri.ToString(), uri);

        FileUri fileUri2(uri);
        EXPECT_EQ(fileUri2.ToString(), uri);
        GTEST_LOG_(INFO) << "FileUriTest-end File_uri_ToString_0000";
    }

    /**
     * @tc.name: file_uri_test_0001
     * @tc.desc: Test function of GetName() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7LW57
     */
    HWTEST_F(FileUriTest, File_uri_GetName_0000, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetName_0000";

        string fileStr = "/data/storage/el2/base/files/test.txt";
        string uri = "file://" + BUNDLE_A + fileStr;
        FileUri fileUri(fileStr);
        string name = fileUri.GetName();
        EXPECT_EQ(name, "test.txt");
        GTEST_LOG_(INFO) << "FileUriTest-end File_uri_GetName_0000";
    }

    /**
     * @tc.name: file_uri_test_0002
     * @tc.desc: Test function of GetPath() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7LW57
     */
    HWTEST_F(FileUriTest, File_uri_GetPath_0000, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0000";
        string fileStr = "/data/storage/el2/base/files/test.txt";
        string uri = "file://" + BUNDLE_A + fileStr;
        FileUri fileUri(uri);
        string path = fileUri.GetPath();
        EXPECT_EQ(path, fileStr);

        string uri2 = "file://media/Photo/12/IMG_12345_999999/test.jpg";
        FileUri fileUri2(uri2);
        path = fileUri2.GetPath();
        EXPECT_EQ(path, "/Photo/12/IMG_12345_999999");
        GTEST_LOG_(INFO) << "FileUriTest-end File_uri_GetPath_0000";
    }

    /**
     * @tc.name: file_uri_test_0003
     * @tc.desc: Test function of GetPath() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7LW57
     */
    HWTEST_F(FileUriTest, File_uri_GetPath_0001, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0001";
        string fileStr = "/Documents/test.txt";
        string uri = "file://" + BUNDLE_A + fileStr;
        FileUri fileUri(uri);
        string path = fileUri.GetRealPath();
        EXPECT_EQ(path, fileStr);

        string fileStrPath = "docs/storage/Users/currentUser/Documents/1.txt";
        string fileStrRealPath = "/storage/Users/currentUser/Documents/1.txt";
        string uri2 = "file://" + fileStrPath;
        FileUri fileUri2(uri2);
        path.clear();
        path = fileUri2.GetRealPath();
        if (CheckFileManagerFullMountEnable()) {
            EXPECT_EQ(path, fileStrRealPath);
        } else {
            EXPECT_EQ(path, PATH_SHARE + MODE_R + fileStrPath);
        };
        GTEST_LOG_(INFO) << "FileUriTest-end File_uri_GetPath_0001";
    }

    /**
     * @tc.name: file_uri_test_0004
     * @tc.desc: Test function of GetPath() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7LW57
     */
    HWTEST_F(FileUriTest, File_uri_GetPath_0002, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0002";
        string fileStr = "/data/storage/el2/base/files/test.txt";
        string bundleB = "com.demo.b";
        string uri = "file://" + bundleB + fileStr;
        string rltStr = PATH_SHARE + MODE_R + bundleB + fileStr;
        FileUri fileUri(uri);
        string path = fileUri.GetRealPath();
        EXPECT_EQ(path, rltStr);
        GTEST_LOG_(INFO) << "FileUriTest-end File_uri_GetPath_0002";
    }

    /**
     * @tc.name: file_uri_test_0005
     * @tc.desc: Test function of GetPath() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7LW57
     */
    HWTEST_F(FileUriTest, File_uri_GetPath_0003, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0003";
        int32_t uid = 100;
        string bundleB = "com.example.fileshareb";
        string fileStr = "/data/app/el2/" + to_string(uid) + "/base/" + bundleB + "/files/test.txt";
        int32_t fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
        ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";

        string actStr = "/data/storage/el2/base/files/test.txt";
        string uri = "file://" + bundleB + actStr;
        uint32_t tokenId = AccessTokenKit::GetHapTokenID(uid, BUNDLE_A, 0);

        int32_t flag = 3;
        vector<string> uriList(1, uri);
        vector<int32_t> retList;
        int32_t ret = FileShare::CreateShareFile(uriList, tokenId, flag, retList);
        EXPECT_EQ(ret, E_OK);

        string rltStr = PATH_SHARE + MODE_R + bundleB + actStr;
        FileUri fileUri(uri);
        string path = fileUri.GetRealPath();
        EXPECT_EQ(path, rltStr);

        vector<string> sharePathList;
        sharePathList.push_back(uri);
        ret = FileShare::DeleteShareFile(tokenId, sharePathList);
        EXPECT_EQ(ret, E_OK);
        close(fd);
        GTEST_LOG_(INFO) << "FileUriTest-end File_uri_GetPath_0003";
    }

    /**
     * @tc.name: file_uri_test_0006
     * @tc.desc: Test function of GetPath() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7LW57
     */
    HWTEST_F(FileUriTest, File_uri_GetPath_0004, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0004";
        string fileStr = "/data/storage/el2/base/files/test.txt";
        string uri = "file://" + fileStr;
        FileUri fileUri(uri);
        EXPECT_EQ(fileUri.ToString(), uri);
        EXPECT_EQ(fileUri.GetName(), "test.txt");
        EXPECT_EQ(fileUri.GetPath(), fileStr);
        EXPECT_EQ(fileUri.GetRealPath(), fileStr);
        GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0004";
    }

    /**
     * @tc.name: file_uri_test_0007
     * @tc.desc: Test function of GetFullDirectoryUri() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require:
     */
    HWTEST_F(FileUriTest, File_uri_GetFullDirectoryUri_0000, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetFullDirectoryUri_0000";
        string fileStr = "/data/test/remote_file_share_test.txt";
        FileUri fileUriObject(fileStr);
        string fileDirectoryUri = "file://" + BUNDLE_A + "/data/test";
        EXPECT_EQ(fileUriObject.GetFullDirectoryUri(), fileDirectoryUri);
        string folderStr = "/data/test";
        string folderUri = "file://" + BUNDLE_A + folderStr;
        FileUri folderUriObject(folderUri);
        string folderDirectoryUri = "file://" + BUNDLE_A + folderStr;
        EXPECT_EQ(folderUriObject.GetFullDirectoryUri(), folderDirectoryUri);
        GTEST_LOG_(INFO) << "FileUriTest-end File_uri_GetPath_0000";
    }

    /**
     * @tc.name: File_uri_IsRemoteUri_0000
     * @tc.desc: Test function of IsRemoteUri() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require:
     */
    HWTEST_F(FileUriTest, File_uri_IsRemoteUri_0000, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetFullDirectoryUri_0000";
        string fileStr = "file://" + BUNDLE_A + "/data/test";
        FileUri fileUriObject(fileStr);
        EXPECT_EQ(fileUriObject.IsRemoteUri(), false);

        string fileStr2 = "file://" + BUNDLE_A + "/data/test?networkid=";
        FileUri fileUriObject2(fileStr2);
        EXPECT_EQ(fileUriObject2.IsRemoteUri(), false);

        string fileStr3 = "file://" + BUNDLE_A + "/data/test?networkid=123456/";
        FileUri fileUriObject3(fileStr3);
        EXPECT_EQ(fileUriObject3.IsRemoteUri(), false);

        string fileStr4 = "file://" + BUNDLE_A + "/data/test?networkid=123456";
        FileUri fileUriObject4(fileStr4);
        EXPECT_EQ(fileUriObject4.IsRemoteUri(), true);
        GTEST_LOG_(INFO) << "FileUriTest-end File_uri_GetPath_0000";
    }
}