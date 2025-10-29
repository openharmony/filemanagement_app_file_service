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

class BFileInfoTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.number: SUB_BFile_Info_Marshalling_0100
 * @tc.name: SUB_BFile_Info_Marshalling_0100
 * @tc.desc: Test function of Marshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_Marshalling_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_Marshalling_0100";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    // unmarshalling
    auto infoPtr = bFileInfo.Unmarshalling(parcel);
    EXPECT_NE(infoPtr, nullptr);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_Marshalling_0100";
}

/**
 * @tc.number: SUB_BFile_Info_Marshalling_0101
 * @tc.name: SUB_BFile_Info_Marshalling_0101
 * @tc.desc: Test function of Marshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_Marshalling_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_Marshalling_0101";
    string bundleName = "";
    BFileInfo bFileInfo(bundleName, FILE_NAME, -1);
    Parcel parcel;
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_Marshalling_0101";
}

/**
 * @tc.number: SUB_BFile_Info_Marshalling_0102
 * @tc.name: SUB_BFile_Info_Marshalling_0102
 * @tc.desc: Test function of Marshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_Marshalling_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_Marshalling_0102";
    string fileName = "";
    BFileInfo bFileInfo(BUNDLE_NAME, fileName, -1);
    Parcel parcel;
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_Marshalling_0102";
}

/**
 * @tc.number: SUB_BFile_Info_Marshalling_0103
 * @tc.name: SUB_BFile_Info_Marshalling_0103
 * @tc.desc: Test function of Marshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_Marshalling_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_Marshalling_0103";
    TmpFileSN sn = 0;
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, sn);
    Parcel parcel;
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_Marshalling_0103";
}

/**
 * @tc.number: SUB_BFile_Info_Marshalling_0104
 * @tc.name: SUB_BFile_Info_Marshalling_0104
 * @tc.desc: Test function of Marshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_Marshalling_0104, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_Marshalling_0104";
    string bundleName = "";
    string fileName = "";
    BFileInfo bFileInfo(bundleName, fileName, -1);
    Parcel parcel;
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_Marshalling_0104";
}

/**
 * @tc.number: SUB_BFile_Info_Marshalling_0105
 * @tc.name: SUB_BFile_Info_Marshalling_0105
 * @tc.desc: Test function of Marshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_Marshalling_0105, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_Marshalling_0105";
    string bundleName = "";
    TmpFileSN sn = 0;
    BFileInfo bFileInfo(bundleName, FILE_NAME, sn);
    Parcel parcel;
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_Marshalling_0105";
}

/**
 * @tc.number: SUB_BFile_Info_Marshalling_0106
 * @tc.name: SUB_BFile_Info_Marshalling_0106
 * @tc.desc: Test function of Marshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_Marshalling_0106, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_Marshalling_0106";
    string fileName = "";
    TmpFileSN sn = 0;
    BFileInfo bFileInfo(BUNDLE_NAME, fileName, sn);
    Parcel parcel;
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_Marshalling_0106";
}

/**
 * @tc.number: SUB_BFile_Info_Marshalling_0107
 * @tc.name: SUB_BFile_Info_Marshalling_0107
 * @tc.desc: Test function of Marshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_Marshalling_0107, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_Marshalling_0107";
    string bundleName = "";
    string fileName = "";
    TmpFileSN sn = 0;
    BFileInfo bFileInfo(bundleName, fileName, sn);
    Parcel parcel;
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_Marshalling_0107";
}

/**
 * @tc.number: SUB_BFile_Info_ReadFromParcel_0100
 * @tc.name: SUB_BFile_Info_ReadFromParcel_0100
 * @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_ReadFromParcel_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_ReadFromParcel_0100";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);

    // ReadFromParcel
    BFileInfo bFileInfoTemp {"", "", -1};
    bFileInfoTemp.ReadFromParcel(parcel);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_ReadFromParcel_0100";
}

/**
 * @tc.number: SUB_BFile_Info_ReadFromParcel_0101
 * @tc.name: SUB_BFile_Info_ReadFromParcel_0101
 * @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_ReadFromParcel_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_ReadFromParcel_0101";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);

    // ReadFromParcel
    BFileInfo bFileInfoTemp {BUNDLE_NAME, "", -1};
    bFileInfoTemp.ReadFromParcel(parcel);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_ReadFromParcel_0101";
}

/**
 * @tc.number: SUB_BFile_Info_ReadFromParcel_0102
 * @tc.name: SUB_BFile_Info_ReadFromParcel_0102
 * @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_ReadFromParcel_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_ReadFromParcel_0102";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);

    // ReadFromParcel
    TmpFileSN sn = 0;
    BFileInfo bFileInfoTemp {BUNDLE_NAME, FILE_NAME, sn};
    bFileInfoTemp.ReadFromParcel(parcel);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_ReadFromParcel_0102";
}

/**
 * @tc.number: SUB_BFile_Info_ReadFromParcel_0103
 * @tc.name: SUB_BFile_Info_ReadFromParcel_0103
 * @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_ReadFromParcel_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_ReadFromParcel_0103";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);

    // ReadFromParcel
    BFileInfo bFileInfoTemp {"", FILE_NAME, -1};
    bFileInfoTemp.ReadFromParcel(parcel);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_ReadFromParcel_0103";
}

/**
 * @tc.number: SUB_BFile_Info_ReadFromParcel_0104
 * @tc.name: SUB_BFile_Info_ReadFromParcel_0104
 * @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_ReadFromParcel_0104, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_ReadFromParcel_0104";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);

    // ReadFromParcel
    TmpFileSN sn = 0;
    BFileInfo bFileInfoTemp {"", FILE_NAME, sn};
    bFileInfoTemp.ReadFromParcel(parcel);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_ReadFromParcel_0104";
}

/**
 * @tc.number: SUB_BFile_Info_ReadFromParcel_0105
 * @tc.name: SUB_BFile_Info_ReadFromParcel_0105
 * @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_ReadFromParcel_0105, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_ReadFromParcel_0105";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);

    // ReadFromParcel
    TmpFileSN sn = 0;
    BFileInfo bFileInfoTemp {BUNDLE_NAME, "", sn};
    bFileInfoTemp.ReadFromParcel(parcel);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_ReadFromParcel_0105";
}

/**
 * @tc.number: SUB_BFile_Info_ReadFromParcel_0106
 * @tc.name: SUB_BFile_Info_ReadFromParcel_0106
 * @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_ReadFromParcel_0106, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_ReadFromParcel_0106";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);

    // ReadFromParcel
    TmpFileSN sn = 0;
    BFileInfo bFileInfoTemp {"", "", sn};
    bFileInfoTemp.ReadFromParcel(parcel);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_ReadFromParcel_0106";
}

/**
 * @tc.number: SUB_BFile_Info_ReadFromParcel_0107
 * @tc.name: SUB_BFile_Info_ReadFromParcel_0107
 * @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_ReadFromParcel_0107, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_ReadFromParcel_0107";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);

    // ReadFromParcel
    BFileInfo bFileInfoTemp {BUNDLE_NAME, FILE_NAME, -1};
    bFileInfoTemp.ReadFromParcel(parcel);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_ReadFromParcel_0107";
}
} // namespace OHOS::FileManagement::Backup