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
        std::string bundleName1 = "com.hos.app02";
        bundleNames.push_back(bundleName);
        bundleNames.push_back(bundleName1);
        std::string pattern = ":";
        std::vector<std::string> detailInfos;
        std::string detail01 = "{
            "infos" : [ {
            "details" : [ {
            "detail" : [ { "source" : "com.ohos.app01", "target" : "com.hos.app01" } ],
            "type" : "app_mapping_relation" } ],
            "type" : "broadcast" } ]
        }";
        detailInfos.push_back(detail01);
        detailInfos.push_back("");
        int32_t userId = 100;
        std::vector<std::string> realBundleNames;
        std::map<std::string, bool> isClearDataFlags;
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap =
            BJsonUtil::BuildBundleInfos(bundleNames, detailInfos, realBundleNames,
            userId, isClearDataFlags);
        std::string key = "com.hos.app01";
        EXPECT_EQ("com.hos.app01", bundleNameDetailMap[key].bundleName[0]);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end b_dir_BuildBundleInfos_0100";
}

/* *
 * @tc.number: b_jsonutil_BuildBundleInfos_0101
 * @tc.name: b_jsonutil_BuildBundleInfos_0101
 * @tc.desc: Test function of BuildBundleInfos for enmpty.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfos_0101, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfos_0101";
    try {
        std::vector<std::string> bundleNames;
        std::string bundleName = "com.hos.app01:1";
        std::string bundleName1 = "com.hos.app02";
        bundleNames.push_back(bundleName);
        bundleNames.push_back(bundleName1);
        std::string pattern = ":";
        std::vector<std::string> detailInfos;
        detailInfos.push_back("");
        int32_t userId = 100;
        std::vector<std::string> realBundleNames;
        std::map<std::string, bool> isClearDataFlags;
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap =
            BJsonUtil::BuildBundleInfos(bundleNames, detailInfos, realBundleNames,
            userId, isClearDataFlags);
        EXPECT_EQ(0, bundleNameDetailMap.size());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end b_jsonutil_BuildBundleInfos_0101";
}
} // namespace OHOS::FileManagement::Backup