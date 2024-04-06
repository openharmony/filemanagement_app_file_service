/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "session_restore_n_exporter.h"

#include <functional>
#include <memory>

#include "b_error/b_error.h"
#include "b_filesystem/b_dir.h"
#include "b_filesystem/b_file.h"
#include "b_incremental_restore_session.h"
#include "b_ohos/startup/backup_para.h"
#include "b_resources/b_constants.h"
#include "b_session_restore.h"
#include "backup_kit_inner.h"
#include "filemgmt_libhilog.h"
#include "general_callbacks.h"
#include "service_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace LibN;

struct RestoreEntity {
    unique_ptr<BSessionRestore> sessionWhole;
    unique_ptr<BIncrementalRestoreSession> sessionSheet;
    shared_ptr<GeneralCallbacks> callbacks;
};

static void OnFileReadyWhole(weak_ptr<GeneralCallbacks> pCallbacks, const BFileInfo &fileInfo, UniqueFd fd)
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

static void OnFileReadySheet(weak_ptr<GeneralCallbacks> pCallbacks,
                             const BFileInfo &fileInfo,
                             UniqueFd fd,
                             UniqueFd manifestFd)
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

    auto cbCompl = [bundleName {fileInfo.owner},
                    fileName {fileInfo.fileName},
                    fd {make_shared<UniqueFd>(fd.Release())},
                    manifestFd {make_shared<UniqueFd>(manifestFd.Release())}](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        NVal obj = NVal::CreateObject(env);
        obj.AddProp({
            NVal::DeclareNapiProperty(BConstants::BUNDLE_NAME.c_str(), NVal::CreateUTF8String(env, bundleName).val_),
            NVal::DeclareNapiProperty(BConstants::URI.c_str(), NVal::CreateUTF8String(env, fileName).val_),
            NVal::DeclareNapiProperty(BConstants::FD.c_str(), NVal::CreateInt32(env, fd->Release()).val_),
            NVal::DeclareNapiProperty(BConstants::MANIFEST_FD.c_str(),
                NVal::CreateInt32(env, manifestFd->Release()).val_)});

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

static void OnResultReport(weak_ptr<GeneralCallbacks> pCallbacks, const std::string result)
{
    HILOGI("callback function onResultReport begin.");
    if (pCallbacks.expired()) {
        HILOGI("callbacks is unbound");
        return;
    }
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("callback function onResultReport has already been released");
        return;
    }
    if (!bool(callbacks->onResultReport)) {
        HILOGI("callback function onResultReport is undefined");
        return;
    }
    auto cbCompl = [res {result}](napi_env env, NError err) -> NVal {
        NVal str = NVal::CreateUTF8String(env, res);
        return str;
    };
    callbacks->onResultReport.ThreadSafeSchedule(cbCompl);
}

