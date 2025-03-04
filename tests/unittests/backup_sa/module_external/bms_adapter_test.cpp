/*)
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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

#include "b_jsonutil_mock.h"
#include "b_sa_utils_mock.h"
#include "if_system_ability_manager.h"
#include "iremote_stub.h"
#include "iservice_registry.h"
#include "module_external/bms_adapter.h"

namespace OHOS {
using namespace std;
using namespace testing;
using namespace FileManagement::Backup;

const string HMOS_HAP_CODE_PATH = "1";
const string LINUX_HAP_CODE_PATH = "2";
const string BUNDLE_NAME = "com.example.app2backup";
const int32_t USER_ID = 100;

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
    MOCK_METHOD((sptr<IRemoteObject>), GetLocalAbilityManagerProxy, (int32_t));
};

class BundleMgrMock : public IRemoteStub<AppExecFwk::IBundleMgr> {
public:
    MOCK_METHOD((sptr<IRemoteObject>), AsObject, ());
    MOCK_METHOD(bool, GetBundleInfos, (const AppExecFwk::BundleFlag, (std::vector<AppExecFwk::BundleInfo>&), int32_t));
    MOCK_METHOD(ErrCode, GetCloneBundleInfo, (const std::string&, int32_t, int32_t, AppExecFwk::BundleInfo&, int32_t));
};

class BmsAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
public:
    static inline sptr<BundleMgrMock> bms = nullptr;
    static inline sptr<SystemAbilityManagerMock> sam = nullptr;
    static inline shared_ptr<SAUtilsMock> saUtils = nullptr;
    static inline shared_ptr<BJsonUtilMock> jsonUtil = nullptr;
};

void BmsAdapterTest::SetUpTestCase()
{
    sam = sptr<SystemAbilityManagerMock>(new SystemAbilityManagerMock());
    bms = sptr<BundleMgrMock>(new BundleMgrMock());
    saUtils = make_shared<SAUtilsMock>();
    SAUtilsMock::utils = saUtils;
    jsonUtil = make_shared<BJsonUtilMock>();
    BJsonUtilMock::jsonUtil = jsonUtil;
}

void BmsAdapterTest::TearDownTestCase()
{
    bms = nullptr;
    sam = nullptr;
    SAUtilsMock::utils = nullptr;
    saUtils = nullptr;
    BJsonUtilMock::jsonUtil = nullptr;
    jsonUtil = nullptr;
}

sptr<ISystemAbilityManager> SystemAbilityManagerClient::GetSystemAbilityManager()
{
    return BmsAdapterTest::sam;
}

/**
 * @tc.number: SUB_bms_adapter_GetBundleInfosForLocalCapabilities_test_0100
 * @tc.name: SUB_bms_adapter_GetBundleInfosForLocalCapabilities_test_0100
 * @tc.desc: 测试 GetBundleInfosForLocalCapabilities 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(BmsAdapterTest, SUB_bms_adapter_GetBundleInfosForLocalCapabilities_test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BmsAdapterTest-begin SUB_bms_adapter_GetBundleInfosForLocalCapabilities_test_0100";
    try {
        EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(bms));
        EXPECT_CALL(*bms, GetBundleInfos(_, _, _)).WillOnce(Return(false));
        auto bundleInfo = BundleMgrAdapter::GetBundleInfosForLocalCapabilities(100);
        EXPECT_EQ(bundleInfo.size(), 0);

        AppExecFwk::BundleInfo info;
        std::vector<AppExecFwk::BundleInfo> infos;
        info.applicationInfo.codePath = HMOS_HAP_CODE_PATH;
        infos.emplace_back(info);
        info.applicationInfo.codePath = LINUX_HAP_CODE_PATH;
        infos.emplace_back(info);
        info.applicationInfo.codePath = "5";
        info.appIndex = 1;
        infos.emplace_back(info);
        EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(bms)).WillOnce(Return(bms));
        EXPECT_CALL(*bms, GetBundleInfos(_, _, _)).WillOnce(DoAll(SetArgReferee<1>(infos), Return(true)));
        EXPECT_CALL(*bms, GetCloneBundleInfo(_, _, _, _, _)).WillOnce(Return(1));
        EXPECT_CALL(*sam, GetExtensionSaIds(_, _)).WillOnce(Return(0));
        bundleInfo = BundleMgrAdapter::GetBundleInfosForLocalCapabilities(100);
        EXPECT_EQ(bundleInfo.size(), 0);

        infos.clear();
        info.applicationInfo.codePath = "5";
        info.appIndex = 0;
        infos.emplace_back(info);
        EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(bms)).WillOnce(Return(bms));
        EXPECT_CALL(*bms, GetBundleInfos(_, _, _)).WillOnce(DoAll(SetArgReferee<1>(infos), Return(true)));
        EXPECT_CALL(*sam, GetExtensionSaIds(_, _)).WillOnce(Return(0));
        bundleInfo = BundleMgrAdapter::GetBundleInfosForLocalCapabilities(100);
        EXPECT_EQ(bundleInfo.size(), 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BmsAdapterTest-an exception occurred by GetBundleInfosForLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "BmsAdapterTest-end SUB_bms_adapter_GetBundleInfosForLocalCapabilities_test_0100";
}

/**
 * @tc.number: SUB_bms_adapter_GetBundleInfosForIndex_test_0100
 * @tc.name: SUB_bms_adapter_GetBundleInfosForIndex_test_0100
 * @tc.desc: 测试 GetBundleInfosForIndex 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(BmsAdapterTest, SUB_bms_adapter_GetBundleInfosForIndex_test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BmsAdapterTest-begin SUB_bms_adapter_GetBundleInfosForIndex_test_0100";
    try {
        vector<string> bundleNames;
        bundleNames.emplace_back("bundleName");
        EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(bms));
        EXPECT_CALL(*bms, GetCloneBundleInfo(_, _, _, _, _)).WillOnce(Return(1));
        auto bundleInfo = BundleMgrAdapter::GetBundleInfosForIndex(bundleNames, 100);
        EXPECT_EQ(bundleInfo.size(), 0);

        AppExecFwk::BundleInfo info;
        info.applicationInfo.codePath = "5";
        EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(bms));
        EXPECT_CALL(*bms, GetCloneBundleInfo(_, _, _, _, _)).WillOnce(DoAll(SetArgReferee<3>(info), Return(ERR_OK)));
        bundleInfo = BundleMgrAdapter::GetBundleInfosForIndex(bundleNames, 100);
        EXPECT_EQ(bundleInfo.size(), 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BmsAdapterTest-an exception occurred by GetBundleInfosForIndex.";
    }
    GTEST_LOG_(INFO) << "BmsAdapterTest-end SUB_bms_adapter_GetBundleInfosForIndex_test_0100";
}

/**
 * @tc.number: SUB_bms_adapter_CreatBackupEnv_test_0000
 * @tc.name: SUB_bms_adapter_CreatBackupEnv_test_0000
 * @tc.desc: 测试 CreatBackupEnv 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(BmsAdapterTest, SUB_bms_adapter_CreatBackupEnv_test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BmsAdapterTest-begin SUB_bms_adapter_CreatBackupEnv_test_0000";
    try {
        std::vector<BIncrementalData> bundleNameList;
        BIncrementalData data;
        data.bundleName = BUNDLE_NAME;
        data.lastIncrementalTime = 0;
        bundleNameList.push_back(data);
        EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(bms));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(true));
        BundleMgrAdapter::CreatBackupEnv(bundleNameList, USER_ID);
        EXPECT_TRUE(true);

        BJsonUtil::BundleDetailInfo info;
        EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(bms));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(info));
        EXPECT_CALL(*bms, GetCloneBundleInfo(_, _, _, _, _)).WillOnce(Return(1));
        BundleMgrAdapter::CreatBackupEnv(bundleNameList, USER_ID);
        EXPECT_TRUE(true);

        AppExecFwk::BundleInfo bundleInfo;
        bundleInfo.applicationInfo.codePath = "5";
        EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(bms));
        EXPECT_CALL(*saUtils, IsSABundleName(_)).WillOnce(Return(false));
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(info));
        EXPECT_CALL(*bms, GetCloneBundleInfo(_, _, _, _, _))
            .WillOnce(DoAll(SetArgReferee<3>(bundleInfo), Return(ERR_OK)));
        BundleMgrAdapter::CreatBackupEnv(bundleNameList, USER_ID);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BmsAdapterTest-an exception occurred by CreatBackupEnv.";
    }
    GTEST_LOG_(INFO) << "BmsAdapterTest-end SUB_bms_adapter_CreatBackupEnv_test_0000";
}
} // namespace OHOS