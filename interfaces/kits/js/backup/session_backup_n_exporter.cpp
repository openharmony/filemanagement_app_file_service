/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "b_error/b_error.h"
#include "b_filesystem/b_file.h"
#include "b_resources/b_constants.h"
#include "b_sa/b_sa_utils.h"
#include "backup_kit_inner.h"
#include "directory_ex.h"
#include "filemgmt_libhilog.h"
#include "parse_inc_info_from_js.h"
#include "service_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace LibN;

static void OnFileReady(weak_ptr<GeneralCallbacks> pCallbacks, const BFileInfo &fileInfo, UniqueFd fd, int sysErrno)
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
    std::string errMsg = "system errno: " + to_string(sysErrno);
    std::tuple<uint32_t, std::string> errInfo = std::make_tuple(errCode, errMsg);
    HILOGI("callback function backup onFileReady begin errCode: %{public}d, bundle: %{public}s, file: %{public}s",
        std::get<0>(errInfo), fileInfo.owner.c_str(), GetAnonyPath(fileInfo.fileName).c_str());
    auto cbCompl = [bundleName {fileInfo.owner}, fileName {fileInfo.fileName},
                    fd {make_shared<UniqueFd>(fd.Release())}, errInfo](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        HILOGI("callback function backup onFileReady errCode: %{public}d, bundle: %{public}s, file: %{public}s",
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
                HILOGE("Failed to set data property, status %{public}d, bundleName %{public}s",
                    status, bundleName.c_str());
            }
        } else {
            obj = NVal::CreateObject(env);
            obj.AddProp({
                NVal::DeclareNapiProperty(BConstants::BUNDLE_NAME.c_str(),
                    NVal::CreateUTF8String(env, bundleName).val_),
                NVal::DeclareNapiProperty(BConstants::URI.c_str(), NVal::CreateUTF8String(env, fileName).val_),
                NVal::DeclareNapiProperty(BConstants::FD.c_str(), NVal::CreateInt32(env, fd->Release()).val_)});
        }
        HILOGI("callback function backup onFileReady end errCode: %{public}d, bundle: %{public}s, file: %{public}s",
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
    HILOGI("callback function backup onBundleBegin start errCode: %{public}d", std::get<0>(errInfo));
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
        HILOGI("callback function backup onBundleBegin end errCode: %{public}d", std::get<0>(errInfo));
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
    HILOGI("callback function backup onBundleEnd start errCode: %{public}d", std::get<0>(errInfo));
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
        HILOGI("callback function backup onBundleEnd end errCode: %{public}d", std::get<0>(errInfo));
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
    HILOGI("callback function backup onAllBundlesEnd begin errCode: %{public}d", std::get<0>(errInfo));
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

static bool SetSessionBackupEntity(napi_env env, NFuncArg &funcArg, std::unique_ptr<BackupEntity> backupEntity)
{
    auto finalize = [](napi_env env, void *data, void *hint) {
        std::unique_ptr<BackupEntity> entity = std::unique_ptr<BackupEntity>(static_cast<BackupEntity *>(data));
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
    if (napi_wrap(env, funcArg.GetThisVar(), backupEntity.release(), finalize, nullptr, nullptr) != napi_ok) {
        HILOGE("Failed to set BackupEntity entity.");
        return false;
    }
    return true;
}

static void OnBackupSizeReport(weak_ptr<GeneralCallbacks> pCallbacks, const std::string scannedResult)
{
    HILOGI("Callback OnBackupSizeReport...");
    auto callbacks = pCallbacks.lock();
    if (!callbacks) {
        HILOGE("callback function OnScanning has already been released");
        return;
    }
    if (!bool(callbacks->onBackupSizeReport)) {
        HILOGE("callback function OnScanning is undefined");
        return;
    }
    auto cbCompl = [scannedInfo {scannedResult}](napi_env env, vector<napi_value> &argv) -> bool {
        napi_value napi_scanned = nullptr;
        if (napi_create_string_utf8(env, scannedInfo.c_str(), scannedInfo.size(), &napi_scanned) != napi_ok) {
            HILOGE("create napi string failed");
            return false;
        }
        argv.push_back(napi_scanned);
        return true;
    };
    callbacks->onBackupSizeReport.CallJsMethod(cbCompl);
}

napi_value SessionBackupNExporter::Constructor(napi_env env, napi_callback_info cbinfo)
{
    HILOGD("called SessionBackup::Constructor begin");
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
    ErrCode errCode;
    std::string errMsg;
    backupEntity->session = BSessionBackup::Init(BSessionBackup::Callbacks {
        .onFileReady = bind(OnFileReady, backupEntity->callbacks, placeholders::_1, placeholders::_2, placeholders::_3),
        .onBundleStarted = bind(onBundleBegin, backupEntity->callbacks, placeholders::_1, placeholders::_2),
        .onBundleFinished = bind(onBundleEnd, backupEntity->callbacks, placeholders::_1, placeholders::_2),
        .onAllBundlesFinished = bind(onAllBundlesEnd, backupEntity->callbacks, placeholders::_1),
        .onResultReport = bind(OnResultReport, backupEntity->callbacks, placeholders::_1, placeholders::_2),
        .onBackupServiceDied = bind(OnBackupServiceDied, backupEntity->callbacks),
        .onProcess = bind(OnProcess, backupEntity->callbacks, placeholders::_1, placeholders::_2),
        .onBackupSizeReport = bind(OnBackupSizeReport, backupEntity->callbacks, placeholders::_1)}, errMsg, errCode);
    if (!backupEntity->session) {
        std::tuple<uint32_t, std::string> errInfo = (errCode == BError(BError::Codes::SA_SESSION_CONFLICT)) ?
            std::make_tuple(errCode, errMsg) : std::make_tuple(errCode, BError::GetBackupMsgByErrno(errCode));
        ErrParam errorParam = [ errInfo ]() { return errInfo;};
        NError(errorParam).ThrowErr(env);
        return nullptr;
    }
    if (!SetSessionBackupEntity(env, funcArg, std::move(backupEntity))) {
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to set BackupEntity entity").GetCode()).ThrowErr(env);
        return nullptr;
    }
    HILOGD("called SessionBackup::Constructor end");
    return funcArg.GetThisVar();
}

napi_value SessionBackupNExporter::GetLocalCapabilities(napi_env env, napi_callback_info cbinfo)
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
    auto backupEntity = NClass::GetEntityOf<BackupEntity>(env, funcArg.GetThisVar());
    if (!(backupEntity && backupEntity->session)) {
        HILOGE("Failed to get backupSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get backupSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    auto fd = make_shared<UniqueFd>();
    auto cbExec = [session {backupEntity->session.get()}, fd]() -> NError {
        if (!session) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "backup session is nullptr").GetCode());
        }
        *fd = session->GetLocalCapabilities();
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

napi_value SessionBackupNExporter::GetBackupDataSize(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("called GetBackupDataSize Begin");
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has no permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System app check failed!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, cbinfo);
    std::vector<BIncrementalData> bundleNames;
    bool isPreciseScan;
    if (!Parse::VerifyAndParseParams(env, funcArg, isPreciseScan, bundleNames)) {
        HILOGE("VerifyAndParseParams failed");
        return nullptr;
    }
    auto backupEntity = NClass::GetEntityOf<BackupEntity>(env, funcArg.GetThisVar());
    if (!(backupEntity && backupEntity->session)) {
        HILOGE("Failed to get backupSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get backupSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    auto cbExec = [session {backupEntity->session.get()}, isPreciseScan {isPreciseScan},
        bundleNames {move(bundleNames)}]() -> NError {
        if (!session) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "backup session is nullptr").GetCode());
        }
        auto ret = session->GetBackupDataSize(isPreciseScan, bundleNames);
        if (ret != ERR_OK) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to GetBackupDataSize").GetCode());
        }
        HILOGI("GetBackupDataSize end");
        return NError(ERRNO_NOERR);
    };
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal {env, err.GetNapiErr(env)} : NVal::CreateUndefined(env);
    };
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
}

