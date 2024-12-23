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
    uv_loop_s *loop = nullptr;
    napi_status status = napi_get_uv_event_loop(env_, &loop);
    if (status != napi_ok) {
        HILOGE("Failed to get uv event loop");
        ptr->cb_.CleanJsEnv();
        return;
    }

    auto work = make_unique<uv_work_t>();
    if (work == nullptr) {
        HILOGE("Failed to new uv_work_t");
        return;
    }
    work->data = static_cast<void *>(ctx_);

    auto task = [work {work.get()}]() {
        if (work == nullptr) {
            HILOGE("failed to get work.");
            return;
        }
        LibN::NAsyncContextCallback *ctx = static_cast<LibN::NAsyncContextCallback *>(work->data);
        HILOGI("BackupRestoreCallback destruct delete ctx");
        delete ctx;
        delete work;
    };
    auto ret = napi_send_event(env_, task, napi_eprio_high);
    if (ret != napi_status::napi_ok) {
        HILOGE("Failed to call napi_send_event, ret:%{public}d, status:%{public}d", ret, status);
        return;
    }
    ptr.release();
    work.release();
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
    napi_value callback = ctx->cb_.Deref(env).val_;
    if (!bool(ctx->cb_)) {
        HILOGE("Failed to get ref.");
        napi_close_handle_scope(env, scope);
        return;
    }
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
    uv_loop_s *loop = nullptr;
    napi_status status = napi_get_uv_event_loop(env_, &loop);
    if (status != napi_ok) {
        HILOGE("failed to get uv event loop.");
        return;
    }
    auto workArgs = make_shared<WorkArgs>();
    auto work = make_unique<uv_work_t>();
    if (workArgs == nullptr || work == nullptr) {
        HILOGE("failed to new workArgs or uv_work_t.");
        return;
    }
    workArgs->ptr = this;
    workArgs->argParser = argParser;
    work->data = reinterpret_cast<void *>(workArgs.get());
    HILOGI("Will execute current js method");
    auto task = [work {work.get()}]() {
        if (work == nullptr) {
            HILOGE("failed to get work.");
            return;
        }
        HILOGI("AsyncWork Enter, %{public}zu", (size_t)work);
        auto workArgs = reinterpret_cast<WorkArgs *>(work->data);
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
        delete work;
    };
    auto ret = napi_send_event(env_, task, napi_eprio_high);
    if (ret != napi_status::napi_ok) {
        HILOGE("failed to napi_send_event, ret:%{public}d.", ret);
        work.reset();
        return;
    }
    std::unique_lock<std::mutex> lock(workArgs->callbackMutex);
    HILOGI("Wait execute callback method end");
    workArgs->callbackCondition.wait(lock, [workArgs]() { return workArgs->isReady.load(); });
    work.release();
    HILOGI("call BackupRestoreCallback CallJsMethod end.");
}
} // namespace OHOS::FileManagement::Backup