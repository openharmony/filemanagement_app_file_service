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

#include <gtest/gtest.h>

#include "parcel.h"
#include "stat_info.h"
#include "anco_restore_result.h"
#include "anco_scan_result.h"
#include "b_error/b_error.h"
#include "b_utils/string_utils.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

constexpr uint64_t MAX_STATUS_NUM = 1 << 7;
static void SetParcelState(uint64_t statusCode)
{
    MockWriteUint32(statusCode & 1);
    statusCode >>= 1;
    MockWriteInt32(statusCode & 1);
    statusCode >>= 1;
    MockWriteUint64(statusCode & 1);
    statusCode >>= 1;
    MockWriteInt64(statusCode & 1);
    statusCode >>= 1;
    MockWriteString(statusCode & 1, 0);
    statusCode >>= 1;
    MockWriteParcelable(statusCode & 1);
    statusCode >>= 1;
    MockReadParcelable(statusCode & 1);
}

static bool CanCaseSuccess(uint64_t mask, uint64_t statusCode)
{
    return (mask & statusCode) == mask;
}

class StatInfoTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override
    {
        ResetParcelState();
    };
    void TearDown() override {};
};

/**
* @tc.number: SUB_StatInfo_Marshalling_0100
* @tc.name: SUB_StatInfo_Marshalling_0100
* @tc.desc: Test function of Marshalling interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(StatInfoTest, SUB_StatInfo_Marshalling_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StatInfoTest-begin SUB_StatInfo_Marshalling_0100";
    struct stat sta;
    StatInfo statInfo(sta);
    Parcel parcel;
    // marshalling
    constexpr uint64_t mask = 0x9; // int64, uint32
    for (uint64_t i = 0; i < MAX_STATUS_NUM; i++) {
        SetParcelState(i);
        if (CanCaseSuccess(mask, i)) {
            EXPECT_TRUE(statInfo.Marshalling(parcel));
        } else {
            EXPECT_FALSE(statInfo.Marshalling(parcel));
        }
    }
    // unmarshalling
    for (uint64_t i = 0; i < MAX_STATUS_NUM; i++) {
        SetParcelState(i);
        if (CanCaseSuccess(mask, i)) {
            EXPECT_NE(statInfo.Unmarshalling(parcel), nullptr);
        } else {
            EXPECT_EQ(statInfo.Unmarshalling(parcel), nullptr);
        }
    }
    GTEST_LOG_(INFO) << "StatInfoTest-end SUB_StatInfo_Marshalling_0100";
}

/**
* @tc.number: SUB_StatInfo_ReadFromParcel_0100
* @tc.name: SUB_StatInfo_ReadFromParcel_0100
* @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(StatInfoTest, SUB_StatInfo_ReadFromParcel_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StatInfoTest-begin SUB_StatInfo_ReadFromParcel_0100";
    struct stat sta;
    StatInfo statInfo(sta);
    Parcel parcel;
    // marshalling
    EXPECT_TRUE(statInfo.Marshalling(parcel));

    // ReadFromParcel
    statInfo.ReadFromParcel(parcel);
    GTEST_LOG_(INFO) << "StatInfoTest-end SUB_StatInfo_ReadFromParcel_0100";
}

class AncoScanResultTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override
    {
        ResetParcelState();
    };
    void TearDown() override {};
};

/**
* @tc.number: SUB_AncoScanResult_Marshalling_0100
* @tc.name: SUB_AncoScanResult_Marshalling_0100
* @tc.desc: Test function of Marshalling interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoScanResultTest, SUB_AncoScanResult_Marshalling_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AncoScanResultTest-begin SUB_AncoScanResult_Marshalling_0100";
    AncoScanResult result(1, 2);
    Parcel parcel;
    // marshalling
    constexpr uint64_t mask = 0x8; // int64
    for (uint64_t i = 0; i < MAX_STATUS_NUM; i++) {
        SetParcelState(i);
        if (CanCaseSuccess(mask, i)) {
            EXPECT_TRUE(result.Marshalling(parcel));
        } else {
            EXPECT_FALSE(result.Marshalling(parcel));
        }
    }
    // unmarshalling
    for (uint64_t i = 0; i < MAX_STATUS_NUM; i++) {
        SetParcelState(i);
        if (CanCaseSuccess(mask, i)) {
            EXPECT_NE(result.Unmarshalling(parcel), nullptr);
        } else {
            EXPECT_EQ(result.Unmarshalling(parcel), nullptr);
        }
    }
    GTEST_LOG_(INFO) << "AncoScanResultTest-end SUB_AncoScanResult_Marshalling_0100";
}

/**
* @tc.number: SUB_AncoScanResult_ReadFromParcel_0100
* @tc.name: SUB_AncoScanResult_ReadFromParcel_0100
* @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoScanResultTest, SUB_AncoScanResult_ReadFromParcel_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AncoScanResultTest-begin SUB_AncoScanResult_ReadFromParcel_0100";
    AncoScanResult result(1, 2);
    Parcel parcel;
    // marshalling
    EXPECT_TRUE(result.Marshalling(parcel));

    // unmarshalling
    result.ReadFromParcel(parcel);
    GTEST_LOG_(INFO) << "AncoScanResultTest-end SUB_AncoScanResult_ReadFromParcel_0100";
}

/**
* @tc.number: StringUtils_IsAncoFile_0000
* @tc.name: StringUtils_IsAncoFile_0000
* @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoScanResultTest, StringUtils_IsAncoFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AncoScanResultTest-begin StringUtils_IsAncoFile_0000";
    EXPECT_FALSE(StringUtils::IsAncoFile("BgncqicvZqoSe0SC0mVxMyIhnVjlF2YCE"));
    EXPECT_FALSE(StringUtils::IsAncoFile("Bv1Pp2HfJ0lMmhZxc13lB8rVDlWj5yfhT.tar"));
    EXPECT_FALSE(StringUtils::IsAncoFile("BgncqicvZqoSe0SC0mVxMyIhnVjlF2YC_"));
    EXPECT_FALSE(StringUtils::IsAncoFile("Bv1Pp2HfJ0lMmhZxc13lB8rVDlWj5yfh_.tar"));
    EXPECT_FALSE(StringUtils::IsAncoFile("BgncqicvZqoSe0SC0mVxMyIhnVjl_anco"));
    EXPECT_FALSE(StringUtils::IsAncoFile("BgncqicvZqoSe0SC0mVxMyIhnVjl_anco.tar"));
    EXPECT_TRUE(StringUtils::IsAncoFile("c2ef43d4b51e3282_anco"));
    EXPECT_FALSE(StringUtils::IsAncoFile("c2ef43d4b51e3282"));
    EXPECT_TRUE(StringUtils::IsAncoFile("part_anco.0.tar"));
    EXPECT_FALSE(StringUtils::IsAncoFile("part.0.tar"));
    EXPECT_FALSE(StringUtils::IsAncoFile("c2ef43d4b51e3282.tar_anco_aa"));
    EXPECT_TRUE(StringUtils::IsAncoFile("c2ef43d4b51e3282_anco.tar_aa"));
    GTEST_LOG_(INFO) << "AncoScanResultTest-end StringUtils_IsAncoFile_0000";
}

/**
* @tc.number: StringUtils_RemoveFileExtension_0000
* @tc.name: StringUtils_RemoveFileExtension_0000
* @tc.desc: Test function of RemoveFileExtension.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoScanResultTest, StringUtils_RemoveFileExtension_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AncoScanResultTest-begin StringUtils_RemoveFileExtension_0000";
    EXPECT_EQ(StringUtils::RemoveFileExtension("Bv1Pp2HfrVDlWj5yfhT.tar"), "Bv1Pp2HfrVDlWj5yfhT");
    EXPECT_EQ(StringUtils::RemoveFileExtension("c2ef43d4b51e3282"), "c2ef43d4b51e3282");
    EXPECT_EQ(StringUtils::RemoveFileExtension("c2ef43d4b51e3282_anco.tar_anco"), "c2ef43d4b51e3282_anco");
    GTEST_LOG_(INFO) << "AncoScanResultTest-end StringUtils_RemoveFileExtension_0000";
}
} // namespace OHOS::FileManagement::Backup