static bool VerifyParamSuccess(NFuncArg &funcArg, std::vector<std::string> &bundleNames,
    std::vector<std::string> &bundleInfos, napi_env env)
{
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return false;
    }
    NVal jsBundles(env, funcArg[NARG_POS::FIRST]);
    auto [succ, jsBundleNames, ignore] = jsBundles.ToStringArray();
    if (!succ) {
        HILOGE("First argument is not bundles array.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "First argument is not bundles array.").GetCode()).ThrowErr(env);
        return false;
    }
    bundleNames = jsBundleNames;
    NVal jsInfos(env, funcArg[NARG_POS::SECOND]);
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
    HILOGI("Second param is callback");
    return true;
}

napi_value SessionBackupNExporter::AppendBundles(napi_env env, napi_callback_info cbinfo)
{
    HILOGD("called SessionBackup::AppendBundles begin");
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
    std::vector<std::string> bundleNames;
    std::vector<std::string> bundleInfos;
    NFuncArg funcArg(env, cbinfo);
    if (!VerifyParamSuccess(funcArg, bundleNames, bundleInfos, env)) {
        HILOGE("VerifyParamSuccess fail");
        return nullptr;
    }
    auto backupEntity = NClass::GetEntityOf<BackupEntity>(env, funcArg.GetThisVar());
    if (!(backupEntity && backupEntity->session)) {
        HILOGE("Failed to get backupSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get backupSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = [session {backupEntity->session.get()}, bundles {bundleNames}, infos {bundleInfos}]() -> NError {
        if (!session) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "backup session is nullptr").GetCode());
        }
        if (!infos.empty()) {
            return NError(session->AppendBundles(bundles, infos));
        }
        return NError(session->AppendBundles(bundles));
    };
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        return err ? NVal {env, err.GetNapiErr(env)} : NVal::CreateUndefined(env);
    };

    HILOGD("Called SessionBackup::AppendBundles end.");

    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
    } else if (!bundleInfos.empty()) {
        return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
    } else {
        NVal cb(env, funcArg[NARG_POS::SECOND]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(className, cbExec, cbCompl).val_;
    }
}

