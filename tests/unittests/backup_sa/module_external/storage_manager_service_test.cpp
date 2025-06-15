/*)
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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "b_jsonutil_mock.h"
#include "b_sa_utils_mock.h"

#include <fstream>
#include <iostream>
#include <refbase.h>
#include "file_uri.h"
#include "sandbox_helper.h"
#include "module_external/storage_manager_service.h"
#include "module_external/storage_manager_service.cpp"

namespace OHOS {
const std::string CAMERA_BUNDLENAME = "file";
using namespace std;
using namespace testing;
using namespace FileManagement::Backup;
namespace fs = std::filesystem;
class StorageManagerServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void StorageManagerServiceTest::SetUpTestCase(void) {}
void StorageManagerServiceTest::TearDownTestCase(void) {}
void StorageManagerServiceTest::SetUp(void) {}
void StorageManagerServiceTest::TearDown(void) {}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetBundleStatsForIncrease_001
 * @tc.desc: check the GetBundleStatsForIncrease function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetBundleStatsForIncrease_001,
    testing::ext::TestSize.Level1)
{
    uint32_t userId = 100;
    std::vector<std::string> bundleNames = {"com.example.app1", "com.example.app2"};
    std::vector<int64_t> incrementalBackTimes = {123456789};
    std::vector<int64_t> pkgFileSizes;
    std::vector<int64_t> incPkgFileSizes;
    int32_t result = StorageManagerService::GetInstance().GetBundleStatsForIncrease(userId, bundleNames,
        incrementalBackTimes, pkgFileSizes, incPkgFileSizes);
    EXPECT_EQ(result, E_ERR);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetBundleStatsForIncrease_002
 * @tc.desc: check the GetBundleStatsForIncrease function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetBundleStatsForIncrease_002,
    testing::ext::TestSize.Level1)
{
    uint32_t userId = 100;
    std::vector<std::string> bundleNames = {"com.example.app1", "com.example.app2"};
    std::vector<int64_t> incrementalBackTimes = {123456789, 987654321};
    std::vector<int64_t> pkgFileSizes;
    std::vector<int64_t> incPkgFileSizes;
    int32_t result = StorageManagerService::GetInstance().GetBundleStatsForIncrease(userId, bundleNames,
        incrementalBackTimes, pkgFileSizes, incPkgFileSizes);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetBundleStatsForIncreaseEach_001
 * @tc.desc: check the GetBundleStatsForIncreaseEach function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetBundleStatsForIncreaseEach_001,
    testing::ext::TestSize.Level1)
{
    uint32_t userId = 100;
    std::string bundleName = "testBundle";
    int64_t lastBackupTime = 123456789;
    std::vector<int64_t> pkgFileSizes;
    std::vector<int64_t> incPkgFileSizes;
    StorageManagerService::GetInstance().GetBundleStatsForIncreaseEach(userId, bundleName, lastBackupTime,
        pkgFileSizes, incPkgFileSizes);
    EXPECT_EQ(pkgFileSizes.size(), 1);
    EXPECT_EQ(pkgFileSizes[0], 0);
    EXPECT_EQ(incPkgFileSizes.size(), 1);
    EXPECT_EQ(incPkgFileSizes[0], 0);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_ReadIncludesExcludesPath_001
 * @tc.desc: check the ReadIncludesExcludesPath function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ReadIncludesExcludesPath_001,
    testing::ext::TestSize.Level1)
{
    std::string bundleName = "";
    int64_t lastBackupTime = 123456789;
    uint32_t userId = 100;
    auto result = StorageManagerService::GetInstance().ReadIncludesExcludesPath(bundleName, lastBackupTime, userId);
    EXPECT_TRUE(std::get<0>(result).empty());
    EXPECT_TRUE(std::get<1>(result).empty());
}

/**
 * @tc.name: Storage_Manager_ServiceTest_ReadIncludesExcludesPath_002
 * @tc.desc: check the ReadIncludesExcludesPath function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ReadIncludesExcludesPath_002,
    testing::ext::TestSize.Level1)
{
    std::string bundleName = "testBundle";
    int64_t lastBackupTime = 123456789;
    uint32_t userId = 100;
    // Assuming the file does not exist or cannot be opened
    auto result = StorageManagerService::GetInstance().ReadIncludesExcludesPath(bundleName, lastBackupTime, userId);
    EXPECT_TRUE(std::get<0>(result).empty());
    EXPECT_TRUE(std::get<1>(result).empty());
}
}
