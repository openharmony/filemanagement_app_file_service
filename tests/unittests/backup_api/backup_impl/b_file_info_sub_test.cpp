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

#include <gtest/gtest.h>

#include "b_file_info.h"
#include "parcel.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string FILE_NAME = "1.tar";
} // namespace

class BFileInfoSubTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.number: SUB_BFile_Info_Unmarshalling_0100
 * @tc.name: SUB_BFile_Info_Unmarshalling_0100
 * @tc.desc: Test function of Unmarshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoSubTest, SUB_BFile_Info_Unmarshalling_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoSubTest-begin SUB_BFile_Info_Unmarshalling_0100";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    // unmarshalling
    BFileInfo bFileInfoTemp {"", "", -1};
    auto infoPtr = bFileInfoTemp.Unmarshalling(parcel);
    EXPECT_NE(infoPtr, nullptr);
    GTEST_LOG_(INFO) << "BFileInfoSubTest-end SUB_BFile_Info_Unmarshalling_0100";
}

/**
 * @tc.number: SUB_BFile_Info_Unmarshalling_0101
 * @tc.name: SUB_BFile_Info_Unmarshalling_0101
 * @tc.desc: Test function of Unmarshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoSubTest, SUB_BFile_Info_Unmarshalling_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoSubTest-begin SUB_BFile_Info_Unmarshalling_0101";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    // unmarshalling
    TmpFileSN sn = 0;
    BFileInfo bFileInfoTemp {"", FILE_NAME, sn};
    auto infoPtr = bFileInfoTemp.Unmarshalling(parcel);
    EXPECT_NE(infoPtr, nullptr);
    GTEST_LOG_(INFO) << "BFileInfoSubTest-end SUB_BFile_Info_Unmarshalling_0101";
}

/**
 * @tc.number: SUB_BFile_Info_Unmarshalling_0102
 * @tc.name: SUB_BFile_Info_Unmarshalling_0102
 * @tc.desc: Test function of Unmarshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoSubTest, SUB_BFile_Info_Unmarshalling_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoSubTest-begin SUB_BFile_Info_Unmarshalling_0102";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    // unmarshalling
    TmpFileSN sn = 0;
    BFileInfo bFileInfoTemp {BUNDLE_NAME, "", sn};
    auto infoPtr = bFileInfoTemp.Unmarshalling(parcel);
    EXPECT_NE(infoPtr, nullptr);
    GTEST_LOG_(INFO) << "BFileInfoSubTest-end SUB_BFile_Info_Unmarshalling_0102";
}

/**
 * @tc.number: SUB_BFile_Info_Unmarshalling_0103
 * @tc.name: SUB_BFile_Info_Unmarshalling_0103
 * @tc.desc: Test function of Unmarshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoSubTest, SUB_BFile_Info_Unmarshalling_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoSubTest-begin SUB_BFile_Info_Unmarshalling_0103";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    // unmarshalling
    BFileInfo bFileInfoTemp {BUNDLE_NAME, "", -1};
    auto infoPtr = bFileInfoTemp.Unmarshalling(parcel);
    EXPECT_NE(infoPtr, nullptr);
    GTEST_LOG_(INFO) << "BFileInfoSubTest-end SUB_BFile_Info_Unmarshalling_0103";
}

/**
 * @tc.number: SUB_BFile_Info_Unmarshalling_0104
 * @tc.name: SUB_BFile_Info_Unmarshalling_0104
 * @tc.desc: Test function of Unmarshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoSubTest, SUB_BFile_Info_Unmarshalling_0104, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoSubTest-begin SUB_BFile_Info_Unmarshalling_0104";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    // unmarshalling
    BFileInfo bFileInfoTemp {"", FILE_NAME, -1};
    auto infoPtr = bFileInfoTemp.Unmarshalling(parcel);
    EXPECT_NE(infoPtr, nullptr);
    GTEST_LOG_(INFO) << "BFileInfoSubTest-end SUB_BFile_Info_Unmarshalling_0104";
}

/**
 * @tc.number: SUB_BFile_Info_Unmarshalling_0105
 * @tc.name: SUB_BFile_Info_Unmarshalling_0105
 * @tc.desc: Test function of Unmarshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoSubTest, SUB_BFile_Info_Unmarshalling_0105, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoSubTest-begin SUB_BFile_Info_Unmarshalling_0105";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    // unmarshalling
    TmpFileSN sn = 0;
    BFileInfo bFileInfoTemp {BUNDLE_NAME, FILE_NAME, sn};
    auto infoPtr = bFileInfoTemp.Unmarshalling(parcel);
    EXPECT_NE(infoPtr, nullptr);
    GTEST_LOG_(INFO) << "BFileInfoSubTest-end SUB_BFile_Info_Unmarshalling_0105";
}

/**
 * @tc.number: SUB_BFile_Info_Unmarshalling_0106
 * @tc.name: SUB_BFile_Info_Unmarshalling_0106
 * @tc.desc: Test function of Unmarshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoSubTest, SUB_BFile_Info_Unmarshalling_0106, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoSubTest-begin SUB_BFile_Info_Unmarshalling_0106";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    // unmarshalling
    TmpFileSN sn = 0;
    BFileInfo bFileInfoTemp {"", "", sn};
    auto infoPtr = bFileInfoTemp.Unmarshalling(parcel);
    EXPECT_NE(infoPtr, nullptr);
    GTEST_LOG_(INFO) << "BFileInfoSubTest-end SUB_BFile_Info_Unmarshalling_0106";
}

/**
 * @tc.number: SUB_BFile_Info_Unmarshalling_0107
 * @tc.name: SUB_BFile_Info_Unmarshalling_0107
 * @tc.desc: Test function of Unmarshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoSubTest, SUB_BFile_Info_Unmarshalling_0107, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoSubTest-begin SUB_BFile_Info_Unmarshalling_0107";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    // unmarshalling
    BFileInfo bFileInfoTemp {BUNDLE_NAME, FILE_NAME, -1};
    auto infoPtr = bFileInfoTemp.Unmarshalling(parcel);
    EXPECT_NE(infoPtr, nullptr);
    GTEST_LOG_(INFO) << "BFileInfoSubTest-end SUB_BFile_Info_Unmarshalling_0107";
}

/**
 * @tc.number: SUB_BFile_Info_0200
 * @tc.name: SUB_BFile_Info_0200
 * @tc.desc: 分支测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoSubTest, SUB_BFile_Info_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoSubTest-begin SUB_BFile_Info_0200";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    ResetParcelState();
    MockWriteUint32(false);
    EXPECT_EQ(bFileInfo.Marshalling(parcel), false);
    MockWriteUint32(true);
    MockWriteString(false, 1);
    EXPECT_EQ(bFileInfo.Marshalling(parcel), false);
    EXPECT_EQ(bFileInfo.Marshalling(parcel), false);

    ResetParcelState();
    MockWriteUint32(false);
    EXPECT_EQ(bFileInfo.ReadFromParcel(parcel), false);
    MockWriteUint32(true);
    MockWriteString(false, 1);
    EXPECT_EQ(bFileInfo.ReadFromParcel(parcel), false);
    EXPECT_EQ(bFileInfo.ReadFromParcel(parcel), false);

    auto infoPtr = bFileInfo.Unmarshalling(parcel);
    EXPECT_EQ(infoPtr, nullptr);

    GTEST_LOG_(INFO) << "BFileInfoSubTest-end SUB_BFile_Info_0200";
}
} // namespace OHOS::FileManagement::Backup