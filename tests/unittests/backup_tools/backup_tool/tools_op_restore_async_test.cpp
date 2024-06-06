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
 * @tc.number: SUB_backup_tools_op_restore_async_0100
 * @tc.name: SUB_backup_tools_op_restore_async_0100
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(ToolsOpRestoreAsyncTest, SUB_backup_tools_op_restore_async_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin SUB_backup_tools_op_restore_async_0100";
    try {
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-info";
        map<string, vector<string>> mapArgToVal;
        string localCap = string(BConstants::SA_BUNDLE_BACKUP_TMP_DIR.data()) + "/tmp";
        vector<string> path = { localCap.data() };
        mapArgToVal.insert(make_pair("pathCapFile", path));
        vector<string> bundles = { "com.example.app2backup" };
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> restoreType = { "true" };
        mapArgToVal.insert(make_pair("restoreType", restoreType));
        vector<string> userId = { "100" };
        mapArgToVal.insert(make_pair("userId", userId));

        // 创建测试路径以及测试环境
        string cmdMkdir = string("mkdir -p ") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data() + BUNDLE_NAME;
        system(cmdMkdir.c_str());
        string cmdTool = string("mkdir -p ") + BConstants::SA_BUNDLE_BACKUP_TMP_DIR.data();
        system(cmdTool.c_str());
        string touchTar = string("touch ") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data() + BUNDLE_NAME + FILE_NAME;
        system(touchTar.c_str());
        string touchManage = string("touch ") + BConstants::BACKUP_TOOL_RECEIVE_DIR.data() + BUNDLE_NAME + MANAGE_JSON;
        system(touchManage.c_str());
        string touchTmp = string("touch ") + localCap;
        system(touchTmp.c_str());

        // 尝试匹配当前命令，成功后执行
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-restoreAsync";
        vector<string_view> curOp;
        curOp.emplace_back("restoreAsync");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        if (matchedOp != opeartions.end()) {
            auto ret = matchedOp->Execute(mapArgToVal);
            EXPECT_EQ(ret, 0);
        }

        mapArgToVal.clear();
        mapArgToVal.insert(make_pair("pathCapFile", path));
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> restoreTypeF = { "false" };
        mapArgToVal.insert(make_pair("restoreType", restoreTypeF));
        mapArgToVal.insert(make_pair("userId", userId));
        if (matchedOp != opeartions.end()) {
            auto ret = matchedOp->Execute(mapArgToVal);
            EXPECT_EQ(ret, 0);
        }

    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end SUB_backup_tools_op_restore_async_0100";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_0200
 * @tc.name: SUB_backup_tools_op_restore_async_0200
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7L7A6
 */
HWTEST_F(ToolsOpRestoreAsyncTest, SUB_backup_tools_op_restore_async_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin SUB_backup_tools_op_restore_async_0200";
    try {
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-The pathCapFile field is not contained.";
        map<string, vector<string>> mapArgToVal;
        vector<string> bundles = {"com.example.app2backup"};
        vector<string> path = {"/data/backup/tmp"};
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> restoreType = {"false"};
        mapArgToVal.insert(make_pair("restoreType", restoreType));
        vector<string> userId = {"100"};
        mapArgToVal.insert(make_pair("userId", userId));

        vector<string_view> curOp;
        curOp.emplace_back("restoreAsync");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        int ret = 0;
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-The bundles field is not contained.";
        mapArgToVal.clear();
        if (matchedOp != opeartions.end()) {
            mapArgToVal.insert(make_pair("pathCapFile", path));
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);

            mapArgToVal.clear();
            mapArgToVal.insert(make_pair("pathCapFile", path));
            mapArgToVal.insert(make_pair("bundles", bundles));
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);

            mapArgToVal.clear();
            mapArgToVal.insert(make_pair("pathCapFile", path));
            mapArgToVal.insert(make_pair("bundles", bundles));
            mapArgToVal.insert(make_pair("restoreType", restoreType));
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);

            mapArgToVal.clear();
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end SUB_backup_tools_op_restore_async_0200";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_0300
 * @tc.name: tools_op_restore_async_0300
 * @tc.desc: 测试文件名包含BConstants::EXT_BACKUP_MANAGE,且tmpPath存在
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_0300";
    try {
        auto ctx = make_shared<SessionAsync>();
        BFileInfo fileInfo;
        fileInfo.owner = "test";
        fileInfo.fileName = "manage.json";
        fileInfo.sn = 1;
        UniqueFd fd(open("text.txt", O_RDWR | O_CREAT, 0666));
        OnFileReady(ctx, fileInfo, move(fd), 0);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_0300";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_0301
 * @tc.name: tools_op_restore_async_0301
 * @tc.desc: 测试文件名不包含BConstants::EXT_BACKUP_MANAGE,且tmpPath存在
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_0301, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_0301";
    try {
        auto ctx = make_shared<SessionAsync>();
        BFileInfo fileInfo;
        fileInfo.owner = "test";
        fileInfo.fileName = "test.json";
        fileInfo.sn = 1;
        UniqueFd fd(open("text.txt", O_RDWR | O_CREAT, 0666));
        OnFileReady(ctx, fileInfo, move(fd), 0);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_0301";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_0400
 * @tc.name: tools_op_restore_async_0400
 * @tc.desc: 测试当err=0时，不会调用UpdateBundleFinishedCount和TryNotify方法
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_0400";
    try {
        auto ctx = make_shared<SessionAsync>();
        ErrCode err = 0;
        BundleName name = "testBundle";
        OnBundleStarted(ctx, err, name);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_0400";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_0500
 * @tc.name: tools_op_restore_async_0500
 * @tc.desc: 测试OnBundleFinished方法
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_0500";
    try {
        auto ctx = make_shared<SessionAsync>();
        ErrCode err = 0;
        BundleName name = "testBundle";
        OnBundleFinished(ctx, err, name);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_0500";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_0600
 * @tc.name: tools_op_restore_async_0600
 * @tc.desc: 测试OnAllBundlesFinished方法
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_0600";
    try {
        auto ctx = make_shared<SessionAsync>();
        ErrCode err = 0;
        OnAllBundlesFinished(ctx, err);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_0600";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_0700
 * @tc.name: tools_op_restore_async_0700
 * @tc.desc: 测试OnResultReport方法
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_0700";
    try {
        auto ctx = make_shared<SessionAsync>();
        std::string resultInfo = "test result info";
        OnResultReport(ctx, resultInfo);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_0700";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_0800
 * @tc.name: tools_op_restore_async_0800
 * @tc.desc: 测试OnBackupServiceDied方法
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_0800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_0800";
    try {
        auto ctx = make_shared<SessionAsync>();
        OnBackupServiceDied(ctx);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_0800";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_0900
 * @tc.name: tools_op_restore_async_0900
 * @tc.desc: 测试AdapteCloneOptimize方法路径存在
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_0900";
    try {
        string path = "/data/backup/receive";
        AdapteCloneOptimize(path);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_0900";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1000
 * @tc.name: tools_op_restore_async_1000
 * @tc.desc: 测试restore为空的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1000";
    try {
        vector<BundleName> bundleNames;
        RestoreApp(nullptr, bundleNames);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1000";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1001
 * @tc.name: tools_op_restore_async_1001
 * @tc.desc: 测试restore不为空bundlename包含'/'的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1001";
    try {
        vector<BundleName> bundleNames;
        bundleNames.push_back("bundle/name");\
        shared_ptr<SessionAsync> restore = make_shared<SessionAsync>();
        RestoreApp(restore, bundleNames);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1001";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1002
 * @tc.name: tools_op_restore_async_1002
 * @tc.desc: 测试正常情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1002";
    try {
        vector<BundleName> bundleNames;
        bundleNames.push_back("bundlename");\
        shared_ptr<SessionAsync> restore = make_shared<SessionAsync>();
        RestoreApp(restore, bundleNames);
    } catch (BError &e) {
        EXPECT_EQ(e.GetCode(), BError(BError::Codes::TOOL_INVAL_ARG).GetCode());
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1002";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1100
 * @tc.name: tools_op_restore_async_1100
 * @tc.desc: 测试正type =false 的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1100";
    try {
        string pathCapFile = "/data/user/0/test/files/bundleInfo.json";
        vector<string> bundleNames = {"bundlenames"};
        string type = "false";
        int32_t ret =ChangeBundleInfo(pathCapFile, bundleNames, type);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1100";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1101
 * @tc.name: tools_op_restore_async_1101
 * @tc.desc: 测试正打开失败的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1101";
    try {
        string pathCapFile = " ";
        vector<string> bundleNames = {"bundlenames"};
        string type = "false";
        int32_t ret =ChangeBundleInfo(pathCapFile, bundleNames, type);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1101";
}

/**
 * @tc.number: SUB_backup_tools_op_restore_async_1102
 * @tc.name: tools_op_restore_async_1102
 * @tc.desc: 测试正打开失败的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9JNFM
 */
HWTEST_F(ToolsOpRestoreAsyncTest, tools_op_restore_async_1102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-begin tools_op_restore_async_1102";
    try {
        string pathCapFile = "/data/user/0/test/files/bundleInfo.json";
        vector<string> bundleNames = {};
        string type = "true";
        int32_t ret =ChangeBundleInfo(pathCapFile, bundleNames, type);
        EXPECT_LT(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpRestoreAsyncTest-end tools_op_restore_async_1102";
}

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
        string pathCapFile = string(BConstants::SA_BUNDLE_BACKUP_TMP_DIR.data()) + "/tmp";
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
} // namespace OHOS::FileManagement::Backup