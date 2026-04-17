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
#include "b_error/b_error.h"
#include "stat_info.h"
#include "anco_restore_result.h"
#include "anco_scan_result.h"
#include "cJsonMock.h"

#include "cjson_func_define.h"
#include "anco_restore_result.cpp"
#include "cjson_func_undef.h"
#include "anco_scan_result.cpp"
#include "stat_info.cpp"

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

class AncoRestoreResultTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override
    {
        cJsonMock = make_shared<CJsonMock>();
        CJson::cJsonPtr = cJsonMock;
    };
    void TearDown() override
    {
        CJson::cJsonPtr = nullptr;
        cJsonMock = nullptr;
    };

    static inline shared_ptr<CJsonMock> cJsonMock = nullptr;
};

/**
* @tc.number: SUB_AncoRestoreResult_Serialize_0100
* @tc.name: SUB_AncoRestoreResult_Serialize_0100
* @tc.desc: Test function of Serialize interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoRestoreResultTest, SUB_AncoRestoreResult_Serialize_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-begin SUB_AncoRestoreResult_Serialize_0100";
    AncoRestoreResult res;
    auto mockRoot = make_shared<cJSON>();

    EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(nullptr));
    res.Serialize();

    std::vector<ErrCode> codes;
    codes.push_back(BError(BError::Codes::OK));
    res.endFileInfos.emplace("1", 0);
    res.errFileInfos.emplace("1", codes);
    EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillRepeatedly(Return(mockRoot.get()));
    EXPECT_CALL(*cJsonMock, cJSON_AddNumberToObject(_, _, _)).WillRepeatedly(Return(mockRoot.get()));
    EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillRepeatedly(Return(mockRoot.get()));
    EXPECT_CALL(*cJsonMock, cJSON_AddItemToArray(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillRepeatedly(Return(const_cast<char*>("{}")));

    res.Serialize();
    EXPECT_NE(res.data, nullptr);

    EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(mockRoot.get()));
    EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillRepeatedly(Return(mockRoot.get()));
    EXPECT_EQ(res.RawDataCpy(res.data), 0);

    GTEST_LOG_(INFO) << "AncoRestoreResultTest-end SUB_AncoRestoreResult_Serialize_0100";
}

/**
* @tc.number: SUB_AncoRestoreResult_Serialize_0200
* @tc.name: SUB_AncoRestoreResult_Serialize_0200
* @tc.desc: Test function of Serialize interface for SUCCESS with empty data.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoRestoreResultTest, SUB_AncoRestoreResult_Serialize_0200, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-begin SUB_AncoRestoreResult_Serialize_0200";
    AncoRestoreResult res;
    auto mockRoot = make_shared<cJSON>();

    EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillRepeatedly(Return(mockRoot.get()));
    EXPECT_CALL(*cJsonMock, cJSON_AddNumberToObject(_, _, _)).WillRepeatedly(Return(mockRoot.get()));
    EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillRepeatedly(Return(mockRoot.get()));
    EXPECT_CALL(*cJsonMock, cJSON_AddItemToArray(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(const_cast<char*>("{}")));

    res.Serialize();
    EXPECT_NE(res.data, nullptr);

    GTEST_LOG_(INFO) << "AncoRestoreResultTest-end SUB_AncoRestoreResult_Serialize_0200";
}

/**
* @tc.number: SUB_AncoRestoreResult_RawDataCpy_0100
* @tc.name: SUB_AncoRestoreResult_RawDataCpy_0100
* @tc.desc: Test function of RawDataCpy interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoRestoreResultTest, SUB_AncoRestoreResult_RawDataCpy_0100, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-begin SUB_AncoRestoreResult_RawDataCpy_0100";
    AncoRestoreResult res;
    auto mockRoot = make_shared<cJSON>();

    EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(mockRoot.get()));
    EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillRepeatedly(Return(mockRoot.get()));
    EXPECT_CALL(*cJsonMock, cJSON_IsNumber(_)).WillRepeatedly(Return(true));

    res.RawDataCpy("{}");
    EXPECT_EQ(res.RawDataCpy(nullptr), -1);

    GTEST_LOG_(INFO) << "AncoRestoreResultTest-end SUB_AncoRestoreResult_RawDataCpy_0100";
}

/**
* @tc.number: SUB_AncoRestoreResult_RawDataCpy_0200
* @tc.name: SUB_AncoRestoreResult_RawDataCpy_0200
* @tc.desc: Test function of RawDataCpy interface for FAILURE.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoRestoreResultTest, SUB_AncoRestoreResult_RawDataCpy_0200, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-begin SUB_AncoRestoreResult_RawDataCpy_0200";
    AncoRestoreResult res;

    EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(nullptr));
    EXPECT_EQ(res.RawDataCpy("{}"), -1);

    GTEST_LOG_(INFO) << "AncoRestoreResultTest-end SUB_AncoRestoreResult_RawDataCpy_0200";
}

/**
* @tc.number: SUB_AncoRestoreResult_ParseBasicCounts_0100
* @tc.name: SUB_AncoRestoreResult_ParseBasicCounts_0100
* @tc.desc: Test function of ParseBasicCounts interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoRestoreResultTest, SUB_AncoRestoreResult_ParseBasicCounts_0100, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-begin SUB_AncoRestoreResult_ParseBasicCounts_0100";
    AncoRestoreResult res;
    auto mockRoot = make_shared<cJSON>();

    EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillRepeatedly(Return(mockRoot.get()));
    EXPECT_CALL(*cJsonMock, cJSON_IsNumber(_)).WillRepeatedly(Return(true));

    res.ParseBasicCounts(mockRoot.get());
    EXPECT_TRUE(res.ParseBasicCounts(nullptr));

    GTEST_LOG_(INFO) << "AncoRestoreResultTest-end SUB_AncoRestoreResult_ParseBasicCounts_0100";
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
} // namespace OHOS::FileManagement::Backup