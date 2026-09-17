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
#include "parameters.h"

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
    TimeUtils::amsTimeoutRatio_ = INVALID_RATIO;
    int32_t timeout1 = TimeUtils::GetAmsTimeout();
    EXPECT_GT(TimeUtils::amsTimeoutRatio_, 0);
    int32_t timeout2 = TimeUtils::GetAmsTimeout();
    EXPECT_EQ(timeout1, timeout2);
    GTEST_LOG_(INFO) << "BTimeUtilsTest-end GetAmsTimeout_001";
}

/**
 * @tc.number: GetAmsTimeout_002
 * @tc.name: GetAmsTimeout_002
 * @tc.desc: Test function of GetAmsTimeout
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BTimeUtilsTest, GetAmsTimeout_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTimeUtilsTest-begin GetAmsTimeout_002";
    TimeUtils::amsTimeoutRatio_ = INVALID_RATIO;
    std::string ratio = OHOS::system::GetParameter("persist.sys.abilityms.timeout_unit_time_ratio", "1");
    OHOS::system::SetParameter("persist.sys.abilityms.timeout_unit_time_ratio", "-1");
    EXPECT_EQ(TimeUtils::GetAmsTimeout(), 50 * CONNECT_EXTENSION_TIMEOUT);
    OHOS::system::SetParameter("persist.sys.abilityms.timeout_unit_time_ratio", ratio);
    GTEST_LOG_(INFO) << "BTimeUtilsTest-end GetAmsTimeout_002";
}

/**
 * @tc.number: GetAmsTimeout_003
 * @tc.name: GetAmsTimeout_003
 * @tc.desc: Test function of GetAmsTimeout
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BTimeUtilsTest, GetAmsTimeout_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTimeUtilsTest-begin GetAmsTimeout_003";
    TimeUtils::amsTimeoutRatio_ = INVALID_RATIO;
    std::string ratio = OHOS::system::GetParameter("persist.sys.abilityms.timeout_unit_time_ratio", "1");
    OHOS::system::SetParameter("persist.sys.abilityms.timeout_unit_time_ratio", "100");
    EXPECT_EQ(TimeUtils::GetAmsTimeout(), 50 * CONNECT_EXTENSION_TIMEOUT);
    OHOS::system::SetParameter("persist.sys.abilityms.timeout_unit_time_ratio", ratio);
    GTEST_LOG_(INFO) << "BTimeUtilsTest-end GetAmsTimeout_003";
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

HWTEST_F(BTimeUtilsTest, SpendTime_001, testing::ext::TestSize.Level1)
{
    const int64_t nowS = TimeUtils::GetTimeS();
    const int64_t nowMs = TimeUtils::GetTimeMS();
    const int64_t nowUs = TimeUtils::GetTimeUS();
    EXPECT_GT(nowS, 0);
    EXPECT_GT(nowMs, 0);
    EXPECT_GT(nowUs, 0);

    EXPECT_EQ(TimeUtils::GetSpendSecond(0), 0U);
    EXPECT_EQ(TimeUtils::GetSpendSecond(nowS + 10), 0U);
    EXPECT_LE(TimeUtils::GetSpendSecond(nowS), 1U);
    EXPECT_EQ(TimeUtils::GetSpendMS(0), 0U);
    EXPECT_EQ(TimeUtils::GetSpendMS(nowMs + 10000), 0U);
    EXPECT_LE(TimeUtils::GetSpendMS(nowMs), 1000U);
    EXPECT_EQ(TimeUtils::GetSpendUS(0), 0U);
    EXPECT_EQ(TimeUtils::GetSpendUS(nowUs + 10000000), 0U);
    EXPECT_LE(TimeUtils::GetSpendUS(nowUs), 1000000U);
    EXPECT_FALSE(TimeUtils::GetCurrentTime().empty());
}

HWTEST_F(BTimeUtilsTest, CachedAmsTimeout_001, testing::ext::TestSize.Level1)
{
    TimeUtils::amsTimeoutRatio_ = 2;
    EXPECT_EQ(TimeUtils::GetAmsTimeout(), 2 * CONNECT_EXTENSION_TIMEOUT);
    EXPECT_EQ(TimeUtils::GenAfsTimeout(), 2 * CONNECT_EXTENSION_TIMEOUT + 5);
}
} // namespace OHOS::FileManagement::Backup
