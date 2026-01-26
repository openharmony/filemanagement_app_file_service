/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

namespace OHOS::AppFileService::ModuleFileUri {
    string filePath = "/data/app/el2/100/base/com.example.fileshareb/files/";
class FileUriTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() {};
    void TearDown() {};
};

void FileUriTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    string cmdMkdir = string("mkdir -p ") + filePath;
    system(cmdMkdir.c_str());
    string touchFile = string("touch ") + filePath + "test.txt";
    system(touchFile.c_str());
}

void FileUriTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    string rmDir = string("rm -r ") + filePath;
    system(rmDir.c_str());
}

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

    string fileStr2 = "/data/storage/el2/base/files/test.txt/";
    string uri2 = "file://" + BUNDLE_A + fileStr;
    FileUri fileUri2(fileStr2);
    EXPECT_EQ(fileUri2.GetName(), "");
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

    string uri4 = "file://media/Photo/12/IMG_12345_999999";
    FileUri fileUri4(uri4);
    EXPECT_EQ(fileUri2.GetPath(), "/Photo/12/IMG_12345_999999");
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
    EXPECT_EQ(path, fileStrRealPath);
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
    string rltStr = "/storage/Users/currentUser/appdata/el2/base/" + bundleB + "/files/test.txt";
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
    string bundleB = "com.example.fileshareb";
    string actStr = "/data/storage/el2/base/files/test.txt";
    string uri = "file://" + bundleB + actStr;

    string rltStr = "/storage/Users/currentUser/appdata/el2/base/" + bundleB + "/files/test.txt";
    FileUri fileUri(uri);
    string path = fileUri.GetRealPath();
    EXPECT_EQ(path, rltStr);

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
    * @tc.desc: Test function of GetPath() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPath_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0005";
    string fileStr = "/data/storage/el2/media/Photo/12/IMG_12345_999999/test.jpg";
    string uri = "file://media/Photo/12/IMG_12345_999999/test.jpg";
    FileUri fileUri(uri);
    EXPECT_EQ(fileUri.GetRealPath(), fileStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0005";
}
/**
    * @tc.name: file_uri_test_0008
    * @tc.desc: Test function of GetPath() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPath_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0006";
    string fileStr = "file://docs/storage/Users/currentUser/Documents/1.txt?networkid=xxx";
    string rltStr = "/data/storage/el2/share/r/xxx/docs/storage/Users/currentUser/Documents/1.txt";
    FileUri fileUri(fileStr);
    string path = fileUri.GetRealPath();
    EXPECT_EQ(path, rltStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0006";
}
/**
    * @tc.name: file_uri_test_0009
    * @tc.desc: Test function of GetPath() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPath_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0007";
    string fileStr = "/mnt/data/fuse/1.txt";
    string bundleB = "com.ohos.dlpmanager";
    string uri = "file://" + bundleB + fileStr;
    string rltStr = "/mnt/data/fuse/1.txt";
    FileUri fileUri(uri);
    string path = fileUri.GetRealPath();
    EXPECT_EQ(path, rltStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0007";
}
/**
    * @tc.name: file_uri_test_00010
    * @tc.desc: Test function of GetPath() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPath_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0008";
    string fileStr = "/data/appdata/el2/base/files/test.txt";
    string bundleB = "com.demo.b";
    string uri = "file://" + bundleB + fileStr;
    string rltStr = "";
    FileUri fileUri(uri);
    string path = fileUri.GetRealPath();
    EXPECT_EQ(path, rltStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0008";
}

/**
    * @tc.name: file_uri_test_00011
    * @tc.desc: Test function of GetPath() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPath_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0009";
    string uri = "file://docs/storage/Users/currentUser/Documents/1.txt?networkid=***";
    string fileStr = "/data/storage/el2/share/r/***/docs/storage/Users/currentUser/Documents/1.txt";
    FileUri fileUri(uri);
    EXPECT_EQ(fileUri.GetRealPath(), fileStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0009";
}
/**
    * @tc.name: file_uri_test_00012
    * @tc.desc: Test function of GetPath() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPath_0010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0010";
    string uri = "file://docs/storage/Users/currentUser/Documents/1.txt?networkid=";
    string fileStr = "/data/storage/el2/share/r/docs/storage/Users/currentUser/Documents/1.txt";
    FileUri fileUri(uri);
    EXPECT_EQ(fileUri.GetRealPath(), fileStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPath_0010";
}

/**
    * @tc.name: file_uri_test_0000
    * @tc.desc: Test function of GetPathBySA() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPathBySA_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0000";
    string fileStr = "/data/storage/el2/base/files/test.txt";
    string bundleB = "com.demo.b";
    string uri = "file://" + bundleB + fileStr;
    string rltStr = "/storage/Users/currentUser/appdata/el2/base/" + bundleB + "/files/test.txt";
    FileUri fileUri(uri);
    string path = fileUri.GetRealPathBySA("");
    EXPECT_EQ(path, rltStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0000";
}

/**
    * @tc.name: file_uri_test_0001
    * @tc.desc: Test function of GetPathBySA() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPathBySA_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0001";
    string fileStr = "/data/storage/el2/base/files/test.txt";
    string bundleB = "com.demo.b";
    string uri = "file://" + bundleB + fileStr;
    string rltStr = "/data/storage/el2/base/files/test.txt";
    FileUri fileUri(uri);
    string path = fileUri.GetRealPathBySA("com.demo.b");
    EXPECT_EQ(path, rltStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0001";
}

/**
    * @tc.name: file_uri_test_0002
    * @tc.desc: Test function of GetPathBySA() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPathBySA_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0002";
    string fileStr = "/storage/Users/currentUser/Documents/1.txt";
    string bundleB = "docs";
    string uri = "file://" + bundleB + fileStr;
    string rltStr = "/storage/Users/currentUser/Documents/1.txt";
    FileUri fileUri(uri);
    string path = fileUri.GetRealPathBySA("");
    EXPECT_EQ(path, rltStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0002";
}
    
/**
    * @tc.name: file_uri_test_0003
    * @tc.desc: Test function of GetPathBySA() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPathBySA_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0003";
    string fileStr = "/storage/Users/currentUser/Documents/1.txt";
    string bundleB = "docs";
    string uri = "file://" + bundleB + fileStr;
    string rltStr = "/storage/Users/currentUser/Documents/1.txt";
    FileUri fileUri(uri);
    string path = fileUri.GetRealPathBySA("com.demo.b");
    EXPECT_EQ(path, rltStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0003";
}
    
/**
    * @tc.name: file_uri_test_0004
    * @tc.desc: Test function of GetPathBySA() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPathBySA_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0004";
    string fileStr = "/mnt/data/fuse/1.txt";
    string bundleB = "com.ohos.dlpmanager";
    string uri = "file://" + bundleB + fileStr;
    string rltStr = "/mnt/data/fuse/1.txt";
    FileUri fileUri(uri);
    string path = fileUri.GetRealPathBySA("");
    EXPECT_EQ(path, rltStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0004";
}

/**
    * @tc.name: file_uri_test_0005
    * @tc.desc: Test function of GetPathBySA() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPathBySA_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0005";
    string fileStr = "/data/storage/el2/base/files/test.txt";
    string bundleB = "com.ohos.dlpmanager";
    string uri = "file://" + bundleB + fileStr;
    string rltStr = "/storage/Users/currentUser/appdata/el2/base/" + bundleB + "/files/test.txt";
    FileUri fileUri(uri);
    string path = fileUri.GetRealPathBySA("");
    EXPECT_EQ(path, rltStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0005";
}

/**
    * @tc.name: file_uri_test_0006
    * @tc.desc: Test function of GetPathBySA() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPathBySA_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0006";
    string fileStr = "/data/data/el2/base/files/test.txt";
    string bundleB = "com.ohos.dlpmanager";
    string uri = "file://" + bundleB + fileStr;
    string rltStr = "";
    FileUri fileUri(uri);
    string path = fileUri.GetRealPathBySA("");
    EXPECT_EQ(path, rltStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0006";
}

/**
    * @tc.name: file_uri_GetPathBySA_0007
    * @tc.desc: Test function of GetPath) interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPathBySA_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0007";
    string fileStr = "";
    string uri = "file://com.example.demoa/data/storage/el2/base%()test.jpg";
    FileUri fileUri(uri);
    EXPECT_EQ(fileUri.GetRealPathBySA(), fileStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0007";
}
/**
    * @tc.name: file_uri_GetPathBySA_0008
    * @tc.desc: Test function of GetPath() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPathBySA_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0008";
    string fileStr = "";
    string uri = "file://com.example.demoa/data/storage/el2/base/files/%你好test.jpg";
    FileUri fileUri(uri);
    EXPECT_EQ(fileUri.GetRealPathBySA(), fileStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0008";
}

/**
    * @tc.name: file_uri_GetPathBySA_0009
    * @tc.desc: Test function of GetPath() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPathBySA_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0009";
    string fileStr = "";
    string uri = "file://com.example.demoa/data/storage/el2%/base/files/test.jpg";
    FileUri fileUri(uri);
    EXPECT_EQ(fileUri.GetRealPathBySA(), fileStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0009";
}
/**
    * @tc.name: file_uri_GetPathBySA_0010
    * @tc.desc: Test function of GetPath() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require: I7LW57
*/
HWTEST_F(FileUriTest, File_uri_GetPathBySA_0010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0010";
    string fileStr = "";
    string uri = "file://com.example.demoa/data/storage/el2/base%#files/test.jpg";
    FileUri fileUri(uri);
    EXPECT_EQ(fileUri.GetRealPathBySA(), fileStr);
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetPathBySA_0010";
}
/**
    * @tc.name: File_uri_GetFullDirectoryUri_0000
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
    * @tc.name: file_uri_test_0008
    * @tc.desc: Test function of GetFullDirectoryUri() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require:
*/
HWTEST_F(FileUriTest, File_uri_GetFullDirectoryUri_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetFullDirectoryUri_0001";
    std::string fileUri0 = "file://docs/storage/Users/currentUser/../test../test1";
    std::string fileUri1 = "file://docs/storage/Users/currentUser/test../../test";
    std::string fileUri2 = "file://docs/storage/Users/currentUser/test../../";
    std::string fileUri3 = "file://docs/storage/Users/currentUser/test../test../..";
    std::string fileUri4 = "file://docs/storage/Users/currentUser/test/..test/..";
    FileUri fileUriObject0(fileUri0);
    EXPECT_EQ(fileUriObject0.GetFullDirectoryUri(), "");
    FileUri fileUriObject1(fileUri1);
    EXPECT_EQ(fileUriObject1.GetFullDirectoryUri(), "");
    FileUri fileUriObject2(fileUri2);
    EXPECT_EQ(fileUriObject2.GetFullDirectoryUri(), "");
    FileUri fileUriObject3(fileUri3);
    EXPECT_EQ(fileUriObject3.GetFullDirectoryUri(), "");
    FileUri fileUriObject4(fileUri4);
    EXPECT_EQ(fileUriObject4.GetFullDirectoryUri(), "");
    GTEST_LOG_(INFO) << "FileUriTest-end File_uri_GetPath_0001";
}

