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
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "module_external/bms_adapter.h"
#include "module_external/inner_receiver_impl.h"
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
        BConstants::ServiceSchedAction action = sessionPtr_->GetServiceSchedAction(bundleName);
        if (action == BConstants::ServiceSchedAction::WAIT) {
            sessionPtr_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::INSTALLING);
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
    InstallingState(bundleName);
    BConstants::ServiceSchedAction action = sessionPtr_->GetServiceSchedAction(bundleName);
    if (action == BConstants::ServiceSchedAction::START) {
        // 注册启动定时器
        auto callStart = [reversePtr {reversePtr_}, bundleName]() {
            HILOGE("Extension connect failed = %{public}s", bundleName.data());
            auto ptr = reversePtr.promote();
            if (ptr) {
                ptr->ExtConnectFailed(bundleName, EPERM);
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

static bool GetRealPath(string &path)
{
    unique_ptr<char[]> absPath = make_unique<char[]>(PATH_MAX + 1);
    if (realpath(path.c_str(), absPath.get()) == nullptr) {
        return false;
    }
    path = absPath.get();
    return true;
}

void SchedScheduler::InstallingState(const string &bundleName)
{
    BConstants::ServiceSchedAction action = sessionPtr_->GetServiceSchedAction(bundleName);
    if (action == BConstants::ServiceSchedAction::INSTALLING) {
        IServiceReverse::Scenario scenario = sessionPtr_->GetScenario();
        if (scenario == IServiceReverse::Scenario::BACKUP || !sessionPtr_->GetNeedToInstall(bundleName)) {
            sessionPtr_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::START);
            return;
        }
        string state = sessionPtr_->GetInstallState(bundleName);
        string path = string(BConstants::SA_BUNDLE_BACKUP_ROOT_DIR).append(bundleName);
        string filePath = path + "/bundle.hap";
        if (!GetRealPath(filePath)) {
            throw BError(BError::Codes::SA_INVAL_ARG, string("File path is invalid"));
        }

        if (state == BConstants::RESTORE_INSTALL_PATH) {
            if (!ForceCreateDirectory(path)) {
                throw BError(BError::Codes::SA_INVAL_ARG, string("Failed to create directory"));
            }
            sessionPtr_->GetServiceReverseProxy()->RestoreOnFileReady(
                bundleName, state,
                UniqueFd(open(filePath.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IROTH)));
        } else if (state == "OK") {
            if (access(filePath.data(), F_OK) != 0) {
                throw BError(BError::Codes::SA_INVAL_ARG, string("File already exists"));
            }
            sptr<InnerReceiverImpl> statusReceiver = sptr(new InnerReceiverImpl(bundleName, wptr(this)));
            ErrCode err = BundleMgrAdapter::Install(statusReceiver, filePath);
            if (err != ERR_OK) {
                InstallSuccess(bundleName, err);
            }
        }
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

    // When force is true, the timer is unregistered immediately and then try to unload the service.
    if (auto tmp = timerEventId.exchange(0); tmp != 0) {
        extTime_.Unregister(tmp);
    }
    tryUnload();
}

void SchedScheduler::InstallSuccess(const std::string &bundleName, const int32_t resultCode)
{
    if (!resultCode) {
        sessionPtr_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::START);
        Sched(bundleName);
    } else {
        sessionPtr_->GetServiceReverseProxy()->RestoreOnBundleStarted(resultCode, bundleName);
        sessionPtr_->RemoveExtInfo(bundleName);
    }
    string path = string(BConstants::SA_BUNDLE_BACKUP_ROOT_DIR).append(bundleName);
    if (!ForceRemoveDirectory(path)) {
        HILOGE("RemoveDirectory failed");
    }
}
}; // namespace OHOS::FileManagement::Backup
