/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#include <fcntl.h>

#include "b_error/b_error.h"
#include "b_filesystem/b_dir.h"
#include "b_filesystem/b_file.h"
#include "b_ohos/startup/backup_para.h"
#include "b_resources/b_constants.h"
#include "b_sa/b_sa_utils.h"
#include "backup_kit_inner.h"
#include "filemgmt_libhilog.h"
#include "service_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace LibN;

static void OnFileReadySheet(weak_ptr<GeneralCallbacks> pCallbacks, const BFileInfo &fileInfo,
    UniqueFd fd, UniqueFd manifestFd, int32_t sysErrno)
{
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("callback function onFileReady has already been released");
        return;
    }
    if (!bool(callbacks->onFileReady)) {
        HILOGI("callback function onFileReady is undefined");
        return;
    }
    ErrCode errCode = BError::GetCodeByErrno(sysErrno);
    std::tuple<uint32_t, std::string> errInfo = std::make_tuple(errCode, "system errno: " + to_string(sysErrno));
    HILOGI("callback function restore onFileReadySheet begin errCode: %{public}d, bundle: %{public}s, file: %{public}s",
        std::get<0>(errInfo), fileInfo.owner.c_str(), GetAnonyPath(fileInfo.fileName).c_str());
    auto cbCompl = [bundleName {fileInfo.owner}, fileName {fileInfo.fileName},
                    fd {make_shared<UniqueFd>(fd.Release())}, manifestFd {make_shared<UniqueFd>(manifestFd.Release())},
                    errInfo](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        HILOGI("callback function restore OnFileReadySheet errCode: %{public}d, bundle: %{public}s, file: %{public}s",
            std::get<0>(errInfo), bundleName.c_str(), GetAnonyPath(fileName).c_str());
        NVal obj;
        ErrParam errorParam = [ errInfo ]() {
            return errInfo;
        };
        if (std::get<0>(errInfo) != 0) {
            obj = NVal {env, NError(errorParam).GetNapiErr(env)};
            napi_status status = napi_set_named_property(env, obj.val_, FILEIO_TAG_ERR_DATA.c_str(),
                NVal::CreateUTF8String(env, bundleName).val_);
            if (status != napi_ok) {
                HILOGE("Failed to set data, status %{public}d, bundleName %{public}s", status, bundleName.c_str());
            }
        } else {
            obj = NVal::CreateObject(env);
            obj.AddProp({
                NVal::DeclareNapiProperty(BConstants::BUNDLE_NAME.c_str(),
                    NVal::CreateUTF8String(env, bundleName).val_),
                NVal::DeclareNapiProperty(BConstants::URI.c_str(), NVal::CreateUTF8String(env, fileName).val_),
                NVal::DeclareNapiProperty(BConstants::FD.c_str(), NVal::CreateInt32(env, fd->Release()).val_),
                NVal::DeclareNapiProperty(BConstants::MANIFEST_FD.c_str(),
                    NVal::CreateInt32(env, manifestFd->Release()).val_)});
        }
        HILOGI("callback function restore onFileReadySheet end errCode:%{public}d, bundle:%{public}s, file:%{public}s",
            std::get<0>(errInfo), bundleName.c_str(), GetAnonyPath(fileName).c_str());
        return {obj};
    };

    callbacks->onFileReady.ThreadSafeSchedule(cbCompl);
}

static void onBundleBegin(weak_ptr<GeneralCallbacks> pCallbacks, ErrCode err, const BundleName name)
{
    HILOGI("Callback onBundleBegin, bundleName=%{public}s, errCode=%{public}d", name.c_str(), err);
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("callback function onBundleBegin has already been released");
        return;
    }
    if (!bool(callbacks->onBundleBegin)) {
        HILOGI("callback function onBundleBegin is undefined");
        return;
    }

    ErrCode errCode = BError::GetBackupCodeByErrno(err);
    std::string errMsg = BError::GetBackupMsgByErrno(errCode) + ", origin errno: " + to_string(err);
    std::tuple<uint32_t, std::string> errInfo = std::make_tuple(errCode, errMsg);
    HILOGI("callback function restore onBundleBegin start errCode: %{public}d", std::get<0>(errInfo));
    auto cbCompl = [name {name}, errCode {err}, errInfo](napi_env env, NError err) -> NVal {
        NVal bundleName = NVal::CreateUTF8String(env, name);
        if (!err && errCode == 0) {
            return bundleName;
        }
        ErrParam errorParam = [ errInfo ]() {
            return errInfo;
        };
        NVal res;
        if (err) {
            res = NVal {env, err.GetNapiErr(env)};
        } else {
            res = NVal {env, NError(errorParam).GetNapiErr(env)};
        }
        napi_status status = napi_set_named_property(env, res.val_, FILEIO_TAG_ERR_DATA.c_str(), bundleName.val_);
        if (status != napi_ok) {
            HILOGE("Failed to set data property, status %{public}d, bundleName %{public}s", status, name.c_str());
        }
        HILOGI("callback function restore onBundleBegin end errCode: %{public}d", std::get<0>(errInfo));
        return res;
    };

    callbacks->onBundleBegin.ThreadSafeSchedule(cbCompl);
}

