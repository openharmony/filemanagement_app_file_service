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

#include <future>
#include <string>

#include <unistd.h>

#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "module_ipc/service.h"
#include "module_ipc/svc_session_manager.h"
#include "module_sched/sched_scheduler.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
constexpr int32_t CLIENT_TOKEN_ID = 100;
constexpr int32_t SERVICE_ID = 5203;
constexpr int32_t WAIT_TIME = 3;
} // namespace

class SchedSchedulerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};
    void Init(IServiceReverse::Scenario scenario);

    static inline sptr<SchedScheduler> schedPtr_ = nullptr;
    static inline sptr<SvcSessionManager> sessionManagerPtr_ = nullptr;
    static inline sptr<Service> servicePtr_ = nullptr;
};

void SchedSchedulerTest::SetUpTestCase()
{
    servicePtr_ = sptr<Service>(new Service(SERVICE_ID));
    sessionManagerPtr_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(servicePtr_)));
    schedPtr_ = sptr(new SchedScheduler(wptr(servicePtr_), wptr(sessionManagerPtr_)));
}

void SchedSchedulerTest::TearDownTestCase()
{
    schedPtr_ = nullptr;
    servicePtr_ = nullptr;
    sessionManagerPtr_ = nullptr;
}

void SchedSchedulerTest::Init(IServiceReverse::Scenario scenario)
{
    vector<string> bundleNames;
    map<string, BackupExtInfo> backupExtNameMap;
    bundleNames.emplace_back(BUNDLE_NAME);
    auto setBackupExtNameMap = [](const string &bundleName) {
        BackupExtInfo info {};
        info.backupExtName = BUNDLE_NAME;
        info.receExtManageJson = true;
        info.receExtAppDone = true;
        return make_pair(bundleName, info);
    };
    transform(bundleNames.begin(), bundleNames.end(), inserter(backupExtNameMap, backupExtNameMap.end()),
              setBackupExtNameMap);

    sessionManagerPtr_->Active({
        .clientToken = CLIENT_TOKEN_ID,
        .scenario = scenario,
        .backupExtNameMap = move(backupExtNameMap),
    });
}

/**
 * @tc.number: SUB_Service_Sched_0100
 * @tc.name: SUB_Service_Sched_0100
 * @tc.desc: 测试 Sched接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SchedSchedulerTest, SUB_Service_Sched_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SchedSchedulerTest-begin SUB_Service_Sched_0100";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        schedPtr_->Sched();
        GTEST_LOG_(INFO) << "SchedSchedulerTest-Sched Branches";
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::FINISH);
        schedPtr_->Sched();
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::WAIT);
        schedPtr_->Sched();
        GTEST_LOG_(INFO) << "SchedSchedulerTest-ExecutingQueueTasks time callback";
        schedPtr_->RemoveExtConn(BUNDLE_NAME);
        schedPtr_->Sched("test");
        // SchedScheduler在析构时释放OHOS::ThreadPool若此时没有完成的任务，ThreadPool在析构调用stop时会出现异常，目前暂时sleep处理，后续更改ThreadPool方案
        // 原因是sleep等待ThreadPool任务完成后，任务不再额外持有SchedScheduler的引用计数
        // 于是本函数执行结束后SchedScheduler引用计数清空，得以在另外的线程上析构SchedScheduler对象及其中的线程池。
        sleep(WAIT_TIME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SchedSchedulerTest-an exception occurred by Sched.";
    }
    GTEST_LOG_(INFO) << "SchedSchedulerTest-end SUB_Service_Sched_0100";
}

/**
 * @tc.number: SUB_Service_ExecutingQueueTasks_0100
 * @tc.name: SUB_Service_ExecutingQueueTasks_0100
 * @tc.desc: 测试 ExecutingQueueTasks 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SchedSchedulerTest, SUB_Service_ExecutingQueueTasks_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SchedSchedulerTest-begin SUB_Service_ExecutingQueueTasks_0100";
    try {
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        schedPtr_->ExecutingQueueTasks(BUNDLE_NAME);
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::RUNNING);
        schedPtr_->ExecutingQueueTasks(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SchedSchedulerTest-an exception occurred by ExecutingQueueTasks.";
    }
    GTEST_LOG_(INFO) << "SchedSchedulerTest-end SUB_Service_ExecutingQueueTasks_0100";
}

/**
 * @tc.number: SUB_Service_RemoveExtConn_0100
 * @tc.name: SUB_Service_RemoveExtConn_0100
 * @tc.desc: 测试 RemoveExtConn 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SchedSchedulerTest, SUB_Service_RemoveExtConn_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SchedSchedulerTest-begin SUB_Service_RemoveExtConn_0100";
    try {
        schedPtr_->RemoveExtConn("test");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SchedSchedulerTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "SchedSchedulerTest-end SUB_Service_RemoveExtConn_0100";
}
} // namespace OHOS::FileManagement::Backup