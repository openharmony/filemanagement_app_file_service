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

#include "cjson_func_define.h"
#include "b_jsonutil.cpp"
#include "cjson_func_undef.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class BJsonUtilUtTest : public testing::Test {
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

void BJsonUtilUtTest::SetUpTestCase()
{
    cJsonMock = make_shared<CJsonMock>();
    CJson::cJsonPtr = cJsonMock;
}

void BJsonUtilUtTest::TearDownTestCase()
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
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilUtTest, b_jsonutil_BuildExtensionErrInfo_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilUtTest-begin BuildExtensionErrInfo_0101";
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
        GTEST_LOG_(INFO) << "BJsonUtilUtTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilUtTest-end BuildExtensionErrInfo_0101";
}

/**
 * @tc.number: b_jsonutil_BuildExtensionErrInfo_0201
 * @tc.name: b_jsonutil_BuildExtensionErrInfo_0201
 * @tc.desc: Test function of BuildExtensionErrInfo interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonUtilUtTest, b_jsonutil_BuildExtensionErrInfo_0201, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonUtilUtTest-begin BuildExtensionErrInfo_0201";
    try {
        std::string jsonStr;
        std::map<std::string, std::vector<int>> errFileInfo;
        int cjson = 0;

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(nullptr));
        bool result = BJsonUtil::BuildExtensionErrInfo(jsonStr, errFileInfo);
        EXPECT_FALSE(result);

        EXPECT_CALL(*cJsonMock, cJSON_CreateObject()).WillOnce(Return(reinterpret_cast<cJSON *>(&cjson)));
        EXPECT_CALL(*cJsonMock, cJSON_CreateArray()).WillOnce(Return(nullptr));
        EXPECT_CALL(*cJsonMock, cJSON_Delete(_)).WillOnce(Return());
        result = BJsonUtil::BuildExtensionErrInfo(jsonStr, errFileInfo);
        EXPECT_FALSE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonUtilUtTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonUtilUtTest-end BuildExtensionErrInfo_0201";
}
}