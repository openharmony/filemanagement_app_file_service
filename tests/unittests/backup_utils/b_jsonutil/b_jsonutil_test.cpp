/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include <cstdlib>

#include <dirent.h>
#include <fcntl.h>

#include <errors.h>
#include <file_ex.h>
#include <gtest/gtest.h>

#include "b_jsonutil/b_jsonutil.h"
#include "b_process/b_process.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BJsonUtilTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: b_jsonutil_ParseBundleNameIndexStr_0100
 * @tc.name: b_jsonutil_ParseBundleNameIndexStr_0100
 * @tc.desc: Test function of ParseBundleNameIndexStr interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_ParseBundleNameIndexStr_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin b_dir_GetDirFiles_0100";
    try {
        std::string bundleName = "com.hos.app01:1";
        BJsonUtil::BundleDetailInfo detailInfo = BJsonUtil::ParseBundleNameIndexStr(bundleName);
        EXPECT_EQ("com.hos.app01", detailInfo.bundleName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetDirFiles_0100";
}

/**
 * @tc.number: b_jsonutil_ParseBundleNameIndexStr_0200
 * @tc.name: b_jsonutil_ParseBundleNameIndexStr_0200
 * @tc.desc: Test function of ParseBundleNameIndexStr interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_ParseBundleNameIndexStr_0200, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin ParseBundleNameIndexStr_0200";
    try {
        std::string bundleName = "com.hos.app01";
        BJsonUtil::BundleDetailInfo detailInfo = BJsonUtil::ParseBundleNameIndexStr(bundleName);
        EXPECT_EQ("com.hos.app01", detailInfo.bundleName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end ParseBundleNameIndexStr_0200";
}

/**
 * @tc.number: b_jsonutil_BuildBundleInfos_0100
 * @tc.name: b_jsonutil_BuildBundleInfos_0100
 * @tc.desc: Test function of BuildBundleInfos interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfos_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfos_0100";
    try {
        std::vector<std::string> bundleNames;
        std::string bundleName = "com.hos.app01:1";
        bundleNames.push_back(bundleName);
        std::vector<std::string> bundleInfos;
        std::string bundleInfo = "info1";
        std::string bundleInfo2 = "info2";
        bundleInfos.push_back(bundleInfo);
        bundleInfos.push_back(bundleInfo2);
        int32_t userId = 100;
        std::vector<std::string> bundleNamesOnly;
        auto result = BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly, userId);
        EXPECT_TRUE(result.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildBundleInfos_0100";
}

/**
 * @tc.number: b_jsonutil_BuildBundleInfos_0200
 * @tc.name: b_jsonutil_BuildBundleInfos_0200
 * @tc.desc: Test function of BuildBundleInfos interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfos_0200, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfos_0200";
    try {
        std::vector<std::string> bundleNames;
        std::string bundleName = "bundle";
        bundleNames.push_back(bundleName);
        std::vector<std::string> bundleInfos;
        std::string bundleInfo = "info1";
        bundleInfos.push_back(bundleInfo);
        int32_t userId = 100;
        std::vector<std::string> bundleNamesOnly;
        auto result = BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly, userId);
        EXPECT_FALSE(result.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildBundleInfos_0200";
}

/**
 * @tc.number: b_jsonutil_BuildBundleInfos_0300
 * @tc.name: b_jsonutil_BuildBundleInfos_0300
 * @tc.desc: Test function of BuildBundleInfos interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfos_0300, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfos_0300";
    try {
        std::vector<std::string> bundleNames;
        std::string bundleName = "bundle1";
        std::string bundleName2 = "bundle2";
        std::string bundleName3 = "bundle3";
        bundleNames.push_back(bundleName);
        bundleNames.push_back(bundleName2);
        bundleNames.push_back(bundleName3);
        std::vector<std::string> bundleInfos;
        std::string bundleInfo = "info1";
        std::string bundleInfo2 = "info2";
        std::string bundleInfo3 = "info3";
        bundleInfos.push_back(bundleInfo);
        bundleInfos.push_back(bundleInfo2);
        bundleInfos.push_back(bundleInfo3);
        int32_t userId = 100;
        std::vector<std::string> bundleNamesOnly;
        int32_t size = 3;
        auto result = BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly, userId);
        EXPECT_EQ(bundleNamesOnly.size(), size);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildBundleInfos_0300";
}

/**
 * @tc.number: b_jsonutil_BuildBundleInfos_0400
 * @tc.name: b_jsonutil_BuildBundleInfos_0400
 * @tc.desc: Test function of BuildBundleInfos interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfos_0400, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfos_0400";
    try {
        std::vector<std::string> bundleNames;
        std::string bundleName = "bundle1";
        bundleNames.push_back(bundleName);
        std::vector<std::string> bundleInfos;
        std::string bundleInfo = {"{\"infos\":[{\"type\":\"type1\",\"details\":\"details1\"}]}"};
        bundleInfos.push_back(bundleInfo);
        int32_t userId = 100;
        std::vector<std::string> bundleNamesOnly;

        auto result = BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly, userId);
        EXPECT_FALSE(result.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildBundleInfos_0400";
}

/**
 * @tc.number: b_jsonutil_FindBundleInfoByName_0100
 * @tc.name: b_jsonutil_FindBundleInfoByName_0100
 * @tc.desc: Test function of FindBundleInfoByName interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_FindBundleInfoByName_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin FindBundleInfoByName_0100";
    try {
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailsMap;
        std::string bundleName = "bundle1";
        std::string jobType = "type";
        BJsonUtil::BundleDetailInfo bundleDetail;

        bool result = BJsonUtil::FindBundleInfoByName(bundleNameDetailsMap, bundleName, jobType, bundleDetail);
        EXPECT_EQ(false, result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end FindBundleInfoByName_0100";
}

/**
 * @tc.number: b_jsonutil_FindBundleInfoByName_0200
 * @tc.name: b_jsonutil_FindBundleInfoByName_0200
 * @tc.desc: Test function of FindBundleInfoByName interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_FindBundleInfoByName_0200, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin FindBundleInfoByName_0200";
    try {
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailsMap;
        std::string bundleName = "bundle1";
        std::string jobType = "type";
        BJsonUtil::BundleDetailInfo detailInfo;
        detailInfo.bundleName = bundleName;
        detailInfo.type = jobType;
        bundleNameDetailsMap[bundleName] = {detailInfo};
        BJsonUtil::BundleDetailInfo bundleDetail;

        bool result = BJsonUtil::FindBundleInfoByName(bundleNameDetailsMap, bundleName, jobType, bundleDetail);
        EXPECT_EQ(true, result);
        EXPECT_EQ(bundleDetail.type, detailInfo.type);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end FindBundleInfoByName_0200";
}

/**
 * @tc.number: b_jsonutil_BuildRestoreErrInfo_0100
 * @tc.name: b_jsonutil_BuildRestoreErrInfo_0100
 * @tc.desc: Test function of BuildRestoreErrInfo interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildRestoreErrInfo_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildRestoreErrInfo_0100";
    try {
        std::string jsonStr;
        int errCode = 1;
        std::string errMsg = "error";

        bool result = BJsonUtil::BuildRestoreErrInfo(jsonStr, errCode, errMsg);
        EXPECT_EQ(true, result);
        EXPECT_NE(jsonStr.find("errorCode"), std::string::npos);
        EXPECT_NE(jsonStr.find("errorInfo"), std::string::npos);
        EXPECT_NE(jsonStr.find("type"), std::string::npos);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildRestoreErrInfo_0100";
}
} // namespace OHOS::FileManagement::Backup