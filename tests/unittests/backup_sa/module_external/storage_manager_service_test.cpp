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
#include "b_jsonutil_mock.h"
#include "b_sa_utils_mock.h"
#include "if_system_ability_manager.h"
#include "iremote_stub.h"
#include "iservice_registry.h"
#include "module_external/bms_adapter.h"
#include "module_external/storage_manager_service.h"
#include "module_external/storage_manager_service.cpp"
#include "datashare_helper_mock.h"
#include "datashare_result_set_mock.h"
#include "directory_ex.h"

namespace OHOS {
const std::string CAMERA_BUNDLENAME = "file";
const std::string CALENDAR_BUNDLENAME = "com.huawei.hmos.calendar";
using namespace std;
using namespace testing;
using namespace FileManagement::Backup;
namespace fs = std::filesystem;

class SystemAbilityManagerMock : public ISystemAbilityManager {
public:
    MOCK_METHOD((sptr<IRemoteObject>), AsObject, ());
    MOCK_METHOD((std::vector<std::u16string>), ListSystemAbilities, (unsigned int));
    MOCK_METHOD((sptr<IRemoteObject>), GetSystemAbility, (int32_t));
    MOCK_METHOD((sptr<IRemoteObject>), CheckSystemAbility, (int32_t));
    MOCK_METHOD(int32_t, RemoveSystemAbility, (int32_t));
    MOCK_METHOD(int32_t, SubscribeSystemAbility, (int32_t, (const sptr<ISystemAbilityStatusChange>&)));
    MOCK_METHOD(int32_t, UnSubscribeSystemAbility, (int32_t, (const sptr<ISystemAbilityStatusChange>&)));
    MOCK_METHOD((sptr<IRemoteObject>), GetSystemAbility, (int32_t, const std::string&));
    MOCK_METHOD(sptr<IRemoteObject>, CheckSystemAbility, (int32_t, const std::string&));
    MOCK_METHOD(int32_t, AddOnDemandSystemAbilityInfo, (int32_t, const std::u16string&));
    MOCK_METHOD((sptr<IRemoteObject>), CheckSystemAbility, (int32_t, bool&));
    MOCK_METHOD(int32_t, AddSystemAbility, (int32_t, (const sptr<IRemoteObject>&), const SAExtraProp&));
    MOCK_METHOD(int32_t, AddSystemProcess, (const std::u16string&, (const sptr<IRemoteObject>&)));
    MOCK_METHOD((sptr<IRemoteObject>), LoadSystemAbility, (int32_t, int32_t));
    MOCK_METHOD(int32_t, LoadSystemAbility, (int32_t, (const sptr<ISystemAbilityLoadCallback>&)));
    MOCK_METHOD(int32_t, LoadSystemAbility, (int32_t, const std::string&, (const sptr<ISystemAbilityLoadCallback>&)));
    MOCK_METHOD(int32_t, UnloadSystemAbility, (int32_t));
    MOCK_METHOD(int32_t, CancelUnloadSystemAbility, (int32_t));
    MOCK_METHOD(int32_t, UnloadAllIdleSystemAbility, ());
    MOCK_METHOD(int32_t, GetSystemProcessInfo, (int32_t, SystemProcessInfo&));
    MOCK_METHOD(int32_t, GetRunningSystemProcess, (std::list<SystemProcessInfo>&));
    MOCK_METHOD(int32_t, SubscribeSystemProcess, (const sptr<ISystemProcessStatusChange>&));
    MOCK_METHOD(int32_t, SendStrategy, (int32_t, (std::vector<int32_t>&), int32_t, std::string&));
    MOCK_METHOD(int32_t, UnSubscribeSystemProcess, (const sptr<ISystemProcessStatusChange>&));
    MOCK_METHOD(int32_t, GetOnDemandReasonExtraData, (int64_t, MessageParcel&));
    MOCK_METHOD(int32_t, GetOnDemandPolicy, (int32_t, OnDemandPolicyType, (std::vector<SystemAbilityOnDemandEvent>&)));
    MOCK_METHOD(int32_t, UpdateOnDemandPolicy, (int32_t, OnDemandPolicyType,
        (const std::vector<SystemAbilityOnDemandEvent>&)));
    MOCK_METHOD(int32_t, GetOnDemandSystemAbilityIds, (std::vector<int32_t>&));
    MOCK_METHOD(int32_t, GetExtensionSaIds, (const std::string&, std::vector<int32_t>&));
    MOCK_METHOD(int32_t, GetExtensionRunningSaList, (const std::string&, (std::vector<sptr<IRemoteObject>>&)));
    MOCK_METHOD(int32_t, GetRunningSaExtensionInfoList, (const std::string&, (std::vector<SaExtensionInfo>&)));
    MOCK_METHOD(int32_t, GetCommonEventExtraDataIdlist, (int32_t, (std::vector<int64_t>&), const std::string&));
};

class BundleMgrMock : public IRemoteStub<AppExecFwk::IBundleMgr> {
public:
    MOCK_METHOD((sptr<IRemoteObject>), AsObject, ());
    MOCK_METHOD(bool, GetBundleInfos, (const AppExecFwk::BundleFlag, (std::vector<AppExecFwk::BundleInfo>&), int32_t));
    MOCK_METHOD(ErrCode, GetCloneBundleInfo, (const std::string&, int32_t, int32_t, AppExecFwk::BundleInfo&, int32_t));
};

class StorageManagerServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline sptr<BundleMgrMock> bms = nullptr;
    static inline sptr<SystemAbilityManagerMock> sam = nullptr;
    static inline shared_ptr<DataShareHelperMock> dsh = nullptr;
    static inline shared_ptr<DataShareResultSetMock> dsrs = nullptr;
    static inline shared_ptr<DataShareHelperImpl> dhelper = nullptr;
    static inline shared_ptr<DataShare::DataShareResultSet> dset = nullptr;
};

