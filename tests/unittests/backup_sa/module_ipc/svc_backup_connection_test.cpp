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
#include "include/svc_extension_proxy_mock.h"
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

static void CallDied(const std::string &name, bool isCleanCalled)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-CallDied SUCCESS";
}

static void CallDone(const std::string &name)
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
        element.SetBundleName("com.example.app");
        EXPECT_TRUE(backupCon_ != nullptr);
        EXPECT_TRUE(castMock != nullptr);
        backupCon_->connState_ = SvcBackupConnection::ConnState::CONNECTING;
        backupCon_->OnAbilityConnectDone(element, nullptr, resultCode);
        EXPECT_TRUE(true);

        // iface_cast 返回 nullptr 时，OnAbilityConnectDone 提前返回，状态不变
        sptr<IRemoteObject> remoteObject = sptr(new BackupExtExtensionMock);
        backupCon_->connState_ = SvcBackupConnection::ConnState::CONNECTING;
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(nullptr));
        backupCon_->OnAbilityConnectDone(element, remoteObject, resultCode);
        EXPECT_EQ(backupCon_->GetBackupExtProxy(), nullptr);

        // iface_cast 返回有效代理时，状态转为 CONNECTED，GetBackupExtProxy 返回缓存的代理
        sptr<SvcExtensionProxyMock> svcProxy = sptr(new SvcExtensionProxyMock());
        backupCon_->connState_ = SvcBackupConnection::ConnState::CONNECTING;
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(svcProxy));
        backupCon_->OnAbilityConnectDone(element, remoteObject, resultCode);
        EXPECT_NE(backupCon_->GetBackupExtProxy(), nullptr);
        EXPECT_EQ(backupCon_->connState_, SvcBackupConnection::ConnState::CONNECTED);
        backupCon_->backupProxy_ = nullptr;
        backupCon_->deathRecipient_ = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnAbilityConnectDone.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnAbilityConnectDone_0100";
}