static void onBundleEnd(weak_ptr<GeneralCallbacks> pCallbacks, ErrCode err, const BundleName name)
{
    HILOGI("Callback onBundleEnd, bundleName=%{public}s, errCode=%{public}d", name.c_str(), err);
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("callback function onBundleEnd has already been released");
        return;
    }
    if (!bool(callbacks->onBundleEnd)) {
        HILOGI("callback function onBundleEnd is undefined");
        return;
    }

    ErrCode errCode = BError::GetBackupCodeByErrno(err);
    std::string errMsg = BError::GetBackupMsgByErrno(errCode) + ", origin errno: " + to_string(err);
    std::tuple<uint32_t, std::string> errInfo = std::make_tuple(errCode, errMsg);
    HILOGI("callback function restore onBundleEnd start errCode: %{public}d", std::get<0>(errInfo));
    auto cbCompl = [name {name}, errCode {err}, errInfo](napi_env env, NError err) -> NVal {
        NVal bundleName = NVal::CreateUTF8String(env, name);
        if (!err && errCode == 0) {
            return bundleName;
        }
        ErrParam errorParam = [ errInfo ]() {
            return errInfo;
        };
        NVal res;
        if (err) {
            res = NVal {env, err.GetNapiErr(env)};
        } else {
            res = NVal {env, NError(errorParam).GetNapiErr(env)};
        }
        napi_status status = napi_set_named_property(env, res.val_, FILEIO_TAG_ERR_DATA.c_str(), bundleName.val_);
        if (status != napi_ok) {
            HILOGE("Failed to set data property, status %{public}d, bundleName %{public}s", status, name.c_str());
        }
        HILOGI("callback function restore onBundleEnd end errCode: %{public}d", std::get<0>(errInfo));
        return res;
    };

    callbacks->onBundleEnd.ThreadSafeSchedule(cbCompl);
}

static void onAllBundlesEnd(weak_ptr<GeneralCallbacks> pCallbacks, ErrCode err)
{
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("callback function onAllBundlesEnd has already been released");
        return;
    }
    if (!bool(callbacks->onAllBundlesEnd)) {
        HILOGI("callback function onAllBundlesEnd is undefined");
        return;
    }

    ErrCode errCode = BError::GetBackupCodeByErrno(err);
    std::string errMsg = BError::GetBackupMsgByErrno(errCode) + ", origin errno: " + to_string(err);
    std::tuple<uint32_t, std::string> errInfo = std::make_tuple(errCode, errMsg);
    HILOGI("callback function restore onAllBundlesEnd begin errCode: %{public}d", std::get<0>(errInfo));
    auto cbCompl = [errCode {err}, errInfo](napi_env env, NError err) -> NVal {
        if (!err && errCode == 0) {
            return NVal::CreateUndefined(env);
        }
        ErrParam errorParam = [ errInfo ]() {
            return errInfo;
        };
        NVal res;
        if (err) {
            res = NVal {env, err.GetNapiErr(env)};
        } else {
            res = NVal {env, NError(errorParam).GetNapiErr(env)};
        }
        HILOGI("callback function backup onAllBundlesEnd end errCode: %{public}d", std::get<0>(errInfo));
        return res;
    };

    callbacks->onAllBundlesEnd.ThreadSafeSchedule(cbCompl);
}

