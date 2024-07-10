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

#include <gtest/gtest.h>
#include <string>

#include "module_ipc/service.h"
#include "svc_session_manager_throw_mock.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

constexpr int32_t SERVICE_ID = 5203;

class ServiceThrowTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() {};
    void TearDown() {};

    static inline sptr<Service> service = nullptr;
    static inline shared_ptr<SvcSessionManagerMock> sessionMock = nullptr;
};

void ServiceThrowTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    service = sptr<Service>(new Service(SERVICE_ID));
    sessionMock = make_shared<SvcSessionManagerMock>();
    SvcSessionManagerMock::session = sessionMock;
}

void ServiceThrowTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    service = nullptr;
    SvcSessionManagerMock::session = nullptr;
    sessionMock = nullptr;
}

/**
 * @tc.number: SUB_Service_throw_GetLocalCapabilities_0100
 * @tc.name: SUB_Service_throw_GetLocalCapabilities_0100
 * @tc.desc: 测试 GetLocalCapabilities 接口的 catch 分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesIAC04T
 */
HWTEST_F(ServiceThrowTest, SUB_Service_throw_GetLocalCapabilities_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceThrowTest-begin SUB_Service_throw_GetLocalCapabilities_0100";
    try {
        EXPECT_NE(service, nullptr);
        EXPECT_CALL(*sessionMock, IncreaseSessionCnt()).WillOnce(Invoke([]() {
            throw BError(BError::Codes::EXT_THROW_EXCEPTION);
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt()).WillOnce(Return());
        auto ret = service->GetLocalCapabilities();
        EXPECT_EQ(-ret, BError(BError::Codes::EXT_THROW_EXCEPTION).GetCode());

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt()).WillOnce(Invoke([]() {
            throw runtime_error("运行时错误");
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt()).WillOnce(Return());
        ret = service->GetLocalCapabilities();
        EXPECT_EQ(-ret, EPERM);

        EXPECT_CALL(*sessionMock, IncreaseSessionCnt()).WillOnce(Invoke([]() {
            throw "未知错误";
        }));
        EXPECT_CALL(*sessionMock, DecreaseSessionCnt()).WillOnce(Return());
        ret = service->GetLocalCapabilities();
        EXPECT_EQ(-ret, EPERM);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceThrowTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "ServiceThrowTest-end SUB_Service_throw_GetLocalCapabilities_0100";
}
} // namespace OHOS::FileManagement::Backup