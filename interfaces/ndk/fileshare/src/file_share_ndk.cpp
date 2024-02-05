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

#include "file_share_ndk.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "parameter.h"
#include "securec.h"
#include "tokenid_kit.h"
#include <cstdint>
#include <iostream>
#include "file_permission.h"

using namespace OHOS;
using namespace OHOS::AppFileService;
using namespace OHOS::Security::AccessToken;
const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_PERSIST";
const char *g_fullMountEnableParameter = "const.filemanager.full_mount.enable";
using Exec = std::function<int(const vector<UriPolicyInfo> &uriPolicies,
                                         deque<struct PolicyErrorResult> &errorResults)>;
static bool CheckPermission(const string &permission)
{
    AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    return AccessTokenKit::VerifyAccessToken(tokenCaller, permission) == PermissionState::PERMISSION_GRANTED;
}

static bool CheckFileManagerFullMountEnable()
{
    char value[] = "false";
    int retSystem = GetParameter(g_fullMountEnableParameter, "false", value, sizeof(value));
    if (retSystem > 0 && !strcmp(value, "true")) {
        LOGE("The full mount enable parameter is true");
        return true;
    }
    LOGD("The full mount enable parameter is false");
    return false;
}

bool ConvertPolicyInfo(const OH_PolicyInfo *policies, int policyNum, std::vector<UriPolicyInfo> &uriPolicies)
{
    if (policies == nullptr || policyNum <= 0) {
        return false;
    }
    for (int32_t i = 0; i < policyNum; i++) {
        UriPolicyInfo policyInfo;
        policyInfo.uri = policies[i].uri;
        policyInfo.mode = policies[i].operationMode;
        uriPolicies.push_back(policyInfo);
    }
    return true;
}

bool ConvertPolicyErrorResult(const deque<PolicyErrorResult> &errorResults, OH_PolicyErrorResult **result)
{
    auto count = errorResults.size();
    *result = (OH_PolicyErrorResult*)malloc(count * sizeof(OH_PolicyErrorResult));
    if (*result == nullptr) {
        return false;
    }
    for (int32_t i = 0; i < count; i++) {
       strcpy_s((*result)[i].uri, errorResults[i].uri.size() + 1, errorResults[i].uri.c_str());
       (*result)[i].code = static_cast<OH_PolicyErrorCode>(errorResults[i].code);
       strcpy_s((*result)[i].message, errorResults[i].message.size() + 1, errorResults[i].message.c_str());
    }
    return true;
}

bool ConvertPolicyErrorResult(const vector<bool> &errorResults, bool **result)
{
    auto count = errorResults.size();
    *result = (bool*)malloc(count * sizeof(bool));
    if (*result == nullptr) {
        return false;
    }
    for (uint32_t i = 0; i < count; i++) {
       (*result)[i] = errorResults[i];
    }
    return true;
}

int ExecAction(const OH_PolicyInfo *policies, unsigned int policyNum, OH_PolicyErrorResult **result, unsigned int *resultNum, Exec exec)
{
    if (!CheckFileManagerFullMountEnable()) {
        return -1;
    }
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        return -1;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (!ConvertPolicyInfo(policies, policyNum, uriPolicies)) {
        return -1;
    }
    deque<PolicyErrorResult> errorResults;
    auto ret = exec(uriPolicies, errorResults);
    (*resultNum) = errorResults.size();
    if (ret == 0) {
        return ret;
    }
    if (!ConvertPolicyErrorResult(errorResults, result)) {
        return -1;
    }
    return ret;
}
int OH_FileShare_PersistPermission(const OH_PolicyInfo *policies, unsigned int policyNum, OH_PolicyErrorResult **result, unsigned int *resultNum)
{
    return ExecAction(policies, policyNum, result, resultNum, FilePermission::PersistPermission);
}

int OH_FileShare_RevokePermission(const OH_PolicyInfo *policies, unsigned int policyNum, OH_PolicyErrorResult **result, unsigned int *resultNum)
{
    return ExecAction(policies, policyNum, result, resultNum, FilePermission::RevokePermission);
}

int OH_FileShare_ActivatePermission(const OH_PolicyInfo *policies, unsigned int policyNum, OH_PolicyErrorResult **result, unsigned int *resultNum)
{
    return ExecAction(policies, policyNum, result, resultNum, FilePermission::ActivatePermission);
}

int OH_FileShare_DeactivatePermission(const OH_PolicyInfo *policies, unsigned int policyNum, OH_PolicyErrorResult **result, unsigned int *resultNum)
{
    return ExecAction(policies, policyNum, result, resultNum, FilePermission::DeactivatePermission);
}

int OH_FileShare_CheckPersistentPermission(const OH_PolicyInfo *policies, unsigned int policyNum, bool **result, unsigned int *resultNum)
{
    if (!CheckFileManagerFullMountEnable()) {
        return -1;
    }
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        return -1;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (!ConvertPolicyInfo(policies, policyNum, uriPolicies)) {
        return -1;
    }
    vector<bool> errorResults;
    auto ret = FilePermission::CheckPersistentPermission(uriPolicies, errorResults);
    (*resultNum) = errorResults.size();
    if (!ConvertPolicyErrorResult(errorResults, result)) {
        return -1;
    }
    return ret;
}