static void OnBackupServiceDied(weak_ptr<GeneralCallbacks> pCallbacks)
{
    HILOGI("Callback OnBackupServiceDied.");
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("js callback function onBackupServiceDied has already been released");
        return;
    }
    if (!bool(callbacks->onBackupServiceDied)) {
        HILOGI("callback function onBackupServiceDied is undefined");
        return;
    }

    auto cbCompl = [](napi_env env, vector<napi_value> &argv) -> bool {
        argv.emplace_back(nullptr);
        napi_value napi_res = nullptr;
        if (napi_get_undefined(env, &napi_res) != napi_ok) {
            HILOGE("create undefined napi object failed");
            return false;
        }
        argv.emplace_back(napi_res);
        return true;
    };
    callbacks->onBackupServiceDied.CallJsMethod(cbCompl);
}

static void OnResultReport(weak_ptr<GeneralCallbacks> pCallbacks, const std::string bundleName,
    const std::string result)
{
    HILOGI("Callback OnResultReport, bundleName=%{public}s", bundleName.c_str());
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("callback function onResultReport has already been released");
        return;
    }
    if (!bool(callbacks->onResultReport)) {
        HILOGI("callback function onResultReport is undefined");
        return;
    }
    auto cbCompl = [bName {bundleName}, res {result}](napi_env env, vector<napi_value> &argv) -> bool {
        napi_value napi_bName = nullptr;
        if (napi_create_string_utf8(env, bName.c_str(), bName.size(), &napi_bName) != napi_ok) {
            HILOGE("create napi string failed");
            return false;
        }
        argv.emplace_back(napi_bName);
        napi_value napi_res = nullptr;
        if (napi_create_string_utf8(env, res.c_str(), res.size(), &napi_res) != napi_ok) {
            HILOGE("create napi string failed");
            return false;
        }
        argv.emplace_back(napi_res);
        return true;
    };
    callbacks->onResultReport.CallJsMethod(cbCompl);
}

static void OnProcess(weak_ptr<GeneralCallbacks> pCallbacks, const BundleName name, const std::string processInfo)
{
    HILOGI("Callback OnProcess, bundleName=%{public}s", name.c_str());
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGI("callback function OnProcess has already been released");
        return;
    }
    if (!bool(callbacks->onProcess)) {
        HILOGI("callback function OnProcess is undefined");
        return;
    }
    auto cbCompl = [bundleName {name}, process {processInfo}](napi_env env, vector<napi_value> &argv) -> bool {
        napi_value napi_bName = nullptr;
        if (napi_create_string_utf8(env, bundleName.c_str(), bundleName.size(), &napi_bName) != napi_ok) {
            HILOGE("create napi string failed");
            return false;
        }
        argv.emplace_back(napi_bName);
        napi_value napi_process = nullptr;
        if (napi_create_string_utf8(env, process.c_str(), process.size(), &napi_process) != napi_ok) {
            HILOGE("create napi string failed");
            return false;
        }
        argv.emplace_back(napi_process);
        return true;
    };
    callbacks->onProcess.CallJsMethod(cbCompl);
}

