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

#include <cstddef>
#include <cstdio>
#include <future>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "b_error/b_error.h"
#include "element_name.h"
#include "ext_extension_mock.h"
#include "module_ipc/svc_backup_connection.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
constexpr int32_t WAIT_TIME = 1;
} // namespace

class SvcBackupConnectionTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override;
    sptr<SvcBackupConnection> backupCon_ = nullptr;
};

static void CallDied(const std::string &&name)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-CallDied SUCCESS";
}

static void CallDone(const std::string &&name)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-CallDone SUCCESS";
}

void SvcBackupConnectionTest::SetUp()
{
    backupCon_ = sptr(new SvcBackupConnection(CallDied, CallDone));
}
void SvcBackupConnectionTest::TearDown()
{
    backupCon_ = nullptr;
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
    AppExecFwk::ElementName element;
    string bundleName = "";
    element.SetBundleName(bundleName);
    int resultCode = 1;
    sptr<BackupExtExtensionMock> mock = sptr(new BackupExtExtensionMock());
    backupCon_->OnAbilityConnectDone(element, mock->AsObject(), resultCode);
    bool ret = backupCon_->IsExtAbilityConnected();
    EXPECT_TRUE(ret);
    mock = nullptr;
    backupCon_->OnAbilityConnectDone(element, nullptr, resultCode);
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
    AppExecFwk::ElementName element;
    string bundleName = "";
    element.SetBundleName(bundleName);
    int resultCode = 1;
    backupCon_->OnAbilityDisconnectDone(element, resultCode);
    bool ret = backupCon_->IsExtAbilityConnected();
    EXPECT_FALSE(ret);
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
    auto proxy = backupCon_->GetBackupExtProxy();
    EXPECT_EQ(proxy, nullptr);
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_GetBackupExtProxy_0100";
}

/**
 * @tc.number: SUB_BackupConnection_ConnectBackupExtAbility_0100
 * @tc.name: SUB_BackupConnection_ConnectBackupExtAbility_0100
 * @tc.desc: 测试 ConnectBackupExtAbility 拉起extension接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_ConnectBackupExtAbility_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_ConnectBackupExtAbility_0100";
    AAFwk::Want want;
    ErrCode ret = backupCon_->ConnectBackupExtAbility(want);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_ConnectBackupExtAbility_0100";
}

static void CallBack(sptr<SvcBackupConnection> backupCon)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-CallBack Begin";
    sleep(WAIT_TIME);
    backupCon->OnAbilityDisconnectDone({}, WAIT_TIME);
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-CallBack End";
}

/**
 * @tc.number: SUB_BackupConnection_DisconnectBackupExtAbility_0100
 * @tc.name: SUB_BackupConnection_DisconnectBackupExtAbility_0100
 * @tc.desc: 测试 DisconnectBackupExtAbility 拉起extension接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_DisconnectBackupExtAbility_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_DisconnectBackupExtAbility_0100";
    ErrCode ret = backupCon_->DisconnectBackupExtAbility();
    EXPECT_EQ(ret, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-DisconnectBackupExtAbility async Begin";
    auto future = std::async(std::launch::async, CallBack, backupCon_);
    sleep(WAIT_TIME);
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-DisconnectBackupExtAbility Branches Begin";
    ret = backupCon_->DisconnectBackupExtAbility();
    EXPECT_EQ(ret, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-DisconnectBackupExtAbility Branches End";
    future.get();
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_DisconnectBackupExtAbility_0100";
}
} // namespace OHOS::FileManagement::Backup