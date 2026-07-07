/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "module_ipc/service.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
const string BUNDLE_NAME_DEFAULT = "com.example.default";
const string BUNDLE_NAME_NORMAL = "com.example.normal";
const string BUNDLE_NAME_OTHER = "com.example.other";
constexpr int32_t SERVICE_ID = 5203;
constexpr int32_t USER_ID = 100;
} // namespace

class DefaultAppManagerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        service_ = sptr(new Service(SERVICE_ID));
    }
    static void TearDownTestCase()
    {
        service_ = nullptr;
    }
    void SetUp() override
    {
        service_->defaultAppManager_->defaultAppMap_.clear();
    }
    void TearDown() override {}

    static inline sptr<Service> service_;
};

/**
 * @tc.name: DefaultAppManagerTest_SetDefaultBundleName_Add_0100
 * @tc.desc: Verify SetDefaultBundleName with result=true adds bundle to map
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_SetDefaultBundleName_Add_0100, testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
}

/**
 * @tc.name: DefaultAppManagerTest_SetDefaultBundleName_Remove_0200
 * @tc.desc: Verify SetDefaultBundleName with result=false removes bundle from map
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_SetDefaultBundleName_Remove_0200, testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));

    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, false);
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
}

/**
 * @tc.name: DefaultAppManagerTest_SetDefaultBundleName_RemoveNonExistent_0300
 * @tc.desc: Verify SetDefaultBundleName with result=false on non-existent bundle does nothing
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_SetDefaultBundleName_RemoveNonExistent_0300,
    testing::ext::TestSize.Level1)
{
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_OTHER}, false);
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_OTHER));
}

/**
 * @tc.name: DefaultAppManagerTest_SetDefaultBundleName_MultipleBundles_0400
 * @tc.desc: Verify SetDefaultBundleName handles multiple bundles
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_SetDefaultBundleName_MultipleBundles_0400,
    testing::ext::TestSize.Level1)
{
    vector<string> bundleNames = {BUNDLE_NAME_DEFAULT, BUNDLE_NAME_OTHER};
    service_->defaultAppManager_->SetDefaultBundleName(bundleNames, true);
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_OTHER));

    service_->defaultAppManager_->SetDefaultBundleName(bundleNames, false);
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_OTHER));
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_NonExistent_0500
 * @tc.desc: Verify IsDefaultBundle returns false for bundle not in map
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_NonExistent_0500,
    testing::ext::TestSize.Level1)
{
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_NORMAL));
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_Null_0600
 * @tc.desc: Verify IsDefaultBundle returns false for empty name
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_Null_0600, testing::ext::TestSize.Level1)
{
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(""));
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_AllDefault_0700
 * @tc.desc: Verify IsDefaultBundle returns true when all bundles are default
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_AllDefault_0700,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle({BUNDLE_NAME_DEFAULT}));
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_NotDefault_0800
 * @tc.desc: Verify IsDefaultBundle returns false when bundle is not default
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_NotDefault_0800,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_OTHER}, true);
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle({BUNDLE_NAME_DEFAULT}));
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_MultipleMixed_0900
 * @tc.desc: Verify IsDefaultBundle returns first bundle's result when multiple are passed
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_MultipleMixed_0900,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    vector<string> bundleNames = {BUNDLE_NAME_DEFAULT, BUNDLE_NAME_NORMAL};
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(bundleNames));
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_EmptyVector_1000
 * @tc.desc: Verify IsDefaultBundle returns false for empty vector
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_EmptyVector_1000,
    testing::ext::TestSize.Level1)
{
    std::vector<std::string> bundles = {};
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(bundles));
}

/**
 * @tc.name: DefaultAppManagerTest_GetCallerNameByFilePath_Found_1100
 * @tc.desc: Verify GetCallerNameByFilePath returns bundle name when file path contains it
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetCallerNameByFilePath_Found_1100,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    string filePath = "/data/app/el2/100/base/" + BUNDLE_NAME_DEFAULT + "/backup.tar";
    EXPECT_EQ(service_->defaultAppManager_->GetCallerNameByFilePath(filePath), BUNDLE_NAME_DEFAULT);
}

/**
 * @tc.name: DefaultAppManagerTest_GetCallerNameByFilePath_NotFound_1200
 * @tc.desc: Verify GetCallerNameByFilePath returns empty when no bundle name matches
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetCallerNameByFilePath_NotFound_1200,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    string filePath = "/data/unknown/path.tar";
    EXPECT_EQ(service_->defaultAppManager_->GetCallerNameByFilePath(filePath), "");
}

/**
 * @tc.name: DefaultAppManagerTest_GetCallerNameByFilePath_EmptyMap_1300
 * @tc.desc: Verify GetCallerNameByFilePath returns empty when map is empty
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetCallerNameByFilePath_EmptyMap_1300,
    testing::ext::TestSize.Level1)
{
    string filePath = "/data/app/el2/100/base/" + BUNDLE_NAME_DEFAULT + "/backup.tar";
    EXPECT_EQ(service_->defaultAppManager_->GetCallerNameByFilePath(filePath), "");
}

/**
 * @tc.name: DefaultAppManagerTest_GetCallerNameByFilePath_FirstMatch_1400
 * @tc.desc: Verify GetCallerNameByFilePath returns first matching bundle
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetCallerNameByFilePath_FirstMatch_1400,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT, BUNDLE_NAME_OTHER}, true);
    string filePath = "/data/app/el2/100/base/" + BUNDLE_NAME_DEFAULT + "/" + BUNDLE_NAME_OTHER + "/backup.tar";
    string result = service_->defaultAppManager_->GetCallerNameByFilePath(filePath);
    EXPECT_EQ(result, BUNDLE_NAME_DEFAULT);
}

/**
 * @tc.name: DefaultAppManagerTest_GetMigrateInstance_Create_1500
 * @tc.desc: Verify GetMigrateInstance creates new MigrateManager for default bundle
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetMigrateInstance_Create_1500,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    EXPECT_NE(instance, nullptr);
}

/**
 * @tc.name: DefaultAppManagerTest_GetMigrateInstance_Reuse_1600
 * @tc.desc: Verify GetMigrateInstance returns same instance for same default bundle
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetMigrateInstance_Reuse_1600,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance1 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    auto instance2 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    EXPECT_EQ(instance1, instance2);
}

/**
 * @tc.name: DefaultAppManagerTest_GetMigrateInstance_DifferentBundle_1700
 * @tc.desc: Verify GetMigrateInstance returns different instances for different default bundles
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetMigrateInstance_DifferentBundle_1700,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT, BUNDLE_NAME_OTHER}, true);
    auto instance1 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    auto instance2 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_OTHER, USER_ID);
    EXPECT_NE(instance1, instance2);
}

/**
 * @tc.name: DefaultAppManagerTest_ClearDefaultAppData_HasClearedFlag_1800
 * @tc.desc: Verify ClearDefaultAppData sets hasCleared_ flag on MigrateManager
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_ClearDefaultAppData_HasClearedFlag_1800,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    EXPECT_FALSE(instance->hasCleared_.load());

    service_->defaultAppManager_->ClearDefaultAppData();
    EXPECT_TRUE(instance->hasCleared_.load());
}

/**
 * @tc.name: DefaultAppManagerTest_ClearDefaultAppData_MultipleBundles_1900
 * @tc.desc: Verify ClearDefaultAppData clears all default bundles
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_ClearDefaultAppData_MultipleBundles_1900,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT, BUNDLE_NAME_OTHER}, true);
    auto instance1 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    auto instance2 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_OTHER, USER_ID);

    service_->defaultAppManager_->ClearDefaultAppData();
    EXPECT_TRUE(instance1->hasCleared_.load());
    EXPECT_TRUE(instance2->hasCleared_.load());
}

/**
 * @tc.name: DefaultAppManagerTest_ClearDefaultAppData_Idempotent_2000
 * @tc.desc: Verify ClearDefaultAppData is idempotent (calling twice doesn't crash)
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_ClearDefaultAppData_Idempotent_2000,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);

    EXPECT_TRUE(service_->defaultAppManager_->ClearDefaultAppData());
    EXPECT_TRUE(service_->defaultAppManager_->ClearDefaultAppData());
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_AfterClear_2100
 * @tc.desc: Verify IsDefaultBundle still returns true after ClearDefaultAppData (bundle remains in map)
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_AfterClear_2100,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    service_->defaultAppManager_->ClearDefaultAppData();

    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
}

/**
 * @tc.name: DefaultAppManagerTest_GetMigrateInstance_AfterClear_2200
 * @tc.desc: Verify GetMigrateInstance returns same instance after ClearDefaultAppData
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetMigrateInstance_AfterClear_2200,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance1 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    service_->defaultAppManager_->ClearDefaultAppData();
    auto instance2 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    EXPECT_EQ(instance1, instance2);
}

/**
 * @tc.name: DefaultAppManagerTest_GetKeyInMap_Found_2300
 * @tc.desc: Verify GetCallerNameByFilePath finds bundle when path ends with /bundlename/
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetKeyInMap_Found_2300, testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    string filePath = "/data/app/el2/100/base/" + BUNDLE_NAME_DEFAULT + "/";
    string result = service_->defaultAppManager_->GetCallerNameByFilePath(filePath);
    EXPECT_EQ(result, BUNDLE_NAME_DEFAULT);
}

/**
 * @tc.name: DefaultAppManagerTest_GetKeyInMap_NoFalsePositive_2400
 * @tc.desc: Verify GetCallerNameByFilePath does NOT match when bundle name is only a prefix of a directory
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetKeyInMap_NoFalsePositive_2400,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    string filePath = "/data/app/el2/100/base/" + BUNDLE_NAME_DEFAULT + ".extra/backup.tar";
    string result = service_->defaultAppManager_->GetCallerNameByFilePath(filePath);
    EXPECT_EQ(result, "");
}

/**
 * @tc.name: DefaultAppManagerTest_ReAddAfterRemove_2500
 * @tc.desc: Verify adding bundle after removal works correctly
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_ReAddAfterRemove_2500, testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));

    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, false);
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));

    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
}

/**
 * @tc.name: DefaultAppManagerTest_GetCallerNameByFilePath_EmptyPath_2600
 * @tc.desc: Verify GetCallerNameByFilePath returns empty for empty file path
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetCallerNameByFilePath_EmptyPath_2600,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    EXPECT_EQ(service_->defaultAppManager_->GetCallerNameByFilePath(""), "");
}

/**
 * @tc.name: DefaultAppManagerTest_GetMigrateInstance_NonDefault_2700
 * @tc.desc: Verify GetMigrateInstance returns nullptr for non-default bundle
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetMigrateInstance_NonDefault_2700,
    testing::ext::TestSize.Level1)
{
    auto instance = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_NORMAL, USER_ID);
    EXPECT_EQ(instance, nullptr);
}
} // namespace OHOS::FileManagement::Backup