/**
 * @tc.number: SUB_BackupConnection_GenErrorByStatus_0100
 * @tc.name: SUB_BackupConnection_GenErrorByStatus_0100
 * @tc.desc: 测试 GenErrorByStatus
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_GenErrorByStatus_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_GenErrorByStatus_0100";
    int resultCode = -1;
    AppExecFwk::ElementName element;
    EXPECT_TRUE(backupCon_ != nullptr);
    backupCon_->GenErrorByStatus(resultCode, true, false);
    EXPECT_EQ(backupCon_->error_.GetRawCode(), BError::Codes::EXT_ABILITY_DIED);

    backupCon_->connectSpend_.End();
    backupCon_->GenErrorByStatus(resultCode, false, false);
    EXPECT_EQ(backupCon_->error_.GetRawCode(), BError::Codes::EXT_ABILITY_DIED);

    backupCon_->connectSpend_.startMilli_ = 1;
    backupCon_->connectSpend_.End();
    backupCon_->GenErrorByStatus(resultCode, false, false);
    EXPECT_EQ(backupCon_->error_.GetRawCode(), BError::Codes::SA_BOOT_EXT_TIMEOUT);
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_GenErrorByStatus_0100";
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
        backupCon_->onDiedCb_ = nullptr;
        backupCon_->OnAbilityDisconnectDone(element, resultCode);
        bool ret = backupCon_->IsExtAbilityConnected();
        EXPECT_FALSE(ret);

        backupCon_->onDiedCb_ = CallDied;
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
 * @tc.number: SUB_BackupConnection_OnAbilityDisconnectDone_0200
 * @tc.name: SUB_BackupConnection_OnAbilityDisconnectDone_0200
 * @tc.desc: 测试 OnAbilityDisconnectDone 链接回调接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_OnAbilityDisconnectDone_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_OnAbilityDisconnectDone_0200";
    try {
        AppExecFwk::ElementName element;
        string bundleName = "test";
        element.SetBundleName(bundleName);
        int resultCode = 1;

        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->bundleNameIndexInfo_ = "app";
        backupCon_->OnAbilityDisconnectDone(element, resultCode);
        bool ret = backupCon_->IsExtAbilityConnected();
        EXPECT_FALSE(ret);

        backupCon_->onDiedCb_ = CallDied;
        backupCon_->isCleanMode_ = true;
        backupCon_->bundleNameIndexInfo_ = "test";
        backupCon_->OnAbilityDisconnectDone(element, resultCode);
        ret = backupCon_->IsExtAbilityConnected();
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnAbilityDisconnectDone.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnAbilityDisconnectDone_0200";
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

/**
 * @tc.number: SUB_BackupConnection_WaitDisconnectDone_0100
 * @tc.name: SUB_BackupConnection_WaitDisconnectDone_0100
 * @tc.desc: 测试 WaitDisconnectDone 获取连接状态接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_WaitDisconnectDone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_WaitDisconnectDone_0100";
    EXPECT_TRUE(backupCon_ != nullptr);
    auto res = backupCon_->WaitDisconnectDone();
    EXPECT_TRUE(res);
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_WaitDisconnectDone_0100";
}

/**
 * @tc.number: SUB_BackupConnection_OnExtensionDied_0100
 * @tc.name: SUB_BackupConnection_OnExtensionDied_0100
 * @tc.desc: 测试 OnExtensionDied 超时兜底处理（OnAbilityDisconnectDone 未到达）
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_OnExtensionDied_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_OnExtensionDied_0100";
    try {
        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->onDiedCb_ = CallDied;
        backupCon_->connState_ = SvcBackupConnection::ConnState::CONNECTED;
        backupCon_->bundleNameIndexInfo_ = "com.example.app";
        wptr<IRemoteObject> remoteObj = sptr(new BackupExtExtensionMock);
        backupCon_->OnExtensionDied(remoteObj);
        EXPECT_FALSE(backupCon_->IsExtAbilityConnected());
        EXPECT_EQ(backupCon_->GetBackupExtProxy(), nullptr);
        EXPECT_FALSE(backupCon_->onDiedCb_);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnExtensionDied.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnExtensionDied_0100";
}

/**
 * @tc.number: SUB_BackupConnection_OnExtensionDied_0200
 * @tc.name: SUB_BackupConnection_OnExtensionDied_0200
 * @tc.desc: 测试 OnExtensionDied 重复调用去重
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_OnExtensionDied_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_OnExtensionDied_0200";
    try {
        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->onDiedCb_ = CallDied;
        backupCon_->connState_ = SvcBackupConnection::ConnState::CONNECTED;
        wptr<IRemoteObject> remoteObj = sptr(new BackupExtExtensionMock);
        backupCon_->OnExtensionDied(remoteObj);
        EXPECT_FALSE(backupCon_->onDiedCb_);
        bool connectedBefore = backupCon_->IsExtAbilityConnected();
        EXPECT_FALSE(connectedBefore);
        // 第二次调用应被去重，不会崩溃
        backupCon_->OnExtensionDied(remoteObj);
        EXPECT_FALSE(backupCon_->IsExtAbilityConnected());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnExtensionDied dedup.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnExtensionDied_0200";
}

/**
 * @tc.number: SUB_BackupConnection_OnExtensionDied_0300
 * @tc.name: SUB_BackupConnection_OnExtensionDied_0300
 * @tc.desc: 测试 OnExtensionDied 在 onDiedCb_ 为 null 时不触发 onDiedCb_
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_OnExtensionDied_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_OnExtensionDied_0300";
    try {
        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->onDiedCb_ = nullptr;
        backupCon_->connState_ = SvcBackupConnection::ConnState::CONNECTED;
        wptr<IRemoteObject> remoteObj = sptr(new BackupExtExtensionMock);
        backupCon_->OnExtensionDied(remoteObj);
        // onDiedCb_ 为 null 时 OnExtensionDied 直接返回，不改变 connState_
        EXPECT_TRUE(backupCon_->IsExtAbilityConnected());
        EXPECT_FALSE(backupCon_->onDiedCb_);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnExtensionDied no connect.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnExtensionDied_0300";
}

/**
 * @tc.number: SUB_BackupConnection_OnExtensionDied_0400
 * @tc.name: SUB_BackupConnection_OnExtensionDied_0400
 * @tc.desc: 测试 OnAbilityDisconnectDone 先到时携带 resultCode 处理 onDiedCb_，OnExtensionDied 后到被去重
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_OnExtensionDied_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_OnExtensionDied_0400";
    try {
        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->onDiedCb_ = CallDied;
        backupCon_->connState_ = SvcBackupConnection::ConnState::CONNECTING;
        backupCon_->bundleNameIndexInfo_ = "com.example.app";
        sptr<IRemoteObject> remoteObj = sptr(new BackupExtExtensionMock);
        AppExecFwk::ElementName connElement;
        connElement.SetBundleName("com.example.app");
        sptr<SvcExtensionProxyMock> svcProxy = sptr(new SvcExtensionProxyMock());
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(svcProxy));
        backupCon_->OnAbilityConnectDone(connElement, remoteObj, 0);
        EXPECT_NE(backupCon_->GetBackupExtProxy(), nullptr);

        // OnAbilityDisconnectDone 先到：携带 resultCode 处理 onDiedCb_ 并清理 backupProxy_
        AppExecFwk::ElementName element;
        element.SetBundleName("com.example.app");
        backupCon_->OnAbilityDisconnectDone(element, 1);
        EXPECT_FALSE(backupCon_->IsExtAbilityConnected());
        EXPECT_EQ(backupCon_->GetBackupExtProxy(), nullptr);
        EXPECT_FALSE(backupCon_->onDiedCb_);
        EXPECT_EQ(backupCon_->deathRecipient_, nullptr);

        // OnExtensionDied 后到：notify 已发出但 wait 已错过，超时后 CAS 失败，直接返回
        backupCon_->OnExtensionDied(remoteObj);
        EXPECT_FALSE(backupCon_->IsExtAbilityConnected());
        EXPECT_EQ(backupCon_->GetBackupExtProxy(), nullptr);

        backupCon_->backupProxy_ = nullptr;
        backupCon_->deathRecipient_ = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnExtensionDied race.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnExtensionDied_0400";
}

/**
 * @tc.number: SUB_BackupConnection_OnExtensionDied_0500
 * @tc.name: SUB_BackupConnection_OnExtensionDied_0500
 * @tc.desc: 测试 OnExtensionDied 先到等待，OnAbilityDisconnectDone 后到唤醒并处理 onDiedCb_，OnExtensionDied 被去重
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_OnExtensionDied_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_OnExtensionDied_0500";
    try {
        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->onDiedCb_ = CallDied;
        backupCon_->connState_ = SvcBackupConnection::ConnState::CONNECTED;
        backupCon_->bundleNameIndexInfo_ = "com.example.app";

        // OnExtensionDied 先到：进入 wait 等待 OnAbilityDisconnectDone
        sptr<IRemoteObject> remoteObj = sptr(new BackupExtExtensionMock);
        auto future = std::async(std::launch::async, [this, &remoteObj]() {
            backupCon_->OnExtensionDied(remoteObj);
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // OnAbilityDisconnectDone 后到：携带 resultCode 处理 onDiedCb_ 并 notify 唤醒
        AppExecFwk::ElementName element;
        element.SetBundleName("com.example.app");
        backupCon_->OnAbilityDisconnectDone(element, 1);
        EXPECT_FALSE(backupCon_->IsExtAbilityConnected());
        EXPECT_EQ(backupCon_->GetBackupExtProxy(), nullptr);
        EXPECT_FALSE(backupCon_->onDiedCb_);

        future.get();
        EXPECT_FALSE(backupCon_->IsExtAbilityConnected());
        EXPECT_EQ(backupCon_->GetBackupExtProxy(), nullptr);

        backupCon_->backupProxy_ = nullptr;
        backupCon_->deathRecipient_ = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnExtensionDied wait.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnExtensionDied_0500";
}

/**
 * @tc.number: SUB_BackupConnection_OnExtensionDied_0600
 * @tc.name: SUB_BackupConnection_OnExtensionDied_0600
 * @tc.desc: 测试 OnExtensionDied 先到清空 backupProxy_ 后，OnAbilityDisconnectDone 后到仍清空 deathRecipient_
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_OnExtensionDied_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_OnExtensionDied_0600";
    try {
        EXPECT_TRUE(backupCon_ != nullptr);
        // 先 connect 设置 backupProxy_ 和 deathRecipient_
        backupCon_->onDiedCb_ = CallDied;
        backupCon_->connState_ = SvcBackupConnection::ConnState::CONNECTING;
        backupCon_->bundleNameIndexInfo_ = "com.example.app";
        sptr<IRemoteObject> remoteObj = sptr(new BackupExtExtensionMock);
        AppExecFwk::ElementName connElement;
        connElement.SetBundleName("com.example.app");
        sptr<SvcExtensionProxyMock> svcProxy = sptr(new SvcExtensionProxyMock());
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(svcProxy));
        backupCon_->OnAbilityConnectDone(connElement, remoteObj, 0);
        EXPECT_NE(backupCon_->GetBackupExtProxy(), nullptr);
        EXPECT_NE(backupCon_->deathRecipient_, nullptr);

        // OnExtensionDied 先到：清空 backupProxy_，但 deathRecipient_ 仍非空
        backupCon_->OnExtensionDied(remoteObj);
        EXPECT_FALSE(backupCon_->IsExtAbilityConnected());
        EXPECT_EQ(backupCon_->GetBackupExtProxy(), nullptr);
        EXPECT_FALSE(backupCon_->onDiedCb_);
        EXPECT_NE(backupCon_->deathRecipient_, nullptr);

        // OnAbilityDisconnectDone 后到：backupProxy_ 已空，RemoveDeathRecipientLocked 判空跳过，deathRecipient_ 不被清空
        AppExecFwk::ElementName element;
        element.SetBundleName("com.example.app");
        backupCon_->OnAbilityDisconnectDone(element, 1);
        EXPECT_FALSE(backupCon_->IsExtAbilityConnected());
        EXPECT_EQ(backupCon_->GetBackupExtProxy(), nullptr);
        EXPECT_NE(backupCon_->deathRecipient_, nullptr);

        backupCon_->backupProxy_ = nullptr;
        backupCon_->deathRecipient_ = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnExtensionDied deathRecipient cleanup.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnExtensionDied_0600";
}

/**
 * @tc.number: SUB_BackupConnection_GetWasEverConnected_0100
 * @tc.name: SUB_BackupConnection_GetWasEverConnected_0100
 * @tc.desc: 测试 GetWasEverConnected 在连接成功前为 false，连接后为 true
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_GetWasEverConnected_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_GetWasEverConnected_0100";
    try {
        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->connState_ = SvcBackupConnection::ConnState::IDLE;
        EXPECT_FALSE(backupCon_->GetWasEverConnected());

        AppExecFwk::ElementName element;
        element.SetBundleName("com.example.app");
        sptr<IRemoteObject> remoteObj = sptr(new BackupExtExtensionMock);
        sptr<SvcExtensionProxyMock> svcProxy = sptr(new SvcExtensionProxyMock());
        EXPECT_CALL(*castMock, iface_cast(_)).WillOnce(Return(svcProxy));
        backupCon_->connState_ = SvcBackupConnection::ConnState::CONNECTING;
        backupCon_->OnAbilityConnectDone(element, remoteObj, 0);
        EXPECT_TRUE(backupCon_->GetWasEverConnected());
        EXPECT_NE(backupCon_->GetBackupExtProxy(), nullptr);

        backupCon_->backupProxy_ = nullptr;
        backupCon_->deathRecipient_ = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by GetWasEverConnected.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_GetWasEverConnected_0100";
}

/**
 * @tc.number: SUB_BackupConnection_OnAbilityConnectDone_0200
 * @tc.name: SUB_BackupConnection_OnAbilityConnectDone_0200
 * @tc.desc: 测试 OnAbilityConnectDone 状态非 CONNECTING 时直接返回，不改变状态
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_OnAbilityConnectDone_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_OnAbilityConnectDone_0200";
    try {
        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->connState_ = SvcBackupConnection::ConnState::IDLE;
        backupCon_->bundleNameIndexInfo_ = "com.example.app";
        AppExecFwk::ElementName element;
        element.SetBundleName("com.example.app");
        sptr<IRemoteObject> remoteObj = sptr(new BackupExtExtensionMock);
        backupCon_->OnAbilityConnectDone(element, remoteObj, 0);
        EXPECT_FALSE(backupCon_->IsExtAbilityConnected());
        EXPECT_FALSE(backupCon_->GetWasEverConnected());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnAbilityConnectDone state guard.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnAbilityConnectDone_0200";
}

/**
 * @tc.number: SUB_BackupConnection_OnAbilityConnectDone_0300
 * @tc.name: SUB_BackupConnection_OnAbilityConnectDone_0300
 * @tc.desc: 测试 OnAbilityConnectDone 在状态非 CONNECTING 时（死亡/断开已先行处理）直接返回，不触发回调
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_OnAbilityConnectDone_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_OnAbilityConnectDone_0300";
    try {
        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->connState_ = SvcBackupConnection::ConnState::FAILED;
        backupCon_->backupProxy_ = nullptr;
        backupCon_->onConnectedCb_ = CallDone;
        backupCon_->bundleNameIndexInfo_ = "com.example.app";
        AppExecFwk::ElementName element;
        element.SetBundleName("com.example.app");
        sptr<IRemoteObject> remoteObj = sptr(new BackupExtExtensionMock);
        backupCon_->OnAbilityConnectDone(element, remoteObj, 0);
        // 状态应保持 FAILED，不改为 CONNECTED
        EXPECT_EQ(backupCon_->connState_, SvcBackupConnection::ConnState::FAILED);
        EXPECT_FALSE(backupCon_->IsExtAbilityConnected());
        // backupProxy_ 不应被设置
        EXPECT_EQ(backupCon_->backupProxy_, nullptr);
        // onConnectedCb_ 不应被消费（仍保留原值）
        EXPECT_TRUE(backupCon_->onConnectedCb_);

        backupCon_->connState_ = SvcBackupConnection::ConnState::IDLE;
        backupCon_->onConnectedCb_ = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnAbilityConnectDone state guard.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnAbilityConnectDone_0300";
}

/**
 * @tc.number: SUB_BackupConnection_OnAbilityDisconnectDone_0300
 * @tc.name: SUB_BackupConnection_OnAbilityDisconnectDone_0300
 * @tc.desc: 测试 OnAbilityDisconnectDone 不校验 bundleName，始终处理断开并清空 onDiedCb_
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_OnAbilityDisconnectDone_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_OnAbilityDisconnectDone_0300";
    try {
        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->connState_ = SvcBackupConnection::ConnState::CONNECTED;
        backupCon_->onDiedCb_ = CallDied;
        backupCon_->bundleNameIndexInfo_ = "com.example.app";
        AppExecFwk::ElementName element;
        element.SetBundleName("wrong.bundle");
        backupCon_->OnAbilityDisconnectDone(element, 1);
        EXPECT_FALSE(backupCon_->IsExtAbilityConnected());
        EXPECT_FALSE(backupCon_->onDiedCb_);
        EXPECT_EQ(backupCon_->connState_, SvcBackupConnection::ConnState::DISCONNECTED);

        backupCon_->connState_ = SvcBackupConnection::ConnState::IDLE;
        backupCon_->onDiedCb_ = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by OnAbilityDisconnectDone.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_OnAbilityDisconnectDone_0300";
}

/**
 * @tc.number: SUB_BackupConnection_ConnectBackupExtAbility_0100
 * @tc.name: SUB_BackupConnection_ConnectBackupExtAbility_0100
 * @tc.desc: 测试 ConnectBackupExtAbility 一次性 guard，已连接过不允许重用
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_ConnectBackupExtAbility_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_ConnectBackupExtAbility_0100";
    try {
        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->connState_ = SvcBackupConnection::ConnState::CONNECTED;
        AAFwk::Want want;
        ErrCode ret = backupCon_->ConnectBackupExtAbility(want, 100, false);
        EXPECT_EQ(ret, BError(BError::Codes::SA_REFUSED_ACT).GetCode());

        backupCon_->connState_ = SvcBackupConnection::ConnState::IDLE;
        backupCon_->bundleNameIndexInfo_ = "com.example.app";
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by ConnectBackupExtAbility guard.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_ConnectBackupExtAbility_0100";
}

/**
 * @tc.number: SUB_BackupConnection_GetWasEverConnected_0200
 * @tc.name: SUB_BackupConnection_GetWasEverConnected_0200
 * @tc.desc: 测试 GetWasEverConnected 在 FAILED 状态下返回 false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(SvcBackupConnectionTest, SUB_BackupConnection_GetWasEverConnected_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-begin SUB_BackupConnection_GetWasEverConnected_0200";
    try {
        EXPECT_TRUE(backupCon_ != nullptr);
        backupCon_->connState_ = SvcBackupConnection::ConnState::FAILED;
        EXPECT_FALSE(backupCon_->GetWasEverConnected());

        backupCon_->connState_ = SvcBackupConnection::ConnState::DISCONNECTED;
        EXPECT_TRUE(backupCon_->GetWasEverConnected());

        backupCon_->connState_ = SvcBackupConnection::ConnState::IDLE;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcBackupConnectionTest-an exception occurred by GetWasEverConnected FAILED.";
    }
    GTEST_LOG_(INFO) << "SvcBackupConnectionTest-end SUB_BackupConnection_GetWasEverConnected_0200";
}
} // namespace OHOS::FileManagement::Backup