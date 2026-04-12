/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "grant_permissions.h"

#include <string>
#include <vector>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "js_native_api.h"
#include "log.h"
#include "n_napi.h"
#include "tokenid_kit.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileShare {
using namespace OHOS::FileManagement::LibN;
using namespace OHOS::Security::AccessToken;
using namespace std;

static napi_value GetErrData(napi_env env, deque<struct PolicyErrorResult> &errorResults)
{
    napi_value res = nullptr;
    napi_status status = napi_create_array(env, &res);
    if (status != napi_ok) {
        LOGE("Failed to create array");
        return nullptr;
    }
    size_t index = 0;
    for (auto &iter : errorResults) {
        NVal obj = NVal::CreateObject(env);
        obj.AddProp("uri", NVal::CreateUTF8String(env, iter.uri).val_);
        obj.AddProp("code", NVal::CreateInt32(env, iter.code).val_);
        obj.AddProp("message", NVal::CreateUTF8String(env, iter.message).val_);
        status = napi_set_element(env, res, index++, obj.val_);
        if (status != napi_ok) {
            LOGE("Failed to set element on data");
            return nullptr;
        }
    }
    return res;
}

static napi_value GetResultData(napi_env env, const vector<bool> &results)
{
    napi_value res = nullptr;
    napi_status status = napi_create_array(env, &res);
    if (status != napi_ok) {
        LOGE("Failed to create array");
        return nullptr;
    }
    size_t index = 0;
    for (const auto &iter : results) {
        napi_value value;
        napi_get_boolean(env, iter, &value);
        status = napi_set_element(env, res, index++, value);
        if (status != napi_ok) {
            LOGE("Failed to set element on data");
            return nullptr;
        }
    }
    return res;
}

static napi_value ConvertUriPoliciesToJSArray(napi_env env, const std::vector<UriPolicyInfo> &uriPolicies)
{
    napi_value res = nullptr;
    napi_status status = napi_create_array(env, &res);
    if (status != napi_ok) {
        LOGE("Failed to create array");
        napi_value undefined;
        napi_get_undefined(env, &undefined);
        return undefined;
    }

    for (size_t i = 0; i < uriPolicies.size(); i++) {
        const auto& uriPolicy = uriPolicies[i];
        
        napi_value policyObj;
        status = napi_create_object(env, &policyObj);
        if (status != napi_ok) {
            continue;
        }

        napi_value uriValue;
        status = napi_create_string_utf8(env, uriPolicy.uri.c_str(), NAPI_AUTO_LENGTH, &uriValue);
        if (status == napi_ok) {
            napi_set_named_property(env, policyObj, "uri", uriValue);
        }

        napi_value modeValue;
        status = napi_create_uint32(env, uriPolicy.mode, &modeValue);
        if (status == napi_ok) {
            napi_set_named_property(env, policyObj, "operationMode", modeValue);
        }

        status = napi_set_element(env, res, i, policyObj);
        if (status != napi_ok) {
            LOGE("Failed to set element on data");
        }
    }
    
    return res;
}

static napi_status GetUriPolicy(napi_env env, napi_value agrv, std::vector<UriPolicyInfo> &uriPolicies, uint32_t index)
{
    napi_value object;
    napi_status status = napi_get_element(env, agrv, index, &object);
    if (status != napi_ok) {
        LOGE("get element failed");
        return status;
    }
    napi_value uriValue;
    napi_value modeValue;
    status = napi_get_named_property(env, object, "uri", &uriValue);
    if (status != napi_ok) {
        LOGE("get named property failed");
        return status;
    }
    status = napi_get_named_property(env, object, "operationMode", &modeValue);
    if (status != napi_ok) {
        LOGE("get named property failed");
        return status;
    }
    auto [succStr, str, ignore] = NVal(env, uriValue).ToUTF8String();
    auto [succMode, mode] = NVal(env, modeValue).ToUint32();
    if (!succStr || !succMode) {
        LOGE("the argument error, succStr = %{public}d, succMode = %{public}d", succStr, succMode);
        return napi_invalid_arg;
    }
    UriPolicyInfo uriPolicy {.uri = str.get(), .mode = mode};
    uriPolicies.emplace_back(uriPolicy);
    return napi_ok;
}

