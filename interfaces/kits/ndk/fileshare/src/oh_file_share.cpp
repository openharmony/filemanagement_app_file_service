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
#include "file_permission.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "parameter.h"
#include "securec.h"
#include "tokenid_kit.h"
#include <cstdint>
#include <iostream>
#include <cstdlib>

constexpr int32_t FOO_MAX_LEN = 24000;  // sizeof(FileShare_PolicyErrorResult) * 500
const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_PERSIST";
const char *g_fullMountEnableParameter = "const.filemanager.full_mount.enable";
using Exec = std::function<int(const std::vector<OHOS::AppFileService::UriPolicyInfo> &uriPolicies,
                               std::deque<struct OHOS::AppFileService::PolicyErrorResult> &errorResults)>;
static bool CheckPermission(const std::string &permission)
{
    OHOS::Security::AccessToken::AccessTokenID tokenCaller = OHOS::IPCSkeleton::GetCallingTokenID();
    return OHOS::Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission) ==
           OHOS::Security::AccessToken::PermissionState::PERMISSION_GRANTED;
}

static bool CheckFileManagerFullMountEnable()
{
    char value[] = "false";
    int retSystem = GetParameter(g_fullMountEnableParameter, "false", value, sizeof(value));
    if (retSystem > 0 && !strcmp(value, "true")) {
        LOGI("The full mount enable parameter is true");
        return true;
    }
    LOGI("The full mount enable parameter is false");
    return false;
}

bool ConvertPolicyInfo(const FileShare_PolicyInfo *policies,
                       int policyNum,
                       std::vector<OHOS::AppFileService::UriPolicyInfo> &uriPolicies)
{
    if (policies == nullptr || policyNum <= 0) {
        LOGE("The policies pointer is nullptr or policyNum is 0");
        return false;
    }
    for (int32_t i = 0; i < policyNum; i++) {
        OHOS::AppFileService::UriPolicyInfo policyInfo;
        if (policies[i].uri == nullptr || policies[i].length == 0) {
            LOGE("The uri pointer is nullptr or length is 0");
            return false;
        }
        auto uriLength = strnlen(policies[i].uri, policies[i].length);
        if (uriLength != policies[i].length) {
            LOGE("The uri length abnormal");
            return false;
        }
        policyInfo.uri = std::string(policies[i].uri, policies[i].length);
        policyInfo.mode = policies[i].operationMode;
        uriPolicies.push_back(policyInfo);
    }
    return true;
}

bool ConvertPolicyErrorResult(const std::deque<OHOS::AppFileService::PolicyErrorResult> &errorResults,
                              FileShare_PolicyErrorResult **result,
                              unsigned int *resultNum)
{
    *resultNum = 0;
    auto count = errorResults.size();
    auto memorySize = count * sizeof(FileShare_PolicyErrorResult);
    if (memorySize == 0 || memorySize > FOO_MAX_LEN) {
        LOGE("malloc size is abnormal.");
        return false;
    }
    *result = (FileShare_PolicyErrorResult *)malloc(memorySize);
    if (*result == nullptr) {
        LOGE("*result is nullptr");
        return false;
    }
    for (uint32_t i = 0; i < count; i++) {
        int size = errorResults[i].uri.size() + 1;
        (*result)[i].uri = (char *)malloc(size);
        auto ret = strcpy_s((*result)[i].uri, size, errorResults[i].uri.c_str());
        if (ret != 0) {
            LOGE("strcpy uri failed uri:%{public}s, errno:%{public}d", errorResults[i].uri.c_str(), ret);
            *resultNum = i;
            free((*result)[i].uri);
            return false;
        }
        (*result)[i].code = static_cast<FileShare_PolicyErrorCode>(errorResults[i].code);
        size = errorResults[i].message.size() + 1;
        (*result)[i].message = (char *)malloc(size);
        ret = strcpy_s((*result)[i].message, size, errorResults[i].message.c_str());
        if (ret != 0) {
            LOGE("strcpy message failed message:%{public}s, errno:%{public}d", errorResults[i].uri.c_str(), ret);
            *resultNum = i;
            free((*result)[i].uri);
            free((*result)[i].message);
            return false;
        }
    }
    *resultNum = count;
    return true;
}

bool ConvertPolicyErrorResultBool(const std::vector<bool> &errorResults, bool **result)
{
    auto count = errorResults.size();
    auto memorySize = count * sizeof(bool);
    if (memorySize == 0 || memorySize > FOO_MAX_LEN) {
        LOGE("malloc size is abnormal.");
        return false;
    }
    *result = (bool *)malloc(memorySize);
    if (*result == nullptr) {
        return false;
    }
    for (uint32_t i = 0; i < count; i++) {
        (*result)[i] = errorResults[i];
    }
    return true;
}

