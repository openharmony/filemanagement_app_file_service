/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "prop_n_operation.h"

#include "b_error/b_error.h"
#include "b_incremental_data.h"
#include "b_resources/b_constants.h"
#include "b_sa/b_sa_utils.h"
#include "filemgmt_libhilog.h"
#include "filemgmt_libn.h"
#include "incremental_backup_data.h"
#include "service_proxy.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace LibN;

static napi_value AsyncCallback(napi_env env, const NFuncArg& funcArg)
{
    HILOGD("called LocalCapabilities::AsyncCallback begin");

    auto fd = make_shared<UniqueFd>();
    auto cbExec = [fd]() -> NError {
        HILOGI("called LocalCapabilities::AsyncCallback cbExec");
        ServiceProxy::InvaildInstance();
        auto proxy = ServiceProxy::GetInstance();
        if (!proxy) {
            HILOGI("called LocalCapabilities::AsyncCallback cbExec, failed to get proxy");
            return NError(errno);
        }
        *fd = proxy->GetLocalCapabilities();
        HILOGI("called LocalCapabilities::AsyncCallback cbExec success");
        return NError(ERRNO_NOERR);
    };
    auto cbCompl = [fd](napi_env env, NError err) -> NVal {
        HILOGI("called LocalCapabilities::AsyncCallback cbCompl");
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        NVal obj = NVal::CreateObject(env);
        obj.AddProp({NVal::DeclareNapiProperty(BConstants::FD.c_str(), NVal::CreateInt32(env, fd->Release()).val_)});
        return {obj};
    };

    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ZERO) {
        HILOGI("called LocalCapabilities::Async::promise");
        return NAsyncWorkPromise(env, thisVar).Schedule(PROCEDURE_LOCALCAPABILITIES_NAME, cbExec, cbCompl).val_;
    } else {
        HILOGI("called LocalCapabilities::Async::callback");
        NVal cb(env, funcArg[NARG_POS::FIRST]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(PROCEDURE_LOCALCAPABILITIES_NAME, cbExec, cbCompl).val_;
    }
}

static bool CheckDataList(const LibN::NVal &data)
{
    LibN::NVal name = data.GetProp(BConstants::BUNDLE_NAME);
    if (name.val_ == nullptr) {
        return false;
    }
    auto [succ, str, ignore] = name.ToUTF8String();
    if (!succ) {
        return false;
    }

    LibN::NVal time = data.GetProp(BConstants::LAST_INCREMENTAL_TIME);
    if (time.val_ == nullptr) {
        return false;
    }
    tie(succ, ignore) = time.ToInt64();
    if (!succ) {
        return false;
    }
    return true;
}

static std::tuple<bool, std::vector<BIncrementalData>> ParseDataList(napi_env env, const napi_value& value)
{
    uint32_t size = 0;
    napi_status status = napi_get_array_length(env, value, &size);
    if (status != napi_ok) {
        HILOGE("Get array length failed!");
        return {false, {}};
    }
    if (size == 0) {
        HILOGI("array length is zero!");
        return {true, {}};
    }

    napi_value result;
    std::vector<BIncrementalData> backupData;
    for (uint32_t i = 0; i < size; i++) {
        status = napi_get_element(env, value, i, &result);
        if (status != napi_ok) {
            HILOGE("Get element failed! index is :%{public}u", i);
            return {false, {}};
        } else {
            NVal element(env, result);
            if (!CheckDataList(element)) {
                HILOGE("bundles are invalid!");
                return {false, {}};
            }
            IncrementalBackupData data(element);
            backupData.emplace_back(data.bundleName,
                                    data.lastIncrementalTime,
                                    data.manifestFd,
                                    data.parameters,
                                    data.priority);
        }
    }
    return {true, backupData};
}

