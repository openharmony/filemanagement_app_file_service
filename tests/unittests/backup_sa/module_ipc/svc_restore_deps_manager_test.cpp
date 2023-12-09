/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#define private public
#include "module_ipc/svc_restore_deps_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app";
} // namespace

class SvcRestoreDepsManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

static void ClearCache()
{
    SvcRestoreDepsManager::GetInstance().depsMap_.clear();
    SvcRestoreDepsManager::GetInstance().allBundles_.clear();
    SvcRestoreDepsManager::GetInstance().toRestoreBundleMap_.clear();
    SvcRestoreDepsManager::GetInstance().restoredBundles_.clear();
}

static bool IsEqual(const vector<string> &lf, const vector<string> &rh)
{
    if (lf.size() != rh.size()) {
        return false;
    }
    for (auto i = 0; i < lf.size(); ++i) {
        if (lf.at(i) != rh.at(i)) {
            return false;
        }
    }
    return true;
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0100
 * @tc.name: SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0100
 * @tc.desc: 测试 GetRestoreBundleNames 获取应用恢复列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0100";
    vector<BJsonEntityCaps::BundleInfo> bundleInfos1 {};
    RestoreTypeEnum restoreType = RESTORE_DATA_WAIT_SEND;
    auto bundleNames = SvcRestoreDepsManager::GetInstance().GetRestoreBundleNames(bundleInfos1, restoreType);
    EXPECT_EQ(bundleNames.size(), 0);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0100";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0200
 * @tc.name: SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0200
 * @tc.desc: 测试 GetRestoreBundleNames 获取应用恢复列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0200";
    BJsonEntityCaps::BundleInfo info1 = {.name = "com.example.app1",
                                         .versionCode = 1000000,
                                         .versionName = "1.0.0",
                                         .spaceOccupied = 0,
                                         .allToBackup = false,
                                         .extensionName = "",
                                         .needToInstall = false,
                                         .restoreDeps = ""};
    BJsonEntityCaps::BundleInfo info2 = {.name = "com.example.app2",
                                         .versionCode = 1000000,
                                         .versionName = "1.0.0",
                                         .spaceOccupied = 0,
                                         .allToBackup = false,
                                         .extensionName = "",
                                         .needToInstall = false,
                                         .restoreDeps = "com.example.app1"};
    vector<BJsonEntityCaps::BundleInfo> bundleInfos {info1, info2};
    RestoreTypeEnum restoreType = RESTORE_DATA_WAIT_SEND;
    auto bundleNames = SvcRestoreDepsManager::GetInstance().GetRestoreBundleNames(bundleInfos, restoreType);
    EXPECT_EQ(bundleNames.size(), 1);
    EXPECT_FALSE(SvcRestoreDepsManager::GetInstance().IsAllBundlesRestored());
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0200";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0300
 * @tc.name: SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0300
 * @tc.desc: 测试 GetRestoreBundleNames 获取应用恢复列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0300";
    BJsonEntityCaps::BundleInfo info = {.name = BUNDLE_NAME,
                                        .versionCode = 1000000,
                                        .versionName = "1.0.0",
                                        .spaceOccupied = 0,
                                        .allToBackup = false,
                                        .extensionName = "",
                                        .needToInstall = false,
                                        .restoreDeps = "com.example.app1"};
    vector<BJsonEntityCaps::BundleInfo> bundleInfos {info};
    RestoreTypeEnum restoreType = RESTORE_DATA_WAIT_SEND;
    vector<string> depList = {};
    SvcRestoreDepsManager::GetInstance().depsMap_.emplace(make_pair(BUNDLE_NAME, depList));
    SvcRestoreDepsManager::GetInstance().restoredBundles_.insert("com.example.app1");
    auto bundleNames = SvcRestoreDepsManager::GetInstance().GetRestoreBundleNames(bundleInfos, restoreType);
    EXPECT_EQ(bundleNames.size(), 1);
    EXPECT_TRUE(SvcRestoreDepsManager::GetInstance().IsAllBundlesRestored());
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_GetRestoreBundleNames_0300";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0100
 * @tc.name: SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0100
 * @tc.desc: 测试 GetRestoreBundleMap 需要恢复的应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0100";
    auto map = SvcRestoreDepsManager::GetInstance().GetRestoreBundleMap();
    EXPECT_EQ(map.size(), 0);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0100";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0200
 * @tc.name: SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0200
 * @tc.desc: 测试 GetRestoreBundleMap 需要恢复的应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0200";
    SvcRestoreDepsManager::RestoreInfo info = {.restoreType_ = RESTORE_DATA_WAIT_SEND, .fileNames_ = {""}};
    SvcRestoreDepsManager::GetInstance().toRestoreBundleMap_.emplace(make_pair(BUNDLE_NAME, info));
    vector<string> depList = {};
    SvcRestoreDepsManager::GetInstance().depsMap_.emplace(make_pair(BUNDLE_NAME, depList));
    auto map = SvcRestoreDepsManager::GetInstance().GetRestoreBundleMap();
    EXPECT_EQ(map.size(), 1);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0200";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0300
 * @tc.name: SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0300
 * @tc.desc: 测试 GetRestoreBundleMap 需要恢复的应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0300";
    SvcRestoreDepsManager::RestoreInfo info = {.restoreType_ = RESTORE_DATA_WAIT_SEND, .fileNames_ = {""}};
    SvcRestoreDepsManager::GetInstance().toRestoreBundleMap_.emplace(make_pair(BUNDLE_NAME, info));
    vector<string> depList = {"com.example.app1"};
    SvcRestoreDepsManager::GetInstance().depsMap_.emplace(make_pair(BUNDLE_NAME, depList));
    auto map = SvcRestoreDepsManager::GetInstance().GetRestoreBundleMap();
    EXPECT_EQ(map.size(), 0);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_GetRestoreBundleMap_0300";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_AddRestoredBundles_0100
 * @tc.name: SUB_SvcRestoreDepsManager_AddRestoredBundles_0100
 * @tc.desc: 测试 AddRestoredBundles 追加已经恢复完成的应用接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_AddRestoredBundles_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_AddRestoredBundles_0100";
    SvcRestoreDepsManager::GetInstance().AddRestoredBundles("");
    EXPECT_EQ(SvcRestoreDepsManager::GetInstance().restoredBundles_.size(), 1);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_AddRestoredBundles_0100";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_AddRestoredBundles_0200
 * @tc.name: SUB_SvcRestoreDepsManager_AddRestoredBundles_0200
 * @tc.desc: 测试 AddRestoredBundles 追加已经恢复完成的应用接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_AddRestoredBundles_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_AddRestoredBundles_0200";
    SvcRestoreDepsManager::GetInstance().AddRestoredBundles(BUNDLE_NAME);
    EXPECT_EQ(SvcRestoreDepsManager::GetInstance().restoredBundles_.size(), 1);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_AddRestoredBundles_0200";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_GetAllBundles_0100
 * @tc.name: SUB_SvcRestoreDepsManager_GetAllBundles_0100
 * @tc.desc: 测试 GetAllBundles 追加已经恢复完成的应用接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_GetAllBundles_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_GetAllBundles_0100";
    auto allBundles = SvcRestoreDepsManager::GetInstance().GetAllBundles();
    EXPECT_EQ(allBundles.size(), 0);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_GetAllBundles_0100";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_GetAllBundles_0200
 * @tc.name: SUB_SvcRestoreDepsManager_GetAllBundles_0200
 * @tc.desc: 测试 GetAllBundles 追加已经恢复完成的应用接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_GetAllBundles_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_GetAllBundles_0200";
    BJsonEntityCaps::BundleInfo info = {.name = BUNDLE_NAME,
                                        .versionCode = 1000000,
                                        .versionName = "1.0.0",
                                        .spaceOccupied = 0,
                                        .allToBackup = false,
                                        .extensionName = "",
                                        .needToInstall = false,
                                        .restoreDeps = ""};
    SvcRestoreDepsManager::GetInstance().allBundles_.emplace_back(info);
    auto allBundles = SvcRestoreDepsManager::GetInstance().GetAllBundles();
    EXPECT_EQ(allBundles.size(), 1);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_GetAllBundles_0200";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_IsAllBundlesRestored_0100
 * @tc.name: SUB_SvcRestoreDepsManager_IsAllBundlesRestored_0100
 * @tc.desc: 测试 IsAllBundlesRestored 获取所有应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_IsAllBundlesRestored_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_IsAllBundlesRestored_0100";
    bool ret = SvcRestoreDepsManager::GetInstance().IsAllBundlesRestored();
    EXPECT_TRUE(ret);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_IsAllBundlesRestored_0100";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_IsAllBundlesRestored_0200
 * @tc.name: SUB_SvcRestoreDepsManager_IsAllBundlesRestored_0200
 * @tc.desc: 测试 IsAllBundlesRestored 获取所有应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_IsAllBundlesRestored_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_IsAllBundlesRestored_0200";
    SvcRestoreDepsManager::RestoreInfo info = {.restoreType_ = RESTORE_DATA_WAIT_SEND, {""}};
    SvcRestoreDepsManager::GetInstance().toRestoreBundleMap_.emplace(make_pair(BUNDLE_NAME, info));
    bool ret = SvcRestoreDepsManager::GetInstance().IsAllBundlesRestored();
    EXPECT_FALSE(ret);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_IsAllBundlesRestored_0200";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_UpdateToRestoreBundleMap_0100
 * @tc.name: SUB_SvcRestoreDepsManager_UpdateToRestoreBundleMap_0100
 * @tc.desc: 测试 UpdateToRestoreBundleMap 更新需要恢复的应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest,
         SUB_SvcRestoreDepsManager_UpdateToRestoreBundleMap_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_UpdateToRestoreBundleMap_0100";
    SvcRestoreDepsManager::GetInstance().UpdateToRestoreBundleMap("", "");
    EXPECT_TRUE(SvcRestoreDepsManager::GetInstance().IsAllBundlesRestored());
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_UpdateToRestoreBundleMap_0100";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_UpdateToRestoreBundleMap_0200
 * @tc.name: SUB_SvcRestoreDepsManager_UpdateToRestoreBundleMap_0200
 * @tc.desc: 测试 UpdateToRestoreBundleMap 更新需要恢复的应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest,
         SUB_SvcRestoreDepsManager_UpdateToRestoreBundleMap_0200,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_UpdateToRestoreBundleMap_0200";
    SvcRestoreDepsManager::RestoreInfo info = {.restoreType_ = RESTORE_DATA_WAIT_SEND, {""}};
    SvcRestoreDepsManager::GetInstance().toRestoreBundleMap_.emplace(make_pair(BUNDLE_NAME, info));
    SvcRestoreDepsManager::GetInstance().UpdateToRestoreBundleMap(BUNDLE_NAME, "1.txt");
    auto it = SvcRestoreDepsManager::GetInstance().toRestoreBundleMap_.find(BUNDLE_NAME);
    bool ret = (it != SvcRestoreDepsManager::GetInstance().toRestoreBundleMap_.end());
    EXPECT_TRUE(ret);
    if (ret) {
        EXPECT_EQ(it->second.fileNames_.size(), 2);
    }
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_UpdateToRestoreBundleMap_0200";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_BuildDepsMap_0100
 * @tc.name: SUB_SvcRestoreDepsManager_BuildDepsMap_0100
 * @tc.desc: 测试 BuildDepsMap 更新需要恢复的应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_BuildDepsMap_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_BuildDepsMap_0100";
    vector<BJsonEntityCaps::BundleInfo> bundleInfos {};
    SvcRestoreDepsManager::GetInstance().BuildDepsMap(bundleInfos);
    EXPECT_EQ(SvcRestoreDepsManager::GetInstance().allBundles_.size(), 0);
    EXPECT_EQ(SvcRestoreDepsManager::GetInstance().depsMap_.size(), 0);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_BuildDepsMap_0100";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_BuildDepsMap_0200
 * @tc.name: SUB_SvcRestoreDepsManager_BuildDepsMap_0200
 * @tc.desc: 测试 BuildDepsMap 更新需要恢复的应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_BuildDepsMap_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_BuildDepsMap_0200";
    BJsonEntityCaps::BundleInfo info1 = {.name = "com.example.app1",
                                         .versionCode = 1000000,
                                         .versionName = "1.0.0",
                                         .spaceOccupied = 0,
                                         .allToBackup = false,
                                         .extensionName = "",
                                         .needToInstall = false,
                                         .restoreDeps = ""};
    BJsonEntityCaps::BundleInfo info2 = {.name = "com.example.app2",
                                         .versionCode = 1000000,
                                         .versionName = "1.0.0",
                                         .spaceOccupied = 0,
                                         .allToBackup = false,
                                         .extensionName = "",
                                         .needToInstall = false,
                                         .restoreDeps = "com.example.app1"};
    BJsonEntityCaps::BundleInfo info3 = {.name = "com.example.app3",
                                         .versionCode = 1000000,
                                         .versionName = "1.0.0",
                                         .spaceOccupied = 0,
                                         .allToBackup = false,
                                         .extensionName = "",
                                         .needToInstall = false,
                                         .restoreDeps = "com.example.app1,com.example.app2"};
    vector<BJsonEntityCaps::BundleInfo> bundleInfos = {info1, info2, info3};
    SvcRestoreDepsManager::GetInstance().BuildDepsMap(bundleInfos);
    EXPECT_EQ(SvcRestoreDepsManager::GetInstance().allBundles_.size(), 3);
    EXPECT_EQ(SvcRestoreDepsManager::GetInstance().depsMap_.size(), 3);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_BuildDepsMap_0200";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_BuildDepsMap_0300
 * @tc.name: SUB_SvcRestoreDepsManager_BuildDepsMap_0300
 * @tc.desc: 测试 BuildDepsMap 更新需要恢复的应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_BuildDepsMap_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_BuildDepsMap_0300";
    BJsonEntityCaps::BundleInfo info = {.name = BUNDLE_NAME,
                                        .versionCode = 1000000,
                                        .versionName = "1.0.0",
                                        .spaceOccupied = 0,
                                        .allToBackup = false,
                                        .extensionName = "",
                                        .needToInstall = false,
                                        .restoreDeps = ""};
    vector<BJsonEntityCaps::BundleInfo> bundleInfos = {info};
    vector<string> depList = {""};
    SvcRestoreDepsManager::GetInstance().depsMap_.emplace(make_pair(BUNDLE_NAME, depList));
    SvcRestoreDepsManager::GetInstance().BuildDepsMap(bundleInfos);
    EXPECT_EQ(SvcRestoreDepsManager::GetInstance().allBundles_.size(), 0);
    EXPECT_EQ(SvcRestoreDepsManager::GetInstance().depsMap_.size(), 1);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_BuildDepsMap_0300";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_SplitString_0100
 * @tc.name: SUB_SvcRestoreDepsManager_SplitString_0100
 * @tc.desc: 测试 SplitString 更新需要恢复的应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_SplitString_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_SplitString_0100";
    vector<string> dst = SvcRestoreDepsManager::GetInstance().SplitString("", "");
    EXPECT_TRUE(dst.empty());
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_SplitString_0100";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_SplitString_0200
 * @tc.name: SUB_SvcRestoreDepsManager_SplitString_0200
 * @tc.desc: 测试 SplitString 更新需要恢复的应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_SplitString_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_SplitString_0200";
    auto dst = SvcRestoreDepsManager::GetInstance().SplitString("com.example.app1,com.example.app2", ",");
    vector<string> bundles = {"com.example.app1", "com.example.app2"};
    EXPECT_TRUE(IsEqual(dst, bundles));
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_SplitString_0200";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_IsAllDepsRestored_0100
 * @tc.name: SUB_SvcRestoreDepsManager_IsAllDepsRestored_0100
 * @tc.desc: 测试 IsAllDepsRestored 更新需要恢复的应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_IsAllDepsRestored_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_IsAllDepsRestored_0100";
    bool ret = SvcRestoreDepsManager::GetInstance().IsAllDepsRestored("");
    EXPECT_FALSE(ret);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_IsAllDepsRestored_0100";
}

/**
 * @tc.number: SUB_SvcRestoreDepsManager_IsAllDepsRestored_0200
 * @tc.name: SUB_SvcRestoreDepsManager_IsAllDepsRestored_0200
 * @tc.desc: 测试 IsAllDepsRestored 更新需要恢复的应用列表接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcRestoreDepsManagerTest, SUB_SvcRestoreDepsManager_IsAllDepsRestored_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-begin SUB_SvcRestoreDepsManager_IsAllDepsRestored_0200";
    vector<string> depList = {};
    SvcRestoreDepsManager::GetInstance().depsMap_.emplace(make_pair(BUNDLE_NAME, depList));
    // SvcRestoreDepsManager::GetInstance().restoredBundles_.insert("com.example.app1");
    bool ret = SvcRestoreDepsManager::GetInstance().IsAllDepsRestored(BUNDLE_NAME);
    EXPECT_TRUE(ret);
    ClearCache();
    GTEST_LOG_(INFO) << "SvcRestoreDepsManagerTest-end SUB_SvcRestoreDepsManager_IsAllDepsRestored_0200";
}

} // namespace OHOS::FileManagement::Backup