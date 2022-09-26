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
}
