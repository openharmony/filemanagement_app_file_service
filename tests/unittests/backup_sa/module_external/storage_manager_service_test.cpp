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

/**
 * @tc.name: Storage_Manager_ServiceTest_DealWithIncludeFiles_001
 * @tc.desc: check the DealWithIncludeFiles function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_DealWithIncludeFiles_001,
    testing::ext::TestSize.Level1)
{
    std::string bundleName = CAMERA_BUNDLENAME;
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};
    std::vector<std::string> includes = {"data/storage/el1/base/" + DEFAULT_PATH_WITH_WILDCARD};
    std::vector<std::string> phyIncludes;
    std::map<std::string, std::string> pathMap;
    StorageManagerService::GetInstance().DealWithIncludeFiles(paras, includes, phyIncludes, pathMap);
    EXPECT_TRUE(phyIncludes.empty());
}

/**
 * @tc.name: Storage_Manager_ServiceTest_DealWithIncludeFiles_002
 * @tc.desc: check the DealWithIncludeFiles function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_DealWithIncludeFiles_002,
    testing::ext::TestSize.Level1)
{
    std::string bundleName = CAMERA_BUNDLENAME;
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};
    std::vector<std::string> includes = {NORMAL_SAND_PREFIX};
    std::vector<std::string> phyIncludes;
    std::map<std::string, std::string> pathMap;
    StorageManagerService::GetInstance().DealWithIncludeFiles(paras, includes, phyIncludes, pathMap);
    EXPECT_TRUE(phyIncludes.empty());
}

/**
 * @tc.name: Storage_Manager_ServiceTest_DealWithIncludeFiles_003
 * @tc.desc: check the DealWithIncludeFiles function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_DealWithIncludeFiles_003,
    testing::ext::TestSize.Level1)
{
    std::string bundleName = "testBundle";
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};
    std::vector<std::string> includes = {};
    std::vector<std::string> phyIncludes;
    std::map<std::string, std::string> pathMap;
    StorageManagerService::GetInstance().DealWithIncludeFiles(paras, includes, phyIncludes, pathMap);
    EXPECT_TRUE(phyIncludes.empty());
}

/**
 * @tc.name: Storage_Manager_ServiceTest_ConvertSandboxRealPath_001
 * @tc.desc: check the ConvertSandboxRealPath function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ConvertSandboxRealPath_001,
    testing::ext::TestSize.Level1)
{
    uint32_t userId = 100;
    std::string bundleName = "com.example.app";
    std::string sandboxPathStr = NORMAL_SAND_PREFIX + "/path/to/file";
    std::vector<std::string> realPaths;
    std::map<std::string, std::string> pathMap;
    StorageManagerService::GetInstance().ConvertSandboxRealPath(userId, bundleName, sandboxPathStr, realPaths, pathMap);
    EXPECT_TRUE(realPaths.empty());
    EXPECT_TRUE(pathMap.empty());
}

/**
 * @tc.name: Storage_Manager_ServiceTest_ConvertSandboxRealPath_002
 * @tc.desc: check the ConvertSandboxRealPath function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ConvertSandboxRealPath_002,
    testing::ext::TestSize.Level1)
{
    uint32_t userId = 100;
    std::string bundleName = CAMERA_BUNDLENAME;
    std::string sandboxPathStr = NORMAL_SAND_PREFIX + "/el2/database/";
    std::vector<std::string> realPaths;
    std::map<std::string, std::string> pathMap;
    StorageManagerService::GetInstance().ConvertSandboxRealPath(userId, bundleName, sandboxPathStr, realPaths, pathMap);
    std::string uriString = URI_PREFIX + bundleName + AppFileService::SandboxHelper::Encode(sandboxPathStr);
    AppFileService::ModuleFileUri::FileUri uri(uriString);
    std::string physicalPath;
    AppFileService::SandboxHelper::GetBackupPhysicalPath(uri.ToString(), std::to_string(userId), physicalPath);
    EXPECT_FALSE(realPaths.empty());
    EXPECT_FALSE(pathMap.empty());
    EXPECT_EQ(realPaths[0], physicalPath);
    EXPECT_EQ(pathMap[realPaths[0]], sandboxPathStr);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_CheckIfDirForIncludes_001
 * @tc.desc: check the CheckIfDirForIncludes function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_CheckIfDirForIncludes_001,
    testing::ext::TestSize.Level1)
{
    ofstream closedStatFile;
    std::string bundleName = "testBundle";
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};
    std::map<std::string, std::string> pathMap;
    std::map<std::string, bool> excludesMap;
    auto result = StorageManagerService::GetInstance().CheckIfDirForIncludes("test_path", paras,
        pathMap, closedStatFile, excludesMap);
    EXPECT_EQ(result, std::make_tuple(false, false));
}

/**
 * @tc.name: Storage_Manager_ServiceTest_CheckIfDirForIncludes_002
 * @tc.desc: check the CheckIfDirForIncludes function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_CheckIfDirForIncludes_002,
    testing::ext::TestSize.Level1)
{
    ofstream statFile;
    std::string bundleName = "testBundle";
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};
    std::map<std::string, std::string> pathMap;
    std::map<std::string, bool> excludesMap;
    auto result = StorageManagerService::GetInstance().CheckIfDirForIncludes("", paras, pathMap, statFile, excludesMap);
    EXPECT_EQ(result, std::make_tuple(false, false));
}

/**
 * @tc.name: Storage_Manager_ServiceTest_CheckIfDirForIncludes_003
 * @tc.desc: check the CheckIfDirForIncludes function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_CheckIfDirForIncludes_003,
    testing::ext::TestSize.Level1)
{
    std::ofstream statFile;
    fs::path tempPath = "/data/temp.txt";

    try {
        fs::path dirPath = tempPath.parent_path();
        if (!fs::exists(dirPath)) {
            fs::create_directories(dirPath);
        }
        fs::path canonicalPath = fs::canonical(fs::absolute(tempPath));
        statFile.open(canonicalPath, std::ios::out | std::ios::trunc);
        EXPECT_FALSE(!statFile.is_open()) << "file can not open";
        std::string bundleName = "testBundle";
        BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                                .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};
        std::map<std::string, std::string> pathMap;
        std::map<std::string, bool> excludesMap;
        auto result = StorageManagerService::GetInstance().CheckIfDirForIncludes("/data/service", paras, pathMap,
            statFile, excludesMap);
        EXPECT_EQ(result, std::make_tuple(true, true));
        fs::remove(canonicalPath);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
    }

    statFile.open("/system/etc/NOTICE.TXT", ios::out | ios::trunc);
    EXPECT_FALSE(!statFile.is_open()) << "file can not open";
    std::string bundleName = "testBundle";
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};
    std::map<std::string, std::string> pathMap;
    std::map<std::string, bool> excludesMap;
    auto result = StorageManagerService::GetInstance().CheckIfDirForIncludes("/data/service", paras, pathMap,
        statFile, excludesMap);
    EXPECT_EQ(result, std::make_tuple(true, true));
}

/**
 * @tc.name: Storage_Manager_ServiceTest_ExcludeFilter_001
 * @tc.desc: check the ExcludeFilter function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ExcludeFilter_001,
    testing::ext::TestSize.Level1)
{
    std::map<std::string, bool> excludesMap;
    std::string path = "";
    bool result = StorageManagerService::GetInstance().ExcludeFilter(excludesMap, path);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_ExcludeFilter_002
 * @tc.desc: check the ExcludeFilter function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ExcludeFilter_002,
    testing::ext::TestSize.Level1)
{
    std::map<std::string, bool> excludesMap;
    std::string path = "/path/to/file";
    bool result = StorageManagerService::GetInstance().ExcludeFilter(excludesMap, path);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetUserStorageStatsByType_001
 * @tc.desc: check the GetUserStorageStatsByType function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetUserStorageStatsByType_001,
    testing::ext::TestSize.Level1)
{
    StorageManager::StorageStats storageStats;
    std::string type = "MEDIA_TYPE";
    int32_t userId = 100;
    int64_t result = StorageManagerService::GetInstance().GetUserStorageStatsByType(userId, storageStats, type);
    EXPECT_EQ(result, E_ERR);
    EXPECT_EQ(storageStats.video_, 0);
    EXPECT_EQ(storageStats.image_, 0);
    EXPECT_EQ(storageStats.file_, 0);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetUserStorageStatsByType_002
 * @tc.desc: check the GetUserStorageStatsByType function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetUserStorageStatsByType_002,
    testing::ext::TestSize.Level1)
{
    StorageManager::StorageStats storageStats;
    std::string type = "FILE_TYPE";
    int32_t userId = 100;
    int64_t result = StorageManagerService::GetInstance().GetUserStorageStatsByType(userId, storageStats, type);
    EXPECT_EQ(result, E_ERR);
    EXPECT_EQ(storageStats.video_, 0);
    EXPECT_EQ(storageStats.image_, 0);
    EXPECT_EQ(storageStats.file_, 0);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetUserStorageStatsByType_003
 * @tc.desc: check the GetUserStorageStatsByType function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetUserStorageStatsByType_003,
    testing::ext::TestSize.Level1)
{
    StorageManager::StorageStats storageStats;
    std::string type = "OTHER_TYPE";
    int32_t userId = 100;
    int64_t result = StorageManagerService::GetInstance().GetUserStorageStatsByType(userId, storageStats, type);
    EXPECT_EQ(result, E_ERR);
    EXPECT_EQ(storageStats.video_, 0);
    EXPECT_EQ(storageStats.image_, 0);
    EXPECT_EQ(storageStats.file_, 0);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetMediaTypeAndSize_001
 * @tc.desc: check the GetMediaTypeAndSize function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetMediaTypeAndSize_001,
    testing::ext::TestSize.Level1)
{
    StorageManager::StorageStats storageStats;
    StorageManagerService::GetInstance().GetMediaTypeAndSize(nullptr, storageStats);
    EXPECT_EQ(storageStats.image_, 0);
    EXPECT_EQ(storageStats.audio_, 0);
    EXPECT_EQ(storageStats.video_, 0);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetMediaTypeAndSize_002
 * @tc.desc: check the GetMediaTypeAndSize function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetMediaTypeAndSize_002,
    testing::ext::TestSize.Level1)
{
    auto resultSet = std::make_shared<DataShare::DataShareResultSet>();
    StorageManager::StorageStats storageStats;
    StorageManagerService::GetInstance().GetMediaTypeAndSize(nullptr, storageStats);
    EXPECT_EQ(storageStats.image_, 0);
    EXPECT_EQ(storageStats.audio_, 0);
    EXPECT_EQ(storageStats.video_, 0);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetQuotaSrcMountPath_001
 * @tc.desc: check the GetQuotaSrcMountPath function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetQuotaSrcMountPath_001,
    testing::ext::TestSize.Level1)
{
    mQuotaReverseMounts.insert(std::make_pair("/path1", "/mount1"));
    mQuotaReverseMounts.insert(std::make_pair("/path2", "/mount2"));
    std::string target = "/path1";
    std::string expectedPath = "/mount1";
    std::string actualPath = GetQuotaSrcMountPath(target);
    EXPECT_EQ(expectedPath, actualPath);
    mQuotaReverseMounts.clear();
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetQuotaSrcMountPath_002
 * @tc.desc: check the GetQuotaSrcMountPath function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetQuotaSrcMountPath_002,
    testing::ext::TestSize.Level1)
{
    mQuotaReverseMounts.insert(std::make_pair("/path1", "/mount1"));
    mQuotaReverseMounts.insert(std::make_pair("/path1", "/mount1"));
    std::string target = "/path3";
    std::string expectedPath = "";
    std::string actualPath = GetQuotaSrcMountPath(target);
    EXPECT_EQ(expectedPath, actualPath);
    mQuotaReverseMounts.clear();
}

/**
 * @tc.name: Storage_Manager_ServiceTest_PathSortFunc_001
 * @tc.desc: check the PathSortFunc function
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_PathSortFunc_001,
    testing::ext::TestSize.Level1)
{
    std::string path1 = "AAA";
    std::string path2 = "BBB";
    EXPECT_TRUE(PathSortFunc(path1, path2));
}
}