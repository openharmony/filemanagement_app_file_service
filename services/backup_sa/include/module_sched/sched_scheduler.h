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

#ifndef OHOS_FILEMGMT_BACKUP_SCHED_SCHEDULER_H
#define OHOS_FILEMGMT_BACKUP_SCHED_SCHEDULER_H

#include <cstdint>
#include <refbase.h>
#include <set>
#include <shared_mutex>
#include <string>
#include <vector>

#include "b_resources/b_constants.h"
#include "iremote_broker.h"
#include "thread_pool.h"
#include "timer.h"

namespace OHOS::FileManagement::Backup {
class Service;
class SvcSessionManager;

class SchedScheduler final : public virtual RefBase {
public:
    /**
     * @brief 给threadPool下发任务
     *
     */
    void Sched(std::string bundleName = "");

    /**
     * @brief 执行队列任务
     *
     * @param bundleName
     */
    void ExecutingQueueTasks(const std::string &bundleName);

    /**
     * @brief 移除定时器信息
     *
     * @param bundleName 应用名称
     */
    void RemoveExtConn(const std::string &bundleName);

    /**
     * @brief install success
     *
     * @param bundleName
     * @param resultCode
     */
    void InstallSuccess(const std::string &bundleName, const int32_t resultCode);

    /**
     * @brief try unload service timer
     *
     */
    void TryUnloadServiceTimer(bool force = false);

    /**
     * @brief clear scheduler data
     *
     */
    void ClearSchedulerData();

    /**
     * @brief unload service
     *
     */
    void TryUnloadService();

    void StartTimer()
    {
        extTime_.Setup();
        TryUnloadServiceTimer();
    }

public:
    explicit SchedScheduler(wptr<Service> reversePtr, wptr<SvcSessionManager> sessionPtr)
        : reversePtr_(reversePtr), sessionPtr_(sessionPtr)
    {
        threadPool_.Start(BConstants::SA_THREAD_POOL_COUNT);
    }

    ~SchedScheduler() override
    {
        extTime_.Shutdown();
        reversePtr_ = nullptr;
        sessionPtr_ = nullptr;
        bundleTimeVec_.clear();
    }

private:
    /**
     * @brief install state
     *
     * @param bundleName
     */
    void InstallingState(const std::string &bundleName);

private:
    mutable std::shared_mutex lock_;
    OHOS::ThreadPool threadPool_;

    // 注册心跳信息
    std::vector<std::tuple<std::string, uint32_t>> bundleTimeVec_;

    wptr<Service> reversePtr_;
    wptr<SvcSessionManager> sessionPtr_;
    Utils::Timer extTime_ {"backupSchedTimer"};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SCHED_SCHEDULER_H
