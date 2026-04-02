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
    void SetUp() override {
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
            EXPECT_EQ(statInfo.Marshalling(parcel), true);
        } else {
            EXPECT_EQ(statInfo.Marshalling(parcel), false);
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
    EXPECT_EQ(statInfo.Marshalling(parcel), true);

    // ReadFromParcel
    statInfo.ReadFromParcel(parcel);
    GTEST_LOG_(INFO) << "StatInfoTest-end SUB_StatInfo_ReadFromParcel_0100";
}

class AncoRestoreResultTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override {
        ResetParcelState();
    };
    void TearDown() override {};
};

/**
* @tc.number: SUB_AncoRestoreResult_Marshalling_0100
* @tc.name: SUB_AncoRestoreResult_Marshalling_0100
* @tc.desc: Test function of Marshalling interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoRestoreResultTest, SUB_AncoRestoreResult_Marshalling_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-begin SUB_AncoRestoreResult_Marshalling_0100";
    std::map<std::string, int64_t> endFileInfos;
    std::map<std::string, std::vector<ErrCode>> errFileInfos;
    std::vector<ErrCode> errCodes;
    errCodes.push_back(BError(BError::Codes::OK));
    endFileInfos.emplace("123", 123);
    errFileInfos.emplace("123", errCodes);
    AncoRestoreResult result(1, 2, 3, endFileInfos, errFileInfos);
    Parcel parcel;
    // marshalling
    constexpr uint64_t mask1 = 0x1e; // string, int64, uint64, int32
    for (uint64_t i = 0; i < MAX_STATUS_NUM; i++) {
        SetParcelState(i);
        if (CanCaseSuccess(mask1, i)) {
            EXPECT_EQ(result.Marshalling(parcel), true);
        } else {
            EXPECT_EQ(result.Marshalling(parcel), false);
        }
    }
    // unmarshalling
    constexpr uint64_t mask2 = 0xc; // int64, uint64
    for (uint64_t i = 0; i < MAX_STATUS_NUM; i++) {
        SetParcelState(i);
        if (CanCaseSuccess(mask2, i)) {
            EXPECT_NE(result.Unmarshalling(parcel), nullptr);
        } else {
            EXPECT_EQ(result.Unmarshalling(parcel), nullptr);
        }
    }
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-end SUB_AncoRestoreResult_Marshalling_0100";
}

/**
* @tc.number: SUB_AncoRestoreResult_Marshalling_0101
* @tc.name: SUB_AncoRestoreResult_Marshalling_0101
* @tc.desc: Test function of Marshalling interface for FAILURE.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoRestoreResultTest, SUB_AncoRestoreResult_Marshalling_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-begin SUB_AncoRestoreResult_Marshalling_0101";
    std::map<std::string, int64_t> endFileInfos;
    std::map<std::string, std::vector<ErrCode>> errFileInfos;
    std::vector<ErrCode> errCodes;
    errCodes.push_back(BError(BError::Codes::OK));
    endFileInfos.emplace("123", 123);
    errFileInfos.emplace("123", errCodes);
    AncoRestoreResult result(1, 2, 3, endFileInfos, errFileInfos);
    Parcel parcel;
    parcel.SetEasyMode(false);
    // marshalling
    std::vector<bool> sequence = {
        true, // WriteInt64
        true, // WriteInt64
        true, // WriteInt64
        true, // WriteMap - WriteUint64
        true, // WriteMap - WriteString
        true, // WriteMap - WriteInt64
        false, // WriteMap2 - WriteUint64
    };
    SetSpecialParcelSequence(sequence);
    EXPECT_NE(result.Marshalling(parcel), true);
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-end SUB_AncoRestoreResult_Marshalling_0101";
}

/**
* @tc.number: SUB_AncoRestoreResult_Marshalling_0102
* @tc.name: SUB_AncoRestoreResult_Marshalling_0102
* @tc.desc: Test function of Marshalling interface for FAILURE.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoRestoreResultTest, SUB_AncoRestoreResult_Marshalling_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-begin SUB_AncoRestoreResult_Marshalling_0102";
    std::map<std::string, int64_t> endFileInfos;
    std::map<std::string, std::vector<ErrCode>> errFileInfos;
    std::vector<ErrCode> errCodes;
    errCodes.push_back(BError(BError::Codes::OK));
    endFileInfos.emplace("123", 123);
    errFileInfos.emplace("123", errCodes);
    AncoRestoreResult result(1, 2, 3, endFileInfos, errFileInfos);
    Parcel parcel;
    parcel.SetEasyMode(false);
    // marshalling
    std::vector<bool> sequence = {
        true, // WriteInt64
        true, // WriteInt64
        true, // WriteInt64
        true, // WriteMap - WriteUint64
        true, // WriteMap - WriteString
        true, // WriteMap - WriteInt64
        true, // WriteMap2 - WriteUint64
        true, // WriteMap2 - WriteString
        false, // WriteVector - WriteUint64
    };
    SetSpecialParcelSequence(sequence);
    EXPECT_NE(result.Marshalling(parcel), true);
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-end SUB_AncoRestoreResult_Marshalling_0102";
}

/**
* @tc.number: SUB_AncoRestoreResult_ReadFromParcel_0100
* @tc.name: SUB_AncoRestoreResult_ReadFromParcel_0100
* @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoRestoreResultTest, SUB_AncoRestoreResult_ReadFromParcel_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-begin SUB_AncoRestoreResult_ReadFromParcel_0100";
    std::map<std::string, int64_t> endFileInfos;
    std::map<std::string, std::vector<ErrCode>> errFileInfos;
    std::vector<ErrCode> errCodes;
    errCodes.push_back(BError(BError::Codes::OK));
    endFileInfos.emplace("123", 123);
    errFileInfos.emplace("123", errCodes);
    AncoRestoreResult result(1, 2, 3, endFileInfos, errFileInfos);
    Parcel parcel;
    parcel.SetEasyMode(false);
    // marshalling
    EXPECT_EQ(result.Marshalling(parcel), true);
    // unmarshalling
    AncoRestoreResult tempRes;
    tempRes.ReadFromParcel(parcel);
    EXPECT_EQ(tempRes.successCount, result.successCount);
    EXPECT_EQ(tempRes.duplicateCount, result.duplicateCount);
    EXPECT_EQ(tempRes.failedCount, result.failedCount);
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-end SUB_AncoRestoreResult_ReadFromParcel_0100";
}

/**
* @tc.number: SUB_AncoRestoreResult_ReadFromParcel_0101
* @tc.name: SUB_AncoRestoreResult_ReadFromParcel_0101
* @tc.desc: Test function of ReadFromParcel interface for FAILURE.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoRestoreResultTest, SUB_AncoRestoreResult_ReadFromParcel_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-begin SUB_AncoRestoreResult_ReadFromParcel_0101";
    std::map<std::string, int64_t> endFileInfos;
    std::map<std::string, std::vector<ErrCode>> errFileInfos;
    std::vector<ErrCode> errCodes;
    errCodes.push_back(BError(BError::Codes::OK));
    endFileInfos.emplace("123", 123);
    errFileInfos.emplace("123", errCodes);
    AncoRestoreResult result(1, 2, 3, endFileInfos, errFileInfos);
    Parcel parcel;
    parcel.SetEasyMode(false);
    // marshalling
    EXPECT_EQ(result.Marshalling(parcel), true);
    // unmarshalling
    std::vector<bool> sequence = {
        true, // ReadInt64
        true, // ReadInt64
        true, // ReadInt64
        true, // ReadMap - ReadUint64
        true, // ReadMap - ReadString
        true, // ReadMap - ReadInt64
        false, // ReadMap2 - ReadUint64
    };
    SetSpecialParcelSequence(sequence);
    AncoRestoreResult tempRes;
    EXPECT_EQ(tempRes.Unmarshalling(parcel), nullptr);
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-end SUB_AncoRestoreResult_ReadFromParcel_0101";
}

/**
* @tc.number: SUB_AncoRestoreResult_ReadFromParcel_0102
* @tc.name: SUB_AncoRestoreResult_ReadFromParcel_0102
* @tc.desc: Test function of ReadFromParcel interface for FAILURE.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoRestoreResultTest, SUB_AncoRestoreResult_ReadFromParcel_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-begin SUB_AncoRestoreResult_ReadFromParcel_0102";
    std::map<std::string, int64_t> endFileInfos;
    std::map<std::string, std::vector<ErrCode>> errFileInfos;
    std::vector<ErrCode> errCodes;
    errCodes.push_back(BError(BError::Codes::OK));
    endFileInfos.emplace("123", 123);
    errFileInfos.emplace("123", errCodes);
    AncoRestoreResult result(1, 2, 3, endFileInfos, errFileInfos);
    Parcel parcel;
    parcel.SetEasyMode(false);
    // marshalling
    EXPECT_EQ(result.Marshalling(parcel), true);
    // unmarshalling
    std::vector<bool> sequence = {
        true, // ReadInt64
        true, // ReadInt64
        true, // ReadInt64
        true, // ReadMap - ReadUint64
        true, // ReadMap - ReadString
        true, // ReadMap - ReadInt64
        true, // ReadMap2 - ReadUint64
        true, // ReadMap2 - ReadString
        false, // ReadVector - ReadUint64
    };
    SetSpecialParcelSequence(sequence);
    AncoRestoreResult tempRes;
    EXPECT_EQ(tempRes.Unmarshalling(parcel), nullptr);
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-end SUB_AncoRestoreResult_ReadFromParcel_0102";
}

/**
* @tc.number: SUB_AncoRestoreResult_ReadFromParcel_0103
* @tc.name: SUB_AncoRestoreResult_ReadFromParcel_0103
* @tc.desc: Test function of ReadFromParcel interface for FAILURE.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: I6F3GV
*/
HWTEST_F(AncoRestoreResultTest, SUB_AncoRestoreResult_ReadFromParcel_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-begin SUB_AncoRestoreResult_ReadFromParcel_0103";
    std::map<std::string, int64_t> endFileInfos;
    std::map<std::string, std::vector<ErrCode>> errFileInfos;
    std::vector<ErrCode> errCodes;
    errCodes.push_back(BError(BError::Codes::OK));
    endFileInfos.emplace("123", 123);
    errFileInfos.emplace("123", errCodes);
    AncoRestoreResult result(1, 2, 3, endFileInfos, errFileInfos);
    Parcel parcel;
    parcel.SetEasyMode(false);
    // marshalling
    EXPECT_EQ(result.Marshalling(parcel), true);
    // unmarshalling
    std::vector<bool> sequence = {
        true, // ReadInt64
        true, // ReadInt64
        true, // ReadInt64
        true, // ReadMap - ReadUint64
        true, // ReadMap - ReadString
        true, // ReadMap - ReadInt64
        true, // ReadMap2 - ReadUint64
        true, // ReadMap2 - ReadString
        true, // ReadVector - ReadUint64
        false, // ReadVector - ReadInt32
    };
    SetSpecialParcelSequence(sequence);
    AncoRestoreResult tempRes;
    EXPECT_EQ(tempRes.Unmarshalling(parcel), nullptr);
    GTEST_LOG_(INFO) << "AncoRestoreResultTest-end SUB_AncoRestoreResult_ReadFromParcel_0103";
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
            EXPECT_EQ(result.Marshalling(parcel), true);
        } else {
            EXPECT_EQ(result.Marshalling(parcel), false);
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
    EXPECT_EQ(result.Marshalling(parcel), true);

    // unmarshalling
    result.ReadFromParcel(parcel);
    GTEST_LOG_(INFO) << "AncoScanResultTest-end SUB_AncoScanResult_ReadFromParcel_0100";
}
} // namespace OHOS::FileManagement::Backup