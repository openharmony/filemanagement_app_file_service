/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <string_view>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_caps.h"
#include "b_json/b_json_entity_extension_config.h"
#include "b_process/b_process.h"
#include "b_resources/b_constants.h"
#include "directory_ex.h"
#include "file_ex.h"
#include "parameter.h"
#include "test_manager.h"
#include "json/value.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BJsonEntityExtensionConfigTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

static vector<string> DEFAULT_INCLUDE_DIR = {
    "data/storage/el1/database/",
    "data/storage/el1/base/files/",
    "data/storage/el1/base/preferences/",
    "data/storage/el1/base/haps/*/database/",
    "data/storage/el1/base/haps/*/files/",
    "data/storage/el1/base/haps/*/preferences/",
    "data/storage/el2/database/",
    "data/storage/el2/base/files/",
    "data/storage/el2/base/preferences/",
    "data/storage/el2/base/haps/*/database/",
    "data/storage/el2/base/haps/*/files/",
    "data/storage/el2/base/haps/*/preferences/",
    "data/storage/el2/distributedfiles/",
};
static vector<string> DEFAULT_EXCLUDE_DIR = {};

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_0100
 * @tc.name: b_json_entity_extension_config_0100
 * @tc.desc: 不包含includes和excludes
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_0100";
    try {
        TestManager tm("b_json_entity_extension_config_0100");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, "");

        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> excludes = cache.GetExcludes();
        EXPECT_EQ(excludes, DEFAULT_EXCLUDE_DIR);
        vector<string> includes = cache.GetIncludes();
        EXPECT_EQ(includes, DEFAULT_INCLUDE_DIR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_0100";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_0200
 * @tc.name: b_json_entity_extension_config_0200
 * @tc.desc: json文件中只包含includes
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_0200";
    try {
        TestManager tm("b_json_entity_extension_config_0200");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"includes":["", "", ""]})");

        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        vector<string> includesExpect = {"", "", ""};
        EXPECT_EQ(includes, includesExpect);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_0200";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_0300
 * @tc.name: b_json_entity_extension_config_0300
 * @tc.desc: json文件中只包含excludes
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_0300";
    try {
        TestManager tm("b_json_entity_extension_config_0300");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"excludes":["", "", ""]})");

        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> excludes = cache.GetExcludes();
        vector<string> excludesExpect = {"", "", ""};
        EXPECT_EQ(excludes, excludesExpect);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_0300";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_0400
 * @tc.name: b_json_entity_extension_config_0400
 * @tc.desc: 同时包含includes和excludes
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_0400";
    try {
        TestManager tm("b_json_entity_extension_config_0400");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"includes":["", "", ""], "excludes":["", "", ""]})");

        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        vector<string> includesExpect = {"", "", ""};
        EXPECT_EQ(includes, includesExpect);
        vector<string> excludes = cache.GetExcludes();
        vector<string> excludesExpect = {"", "", ""};
        EXPECT_EQ(excludes, excludesExpect);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_0400";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_0500
 * @tc.name: b_json_entity_extension_config_0500
 * @tc.desc: json文件中标签为特殊字符
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_0500, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_0500";
    try {
        TestManager tm("b_json_entity_extension_config_0500");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"%#$%445":["", "", ""]})");

        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        vector<string> includesExpect = {DEFAULT_INCLUDE_DIR};
        EXPECT_EQ(includes, includesExpect);
        vector<string> excludes = cache.GetExcludes();
        vector<string> excludesExpect = {DEFAULT_EXCLUDE_DIR};
        EXPECT_EQ(excludes, excludesExpect);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_0500";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_0600
 * @tc.name: b_json_entity_extension_config_0600
 * @tc.desc: json文件中标签为中文汉字
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_0600, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_0600";
    try {
        TestManager tm("b_json_entity_extension_config_0600");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"测试代码":["", "", ""]})");

        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        vector<string> excludes = cache.GetExcludes();
        EXPECT_EQ(includes, DEFAULT_INCLUDE_DIR);
        EXPECT_EQ(excludes, DEFAULT_EXCLUDE_DIR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_0600";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_0700
 * @tc.name: b_json_entity_extension_config_0700
 * @tc.desc: json文件中无标签
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_0700, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_0700";
    try {
        TestManager tm("b_json_entity_extension_config_0700");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"":["", "", ""]})");

        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        EXPECT_EQ(includes, DEFAULT_INCLUDE_DIR);
        vector<string> excludes = cache.GetExcludes();
        EXPECT_EQ(excludes, DEFAULT_EXCLUDE_DIR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_0700";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_0900
 * @tc.name: b_json_entity_extension_config_0900
 * @tc.desc: 测试 GetJSonSource 接口能否在非service进程下正确读取backup_config.json
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_0900, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_0900";
    try {
        string jsonContent = R"({"allowToBackupRestore":true})";
        auto [bFatalErr, ret] = BProcess::ExecuteCmd({"mkdir", "-p", BConstants::BACKUP_CONFIG_EXTENSION_PATH});
        EXPECT_FALSE(bFatalErr);
        EXPECT_EQ(ret, 0);
        string jsonFilePath = string(BConstants::BACKUP_CONFIG_EXTENSION_PATH).append(BConstants::BACKUP_CONFIG_JSON);
        SaveStringToFile(jsonFilePath, jsonContent);
        string_view sv = R"({"allowToBackupRestore":false})";
        SetMockParameter(true);
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        string jsonRead = cache.GetJSonSource(sv, any());
        EXPECT_NE(jsonRead, jsonContent);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetJSonSource.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_0900";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_1000
 * @tc.name: b_json_entity_extension_config_1000
 * @tc.desc: 测试GetJSonSource接口能否在service进程下正确读取backup_config.json
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_1000, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_1000";
    try {
        string bundleName = "com.example.app2backup";
        string jsonFileDir = BConstants::GetSaBundleBackupRootDir(BConstants::DEFAULT_USER_ID).append(bundleName);
        string jsonContent = R"({"allowToBackupRestore":true})";
        auto [bFatalErr, ret] = BProcess::ExecuteCmd({"mkdir", "-p", jsonFileDir});
        EXPECT_FALSE(bFatalErr);
        EXPECT_EQ(ret, 0);
        string jsonFilePath = jsonFileDir.append("/").append(BConstants::BACKUP_CONFIG_JSON);
        SaveStringToFile(jsonFilePath, jsonContent);
        uid_t currUid = getuid();
        setuid(BConstants::BACKUP_UID);
        string_view sv = R"({"allowToBackupRestore":false})";
        SetMockParameter(true);
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv, bundleName);
        auto cache = cachedEntity.Structuralize();
        string jsonRead = cache.GetJSonSource(sv, bundleName);
        setuid(currUid);
        EXPECT_NE(jsonRead, jsonContent);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetJSonSource.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_1000";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_1100
 * @tc.name: b_json_entity_extension_config_1100
 * @tc.desc: 测试GetJSonSource接口能否在backup.debug.overrideExtensionConfig为false的情况下保持原JSon字符串不变
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_1100, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_1100";
    try {
        string_view sv = R"({"allowToBackupRestore":false})";
        SetMockParameter(false);
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        string jsonRead = cache.GetJSonSource(sv, any());
        EXPECT_EQ(jsonRead, string(sv));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetJSonSource.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_1100";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_1200
 * @tc.name: b_json_entity_extension_config_1200
 * @tc.desc: 测试GetIncludes接口在Json数据中键为includes的值不为数组时能否成功返回默认目录
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_1200, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_1200";
    try {
        string_view sv = R"({"includes":1})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        vector<string> vs = cache.GetIncludes();
        EXPECT_EQ(vs, DEFAULT_INCLUDE_DIR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetIncludes.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_1200";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_1300
 * @tc.name: b_json_entity_extension_config_1300
 * @tc.desc: 测试GetIncludes接口在Json数据中键为includes的值为数组且数组元素全都不为字符串时能否成功返回空目录
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_1300, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_1300";
    try {
        string_view sv = R"({"includes":[1]})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        EXPECT_EQ(includes, vector<string>({""}));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetIncludes.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_1300";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_1400
 * @tc.name: b_json_entity_extension_config_1400
 * @tc.desc: 测试GetExcludes接口在Json数据中键为excludes的值不为数组时能否成功返回空vector
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_1400, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_1400";
    try {
        string_view sv = R"({"excludes":1})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        vector<string> excludes = cache.GetExcludes();
        EXPECT_EQ(excludes, vector<string>());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetExcludes.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_1400";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_1500
 * @tc.name: b_json_entity_extension_config_1500
 * @tc.desc: 测试GetExcludes接口在Json数据中键为excludes的值为数组且数组元素全都不为字符串时能否成功返回空vector
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_1500, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_1500";
    try {
        string_view sv = R"({"excludes":[1]})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        vector<string> cludes = cache.GetExcludes();
        EXPECT_EQ(cludes, vector<string>());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetExcludes.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_1500";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_1600
 * @tc.name: b_json_entity_extension_config_1600
 * @tc.desc: 测试GetAllowToBackupRestore接口在Json数据对象nullValue时能否成功返回false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_1600, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_1600";
    try {
        Json::Value jv(Json::nullValue);
        BJsonEntityExtensionConfig extCfg(jv);
        EXPECT_FALSE(extCfg.GetAllowToBackupRestore());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetAllowToBackupRestore.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_1600";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_1700
 * @tc.name: b_json_entity_extension_config_1700
 * @tc.desc: 测试GetAllowToBackupRestore接口在Json数据对象不含allowToBackupRestore键时能否成功返回false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_1700, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_1700";
    try {
        string_view sv = R"({"allowToBackupRestore_":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_FALSE(cache.GetAllowToBackupRestore());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetAllowToBackupRestore.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_1700";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_1800
 * @tc.name: b_json_entity_extension_config_1800
 * @tc.desc: 测试GetAllowToBackupRestore接口在Json数据对象键为allowToBackupRestore的值不为布尔值时能否成功返回false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_1800, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_1800";
    try {
        string_view sv = R"({"allowToBackupRestore":1})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_FALSE(cache.GetAllowToBackupRestore());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetAllowToBackupRestore.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_1800";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_1900
 * @tc.name: b_json_entity_extension_config_1900
 * @tc.desc: 测试GetAllowToBackupRestore接口在Json数据对象键为allowToBackupRestore的值不为TRUE时能否成功返回TRUE
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: issuesI8J01W
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_1900, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_1900";
    try {
        string_view sv = R"({"allowToBackupRestore":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_TRUE(cache.GetAllowToBackupRestore());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetAllowToBackupRestore.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_1900";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_2000
 * @tc.name: b_json_entity_extension_config_2000
 * @tc.desc: 测试GetFullBackupOnly接口在Json数据对象nullValue时能否成功返回false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: issuesI8J01W
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_2000, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_2000";
    try {
        Json::Value jv(Json::nullValue);
        BJsonEntityExtensionConfig extCfg(jv);
        EXPECT_FALSE(extCfg.GetFullBackupOnly());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetFullBackupOnly.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_2000";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_2100
 * @tc.name: b_json_entity_extension_config_2100
 * @tc.desc: 测试GetFullBackupOnly接口在Json数据对象不含fullBackupOnly键时能否成功返回false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: issuesI8J01W
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_2100, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_2100";
    try {
        string_view sv = R"({"fullBackupOnly_":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_FALSE(cache.GetFullBackupOnly());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetFullBackupOnly.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_2100";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_2200
 * @tc.name: b_json_entity_extension_config_2200
 * @tc.desc: 测试GetFullBackupOnly接口在Json数据对象键为fullBackupOnly的值不为布尔值时能否成功返回false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: issuesI8J01W
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_2200, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_2200";
    try {
        string_view sv = R"({"fullBackupOnly":1})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_FALSE(cache.GetFullBackupOnly());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetFullBackupOnly.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_2200";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_2300
 * @tc.name: b_json_entity_extension_config_2300
 * @tc.desc: 测试GetFullBackupOnly接口在Json数据对象键为fullBackupOnly的值为TRUE时能否成功返回TRUE
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: issuesI8J01W
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_2300, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_2300";
    try {
        string_view sv = R"({"fullBackupOnly":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_TRUE(cache.GetFullBackupOnly());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetFullBackupOnly.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_2300";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_2400
 * @tc.name: b_json_entity_extension_config_2400
 * @tc.desc: 测试GetRestoreDeps接口在Json数据对象nullValue时能否成功返回空串
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: issuesI8J01W
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_2400, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_2400";
    try {
        Json::Value jv(Json::nullValue);
        BJsonEntityExtensionConfig extCfg(jv);
        EXPECT_TRUE(extCfg.GetRestoreDeps().empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetRestoreDeps.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_2400";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_2500
 * @tc.name: b_json_entity_extension_config_2500
 * @tc.desc: 测试GetRestoreDeps接口在Json数据对象不含restoreDeps键时能否成功返回空串
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: issuesI8J01W
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_2500, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_2500";
    try {
        string_view sv = R"({"restoreDeps_":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_TRUE(cache.GetRestoreDeps().empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetRestoreDeps.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_2500";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_2600
 * @tc.name: b_json_entity_extension_config_2600
 * @tc.desc: 测试GetRestoreDeps接口在Json数据对象键为restoreDeps的值不为字符串时能否成功返回空串
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: issuesI8J01W
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_2600, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_2600";
    try {
        string_view sv = R"({"restoreDeps":1})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_TRUE(cache.GetRestoreDeps().empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetRestoreDeps.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_2600";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_2700
 * @tc.name: b_json_entity_extension_config_2700
 * @tc.desc: 测试GetRestoreDeps接口在Json数据对象键为restoreDeps的值为"true"时能否成功返回字符串"true"
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: issuesI8J01W
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_2700, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_2700";
    try {
        string_view sv = R"({"restoreDeps":"true"})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_EQ(cache.GetRestoreDeps(), "true");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetRestoreDeps.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_2700";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_2800
 * @tc.name: b_json_entity_extension_config_2800
 * @tc.desc: 测试 GetIncludes 接口在Json数据中键为includes的值为数组且数组元素为空时能否成功返回空目录
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_2800, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_2800";
    try {
        string_view sv = R"({"includes":[]})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        vector<string> vs = cache.GetIncludes();
        EXPECT_EQ(vs, vector<string>({""}));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetIncludes.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_2800";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_2900
 * @tc.name: b_json_entity_extension_config_2900
 * @tc.desc: 测试GetExcludes接口在Json数据中键为excludes的值为数组且数组元素为空时能否成功返回空vector
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_2900, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_2900";
    try {
        string_view sv = R"({"excludes":[]})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        vector<string> vs = cache.GetExcludes();
        EXPECT_EQ(vs, vector<string>());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetExcludes.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_2900";
}


/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_3000
 * @tc.name: b_json_entity_extension_config_3000
 * @tc.desc: 测试GetFullBackupOnly接口在Json数据对象键为fullBackupOnly的值不为布尔值时能否成功返回false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_3000, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_3000";
    try {
        string_view sv = R"({"fullBackupOnly":1})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_FALSE(cache.GetFullBackupOnly());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetFullBackupOnly.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_3000";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_3100
 * @tc.name: b_json_entity_extension_config_3100
 * @tc.desc: 测试GetFullBackupOnly接口在Json数据对象不含fullBackupOnly键时能否成功返回TRUE
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_3100, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_3100";
    try {
        string_view sv = R"({"fullBackupOnly":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_TRUE(cache.GetFullBackupOnly());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetAllowToBackupRestore.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_3100";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_3200
 * @tc.name: b_json_entity_extension_config_3200
 * @tc.desc: 测试GetFullBackupOnly接口在Json数据对象nullValue时能否成功返回false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_3200, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_3200";
    try {
        Json::Value jv(Json::nullValue);
        BJsonEntityExtensionConfig extCfg(jv);
        EXPECT_FALSE(extCfg.GetFullBackupOnly());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetFullBackupOnly.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_3200";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_3300
 * @tc.name: b_json_entity_extension_config_3300
 * @tc.desc: 测试GetRestoreDeps接口能否在Json数据对象的键为restoreDeps的值不为字符串值时能否成功返回string
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_3300, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_3300";
    try {
        string_view sv = R"({"restoreDeps":1})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        string restoreDeps = cache.GetRestoreDeps();
        EXPECT_NE(restoreDeps, string(sv));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetRestoreDeps.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_3300";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_3400
 * @tc.name: b_json_entity_extension_config_3400
 * @tc.desc: 测试GetRestoreDeps接口能否在Json数据对象不含restoreDeps键时能否成功返回string
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_3400, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_3400";
    try {
        string_view sv = R"({"restoreDeps_":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        string depRead = cache.GetRestoreDeps();
        EXPECT_NE(depRead, string(sv));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetRestoreDeps.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_3400";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_3500
 * @tc.name: b_json_entity_extension_config_3500
 * @tc.desc: 测试GetRestoreDeps接口能否在Json数据对象不含restoreDeps键时能否成功返回空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_3500, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_3500";
    try {
        string_view sv = R"({"":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        string depRead = cache.GetRestoreDeps();
        EXPECT_EQ(depRead, "");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetRestoreDeps.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_3500";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_3600
 * @tc.name: b_json_entity_extension_config_3600
 * @tc.desc: 测试GetSupportScene接口能否成功返回
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: issuesI9JXNH
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_3600, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_3600";
    try {
        string_view sv1 = R"({"":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
        auto cache1 = cachedEntity1.Structuralize();
        string supportScene = cache1.GetSupportScene();
        EXPECT_EQ(supportScene, "");

        string_view sv2 = R"({"supportScene":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity2(sv2);
        auto cache2 = cachedEntity2.Structuralize();
        supportScene = cache2.GetSupportScene();
        EXPECT_EQ(supportScene, "");

        string_view sv3 = R"({"supportScene":"true"})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity3(sv3);
        auto cache3 = cachedEntity3.Structuralize();
        supportScene = cache3.GetSupportScene();
        EXPECT_EQ(supportScene, "true");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetRestoreDeps.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_3600";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_3700
 * @tc.name: b_json_entity_extension_config_3700
 * @tc.desc: 测试GetExtraInfo接口能否成功返回
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: issuesI9JXNH
 */
HWTEST_F(BJsonEntityExtensionConfigTest, b_json_entity_extension_config_3700, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin b_json_entity_extension_config_3700";
    try {
        string_view sv1 = R"({"":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
        auto cache1 = cachedEntity1.Structuralize();
        auto extraInfo1 = cache1.GetExtraInfo();
        EXPECT_TRUE(extraInfo1.isNull());

        string_view sv2 = R"({"extraInfo":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity2(sv2);
        auto cache2 = cachedEntity2.Structuralize();
        auto extraInfo2 = cache2.GetExtraInfo();
        EXPECT_TRUE(extraInfo2.isNull());

        string_view sv3 = R"({"extraInfo":{}})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity3(sv3);
        auto cache3 = cachedEntity3.Structuralize();
        auto extraInfo3 = cache3.GetExtraInfo();
        EXPECT_TRUE(extraInfo3.isObject());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetRestoreDeps.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end b_json_entity_extension_config_3700";
}
} // namespace OHOS::FileManagement::Backup