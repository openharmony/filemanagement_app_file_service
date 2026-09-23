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
#include <limits>
#include <refbase.h>
#include "file_uri.h"
#include "sandbox_helper.h"
#include "module_external/storage_manager_service.h"

namespace OHOS {
const std::string CAMERA_BUNDLENAME = "file";
const std::string MMS_BUNDLENAME = "com.ohos.mms";
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
 * @tc.name: Storage_Manager_ServiceTest_ConvertSandboxRealPath_003
 * @tc.desc: check ConvertSandboxRealPath with empty sandboxPathStr
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ConvertSandboxRealPath_003,
    testing::ext::TestSize.Level1)
{
    uint32_t userId = 100;
    std::string bundleName = "com.example.app";
    std::string sandboxPathStr;
    std::vector<std::string> realPaths;
    std::map<std::string, std::string> pathMap;
    StorageManagerService::GetInstance().ConvertSandboxRealPath(userId, bundleName, sandboxPathStr, realPaths, pathMap);
    EXPECT_TRUE(realPaths.empty());
    EXPECT_TRUE(pathMap.empty());
}

/**
 * @tc.name: Storage_Manager_ServiceTest_ConvertSandboxRealPath_004
 * @tc.desc: check ConvertSandboxRealPath with invalid path containing traversal
 * @tc.type: FUNC
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ConvertSandboxRealPath_004,
    testing::ext::TestSize.Level1)
{
    uint32_t userId = 100;
    std::string bundleName = "com.example.app";
    std::string sandboxPathStr = NORMAL_SAND_PREFIX + "/../etc/passwd";
    std::vector<std::string> realPaths;
    std::map<std::string, std::string> pathMap;
    StorageManagerService::GetInstance().ConvertSandboxRealPath(userId, bundleName, sandboxPathStr, realPaths, pathMap);
    EXPECT_TRUE(realPaths.empty());
    EXPECT_TRUE(pathMap.empty());
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
    std::string bundleName = "com.example.app";
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
    std::string bundleName = "com.example.app";
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
    std::string type = FILE_TYPE;
    int32_t userId = 100;
    int64_t result = StorageManagerService::GetInstance().GetUserStorageStatsByType(userId, storageStats, type);
    EXPECT_EQ(result, 0);
    EXPECT_GE(storageStats.video_, 0);
    EXPECT_GE(storageStats.image_, 0);
    EXPECT_GE(storageStats.file_, 0);
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
    std::string type = "other";
    int32_t userId = 100;
    int64_t result = StorageManagerService::GetInstance().GetUserStorageStatsByType(userId, storageStats, type);
    EXPECT_EQ(result, E_ERR);
    EXPECT_GE(storageStats.video_, 0);
    EXPECT_GE(storageStats.image_, 0);
    EXPECT_GE(storageStats.file_, 0);
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

    auto resultSet = std::make_shared<DataShare::DataShareResultSet>();
    StorageManagerService::GetInstance().GetMediaTypeAndSize(resultSet, storageStats);

    EXPECT_EQ(storageStats.image_, 0);
    EXPECT_EQ(storageStats.audio_, 0);
    EXPECT_EQ(storageStats.video_, 0);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_DeduplicationPath_001
 * @tc.number: DeduplicationPathTest_001
 * @tc.desc: 测试configPath为空时，不执行任何操作
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_DeduplicationPath_001,
    testing::ext::TestSize.Level1)
{
    std::vector<std::string> configPaths;
    StorageManagerService::GetInstance().DeduplicationPath(configPaths);
    EXPECT_TRUE(configPaths.size() == 0);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_DeduplicationPath_002
 * @tc.number: DeduplicationPathTest_002
 * @tc.desc: 测试configPath不为空时，执行去重
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_DeduplicationPath_002,
    testing::ext::TestSize.Level1)
{
    std::vector<std::string> configPaths = {"path1", "path2", "path2"};
    StorageManagerService::GetInstance().DeduplicationPath(configPaths);
    EXPECT_TRUE(configPaths.size() == 2);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_ScanExtensionPath_001
 * @tc.number: ScanExtensionPathTest_001
 * @tc.desc: ScanExtensionPath 有效路径正确扫描
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ScanExtensionPath_001,
    testing::ext::TestSize.Level1)
{
    std::string bundleName = MMS_BUNDLENAME;
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 123456789, .fileSizeSum = 0, .incFileSizeSum = 0};
    std::vector<std::string> includes = {"/path/to/include"};
    std::vector<std::string> excludes = {"/path/to/exclude"};
    std::map<std::string, std::string> pathMap;
    std::ofstream statFile("statfile.txt");

    StorageManagerService::GetInstance().ScanExtensionPath(paras, includes, excludes, pathMap, statFile);
    EXPECT_TRUE(pathMap.empty());
    EXPECT_TRUE(statFile.good());

    statFile.close();
    remove("statfile.txt");
}

/**
 * @tc.name: Storage_Manager_ServiceTest_ScanExtensionPath_002
 * @tc.number: ScanExtensionPathTest_002
 * @tc.desc: ScanExtensionPath 扫描时排除符号链接
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ScanExtensionPath_002,
    testing::ext::TestSize.Level1)
{
    std::string testSuffix = std::to_string(getpid());
    fs::path testRoot = fs::temp_directory_path() / ("backup_scan_link_" + testSuffix);
    fs::path targetFile = fs::temp_directory_path() / ("backup_scan_target_" + testSuffix);
    fs::path linkPath = testRoot / "file_link";
    fs::path statPath = fs::temp_directory_path() / ("backup_scan_stat_" + testSuffix);
    fs::create_directories(testRoot);
    {
        std::ofstream target(targetFile);
        target << std::string(4096, 'a');
    }
    ASSERT_EQ(symlink(targetFile.c_str(), linkPath.c_str()), 0);
    struct stat rootStat = {0};
    ASSERT_EQ(lstat(testRoot.c_str(), &rootStat), 0);

    std::string bundleName = MMS_BUNDLENAME;
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
        .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};
    std::vector<std::string> includes = {testRoot.string()};
    std::vector<std::string> excludes;
    std::map<std::string, std::string> pathMap;
    std::ofstream statFile(statPath);
    StorageManagerService::GetInstance().ScanExtensionPath(paras, includes, excludes, pathMap, statFile);
    statFile.close();

    std::ifstream resultFile(statPath);
    std::string result((std::istreambuf_iterator<char>(resultFile)), std::istreambuf_iterator<char>());
    EXPECT_EQ(result.find("file_link"), std::string::npos);
    EXPECT_EQ(paras.fileSizeSum, rootStat.st_size);
    EXPECT_EQ(paras.incFileSizeSum, rootStat.st_size);

    fs::remove_all(testRoot);
    fs::remove(targetFile);
    fs::remove(statPath);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_AddOuterDirIntoFileStat_001
 * @tc.number: AddOuterDirIntoFileStat_001
 * @tc.desc: AddOuterDirIntoFileStat 调用时正常返回
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_AddOuterDirIntoFileStat_001,
    testing::ext::TestSize.Level1)
{
    std::string bundleName = MMS_BUNDLENAME;
    std::string dir = "/data/app/el1/100/base/" + bundleName + "/.backup";
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};
    std::string sandboxDir = "/path/to/sandboxDir";
    std::ofstream statFile("statfile.txt");
    std::map<std::string, bool> excludesMap;

    StorageManagerService::GetInstance().AddOuterDirIntoFileStat(dir, paras, sandboxDir, statFile, excludesMap);
    EXPECT_TRUE(excludesMap.empty());

    dir = "";
    StorageManagerService::GetInstance().AddOuterDirIntoFileStat(dir, paras, sandboxDir, statFile, excludesMap);
    EXPECT_TRUE(excludesMap.empty());

    statFile.close();
    remove("statfile.txt");
}

/**
 * @tc.name: Storage_Manager_ServiceTest_InsertStatFile_001
 * @tc.number: InsertStatFile_001
 * @tc.desc: 测试 InsertStatFile 函数在输入有效时是否正确插入文件信息
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_InsertStatFile_001, testing::ext::TestSize.Level1)
{
    std::string bundleName = MMS_BUNDLENAME;
    std::string path = "/data/app/el1/100/base/" + bundleName + "/.backup";
    struct FileStat fileStat = {.isDir = true};
    std::ofstream statFile("test_stat_file.txt");
    std::map<std::string, bool> excludesMap;
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};

    StorageManagerService::GetInstance().InsertStatFile(path, fileStat, statFile, excludesMap, paras);
    EXPECT_TRUE(excludesMap.empty());
    statFile.close();
}

/**
 * @tc.name: Storage_Manager_ServiceTest_InsertStatFile_002
 * @tc.number: InsertStatFile_002
 * @tc.desc: 测试 InsertStatFile 函数路径在excludesMap中时是否正确排除文件信息
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_InsertStatFile_002, testing::ext::TestSize.Level1)
{
    std::string bundleName = MMS_BUNDLENAME;
    std::string path = "/data/app/el1/100/base/" + bundleName + "/.backup";
    struct FileStat fileStat = {};
    std::ofstream statFile("test_stat_file.txt");
    std::map<std::string, bool> excludesMap = {{path, true}};
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};

    StorageManagerService::GetInstance().InsertStatFile(path, fileStat, statFile, excludesMap, paras);
    EXPECT_TRUE(excludesMap.find(path) != excludesMap.end());
    statFile.close();
}

/**
 * @tc.name: Storage_Manager_ServiceTest_InsertStatFile_003
 * @tc.number: InsertStatFile_003
 * @tc.desc: 测试 InsertStatFile 函数路径在输入无效文件状态信息时的处理
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_InsertStatFile_003, testing::ext::TestSize.Level1)
{
    std::string bundleName = MMS_BUNDLENAME;
    std::string path = "/invalid/path";
    struct FileStat fileStat = {};
    std::ofstream statFile("test_stat_file.txt");
    std::map<std::string, bool> excludesMap;
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};

    StorageManagerService::GetInstance().InsertStatFile(path, fileStat, statFile, excludesMap, paras);
    EXPECT_TRUE(excludesMap.empty());
    statFile.close();
}

/**
 * @tc.name: Storage_Manager_ServiceTest_AddPathMapForPathWildCard_001
 * @tc.number: AddPathMapForPathWildCard_001
 * @tc.desc: 测试 AddPathMapForPathWildCard 函数返回值正常
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_AddPathMapForPathWildCard_001,
    testing::ext::TestSize.Level1)
{
    uint32_t userId = 100;
    std::string name = MMS_BUNDLENAME;
    std::string phyPath = "/data/app/el1/100/base/com.ohos.mms/.backup";
    std::map<std::string, std::string> pathMap;

    bool result = StorageManagerService::GetInstance().AddPathMapForPathWildCard(userId, name, phyPath, pathMap);
    EXPECT_EQ(result, true);

    name = "com.example.app2";
    phyPath = "/data/app/el2/100/base/com.example.app2/.backup";
    pathMap.insert({phyPath, ".backup"});
    result = StorageManagerService::GetInstance().AddPathMapForPathWildCard(userId, name, phyPath, pathMap);
    EXPECT_EQ(result, true);

    phyPath = "";
    result = StorageManagerService::GetInstance().AddPathMapForPathWildCard(userId, name, phyPath, pathMap);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_WriteFileList_001
 * @tc.number: WriteFileList_001
 * @tc.desc: 测试 WriteFileList 函数返回值正常
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_WriteFileList_001,
    testing::ext::TestSize.Level1)
{
    std::ofstream statFile;
    fs::path tempPath = "/data/temp.txt";
    std::string bundleName = "bundle";
    try {
        fs::path dirPath = tempPath.parent_path();
        if (!fs::exists(dirPath)) {
            fs::create_directories(dirPath);
        }
        fs::path canonicalPath = fs::weakly_canonical(fs::absolute(tempPath));
        statFile.open(canonicalPath, std::ios::out | std::ios::trunc);
        ASSERT_FALSE(!statFile.is_open()) << "file can not open";

        struct FileStat fileStat = {.filePath = "/special;path/special;aaa;filename", .fileSize = 0,
            .lastUpdateTime = 0, .mode = 0, .isDir = false, .isIncre = true};
        struct FileStat fileStat2 = {.filePath = "/special\npath/special;aaa;filename", .fileSize = 0,
            .lastUpdateTime = 0, .mode = 0, .isDir = false, .isIncre = true};
        BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
            .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};
        StorageManagerService::GetInstance().WriteFileList(statFile, fileStat, paras);
        StorageManagerService::GetInstance().WriteFileList(statFile, fileStat2, paras);
        statFile.close();

        std::ifstream iStatFile;
        iStatFile.open(canonicalPath);
        ASSERT_FALSE(!iStatFile.is_open()) << "file can not open";
        std::string line;
        std::getline(iStatFile, line);
        size_t pos = line.rfind(';');
        ASSERT_TRUE(pos != std::string::npos);
        std::string encodeFlag = line.substr(pos + 1);
        ASSERT_FALSE(encodeFlag.empty());
        EXPECT_EQ(std::stoi(encodeFlag), 1);
        iStatFile.close();
        fs::remove(canonicalPath);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
    }
}

/**
 * @tc.name: Storage_Manager_ServiceTest_BasicApi_001
 * @tc.desc: cover simple APIs and deterministic media path conversion branches
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_BasicApi_001,
    testing::ext::TestSize.Level1)
{
    auto &service = StorageManagerService::GetInstance();
    StorageManager::BundleStats bundleStats;
    EXPECT_TRUE(service.GetBundleStats("bundle", bundleStats));
    EXPECT_EQ(service.UpdateMemoryPara(1, 0), E_OK);

    uint32_t userId = 100;
    std::vector<std::string> realPaths;
    std::map<std::string, std::string> pathMap;
    std::string mediaPath = MEDIA_SAND_PREFIX + "/Photo/test.jpg";
    service.ConvertSandboxRealPath(userId, "bundle", mediaPath, realPaths, pathMap);
    ASSERT_EQ(realPaths.size(), 1);
    EXPECT_EQ(realPaths[0], MEDIA_SAND_PREFIX + "/100/Photo/test.jpg");
    EXPECT_EQ(pathMap[realPaths[0]], mediaPath);

    std::string cloudPath = MEDIA_CLOUD_SAND_PREFIX + "/Photo/cloud.jpg";
    service.ConvertSandboxRealPath(userId, "bundle", cloudPath, realPaths, pathMap);
    ASSERT_EQ(realPaths.size(), 2);
    EXPECT_EQ(realPaths[1], MEDIA_CLOUD_SAND_PREFIX + "/100/Photo/cloud.jpg");
    EXPECT_EQ(pathMap[realPaths[1]], cloudPath);

    service.ConvertSandboxRealPath(userId, "bundle", "/valid/but/unsupported", realPaths, pathMap);
    EXPECT_EQ(realPaths.size(), 2);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_ReadIncludesExcludesPath_003
 * @tc.desc: cover successful parsing of include and exclude sections
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ReadIncludesExcludesPath_003,
    testing::ext::TestSize.Level1)
{
    uint32_t userId = 100;
    int64_t backupTime = 0;
    std::string bundleName = "coverage.bundle." + std::to_string(getpid());
    fs::path bundleDir = BACKUP_PATH_PREFIX + std::to_string(userId) + BACKUP_PATH_SURFFIX + bundleName;
    fs::create_directories(bundleDir);
    fs::path configPath = bundleDir / (BACKUP_INCEXC_SYMBOL + std::to_string(backupTime));
    {
        std::ofstream configFile(configPath);
        ASSERT_TRUE(configFile.is_open());
        configFile << BACKUP_INCLUDE << '\n';
        configFile << MEDIA_SAND_PREFIX + "/Photo" << '\n';
        configFile << BACKUP_EXCLUDE << '\n';
        configFile << MEDIA_SAND_PREFIX + "/Photo/hidden" << '\n';
    }

    auto [includes, excludes] = StorageManagerService::GetInstance().ReadIncludesExcludesPath(
        bundleName, backupTime, userId);
    ASSERT_EQ(includes.size(), 1);
    ASSERT_EQ(excludes.size(), 1);
    EXPECT_EQ(includes[0], MEDIA_SAND_PREFIX + "/Photo");
    EXPECT_EQ(excludes[0], MEDIA_SAND_PREFIX + "/Photo/hidden");
    fs::remove_all(bundleDir);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetBundleStatsForIncreaseEach_002
 * @tc.desc: cover the normal include/exclude conversion and stat-file path
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetBundleStatsForIncreaseEach_002,
    testing::ext::TestSize.Level1)
{
    uint32_t userId = 100;
    int64_t backupTime = 0;
    std::string bundleName = "coverage.stats." + std::to_string(getpid());
    fs::path bundleDir = BACKUP_PATH_PREFIX + std::to_string(userId) + BACKUP_PATH_SURFFIX + bundleName;
    fs::create_directories(bundleDir);
    fs::path configPath = bundleDir / (BACKUP_INCEXC_SYMBOL + std::to_string(backupTime));
    {
        std::ofstream configFile(configPath);
        ASSERT_TRUE(configFile.is_open());
        configFile << BACKUP_INCLUDE << '\n';
        configFile << MEDIA_SAND_PREFIX + "/coverage_missing" << '\n';
        configFile << BACKUP_EXCLUDE << '\n';
        configFile << "storage/media/coverage_missing/exclude" << '\n';
    }

    std::vector<int64_t> pkgFileSizes;
    std::vector<int64_t> incPkgFileSizes;
    StorageManagerService::GetInstance().GetBundleStatsForIncreaseEach(userId, bundleName, backupTime,
        pkgFileSizes, incPkgFileSizes);
    ASSERT_EQ(pkgFileSizes.size(), 1);
    ASSERT_EQ(incPkgFileSizes.size(), 1);
    EXPECT_EQ(pkgFileSizes[0], 0);
    EXPECT_EQ(incPkgFileSizes[0], 0);
    EXPECT_TRUE(fs::exists(bundleDir / (BACKUP_STAT_SYMBOL + std::to_string(backupTime))));
    fs::remove_all(bundleDir);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_SetExcludePathMap_001
 * @tc.desc: cover empty, missing, regular-file and directory exclude paths
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_SetExcludePathMap_001,
    testing::ext::TestSize.Level1)
{
    auto &service = StorageManagerService::GetInstance();
    fs::path root = fs::temp_directory_path() / ("backup_exclude_" + std::to_string(getpid()));
    fs::path dirPath = root / "dir";
    fs::path filePath = root / "file";
    fs::create_directories(dirPath);
    {
        std::ofstream file(filePath);
        file << "data";
    }
    std::map<std::string, bool> excludesMap;
    std::string emptyPath;
    service.SetExcludePathMap(emptyPath, excludesMap);
    std::string missingPath = (root / "missing").string();
    service.SetExcludePathMap(missingPath, excludesMap);
    std::string file = filePath.string();
    service.SetExcludePathMap(file, excludesMap);
    std::string dir = dirPath.string();
    service.SetExcludePathMap(dir, excludesMap);
    ASSERT_EQ(excludesMap.size(), 2);
    auto fileIt = excludesMap.find(file);
    ASSERT_NE(fileIt, excludesMap.end());
    EXPECT_FALSE(fileIt->second);
    EXPECT_EQ(dir, dirPath.string() + "/");
    auto dirIt = excludesMap.find(dir);
    ASSERT_NE(dirIt, excludesMap.end());
    EXPECT_TRUE(dirIt->second);

    std::map<std::string, bool> trailingSlashMap;
    std::string dirWithSlash = dirPath.string() + "/";
    service.SetExcludePathMap(dirWithSlash, trailingSlashMap);
    EXPECT_EQ(dirWithSlash, dirPath.string() + "/");
    ASSERT_EQ(trailingSlashMap.size(), 1);
    auto trailingSlashIt = trailingSlashMap.find(dirWithSlash);
    ASSERT_NE(trailingSlashIt, trailingSlashMap.end());
    EXPECT_TRUE(trailingSlashIt->second);
    fs::remove_all(root);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_ExcludeFilter_003
 * @tc.desc: cover exact file exclusion, directory exclusion and non-matches
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ExcludeFilter_003,
    testing::ext::TestSize.Level1)
{
    auto &service = StorageManagerService::GetInstance();
    std::map<std::string, bool> excludesMap = {
        {"/tmp/excluded_file", false},
        {"/tmp/excluded_dir/", true},
    };
    EXPECT_TRUE(service.ExcludeFilter(excludesMap, "/tmp/excluded_file"));
    EXPECT_TRUE(service.ExcludeFilter(excludesMap, "/tmp/excluded_dir/child"));
    EXPECT_FALSE(service.ExcludeFilter(excludesMap, "/tmp/excluded_file_suffix"));
    EXPECT_FALSE(service.ExcludeFilter(excludesMap, "/tmp/other"));
}

/**
 * @tc.name: Storage_Manager_ServiceTest_CheckIfDirForIncludes_004
 * @tc.desc: cover missing paths, symbolic links, directories and regular files
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_CheckIfDirForIncludes_004,
    testing::ext::TestSize.Level1)
{
    auto &service = StorageManagerService::GetInstance();
    fs::path root = fs::temp_directory_path() / ("backup_check_include_" + std::to_string(getpid()));
    fs::path filePath = root / "file";
    fs::path linkPath = root / "link";
    fs::path statPath = root / "stat";
    fs::create_directories(root);
    {
        std::ofstream file(filePath);
        file << "content";
    }
    ASSERT_EQ(symlink(filePath.c_str(), linkPath.c_str()), 0);
    std::ofstream statFile(statPath);
    ASSERT_TRUE(statFile.is_open());
    std::string bundleName = "bundle";
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
        .lastBackupTime = std::numeric_limits<int64_t>::max(), .fileSizeSum = 0, .incFileSizeSum = 0};
    std::map<std::string, std::string> pathMap = {{filePath.string(), "/sandbox/file"}};
    std::map<std::string, bool> excludesMap;

    EXPECT_EQ(service.CheckIfDirForIncludes((root / "missing").string(), paras, pathMap, statFile, excludesMap),
        std::make_tuple(false, false));
    EXPECT_EQ(service.CheckIfDirForIncludes(linkPath.string(), paras, pathMap, statFile, excludesMap),
        std::make_tuple(false, false));
    EXPECT_EQ(service.CheckIfDirForIncludes(root.string(), paras, pathMap, statFile, excludesMap),
        std::make_tuple(true, true));
    EXPECT_EQ(service.CheckIfDirForIncludes(filePath.string(), paras, pathMap, statFile, excludesMap),
        std::make_tuple(true, false));
    EXPECT_GT(paras.fileSizeSum, 0);
    EXPECT_EQ(paras.incFileSizeSum, 0);
    statFile.close();
    fs::remove_all(root);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetIncludesFileStats_001
 * @tc.desc: cover recursive scanning of files, directories and symbolic links
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetIncludesFileStats_001,
    testing::ext::TestSize.Level1)
{
    auto &service = StorageManagerService::GetInstance();
    fs::path root = fs::temp_directory_path() / ("backup_file_stats_" + std::to_string(getpid()));
    fs::path subDir = root / "sub";
    fs::path filePath = root / "file";
    fs::path nestedFile = subDir / "nested";
    fs::path linkPath = root / "link";
    fs::path statPath = fs::temp_directory_path() / ("backup_file_stats_result_" + std::to_string(getpid()));
    fs::create_directories(subDir);
    {
        std::ofstream file(filePath);
        file << "root-file";
        std::ofstream nested(nestedFile);
        nested << "nested-file";
    }
    ASSERT_EQ(symlink(filePath.c_str(), linkPath.c_str()), 0);
    std::ofstream statFile(statPath);
    ASSERT_TRUE(statFile.is_open());
    std::string bundleName = "bundle";
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
        .lastBackupTime = std::numeric_limits<int64_t>::max(), .fileSizeSum = 0, .incFileSizeSum = 0};
    std::map<std::string, std::string> pathMap = {{root.string(), "/sandbox/root"}};
    std::map<std::string, bool> excludesMap;
    EXPECT_TRUE(service.GetIncludesFileStats(root.string(), paras, pathMap, statFile, excludesMap));
    EXPECT_GT(paras.fileSizeSum, 0);
    EXPECT_EQ(paras.incFileSizeSum, 0);
    statFile.close();

    std::ifstream resultFile(statPath);
    std::string result((std::istreambuf_iterator<char>(resultFile)), std::istreambuf_iterator<char>());
    EXPECT_NE(result.find("/sandbox/root/file"), std::string::npos);
    EXPECT_NE(result.find("/sandbox/root/sub/nested"), std::string::npos);
    EXPECT_EQ(result.find("/sandbox/root/link"), std::string::npos);
    fs::remove_all(root);
    fs::remove(statPath);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetPathWildCard_001
 * @tc.desc: cover invalid and valid wildcard directory expansion
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetPathWildCard_001,
    testing::ext::TestSize.Level1)
{
    auto &service = StorageManagerService::GetInstance();
    std::vector<std::string> includePaths;
    std::map<std::string, std::string> pathMap;
    EXPECT_FALSE(service.GetPathWildCard(100, "bundle", "/tmp/no_wildcard", includePaths, pathMap));
    EXPECT_FALSE(service.GetPathWildCard(100, "bundle", "/tmp/not_exist_for_backup/*", includePaths, pathMap));

    fs::path root = fs::temp_directory_path() / ("backup_wildcard_" + std::to_string(getpid()));
    fs::path hapDir = root / "hap";
    fs::create_directories(hapDir / DEFAULT_INCLUDE_PATH_IN_HAP_FILES);
    fs::create_directories(hapDir / DEFAULT_INCLUDE_PATH_IN_HAP_DATABASE);
    fs::create_directories(hapDir / DEFAULT_INCLUDE_PATH_IN_HAP_PREFERENCE);
    fs::create_directories(hapDir / "other");
    {
        std::ofstream nonDir(root / "plain_file");
        nonDir << "data";
    }
    EXPECT_TRUE(service.GetPathWildCard(100, "bundle", root.string() + "/*", includePaths, pathMap));
    EXPECT_EQ(includePaths.size(), 3);
    fs::remove_all(root);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_WriteFileList_002
 * @tc.desc: cover invalid output, plain paths, directories and non-incremental files
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_WriteFileList_002,
    testing::ext::TestSize.Level1)
{
    auto &service = StorageManagerService::GetInstance();
    std::string bundleName = "bundle";
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
        .lastBackupTime = 1, .fileSizeSum = 0, .incFileSizeSum = 0};
    FileStat fileStat = {.filePath = "/plain/file", .fileSize = 8, .lastUpdateTime = 1,
        .mode = 0644, .isDir = false, .isIncre = false};
    std::ofstream closedFile;
    service.WriteFileList(closedFile, fileStat, paras);

    fs::path statPath = fs::temp_directory_path() / ("backup_write_list_" + std::to_string(getpid()));
    std::ofstream statFile(statPath);
    ASSERT_TRUE(statFile.is_open());
    FileStat emptyPath = fileStat;
    emptyPath.filePath.clear();
    service.WriteFileList(statFile, emptyPath, paras);
    service.WriteFileList(statFile, fileStat, paras);
    FileStat dirStat = {.filePath = "/plain/dir", .fileSize = 4, .lastUpdateTime = 2,
        .mode = 0755, .isDir = true, .isIncre = true};
    service.WriteFileList(statFile, dirStat, paras);
    statFile.close();
    EXPECT_EQ(paras.fileSizeSum, 12);
    EXPECT_EQ(paras.incFileSizeSum, 4);

    std::ifstream resultFile(statPath);
    std::string firstLine;
    std::string secondLine;
    std::getline(resultFile, firstLine);
    std::getline(resultFile, secondLine);
    EXPECT_NE(firstLine.find("/plain/file;"), std::string::npos);
    EXPECT_NE(firstLine.rfind(";0"), std::string::npos);
    EXPECT_NE(secondLine.find("/plain/dir;"), std::string::npos);
    fs::remove(statPath);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_AddOuterDirIntoFileStat_002
 * @tc.desc: cover invalid, missing, included and excluded outer directories
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_AddOuterDirIntoFileStat_002,
    testing::ext::TestSize.Level1)
{
    auto &service = StorageManagerService::GetInstance();
    fs::path root = fs::temp_directory_path() / ("backup_outer_dir_" + std::to_string(getpid()));
    fs::create_directories(root);
    fs::path statPath = root / "stat";
    std::string bundleName = "bundle";
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
        .lastBackupTime = 0, .fileSizeSum = 0, .incFileSizeSum = 0};
    std::map<std::string, bool> excludesMap;
    std::ofstream closedFile;
    EXPECT_FALSE(service.AddOuterDirIntoFileStat(root.string(), paras, "/sandbox", closedFile, excludesMap));

    std::ofstream statFile(statPath);
    ASSERT_TRUE(statFile.is_open());
    EXPECT_FALSE(service.AddOuterDirIntoFileStat((root / "missing").string(), paras,
        "/sandbox", statFile, excludesMap));
    EXPECT_TRUE(service.AddOuterDirIntoFileStat(root.string(), paras, "/sandbox", statFile, excludesMap));
    EXPECT_GT(paras.fileSizeSum, 0);

    int64_t previousSize = paras.fileSizeSum;
    std::map<std::string, bool> excluded = {{root.string() + "/", true}};
    EXPECT_TRUE(service.AddOuterDirIntoFileStat(root.string() + "/", paras, "/sandbox", statFile, excluded));
    EXPECT_EQ(paras.fileSizeSum, previousSize);
    statFile.close();
    fs::remove_all(root);
}
}