static napi_status GetUriPoliciesArg(napi_env env, napi_value agrv, std::vector<UriPolicyInfo> &uriPolicies)
{
    uint32_t count;
    napi_status status = napi_get_array_length(env, agrv, &count);
    if (status != napi_ok) {
        LOGE("get array length failed");
        return status;
    }
    if (count > MAX_ARRAY_SIZE) {
        LOGE("The length of the array is extra-long");
        return napi_invalid_arg;
    }
    for (uint32_t i = 0; i < count; i++) {
        napi_handle_scope scope;
        status = napi_open_handle_scope(env, &scope);
        if (status != napi_ok || scope == nullptr) {
            return status;
        }
        status = GetUriPolicy(env, agrv, uriPolicies, i);
        if (status != napi_ok) {
            napi_close_handle_scope(env, scope);
            return status;
        }
        status = napi_close_handle_scope(env, scope);
        if (status != napi_ok) {
            return status;
        }
    }
    return napi_ok;
}

static napi_status CheckPathArray(napi_env env, napi_value agrv, uint32_t &count)
{
    napi_status status = napi_get_array_length(env, agrv, &count);
    if (status != napi_ok) {
        LOGE("get array length failed");
        return status;
    }
    if (count == 0 || count > MAX_ARRAY_SIZE) {
        LOGE("The length of the array is extra-long or length is 0");
        return napi_invalid_arg;
    }
    return napi_ok;
}

static napi_status GetPathPolicy(napi_env env,
                                 napi_value agrv,
                                 std::vector<PathPolicyInfo> &pathPolicies,
                                 uint32_t index)
{
    napi_value object;
    napi_status status = napi_get_element(env, agrv, index, &object);
    if (status != napi_ok) {
        LOGE("get element failed");
        return status;
    }
    napi_value pathValue;
    napi_value modeValue;
    status = napi_get_named_property(env, object, "path", &pathValue);
    if (status != napi_ok) {
        LOGE("get named property failed");
        return status;
    }
    status = napi_get_named_property(env, object, "operationMode", &modeValue);
    if (status != napi_ok) {
        LOGE("get named property failed");
        return status;
    }
    auto [succStr, str, ignore] = NVal(env, pathValue).ToUTF8String();
    auto [succMode, mode] = NVal(env, modeValue).ToUint32();
    if (!succStr || !succMode) {
        LOGE("the argument error, succStr = %{public}d, succMode = %{public}d", succStr, succMode);
        return napi_invalid_arg;
    }
    PathPolicyInfo pathPolicy {.path = str.get(), .mode = mode};
    pathPolicies.emplace_back(pathPolicy);
    return napi_ok;
}

static napi_status GetPathPoliciesArg(napi_env env, napi_value agrv, std::vector<PathPolicyInfo> &pathPolicies)
{
    uint32_t count;
    napi_status status = CheckPathArray(env, agrv, count);
    if (status != napi_ok) {
        return status;
    }
    for (uint32_t i = 0; i < count; i++) {
        napi_handle_scope scope;
        status = napi_open_handle_scope(env, &scope);
        if (status != napi_ok || scope == nullptr) {
            LOGE("open handle scope failed");
            return status;
        }
        status = GetPathPolicy(env, agrv, pathPolicies, i);
        if (status != napi_ok) {
            napi_close_handle_scope(env, scope);
            return status;
        }
        status = napi_close_handle_scope(env, scope);
        if (status != napi_ok) {
            return status;
        }
    }
    return napi_ok;
}

static bool IsSystemApp()
{
    uint64_t fullTokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
    return TokenIdKit::IsSystemAppByFullTokenID(fullTokenId);
}

