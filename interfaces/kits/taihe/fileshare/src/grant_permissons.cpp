/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "grant_permissons.h"
#include "log.h"
#include "oh_file_share.h"
#include <iostream>

namespace ANI::FileShare {

ohos::fileshare::PolicyInfo makePolicyInfo(taihe::string_view uri, int32_t operationMode)
{
    return {uri, operationMode};
}

int32_t GetUriPoliciesArg(taihe::array_view<ohos::fileshare::PolicyInfo> policies,
    std::vector<OHOS::AppFileService::UriPolicyInfo> &uriPolicies)
{
    uint32_t count = policies.size();
    if (count > OHOS::AppFileService::MAX_ARRAY_SIZE) {
        LOGE("The length of the array is extra-long");
        return E_PARAMS;
    }
    for (uint32_t i = 0; i < count; i++) {
        OHOS::AppFileService::UriPolicyInfo uriPolicy;
        uriPolicy.uri = policies[i].uri;
        uriPolicy.mode = policies[i].operationMode;
        if (uriPolicy.uri == "") {
            LOGE("URI is empty");
            return E_PARAMS;
        }
        if (uriPolicy.mode != READ_MODE &&
            uriPolicy.mode != (READ_MODE | WRITE_MODE)) {
            LOGE("Invalid operation mode");
            return E_PARAMS;
        }
        uriPolicies.emplace_back(uriPolicy);
    }
    return E_NO_ERROR;
}

void activatePermissionSync(taihe::array_view<ohos::fileshare::PolicyInfo> policies)
{
    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(policies, uriPolicies)) {
        LOGE("Failed to get URI policies");
        taihe::set_business_error(E_PARAMS, "Failed to get URI policies");
        return ;
    }

    std::shared_ptr<PolicyErrorArgs> arg = std::make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("PolicyErrorArgs make make_shared failed");
        taihe::set_business_error(E_UNKNOWN_ERROR, "PolicyErrorArgs make make_shared failed");
        return ;
    }

    arg->errNo = OHOS::AppFileService::FilePermission::ActivatePermission(uriPolicies, arg->errorResults);
    if (arg->errNo) {
        LOGE("Activation failed");
        taihe::set_business_error(arg->errNo, "Activation failed");
    }
}

void deactivatePermissionSync(taihe::array_view<ohos::fileshare::PolicyInfo> policies)
{
    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(policies, uriPolicies)) {
        LOGE("Failed to get URI policies");
        taihe::set_business_error(E_PARAMS, "Failed to get URI policies");
        return ;
    }

    std::shared_ptr<PolicyErrorArgs> arg = std::make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("PolicyErrorArgs make make_shared failed");
        taihe::set_business_error(E_UNKNOWN_ERROR, "PolicyErrorArgs make make_shared failed");
        return ;
    }

    arg->errNo = OHOS::AppFileService::FilePermission::DeactivatePermission(uriPolicies, arg->errorResults);
    if (arg->errNo) {
        LOGE("Deactivation failed");
        taihe::set_business_error(arg->errNo, "Deactivation failed");
        return ;
    }
}
} // namespace ANI::FileShare

// NOLINTBEGIN
TH_EXPORT_CPP_API_makePolicyInfo(ANI::FileShare::makePolicyInfo);
TH_EXPORT_CPP_API_activatePermissionSync(ANI::FileShare::activatePermissionSync);
TH_EXPORT_CPP_API_deactivatePermissionSync(ANI::FileShare::deactivatePermissionSync);
 // NOLINTEND
