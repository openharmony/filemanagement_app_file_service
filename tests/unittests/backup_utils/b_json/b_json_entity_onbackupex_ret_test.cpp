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

#include <cstdio>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <string_view>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "b_json/b_json_entity_onbackupex_ret.h"
#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BJsonEntityOnBackupExRetTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: ONBACKUPEX_RET_GET_COMPAT_DIRS_001
 * @tc.name: ONBACKUPEX_RET_GET_COMPAT_DIRS_001
 * @tc.desc: Test function of GetCompatibleDirs fail 1
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityOnBackupExRetTest, ONBACKUPEX_RET_GET_COMPAT_DIRS_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityOnBackupExRetTest-begin ONBACKUPEX_RET_GET_COMPAT_DIRS_001";
    GTEST_LOG_(INFO) << "1. test HasArray false";
    string_view sv1 = R"({"compatibleDirMapping": 123})";
    BJsonCachedEntity<BJsonEntityOnBackupExRet> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirs().size(), 0);
    GTEST_LOG_(INFO) << "BJsonEntityOnBackupExRetTest-end ONBACKUPEX_RET_GET_COMPAT_DIRS_001";
}

/**
 * @tc.number: ONBACKUPEX_RET_GET_COMPAT_DIRS_002
 * @tc.name: ONBACKUPEX_RET_GET_COMPAT_DIRS_002
 * @tc.desc: Test function of GetCompatibleDirs item fail 1
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityOnBackupExRetTest, ONBACKUPEX_RET_GET_COMPAT_DIRS_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityOnBackupExRetTest-begin ONBACKUPEX_RET_GET_COMPAT_DIRS_002";
    GTEST_LOG_(INFO) << "2. test item not string";
    string_view sv1 = R"({"compatibleDirMapping": [{}]})";
    BJsonCachedEntity<BJsonEntityOnBackupExRet> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirs().size(), 0);
    GTEST_LOG_(INFO) << "BJsonEntityOnBackupExRetTest-end ONBACKUPEX_RET_GET_COMPAT_DIRS_002";
}

/**
 * @tc.number: ONBACKUPEX_RET_GET_COMPAT_DIRS_003
 * @tc.name: ONBACKUPEX_RET_GET_COMPAT_DIRS_003
 * @tc.desc: Test function of GetCompatibleDirs item fail 2
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityOnBackupExRetTest, ONBACKUPEX_RET_GET_COMPAT_DIRS_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityOnBackupExRetTest-begin ONBACKUPEX_RET_GET_COMPAT_DIRS_003";
    GTEST_LOG_(INFO) << "3. test item empty";
    string_view sv1 = R"({"compatibleDirMapping": [""]})";
    BJsonCachedEntity<BJsonEntityOnBackupExRet> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirs().size(), 0);
    GTEST_LOG_(INFO) << "BJsonEntityOnBackupExRetTest-end ONBACKUPEX_RET_GET_COMPAT_DIRS_003";
}

/**
 * @tc.number: ONBACKUPEX_RET_GET_COMPAT_DIRS_004
 * @tc.name: ONBACKUPEX_RET_GET_COMPAT_DIRS_004
 * @tc.desc: Test function of GetCompatibleDirs ok
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BJsonEntityOnBackupExRetTest, ONBACKUPEX_RET_GET_COMPAT_DIRS_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityOnBackupExRetTest-begin ONBACKUPEX_RET_GET_COMPAT_DIRS_004";
    GTEST_LOG_(INFO) << "4. test ok";
    string_view sv1 = R"({"compatibleDirMapping": ["abc", "123"]})";
    BJsonCachedEntity<BJsonEntityOnBackupExRet> cachedEntity1(sv1);
    auto cache1 = cachedEntity1.Structuralize();
    EXPECT_EQ(cache1.GetCompatibleDirs().size(), 2);
    GTEST_LOG_(INFO) << "BJsonEntityOnBackupExRetTest-end ONBACKUPEX_RET_GET_COMPAT_DIRS_004";
}
} // namespace OHOS::FileManagement::Backup