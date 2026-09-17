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

#include <condition_variable>
#include <cstdio>

#include <gtest/gtest.h>

#include "iservice_registry.h"
#include "service_reverse_mock.h"
#include "svc_death_recipient.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

const string FILE_NAME = "temp.json";
static condition_variable g_cv;
static atomic<bool> g_serviceDie = false;

class SvcDeathRecipientTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};

public:
    mutex lock_;
};

static void CallBack(const wptr<IRemoteObject> &obj)
{
    g_serviceDie.store(true);
    g_cv.notify_all();
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-CallBack success";
}

/**
 * @tc.number: SUB_backup_sa_deathecipient_OnRemoteDied_0100
 * @tc.name: SUB_backup_sa_deathecipient_OnRemoteDied_0100
 * @tc.desc: 测试 OnRemoteDied 接口调用成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcDeathRecipientTest, SUB_backup_sa_deathecipient_OnRemoteDied_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_deathecipient_OnRemoteDied_0100";
    sptr<ServiceReverseMock> remote = sptr(new ServiceReverseMock());
    sptr<SvcDeathRecipient> deathRecipient = sptr(new SvcDeathRecipient(CallBack));
    remote->AddDeathRecipient(deathRecipient);
    deathRecipient->OnRemoteDied(remote);
    remote = nullptr;
    deathRecipient = nullptr;
    unique_lock<mutex> lk(lock_);
    g_cv.wait(lk, [&] { return true; });
    ASSERT_TRUE(g_serviceDie);
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_deathecipient_OnRemoteDied_0100";
}

/**
 * @tc.number: SUB_backup_sa_deathecipient_OnRemoteDied_0200
 * @tc.name: SUB_backup_sa_deathecipient_OnRemoteDied_0200
 * @tc.desc: 测试 OnRemoteDied 在底层对象已销毁(promote 返回空)时不崩溃，且回调仍被触发
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcDeathRecipientTest, SUB_backup_sa_deathecipient_OnRemoteDied_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_deathecipient_OnRemoteDied_0200";
    g_serviceDie.store(false);
    sptr<SvcDeathRecipient> deathRecipient = sptr(new SvcDeathRecipient(CallBack));
    wptr<IRemoteObject> deadObj = nullptr;
    {
        sptr<ServiceReverseMock> remote = sptr(new ServiceReverseMock());
        deadObj = wptr<IRemoteObject>(remote->AsObject());
        // 不 AddDeathRecipient，避免销毁 remote 时自动触发 OnRemoteDied
        remote = nullptr;
    }
    // 此时 deadObj.promote() 返回 nullptr，验证不发生空指针解引用
    deathRecipient->OnRemoteDied(deadObj);
    ASSERT_TRUE(g_serviceDie);
    deathRecipient = nullptr;
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_deathecipient_OnRemoteDied_0200";
}
} // namespace OHOS::FileManagement::Backup