void StorageManagerServiceTest::SetUpTestCase(void) {
    bms = sptr<BundleMgrMock>(new BundleMgrMock());
    sam = sptr<SystemAbilityManagerMock>(new SystemAbilityManagerMock());
    dsh = make_shared<DataShareHelperMock>();
    DataShareHelperMock::idsh = dsh;
    dsrs = make_shared<DataShareResultMock>();
    DataShareResultMock::idrs = dsrs;
    dhelper = make_shared<DataShareHelperImpl>();
    dset = std::make_shared<DataShare::DataShareResultSet>();
}

void StorageManagerServiceTest::TearDownTestCase(void) 
{
    bms = nullptr;
    sam = nullptr;
}

void StorageManagerServiceTest::SetUp(void) {}
void StorageManagerServiceTest::TearDown(void) {}

sptr<ISystemAbilityManager> SystemAbilityManagerClient::GetSystemAbilityManager()
{
    return StorageManagerServiceTest::sam;
}

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
    std::vector<std::string> bundleNames = {"com.example.app1", "com.huawei.hmos.calendar"};
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
    std::vector<std::string> bundleNames = {"com.example.app1", "com.huawei.hmos.calendar"};
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
    std::string bundleName = CALENDAR_BUNDLENAME;
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
    std::string bundleName = CALENDAR_BUNDLENAME;
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
    std::string bundleName = CALENDAR_BUNDLENAME;
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
    std::string bundleName = CALENDAR_BUNDLENAME;
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
    std::string bundleName = CALENDAR_BUNDLENAME;
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
    std::string bundleName = CALENDAR_BUNDLENAME;
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
    std::string bundleName = CALENDAR_BUNDLENAME;
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
    std::string bundleName = CALENDAR_BUNDLENAME;
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
    std::string bundleName = CALENDAR_BUNDLENAME;
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
    int_32 userId = 100;
    string basePath = "data/storage/el2/" + to_string(useId);
    string path = basePath + "/base"
    EXPECT_TRUE(OHOS::ForceCreateDirectory(path));

    StorageManager::StorageStats storageStats;
    std::string type = MEDIA_TYPE;
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillRepeatedly(Return(bms));
    EXPECT_CALL(*dsh, Creator(_, _, _, _, _)).WillRepeatedly(Return(dhelper));
    EXPECT_CALL(*dhelper, Query(_, _, _, _)).WillOnce(Return(dset));
    EXPECT_CALL(*dsrs, GetRowCount(_)).WillOnce(SetArgReferee<0>(100), Return(0));
    int64_t result = StorageManagerService::GetInstance().GetUserStorageStatsByType(userId, storageStats, type);
    EXPECT_EQ(result, E_GETROWCOUNT);   
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
}

<<<<<<< HEAD
<<<<<<< HEAD
=======
// /**
//  * @tc.name: Storage_Manager_ServiceTest_GetUserStorageStatsByType_003
//  * @tc.desc: check the GetUserStorageStatsByType function
//  * @tc.type: FUNC
//  */
// HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetUserStorageStatsByType_003,
//     testing::ext::TestSize.Level1) {
//     StorageManager::StorageStats storageStats;
//     sam = nullptr;
//     int64_t result = StorageManagerService::GetInstance().GetMediaStorageStats(storageStats);
//     sanm = ;
//     EXPECT_EQ(result, E_ERR);
// }

