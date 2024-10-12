/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <cstddef>
#include <cstdio>
#include <future>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "b_error/b_error.h"
#include "element_name.h"
#include "ext_extension_mock.h"
#include "message_parcel_mock.h"
#include "module_ipc/svc_backup_connection.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class SvcBackupConnectionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
public:
    static inline sptr<SvcBackupConnection> backupCon_ = nullptr;
    static inline shared_ptr<IfaceCastMock> castMock = nullptr;
};

static void CallDied(const std::string &&name, bool isSecondCalled)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-CallDied SUCCESS";
}

static void CallDone(const std::string &&name)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-CallDone SUCCESS";
}

void SvcBackupConnectionTest::SetUpTestCase()
{
    backupCon_ = sptr(new SvcBackupConnection(CallDied, CallDone, "com.example.app"));
    castMock = std::make_shared<IfaceCastMock>();
    IfaceCastMock::cast = castMock;
}
void SvcBackupConnectionTest::TearDownTestCase()
{
    backupCon_ = nullptr;
    IfaceCastMock::cast = nullptr;
    castMock = nullptr;
}

/**
 * @tc.number: SUB_BackupConnection_OnAbilityConnectDone_0100
 * @tc.name: SUB_BackupConnection_OnAbilityConnectDone_0100
 * @tc.desc: 测试 OnAbilityConnectDone 链接回调接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_OnAbilityConnectDone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_OnAbilityConnectDone_0100";
    try {
        int resultCode = 0;
        AppExecFwk::ElementName element;
        EXPECT_TRUE(backupCon_ != nullptr);
        EXPECT_TRUE(castMock != nullptr);
        backupCon_->OnAbilityConnectDone(element, nullptr, resultCode);
        EXPECT_TRUE(true);

        sptr<IRemoteObject> remoteObject = sptr(new BackupExtExtensionMock);
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(nullptr));
        backupCon_->OnAbilityConnectDone(element, remoteObject, resultCode);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnAbilityConnectDone.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnAbilityConnectDone_0100";
}

/**
 * @tc.number: SUB_BackupConnection_OnAbilityDisconnectDone_0100
 * @tc.name: SUB_BackupConnection_OnAbilityDisconnectDone_0100
 * @tc.desc: 测试 OnAbilityDisconnectDone 链接回调接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_OnAbilityDisconnectDone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_OnAbilityDisconnectDone_0100";
    try {
        AppExecFwk::ElementName element;
        string bundleName = "";
        element.SetBundleName(bundleName);
        int resultCode = 1;

        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->isConnectedDone_ = false;
        backupCon_->OnAbilityDisconnectDone(element, resultCode);
        bool ret = backupCon_->IsExtAbilityConnected();
        EXPECT_FALSE(ret);

        backupCon_->isConnectedDone_ = true;
        backupCon_->OnAbilityDisconnectDone(element, resultCode);
        ret = backupCon_->IsExtAbilityConnected();
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnAbilityDisconnectDone.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnAbilityDisconnectDone_0100";
}

/**
 * @tc.number: SUB_BackupConnection_GetBackupExtProxy_0100
 * @tc.name: SUB_BackupConnection_GetBackupExtProxy_0100
 * @tc.desc: 测试 GetBackupExtProxy 获取连接状态接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_GetBackupExtProxy_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_GetBackupExtProxy_0100";
    EXPECT_TRUE(backupCon_ != nullptr);
    auto proxy = backupCon_->GetBackupExtProxy();
    EXPECT_EQ(proxy, nullptr);
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_GetBackupExtProxy_0100";
}
} // namespace OHOS::FileManagement::Backup