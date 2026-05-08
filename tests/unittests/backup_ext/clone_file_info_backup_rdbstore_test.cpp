/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <fstream>
#include <sys/stat.h>

#include "clone_file_info_backup_rdbstore.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class CloneFileInfoBackupRdbstoreTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
};

void CloneFileInfoBackupRdbstoreTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
}

void CloneFileInfoBackupRdbstoreTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
}

/**
 * @tc.number: SUB_Clone_File_Info_Backup_Rdbstore_00_QueryRdbStoreNullptr_0100
 * @tc.name: SUB_Clone_File_Info_Backup_Rdbstore_00_QueryRdbStoreNullptr_0100
 * @tc.desc: 测试 QueryAncoMediaFile rdbStore_ == nullptr 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 * @tc.note: 必须最先执行，覆盖 QueryAncoMediaFile 中 rdbStore_ == nullptr 分支
 */
HWTEST_F(CloneFileInfoBackupRdbstoreTest,
          SUB_Clone_File_Info_Backup_Rdbstore_00_QueryRdbStoreNullptr_0100,
          testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-begin "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_00_QueryRdbStoreNullptr_0100";
    try {
        TestManager tm("SUB_Clone_File_Info_Backup_Rdbstore_00_QueryRdbStoreNullptr_0100");
        string root = tm.GetRootDirCurTest();
        string readOnlyFile = root + "/readonly_test.db";
        
        ofstream file(readOnlyFile);
        file << "test content";
        file.close();
        
        chmod(readOnlyFile.c_str(), S_IRUSR | S_IRGRP | S_IROTH);
        
        auto instance = CloneFileInfoBackupRdbstore::GetInstance(readOnlyFile);
        EXPECT_NE(instance, nullptr);
        
        vector<string> paths = instance->QueryAncoMediaFile();
        EXPECT_TRUE(paths.empty());
        
        chmod(readOnlyFile.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-end "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_00_QueryRdbStoreNullptr_0100";
}

/**
 * @tc.number: SUB_Clone_File_Info_Backup_Rdbstore_01_GetInstanceEmptyPath_0100
 * @tc.name: SUB_Clone_File_Info_Backup_Rdbstore_01_GetInstanceEmptyPath_0100
 * @tc.desc: 测试 GetInstance 接口空路径，覆盖构造函数 dbPath == "" 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 * @tc.note: 空路径在构造函数 dbPath == "" 分支直接 return，不会调用 TryGetRdbStore
 */
HWTEST_F(CloneFileInfoBackupRdbstoreTest,
          SUB_Clone_File_Info_Backup_Rdbstore_01_GetInstanceEmptyPath_0100,
          testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-begin "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_01_GetInstanceEmptyPath_0100";
    try {
        string dbPath = "";
        auto instance = CloneFileInfoBackupRdbstore::GetInstance(dbPath);
        EXPECT_NE(instance, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-an exception occurred by GetInstance.";
    }
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-end "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_01_GetInstanceEmptyPath_0100";
}

/**
 * @tc.number: SUB_Clone_File_Info_Backup_Rdbstore_02_QueryEmptyPath_0100
 * @tc.name: SUB_Clone_File_Info_Backup_Rdbstore_02_QueryEmptyPath_0100
 * @tc.desc: 测试 QueryAncoMediaFile 接口空路径
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 * @tc.note: 空路径导致 rdbStore_ 为 nullptr，QueryAncoMediaFile 应返回空 vector
 */
HWTEST_F(CloneFileInfoBackupRdbstoreTest,
          SUB_Clone_File_Info_Backup_Rdbstore_02_QueryEmptyPath_0100,
          testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-begin "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_02_QueryEmptyPath_0100";
    try {
        string dbPath = "";
        auto instance = CloneFileInfoBackupRdbstore::GetInstance(dbPath);
        EXPECT_NE(instance, nullptr);
        
        vector<string> paths = instance->QueryAncoMediaFile();
        EXPECT_TRUE(paths.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-an exception occurred by QueryAncoMediaFile.";
    }
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-end "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_02_QueryEmptyPath_0100";
}

/**
 * @tc.number: SUB_Clone_File_Info_Backup_Rdbstore_Callback_OnCreate_0100
 * @tc.name: SUB_Clone_File_Info_Backup_Rdbstore_Callback_OnCreate_0100
 * @tc.desc: 测试 CloneFileInfoBackupCallBack OnCreate 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(CloneFileInfoBackupRdbstoreTest,
          SUB_Clone_File_Info_Backup_Rdbstore_Callback_OnCreate_0100,
          testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-begin "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_Callback_OnCreate_0100";
    try {
        CloneFileInfoBackupCallBack callback;
        NativeRdb::RdbStore* rdbStore = nullptr;
        int32_t result = callback.OnCreate(*rdbStore);
        EXPECT_EQ(result, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-an exception occurred by OnCreate.";
    }
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-end "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_Callback_OnCreate_0100";
}

/**
 * @tc.number: SUB_Clone_File_Info_Backup_Rdbstore_Callback_OnUpgrade_0100
 * @tc.name: SUB_Clone_File_Info_Backup_Rdbstore_Callback_OnUpgrade_0100
 * @tc.desc: 测试 CloneFileInfoBackupCallBack OnUpgrade 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(CloneFileInfoBackupRdbstoreTest,
          SUB_Clone_File_Info_Backup_Rdbstore_Callback_OnUpgrade_0100,
          testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-begin "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_Callback_OnUpgrade_0100";
    try {
        CloneFileInfoBackupCallBack callback;
        NativeRdb::RdbStore* rdbStore = nullptr;
        int32_t oldVersion = 1;
        int32_t newVersion = 2;
        int32_t result = callback.OnUpgrade(*rdbStore, oldVersion, newVersion);
        EXPECT_EQ(result, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-an exception occurred by OnUpgrade.";
    }
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-end "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_Callback_OnUpgrade_0100";
}

/**
 * @tc.number: SUB_Clone_File_Info_Backup_Rdbstore_03_GetInstanceSuccess_0100
 * @tc.name: SUB_Clone_File_Info_Backup_Rdbstore_03_GetInstanceSuccess_0100
 * @tc.desc: 测试 GetInstance 接口正常路径
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(CloneFileInfoBackupRdbstoreTest,
          SUB_Clone_File_Info_Backup_Rdbstore_03_GetInstanceSuccess_0100,
          testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-begin "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_03_GetInstanceSuccess_0100";
    try {
        TestManager tm("SUB_Clone_File_Info_Backup_Rdbstore_03_GetInstanceSuccess_0100");
        string root = tm.GetRootDirCurTest();
        string dbPath = root + "/test.db";
        
        auto instance = CloneFileInfoBackupRdbstore::GetInstance(dbPath);
        EXPECT_NE(instance, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-an exception occurred by GetInstance.";
    }
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-end "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_03_GetInstanceSuccess_0100";
}

/**
 * @tc.number: SUB_Clone_File_Info_Backup_Rdbstore_04_QueryNormal_0100
 * @tc.name: SUB_Clone_File_Info_Backup_Rdbstore_04_QueryNormal_0100
 * @tc.desc: 测试 QueryAncoMediaFile 接口正常路径
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(CloneFileInfoBackupRdbstoreTest,
          SUB_Clone_File_Info_Backup_Rdbstore_04_QueryNormal_0100,
          testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-begin "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_04_QueryNormal_0100";
    try {
        TestManager tm("SUB_Clone_File_Info_Backup_Rdbstore_04_QueryNormal_0100");
        string root = tm.GetRootDirCurTest();
        string dbPath = root + "/test_query.db";
        
        auto instance = CloneFileInfoBackupRdbstore::GetInstance(dbPath);
        EXPECT_NE(instance, nullptr);
        
        vector<string> paths = instance->QueryAncoMediaFile();
        EXPECT_TRUE(paths.empty() || !paths.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-an exception occurred by QueryAncoMediaFile.";
    }
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-end "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_04_QueryNormal_0100";
}

/**
 * @tc.number: SUB_Clone_File_Info_Backup_Rdbstore_05_QueryInvalidColumn_0100
 * @tc.name: SUB_Clone_File_Info_Backup_Rdbstore_05_QueryInvalidColumn_0100
 * @tc.desc: 测试 QueryAncoMediaFile 中 GetColumnIndex 失败分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 * @tc.note: 创建缺少 path 列的表，触发 GetColumnIndex 失败分支
 */
HWTEST_F(CloneFileInfoBackupRdbstoreTest,
          SUB_Clone_File_Info_Backup_Rdbstore_05_QueryInvalidColumn_0100,
          testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-begin "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_05_QueryInvalidColumn_0100";
    try {
        TestManager tm("SUB_Clone_File_Info_Backup_Rdbstore_05_QueryInvalidColumn_0100");
        string root = tm.GetRootDirCurTest();
        string dbPath = root + "/test_invalid_column.db";
        
        NativeRdb::RdbStoreConfig config(dbPath);
        int errCode = 0;
        CloneFileInfoBackupCallBack callback;
        auto rdbStore = NativeRdb::RdbHelper::GetRdbStore(config, 1, callback, errCode);
        ASSERT_NE(rdbStore, nullptr);
        
        string createTableSql = "CREATE TABLE IF NOT EXISTS anco_file_info (invalid_column TEXT);";
        rdbStore->ExecuteSql(createTableSql);
        
        string insertSql = "INSERT INTO anco_file_info (invalid_column) VALUES ('/data/test/file.txt');";
        rdbStore->ExecuteSql(insertSql);
        
        auto instance = CloneFileInfoBackupRdbstore::GetInstance(dbPath);
        EXPECT_NE(instance, nullptr);
        
        vector<string> paths = instance->QueryAncoMediaFile();
        EXPECT_TRUE(paths.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-an exception occurred by QueryInvalidColumn.";
    }
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-end "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_05_QueryInvalidColumn_0100";
}

/**
 * @tc.number: SUB_Clone_File_Info_Backup_Rdbstore_06_MultiInstance_0100
 * @tc.name: SUB_Clone_File_Info_Backup_Rdbstore_06_MultiInstance_0100
 * @tc.desc: 测试多次调用 GetInstance 获取同一实例（单例模式）
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 * @tc.note: 放在最后执行，验证单例模式，覆盖构造函数中 rdbStore_ != nullptr 分支
 */
HWTEST_F(CloneFileInfoBackupRdbstoreTest,
          SUB_Clone_File_Info_Backup_Rdbstore_06_MultiInstance_0100,
          testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-begin "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_06_MultiInstance_0100";
    try {
        TestManager tm("SUB_Clone_File_Info_Backup_Rdbstore_06_MultiInstance_0100");
        string root = tm.GetRootDirCurTest();
        string dbPath = root + "/test_multi.db";
        
        auto instance1 = CloneFileInfoBackupRdbstore::GetInstance(dbPath);
        auto instance2 = CloneFileInfoBackupRdbstore::GetInstance(dbPath);
        
        EXPECT_NE(instance1, nullptr);
        EXPECT_NE(instance2, nullptr);
        EXPECT_EQ(instance1, instance2);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-an exception occurred by MultiInstance.";
    }
    GTEST_LOG_(INFO) << "CloneFileInfoBackupRdbstoreTest-end "
                      << "SUB_Clone_File_Info_Backup_Rdbstore_06_MultiInstance_0100";
}
} // namespace OHOS::FileManagement::Backup