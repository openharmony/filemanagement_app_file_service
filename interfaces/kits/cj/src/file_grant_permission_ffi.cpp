/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "file_grant_permission_ffi.h"

#include "file_permission.h"
#include "macro.h"
#include "n_error.h"
#include "uv.h"

using namespace OHOS::FFI;

namespace OHOS {
namespace CJSystemapi {
namespace FileGrantPermission {

using namespace OHOS::AppFileService;
using namespace OHOS::FileManagement::LibN;

char* MallocCString(const std::string& origin)
{
    if (origin.empty()) {
        return nullptr;
    }
    auto length = origin.length() + 1;
    char* res = static_cast<char*>(malloc(sizeof(char) * length));
    if (res == nullptr) {
        return nullptr;
    }
    return std::char_traits<char>::copy(res, origin.c_str(), length);
}

int ConvertUVCode2ErrorCode(int errCode)
{
    if (errCode >= 0) {
        return errCode;
    }
    auto uvCode = string_view(uv_err_name(errCode));
    if (uvCode2ErrCodeTable.find(uvCode) != uvCode2ErrCodeTable.end()) {
        return uvCode2ErrCodeTable.at(uvCode);
    }
    return UNKROWN_ERR;
}

CJError ConvertError(int errCode)
{
    CJError err = { 0, "" };
    int genericCode = ConvertUVCode2ErrorCode(errCode);
    auto it = errCodeTable.find(genericCode);
    if (it != errCodeTable.end()) {
        err.errorcode = it->second.first;
        err.message = it->second.second;
    } else {
        err.errorcode = errCodeTable.at(UNKROWN_ERR).first;
        err.message = errCodeTable.at(UNKROWN_ERR).second + ", errno is " + to_string(abs(errCode));
    }
    return err;
}

extern "C" {
CArrCPolicyErrorResult FfiOHOSFileGPPersistPermission(CArrCPolicyInfo policies, int32_t* code)
{
    CArrCPolicyErrorResult res = { nullptr, 0 };
    if (policies.size > MAX_ARRAY_SIZE) {
        LOGE("The length of the array is extra-long");
        *code = E_PARAMS;
        return res;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (policies.head == nullptr) {
        return res;
    }
    for (int i = 0; i < policies.size; i++) {
        auto head = policies.head + i;
        std::string uriStr(head->uri);
        UriPolicyInfo uriPolicy { .uri = uriStr, .mode = head->mode };
        uriPolicies.emplace_back(uriPolicy);
    }

    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        *code = E_UNKNOWN_ERROR;
        return res;
    }
    arg->errNo = FilePermission::PersistPermission(uriPolicies, arg->errorResults);
    CJError err = ConvertError(arg->errNo);
    if (err.errorcode != 0) {
        *code = err.errorcode;
        res.size = static_cast<int32_t>(arg->errorResults.size());
        CPolicyErrorResult* retArrValue =
            static_cast<CPolicyErrorResult*>(malloc(sizeof(CPolicyErrorResult) * res.size));
        if (retArrValue == nullptr) {
            LOGE("malloc is failed");
            *code = E_UNKNOWN_ERROR;
            return res;
        }
        for (int i = 0; i < res.size; i++) {
            retArrValue[i].uri = MallocCString(arg->errorResults[i].uri);
            retArrValue[i].code = arg->errorResults[i].code;
            retArrValue[i].message = MallocCString(arg->errorResults[i].message);
        }
        res.head = retArrValue;
    }
    return res;
}

CArrCPolicyErrorResult FfiOHOSFileGPRevokePermission(CArrCPolicyInfo policies, int32_t* code)
{
    CArrCPolicyErrorResult res = { nullptr, 0 };
    if (policies.size > MAX_ARRAY_SIZE) {
        LOGE("The length of the array is extra-long");
        *code = E_PARAMS;
        return res;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (policies.head == nullptr) {
        return res;
    }
    for (int i = 0; i < policies.size; i++) {
        auto head = policies.head + i;
        std::string uriStr(head->uri);
        UriPolicyInfo uriPolicy { .uri = uriStr, .mode = head->mode };
        uriPolicies.emplace_back(uriPolicy);
    }

    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        *code = E_UNKNOWN_ERROR;
        return res;
    }
    arg->errNo = FilePermission::RevokePermission(uriPolicies, arg->errorResults);
    CJError err = ConvertError(arg->errNo);
    if (arg->errNo != 0) {
        *code = err.errorcode;
        res.size = static_cast<int32_t>(arg->errorResults.size());
        CPolicyErrorResult* retArrValue =
            static_cast<CPolicyErrorResult*>(malloc(sizeof(CPolicyErrorResult) * res.size));
        if (retArrValue == nullptr) {
            LOGE("malloc is failed");
            *code = E_UNKNOWN_ERROR;
            return res;
        }
        for (int i = 0; i < res.size; i++) {
            retArrValue[i].uri = MallocCString(arg->errorResults[i].uri);
            retArrValue[i].code = arg->errorResults[i].code;
            retArrValue[i].message = MallocCString(arg->errorResults[i].message);
        }
    }
    return res;
}

CArrCPolicyErrorResult FfiOHOSFileGPActivatePermission(CArrCPolicyInfo policies, int32_t* code)
{
    CArrCPolicyErrorResult res = { nullptr, 0 };
    if (policies.size > MAX_ARRAY_SIZE) {
        LOGE("The length of the array is extra-long");
        *code = E_PARAMS;
        return res;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (policies.head == nullptr) {
        return res;
    }
    for (int i = 0; i < policies.size; i++) {
        auto head = policies.head + i;
        std::string uriStr(head->uri);
        UriPolicyInfo uriPolicy { .uri = uriStr, .mode = head->mode };
        uriPolicies.emplace_back(uriPolicy);
    }

    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        *code = E_UNKNOWN_ERROR;
        return res;
    }
    arg->errNo = FilePermission::ActivatePermission(uriPolicies, arg->errorResults);
    CJError err = ConvertError(arg->errNo);
    if (arg->errNo != 0) {
        *code = err.errorcode;
        res.size = static_cast<int32_t>(arg->errorResults.size());
        CPolicyErrorResult* retArrValue =
            static_cast<CPolicyErrorResult*>(malloc(sizeof(CPolicyErrorResult) * res.size));
        if (retArrValue == nullptr) {
            LOGE("malloc is failed");
            *code = E_UNKNOWN_ERROR;
            return res;
        }
        for (int i = 0; i < res.size; i++) {
            retArrValue[i].uri = MallocCString(arg->errorResults[i].uri);
            retArrValue[i].code = arg->errorResults[i].code;
            retArrValue[i].message = MallocCString(arg->errorResults[i].message);
        }
    }
    return res;
}

CArrCPolicyErrorResult FfiOHOSFileGPDeactivatePermission(CArrCPolicyInfo policies, int32_t* code)
{
    CArrCPolicyErrorResult res = { nullptr, 0 };
    if (policies.size > MAX_ARRAY_SIZE) {
        LOGE("The length of the array is extra-long");
        *code = E_PARAMS;
        return res;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (policies.head == nullptr) {
        return res;
    }
    for (int i = 0; i < policies.size; i++) {
        auto head = policies.head + i;
        std::string uriStr(head->uri);
        UriPolicyInfo uriPolicy { .uri = uriStr, .mode = head->mode };
        uriPolicies.emplace_back(uriPolicy);
    }

    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        *code = E_UNKNOWN_ERROR;
        return res;
    }
    arg->errNo = FilePermission::DeactivatePermission(uriPolicies, arg->errorResults);
    CJError err = ConvertError(arg->errNo);
    if (arg->errNo != 0) {
        *code = err.errorcode;
        res.size = static_cast<int32_t>(arg->errorResults.size());
        CPolicyErrorResult* retArrValue =
            static_cast<CPolicyErrorResult*>(malloc(sizeof(CPolicyErrorResult) * res.size));
        if (retArrValue == nullptr) {
            LOGE("malloc is failed");
            *code = E_UNKNOWN_ERROR;
            return res;
        }
        for (int i = 0; i < res.size; i++) {
            retArrValue[i].uri = MallocCString(arg->errorResults[i].uri);
            retArrValue[i].code = arg->errorResults[i].code;
            retArrValue[i].message = MallocCString(arg->errorResults[i].message);
        }
    }
    return res;
}

CArrBool FfiOHOSFileGPCheckPersistentPermission(CArrCPolicyInfo policies, int32_t* code)
{
    CArrBool res = { nullptr, 0 };
    if (policies.size > MAX_ARRAY_SIZE) {
        LOGE("The length of the array is extra-long");
        *code = E_PARAMS;
        return res;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (policies.head == nullptr) {
        return res;
    }
    for (int i = 0; i < policies.size; i++) {
        auto head = policies.head + i;

        std::string uriStr(head->uri);
        UriPolicyInfo uriPolicy { .uri = uriStr, .mode = head->mode };
        uriPolicies.emplace_back(uriPolicy);
    }

    shared_ptr<PolicyInfoResultArgs> arg = make_shared<PolicyInfoResultArgs>();
    if (arg == nullptr) {
        LOGE("Make_shared is failed");
        *code = E_UNKNOWN_ERROR;
        return res;
    }
    arg->errNo = FilePermission::CheckPersistentPermission(uriPolicies, arg->resultData);
    CJError err = ConvertError(arg->errNo);
    if (err.errorcode != 0) {
        *code = err.errorcode;
    } else {
        res.size = static_cast<int32_t>(arg->resultData.size());
        bool* retArrValue = static_cast<bool*>(malloc(sizeof(bool) * res.size));
        if (retArrValue == nullptr) {
            LOGE("malloc is failed");
            *code = E_UNKNOWN_ERROR;
            return res;
        }
        for (int i = 0; i < res.size; i++) {
            retArrValue[i] = arg->resultData[i];
        }
        res.head = retArrValue;
    }
    return res;
}
}
} // namespace FileGrantPermission
} // namespace CJSystemapi
} // namespace OHOS