napi_value PersistPermission(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("PersistPermission Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(env, funcArg[NARG_POS::FIRST], uriPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        std::tuple<uint32_t, std::string> errInfo =
            std::make_tuple(E_UNKNOWN_ERROR, "Out of memory, execute make_shared function failed");
        ErrParam errorParam = [errInfo]() { return errInfo; };
        NError(errorParam).ThrowErr(env);
        return nullptr;
    }
    auto cbExec = [uriPolicies, arg]() -> NError {
        arg->errNo = FilePermission::PersistPermission(uriPolicies, arg->errorResults);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (err) {
            if (arg->errNo == EPERM) {
                napi_value data = err.GetNapiErr(env);
                napi_set_named_property(env, data, FILEIO_TAG_ERR_DATA.c_str(), GetErrData(env, arg->errorResults));
                return NVal(env, data);
            }
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "persist_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

static napi_value HandleRevokeByTokenId(napi_env env, NFuncArg &funcArg, uint32_t tokenId)
{
    if (!IsSystemApp()) {
        LOGE("FileShare::HandleRevokeByTokenId is not System App!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = [tokenId {move(tokenId)}]() -> NError {
        int32_t ret = FilePermission::UnPersistPolicyByTokenId(tokenId);
        return NError(ret);
    };
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "revoke_permission_by_token_id";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

static napi_value HandleRevokeByUriPolicies(napi_env env, NFuncArg &funcArg)
{
    std::vector<UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(env, funcArg[NARG_POS::FIRST], uriPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        std::tuple<uint32_t, std::string> errInfo =
            std::make_tuple(E_UNKNOWN_ERROR, "Out of memory, execute make_shared function failed");
        ErrParam errorParam = [errInfo]() { return errInfo; };
        NError(errorParam).ThrowErr(env);
        return nullptr;
    }
    auto cbExec = [uriPolicies, arg]() -> NError {
        arg->errNo = FilePermission::RevokePermission(uriPolicies, arg->errorResults);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (err) {
            if (arg->errNo == EPERM) {
                napi_value data = err.GetNapiErr(env);
                napi_set_named_property(env, data, FILEIO_TAG_ERR_DATA.c_str(), GetErrData(env, arg->errorResults));
                return NVal(env, data);
            }
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "revoke_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

static napi_value HandleRevokeByTokenIdAndPolicies(napi_env env, NFuncArg &funcArg)
{
    if (!IsSystemApp()) {
        LOGE("FileShare::HandleRevokeByTokenIdAndPolicies is not System App!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }

    auto [succTokenId, tokenId] = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succTokenId || tokenId == 0) {
        LOGE("Failed to get tokenid or tokenid is 0");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    std::vector<UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(env, funcArg[NARG_POS::SECOND], uriPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }

    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        std::tuple<uint32_t, std::string> errInfo =
            std::make_tuple(E_UNKNOWN_ERROR, "Out of memory, execute make_shared function failed");
        ErrParam errorParam = [errInfo]() { return errInfo; };
        NError(errorParam).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = [tokenId {move(tokenId)}, uriPolicies, arg]() -> NError {
        arg->errNo = FilePermission::UnPersistPolicyByTokenIdAndPolicies(tokenId, uriPolicies, arg->errorResults);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (err) {
            if (arg->errNo == EPERM) {
                napi_value data = err.GetNapiErr(env);
                napi_set_named_property(env, data, FILEIO_TAG_ERR_DATA.c_str(), GetErrData(env, arg->errorResults));
                return NVal(env, data);
            }
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "revoke_permission_by_token_id_and_policies";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

napi_value RevokePermission(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        LOGE("RevokePermission Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }

    size_t argc = funcArg.GetArgc();
    if (argc == NARG_CNT::ONE) {
        napi_valuetype valuetype;
        napi_typeof(env, funcArg[NARG_POS::FIRST], &valuetype);

        if (valuetype == napi_number) {
            auto [succTokenId, tokenId] = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
            if (!succTokenId || tokenId == 0) {
                LOGE("Failed to get tokenid or tokenid is 0");
                NError(EINVAL).ThrowErr(env);
                return nullptr;
            }
            return HandleRevokeByTokenId(env, funcArg, tokenId);
        } else {
            return HandleRevokeByUriPolicies(env, funcArg);
        }
    } else if (argc == NARG_CNT::TWO) {
        return HandleRevokeByTokenIdAndPolicies(env, funcArg);
    } else {
        LOGE("RevokePermission invalid number of arguments");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
}

napi_value GetPersistentPolicy(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("GetPersistentPolicy Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }

    auto [succTokenId, tokenId] = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succTokenId || tokenId == 0) {
        LOGE("Failed to get tokenid or tokenid is 0");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    struct GetPolicyResult {
        int32_t errNo = 0;
        std::vector<UriPolicyInfo> uriPolicies;
    };
    shared_ptr<GetPolicyResult> arg = make_shared<GetPolicyResult>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        std::tuple<uint32_t, std::string> errInfo =
            std::make_tuple(E_UNKNOWN_ERROR, "Out of memory, execute make_shared function failed");
        ErrParam errorParam = [errInfo]() { return errInfo; };
        NError(errorParam).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = [tokenId {move(tokenId)}, arg]() -> NError {
        arg->errNo = FilePermission::GetPersistPolicyByTokenId(tokenId, arg->uriPolicies);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }

        napi_value res = ConvertUriPoliciesToJSArray(env, arg->uriPolicies);
        return NVal(env, res);
    };
    const string procedureName = "get_persist_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

napi_value ActivatePermission(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("ActivatePermission Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(env, funcArg[NARG_POS::FIRST], uriPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        std::tuple<uint32_t, std::string> errInfo =
            std::make_tuple(E_UNKNOWN_ERROR, "Out of memory, execute make_shared function failed");
        ErrParam errorParam = [errInfo]() { return errInfo; };
        NError(errorParam).ThrowErr(env);
        return nullptr;
    }
    auto cbExec = [uriPolicies, arg]() -> NError {
        arg->errNo = FilePermission::ActivatePermission(uriPolicies, arg->errorResults);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (err) {
            if (arg->errNo == EPERM) {
                napi_value data = err.GetNapiErr(env);
                napi_set_named_property(env, data, FILEIO_TAG_ERR_DATA.c_str(), GetErrData(env, arg->errorResults));
                return NVal(env, data);
            }
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "activate_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

napi_value DeactivatePermission(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("DeactivatePermission Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(env, funcArg[NARG_POS::FIRST], uriPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        std::tuple<uint32_t, std::string> errInfo =
            std::make_tuple(E_UNKNOWN_ERROR, "Out of memory, execute make_shared function failed");
        ErrParam errorParam = [errInfo]() { return errInfo; };
        NError(errorParam).ThrowErr(env);
        return nullptr;
    }
    auto cbExec = [uriPolicies, arg]() -> NError {
        arg->errNo = FilePermission::DeactivatePermission(uriPolicies, arg->errorResults);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (err) {
            if (arg->errNo == EPERM) {
                napi_value data = err.GetNapiErr(env);
                napi_set_named_property(env, data, FILEIO_TAG_ERR_DATA.c_str(), GetErrData(env, arg->errorResults));
                return NVal(env, data);
            }
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "deactivate_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

napi_value CheckPersistentPermission(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("ActivatePermission Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(env, funcArg[NARG_POS::FIRST], uriPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    shared_ptr<PolicyInfoResultArgs> arg = make_shared<PolicyInfoResultArgs>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        std::tuple<uint32_t, std::string> errInfo =
            std::make_tuple(E_UNKNOWN_ERROR, "Out of memory, execute make_shared function failed");
        ErrParam errorParam = [errInfo]() { return errInfo; };
        NError(errorParam).ThrowErr(env);
        return nullptr;
    }
    auto cbExec = [uriPolicies, arg]() -> NError {
        arg->errNo = FilePermission::CheckPersistentPermission(uriPolicies, arg->resultData);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (arg->errNo != 0) {
            return {env, err.GetNapiErr(env)};
        }
        return {env, GetResultData(env, arg->resultData)};
    };
    const string procedureName = "check_persist_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

static bool CheckTokenIdPermission(uint32_t tokenCaller, const string &permission)
{
    return AccessTokenKit::VerifyAccessToken(tokenCaller, permission) == PermissionState::PERMISSION_GRANTED;
}

static bool CheckArgs(napi_env env, napi_callback_info info, NFuncArg &funcArg)
{
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        LOGE("ActivatePermission Number of arguments unmatched");
        return false;
    }

    auto [succTokenId, tokenId] = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succTokenId || tokenId == 0) {
        LOGE("Failed to get tokenid or tokenid is 0");
        return false;
    }

    auto [succPolicyType, policyType] = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succPolicyType || policyType < TEMPORARY_TYPE || policyType > PERSISTENT_TYPE) {
        LOGE("Failed to get policy type or policy type is invalid");
        return false;
    }
    return true;
}

napi_value CheckPathPermission(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        LOGE("FileShare::CheckPathPermission is not System App!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }

    NFuncArg funcArg(env, info);
    if (!CheckArgs(env, info, funcArg)) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }

    auto [succTokenId, tokenId] = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();

    uint32_t callerTokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    if (tokenId != static_cast<int32_t>(callerTokenId)) {
        if (!CheckTokenIdPermission(callerTokenId, "ohos.permission.CHECK_SANDBOX_POLICY")) {
            NError(E_PERMISSION).ThrowErr(env);
            return nullptr;
        }
    }

    std::vector<PathPolicyInfo> pathPolicies;
    if (GetPathPoliciesArg(env, funcArg[NARG_POS::SECOND], pathPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }

    auto [succPolicyType, policyType] = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();

    LOGI("check permission target:%{public}d type:%{public}d", tokenId, policyType);

    shared_ptr<PolicyInfoResultArgs> arg = make_shared<PolicyInfoResultArgs>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        std::tuple<uint32_t, std::string> errInfo =
            std::make_tuple(E_UNKNOWN_ERROR, "Out of memory, execute make_shared function failed");
        ErrParam errorParam = [errInfo]() { return errInfo; };
        NError(errorParam).ThrowErr(env);
        return nullptr;
    }
    auto cbExec = [tokenId {move(tokenId)}, pathPolicies, policyType {move(policyType)}, arg]() -> NError {
        arg->errNo = FilePermission::CheckPathPermission(tokenId, pathPolicies, policyType, arg->resultData);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (arg->errNo != 0) {
            return {env, err.GetNapiErr(env)};
        }
        return {env, GetResultData(env, arg->resultData)};
    };
    const string procedureName = "check_path_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

napi_value GrantDecUriPermission(napi_env env, FileManagement::LibN::NFuncArg &funcArg)
{
    LOGI("GrantDecUriPermission");
    std::vector<UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(env, funcArg[NARG_POS::FIRST], uriPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }

    auto [succBundleName, bundleName, lenBundleName] = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succBundleName) {
        LOGE("FileShare::GetJSArgs get bundleName parameter failed!");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    std::string targetBundleName = string(bundleName.get());

    auto [succAppCloneIndex, appCloneIndex] = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();

    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("PolicyInfoResultArgs make_shared is failed");
        NError(E_NOMEM).ThrowErr(env);
        return nullptr;
    }
    auto cbExec = [uriPolicies, targetBundleName, appCloneIndex {move(appCloneIndex)}, arg]() -> NError {
        arg->errNo = FilePermission::GrantPermission(uriPolicies, targetBundleName, appCloneIndex, arg->errorResults);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (err) {
            if (arg->errNo == EPERM) {
                napi_value data = err.GetNapiErr(env);
                napi_set_named_property(env, data, FILEIO_TAG_ERR_DATA.c_str(), GetErrData(env, arg->errorResults));
                return NVal(env, data);
            }
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "grant_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

napi_value GrantSharedDirectoryPermission(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        LOGE("FileShare::GrantSharedDirectoryPermission is not System App!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }

    LOGI("GrantSharedDirectoryPermission called via NAPI");
    
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(0)) {
        LOGE("GrantSharedDirectoryPermission Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = []() -> NError {
        int32_t ret = FilePermission::GrantSharedDirectoryPermission();
        return NError(ret);
    };
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "grant_shared_directory_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

napi_value RevokeSharedDirectoryPermission(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        LOGE("FileShare::RevokeSharedDirectoryPermission is not System App!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }

    LOGI("RevokeSharedDirectoryPermission called via NAPI");
    
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(0)) {
        LOGE("RevokeSharedDirectoryPermission Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = []() -> NError {
        int32_t ret = FilePermission::RevokeSharedDirectoryPermission();
        return NError(ret);
    };
    auto cbCompl = [](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "revoke_shared_directory_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

napi_value GetSharedDirectoryInfo(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        LOGE("FileShare::GetSharedDirectoryInfo is not System App!");
        NError(E_PERMISSION_SYS).ThrowErr(env);
        return nullptr;
    }

    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(0)) {
        LOGE("GetSharedDirectoryInfo Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }

    // Create shared pointer to store result data
    shared_ptr<vector<SharedDirectoryInfo>> resultData = make_shared<vector<SharedDirectoryInfo>>();
    if (resultData == nullptr) {
        LOGE("GetSharedDirectoryInfo make_shared is failed");
        NError(E_NOMEM).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = [resultData]() -> NError {
        // Call the underlying FilePermission::GetSharedDirectoryInfo function
        int32_t ret = FilePermission::GetSharedDirectoryInfo(*resultData);
        return NError(ret);
    };
    
    auto cbCompl = [resultData](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        
        // Convert result to JavaScript array
        napi_value res = nullptr;
        napi_status status = napi_create_array(env, &res);
        if (status != napi_ok) {
            LOGE("Failed to create array for GetSharedDirectoryInfo");
            return NVal::CreateUndefined(env);
        }
        
        size_t index = 0;
        for (const auto &info : *resultData) {
            NVal obj = NVal::CreateObject(env);
            obj.AddProp("bundleName", NVal::CreateUTF8String(env, info.bundleName).val_);
            obj.AddProp("path", NVal::CreateUTF8String(env, info.path).val_);
            obj.AddProp("permissionMode", NVal::CreateUint32(env, info.permissionMode).val_);

            status = napi_set_element(env, res, index++, obj.val_);
            if (status != napi_ok) {
                LOGE("Failed to set element on data for GetSharedDirectoryInfo");
                return NVal::CreateUndefined(env);
            }
        }
        return NVal(env, res);
    };
    
    const string procedureName = "get_shared_directory_info";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

} // namespace ModuleFileShare
} // namespace AppFileService
} // namespace OHOS
