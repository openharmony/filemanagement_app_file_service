/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_3800
 * @tc.name: backup_b_json_entity_extension_config_3800
 * @tc.desc: 测试GetRequireCompatibility接口能否成功返回
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, backup_b_json_entity_extension_config_3800, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin backup_b_json_entity_extension_config_3800";
    try {
        string_view sv1 = R"({"":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
        auto cache1 = cachedEntity1.Structuralize();
        bool ret = cache1.GetRequireCompatibility();
        EXPECT_FALSE(ret);

        string_view sv2 = R"({"requireCompatibility1":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity2(sv2);
        auto cache2 = cachedEntity2.Structuralize();
        ret = cache2.GetRequireCompatibility();
        EXPECT_FALSE(ret);

        string_view sv3 = R"({"requireCompatibility":123})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity3(sv3);
        auto cache3 = cachedEntity3.Structuralize();
        ret = cache3.GetRequireCompatibility();
        EXPECT_FALSE(ret);

        string_view sv4 = R"({"requireCompatibility":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity4(sv4);
        auto cache4 = cachedEntity4.Structuralize();
        ret = cache4.GetRequireCompatibility();
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetRequireCompatibility.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end backup_b_json_entity_extension_config_3800";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_3900
 * @tc.name: backup_b_json_entity_extension_config_3900
 * @tc.desc: 测试GetBackupScene接口能否返回正确参数
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, backup_b_json_entity_extension_config_3900, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin backup_b_json_entity_extension_config_3900";
    try {
        string_view sv1 = R"({"":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
        auto cache1 = cachedEntity1.Structuralize();
        EXPECT_TRUE(cache1.GetBackupScene() == "");

        string_view sv2 = R"({"backupScene":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity2(sv2);
        auto cache2 = cachedEntity2.Structuralize();
        EXPECT_TRUE(cache2.GetBackupScene() == "");

        string_view sv3 = R"({"backupScene":"123"})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity3(sv3);
        auto cache3 = cachedEntity3.Structuralize();
        EXPECT_TRUE(cache3.GetBackupScene() == "123");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetBackupScene.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end backup_b_json_entity_extension_config_3900";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_4000
 * @tc.name: backup_b_json_entity_extension_config_4000
 * @tc.desc: 测试HasOptionDir接口能否返回正确参数
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, backup_b_json_entity_extension_config_4000, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin backup_b_json_entity_extension_config_4000";
    try {
        string_view sv1 = R"({"":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
        auto cache1 = cachedEntity1.Structuralize();
        EXPECT_FALSE(cache1.HasOptionDir());

        string_view sv2 = R"({"optionDir":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity2(sv2);
        auto cache2 = cachedEntity2.Structuralize();
        EXPECT_FALSE(cache2.HasOptionDir());

        string_view sv3 = R"({"optionDir":["test"]})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity3(sv3);
        auto cache3 = cachedEntity3.Structuralize();
        EXPECT_TRUE(cache3.HasOptionDir());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by HasOptionDir.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end backup_b_json_entity_extension_config_4000";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_4100
 * @tc.name: backup_b_json_entity_extension_config_4100
 * @tc.desc: 测试GetOptionDir接口能否返回正确参数
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, backup_b_json_entity_extension_config_4100, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin backup_b_json_entity_extension_config_4100";
    try {
        string_view sv1 = R"({"":true})";
        string sceneId = "test";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
        auto cache1 = cachedEntity1.Structuralize();
        EXPECT_TRUE(cache1.GetOptionDir(sceneId, "includes").empty());

        string_view sv2 = R"({"optionDir":true})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity2(sv2);
        auto cache2 = cachedEntity2.Structuralize();
        EXPECT_TRUE(cache2.GetOptionDir(sceneId, "includes").empty());

        string_view sv3 = R"({"optionDir":[{"includes":["test"], "sceneId":"test"}]})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity3(sv3);
        auto cache3 = cachedEntity3.Structuralize();
        EXPECT_FALSE(cache3.GetOptionDir(sceneId, "includes").empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetOptionDir.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end backup_b_json_entity_extension_config_4100";
}

/**
 * @tc.number: SUB_backup_b_json_entity_extension_config_4200
 * @tc.name: backup_b_json_entity_extension_config_4200
 * @tc.desc: 测试GetDirList接口能否返回正确参数
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, backup_b_json_entity_extension_config_4200, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin backup_b_json_entity_extension_config_4200";
    try {
        Json::Value jv;
        int value = 0;
        jv.append(value);
        string_view sv1 = R"({test})";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
        auto cache1 = cachedEntity1.Structuralize();
        vector<string> dir;
        EXPECT_TRUE(cache1.GetDirList(jv)[0] == "");

        jv.append("test");
        EXPECT_TRUE(cache1.GetDirList(jv)[0] == "test");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-an exception occurred by GetDirList.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end backup_b_json_entity_extension_config_4200";
}

/**
 * @tc.number: EXT_CONFIG_HAS_ARRAY_001
 * @tc.name: EXT_CONFIG_HAS_ARRAY_001
 * @tc.desc: Test function of HasArray
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_HAS_ARRAY_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_HAS_ARRAY_001";
    GTEST_LOG_(INFO) << "1. test not obj";
    string_view sv1 = R"(test)";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_FALSE(cache1.HasArray(BConstants::COMPATIBLE_DIR_MAPPING));
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_HAS_ARRAY_001";
}

/**
 * @tc.number: EXT_CONFIG_HAS_ARRAY_002
 * @tc.name: EXT_CONFIG_HAS_ARRAY_002
 * @tc.desc: Test function of HasArray
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_HAS_ARRAY_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_HAS_ARRAY_002";
    GTEST_LOG_(INFO) << "2. test no key";
    string_view sv1 = R"({"abc": 123})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_FALSE(cache1.HasArray(BConstants::COMPATIBLE_DIR_MAPPING));
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_HAS_ARRAY_002";
}

/**
 * @tc.number: EXT_CONFIG_HAS_ARRAY_003
 * @tc.name: EXT_CONFIG_HAS_ARRAY_003
 * @tc.desc: Test function of HasArray
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_HAS_ARRAY_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_HAS_ARRAY_003";
    GTEST_LOG_(INFO) << "3. test content not array";
    string_view sv1 = R"({"compatibleDirMapping": 123})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_FALSE(cache1.HasArray(BConstants::COMPATIBLE_DIR_MAPPING));
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_HAS_ARRAY_003";
}

/**
 * @tc.number: EXT_CONFIG_HAS_ARRAY_004
 * @tc.name: EXT_CONFIG_HAS_ARRAY_004
 * @tc.desc: Test function of HasArray
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_HAS_ARRAY_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_HAS_ARRAY_004";
    GTEST_LOG_(INFO) << "4. test ok";
    string_view sv1 = R"({"compatibleDirMapping": []})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_TRUE(cache1.HasArray(BConstants::COMPATIBLE_DIR_MAPPING));
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_HAS_ARRAY_004";
}

/**
 * @tc.number: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_001
 * @tc.name: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_001
 * @tc.desc: Test function of GetCompatibleDirMapping fail 1
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_GET_COMPAT_DIR_MAPPING_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_GET_COMPAT_DIR_MAPPING_001";
    GTEST_LOG_(INFO) << "1. test HasCompatibleDirMapping false";
    string_view sv1 = R"({"compatibleDirMapping": 123})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirMapping().size(), 0);
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_GET_COMPAT_DIR_MAPPING_001";
}

/**
 * @tc.number: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_002
 * @tc.name: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_002
 * @tc.desc: Test function of GetCompatibleDirMapping item fail 1
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_GET_COMPAT_DIR_MAPPING_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_GET_COMPAT_DIR_MAPPING_002";
    GTEST_LOG_(INFO) << "2. test item not object";
    string_view sv1 = R"({"compatibleDirMapping": ["abc"]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirMapping().size(), 0);
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_GET_COMPAT_DIR_MAPPING_002";
}

/**
 * @tc.number: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_003
 * @tc.name: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_003
 * @tc.desc: Test function of GetCompatibleDirMapping item fail 2
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_GET_COMPAT_DIR_MAPPING_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_GET_COMPAT_DIR_MAPPING_003";
    GTEST_LOG_(INFO) << "3. test item empty";
    string_view sv1 = R"({"compatibleDirMapping": [{}]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirMapping().size(), 0);
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_GET_COMPAT_DIR_MAPPING_003";
}

/**
 * @tc.number: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_004
 * @tc.name: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_004
 * @tc.desc: Test function of GetCompatibleDirMapping item fail 3
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_GET_COMPAT_DIR_MAPPING_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_GET_COMPAT_DIR_MAPPING_004";
    GTEST_LOG_(INFO) << "4. test item not contain key backupDir";
    string_view sv1 = R"({"compatibleDirMapping": [{"abc": 123}]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirMapping().size(), 0);
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_GET_COMPAT_DIR_MAPPING_004";
}

/**
 * @tc.number: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_005
 * @tc.name: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_005
 * @tc.desc: Test function of GetCompatibleDirMapping item fail 4
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_GET_COMPAT_DIR_MAPPING_005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_GET_COMPAT_DIR_MAPPING_005";
    GTEST_LOG_(INFO) << "5. test item not contain key restoreDir";
    string_view sv1 = R"({"compatibleDirMapping": [{"backupDir": "123"}]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirMapping().size(), 0);
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_GET_COMPAT_DIR_MAPPING_005";
}

/**
 * @tc.number: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_006
 * @tc.name: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_006
 * @tc.desc: Test function of GetCompatibleDirMapping item fail 5
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_GET_COMPAT_DIR_MAPPING_006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_GET_COMPAT_DIR_MAPPING_006";
    GTEST_LOG_(INFO) << "6. test item restoreDir contain seperator";
    string_view sv1 = R"({"compatibleDirMapping": [{"backupDir": "123", "restoreDir": "456||||"}]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirMapping().size(), 0);

    string_view sv2 = R"({"compatibleDirMapping": [{"backupDir": "123", "restoreDir": "456*"}]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity2(sv2);
    auto cache2 = cachedEntity2.Structuralize();
    EXPECT_EQ(cache2.GetCompatibleDirMapping().size(), 0);
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_GET_COMPAT_DIR_MAPPING_006";
}

/**
 * @tc.number: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_007
 * @tc.name: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_007
 * @tc.desc: Test function of GetCompatibleDirMapping item fail 6
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_GET_COMPAT_DIR_MAPPING_007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_GET_COMPAT_DIR_MAPPING_007";
    GTEST_LOG_(INFO) << "7. test item backupDir contain seperator";
    string_view sv1 = R"({"compatibleDirMapping": [{"backupDir": "123||||", "restoreDir": "456"}]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirMapping().size(), 0);

    string_view sv2 = R"({"compatibleDirMapping": [{"backupDir": "123*", "restoreDir": "456"}]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity2(sv2);
    auto cache2 = cachedEntity2.Structuralize();
    EXPECT_EQ(cache2.GetCompatibleDirMapping().size(), 0);
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_GET_COMPAT_DIR_MAPPING_007";
}

/**
 * @tc.number: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_008
 * @tc.name: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_008
 * @tc.desc: Test function of GetCompatibleDirMapping ok
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_GET_COMPAT_DIR_MAPPING_008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_GET_COMPAT_DIR_MAPPING_008";
    GTEST_LOG_(INFO) << "8. test all ok";
    string_view sv1 = R"({"compatibleDirMapping": [{"backupDir": "123", "restoreDir": "456"}]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirMapping().size(), 1);
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_GET_COMPAT_DIR_MAPPING_008";
}

/**
 * @tc.number: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_009
 * @tc.name: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_009
 * @tc.desc: Test function of GetCompatibleDirMapping repeat key
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_GET_COMPAT_DIR_MAPPING_009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_GET_COMPAT_DIR_MAPPING_009";
    GTEST_LOG_(INFO) << "9. test all ok";
    string sv1 = "{\"compatibleDirMapping\": [{\"backupDir\": \"123\", \"restoreDir\": \"456\"},"
        "{\"backupDir\": \"123\", \"restoreDir\": \"456\"}]}";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirMapping().size(), 1);
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_GET_COMPAT_DIR_MAPPING_009";
}

/**
 * @tc.number: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_010
 * @tc.name: EXT_CONFIG_GET_COMPAT_DIR_MAPPING_010
 * @tc.desc: Test function of GetCompatibleDirMapping over max count
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityExtensionConfigTest, EXT_CONFIG_GET_COMPAT_DIR_MAPPING_010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-begin EXT_CONFIG_GET_COMPAT_DIR_MAPPING_010";
    GTEST_LOG_(INFO) << "10. test all ok";
    string config = R"({"compatibleDirMapping": [{"backupDir": "123", "restoreDir": "456"})";
    for (int i = 0; i < BConstants::MAX_COMPAT_DIR_COUNT; i++) {
        config.append(", {\"backupDir\": \"123\", \"restoreDir\": \"456" + to_string(i) + "\"}");
    }
    config.append("]}");
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(config);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirMapping().size(), 0);
    GTEST_LOG_(INFO) << "BJsonEntityExtensionConfigTest-end EXT_CONFIG_GET_COMPAT_DIR_MAPPING_010";
}
} // namespace OHOS::FileManagement::Backup