static bool VerifyAppendBundlesParam(NFuncArg &funcArg, int32_t &fd, std::vector<std::string> &bundleNames,
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

static bool VerifyPublishFileParam(NFuncArg &funcArg, std::string &bundleName, std::string &fileName, napi_env env)
{
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return false;
    }

    NVal fileMeta(env, funcArg[NARG_POS::FIRST]);
    if (!fileMeta.TypeIs(napi_object)) {
        HILOGE("First arguments is not an object.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "First arguments is not an object.").GetCode()).ThrowErr(env);
        return false;
    }
    auto [succ, bundle, file] = ParseFileMeta(env, fileMeta);
    if (!succ) {
        HILOGE("ParseFileMeta failed.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "ParseFileMeta failed.").GetCode()).ThrowErr(env);
        return false;
    }
    bundleName = string(bundle.get());
    fileName = string(file.get());
    return true;
}

static bool VerifyNapiObject(napi_env env, NFuncArg &funcArg)
{
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return true;
    }
    return false;
}

static bool VerifyNarg(napi_env env, NVal &callbacks)
{
    if (!callbacks.TypeIs(napi_object)) {
        HILOGE("First argument is not an object.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "First argument is not an object.").GetCode()).ThrowErr(env);
        return true;
    }
    return false;
}

static bool SetSessionRestoreEntity(napi_env env, NFuncArg &funcArg, std::unique_ptr<RestoreEntity> restoreEntity)
{
    auto finalize = [](napi_env env, void *data, void *hint) {
        std::unique_ptr<RestoreEntity> entity = std::unique_ptr<RestoreEntity>(static_cast<RestoreEntity *>(data));
        if (entity == nullptr) {
            HILOGE("Entity is nullptr");
            return;
        }
        if (entity->callbacks == nullptr) {
            HILOGE("Callbacks is nullptr");
            return;
        }
        entity->callbacks->RemoveCallbackRef();
    };
    if (napi_wrap(env, funcArg.GetThisVar(), restoreEntity.release(), finalize, nullptr, nullptr) != napi_ok) {
        HILOGE("Failed to set restoreEntity entity.");
        return false;
    }
    return true;
}

napi_value SessionRestoreNExporter::Constructor(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("called SessionRestore::Constructor begin");
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, cbinfo);
    if (VerifyNapiObject(env, funcArg)) {
        return nullptr;
    }
    NVal callbacks(env, funcArg[NARG_POS::FIRST]);
    if (VerifyNarg(env, callbacks)) {
        return nullptr;
    }

    NVal ptr(env, funcArg.GetThisVar());
    auto restoreEntity = std::make_unique<RestoreEntity>();
    restoreEntity->callbacks = make_shared<GeneralCallbacks>(env, ptr, callbacks);
    restoreEntity->sessionWhole = nullptr;
    ErrCode errCode;
    std::string errMsg;
    restoreEntity->sessionSheet = BIncrementalRestoreSession::Init(BIncrementalRestoreSession::Callbacks {
        .onFileReady = bind(OnFileReadySheet, restoreEntity->callbacks, placeholders::_1, placeholders::_2,
            placeholders::_3, placeholders::_4),
        .onBundleStarted = bind(onBundleBegin, restoreEntity->callbacks, placeholders::_1, placeholders::_2),
        .onBundleFinished = bind(onBundleEnd, restoreEntity->callbacks, placeholders::_1, placeholders::_2),
        .onAllBundlesFinished = bind(onAllBundlesEnd, restoreEntity->callbacks, placeholders::_1),
        .onResultReport = bind(OnResultReport, restoreEntity->callbacks, placeholders::_1, placeholders::_2),
        .onBackupServiceDied = bind(OnBackupServiceDied, restoreEntity->callbacks),
        .onProcess = bind(OnProcess, restoreEntity->callbacks, placeholders::_1, placeholders::_2)}, errMsg, errCode);
    if (!restoreEntity->sessionSheet) {
        std::tuple<uint32_t, std::string> errInfo = (errCode == BError(BError::Codes::SA_SESSION_CONFLICT)) ?
            std::make_tuple(errCode, errMsg) : std::make_tuple(errCode, BError::GetBackupMsgByErrno(errCode));
        ErrParam errorParam = [ errInfo ]() { return errInfo;};
        NError(errorParam).ThrowErr(env);
        return nullptr;
    }
    if (!SetSessionRestoreEntity(env, funcArg, std::move(restoreEntity))) {
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to set SessionRestore entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    HILOGI("called SessionRestore::Constructor end");
    return funcArg.GetThisVar();
}

napi_value SessionRestoreNExporter::GetLocalCapabilities(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("called SessionBackup, GetLocalCapabilities Begin");
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    auto restoreEntity = NClass::GetEntityOf<RestoreEntity>(env, funcArg.GetThisVar());
    if (!(restoreEntity && (restoreEntity->sessionWhole || restoreEntity->sessionSheet))) {
        HILOGE("Failed to get restoreSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get restoreSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    auto fd = make_shared<UniqueFd>();
    auto cbExec = [session {restoreEntity}, fd]() -> NError {
        if (!session && (session->sessionWhole || session->sessionSheet)) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "backup session is nullptr").GetCode());
        }
        if (session->sessionWhole) {
            *fd = session->sessionWhole->GetLocalCapabilities();
        } else if (session->sessionSheet) {
            *fd = session->sessionSheet->GetLocalCapabilities();
        }
        return NError(ERRNO_NOERR);
    };
    auto cbCompl = [fd](napi_env env, NError err) -> NVal {
        NVal obj = NVal::CreateObject(env);
        obj.AddProp({NVal::DeclareNapiProperty(BConstants::FD.c_str(), NVal::CreateInt32(env, fd->Release()).val_)});
        return {obj};
    };
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
}

static NContextCBExec GetAppendBundlesCBExec(napi_env env, NFuncArg &funcArg, const int32_t fdRestore,
    const std::vector<std::string> &bundleNames, const std::vector<std::string> &bundleInfos)
{
    auto restoreEntity = NClass::GetEntityOf<RestoreEntity>(env, funcArg.GetThisVar());
    if (!(restoreEntity && (restoreEntity->sessionWhole || restoreEntity->sessionSheet))) {
        HILOGE("Failed to get RestoreSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get RestoreSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    return [entity {restoreEntity}, fd {fdRestore}, bundles {bundleNames}, infos {bundleInfos}]() -> NError {
        if (!(entity && (entity->sessionWhole || entity->sessionSheet))) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "restore session is nullptr").GetCode());
        }
        if (fcntl(fd, F_GETFD) == -1) {
            HILOGE("AppendBundles fd is invalid.");
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "AppendBundles fd is invalid.").GetCode());
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
}

