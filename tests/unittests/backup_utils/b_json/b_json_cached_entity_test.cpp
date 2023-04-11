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
} // namespace OHOS::FileManagement::Backup