napi_value SessionBackupNExporter::Release(napi_env env, napi_callback_info cbinfo)
{
    HILOGD("called SessionBackup::Release begin");
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

    HILOGD("Called SessionBackup::Release end.");

    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(className, cbExec, cbCompl).val_;
}

napi_value SessionBackupNExporter::Cancel(napi_env env, napi_callback_info info)
{
    HILOGI("Called SessionBackup::Cancel begin");
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
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
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

    auto backupEntity = NClass::GetEntityOf<BackupEntity>(env, funcArg.GetThisVar());
    if (!(backupEntity && backupEntity->session)) {
        HILOGE("Failed to get backupSession entity.");
        return nullptr;
    }

    int result = backupEntity->session->Cancel(bundleName);
    napi_value nResult;
    napi_status status = napi_create_int32(env, result, &nResult);
    if (status != napi_ok) {
        HILOGE("napi_create_int32 faild.");
        return nullptr;
    }
    HILOGI("Cancel success with result: %{public}d", result);
    return nResult;
}

static NContextCBExec CleanBundleTempDirCBExec(napi_env env,
                                               const NFuncArg &funcArg, std::unique_ptr<char[]> bundleName)
{
    auto backupEntity = NClass::GetEntityOf<BackupEntity>(env, funcArg.GetThisVar());
    if (!(backupEntity && (backupEntity->session))) {
        HILOGE("Failed to get BackupSession entity.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get BackupSession entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    return [entity {backupEntity}, bundleName {std::string(bundleName.get())}]() -> NError {
        if (!(entity && (entity->session))) {
            return NError(BError(BError::Codes::SDK_INVAL_ARG, "Backup session is nullptr").GetCode());
        }
        return NError(entity->session->CleanBundleTempDir(bundleName));
    };
}

napi_value SessionBackupNExporter::CleanBundleTempDir(napi_env env, napi_callback_info cbinfo)
{
    HILOGI("Called SessionBackupNExporter::CleanBundleTempDir begin.");
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

bool SessionBackupNExporter::Export()
{
    HILOGD("called SessionBackupNExporter::Export begin");
    vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("getLocalCapabilities", GetLocalCapabilities),
        NVal::DeclareNapiFunction("getBackupDataSize", GetBackupDataSize),
        NVal::DeclareNapiFunction("appendBundles", AppendBundles),
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

    HILOGD("called SessionBackupNExporter::Export end");
    return exports_.AddProp(className, classValue);
}

napi_value SessionBackupNExporter::ConstructorFromEntity(napi_env env, napi_callback_info cbinfo)
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
    BackupEntity* entity =  reinterpret_cast<BackupEntity*>(entityRawPtr);
    if (entity == nullptr) {
        NError(BError(BError::Codes::SDK_INVAL_ARG, "First argument is not session pointer.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    std::unique_ptr<BackupEntity> backupEntity(entity);
    if (backupEntity->session == nullptr || backupEntity->callbacks == nullptr) {
        HILOGE("session or callback is null");
        return nullptr;
    }
    if (!SetSessionBackupEntity(env, funcArg, std::move(backupEntity))) {
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to set IncrBackupEntity entity.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    HILOGD("called ConstructorFromEntity end");
    return funcArg.GetThisVar();
}

napi_value SessionBackupNExporter::CreateByEntity(napi_env env, std::unique_ptr<BackupEntity> entity)
{
    HILOGD("CreateByEntity begin");
    if (entity == nullptr) {
        HILOGE("entity is null");
        return nullptr;
    }
    vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("getLocalCapabilities", GetLocalCapabilities),
        NVal::DeclareNapiFunction("getBackupDataSize", GetBackupDataSize),
        NVal::DeclareNapiFunction("appendBundles", AppendBundles),
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
        std::unique_ptr<BackupEntity> entity(static_cast<BackupEntity *>(data));
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
    BackupEntity* entityPtr = entity.release();
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

string SessionBackupNExporter::GetClassName()
{
    return SessionBackupNExporter::className;
}

SessionBackupNExporter::SessionBackupNExporter(napi_env env, napi_value exports) : NExporter(env, exports) {}

SessionBackupNExporter::~SessionBackupNExporter() {}
} // namespace OHOS::FileManagement::Backup