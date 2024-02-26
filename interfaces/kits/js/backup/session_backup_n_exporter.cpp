/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "session_backup_n_exporter.h"

#include <functional>
#include <memory>

#include "b_error/b_error.h"
#include "b_filesystem/b_file.h"
#include "b_resources/b_constants.h"
#include "b_session_backup.h"
#include "backup_kit_inner.h"
#include "directory_ex.h"
#include "filemgmt_libhilog.h"
#include "general_callbacks.h"
#include "service_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace LibN;

struct BackupEntity {
    unique_ptr<BSessionBackup> session;
    shared_ptr<GeneralCallbacks> callbacks;
};

static void OnFileReady(weak_ptr<GeneralCallbacks> pCallbacks, const BFileInfo &fileInfo, UniqueFd fd)
{
    if (pCallbacks.expired()) {
        HILOGI("callbacks is unbound");
        return;
    }
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("callback function onFileReady has already been released");
        return;
    }
    if (!bool(callbacks->onFileReady)) {
        HILOGI("callback function onFileReady is undefined");
        return;
    }

    auto cbCompl = [bundleName {fileInfo.owner}, fileName {fileInfo.fileName},
                    fd {make_shared<UniqueFd>(fd.Release())}](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        NVal obj = NVal::CreateObject(env);
        obj.AddProp({
            NVal::DeclareNapiProperty(BConstants::BUNDLE_NAME.c_str(), NVal::CreateUTF8String(env, bundleName).val_),
            NVal::DeclareNapiProperty(BConstants::URI.c_str(), NVal::CreateUTF8String(env, fileName).val_),
            NVal::DeclareNapiProperty(BConstants::FD.c_str(), NVal::CreateInt32(env, fd->Release()).val_)});

        return {obj};
    };

    callbacks->onFileReady.ThreadSafeSchedule(cbCompl);
}

static void onBundleBegin(weak_ptr<GeneralCallbacks> pCallbacks, ErrCode err, const BundleName name)
{
    if (pCallbacks.expired()) {
        HILOGI("callbacks is unbound");
        return;
    }
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("callback function onBundleBegin has already been released");
        return;
    }
    if (!bool(callbacks->onBundleBegin)) {
        HILOGI("callback function onBundleBegin is undefined");
        return;
    }

    auto cbCompl = [name {name}, errCode {err}](napi_env env, NError err) -> NVal {
        NVal bundleName = NVal::CreateUTF8String(env, name);
        if (!err && errCode == 0) {
            return bundleName;
        }
        
        NVal res;
        if (err) {
            res = NVal {env, err.GetNapiErr(env)};
        } else {
            res = NVal {env, NError(errCode).GetNapiErr(env)};
        }
        napi_status status = napi_set_named_property(env, res.val_, FILEIO_TAG_ERR_DATA.c_str(), bundleName.val_);
        if (status != napi_ok) {
            HILOGE("Failed to set data property, status %{public}d, bundleName %{public}s", status, name.c_str());
        }

        return res;
    };

    callbacks->onBundleBegin.ThreadSafeSchedule(cbCompl);
}

static void onBundleEnd(weak_ptr<GeneralCallbacks> pCallbacks, ErrCode err, const BundleName name)
{
    if (pCallbacks.expired()) {
        HILOGI("callbacks is unbound");
        return;
    }
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("callback function onBundleEnd has already been released");
        return;
    }
    if (!bool(callbacks->onBundleEnd)) {
        HILOGI("callback function onBundleEnd is undefined");
        return;
    }

    auto cbCompl = [name {name}, errCode {err}](napi_env env, NError err) -> NVal {
        NVal bundleName = NVal::CreateUTF8String(env, name);
        if (!err && errCode == 0) {
            return bundleName;
        }
        
        NVal res;
        if (err) {
            res = NVal {env, err.GetNapiErr(env)};
        } else {
            res = NVal {env, NError(errCode).GetNapiErr(env)};
        }
        napi_status status = napi_set_named_property(env, res.val_, FILEIO_TAG_ERR_DATA.c_str(), bundleName.val_);
        if (status != napi_ok) {
            HILOGE("Failed to set data property, status %{public}d, bundleName %{public}s", status, name.c_str());
        }

        return res;
    };

    callbacks->onBundleEnd.ThreadSafeSchedule(cbCompl);
}

static void onAllBundlesEnd(weak_ptr<GeneralCallbacks> pCallbacks, ErrCode err)
{
    if (pCallbacks.expired()) {
        HILOGI("callbacks is unbound");
        return;
    }
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("callback function onAllBundlesEnd has already been released");
        return;
    }
    if (!bool(callbacks->onAllBundlesEnd)) {
        HILOGI("callback function onAllBundlesEnd is undefined");
        return;
    }

    auto cbCompl = [errCode {err}](napi_env env, NError err) -> NVal {
        if (!err && errCode == 0) {
            return NVal::CreateUndefined(env);
        }

        NVal res;
        if (err) {
            res = NVal {env, err.GetNapiErr(env)};
        } else {
            res = NVal {env, NError(errCode).GetNapiErr(env)};
        }

        return res;
    };

    callbacks->onAllBundlesEnd.ThreadSafeSchedule(cbCompl);
}

