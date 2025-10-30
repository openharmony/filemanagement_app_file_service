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
#include <vector>

#include <gtest/gtest.h>

#include "b_resources/b_constants.h"
#include "tools_op.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_file_info.h"
#include "tools_op_restore.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
const string BUNDLE_NAME = "com.example.app2backup/";
const string MANAGE_JSON = "manage.json";
const string FILE_NAME = "1.tar";
} // namespace

class ToolsOpRestoreSubTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_tools_op_restore_0700
 * @tc.name: tools_op_restore_OnBundleFinished_0700
 * @tc.desc: test OnAllBundlesFinished empty with 3 param
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_OnBundleFinished_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_OnBundleFinished_0700";
    try {
        auto ctx = make_shared<Session>();
        ctx->cnt_ = 1;
        ErrCode err = 0;
        BundleName name = BUNDLE_NAME;
        OnBundleFinished(ctx, err, name);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_OnBundleFinished_0700";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_0701
 * @tc.name: tools_op_restore_OnBundleFinished_0701
 * @tc.desc: test OnAllBundlesFinished error with 3 param
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_OnBundleFinished_0701, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_OnBundleFinished_0701";
    try {
        auto ctx = make_shared<Session>();
        ctx->cnt_ = 1;
        ErrCode err = -1;
        BundleName name = BUNDLE_NAME;
        OnBundleFinished(ctx, err, name);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_OnBundleFinished_0701";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_0702
 * @tc.name: tools_op_restore_OnBundleFinished_0702
 * @tc.desc: test OnAllBundlesFinished error with 2 param
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_OnBundleFinished_0702, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_OnBundleFinished_0702";
    try {
        auto ctx = make_shared<Session>();
        ErrCode err = 0;
        OnAllBundlesFinished(ctx, err);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_OnBundleFinished_0702";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_0703
 * @tc.name: tools_op_restore_OnBundleFinished_0703
 * @tc.desc: test OnAllBundlesFinished error with 2 param
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_OnBundleFinished_0703, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_OnBundleFinished_0703";
    try {
        auto ctx = make_shared<Session>();
        ErrCode err = -1;
        OnAllBundlesFinished(ctx, err);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_OnBundleFinished_0703";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_0704
 * @tc.name: tools_op_restore_OnBundleFinished_0704
 * @tc.desc: test OnBackupServiceDied
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_OnBundleFinished_0704, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_OnBundleFinished_0704";
    try {
        auto ctx = make_shared<Session>();
        OnBackupServiceDied(ctx);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_OnBundleFinished_0704";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_0800
 * @tc.name: tools_op_restore_OnResultReport_0800
 * @tc.desc: 测试当err=0时
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_OnResultReport_0800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_OnResultReport_0800";
    try {
        auto ctx = make_shared<Session>();
        std::string bundleName = "com.example.app2backup";
        std::string resultInfo = "result info";
        OnResultReport(ctx, bundleName, resultInfo);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_OnResultReport_0800";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_0900
 * @tc.name: tools_op_restore_RestoreApp_0900
 * @tc.desc: 测试当!restore时
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_RestoreApp_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_RestoreApp_0900";
    try {
        shared_ptr<Session> restore = nullptr;
        vector<BundleName> bundleNames = {"com.example.app2backup/"};
        bool updateSendFiles = false;
        RestoreApp(restore, bundleNames, updateSendFiles);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_RestoreApp_0900";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_0901
 * @tc.name: tools_op_restore_RestoreApp_0901
 * @tc.desc: 测试当!restore->session_时
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_RestoreApp_0901, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_RestoreApp_0901";
    try {
        shared_ptr<Session> restore = make_shared<Session>();
        restore->session_ = nullptr;
        vector<BundleName> bundleNames = {"com.example.app2backup/"};
        bool updateSendFiles = false;
        RestoreApp(restore, bundleNames, updateSendFiles);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_RestoreApp_0901";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_0902
 * @tc.name: tools_op_restore_RestoreApp_0902
 * @tc.desc: 测试当bundlename 包含'/'时
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_RestoreApp_0902, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_RestoreApp_0902";
    try {
        shared_ptr<Session> restore = make_shared<Session>();
        restore->session_ = {};
        vector<BundleName> bundleNames = {"com.example.app2backup/"};
        bool updateSendFiles = false;
        RestoreApp(restore, bundleNames, updateSendFiles);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_RestoreApp_0902";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1000
 * @tc.name: tools_op_restore_GetRealPath_1000
 * @tc.desc: 测试当path为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_GetRealPath_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_GetRealPath_1000";
    try {
        string path = " ";
        bool ret = GetRealPath(path);
        EXPECT_EQ(ret, false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_GetRealPath_1000";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1001
 * @tc.name: tools_op_restore_GetRealPath_1001
 * @tc.desc: 测试当path不存在
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_GetRealPath_1001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_GetRealPath_1001";
    try {
        string cmdMkdir = string("mkdir -p ") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data();
        system(cmdMkdir.c_str());
        string path = "/data/backup/no/recived/";
        bool ret = GetRealPath(path);
        EXPECT_EQ(ret, false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_GetRealPath_1001";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1002
 * @tc.name: tools_op_restore_GetRealPath_1002
 * @tc.desc: 测试当path正常
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_GetRealPath_1002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_GetRealPath_1002";
    try {
        string path = string(BConstants::BACKUP_TOOL_RECEIVE_DIR.data()) + "/tmp";
        int fd = open(path.data(), O_RDWR | O_CREAT, S_IRWXU);
        EXPECT_GT(fd, 0);
        close(fd);
        bool ret = GetRealPath(path);
        EXPECT_EQ(ret, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_GetRealPath_1002";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1100
 * @tc.name: tools_op_restore_InitRestoreSession_1100
 * @tc.desc: 测试当!ctx
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_InitRestoreSession_1100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_GInitRestoreSession_1100";
    try {
        shared_ptr<Session> ctx = nullptr;
        InitRestoreSession(ctx);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_GInitRestoreSession_1100";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1101
 * @tc.name: tools_op_restore_InitRestoreSession_1101
 * @tc.desc: 测试当ctx
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_InitRestoreSession_1101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_InitRestoreSession_1101";
    try {
        shared_ptr<Session> ctx = make_shared<Session>();
        InitRestoreSession(ctx);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_InitRestoreSession_1101";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1200
 * @tc.name: tools_op_restore_InitPathCapFile_1200
 * @tc.desc: realPath 为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_InitPathCapFile_1200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_InitPathCapFile_1200";
    try {
        string pathCapFile = " ";
        vector<string> bundleNames = {"com.example.app2backup/"};
        bool depMode = true;
        int32_t ret = InitPathCapFile(pathCapFile, bundleNames, depMode);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_InitPathCapFile_1200";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1201
 * @tc.name: tools_op_restore_InitPathCapFile_1201
 * @tc.desc: test func InitPathCapFile1
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_InitPathCapFile_1201, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_InitPathCapFile_1201";
    try {
        string pathCapFile = "/data/backup/tmp";
        vector<string> bundleNames = {"com.example.app2backup/"};
        bool depMode = true;
        int32_t ret = InitPathCapFile(pathCapFile, bundleNames, depMode);
        EXPECT_LT(ret, 0);
    } catch (BError &e) {
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_InitPathCapFile_1201";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1202
 * @tc.name: tools_op_restore_InitPathCapFile_1202
 * @tc.desc: test func InitPathCapFile2
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_InitPathCapFile_1202, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_InitPathCapFile_1202";
    try {
        string pathCapFile = "/data/backup/tmp";
        vector<string> bundleNames = {"com.example.app2backup/"};
        bool depMode = false;
        int32_t ret = InitPathCapFile(pathCapFile, bundleNames, depMode);
        EXPECT_LT(ret, 0);
    } catch (BError &e) {
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_InitPathCapFile_1202";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1300
 * @tc.name: tools_op_restore_Exec_1300
 * @tc.desc: 测试depMOde =false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_Exec_1300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_Exec_1300";
    try {
        map<string, vector<string>> mapArgToVal;
        mapArgToVal["depMode"] = {"true"};
        Exec(mapArgToVal);
        EXPECT_EQ(mapArgToVal["depMode"][0], "true");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_Exec_1300";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1301
 * @tc.name: tools_op_restore_Exec_1301
 * @tc.desc: 测试pathCapFile不存在
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_Exec_1301, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_Exec_1301";
    try {
        map<string, vector<string>> mapArgToVal;
        mapArgToVal["depMode"] = {"false"};
        mapArgToVal["bundles"] = {"com.example.app2backup/"};
        int ret = Exec(mapArgToVal);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_Exec_1301";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1302
 * @tc.name: tools_op_restore_Exec_1302
 * @tc.desc: 测试bundles不存在
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_Exec_1302, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_Exec_1302";
    try {
        string cmdMkdir = string("mkdir -p ") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data() + BUNDLE_NAME;
        system(cmdMkdir.c_str());
        map<string, vector<string>> mapArgToVal;
        mapArgToVal["depMode"] = {"false"};
        mapArgToVal["pathCapFile"] = {"/data/backup/recived/"};
        int ret = Exec(mapArgToVal);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_Exec_1302";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1303
 * @tc.name: tools_op_restore_Exec_1303
 * @tc.desc: 测试都存在
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_Exec_1303, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_Exec_1303";
    try {
        string cmdMkdir = string("mkdir -p ") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data() + BUNDLE_NAME;
        system(cmdMkdir.c_str());
        map<string, vector<string>> mapArgToVal;
        mapArgToVal["depMode"] = {"false"};
        mapArgToVal["bundles"] = {"com.example.app2backup/"};
        mapArgToVal["pathCapFile"] = {"/data/backup/recived/com.example.app2backup/"};
        int ret = Exec(mapArgToVal);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_Exec_1303";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_1304
 * @tc.name: tools_op_restore_Exec_1304
 * @tc.desc: test func
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9NOPD
 */
HWTEST_F(ToolsOpRestoreSubTest, tools_op_restore_Exec_1304, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-begin tools_op_restore_Exec_1304";
    try {
        map<string, vector<string>> mapArgToVal;
        mapArgToVal["pathCapFile"] = {"/data/backup/recived/com.example.app2backup/"};
        mapArgToVal["bundles"] = {"com.example.app2backup/"};
        int ret = Exec(mapArgToVal);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreSubTest-end tools_op_restore_Exec_1304";
}
} // namespace OHOS::FileManagement::Backup