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
#include "cJsonMock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class BJsonUtilTest : public testing::Test {
public:
    // 所有测试用例执行之前执行
    static void SetUpTestCase(void);
    // 所有测试用例执行之后执行
    static void TearDownTestCase(void);
    // 每次测试用例执行之前执行
    void SetUp() {};
    // 每次测试用例执行之后执行
    void TearDown() {};

    static inline shared_ptr<CJsonMock> cJsonMock = nullptr;
};

void BJsonUtilTest::SetUpTestCase()
{
    cJsonMock = make_shared<CJsonMock>();
    CJson::cJsonPtr = cJsonMock;
}

void BJsonUtilTest::TearDownTestCase()
{
    CJson::cJsonPtr = nullptr;
    cJsonMock = nullptr;
}

/**
 * @tc.number: b_jsonutil_BuildExtensionErrInfo_0101
 * @tc.name: b_jsonutil_BuildExtensionErrInfo_0101
 * @tc.desc: Test function of BuildExtensionErrInfo interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildExtensionErrInfo_0101, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildExtensionErrInfo_0101";
    try {
        std::string jsonStr;
        int errCode = 1;
        std::string errMsg = "error";
        int cjson = 0;

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(nullptr));
        bool result = BJsonUtil::BuildExtensionErrInfo(jsonStr, errCode, errMsg);
        EXPECT_FALSE(result);

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject())
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        result = BJsonUtil::BuildExtensionErrInfo(jsonStr, errCode, errMsg);
        EXPECT_FALSE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildExtensionErrInfo_0101";
}

/**
 * @tc.number: b_jsonutil_BuildExtensionErrInfo_0201
 * @tc.name: b_jsonutil_BuildExtensionErrInfo_0201
 * @tc.desc: Test function of BuildExtensionErrInfo interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildExtensionErrInfo_0201, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildExtensionErrInfo_0201";
    try {
        std::string jsonStr;
        std::map<std::string, std::vector<int>> errFileInfo;
        int cjson = 0;

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(nullptr));
        bool result = BJsonUtil::BuildExtensionErrInfo(jsonStr, errFileInfo);
        EXPECT_FALSE(result);

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray())
            .WillOnce(Return(nullptr))
            .WillOnce(Return(nullptr))
            .WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        result = BJsonUtil::BuildExtensionErrInfo(jsonStr, errFileInfo);
        EXPECT_FALSE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildExtensionErrInfo_0201";
}

/**
 * @tc.number: b_jsonutil_BuildOnProcessRetInfo_0301
 * @tc.name: b_jsonutil_BuildOnProcessRetInfo_0301
 * @tc.desc: Test function of BuildOnProcessRetInfo interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildOnProcessRetInfo_0301, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildOnProcessRetInfo_0301";
    try {
        std::string jsonStr;
        std::string onProcessRet;
        int cjson = 0;

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(nullptr));
        bool result = BJsonUtil::BuildOnProcessRetInfo(jsonStr, onProcessRet);
        EXPECT_FALSE(result);

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject())
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        result = BJsonUtil::BuildOnProcessRetInfo(jsonStr, onProcessRet);
        EXPECT_FALSE(result);

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject())
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddStringToObject(_, _, _))
            .WillOnce(Return(nullptr))
            .WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        result = BJsonUtil::BuildOnProcessRetInfo(jsonStr, onProcessRet);
        EXPECT_FALSE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildOnProcessRetInfo_0301";
}

/**
 * @tc.number: b_jsonutil_BuildBundleInfoJson_0401
 * @tc.name: b_jsonutil_BuildBundleInfoJson_0401
 * @tc.desc: Test function of BuildBundleInfoJson interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfoJson_0401, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfoJson_0401";
    try {
        int32_t userId = 100;
        std::string detailInfo;
        int cjson = 0;

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(nullptr));
        bool result = BJsonUtil::BuildBundleInfoJson(userId, detailInfo);
        EXPECT_FALSE(result);

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject())
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        result = BJsonUtil::BuildBundleInfoJson(userId, detailInfo);
        EXPECT_FALSE(result);

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject())
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        result = BJsonUtil::BuildBundleInfoJson(userId, detailInfo);
        EXPECT_FALSE(result);

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject())
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_AddStringToObject(_, _, _)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToArray(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(nullptr)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        result = BJsonUtil::BuildBundleInfoJson(userId, detailInfo);
        EXPECT_FALSE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildBundleInfoJson_0401";
}

/**
 * @tc.number: b_jsonutil_BuildBundleInfoJson_0402
 * @tc.name: b_jsonutil_BuildBundleInfoJson_0402
 * @tc.desc: Test function of BuildBundleInfoJson interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilTest, b_jsonutil_BuildBundleInfoJson_0402, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonUtilTest-begin BuildBundleInfoJson_0402";
    try {
        int32_t userId = 100;
        std::string detailInfo;
        int cjson = 0;

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject())
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _))
            .WillOnce(Return(true))
            .WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_AddStringToObject(_, _, _)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToArray(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray())
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        bool result = BJsonUtil::BuildBundleInfoJson(userId, detailInfo);
        EXPECT_FALSE(result);

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject())
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _))
            .WillOnce(Return(true))
            .WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_AddStringToObject(_, _, _))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToArray(_, _))
            .WillOnce(Return(true))
            .WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray())
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        result = BJsonUtil::BuildBundleInfoJson(userId, detailInfo);
        EXPECT_FALSE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilTest-end BuildBundleInfoJson_0402";
    }
}