napi_value SessionRestoreNExporter::AppendBundles(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("called SessionRestore::AppendBundles begin");
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }
    int32_t fdRestore = BConstants::INVALID_FD_NUM;
    std::vector<std::string> bundleNames;
    std::vector<std::string> bundleInfos;
    NFuncArg funcArg(env, cbinfo);
    if (!VerifyAppendBundlesParam(funcArg, fdRestore, bundleNames, bundleInfos, env)) {
        return nullptr;
    }
    auto cbExec = GetAppendBundlesCBExec(env, funcArg, fdRestore, bundleNames, bundleInfos);
    if (cbExec == nullptr) {
        HILOGE("GetAppendBundlesCBExec fail!");
        return nullptr;
    }
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal {env, err.GetNapiErr(env)} : NVal::CreateUndefined(env);
    };
    HILOGD("Called SessionRestore::AppendBundles end.");

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

static NContextCBExec GetPublishFileCBExec(napi_env env, NFuncArg &funcArg, const std::string &bundleName,
    const std::string &fileName)
{
    auto restoreEntity = NClass::GetEntityOf<RestoreEntity>(env, funcArg.GetThisVar());
    if (!(restoreEntity && (restoreEntity->sessionWhole || restoreEntity->sessionSheet))) {
        HILOGE("Failed to get RestoreSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get RestoreSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    return [entity {restoreEntity}, bundleName {bundleName}, fileName {fileName}]() -> NError {
        if (!(entity && (entity->sessionWhole || entity->sessionSheet))) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "restore session is nullptr").GetCode());
        }
        BFileInfo fileInfo(bundleName, fileName, 0);
        if (entity->sessionWhole) {
            return NError(entity->sessionWhole->PublishFile(fileInfo));
        }
        if (SAUtils::IsSABundleName(bundleName)) {
            if (SAUtils::IsSABundleName(fileName)) {
                HILOGI("SA %{public}s pushlish file", bundleName.c_str());
                if (fcntl(std::atoi(fileName.c_str()), F_GETFD) == -1) {
                    HILOGE("PublishFile fd is invalid.");
                    return NError(BError(BError::Codes::SDK_INVAL_ARG, "PublishFile fd is invalid.").GetCode());
                }
                return NError(entity->sessionSheet->PublishSAFile(fileInfo, UniqueFd(std::atoi(fileName.c_str()))));
            }
            HILOGE("SA:%{public}s, fileName:%{private}s is invalid", bundleName.c_str(), fileName.c_str());
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "PublishFile fd is invalid.").GetCode());
        }
        return NError(entity->sessionSheet->PublishFile(fileInfo));
    };
}

napi_value SessionRestoreNExporter::PublishFile(napi_env env, napi_callback_info cbinfo)
{
    HILOGD("called SessionRestore::PublishFile begin");
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }
    std::string bundleName;
    std::string fileName;
    NFuncArg funcArg(env, cbinfo);
    if (!VerifyPublishFileParam(funcArg, bundleName, fileName, env)) {
        return nullptr;
    }
    auto cbExec = GetPublishFileCBExec(env, funcArg, bundleName, fileName);
    if (cbExec == nullptr) {
        HILOGE("GetPublishFileCBExec fail!");
        return nullptr;
    }
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal {env, err.GetNapiErr(env)} : NVal::CreateUndefined(env);
    };
    HILOGD("Called SessionRestore::PublishFile end.");

    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
    } else {
        NVal cb(env, funcArg[NARG_POS::SECOND]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(className, cbExec, cbCompl).val_;
    }
}