static bool VerifyParamSuccess(NFuncArg &funcArg, int32_t &fd, std::vector<std::string> &bundleNames,
    std::vector<std::string> &bundleInfos, napi_env env)
{
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return false;
    }
    NVal remoteCap(env, funcArg[NARG_POS::FIRST]);
    auto [err, jsFd] = remoteCap.ToInt32();
    if (!err) {
        HILOGE("First argument is not remote capabilitily file number.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "First argument is not remote capabilitily file number.").GetCode())
            .ThrowErr(env);
        return false;
    }
    fd = jsFd;
    NVal jsBundles(env, funcArg[NARG_POS::SECOND]);
    auto [succ, jsBundleNames, ignore] = jsBundles.ToStringArray();
    if (!succ) {
        HILOGE("First argument is not bundles array.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "First argument is not bundles array.").GetCode()).ThrowErr(env);
        return false;
    }
    bundleNames = jsBundleNames;
    NVal jsInfos(env, funcArg[NARG_POS::THIRD]);
    if (jsInfos.TypeIs(napi_undefined) || jsInfos.TypeIs(napi_null)) {
        HILOGW("Third param is not exist");
        return true;
    }
    auto [deSuc, jsBundleInfos, deIgnore] = jsInfos.ToStringArray();
    if (deSuc) {
        bundleInfos = jsBundleInfos;
        if (bundleNames.size() != bundleInfos.size()) {
            HILOGE("bundleNames count is not equals bundleInfos count");
            return false;
        }
        return true;
    }
    HILOGI("Third param is callback");
    return true;
}

napi_value SessionRestoreNExporter::Constructor(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("called SessionRestore::Constructor begin");
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
    bool bSheet = BackupPara().GetBackupOverrideIncrementalRestore();
    auto restoreEntity = std::make_unique<RestoreEntity>();
    restoreEntity->callbacks = make_shared<GeneralCallbacks>(env, ptr, callbacks);
    if (bSheet) {
        restoreEntity->sessionWhole = nullptr;
        restoreEntity->sessionSheet = BIncrementalRestoreSession::Init(BIncrementalRestoreSession::Callbacks {
            .onFileReady =
                bind(OnFileReadySheet, restoreEntity->callbacks, placeholders::_1, placeholders::_2, placeholders::_3),
            .onBundleStarted = bind(onBundleBegin, restoreEntity->callbacks, placeholders::_1, placeholders::_2),
            .onBundleFinished = bind(onBundleEnd, restoreEntity->callbacks, placeholders::_1, placeholders::_2),
            .onAllBundlesFinished = bind(onAllBundlesEnd, restoreEntity->callbacks, placeholders::_1),
            .onResultReport = bind(OnResultReport, restoreEntity->callbacks, placeholders::_1),
            .onBackupServiceDied = bind(OnBackupServiceDied, restoreEntity->callbacks)});
    } else {
        restoreEntity->sessionSheet = nullptr;
        restoreEntity->sessionWhole = BSessionRestore::Init(BSessionRestore::Callbacks {
            .onFileReady = bind(OnFileReadyWhole, restoreEntity->callbacks, placeholders::_1, placeholders::_2),
            .onBundleStarted = bind(onBundleBegin, restoreEntity->callbacks, placeholders::_1, placeholders::_2),
            .onBundleFinished = bind(onBundleEnd, restoreEntity->callbacks, placeholders::_1, placeholders::_2),
            .onAllBundlesFinished = bind(onAllBundlesEnd, restoreEntity->callbacks, placeholders::_1),
            .onResultReport = bind(OnResultReport, restoreEntity->callbacks, placeholders::_1),
            .onBackupServiceDied = bind(OnBackupServiceDied, restoreEntity->callbacks)});
    }
    if (!restoreEntity->sessionWhole && !restoreEntity->sessionSheet) {
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to init restore").GetCode()).ThrowErr(env);
        return nullptr;
    }
    if (!NClass::SetEntityFor<RestoreEntity>(env, funcArg.GetThisVar(), move(restoreEntity))) {
        HILOGE("Failed to set SessionRestore entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to set SessionRestore entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    HILOGI("called SessionRestore::Constructor end");
    return funcArg.GetThisVar();
}

napi_value SessionRestoreNExporter::AppendBundles(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("called SessionRestore::AppendBundles begin");
    int32_t fd = BConstants::INVALID_FD_NUM;
    std::vector<std::string> bundleNames;
    std::vector<std::string> bundleInfos;
    NFuncArg funcArg(env, cbinfo);
    if (!VerifyParamSuccess(funcArg, fd, bundleNames, bundleInfos, env)) {
        return nullptr;
    }
    auto restoreEntity = NClass::GetEntityOf<RestoreEntity>(env, funcArg.GetThisVar());
    if (!(restoreEntity && (restoreEntity->sessionWhole || restoreEntity->sessionSheet))) {
        HILOGE("Failed to get RestoreSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get RestoreSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    auto cbExec = [entity {restoreEntity}, fd {fd}, bundles {bundleNames}, infos {bundleInfos}]() -> NError {
        if (!(entity && (entity->sessionWhole || entity->sessionSheet))) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "restore session is nullptr").GetCode());
        }
        if (entity->sessionWhole) {
            if (!infos.empty()) {
                return NError(entity->sessionWhole->AppendBundles(UniqueFd(fd), bundles, infos));
            }
            return NError(entity->sessionWhole->AppendBundles(UniqueFd(fd), bundles));
        }
        if (!infos.empty()) {
            return NError(entity->sessionSheet->AppendBundles(UniqueFd(fd), bundles, infos));
        }
        return NError(entity->sessionSheet->AppendBundles(UniqueFd(fd), bundles));
    };
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal {env, err.GetNapiErr(env)} : NVal::CreateUndefined(env);
    };
    HILOGI("Called SessionRestore::AppendBundles end.");
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::TWO) {
        return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
    } else if (!bundleInfos.empty()) {
        HILOGI("The third param is string array");
        return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
    } else {
        HILOGI("The third param is call back");
        NVal cb(env, funcArg[NARG_POS::THIRD]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(className, cbExec, cbCompl).val_;
    }
}

static std::tuple<bool, std::unique_ptr<char[]>, std::unique_ptr<char[]>> ParseFileMeta(napi_env env,
    const NVal &fileMeta)
{
    bool succ = false;
    std::unique_ptr<char[]> bundleName = nullptr;
    tie(succ, bundleName, ignore) = fileMeta.GetProp(BConstants::BUNDLE_NAME).ToUTF8String();
    if (!succ) {
        HILOGE("First argument is not have property bundle name.");
        return { false, nullptr, nullptr };
    }

    std::unique_ptr<char[]> fileName = nullptr;
    tie(succ, fileName, ignore) = fileMeta.GetProp(BConstants::URI).ToUTF8String();
    if (!succ) {
        HILOGE("First argument is not have property file name.");
        return { false, nullptr, nullptr };
    }

    return { true, move(bundleName), move(fileName) };
}