/**
    * @tc.name: file_uri_test_0009
    * @tc.desc: Test function of CheckUriFormat() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require:
*/
HWTEST_F(FileUriTest, File_uri_CheckUriFormat_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_CheckUriFormat_0000";
    std::string fileUri0 = "file://docs/storage/Users/currentUser/../test../test1";
    std::string fileUri1 = "file://docs/storage/Users/currentUser/test../../test";
    std::string fileUri2 = "file://docs/storage/Users/currentUser/test../../";
    std::string fileUri3 = "file://docs/storage/Users/currentUser/test../test../..";
    std::string fileUri4 = "file://docs/storage/Users/currentUser/test/..test/..";
    std::string fileUri5 = "file://docs/storage/Users/currentUser/test/test../test";
    std::string fileUri6 = "file://docs/storage/Users/currentUser/test../test../test";
    std::string fileUri7 = "file://docs/storage/Users/currentUser/test../test../test../..test";
    FileUri fileUriObject0(fileUri0);
    EXPECT_FALSE(fileUriObject0.CheckUriFormat(fileUri0));
    FileUri fileUriObject1(fileUri1);
    EXPECT_FALSE(fileUriObject1.CheckUriFormat(fileUri1));
    FileUri fileUriObject2(fileUri2);
    EXPECT_FALSE(fileUriObject2.CheckUriFormat(fileUri2));
    FileUri fileUriObject3(fileUri3);
    EXPECT_FALSE(fileUriObject3.CheckUriFormat(fileUri3));
    FileUri fileUriObject4(fileUri4);
    EXPECT_FALSE(fileUriObject4.CheckUriFormat(fileUri4));
    FileUri fileUriObject5(fileUri5);
    EXPECT_TRUE(fileUriObject5.CheckUriFormat(fileUri5));
    FileUri fileUriObject6(fileUri6);
    EXPECT_TRUE(fileUriObject6.CheckUriFormat(fileUri6));
    FileUri fileUriObject7(fileUri7);
    EXPECT_TRUE(fileUriObject7.CheckUriFormat(fileUri7));
    GTEST_LOG_(INFO) << "FileUriTest-end File_uri_CheckUriFormat_0000";
}