static NContextCBExec GetFileHandleCBExec(napi_env env, NFuncArg &funcArg, std::unique_ptr<char[]> bundleName,
    std::unique_ptr<char[]> fileName)
{
    auto restoreEntity = NClass::GetEntityOf<RestoreEntity>(env, funcArg.GetThisVar());
    if (!(restoreEntity && (restoreEntity->sessionWhole || restoreEntity->sessionSheet))) {
        HILOGE("Failed to get RestoreSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get RestoreSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    return [entity {restoreEntity}, bundleName {string(bundleName.get())},
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
}

napi_value SessionRestoreNExporter::GetFileHandle(napi_env env, napi_callback_info cbinfo)
{
    HILOGD("called SessionRestore::GetFileHandle begin");
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }
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

    auto cbExec = GetFileHandleCBExec(env, funcArg, move(bundleName), move(fileName));
    if (cbExec == nullptr) {
        HILOGE("GetFileHandleCBExec fail!");
        return nullptr;
    }
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal {env, err.GetNapiErr(env)} : NVal::CreateUndefined(env);
    };
    HILOGD("Called SessionRestore::GetFileHandle end.");

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
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }
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

napi_value SessionRestoreNExporter::Cancel(napi_env env, napi_callback_info info)
{
    HILOGD("Called SessionRestore::Cancel begin");
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        HILOGE("Number of arguments unmatched.");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    NVal jsBundleStr(env, funcArg[NARG_POS::FIRST]);
    auto [succStr, bundle, sizeStr] = jsBundleStr.ToUTF8String();
    if (!succStr) {
        HILOGE("First argument is not bundleName.");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    std::string bundleName = bundle.get();

    auto restoreEntity = NClass::GetEntityOf<RestoreEntity>(env, funcArg.GetThisVar());
    if (!(restoreEntity && (restoreEntity->sessionWhole || restoreEntity->sessionSheet))) {
        HILOGE("Failed to get restoreSession entity.");
        return nullptr;
    }

    int result = BError(BError::Codes::OK);
    if (restoreEntity->sessionWhole) {
        result = restoreEntity->sessionWhole->Cancel(bundleName);
    } else {
        result = restoreEntity->sessionSheet->Cancel(bundleName);
    }

    napi_value nResult;
    napi_status status = napi_create_int32(env, result, &nResult);
    if (status != napi_ok) {
        HILOGE("napi_create_int32 faild.");
        return nullptr;
    }
    HILOGI("Cancel success with result: %{public}d", result);
    return nResult;
}

static NContextCBExec CleanBundleTempDirCBExec(napi_env env, const NFuncArg &funcArg,
                                               std::unique_ptr<char[]> bundleName)
{
    auto restoreEntity = NClass::GetEntityOf<RestoreEntity>(env, funcArg.GetThisVar());
    if (!(restoreEntity && (restoreEntity->sessionWhole || restoreEntity->sessionSheet))) {
        HILOGE("Failed to get RestoreSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get RestoreSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    return [entity {restoreEntity}, bundleName {string(bundleName.get())}]() -> NError {
        if (!(entity && (entity->sessionWhole || entity->sessionSheet))) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "restore session is nullptr").GetCode());
        }
        if (entity->sessionWhole) {
            return NError(entity->sessionWhole->CleanBundleTempDir(bundleName));
        }
        return NError(entity->sessionSheet->CleanBundleTempDir(bundleName));
    };
}

