/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#include "uv.h"
#include "native_reference.h"
#include "native_value.h"
#include "b_error/b_error.h"
#include "general_callbacks.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void GeneralCallbacks::RemoveCallbackRef()
{
    HILOGI("Called RemoveCallbackRef");
    onFileReady.CleanRef();
    onBundleBegin.CleanRef();
    onBundleEnd.CleanRef();
    onAllBundlesEnd.CleanRef();
    onBackupServiceDied.CleanRef();
    onResultReport.CleanRef();
    onProcess.CleanRef();
}

void BackupRestoreCallback::CleanRef()
{
    HILOGI("BackupRestoreCallback CleanRef");
    if (!ctx_) {
        HILOGE("BackupRestoreCallback ctx is nullptr");
        return;
    }
    if (!bool(ctx_->cb_)) {
        HILOGE("BackupRestoreCallback ref is nullptr");
        return;
    }
    ctx_->cb_.DeleteJsEnv();
}

BackupRestoreCallback::BackupRestoreCallback(napi_env env, LibN::NVal thisPtr, LibN::NVal cb) : env_(env)
{
    ctx_ = new LibN::NAsyncContextCallback(thisPtr, cb);
}

BackupRestoreCallback::~BackupRestoreCallback()
{
    HILOGI("BackupRestoreCallback destruct start");
    if (!ctx_) {
        HILOGE("BackupRestoreCallback ctx is nullptr");
        return;
    }

    unique_ptr<LibN::NAsyncContextCallback> ptr(ctx_);
    auto task = [](void* ptr) ->void {
        auto ctx = reinterpret_cast<LibN::NAsyncContextCallback *>(ptr);
        if (ctx == nullptr) {
            HILOGE("failed to get ctx.");
            return;
        }
        HILOGI("BackupRestoreCallback destruct delete ctx");
        if (bool(ctx->cb_)) {
            ctx->cb_.CleanJsEnv();
        }
        delete ctx;
    };
    uint64_t handleId = 0;
    auto ret = napi_send_cancelable_event(env_, task, ptr.get(), napi_eprio_high, &handleId, "destructor");
    if (ret != napi_status::napi_ok) {
        HILOGE("failed to napi_send_cancelable_event, ret:%{public}d, name:%{public}s.", ret, "destructor");
        if (bool(ctx_->cb_)) {
            ctx_->cb_.CleanJsEnv();
        }
        return;
    }
    ptr.release();
    ctx_ = nullptr;
    HILOGI("BackupRestoreCallback destruct end");
}

BackupRestoreCallback::operator bool() const
{
    return bool(ctx_->cb_);
}

static void DoCallJsMethod(napi_env env, void *data, InputArgsParser argParser)
{
    HILOGI("Start execute DoCallJsMethod");
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        HILOGE("scope is nullptr");
        return;
    }
    auto ctx = static_cast<LibN::NAsyncContextCallback *>(data);
    if (ctx == nullptr) {
        HILOGE("This pointer address is empty");
        napi_close_handle_scope(env, scope);
        return;
    }
    vector<napi_value> argv = {};
    if (argParser != nullptr) {
        if (!argParser(env, argv)) {
            HILOGE("failed to get params.");
            napi_close_handle_scope(env, scope);
            return;
        }
    }
    napi_value global = nullptr;
    napi_get_global(env, &global);
    if (!bool(ctx->cb_)) {
        HILOGE("Failed to get ref.");
        napi_close_handle_scope(env, scope);
        return;
    }
    napi_value callback = ctx->cb_.Deref(env).val_;
    napi_value result = nullptr;
    napi_status status = napi_call_function(env, global, callback, argv.size(), argv.data(), &result);
    if (status != napi_ok) {
        HILOGE("Failed to call function for %{public}d.", status);
    }
    napi_close_handle_scope(env, scope);
    HILOGI("End execute DoCallJsMethod");
}

void BackupRestoreCallback::CallJsMethod(InputArgsParser argParser)
{
    HILOGI("call BackupRestoreCallback CallJsMethod begin.");
    auto workArgs = make_shared<WorkArgs>();
    if (workArgs == nullptr) {
        HILOGE("failed to new workArgs or uv_work_t.");
        return;
    }
    workArgs->ptr = this;
    workArgs->argParser = argParser;
    HILOGI("Will execute current js method");
    auto task = [](void* ptr) ->void {
        auto workArgs = reinterpret_cast<WorkArgs *>(ptr);
        do {
            if (workArgs == nullptr) {
                HILOGE("failed to get workArgs.");
                break;
            }
            DoCallJsMethod(workArgs->ptr->env_, workArgs->ptr->ctx_, workArgs->argParser);
        } while (false);
        HILOGI("will notify current thread info");
        std::unique_lock<std::mutex> lock(workArgs->callbackMutex);
        workArgs->isReady.store(true);
        workArgs->callbackCondition.notify_all();
    };
    uint64_t handleId = 0;
    auto ret = napi_send_cancelable_event(env_, task, workArgs.get(), napi_eprio_high, &handleId, "jsmethod");
    if (ret != napi_status::napi_ok) {
        HILOGE("failed to napi_send_cancelable_event, ret:%{public}d, name:%{public}s.", ret, "jsmethod");
        return;
    }
    std::unique_lock<std::mutex> lock(workArgs->callbackMutex);
    HILOGI("Wait execute callback method end");
    workArgs->callbackCondition.wait(lock, [workArgs]() { return workArgs->isReady.load(); });
    HILOGI("call BackupRestoreCallback CallJsMethod end.");
}
} // namespace OHOS::FileManagement::Backup