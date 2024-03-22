/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <singleton.h>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

#include "remote_file_share.h"

namespace {
    using namespace std;
    using namespace OHOS::AppFileService::ModuleRemoteFileShare;

    const int E_INVALID_ARGUMENT = 22;
    const int E_OK = 0;

    class RemoteFileShareTest : public testing::Test {
    public:
        static void SetUpTestCase(void) {};
        static void TearDownTestCase() {};
        void SetUp() {};
        void TearDown() {};
    };

    /**
     * @tc.name: remote_file_share_test_0000
     * @tc.desc: Test function of RemoteFileShare() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_RemoteFileShare_0000, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin Remote_file_share_RemoteFileShare_0000";
        RemoteFileShare* test = new RemoteFileShare;
        ASSERT_TRUE(test != nullptr) << "RemoteFileShare Construct Failed!";
        delete test;
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_RemoteFileShare_0000";
    }

    /**
     * @tc.name: remote_file_share_test_0001
     * @tc.desc: Test function of CreateSharePath() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_CreateSharePath_0001, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin Remote_file_share_CreateSharePath_0001";
        const int fd = -1;
        const int userId = 100;
        const string deviceId = "0";
        string sharePath = "";
        int ret = RemoteFileShare::CreateSharePath(fd, sharePath, userId, deviceId);
        EXPECT_EQ(ret, E_INVALID_ARGUMENT);
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_CreateSharePath_0001";
    }

    /**
     * @tc.name: remote_file_share_test_0002
     * @tc.desc: Test function of CreateSharePath() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_CreateSharePath_0002, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin Remote_file_share_CreateSharePath_0002";
        const int fd = 10;
        const int userId = 90;
        const string deviceId = "0";
        string sharePath = "";
        int ret = RemoteFileShare::CreateSharePath(fd, sharePath, userId, deviceId);
        EXPECT_EQ(ret, E_INVALID_ARGUMENT);
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_CreateSharePath_0002";
    }

    /**
     * @tc.name: remote_file_share_test_0003
     * @tc.desc: Test function of CreateSharePath() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_CreateSharePath_0003, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin  Remote_file_share_CreateSharePath_0003";
        const int fd = 10;
        const int userId = 100;
        const string deviceId = "00";
        string sharePath = "";
        int ret = RemoteFileShare::CreateSharePath(fd, sharePath, userId, deviceId);
        EXPECT_EQ(ret, E_INVALID_ARGUMENT);
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_CreateSharePath_0003";
    }

    /**
     * @tc.name: remote_file_share_test_0004
     * @tc.desc: Test function of CreateSharePath() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_CreateSharePath_0004, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin  Remote_file_share_CreateSharePath_0004";
        const string fileStr = "/data/test/remote_file_share_test.txt";
        int fd = open(fileStr.c_str(), O_RDWR);
        ASSERT_TRUE(fd != -1) << "RemoteFileShareTest Create File Failed!";
        const int userId = 100;
        const string deviceId = "0";
        string sharePath = "";
        int ret = RemoteFileShare::CreateSharePath(fd, sharePath, userId, deviceId);
        close(fd);
        EXPECT_EQ(ret, E_OK);
        GTEST_LOG_(INFO) << "RemoteFileShareTest Create Share Path " << sharePath;
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_CreateSharePath_0004";
    }

    /**
     * @tc.name: remote_file_share_test_0005
     * @tc.desc: Test function of CreateSharePath() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: SR000H63TL
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_CreateSharePath_0005, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin  Remote_file_share_CreateSharePath_0005";
        const string fileStr = "/data/test/remote_file_share_test.txt";
        int fd = open(fileStr.c_str(), O_RDWR);
        ASSERT_TRUE(fd != -1) << "RemoteFileShareTest Create File Failed!";
        const int userId = 100;
        const string deviceId = "0";
        string sharePath = "";
        char pthreadName[PATH_MAX];
        int ret = pthread_getname_np(pthread_self(), pthreadName, sizeof(pthreadName));
        EXPECT_EQ(ret, E_OK);
        string pthreadNameStr = pthreadName;
        string errPthreadName = "../test";
        ret = pthread_setname_np(pthread_self(), errPthreadName.c_str());
        EXPECT_EQ(ret, E_OK);
        ret = RemoteFileShare::CreateSharePath(fd, sharePath, userId, deviceId);
        close(fd);
        EXPECT_NE(ret, E_OK);
        ret = pthread_setname_np(pthread_self(), pthreadNameStr.c_str());
        EXPECT_EQ(ret, E_OK);
        GTEST_LOG_(INFO) << "RemoteFileShareTest Create Share Path " << sharePath;
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_CreateSharePath_0005";
    }

    /**
     * @tc.name: remote_file_share_test_0005
     * @tc.desc: Test function of GetDfsUriFromLocal() interface for SUCCESS.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7KDF7
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_GetDfsUriFromLocal_0005, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin  Remote_file_share_GetDfsUriFromLocal_0005";
        const string dirPath = "/data/app/el2/100/base/com.demo.a/";
        const string uriStr = "file://com.demo.a/data/storage/el2/base/remote_file_share_test.txt";
        const string fileStr = "/data/app/el2/100/base/com.demo.a/remote_file_share_test.txt";
        const int userId = 100;

        int ret = mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IXOTH);
        ASSERT_TRUE((ret != -1) || (ret == -1 && errno == EEXIST)) << "RemoteFileShareTest mkdir failed! " << errno;

        int fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
        ASSERT_TRUE(fd != -1) << "Failed to open file in RemoteFileShareTest! " << errno;
        close(fd);

        HmdfsUriInfo hui;
        ret = RemoteFileShare::GetDfsUriFromLocal(uriStr, userId, hui);
        EXPECT_EQ(ret, E_OK);

        ret = unlink(fileStr.c_str());
        ASSERT_TRUE(ret != -1) << "Failed to delete file in RemoteFileShareTest! " << errno;

        ret = rmdir(dirPath.c_str());
        ASSERT_TRUE(ret != -1) << "RemoteFileShareTest rmdir failed! " << errno;
        GTEST_LOG_(INFO) << "RemoteFileShareTest uri is " << hui.uriStr;
        GTEST_LOG_(INFO) << "RemoteFileShareTest file size is " << hui.fileSize;
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_GetDfsUriFromLocal_0005";
    }

    /**
     * @tc.name: remote_file_share_test_0006
     * @tc.desc: Test function of GetDfsUriFromLocal() interface for FAILURE.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7KDF7
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_GetDfsUriFromLocal_0006, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin  Remote_file_share_GetDfsUriFromLocal_0006";
        const string uriStr = "";
        const int userId = 100;
        HmdfsUriInfo hui;
        int ret = RemoteFileShare::GetDfsUriFromLocal(uriStr, userId, hui);
        EXPECT_EQ(ret, -EINVAL);
        GTEST_LOG_(INFO) << "RemoteFileShareTest uri is " << hui.uriStr;
        GTEST_LOG_(INFO) << "RemoteFileShareTest file size is " << hui.fileSize;
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_GetDfsUriFromLocal_0006";
    }

    /**
     * @tc.name: remote_file_share_test_0007
     * @tc.desc: Test function of GetDfsUriFromLocal() interface for FAILURE.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7KDF7
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_GetDfsUriFromLocal_0007, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin  Remote_file_share_GetDfsUriFromLocal_0007";
        const string uriStr = "file://com.demo.a/./data/storage/el2/base/remote_file_share_test.txt";
        const int userId = 100;
        HmdfsUriInfo hui;
        int ret = RemoteFileShare::GetDfsUriFromLocal(uriStr, userId, hui);
        EXPECT_EQ(ret, -EINVAL);
        GTEST_LOG_(INFO) << "RemoteFileShareTest uri is " << hui.uriStr;
        GTEST_LOG_(INFO) << "RemoteFileShareTest file size is " << hui.fileSize;
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_GetDfsUriFromLocal_0007";
    }

    /**
     * @tc.name: remote_file_share_test_0008
     * @tc.desc: Test function of GetDfsUriFromLocal() interface for FAILURE.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7KDF7
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_GetDfsUriFromLocal_0008, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin  Remote_file_share_GetDfsUriFromLocal_0008";
        const string uriStr = "file://com.demo.a/../data/storage/el2/base/remote_file_share_test.txt";
        const int userId = 100;
        HmdfsUriInfo hui;
        int ret = RemoteFileShare::GetDfsUriFromLocal(uriStr, userId, hui);
        EXPECT_EQ(ret, -EINVAL);
        GTEST_LOG_(INFO) << "RemoteFileShareTest uri is " << hui.uriStr;
        GTEST_LOG_(INFO) << "RemoteFileShareTest file size is " << hui.fileSize;
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_GetDfsUriFromLocal_0008";
    }

     /**
     * @tc.name: remote_file_share_test_0009
     * @tc.desc: Test function of GetDfsUriFromLocal() interface for FAILURE.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7KDF7
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_GetDfsUriFromLocal_0009, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin  Remote_file_share_GetDfsUriFromLocal_0009";
        const string uriStr = "file://com.demo.a/data/storage/el3/base/remote_file_share_test.txt";
        const int userId = 100;
        HmdfsUriInfo hui;
        int ret = RemoteFileShare::GetDfsUriFromLocal(uriStr, userId, hui);
        EXPECT_EQ(ret, -EINVAL);
        GTEST_LOG_(INFO) << "RemoteFileShareTest uri is " << hui.uriStr;
        GTEST_LOG_(INFO) << "RemoteFileShareTest file size is " << hui.fileSize;
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_GetDfsUriFromLocal_0009";
    }

    /**
     * @tc.name: remote_file_share_test_0010
     * @tc.desc: Test function of GetDfsUriFromLocal() interface for FAILURE.
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7KDF7
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_GetDfsUriFromLocal_0010, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin  Remote_file_share_GetDfsUriFromLocal_0010";
        const string uriStr = "file://com.demo.a/data/storage/el2/base/notExistFile.txt";
        const int userId = 100;
        HmdfsUriInfo hui;
        int ret = RemoteFileShare::GetDfsUriFromLocal(uriStr, userId, hui);
        EXPECT_NE(ret, E_OK);
        GTEST_LOG_(INFO) << "RemoteFileShareTest uri is " << hui.uriStr;
        GTEST_LOG_(INFO) << "RemoteFileShareTest file size is " << hui.fileSize;
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_GetDfsUriFromLocal_0010";
    }

    /**
     * @tc.name: remote_file_share_test_0011
     * @tc.desc: Test function of GetDfsUriFromLocal() interface for SUCCESS.
     *           the file name is chinese which has been encoded
     * @tc.size: MEDIUM
     * @tc.type: FUNC
     * @tc.level Level 1
     * @tc.require: I7KDF7
     */
    HWTEST_F(RemoteFileShareTest, Remote_file_share_GetDfsUriFromLocal_0011, testing::ext::TestSize.Level1)
    {
        GTEST_LOG_(INFO) << "RemoteFileShareTest-begin  Remote_file_share_GetDfsUriFromLocal_0011";
        const string dirPath = "/data/app/el2/100/base/com.demo.a/";
        const string uriStr = "file://com.demo.a/data/storage/el2/base/"
            "%E5%85%B1%20%E4%BA%AB%20%E6%96%87%20%E4%BB%B6%20%E6%B5%8B%20%E8%AF%95.txt";
        const string fileStr = "/data/app/el2/100/base/com.demo.a/共 享 文 件 测 试.txt";
        const int userId = 100;

        int ret = mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IXOTH);
        ASSERT_TRUE((ret != -1) || (ret == -1 && errno == EEXIST)) << "RemoteFileShareTest mkdir failed! " << errno;

        int fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
        ASSERT_TRUE(fd != -1) << "RemoteFileShareTest open file failed! " << errno;
        close(fd);

        HmdfsUriInfo hui;
        ret = RemoteFileShare::GetDfsUriFromLocal(uriStr, userId, hui);
        EXPECT_EQ(ret, E_OK);

        ret = unlink(fileStr.c_str());
        ASSERT_TRUE(ret != -1) << "RemoteFileShareTest delete file failed! " << errno;

        ret = rmdir(dirPath.c_str());
        ASSERT_TRUE(ret != -1) << "RemoteFileShareTest rmdir failed! " << errno;
        GTEST_LOG_(INFO) << "RemoteFileShareTest uri is " << hui.uriStr;
        GTEST_LOG_(INFO) << "RemoteFileShareTest file size is " << hui.fileSize;
        GTEST_LOG_(INFO) << "RemoteFileShareTest-end Remote_file_share_GetDfsUriFromLocal_0011";
    }
}