FileManagement_ErrCode ErrorCodeConversion(int32_t errorCode)
{
    FileManagement_ErrCode errCode = E_UNKNOWN_ERROR;
    switch (errorCode) {
        case static_cast<int32_t>(E_NO_ERROR):
            errCode = E_NO_ERROR;
            break;
        case static_cast<int32_t>(E_PERMISSION):
            errCode = E_PERMISSION;
            break;
        case static_cast<int32_t>(E_PARAMS):
            errCode = E_PARAMS;
            break;
        case EPERM:
            errCode = E_EPERM;
            break;
        default:
            break;
    }
    return errCode;
}

FileManagement_ErrCode ExecAction(const FileShare_PolicyInfo *policies,
                                  unsigned int policyNum,
                                  FileShare_PolicyErrorResult **result,
                                  unsigned int *resultNum,
                                  Exec exec)
{
    (*resultNum) = 0;
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        return E_PERMISSION;
    }

    if (!CheckFileManagerFullMountEnable()) {
        return E_DEVICE_NOT_SUPPORT;
    }

    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (!ConvertPolicyInfo(policies, policyNum, uriPolicies)) {
        return E_PARAMS;
    }
    std::deque<OHOS::AppFileService::PolicyErrorResult> errorResults;
    auto ret = ErrorCodeConversion(exec(uriPolicies, errorResults));
    if (ret == E_NO_ERROR) {
        return E_NO_ERROR;
    }
    if (!ConvertPolicyErrorResult(errorResults, result, resultNum)) {
        return E_UNKNOWN_ERROR;
    }
    return ret;
}

FileManagement_ErrCode OH_FileShare_PersistPermission(const FileShare_PolicyInfo *policies,
                                                      unsigned int policyNum,
                                                      FileShare_PolicyErrorResult **result,
                                                      unsigned int *resultNum)
{
    return ExecAction(policies, policyNum, result, resultNum, OHOS::AppFileService::FilePermission::PersistPermission);
}

FileManagement_ErrCode OH_FileShare_RevokePermission(const FileShare_PolicyInfo *policies,
                                                     unsigned int policyNum,
                                                     FileShare_PolicyErrorResult **result,
                                                     unsigned int *resultNum)
{
    return ExecAction(policies, policyNum, result, resultNum, OHOS::AppFileService::FilePermission::RevokePermission);
}

FileManagement_ErrCode OH_FileShare_ActivatePermission(const FileShare_PolicyInfo *policies,
                                                       unsigned int policyNum,
                                                       FileShare_PolicyErrorResult **result,
                                                       unsigned int *resultNum)
{
    return ExecAction(policies, policyNum, result, resultNum, OHOS::AppFileService::FilePermission::ActivatePermission);
}

FileManagement_ErrCode OH_FileShare_DeactivatePermission(const FileShare_PolicyInfo *policies,
                                                         unsigned int policyNum,
                                                         FileShare_PolicyErrorResult **result,
                                                         unsigned int *resultNum)
{
    return ExecAction(policies, policyNum, result, resultNum,
                      OHOS::AppFileService::FilePermission::DeactivatePermission);
}

FileManagement_ErrCode OH_FileShare_CheckPersistentPermission(const FileShare_PolicyInfo *policies,
                                                              unsigned int policyNum,
                                                              bool **result,
                                                              unsigned int *resultNum)
{
    *resultNum = 0;
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        return E_PERMISSION;
    }

    if (!CheckFileManagerFullMountEnable()) {
        return E_DEVICE_NOT_SUPPORT;
    }
    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (!ConvertPolicyInfo(policies, policyNum, uriPolicies)) {
        return E_PARAMS;
    }
    std::vector<bool> errorResults;
    auto ret = OHOS::AppFileService::FilePermission::CheckPersistentPermission(uriPolicies, errorResults);
    if (ret != 0) {
        return ErrorCodeConversion(ret);
    }
    if (!ConvertPolicyErrorResultBool(errorResults, result)) {
        return E_UNKNOWN_ERROR;
    }
    *resultNum = errorResults.size();
    return E_NO_ERROR;
}

void OH_FileShare_ReleasePolicyErrorResult(FileShare_PolicyErrorResult *result, unsigned int num)
{
    for (unsigned i = 0; i < num; i++) {
        free(result[i].uri);
        free(result[i].message);
    }
    free(result);
}