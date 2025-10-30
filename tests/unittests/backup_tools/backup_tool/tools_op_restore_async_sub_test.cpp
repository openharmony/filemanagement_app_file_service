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

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include <gtest/gtest.h>

#include "b_resources/b_constants.h"
#include "tools_op.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_file_info.h"
#include "tools_op_restore_async.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
const string FILE_NAME = "1.tar";
const string BUNDLE_NAME = "com.example.app2backup/";
const string MANAGE_JSON = "manage.json";
} // namespace

class ToolsOpRestoreAsyncTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1200
 * @tc.name: tools_op_restore_async_1200
 * @tc.desc: 测试当BExcepUltils::VerifyPath方法抛出异常时
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1200";
    try {
        string pathCapFile = "invalid/path";
        vector<string> bundleNames = {"bundle1", "bundle2"};
        string type = "true";
        string userId = "123456";
        shared_ptr<SessionAsync> restore = make_shared<SessionAsync>();
        AppendBundles(restore, pathCapFile, bundleNames, type, userId);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1200";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1201
 * @tc.name: tools_op_restore_async_1201
 * @tc.desc: 测试当open方法返回错误时
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1201, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1201";
    try {
        string pathCapFile = " ";
        vector<string> bundleNames = {"bundle1", "bundle2"};
        string type = "true";
        string userId = "123456";
        shared_ptr<SessionAsync> restore = make_shared<SessionAsync>();
        EXPECT_EQ(-errno, AppendBundles(restore, pathCapFile, bundleNames, type, userId));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1201";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1202
 * @tc.name: tools_op_restore_async_1202
 * @tc.desc: 测试当type = "true"
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1202, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1202";
    try {
        string pathCapFile = "/invalid/path";
        vector<string> bundleNames = {"bundle1", "bundle2"};
        string type = "true";
        string userId = "123456";
        shared_ptr<SessionAsync> restore = make_shared<SessionAsync>();
        int32_t ret = AppendBundles(restore, pathCapFile, bundleNames, type, userId);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1202";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1204
 * @tc.name: tools_op_restore_async_1204
 * @tc.desc: 测试AppendBundles方法抛出异常
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1204, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1204";
    try {
        string pathCapFile = "/invalid/path";
        vector<string> bundleNames = {"bun/dle1"};
        string type = "false";
        string userId = "123456";
        shared_ptr<SessionAsync> restore = make_shared<SessionAsync>();
        int32_t ret = AppendBundles(restore, pathCapFile, bundleNames, type, userId);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1204";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1300
 * @tc.name: tools_op_restore_async_1300
 * @tc.desc: 测试InitArg方法中ChangeBundleInfo返回false的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1300";
    try {
        string pathCapFile = " ";
        vector<string> bundleNames = {"bundle1", "bundle2"};
        string type = "type1";
        string userId = "user1";
        EXPECT_EQ(InitArg(pathCapFile, bundleNames, type, userId), -errno);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1300";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1301
 * @tc.name: tools_op_restore_async_1301
 * @tc.desc: 测试InitArg正常
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1301, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1301";
    try {
        string pathCapFile = "/data/test/tmp";
        int fd = open(pathCapFile.data(), O_RDWR | O_CREAT, S_IRWXU);
        EXPECT_GT(fd, 0);
        close(fd);
        vector<string> bundleNames = {"com.example.app2backup"};
        string type = "true";
        string userId = "100";
        EXPECT_EQ(InitArg(pathCapFile, bundleNames, type, userId), 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1301";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1400
 * @tc.name: tools_op_restore_async_1400
 * @tc.desc: 测试包含所有参数的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1400";
    try {
        map<string, vector<string>> mapArgToVal;
        mapArgToVal["pathCapFile"] = {"/tmp/"};
        mapArgToVal["bundles"] = {"com.example.app2backup"};
        mapArgToVal["restoreType"] = {"true"};
        mapArgToVal["userId"] = {"100"};
        int ret = Exec(mapArgToVal);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1400";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1401
 * @tc.name: tools_op_restore_async_1401
 * @tc.desc: 测试不包含所有参数的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1401, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1401";
    try {
        map<string, vector<string>> mapArgToVal;
        int ret = Exec(mapArgToVal);
        EXPECT_EQ(ret, -EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1401";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1500
 * @tc.name: tools_op_restore_async_1500
 * @tc.desc: test func
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1500";
    try {
        std::string path = "/data/backup/receive";
        std::vector<ExtManageInfo> pkgInfo;
        ExtManageInfo info;
        info.hashName = "hashName";
        info.fileName = "fileName";
        info.isBigFile = false;
        info.isUserTar = false;
        pkgInfo.push_back(info);

        auto result = ReadyExtManage(path, pkgInfo);
        EXPECT_EQ(result.size(), 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1500";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1501
 * @tc.name: tools_op_restore_async_1501
 * @tc.desc: test func
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1501, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1501";
    try {
        std::string path = "/data/backup/receive";
        std::vector<ExtManageInfo> pkgInfo;
        ExtManageInfo info;
        info.hashName = "";
        info.fileName = "";
        info.isBigFile = false;
        info.isUserTar = false;
        pkgInfo.push_back(info);

        auto result = ReadyExtManage(path, pkgInfo);
        EXPECT_EQ(result.size(), 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1501";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1502
 * @tc.name: tools_op_restore_async_1502
 * @tc.desc: test func
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1502, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1502";
    try {
        std::string path = "/data/backup/receive";
        std::vector<ExtManageInfo> pkgInfo;
        ExtManageInfo info;
        info.hashName = "hashName";
        info.fileName = "fileName";
        info.isBigFile = true;
        info.isUserTar = true;
        pkgInfo.push_back(info);

        auto result = ReadyExtManage(path, pkgInfo);
        EXPECT_EQ(result.size(), 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1502";
}
} // namespace OHOS::FileManagement::Backup