static napi_value AsyncDataList(napi_env env, const NFuncArg& funcArg)
{
    HILOGD("called LocalCapabilities::AsyncDataList begin");

    auto [succ, bundles] = ParseDataList(env, funcArg[NARG_POS::FIRST]);
    if (!succ) {
        HILOGE("bundles array invalid.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "bundles array invalid.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto fd = make_shared<UniqueFd>();
    auto cbExec = [fd, bundles { move(bundles) }]() -> NError {
        HILOGI("called LocalCapabilities::AsyncDataList cbExec");
        ServiceProxy::InvaildInstance();
        auto proxy = ServiceProxy::GetInstance();
        if (!proxy) {
            HILOGI("called LocalCapabilities::AsyncDataList cbExec, failed to get proxy");
            return NError(errno);
        }
        *fd = proxy->GetLocalCapabilitiesIncremental(bundles);
        return NError(ERRNO_NOERR);
    };
    auto cbCompl = [fd](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        NVal obj = NVal::CreateObject(env);
        obj.AddProp({NVal::DeclareNapiProperty(BConstants::FD.c_str(), NVal::CreateInt32(env, fd->Release()).val_)});
        return {obj};
    };

    HILOGI("called LocalCapabilities::Async::promise");
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(PROCEDURE_LOCALCAPABILITIES_NAME, cbExec, cbCompl).val_;
}

napi_value PropNOperation::Async(napi_env env, napi_callback_info info)
{
    HILOGD("called LocalCapabilities::Async begin");
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
    if (!funcArg.InitArgs(NARG_CNT::ZERO, NARG_CNT::ONE)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    if (funcArg.GetArgc() == 1) {
        bool result = 0;
        napi_status status = napi_is_array(env, funcArg[NARG_POS::FIRST], &result);
        if (status == napi_ok && result) {
            return AsyncDataList(env, funcArg);
        }
    }

    return AsyncCallback(env, funcArg);
}

napi_value PropNOperation::DoGetBackupInfo(napi_env env, napi_callback_info info)
{
    HILOGD("called DoGetBackupInfo begin");
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
    std::string result;
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return nullptr;
    }
    NVal jsBundle(env, funcArg[NARG_POS::FIRST]);
    auto [succ, bundle, size] = jsBundle.ToUTF8String();
    if (!succ) {
        HILOGE("First argument is not string.");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    ServiceProxy::InvaildInstance();
    auto proxy = ServiceProxy::GetInstance();
    if (!proxy) {
        HILOGE("called DoGetBackupInfo,failed to get proxy");
        return nullptr;
    }
    std::string bundleName = bundle.get();
    ErrCode errcode = proxy->GetBackupInfo(bundleName, result);
    if (errcode != 0) {
        HILOGE("proxy->GetBackupInfo faild.");
        return nullptr;
    }

    napi_value nResult;
    napi_status status = napi_create_string_utf8(env, result.c_str(), result.size(), &nResult);
    if (status != napi_ok) {
        HILOGE("napi_create_string_utf8 faild.");
        return nullptr;
    }
    HILOGI("DoGetBackupInfo success with result: %{public}s", result.c_str());
    return nResult;
}

bool PropNOperation::UpdateSendRate(std::string &bundleName, int32_t sendRate)
{
    bool result = false;
    ServiceProxy::InvaildInstance();
    auto proxy = ServiceProxy::GetInstance();
    if (!proxy) {
        HILOGE("called UpdateSendRate,failed to get proxy");
        return result;
    }
    ErrCode errCode = proxy->UpdateSendRate(bundleName, sendRate, result);
    if (errCode != 0) {
        HILOGE("Proxy execute UpdateSendRate failed. errCode:%{public}d", errCode);
        return result;
    }
    return result;
}

bool PropNOperation::UpdateTimer(std::string &bundleName, uint32_t timeout)
{
    bool result = false;
    ServiceProxy::InvaildInstance();
    auto proxy = ServiceProxy::GetInstance();
    if (!proxy) {
        HILOGE("called DoUpdateTimer,failed to get proxy");
        return result;
    }
    ErrCode errcode = proxy->UpdateTimer(bundleName, timeout, result);
    if (errcode != 0) {
        HILOGE("proxy->UpdateTimer faild.");
        return result;
    }
    return result;
}

napi_value PropNOperation::DoUpdateTimer(napi_env env, napi_callback_info info)
{
    HILOGD("called DoUpdateTimer begin");
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
        HILOGE("First argument is not string.");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    NVal jsBundleInt(env, funcArg[NARG_POS::SECOND]);
    auto [succInt, time] = jsBundleInt.ToInt32();
    if (!succInt || time < 0 || time > static_cast<int32_t>(BConstants::MAX_UPDATE_TIMER)) {
        HILOGE("Second argument is not number.");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }

    std::string bundleName = bundle.get();
    uint32_t timeout = static_cast<uint32_t>(time);
    bool result = UpdateTimer(bundleName, timeout);

    napi_value nResult;
    napi_status status = napi_get_boolean(env, result, &nResult);
    if (status != napi_ok) {
        HILOGE("napi_get_boolean faild.");
        return nullptr;
    }
    HILOGI("DoUpdateTimer success with result: %{public}d", result);
    return nResult;
}

napi_value PropNOperation::DoUpdateSendRate(napi_env env, napi_callback_info info)
{
    HILOGD("called DoUpdateSendRate begin");
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
        HILOGE("First argument is not string.");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    std::string bundleName = bundle.get();
    NVal jsBundleInt(env, funcArg[NARG_POS::SECOND]);
    auto [succInt, jsRate] = jsBundleInt.ToInt32();
    if (!succInt || jsRate < 0) {
        HILOGE("Second argument is invalid. bundleName:%{public}s", bundleName.c_str());
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    int32_t sendFdRate = static_cast<int32_t>(jsRate);
    if (sendFdRate > BConstants::MAX_FD_SEND_RATE) {
        HILOGI("sendFdRate is too large, %{public}d", sendFdRate);
        sendFdRate = BConstants::MAX_FD_SEND_RATE;
    }
    bool result = UpdateSendRate(bundleName, sendFdRate);
    napi_value nResult;
    napi_status status = napi_get_boolean(env, result, &nResult);
    if (status != napi_ok) {
        HILOGE("napi_get_boolean failed.");
        return nullptr;
    }
    HILOGI("DoUpdateSendRate success with result: %{public}d", result);
    return nResult;
}

napi_value PropNOperation::DoGetBackupVersion(napi_env env, napi_callback_info info)
{
    HILOGD("called DoGetBackupVersion begin");
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
    std::string result = BConstants::BACKUP_VERSION;
    napi_value nResult;
    napi_status status = napi_create_string_utf8(env, result.c_str(), result.size(), &nResult);
    if (status != napi_ok) {
        HILOGE("napi_create_string_utf8 faild.");
        return nullptr;
    }
    HILOGI("DoGetBackupVersion success with result: %{public}s", result.c_str());
    return nResult;
}
} // namespace OHOS::FileManagement::Backup