>>>>>>> df18e2da (add ut for storage manager service)
=======
>>>>>>> f9bba3bd (add ut)
/**
 * @tc.name: Storage_Manager_ServiceTest_GetMediaStorageStats_001
 * @tc.number: GetMediaStorageStats_001
 * @tc.desc: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetMediaStorageStats_001,
    testing::ext::TestSize.Level1) {
    StorageManager::StorageStats storageStats;

    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(nullptr)).WillRepeatedly(Return(bms));
    int32_t result = StorageManagerService::GetInstance().GetMediaStorageStats(storageStats);
    EXPECT_EQ(result, E_REMOTE_IS_NULLPTR);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetMediaStorageStats_002
 * @tc.number: GetMediaStorageStats_002
 * @tc.desc: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetMediaStorageStats_002,
    testing::ext::TestSize.Level1) {
    StorageManager::StorageStats storageStats;
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(nullptr)).WillRepeatedly(Return(bms));
    int32_t result = StorageManagerService::GetInstance().GetMediaStorageStats(storageStats);
    EXPECT_EQ(result, E_MEDIALIBRARY_ERROR);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_GetMediaStorageStats_002
 * @tc.number: GetMediaStorageStats_002
 * @tc.desc: FUNC
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetMediaStorageStats_003,
    testing::ext::TestSize.Level1) {
    StorageManager::StorageStats storageStats;
    EXPECT_CALL(*dsh, Creator(_, _, _, _, _)).WillRepeatedly(Return(dhelper));
    EXPECT_CALL(*dhelper, Query(_, _, _, _)).WillOnce(Return(nullptr));
    int32_t result = StorageManagerService::GetInstance().GetMediaStorageStats(storageStats);
    EXPECT_EQ(result, E_QUERY);
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
    EXPECT_CALL(*dsrs, GoToNextRow(_, _, _, _)).WillRepeatedly(Return(true));
    StorageManagerService::GetInstance().GetMediaTypeAndSize(resultSet, storageStats);
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

/**
 * @tc.name: Storage_Manager_ServiceTest_GetBundleStatsTest_001
 * @tc.number: GetBundleStatsTest_001
 * @tc.desc: 测试 GetBundleInfosForLocalCapabilities 接口
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000IGCR7
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_GetBundleStats_001, testing::ext::TestSize.Level1)
{
    string bundleName = CALENDAR_BUNDLENAME;
    StorageManager::StorageStats storageStats;
    
    bool result = StorageManagerService::GetInstance().GetBundleStats(bundleName, storageStats);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_DeduplicationPath_001
 * @tc.desc: DeduplicationPathTest_001
 * @tc.desc: 测试configPath为空时，不执行任何操作
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_DeduplicationPath_001,
    testing::ext::TestSize.Level1)
{
    std::vector<std::string> configPaths;
    StorageManagerService::GetInstance().DeduplicationPath(configPaths);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_DeduplicationPath_002
 * @tc.desc: DeduplicationPathTest_002
 * @tc.desc: 测试configPath不为空时，执行去重
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_DeduplicationPath_002,
    testing::ext::TestSize.Level1)
{
    std::vector<std::string> configPaths = {"path1", "path2", "path3"};
    StorageManagerService::GetInstance().DeduplicationPath(configPaths);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Storage_Manager_ServiceTest_ScanExtensionPath_001
 * @tc.desc: ScanExtensionPathTest_001
 * @tc.desc: ScanExtensionPath 有效路径正确扫描
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_ScanExtensionPath_001,
    testing::ext::TestSize.Level1)
{
    std::string bundleName = CALENDAR_BUNDLENAME;
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 123456789, .fileSizeSum = 0, .incFileSizeSum = 0};
    std::vector<std::string> includes = {"path/to/include"};
    std::vector<std::string> excludes = {"path/to/exclude"};
    std::map<std::string, std::string> pathMap;
    std::ofstream statFile("statfile.txt");

    StorageManagerService::GetInstance().ScanExtensionPath(paras, includes, excludes, pathMap, statFile);
    EXPECT_TRUE(pathMap.empty());
    EXPECT_TRUE(statFile.good());

    statFile.close();
    remove(statfile.txt);

}

/**
 * @tc.name: Storage_Manager_ServiceTest_AddOuterDirIntoFileStat_001
 * @tc.desc: AddOuterDirIntoFileStat_001
 * @tc.desc: AddOuterDirIntoFileStat 调用时正常返回
 */
HWTEST_F(StorageManagerServiceTest, Storage_Manager_ServiceTest_AddOuterDirIntoFileStat_001,
    testing::ext::TestSize.Level1)
{
    std::string bundleName = CALENDAR_BUNDLENAME;
    std::string dir = "/data/app/el1/100/base/" + bundleName +"/.backup"
    BundleStatsParas paras = {.userId = 100, .bundleName = bundleName,
                            .lastBackupTime = 123456789, .fileSizeSum = 0, .incFileSizeSum = 0};
    std::vector<std::string> sandboxDir = {"path/to/sandboxDir"};
    std::map<std::string, std::string> pathMap;
    std::ofstream statFile("statfile.txt");

    StorageManagerService::GetInstance().AddOuterDirIntoFileStat(dir, paras, sandboxDir, statFile, excludesMap);
    EXPECT_TRUE(true);

    dir = ""
    StorageManagerService::GetInstance().AddOuterDirIntoFileStat(dir, paras, sandboxDir, statFile, excludesMap);
    EXPECT_TRUE(true);

    statFile.close();
    remove(statfile.txt);
}
}