napi_value SessionRestoreNExporter::PublishFile(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("called SessionRestore::PublishFile begin");
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    NVal fileMeta(env, funcArg[NARG_POS::FIRST]);
    if (!fileMeta.TypeIs(napi_object)) {
        HILOGE("First arguments is not an object.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "First arguments is not an object.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto [succ, bundleName, fileName] = ParseFileMeta(env, fileMeta);
    if (!succ) {
        HILOGE("ParseFileMeta failed.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "ParseFileMeta failed.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto restoreEntity = NClass::GetEntityOf<RestoreEntity>(env, funcArg.GetThisVar());
    if (!(restoreEntity && (restoreEntity->sessionWhole || restoreEntity->sessionSheet))) {
        HILOGE("Failed to get RestoreSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get RestoreSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = [entity {restoreEntity}, bundleName {string(bundleName.get())},
        fileName {string(fileName.get())}]() -> NError {
        if (!(entity && (entity->sessionWhole || entity->sessionSheet))) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "restore session is nullptr").GetCode());
        }
        BFileInfo fileInfo(bundleName, fileName, 0);
        if (entity->sessionWhole) {
            return NError(entity->sessionWhole->PublishFile(fileInfo));
        }
        return NError(entity->sessionSheet->PublishFile(fileInfo));
    };
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal {env, err.GetNapiErr(env)} : NVal::CreateUndefined(env);
    };

    HILOGI("Called SessionRestore::PublishFile end.");

    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
    } else {
        NVal cb(env, funcArg[NARG_POS::SECOND]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(className, cbExec, cbCompl).val_;
    }
}

napi_value SessionRestoreNExporter::GetFileHandle(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("called SessionRestore::GetFileHandle begin");
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    NVal fileMeta(env, funcArg[NARG_POS::FIRST]);
    if (!fileMeta.TypeIs(napi_object)) {
        HILOGE("First arguments is not an object.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "First arguments is not an object.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto [succ, bundleName, fileName] = ParseFileMeta(env, fileMeta);
    if (!succ) {
        HILOGE("ParseFileMeta failed.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "ParseFileMeta failed.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto restoreEntity = NClass::GetEntityOf<RestoreEntity>(env, funcArg.GetThisVar());
    if (!(restoreEntity && (restoreEntity->sessionWhole || restoreEntity->sessionSheet))) {
        HILOGE("Failed to get RestoreSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get RestoreSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = [entity {restoreEntity}, bundleName {string(bundleName.get())},
        fileName {string(fileName.get())}]() -> NError {
        if (!(entity && (entity->sessionWhole || entity->sessionSheet))) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "restore session is nullptr").GetCode());
        }
        string bundle = bundleName;
        string file = fileName;
        if (entity->sessionWhole) {
            return NError(entity->sessionWhole->GetFileHandle(bundle, file));
        }
        return NError(entity->sessionSheet->GetFileHandle(bundle, file));
    };
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal {env, err.GetNapiErr(env)} : NVal::CreateUndefined(env);
    };

    HILOGI("Called SessionRestore::GetFileHandle end.");

    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
    } else {
        NVal cb(env, funcArg[NARG_POS::SECOND]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(className, cbExec, cbCompl).val_;
    }
}

napi_value SessionRestoreNExporter::Release(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("called SessionRestore::Release begin");
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto restoreEntity = NClass::GetEntityOf<RestoreEntity>(env, funcArg.GetThisVar());
    if (!(restoreEntity && (restoreEntity->sessionWhole || restoreEntity->sessionSheet))) {
        HILOGE("Failed to get RestoreSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get RestoreSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = [entity {restoreEntity}]() -> NError {
        if (!(entity && (entity->sessionWhole || entity->sessionSheet))) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "restore session is nullptr").GetCode());
        }
        if (entity->sessionWhole) {
            return NError(entity->sessionWhole->Release());
        }
        return NError(entity->sessionSheet->Release());
    };
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal {env, err.GetNapiErr(env)} : NVal::CreateUndefined(env);
    };

    HILOGI("Called SessionRestore::Release end.");

    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
}

bool SessionRestoreNExporter::Export()
{
    HILOGI("called SessionRestoreNExporter::Export begin");
    vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("appendBundles", AppendBundles),
        NVal::DeclareNapiFunction("publishFile", PublishFile),
        NVal::DeclareNapiFunction("getFileHandle", GetFileHandle),
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

    HILOGI("called SessionRestoreNExporter::Export end");
    return exports_.AddProp(className, classValue);
}

string SessionRestoreNExporter::GetClassName()
{
    return SessionRestoreNExporter::className;
}

SessionRestoreNExporter::SessionRestoreNExporter(napi_env env, napi_value exports) : NExporter(env, exports) {}

SessionRestoreNExporter::~SessionRestoreNExporter() {}
} // namespace OHOS::FileManagement::Backup