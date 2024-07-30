/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "json/value.h"
#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_json/b_json_service_disposal_config.h"
#include "cJsonMock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string PATH = "/data/service/el2/100/backup/";
const string CONFIG_NAME = "RestoreDisposalConfig.json";
} // namespace

class BJsonServiceDisposalConfigTest : public testing::Test {
public:
    // 所有测试用例执行之前执行
    static void SetUpTestCase(void);
    // 所有测试用例执行之后执行
    static void TearDownTestCase(void);
    // 每次测试用例执行之前执行
    void SetUp();
    // 每次测试用例执行之后执行
    void TearDown();

    static inline shared_ptr<CJsonMock> cJsonMock = nullptr;
};

void BJsonServiceDisposalConfigTest::TearDown() {}

void BJsonServiceDisposalConfigTest::SetUp()
{
    string filePath = PATH + CONFIG_NAME;
    bool result = remove(filePath.c_str());
    if (result) {
        GTEST_LOG_(INFO) << "delete file success.";
    } else {
        GTEST_LOG_(INFO) << "delete file failed.";
    }
}

void BJsonServiceDisposalConfigTest::SetUpTestCase()
{
    cJsonMock = make_shared<CJsonMock>();
    CJson::cJsonPtr = cJsonMock;
}

void BJsonServiceDisposalConfigTest::TearDownTestCase()
{
    CJson::cJsonPtr = nullptr;
    cJsonMock = nullptr;
}

/* *
 * @tc.number: SUB_Disposal_Config_Test_0102
 * @tc.name: SUB_Disposal_Config_Test_0102
 * @tc.desc: 测试print返回空指针情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, SUB_Disposal_Config_Test_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin SUB_Disposal_Config_Test_0102";
    try {
        int cjson = 0;
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonDisposalConfig config;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end SUB_Disposal_Config_Test_0102";
}

/* *
 * @tc.number: SUB_Disposal_Config_Test_0103
 * @tc.name: SUB_Disposal_Config_Test_0103
 * @tc.desc: 测试配置文件存在的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, SUB_Disposal_Config_Test_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin SUB_Disposal_Config_Test_0103";
    try {
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(nullptr));
        BJsonDisposalConfig config;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end SUB_Disposal_Config_Test_0103";
}

/* *
 * @tc.number: SUB_Disposal_Config_Test_0104
 * @tc.name: SUB_Disposal_Config_Test_0104
 * @tc.desc: 测试creatArray返回空指针情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, SUB_Disposal_Config_Test_0104, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin SUB_Disposal_Config_Test_0104";
    try {
        int cjson = 0;
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonDisposalConfig config;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end SUB_Disposal_Config_Test_0104";
}

/* *
 * @tc.number: SUB_Disposal_Config_Test_0202
 * @tc.name: Disposal_Config_Test_0202
 * @tc.desc: 测试修改config文件时print返回空指针
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0202, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0202";
    try {
        string filePath = PATH + CONFIG_NAME;
        int cjson = 0;
        string str = "test";
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
        const string bundleName = "test1";
        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddStringToObject(_, _, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToArray(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        bool retAdd = config.AppendIntoDisposalConfigFile(bundleName);
        EXPECT_FALSE(retAdd);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0202";
}

/* *
 * @tc.number: SUB_Disposal_Config_Test_0203
 * @tc.name: Disposal_Config_Test_0203
 * @tc.desc: 测试修改config文件时parse返回空指针的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0203, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0203";
    try {
        string filePath = PATH + CONFIG_NAME;
        int cjson = 0;
        string str = "test";
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
        const string bundleName = "test1";
        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        bool retAdd = config.AppendIntoDisposalConfigFile(bundleName);
        EXPECT_FALSE(retAdd);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0203";
}
} // namespace OHOS::FileManagement::Backup