static void OnBackupServiceDied(weak_ptr<GeneralCallbacks> pCallbacks)
{
    if (pCallbacks.expired()) {
        HILOGI("callbacks is unbound");
        return;
    }
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("js callback function onBackupServiceDied has already been released");
        return;
    }
    if (!bool(callbacks->onBackupServiceDied)) {
        HILOGI("callback function onBackupServiceDied is undefined");
        return;
    }

    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal {env, err.GetNapiErr(env)} : NVal::CreateUndefined(env);
    };

    callbacks->onBackupServiceDied.ThreadSafeSchedule(cbCompl);
}

napi_value SessionBackupNExporter::Constructor(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("called SessionBackup::Constructor begin");
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    NVal callbacks(env, funcArg[NARG_POS::FIRST]);
    if (!callbacks.TypeIs(napi_object)) {
        HILOGE("First argument is not an object.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "First argument is not an object.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    NVal ptr(env, funcArg.GetThisVar());
    auto backupEntity = std::make_unique<BackupEntity>();
    backupEntity->callbacks = make_shared<GeneralCallbacks>(env, ptr, callbacks);
    backupEntity->session = BSessionBackup::Init(BSessionBackup::Callbacks {
        .onFileReady = bind(OnFileReady, backupEntity->callbacks, placeholders::_1, placeholders::_2),
        .onBundleStarted = bind(onBundleBegin, backupEntity->callbacks, placeholders::_1, placeholders::_2),
        .onBundleFinished = bind(onBundleEnd, backupEntity->callbacks, placeholders::_1, placeholders::_2),
        .onAllBundlesFinished = bind(onAllBundlesEnd, backupEntity->callbacks, placeholders::_1),
        .onBackupServiceDied = bind(OnBackupServiceDied, backupEntity->callbacks)});
    if (!backupEntity->session) {
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to init backup").GetCode()).ThrowErr(env);
        return nullptr;
    }
    if (!NClass::SetEntityFor<BackupEntity>(env, funcArg.GetThisVar(), move(backupEntity))) {
        HILOGE("Failed to set BackupEntity entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to set BackupEntity entity").GetCode()).ThrowErr(env);
        return nullptr;
    }

    HILOGI("called SessionBackup::Constructor end");
    return funcArg.GetThisVar();
}

napi_value SessionBackupNExporter::AppendBundles(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("called SessionBackup::AppendBundles begin");
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    NVal jsBundles(env, funcArg[NARG_POS::FIRST]);
    auto [succ, bundles, ignore] = jsBundles.ToStringArray();
    if (!succ) {
        HILOGE("First argument is not bundles array.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "First argument is not bundles array.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto backupEntity = NClass::GetEntityOf<BackupEntity>(env, funcArg.GetThisVar());
    if (!(backupEntity && backupEntity->session)) {
        HILOGE("Failed to get backupSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get backupSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = [session {backupEntity->session.get()}, bundles {bundles}]() -> NError {
        if (!session) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "backup session is nullptr").GetCode());
        }
        return NError(session->AppendBundles(bundles));
    };
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal {env, err.GetNapiErr(env)} : NVal::CreateUndefined(env);
    };

    HILOGI("Called SessionBackup::AppendBundles end.");

    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
    } else {
        NVal cb(env, funcArg[NARG_POS::SECOND]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(className, cbExec, cbCompl).val_;
    }
}

napi_value SessionBackupNExporter::Release(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("called SessionBackup::Release begin");
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto backupEntity = NClass::GetEntityOf<BackupEntity>(env, funcArg.GetThisVar());
    if (!(backupEntity && backupEntity->session)) {
        HILOGE("Failed to get backupSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get backupSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = [session {backupEntity->session.get()}]() -> NError {
        if (!session) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "backup session is nullptr").GetCode());
        }
        return NError(session->Release());
    };
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal {env, err.GetNapiErr(env)} : NVal::CreateUndefined(env);
    };

    HILOGI("Called SessionBackup::Release end.");

    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
}

bool SessionBackupNExporter::Export()
{
    HILOGI("called SessionBackupNExporter::Export begin");
    vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("appendBundles", AppendBundles),
        NVal::DeclareNapiFunction("release", Release),
    };

    auto [succ, classValue] = NClass::DefineClass(exports_.env_, className, Constructor, std::move(props));
    if (!succ) {
        HILOGE("Failed to define class");
        NError(EIO).ThrowErr(exports_.env_);
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, classValue);
    if (!succ) {
        HILOGE("Failed to save class");
        NError(EIO).ThrowErr(exports_.env_);
        return false;
    }

    HILOGI("called SessionBackupNExporter::Export end");
    return exports_.AddProp(className, classValue);
}

string SessionBackupNExporter::GetClassName()
{
    return SessionBackupNExporter::className;
}

SessionBackupNExporter::SessionBackupNExporter(napi_env env, napi_value exports) : NExporter(env, exports) {}

SessionBackupNExporter::~SessionBackupNExporter() {}
} // namespace OHOS::FileManagement::Backup