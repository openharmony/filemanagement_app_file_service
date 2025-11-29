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

namespace {
constexpr uint32_t TEST_USER_ID = 100;
} // namespace

class BJsonUtilTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: b_jsonutil_BuildBundleInfos_0100
 * @tc.name: b_jsonutil_BuildBundleInfos_0100
 * @tc.desc: Test function of BuildBundleInfos interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfos_0100, testing::ext::TestSize.Level1)
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
        int32_t userId = TEST_USER_ID;
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, bool> isClearDataFlags;
        auto result = BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly,
            userId, isClearDataFlags);
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
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfos_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfos_0200";
    try {
        std::vector<std::string> bundleNames;
        std::string bundleName = "bundle";
        bundleNames.push_back(bundleName);
        std::vector<std::string> bundleInfos;
        std::string bundleInfo = "{\"infos\":\"infos\"}";
        bundleInfos.push_back(bundleInfo);
        int32_t userId = TEST_USER_ID;
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, bool> isClearDataFlags;
        auto result = BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly,
            userId, isClearDataFlags);
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
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfos_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfos_0300";
    try {
        std::vector<std::string> bundleNames;
        std::string bundleName = "bundle1:";
        bundleNames.push_back(bundleName);
        std::vector<std::string> bundleInfos;
        std::string bundleInfo = "info1";
        bundleInfos.push_back(bundleInfo);
        int32_t userId = TEST_USER_ID;
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, bool> isClearDataFlags;
        auto result = BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly,
            userId, isClearDataFlags);
        EXPECT_TRUE(result.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildBundleInfos_0300";
}

