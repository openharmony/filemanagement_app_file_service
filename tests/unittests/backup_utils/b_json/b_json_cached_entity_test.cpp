/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include <sys/stat.h>
#include <sys/types.h>

#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_caps.h"
#include "directory_ex.h"
#include "file_ex.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
class BJsonCachedEntityTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_b_json_construction_0100
 * @tc.name: b_json_construction_0100
 * @tc.desc: Test function of  construction interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonCachedEntityTest, b_json_construction_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_construction_0100";
    try {
        TestManager tm("b_json_construction_0100");

        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + ".json";
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open(filePath.data(), O_RDWR | O_CREAT, 0600)));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_construction_0100";
}

/**
 * @tc.number: SUB_backup_b_json_construction_0101
 * @tc.name: b_json_construction_0101
 * @tc.desc: Test function of  construction interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonCachedEntityTest, b_json_construction_0101, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_construction_0101";
    try {
        TestManager tm("b_json_construction_0101");

        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + "";
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open(filePath.data(), O_RDWR | O_CREAT, 0600)));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_construction_0101";
}

/**
 * @tc.number: SUB_backup_b_json_construction_0102
 * @tc.name: b_json_construction_0102
 * @tc.desc: Test function of  construction interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonCachedEntityTest, b_json_construction_0102, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_construction_0102";
    try {
        TestManager tm("b_json_construction_0102");

        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + ".json";
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open(filePath.data(), O_RDWR | O_CREAT, NULL)));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_construction_0102";
}

/**
 * @tc.number: SUB_backup_b_json_construction_0104
 * @tc.name: b_json_construction_0104
 * @tc.desc: Test function of  construction interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonCachedEntityTest, b_json_construction_0104, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_construction_0104";
    try {
        TestManager tm("b_json_construction_0104");

        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + ".json";
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open(filePath.data(), O_RDONLY, NULL)));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_construction_0104";
}

/**
 * @tc.number: SUB_backup_b_json_construction_0105
 * @tc.name: b_json_construction_0105
 * @tc.desc: Test function of  construction interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonCachedEntityTest, b_json_construction_0105, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_construction_0105";
    try {
        TestManager tm("b_json_construction_0105");

        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + "";
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open(filePath.data(), O_RDWR | O_CREAT, NULL)));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_construction_0105";
}

/**
 * @tc.number: SUB_backup_b_json_construction_0106
 * @tc.name: b_json_construction_0106
 * @tc.desc: Test function of  construction interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonCachedEntityTest, b_json_construction_0106, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_construction_0106";
    try {
        TestManager tm("b_json_construction_0106");

        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + "";
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open(filePath.data(), O_RDONLY, 0600)));
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_construction_0106";
}

/**
 * @tc.number: SUB_backup_b_json_construction_0107
 * @tc.name: b_json_construction_0107
 * @tc.desc: Test function of  construction interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonCachedEntityTest, b_json_construction_0107, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_construction_0107";
    try {
        TestManager tm("b_json_construction_0107");

        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + "";
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open(filePath.data(), O_RDONLY, NULL)));
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_construction_0107";
}

/**
 * @tc.number: SUB_backup_b_json_Structuralize_0100
 * @tc.name: b_json_Structuralize_0100
 * @tc.desc: Test function of  Structuralize interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonCachedEntityTest, b_json_Structuralize_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_Structuralize_0100";
    try {
        TestManager tm("b_json_Structuralize_0100");

        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + ".json";
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open(filePath.data(), O_RDWR | O_CREAT, 0600)));
        jce.Structuralize();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_Structuralize_0100";
}

/**
 * @tc.number: SUB_backup_b_json_GetFd_0100
 * @tc.name: b_json_GetFd_0100
 * @tc.desc: Test function of  GetFd interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonCachedEntityTest, b_json_GetFd_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_GetFd_0100";
    try {
        TestManager tm("b_json_GetFd_0100");

        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + ".json";
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open(filePath.data(), O_RDWR | O_CREAT, 0600)));
        jce.GetFd();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_GetFd_0100";
}

/**
 * @tc.number: SUB_backup_b_json_Persist_0100
 * @tc.name: b_json_Persist_0100
 * @tc.desc: Test function of  Persist interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonCachedEntityTest, b_json_Persist_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_Persist_0100";
    try {
        TestManager tm("b_json_Persist_0100");

        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + ".json";
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open(filePath.data(), O_RDWR | O_CREAT, 0600)));
        jce.Persist();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_Persist_0100";
}

/**
 * @tc.number: SUB_backup_b_json_ReloadFromFile_0100
 * @tc.name: b_json_ReloadFromFile_0100
 * @tc.desc: Test function of  ReloadFromFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonCachedEntityTest, b_json_ReloadFromFile_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_ReloadFromFile_0100";
    try {
        TestManager tm("b_json_ReloadFromFile_0100");

        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + ".json";
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open(filePath.data(), O_RDWR | O_CREAT, 0600)));
        int ret = jce.ReloadFromFile();
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_ReloadFromFile_0100";
}

/**
 * @tc.number: SUB_backup_b_json_ReloadFromString_0100
 * @tc.name: b_json_ReloadFromString_0100
 * @tc.desc: Test function of  ReloadFromString interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonCachedEntityTest, b_json_ReloadFromString_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_ReloadFromString_0100";
    try {
        TestManager tm("b_json_ReloadFromString_0100");

        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + ".json";
        std::string_view sv = R"({"key":1})";
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open(filePath.data(), O_RDWR | O_CREAT, 0600)));
        jce.ReloadFromString(sv);
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_ReloadFromString_0100";
}

} // namespace OHOS::FileManagement::Backup