napi_value SessionRestoreNExporter::CleanBundleTempDir(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("Called SessionRestore::CleanBundleTempDir begin.");
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        HILOGE("Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    NVal jsBundleStr(env, funcArg[NARG_POS::FIRST]);
    auto [succ, bundleName, sizeStr] = jsBundleStr.ToUTF8String();
    if (!succ) {
        HILOGE("First arguments is not string.");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = CleanBundleTempDirCBExec(env, funcArg, std::move(bundleName));
    if (cbExec == nullptr) {
        HILOGE("CleanBundleTempDirCBExec fail!");
        return nullptr;
    }
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal::CreateBool(env, false) : NVal::CreateBool(env, true);
    };

    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
}

bool SessionRestoreNExporter::Export()
{
    HILOGD("called SessionRestoreNExporter::Export begin");
    vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("getLocalCapabilities", GetLocalCapabilities),
        NVal::DeclareNapiFunction("appendBundles", AppendBundles),
        NVal::DeclareNapiFunction("publishFile", PublishFile),
        NVal::DeclareNapiFunction("getFileHandle", GetFileHandle),
        NVal::DeclareNapiFunction("release", Release),
        NVal::DeclareNapiFunction("cancel", Cancel),
        NVal::DeclareNapiFunction("cleanBundleTempDir", CleanBundleTempDir),
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

    HILOGD("called SessionRestoreNExporter::Export end");
    return exports_.AddProp(className, classValue);
}


napi_value SessionRestoreNExporter::ConstructorFromEntity(napi_env env, napi_callback_info cbinfo)
{
    HILOGD("called ConstructorFromEntity begin");
    if (!SAUtils::CheckBackupPermission()) {
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    if (!SAUtils::IsSystemApp()) {
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    void* entityRawPtr = nullptr;
    if (napi_ok != napi_get_value_external(env, funcArg[NARG_POS::FIRST], &entityRawPtr)) {
        HILOGE("parse entity raw ptr for napi_value fail");
        return nullptr;
    }
    RestoreEntity* entity =  reinterpret_cast<RestoreEntity*>(entityRawPtr);
    if (entity == nullptr) {
        NError(BError(BError::Codes::SDK_INVAL_ARG, "First argument is not session pointer.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    std::unique_ptr<RestoreEntity> restoreEntity(entity);
    if ((restoreEntity->sessionWhole == nullptr && restoreEntity->sessionSheet == nullptr) ||
        restoreEntity->callbacks == nullptr) {
        HILOGE("session or callback is null");
        return nullptr;
    }
    if (!SetSessionRestoreEntity(env, funcArg, std::move(restoreEntity))) {
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to set RestoreEntity entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    HILOGD("called ConstructorFromEntity end");
    return funcArg.GetThisVar();
}

napi_value SessionRestoreNExporter::CreateByEntity(napi_env env, std::unique_ptr<RestoreEntity> entity)
{
    HILOGD("CreateByEntity begin");
    if (entity == nullptr) {
        HILOGE("entity is null");
        return nullptr;
    }
    vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("getLocalCapabilities", GetLocalCapabilities),
        NVal::DeclareNapiFunction("getFileHandle", GetFileHandle),
        NVal::DeclareNapiFunction("appendBundles", AppendBundles),
        NVal::DeclareNapiFunction("publishFile", PublishFile),
        NVal::DeclareNapiFunction("release", Release),
        NVal::DeclareNapiFunction("cancel", Cancel),
        NVal::DeclareNapiFunction("cleanBundleTempDir", CleanBundleTempDir),
    };
    auto [defRet, constroctor] = NClass::DefineClass(env, napiClassName_, ConstructorFromEntity,
        std::move(props));
    if (!defRet) {
        HILOGE("Failed to define class");
        return nullptr;
    }
    napi_value instance;
    napi_value napiEntity;
    auto finalize = [](napi_env env, void *data, void *hint) {
        std::unique_ptr<RestoreEntity> entity(static_cast<RestoreEntity *>(data));
        if (entity == nullptr) {
            HILOGE("Entity is nullptr");
            return;
        }
        if (entity->callbacks == nullptr) {
            HILOGE("Callbacks is nullptr");
            return;
        }
        entity->callbacks->RemoveCallbackRef();
    };
    RestoreEntity* entityPtr = entity.release();
    if (napi_ok != napi_create_external(env, (void *)entityPtr, finalize, nullptr, &napiEntity)) {
        HILOGE("wrap entity prt fail");
        delete entityPtr;
        return nullptr;
    }
    size_t argc = 1;
    napi_value args[1] = { napiEntity };
    
    napi_status napiStatus = napi_new_instance(env, constroctor, argc, args, &instance);
    if (napi_status::napi_ok != napiStatus) {
        HILOGE("Failed to napi_new_instance, status=%{public}d", napiStatus);
        return nullptr;
    }
    HILOGD("CreateByEntity end");
    return instance;
}

string SessionRestoreNExporter::GetClassName()
{
    return SessionRestoreNExporter::className;
}

SessionRestoreNExporter::SessionRestoreNExporter(napi_env env, napi_value exports) : NExporter(env, exports) {}

SessionRestoreNExporter::~SessionRestoreNExporter() {}
} // namespace OHOS::FileManagement::Backup