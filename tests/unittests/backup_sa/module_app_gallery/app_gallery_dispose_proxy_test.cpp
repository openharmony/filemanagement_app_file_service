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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "b_error/b_error.h"
#include "b_radar/b_radar.h"
#include "b_sa/b_sa_utils.h"
#include "b_jsonutil/b_jsonutil.h"
#include "filemgmt_libhilog.h"
#include "message_parcel.h"

#include "bms_adapter_mock.h"
#include "module_app_gallery/app_gallery_dispose_proxy.h"
#include "module_app_gallery/app_gallery_service_connection.h"
#include "want.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace testing::ext;

class AppGalleryTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() {};
    void TearDown() {};

    static inline shared_ptr<BundleMgrAdapterMock> bms = nullptr;
};

void AppGalleryTest::SetUpTestCase(void)
{
    bms = make_shared<BundleMgrAdapterMock>();
    BundleMgrAdapterMock::bms = bms;
}

void AppGalleryTest::TearDownTestCase()
{
    BundleMgrAdapterMock::bms = nullptr;
    bms = nullptr;
}

/**
 * @tc.number: App_Gallery_GetInstance_0100
 * @tc.name: App_Gallery_GetInstance_0100
 * @tc.desc: 测试 GetInstance
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(AppGalleryTest, App_Gallery_GetInstance_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin App_Gallery_GetInstance_0100";
    try {
        auto instance = AppGalleryDisposeProxy::GetInstance();
        bool result = instance != nullptr;
        EXPECT_TRUE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by App_Gallery_GetInstance_0100.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end App_Gallery_GetInstance_0100";
}

/**
 * @tc.number: App_Gallery_GetInstance_0200
 * @tc.name: App_Gallery_GetInstance_0200
 * @tc.desc: 测试 GetInstance
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(AppGalleryTest, App_Gallery_GetInstance_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin App_Gallery_GetInstance_0200";
    try {
        auto instance1 = AppGalleryDisposeProxy::GetInstance();
        auto instance2 = AppGalleryDisposeProxy::GetInstance();
        bool result = instance1 == instance2;
        EXPECT_TRUE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by App_Gallery_GetInstance_0200.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end App_Gallery_GetInstance_0200";
}

/**
 * @tc.number: App_Gallery_GetAppGalleryConnection_0100
 * @tc.name: App_Gallery_GetAppGalleryConnection_0100
 * @tc.desc: 测试 GetAppGalleryConnection
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(AppGalleryTest, App_Gallery_GetAppGalleryConnection_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin App_Gallery_GetAppGalleryConnection_0100";
    try {
        auto instance = AppGalleryDisposeProxy::GetInstance();
        EXPECT_TRUE(instance != nullptr);
        int32_t userId = 100;
        auto result = instance->GetAppGalleryConnection(userId);
        EXPECT_TRUE(result == nullptr);
        instance->appGalleryConnectionMap_[userId] = sptr(new AppGalleryConnection(userId));
        result = instance->GetAppGalleryConnection(userId);
        EXPECT_TRUE(result != nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by App_Gallery_GetAppGalleryConnection_0100.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end App_Gallery_GetAppGalleryConnection_0100";
}

/**
 * @tc.number: App_Gallery_ConnectAppGallery_0100
 * @tc.name: App_Gallery_ConnectAppGallery_0100
 * @tc.desc: 测试 ConnectAppGallery
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(AppGalleryTest, App_Gallery_ConnectAppGallery_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin App_Gallery_ConnectAppGallery_0100";
    try {
        auto instance = AppGalleryDisposeProxy::GetInstance();
        EXPECT_TRUE(instance != nullptr);
        int32_t userId = 101;
        EXPECT_CALL(*bms, GetAppGalleryBundleName()).WillOnce(Return(""));
        auto result = instance->ConnectAppGallery(userId);
        EXPECT_TRUE(result == nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by App_Gallery_ConnectAppGallery_0100.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end App_Gallery_ConnectAppGallery_0100";
}

/**
 * @tc.number: App_Gallery_StartRestore_0100
 * @tc.name: App_Gallery_StartRestore_0100
 * @tc.desc: 测试 StartRestore
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(AppGalleryTest, App_Gallery_StartRestore_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin App_Gallery_StartRestore_0100";
    try {
        auto instance = AppGalleryDisposeProxy::GetInstance();
        EXPECT_TRUE(instance != nullptr);
        std::string bundleName = "1234";
        int32_t userId = 102;
        auto result = instance->StartRestore(bundleName, userId);
        EXPECT_EQ(result, DisposeErr::OK);
        result = instance->EndRestore(bundleName, userId);
        EXPECT_EQ(result, DisposeErr::OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by App_Gallery_StartRestore_0100.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end App_Gallery_StartRestore_0100";
}

/**
 * @tc.number: App_Gallery_StartRestore_0200
 * @tc.name: App_Gallery_StartRestore_0200
 * @tc.desc: 测试 StartRestore
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(AppGalleryTest, App_Gallery_StartRestore_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin App_Gallery_StartRestore_0200";
    try {
        auto instance = AppGalleryDisposeProxy::GetInstance();
        EXPECT_TRUE(instance != nullptr);
        std::string bundleName = "testapp";
        int32_t userId = 102;
        EXPECT_CALL(*bms, GetAppGalleryBundleName()).WillOnce(Return(""));
        auto result = instance->StartRestore(bundleName, userId);
        EXPECT_EQ(result, DisposeErr::CONN_FAIL);
        EXPECT_CALL(*bms, GetAppGalleryBundleName()).WillOnce(Return(""));
        result = instance->EndRestore(bundleName, userId);
        EXPECT_EQ(result, DisposeErr::CONN_FAIL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by App_Gallery_StartRestore_0200.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end App_Gallery_StartRestore_0200";
}

/**
 * @tc.number: App_Gallery_StartBackup_0100
 * @tc.name: App_Gallery_StartBackup_0100
 * @tc.desc: 测试 StartBackup
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(AppGalleryTest, App_Gallery_StartBackup_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin App_Gallery_StartBackup_0100";
    try {
        auto instance = AppGalleryDisposeProxy::GetInstance();
        EXPECT_TRUE(instance != nullptr);
        std::string bundleName = "1234";
        int32_t userId = 102;
        EXPECT_CALL(*bms, GetAppGalleryBundleName()).WillOnce(Return(""));
        auto result = instance->StartBackup(bundleName, userId);
        EXPECT_EQ(result, DisposeErr::CONN_FAIL);
        EXPECT_CALL(*bms, GetAppGalleryBundleName()).WillOnce(Return(""));
        result = instance->EndBackup(bundleName, userId);
        EXPECT_EQ(result, DisposeErr::CONN_FAIL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by App_Gallery_StartBackup_0100.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end App_Gallery_StartBackup_0100";
}
}