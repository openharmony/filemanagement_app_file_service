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

#include "module_sched/sched_scheduler.h"

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <tuple>
#include <utility>

#include <sys/stat.h>

#include <directory_ex.h>
#include <unique_fd.h>

#include "b_error/b_error.h"
#include "b_ohos/startup/backup_para.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "module_external/bms_adapter.h"
#include "module_ipc/service.h"
#include "module_ipc/svc_session_manager.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void SchedScheduler::Sched(string bundleName)
{
    if (bundleName == "") {
        if (!sessionPtr_->GetSchedBundleName(bundleName)) {
            return;
        }
    }
    HILOGE("Sched bundleName %{public}s", bundleName.data());
    auto callStart = [schedPtr {wptr(this)}, bundleName]() {
        try {
            auto ptr = schedPtr.promote();
            ptr->ExecutingQueueTasks(bundleName);
        } catch (const BError &e) {
            HILOGE("%{public}s", e.what());
        } catch (const exception &e) {
            HILOGE("%{public}s", e.what());
        } catch (...) {
            HILOGE("");
        }
    };
    threadPool_.AddTask(callStart);
}

void SchedScheduler::ExecutingQueueTasks(const string &bundleName)
{
    HILOGE("start");
    BConstants::ServiceSchedAction action = sessionPtr_->GetServiceSchedAction(bundleName);
    if (action == BConstants::ServiceSchedAction::START) {
        // 注册启动定时器
        auto callStart = [reversePtr {reversePtr_}, bundleName]() {
            HILOGE("Extension connect failed = %{public}s", bundleName.data());
            auto ptr = reversePtr.promote();
            if (ptr) {
                ptr->ExtConnectFailed(bundleName, BError(BError::Codes::SA_BOOT_TIMEOUT));
            }
        };
        auto iTime = extTime_.Register(callStart, BConstants::EXT_CONNECT_MAX_TIME);
        unique_lock<shared_mutex> lock(lock_);
        bundleTimeVec_.emplace_back(make_tuple(bundleName, iTime));
        lock.unlock();
        // 启动extension
        reversePtr_->LaunchBackupExtension(bundleName);
    } else if (action == BConstants::ServiceSchedAction::RUNNING) {
        unique_lock<shared_mutex> lock(lock_);
        auto iter = find_if(bundleTimeVec_.begin(), bundleTimeVec_.end(), [&bundleName](auto &obj) {
            auto &[bName, iTime] = obj;
            return bName == bundleName;
        });
        if (iter == bundleTimeVec_.end()) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Failed to find timer");
        }
        auto &[bName, iTime] = *iter;
        // 移除启动定时器 当前逻辑无启动成功后的ext心跳检测
        extTime_.Unregister(iTime);
        lock.unlock();
        // 开始执行备份恢复流程
        reversePtr_->ExtStart(bundleName);
    }
}

void SchedScheduler::RemoveExtConn(const string &bundleName)
{
    unique_lock<shared_mutex> lock(lock_);
    auto iter = find_if(bundleTimeVec_.begin(), bundleTimeVec_.end(), [&bundleName](auto &obj) {
        auto &[bName, iTime] = obj;
        return bName == bundleName;
    });
    if (iter != bundleTimeVec_.end()) {
        auto &[bName, iTime] = *iter;
        HILOGE("bundleName = %{public}s , iTime = %{public}d", bName.data(), iTime);
        extTime_.Unregister(iTime);
        bundleTimeVec_.erase(iter);
    }
}

void SchedScheduler::StartTimer()
{
    extTime_.Setup();
    if (!BackupPara().GetBackupOverrideBackupSARelease()) {
        TryUnloadServiceTimer();
    }
}

void SchedScheduler::TryUnloadServiceTimer(bool force)
{
    auto tryUnload = [sessionPtr {sessionPtr_}]() {
        auto ptr = sessionPtr.promote();
        if (ptr && !ptr->NeedToUnloadService()) {
            return;
        }
        HILOGI("Unload system ability");
        sptr<ISystemAbilityManager> saManager =
            OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (saManager == nullptr) {
            HILOGE("UnloadSA, GetSystemAbilityManager is null.");
            return;
        }
        int32_t result = saManager->UnloadSystemAbility(FILEMANAGEMENT_BACKUP_SERVICE_SA_ID);
        if (result != ERR_OK) {
            HILOGE("UnloadSA, UnloadSystemAbility result: %{public}d", result);
            return;
        }
    };

    static atomic<uint32_t> timerEventId;
    // When force is false, only one timer is allowed to be registered.
    if (!force) {
        if (timerEventId != 0) {
            return;
        }
        constexpr int checkingIntervalInMs = 30000;
        auto tmpTimerEventId = extTime_.Register(tryUnload, checkingIntervalInMs);
        uint32_t tmp = 0;
        if (timerEventId.compare_exchange_strong(tmp, tmpTimerEventId)) {
            return;
        }
        extTime_.Unregister(tmpTimerEventId);
    }

    tryUnload();
}

void SchedScheduler::TryUnloadService()
{
    HILOGI("Unload system ability");
    sptr<ISystemAbilityManager> saManager = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        HILOGE("UnloadSA, GetSystemAbilityManager is null.");
        return;
    }
    int32_t result = saManager->UnloadSystemAbility(FILEMANAGEMENT_BACKUP_SERVICE_SA_ID);
    if (result != ERR_OK) {
        HILOGE("UnloadSA, UnloadSystemAbility result: %{public}d", result);
        return;
    }
}

void SchedScheduler::ClearSchedulerData()
{
    unique_lock<shared_mutex> lock(lock_);
    for (auto &bundleTime : bundleTimeVec_) {
        auto &[bName, iTime] = bundleTime;
        extTime_.Unregister(iTime);
    }
    bundleTimeVec_.clear();
}
}; // namespace OHOS::FileManagement::Backup