/**
 * @tc.number: b_jsonutil_BuildBundleInfos_0301
 * @tc.name: b_jsonutil_BuildBundleInfos_0301
 * @tc.desc: Test function of BuildBundleInfos interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfos_0301, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfos_0301";
    try {
        std::vector<std::string> bundleNames;
        std::string bundleName = ":bundle1";
        bundleNames.push_back(bundleName);
        std::vector<std::string> bundleInfos;
        std::string bundleInfo = "info1";
        bundleInfos.push_back(bundleInfo);
        int32_t userId = TEST_USER_ID;
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, bool> isClearDataFlags;
        auto result = BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly,
            userId, isClearDataFlags);
        EXPECT_TRUE(result.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildBundleInfos_0301";
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
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfos_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfos_0400";
    try {
        std::vector<std::string> bundleNames;
        std::string bundleName = "bundle1";
        bundleNames.push_back(bundleName);
        std::vector<std::string> bundleInfos;
        std::string bundleInfo = {
            "{\"infos\":[{\"type\":\"type1\",\"details\":\"details1\"}],\"clearBackupData\": \"false\"}"
        };
        bundleInfos.push_back(bundleInfo);
        int32_t userId = TEST_USER_ID;
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, bool> isClearDataFlags;
        auto result = BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly,
            userId, isClearDataFlags);
        EXPECT_EQ(isClearDataFlags[bundleName], false);
        EXPECT_FALSE(result.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildBundleInfos_0400";
}

/**
 * @tc.number: b_jsonutil_BuildBundleInfos_0500
 * @tc.name: b_jsonutil_BuildBundleInfos_0500
 * @tc.desc: Test function of BuildBundleInfos interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfos_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfos_0500";
    try {
        std::vector<std::string> bundleNames;
        std::string bundleName = "bundle";
        bundleNames.push_back(bundleName);
        std::vector<std::string> bundleInfos;
        std::string bundleInfo = {"{\"infos\":[{\"type\":null}]}"};
        bundleInfos.push_back(bundleInfo);
        int32_t userId = TEST_USER_ID;
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, bool> isClearDataFlags;

        auto result = BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly,
            userId, isClearDataFlags);
        EXPECT_EQ(isClearDataFlags[bundleName], true);
        EXPECT_FALSE(result.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildBundleInfos_0500";
}

/**
 * @tc.number: b_jsonutil_BuildBundleInfos_0600
 * @tc.name: b_jsonutil_BuildBundleInfos_0600
 * @tc.desc: Test function of BuildBundleInfos interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfos_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfos_0600";
    try {
        std::vector<std::string> bundleNames;
        std::string bundleName = "bundle";
        bundleNames.push_back(bundleName);
        std::vector<std::string> bundleInfos;
        std::string bundleInfo = {"{\"infos\":[{\"type\":123}],\"clearBackupData\": \"true\"}"};
        bundleInfos.push_back(bundleInfo);
        int32_t userId = TEST_USER_ID;
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, bool> isClearDataFlags;

        auto result = BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly,
            userId, isClearDataFlags);
        EXPECT_EQ(isClearDataFlags[bundleName], true);
        EXPECT_FALSE(result.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildBundleInfos_0600";
}

/**
 * @tc.number: b_jsonutil_BuildOnProcessRetInfo_0100
 * @tc.name: b_jsonutil_BuildOnProcessRetInfo_0100
 * @tc.desc: Test function of BuildOnProcessRetInfo interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildOnProcessRetInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildOnProcessRetInfo_0100";
    try {
        std::string jsonStr;
        std::string onProcessRet = "test result";

        bool result = BJsonUtil::BuildOnProcessRetInfo(jsonStr, onProcessRet);
        EXPECT_EQ(true, result);
        EXPECT_NE(jsonStr.find("timeInfo"), std::string::npos);
        EXPECT_NE(jsonStr.find("resultInfo"), std::string::npos);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildOnProcessRetInfo_0100";
}

/**
 * @tc.number: b_jsonutil_BuildExtensionErrInfo_0100
 * @tc.name: b_jsonutil_BuildExtensionErrInfo_0100
 * @tc.desc: Test function of BuildExtensionErrInfo interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildExtensionErrInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildExtensionErrInfo_0100";
    try {
        std::string jsonStr;
        int errCode = 1;
        std::string errMsg = "error";

        bool result = BJsonUtil::BuildExtensionErrInfo(jsonStr, errCode, errMsg);
        EXPECT_EQ(true, result);
        EXPECT_NE(jsonStr.find("errorCode"), std::string::npos);
        EXPECT_NE(jsonStr.find("errorInfo"), std::string::npos);
        EXPECT_NE(jsonStr.find("type"), std::string::npos);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildExtensionErrInfo_0100";
}

/**
 * @tc.number: b_jsonutil_BuildExtensionErrInfo_0200
 * @tc.name: b_jsonutil_BuildExtensionErrInfo_0200
 * @tc.desc: Test function of BuildExtensionErrInfo interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildExtensionErrInfo_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildExtensionErrInfo_0200";
    try {
        std::string jsonStr;
        std::map<std::string, std::vector<int>> errFileInfo = {{"test", {1, 1}}};

        bool result = BJsonUtil::BuildExtensionErrInfo(jsonStr, errFileInfo);
        EXPECT_EQ(true, result);
        EXPECT_NE(jsonStr.find("errorCode"), std::string::npos);
        EXPECT_NE(jsonStr.find("errorInfo"), std::string::npos);
        EXPECT_NE(jsonStr.find("type"), std::string::npos);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildExtensionErrInfo_0200";
}

/**
 * @tc.number: b_jsonutil_FindBackupSceneByName_0100
 * @tc.name: b_jsonutil_FindBackupSceneByName_0100
 * @tc.desc: Test function of FindBackupSceneByName interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_FindBackupSceneByName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin FindBackupSceneByName_0100";
    try {
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailsMap;
        std::string bundleName = "bundle1";
        std::string backupScene = "";

        bool result = BJsonUtil::FindBackupSceneByName(bundleNameDetailsMap, bundleName, backupScene);
        EXPECT_EQ(false, result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end FindBackupSceneByName_0100";
}

/**
 * @tc.number: b_jsonutil_FindBackupSceneByName_0200
 * @tc.name: b_jsonutil_FindBackupSceneByName_0200
 * @tc.desc: Test function of FindBackupSceneByName interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_FindBackupSceneByName_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin FindBackupSceneByName_0200";
    try {
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailsMap;
        std::string bundleName = "bundle1";
        std::string backupScene = "";
        BJsonUtil::BundleDetailInfo detailInfo;
        detailInfo.bundleName = bundleName;
        detailInfo.backupScene = "backupScene";
        bundleNameDetailsMap[bundleName] = {detailInfo};

        bool result = BJsonUtil::FindBackupSceneByName(bundleNameDetailsMap, bundleName, backupScene);
        EXPECT_EQ(true, result);
        EXPECT_EQ(detailInfo.backupScene, backupScene);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end FindBackupSceneByName_0200";
}

/**
 * @tc.number: b_jsonutil_FindBackupSceneByName_0300
 * @tc.name: b_jsonutil_FindBackupSceneByName_0300
 * @tc.desc: Test function of FindBackupSceneByName interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_FindBackupSceneByName_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin FindBackupSceneByName_0300";
    try {
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailsMap;
        std::string bundleName = "bundle1";
        BJsonUtil::BundleDetailInfo detailInfo;
        detailInfo.bundleName = bundleName;
        detailInfo.backupScene = "";
        std::string backupScene = "";
        bundleNameDetailsMap[bundleName] = {detailInfo};

        bool result = BJsonUtil::FindBackupSceneByName(bundleNameDetailsMap, bundleName, backupScene);
        EXPECT_EQ(false, result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end FindBackupSceneByName_0300";
}

/**
 * @tc.number: b_jsonutil_FindBroadCastInfoByName_0100
 * @tc.name: b_jsonutil_FindBroadCastInfoByName_0100
 * @tc.desc: Test function of FindBroadCastInfoByName interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_FindBroadCastInfoByName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin FindBroadCastInfoByName_0100";
    try {
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailsMap;
        std::map<std::string, std::string> broadCastInfoMap;
        std::string bundleName = "bundle1";
        std::string backupScene = "";

        bool result = BJsonUtil::FindBroadCastInfoByName(bundleNameDetailsMap, bundleName,
            "type", broadCastInfoMap);
        EXPECT_EQ(false, result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end FindBroadCastInfoByName_0100";
}

/**
 * @tc.number: b_jsonutil_FindBroadCastInfoByName_0200
 * @tc.name: b_jsonutil_FindBroadCastInfoByName_0200
 * @tc.desc: Test function of FindBroadCastInfoByName interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_FindBroadCastInfoByName_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin FindBroadCastInfoByName_0200";
    try {
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailsMap;
        std::map<std::string, std::string> broadCastInfoMap;
        std::string bundleName = "bundle1";
        std::string broadCastType = "123";
        std::string type = "testType";
        BJsonUtil::BundleDetailInfo detailInfo;
        bundleNameDetailsMap[bundleName] = {detailInfo};

        bool result = BJsonUtil::FindBroadCastInfoByName(bundleNameDetailsMap, bundleName,
            "type", broadCastInfoMap);
        EXPECT_EQ(false, result);
        
        detailInfo.bundleName = bundleName;
        detailInfo.type = type;
        detailInfo.broadCastType = broadCastType;
        bundleNameDetailsMap[bundleName] = {detailInfo};
        result = BJsonUtil::FindBroadCastInfoByName(bundleNameDetailsMap, bundleName,
            type, broadCastInfoMap);
        EXPECT_EQ(true, result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end FindBroadCastInfoByName_0200";
}

/**
 * @tc.number: b_jsonutil_ParseBundleInfoJson_0100
 * @tc.name: b_jsonutil_ParseBundleInfoJson_0100
 * @tc.desc: Test function of ParseBundleInfoJson interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: NA
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_ParseBundleInfoJson_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin ParseBundleInfoJson_0100";
    try {
        BJsonUtil::BundleDetailInfo detailInfo = {
            .bundleName = "bundle", .bundleIndex = 0, .userId = 100};
        std::string bundleInfo = R"({
            "infos":[{"type":"broadcast","broadcastType":"restore_start","isBroadcastOnly":true,"details":[]}],
            "backupScene":""
        })";
        std::vector<BJsonUtil::BundleDetailInfo> bundleDetailInfos;
        bool isClearData = true;
        BJsonUtil::ParseBundleInfoJson(bundleInfo, bundleDetailInfos, detailInfo, isClearData, 100);
        ASSERT_TRUE(!bundleDetailInfos.empty());
        EXPECT_TRUE(bundleDetailInfos[0].isBroadcastOnly);

        bundleInfo = R"({
            "infos":[{"type":"broadcast","broadcastType":"restore_start","isBroadcastOnly":"true","details":[]}],
            "backupScene":""
        })";
        bundleDetailInfos.clear();
        BJsonUtil::ParseBundleInfoJson(bundleInfo, bundleDetailInfos, detailInfo, isClearData, 100);
        ASSERT_TRUE(!bundleDetailInfos.empty());
        EXPECT_FALSE(bundleDetailInfos[0].isBroadcastOnly);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end ParseBundleInfoJson_0100";
}


/**
 * @tc.number: b_jsonutil_ParseBundleInfoJson_0200
 * @tc.name: b_jsonutil_ParseBundleInfoJson_0200
 * @tc.desc: Test function of ParseBundleInfoJson interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: NA
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_ParseBundleInfoJson_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin ParseBundleInfoJson_0200";
    try {
        BJsonUtil::BundleDetailInfo detailInfo = {
            .bundleName = "bundle", .bundleIndex = 0, .userId = 100};
        std::string bundleInfo = R"({
            "infos":[{"type":"broadcast","broadcastType":"restore_start","isBroadcastOnly":false,"details":[]}],
            "backupScene":""
        })";
        std::vector<BJsonUtil::BundleDetailInfo> bundleDetailInfos;
        bool isClearData = true;
        BJsonUtil::ParseBundleInfoJson(bundleInfo, bundleDetailInfos, detailInfo, isClearData, 100);
        ASSERT_TRUE(!bundleDetailInfos.empty());
        EXPECT_FALSE(bundleDetailInfos[0].isBroadcastOnly);

        bundleInfo = R"({
            "infos":[{"type":"broadcast","broadcastType":"restore_start","details":[]}],
            "backupScene":""
        })";
        bundleDetailInfos.clear();
        BJsonUtil::ParseBundleInfoJson(bundleInfo, bundleDetailInfos, detailInfo, isClearData, 100);
        ASSERT_TRUE(!bundleDetailInfos.empty());
        EXPECT_FALSE(bundleDetailInfos[0].isBroadcastOnly);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end ParseBundleInfoJson_0200";
}

/**
 * @tc.number: b_jsonutil_ParseBundleInfoJson_0300
 * @tc.name: b_jsonutil_ParseBundleInfoJson_0300
 * @tc.desc: Test function of ParseBundleInfoJson interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: NA
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_ParseBundleInfoJson_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin ParseBundleInfoJson_0300";
    try {
        BJsonUtil::BundleDetailInfo detailInfo = {
            .bundleName = "bundle", .bundleIndex = 0, .userId = 100};
        std::string bundleInfo = R"({
            "infos":[{"type":"broadcast","isBroadcastOnly":true,"details":[]}],
            "backupScene":""
        })";
        std::vector<BJsonUtil::BundleDetailInfo> bundleDetailInfos;
        bool isClearData = true;
        BJsonUtil::ParseBundleInfoJson(bundleInfo, bundleDetailInfos, detailInfo, isClearData, 100);
        ASSERT_TRUE(!bundleDetailInfos.empty());
        EXPECT_TRUE(bundleDetailInfos[0].broadCastType.empty());

        bundleInfo = R"({
            "infos":[{"type":"broadcast","broadcastType":123,"isBroadcastOnly":"true","details":[]}],
            "backupScene":""
        })";
        bundleDetailInfos.clear();
        BJsonUtil::ParseBundleInfoJson(bundleInfo, bundleDetailInfos, detailInfo, isClearData, 100);
        ASSERT_TRUE(!bundleDetailInfos.empty());
        EXPECT_TRUE(bundleDetailInfos[0].broadCastType.empty());

        bundleInfo = R"({
            "infos":[{"type":"broadcast","broadcastType":"","details":[]}],
            "backupScene":""
        })";
        bundleDetailInfos.clear();
        BJsonUtil::ParseBundleInfoJson(bundleInfo, bundleDetailInfos, detailInfo, isClearData, 100);
        ASSERT_TRUE(!bundleDetailInfos.empty());
        EXPECT_TRUE(bundleDetailInfos[0].broadCastType.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end ParseBundleInfoJson_0300";
}
} // namespace OHOS::FileManagement::Backup