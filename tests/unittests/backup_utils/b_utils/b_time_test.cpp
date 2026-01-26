/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#include "b_utils/b_time.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BTimeUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: GetAmsTimeout_001
 * @tc.name: GetAmsTimeout_001
 * @tc.desc: Test function of GetAmsTimeout
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BTimeUtilsTest, GetAmsTimeout_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTimeUtilsTest-begin GetAmsTimeout_001";
    TimeUtils::amsTimeoutRatio_ = -1;
    int32_t timeout1 = TimeUtils::GetAmsTimeout();
    EXPECT_GT(TimeUtils::amsTimeoutRatio_, 0);
    int32_t timeout2 = TimeUtils::GetAmsTimeout();
    EXPECT_EQ(timeout1, timeout2);
    GTEST_LOG_(INFO) << "BTimeUtilsTest-end GetAmsTimeout_001";
}

/**
 * @tc.number: GenAfsTimeout_001
 * @tc.name: GenAfsTimeout_001
 * @tc.desc: Test function of GenAfsTimeout
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BTimeUtilsTest, GenAfsTimeout_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTimeUtilsTest-begin GenAfsTimeout_001";
    int32_t timeout1 = TimeUtils::GetAmsTimeout();
    int32_t timeout2 = TimeUtils::GenAfsTimeout();
    EXPECT_EQ(timeout2 - timeout1, 5);
    GTEST_LOG_(INFO) << "BTimeUtilsTest-end GenAfsTimeout_001";
}
} // namespace OHOS::FileManagement::Backup