/**
    * @tc.name: file_uri_test_0010
    * @tc.desc: Test function of GetUriFromPath() interface for SUCCESS.
    * @tc.size: MEDIUM
    * @tc.type: FUNC
    * @tc.level Level 1
    * @tc.require:
*/
HWTEST_F(FileUriTest, File_uri_GetUriFromPath_0010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileUriTest-begin File_uri_GetUriFromPath_0010";
    std::string fileUri0 = "/storage/Users/currentUser/../test../test1";
    std::string fileUri1 = "/storage/Users/currentUser/test../../test";
    std::string fileUri2 = "/storage/Users/currentUser/test../../";
    std::string fileUri3 = "/storage/Users/currentUser/test../test../..";
    std::string fileUri4 = "/storage/Users/currentUser/test/..test/..";
    std::string fileUri5 = "/storage/Users/currentUser/test/test../test";
    std::string fileUri6 = "/storage/Users/currentUser/test../test../test";
    std::string fileUri7 = "/storage/Users/currentUser/test../test../test../..test";
    FileUri fileUriObject0(fileUri0);
    EXPECT_EQ(fileUriObject0.ToString(), "");
    FileUri fileUriObject1(fileUri1);
    EXPECT_EQ(fileUriObject1.ToString(), "");
    FileUri fileUriObject2(fileUri2);
    EXPECT_EQ(fileUriObject2.ToString(), "");
    FileUri fileUriObject3(fileUri3);
    EXPECT_EQ(fileUriObject3.ToString(), "");
    FileUri fileUriObject4(fileUri4);
    EXPECT_EQ(fileUriObject4.ToString(), "");
    FileUri fileUriObject5(fileUri5);
    EXPECT_EQ(fileUriObject5.ToString(), "file://docs/storage/Users/currentUser/test/test../test");
    FileUri fileUriObject6(fileUri6);
    EXPECT_EQ(fileUriObject6.ToString(), "file://docs/storage/Users/currentUser/test../test../test");
    FileUri fileUriObject7(fileUri7);
    EXPECT_EQ(fileUriObject7.ToString(), "file://docs/storage/Users/currentUser/test../test../test../..test");
    GTEST_LOG_(INFO) << "FileUriTest-end File_uri_GetUriFromPath_0010";
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