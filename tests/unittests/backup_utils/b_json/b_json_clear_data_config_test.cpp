/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <fstream>

#include <gtest/gtest.h>
#include "json/value.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_json/b_json_clear_data_config.h"
#include "cJsonMock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string PATH = "/data/service/el2/100/backup/";
const string CONFIG_NAME = "ClearDataConfig.json";
} // namespace

class BJsonClearDataConfigTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline shared_ptr<CJsonMock> cJsonMock = nullptr;
};

void BJsonClearDataConfigTest::TearDown() {}

void BJsonClearDataConfigTest::SetUp()
{
    string filePath = PATH + CONFIG_NAME;
    bool result = remove(filePath.c_str());
    if (result) {
        GTEST_LOG_(INFO) << "delete file success.";
    } else {
        GTEST_LOG_(INFO) << "delete file failed.";
    }
}

void BJsonClearDataConfigTest::SetUpTestCase()
{
    cJsonMock = make_shared<CJsonMock>();
    CJson::cJsonPtr = cJsonMock;
}

void BJsonClearDataConfigTest::TearDownTestCase()
{
    CJson::cJsonPtr = nullptr;
    cJsonMock = nullptr;
}

/**
 * @tc.number: SUB_Clear_Data_Config_Test_0100
 * @tc.name: Clear_Data_Config_Test_0100
 * @tc.desc: 测试返回空指针情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonClearDataConfigTest, Clear_Data_Config_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-begin Clear_Data_Config_Test_0100";
    try {
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(nullptr));
        BJsonClearDataConfig config;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-end Clear_Data_Config_Test_0100";
}

/**
 * @tc.number: SUB_Clear_Data_Config_Test_0200
 * @tc.name: Clear_Data_Config_Test_0200
 * @tc.desc: 测试返回空指针情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonClearDataConfigTest, Clear_Data_Config_Test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-begin Clear_Data_Config_Test_0200";
    try {
        int cjson = 0;
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonClearDataConfig config;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-end Clear_Data_Config_Test_0200";
}

/**
 * @tc.number: SUB_Clear_Data_Config_Test_0201
 * @tc.name: Clear_Data_Config_Test_0201
 * @tc.desc: 测试返回空指针情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonClearDataConfigTest, Clear_Data_Config_Test_0201, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-begin Clear_Data_Config_Test_0201";
    try {
        string filePath = PATH + CONFIG_NAME;
        int cjson = 0;
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonClearDataConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-end Clear_Data_Config_Test_0201";
}

/**
 * @tc.number: SUB_Clear_Data_Config_Test_0300
 * @tc.name: Clear_Data_Config_Test_0300
 * @tc.desc: 测试 HasClearBundleRecord 接口失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonClearDataConfigTest, Clear_Data_Config_Test_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-begin Clear_Data_Config_Test_0300";
    try {
        string filePath = PATH + CONFIG_NAME;
        int cjson = 0;
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonClearDataConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(nullptr));
        auto ret = config.HasClearBundleRecord();
        EXPECT_FALSE(ret);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        ret = config.HasClearBundleRecord();
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-end Clear_Data_Config_Test_0300";
}

/**
 * @tc.number: SUB_Clear_Data_Config_Test_0301
 * @tc.name: Clear_Data_Config_Test_0301
 * @tc.desc: 测试 HasClearBundleRecord 接口成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonClearDataConfigTest, Clear_Data_Config_Test_0301, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-begin Clear_Data_Config_Test_0301";
    try {
        string filePath = PATH + CONFIG_NAME;
        int cjson = 0;
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonClearDataConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetArraySize(_)).WillOnce(Return(1));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        auto ret = config.HasClearBundleRecord();
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-end Clear_Data_Config_Test_0301";
}

/**
 * @tc.number: SUB_Clear_Data_Config_Test_0400
 * @tc.name: Clear_Data_Config_Test_0400
 * @tc.desc: 测试 FindClearBundleRecord 接口失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonClearDataConfigTest, Clear_Data_Config_Test_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-begin Clear_Data_Config_Test_0400";
    try {
        string bundleName = "test1";
        string filePath = PATH + CONFIG_NAME;
        int cjson = 0;
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonClearDataConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(nullptr));
        auto ret = config.FindClearBundleRecord(bundleName);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        ret = config.FindClearBundleRecord(bundleName);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-end Clear_Data_Config_Test_0400";
}

/**
 * @tc.number: SUB_Clear_Data_Config_Test_0401
 * @tc.name: Clear_Data_Config_Test_0401
 * @tc.desc: 测试 FindClearBundleRecord 接口失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonClearDataConfigTest, Clear_Data_Config_Test_0401, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-begin Clear_Data_Config_Test_0401";
    try {
        string bundleName = "test1";
        string filePath = PATH + CONFIG_NAME;
        int cjson = 0;
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonClearDataConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetArraySize(_)).WillOnce(Return(1)).WillOnce(Return(1));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        auto ret = config.FindClearBundleRecord(bundleName);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        ret = config.FindClearBundleRecord(bundleName);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetArraySize(_)).WillOnce(Return(1)).WillOnce(Return(1));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _))
            .WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)))
            .WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        ret = config.FindClearBundleRecord(bundleName);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-end Clear_Data_Config_Test_0401";
}

/**
 * @tc.number: SUB_Clear_Data_Config_Test_0500
 * @tc.name: Clear_Data_Config_Test_0500
 * @tc.desc: 测试 WriteClearBundleRecord 接口失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonClearDataConfigTest, Clear_Data_Config_Test_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-begin Clear_Data_Config_Test_0500";
    try {
        string bundleName = "test1";
        string filePath = PATH + CONFIG_NAME;
        int cjson = 0;
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonClearDataConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(nullptr));
        auto ret = config.WriteClearBundleRecord(bundleName);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        ret = config.WriteClearBundleRecord(bundleName);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        ret = config.WriteClearBundleRecord(bundleName);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddStringToObject(_, _, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToArray(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        ret = config.WriteClearBundleRecord(bundleName);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-end Clear_Data_Config_Test_0500";
}

/**
 * @tc.number: SUB_Clear_Data_Config_Test_0600
 * @tc.name: Clear_Data_Config_Test_0600
 * @tc.desc: 测试 DeleteClearBundleRecord 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonClearDataConfigTest, Clear_Data_Config_Test_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-begin Clear_Data_Config_Test_0600";
    try {
        string bundleName = "test1";
        string filePath = PATH + CONFIG_NAME;
        int cjson = 0;
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonClearDataConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        auto ret = config.DeleteClearBundleRecord(bundleName);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-end Clear_Data_Config_Test_0600";
}

/**
 * @tc.number: SUB_Clear_Data_Config_Test_0700
 * @tc.name: Clear_Data_Config_Test_0700
 * @tc.desc: 测试 InsertClearBundleRecord 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonClearDataConfigTest, Clear_Data_Config_Test_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-begin Clear_Data_Config_Test_0700";
    try {
        string bundleName = "test1";
        string filePath = PATH + CONFIG_NAME;
        int cjson = 0;
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonClearDataConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(nullptr));
        auto ret = config.InsertClearBundleRecord(bundleName);
        EXPECT_FALSE(ret);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        ret = config.InsertClearBundleRecord(bundleName);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-end Clear_Data_Config_Test_0700";
}

/**
 * @tc.number: SUB_Clear_Data_Config_Test_0800
 * @tc.name: Clear_Data_Config_Test_0800
 * @tc.desc: 测试 GetAllClearBundleRecords 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonClearDataConfigTest, Clear_Data_Config_Test_0800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-begin Clear_Data_Config_Test_0800";
    try {
        string bundleName = "test1";
        string filePath = PATH + CONFIG_NAME;
        int cjson = 0;
        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_AddItemToObject(_, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*cJsonMock, cJSON_Print(_)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        BJsonClearDataConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(nullptr));
        auto ret = config.GetAllClearBundleRecords(bundleName);
        EXPECT_EQ(ret, {});

        EXPECT_CALL(*cJsonMock, cJSON_Parse(_)).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_GetObjectItem(_, _)).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        ret = config.GetAllClearBundleRecords(bundleName);
        EXPECT_EQ(ret, {});
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonClearDataConfigTest-end Clear_Data_Config_Test_0800";
}
} // namespace OHOS::FileManagement::Backup