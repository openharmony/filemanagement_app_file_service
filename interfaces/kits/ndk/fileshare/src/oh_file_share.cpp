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

#include "oh_file_share.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "securec.h"
#include "tokenid_kit.h"
#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include "file_permission.h"
#include "native_error.h"

using namespace std;
const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_PERSIST";
const char *g_fullMountEnableParameter = "const.filemanager.full_mount.enable";
using Exec = std::function<int(const vector<OHOS::AppFileService::UriPolicyInfo> &uriPolicies,
                                         deque<struct OHOS::AppFileService::PolicyErrorResult> &errorResults)>;
static bool CheckPermission(const string &permission)
{
    OHOS::Security::AccessToken::AccessTokenID tokenCaller = OHOS::IPCSkeleton::GetCallingTokenID();
    return OHOS::Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission) == OHOS::Security::AccessToken::PermissionState::PERMISSION_GRANTED;
}

bool ConvertPolicyInfo(const FileShare_PolicyInfo *policies, int policyNum, std::vector<OHOS::AppFileService::UriPolicyInfo> &uriPolicies)
{
    if (policies == nullptr || policyNum <= 0) {
        return false;
    }
    for (int32_t i = 0; i < policyNum; i++) {
        OHOS::AppFileService::UriPolicyInfo policyInfo;
        policyInfo.uri = policies[i].uri;
        policyInfo.mode = policies[i].operationMode;
        uriPolicies.push_back(policyInfo);
    }
    return true;
}

bool ConvertPolicyErrorResult(const deque<OHOS::AppFileService::PolicyErrorResult> &errorResults, FileShare_PolicyErrorResult **result)
{
    auto count = errorResults.size();
    *result = (FileShare_PolicyErrorResult*)malloc(count * sizeof(FileShare_PolicyErrorResult));
    if (*result == nullptr) {
        return false;
    }
    for (uint32_t i = 0; i < count; i++) {
       int size = errorResults[i].uri.size() + 1;
       (*result)[i].uri = (char*)malloc(size);
       strcpy_s((*result)[i].uri, size, errorResults[i].uri.c_str());
       (*result)[i].code = static_cast<FileShare_PolicyErrorCode>(errorResults[i].code);
       size = errorResults[i].message.size() + 1;
       (*result)[i].message = (char*)malloc(size);
       strcpy_s((*result)[i].message, size, errorResults[i].message.c_str());
    }
    return true;
}

bool ConvertPolicyErrorResultBool(const vector<bool> &errorResults, bool **result)
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

OH_FileManagement_ErrCode ExecAction(const FileShare_PolicyInfo *policies, unsigned int policyNum, FileShare_PolicyErrorResult **result, unsigned int *resultNum, Exec exec)
{
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        return E_PERMISSION;
    }
    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (!ConvertPolicyInfo(policies, policyNum, uriPolicies)) {
        return E_PARAMS;
    }
    deque<OHOS::AppFileService::PolicyErrorResult> errorResults;
    auto ret = exec(uriPolicies, errorResults);
    (*resultNum) = errorResults.size();
    if (ret == 0) {
        return E_NO_ERROR;
    }
    if (!ConvertPolicyErrorResult(errorResults, result)) {
        return E_UNKNOWN_ERROR;
    }
    return E_NO_ERROR;
}
OH_FileManagement_ErrCode OH_FileShare_PersistPermission(const FileShare_PolicyInfo *policies, unsigned int policyNum, FileShare_PolicyErrorResult **result, unsigned int *resultNum)
{
    return ExecAction(policies, policyNum, result, resultNum, OHOS::AppFileService::FilePermission::PersistPermission);
}

OH_FileManagement_ErrCode OH_FileShare_RevokePermission(const FileShare_PolicyInfo *policies, unsigned int policyNum, FileShare_PolicyErrorResult **result, unsigned int *resultNum)
{
    return ExecAction(policies, policyNum, result, resultNum, OHOS::AppFileService::FilePermission::RevokePermission);
}

OH_FileManagement_ErrCode OH_FileShare_ActivatePermission(const FileShare_PolicyInfo *policies, unsigned int policyNum, FileShare_PolicyErrorResult **result, unsigned int *resultNum)
{
    return ExecAction(policies, policyNum, result, resultNum, OHOS::AppFileService::FilePermission::ActivatePermission);
}

OH_FileManagement_ErrCode OH_FileShare_DeactivatePermission(const FileShare_PolicyInfo *policies, unsigned int policyNum, FileShare_PolicyErrorResult **result, unsigned int *resultNum)
{
    return ExecAction(policies, policyNum, result, resultNum, OHOS::AppFileService::FilePermission::DeactivatePermission);
}

OH_FileManagement_ErrCode OH_FileShare_CheckPersistentPermission(const FileShare_PolicyInfo *policies, unsigned int policyNum, bool **result, unsigned int *resultNum)
{
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        return E_PERMISSION;
    }
    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (!ConvertPolicyInfo(policies, policyNum, uriPolicies)) {
        return E_PARAMS;
    }
    vector<bool> errorResults;
    auto ret = OHOS::AppFileService::FilePermission::CheckPersistentPermission(uriPolicies, errorResults);
    if (ret != 0) {
        return E_UNKNOWN_ERROR;
    }
    (*resultNum) = errorResults.size();
    if (!ConvertPolicyErrorResultBool(errorResults, result)) {
        return E_UNKNOWN_ERROR;
    }
    return E_NO_ERROR;
}

void OH_FileShare_ReleasePolicyErrorResult(FileShare_PolicyErrorResult *result, unsigned int num)
{
    for(unsigned i = 0; i < num; i++) {
       free(result[i].uri);
       free(result[i].message);
    }
    free(result); 
}