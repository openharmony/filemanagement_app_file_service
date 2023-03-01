/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <gtest/gtest.h>

#include "iservice_registry.h"
#include "service_reverse_mock.h"
#include "svc_death_recipient.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

const string FILE_NAME = "temp.json";

class SvcDeathRecipientTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

static void CallBack(const wptr<IRemoteObject> &obj)
{
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
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_deathecipient_OnRemoteDied_0100";
}
} // namespace OHOS::FileManagement::Backup