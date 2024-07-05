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
#include "b_json/b_json_service_disposal_config.h"

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
    static void SetUpTestCase(void) {};
    // 所有测试用例执行之后执行
    static void TearDownTestCase(void) {};
    // 每次测试用例执行之前执行
    void SetUp() {};
    // 每次测试用例执行之后执行
    void TearDown();
};

void BJsonServiceDisposalConfigTest::TearDown()
{
    BJsonDisposalConfig config;
    config.DeleteConfigFile();
};

/**
 * @tc.number: SUB_Disposal_Config_Test_0100
 * @tc.name: Disposal_Config_Test_0100
 * @tc.desc: 测试配置文件存在的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0100";
    try {
        string filePath = PATH + CONFIG_NAME;
        ofstream outFile(filePath);
        outFile.close();
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0100";
}

/**
 * @tc.number: SUB_Disposal_Config_Test_0101
 * @tc.name: Disposal_Config_Test_0101
 * @tc.desc: 测试配置文件不存在的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0101";
    try {
        string filePath = PATH + CONFIG_NAME;
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0101";
}

/**
 * @tc.number: SUB_Disposal_Config_Test_0200
 * @tc.name: Disposal_Config_Test_0200
 * @tc.desc: 测试写入成功的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0200";
    try {
        string filePath = PATH + CONFIG_NAME;
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
        const string bundleName = "test1";
        bool retAdd = config.AppendIntoDisposalConfigFile(bundleName);
        EXPECT_TRUE(retAdd);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0200";
}

/**
 * @tc.number: SUB_Disposal_Config_Test_0201
 * @tc.name: Disposal_Config_Test_0201
 * @tc.desc: 测试写入失败的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0201, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0201";
    try {
        string filePath = PATH + CONFIG_NAME;
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
        bool retDele = config.DeleteConfigFile();
        EXPECT_TRUE(retDele);
        const string bundleName = "test1";
        bool retAdd = config.AppendIntoDisposalConfigFile(bundleName);
        EXPECT_FALSE(retAdd);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0201";
}

/**
 * @tc.number: SUB_Disposal_Config_Test_0300
 * @tc.name: Disposal_Config_Test_0300
 * @tc.desc: 测试删除成功的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0300";
    try {
        string filePath = PATH + CONFIG_NAME;
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
        const string bundleName = "test1";
        bool retAdd = config.AppendIntoDisposalConfigFile(bundleName);
        EXPECT_TRUE(retAdd);
        bool retDel = config.DeleteFromDisposalConfigFile(bundleName);
        EXPECT_TRUE(retDel);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0300";
}

/**
 * @tc.number: SUB_Disposal_Config_Test_0301
 * @tc.name: Disposal_Config_Test_0301
 * @tc.desc: 测试删除失败的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0301, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0301";
    try {
        string filePath = PATH + CONFIG_NAME;
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
        const string bundleName = "test1";
        bool retAdd = config.AppendIntoDisposalConfigFile(bundleName);
        EXPECT_TRUE(retAdd);
        bool retDele = config.DeleteConfigFile();
        EXPECT_TRUE(retDele);
        bool retDel = config.DeleteFromDisposalConfigFile(bundleName);
        EXPECT_FALSE(retDel);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0301";
}

/**
 * @tc.number: SUB_Disposal_Config_Test_0400
 * @tc.name: Disposal_Config_Test_0400
 * @tc.desc: 测试判断成功的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0400";
    try {
        string filePath = PATH + CONFIG_NAME;
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
        const string bundleName = "test1";
        bool retAdd = config.AppendIntoDisposalConfigFile(bundleName);
        EXPECT_TRUE(retAdd);
        bool retIf = config.IfBundleNameInDisposalConfigFile(bundleName);
        EXPECT_TRUE(retIf);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0400";
}

/**
 * @tc.number: SUB_Disposal_Config_Test_0401
 * @tc.name: Disposal_Config_Test_0401
 * @tc.desc: 测试判断失败的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0401, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0401";
    try {
        string filePath = PATH + CONFIG_NAME;
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
        const string bundleName = "test1";
        bool retAdd = config.AppendIntoDisposalConfigFile(bundleName);
        EXPECT_TRUE(retAdd);
        bool retDele = config.DeleteConfigFile();
        EXPECT_TRUE(retDele);
        bool retIf = config.IfBundleNameInDisposalConfigFile(bundleName);
        EXPECT_FALSE(retIf);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0401";
}

/**
 * @tc.number: SUB_Disposal_Config_Test_0500
 * @tc.name: Disposal_Config_Test_0500
 * @tc.desc: 测试获取成功的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0500";
    try {
        string filePath = PATH + CONFIG_NAME;
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
        const string bundleName = "test1";
        bool retAdd = config.AppendIntoDisposalConfigFile(bundleName);
        EXPECT_TRUE(retAdd);
        vector<string> bundleNameList = config.GetBundleNameFromConfigFile();
        bool result = false;
        if (bundleNameList.size() == 1) {
            result = true;
        }
        EXPECT_TRUE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0500";
}

/**
 * @tc.number: SUB_Disposal_Config_Test_0501
 * @tc.name: Disposal_Config_Test_0501
 * @tc.desc: 测试获取失败的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0501, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0501";
    try {
        string filePath = PATH + CONFIG_NAME;
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
        vector<string> bundleNameList = config.GetBundleNameFromConfigFile();
        bool result = false;
        if (bundleNameList.size() == 0) {
            result = true;
        }
        EXPECT_TRUE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0501";
}

/**
 * @tc.number: SUB_Disposal_Config_Test_0600
 * @tc.name: Disposal_Config_Test_0600
 * @tc.desc: 测试删除成功的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0600";
    try {
        string filePath = PATH + CONFIG_NAME;
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
        bool retDel = config.DeleteConfigFile();
        EXPECT_TRUE(retDel);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0600";
}

/**
 * @tc.number: SUB_Disposal_Config_Test_0601
 * @tc.name: Disposal_Config_Test_0601
 * @tc.desc: 测试删除失败的情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: IAAMIK
 */
HWTEST_F(BJsonServiceDisposalConfigTest, Disposal_Config_Test_0601, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-begin Disposal_Config_Test_0601";
    try {
        string filePath = PATH + CONFIG_NAME;
        BJsonDisposalConfig config;
        EXPECT_EQ(access(filePath.c_str(), F_OK), 0);
        bool retDele = config.DeleteConfigFile();
        EXPECT_TRUE(retDele);
        bool retDel = config.DeleteConfigFile();
        EXPECT_FALSE(retDel);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonServiceDisposalConfigTest-end Disposal_Config_Test_0601";
}
} // namespace OHOS::FileManagement::Backup