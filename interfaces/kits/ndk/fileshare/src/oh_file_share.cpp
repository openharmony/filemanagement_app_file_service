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

#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "file_permission.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "parameter.h"
#include "securec.h"
#include "tokenid_kit.h"

const int32_t FOO_MAX_LEN = sizeof(FileShare_PolicyErrorResult) * OHOS::AppFileService::MAX_ARRAY_SIZE;

using Exec = std::function<int(const std::vector<OHOS::AppFileService::UriPolicyInfo> &uriPolicies,
                               std::deque<struct OHOS::AppFileService::PolicyErrorResult> &errorResults)>;

static bool ConvertPolicyInfo(const FileShare_PolicyInfo *policies,
                              int policyNum,
                              std::vector<OHOS::AppFileService::UriPolicyInfo> &uriPolicies)
{
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

static bool ConvertPolicyErrorResult(const std::deque<OHOS::AppFileService::PolicyErrorResult> &errorResults,
                                     FileShare_PolicyErrorResult **result,
                                     unsigned int &resultNum)
{
    resultNum = 0;
    auto count = errorResults.size();
    auto memorySize = count * sizeof(FileShare_PolicyErrorResult);
    if (memorySize == 0 || memorySize > FOO_MAX_LEN) {
        LOGE("The size of the return value array is abnormal");
        return false;
    }
    *result = static_cast<FileShare_PolicyErrorResult *>(malloc(memorySize));
    if (*result == nullptr) {
        LOGE("Failed to apply for FileShare_PolicyErrorResult array memory");
        return false;
    }
    for (uint32_t i = 0; i < count; i++) {
        size_t size = errorResults[i].uri.size() + 1;
        (*result)[i].uri = static_cast<char *>(malloc(size));
        if ((*result)[i].uri == nullptr) {
            LOGE("Failed to apply for URI memory");
            return false;
        }
        auto ret = strcpy_s((*result)[i].uri, size, errorResults[i].uri.c_str());
        if (ret != 0) {
            LOGE("Copy uri failed uri:%{private}s, errno:%{public}d", errorResults[i].uri.c_str(), ret);
            free((*result)[i].uri);
            return false;
        }
        (*result)[i].code = static_cast<FileShare_PolicyErrorCode>(errorResults[i].code);
        size = errorResults[i].message.size() + 1;
        (*result)[i].message = static_cast<char *>(malloc(size));
        if ((*result)[i].message == nullptr) {
            LOGE("Failed to apply for message memory");
            free((*result)[i].uri);
            return false;
        }
        ret = strcpy_s((*result)[i].message, size, errorResults[i].message.c_str());
        if (ret != 0) {
            LOGE("Copy message failed message:%{private}s, errno:%{public}d", errorResults[i].uri.c_str(), ret);
            free((*result)[i].uri);
            free((*result)[i].message);
            return false;
        }
        resultNum++;
    }
    return true;
}

static bool ConvertPolicyErrorResultBool(const std::vector<bool> &errorResults, bool **result)
{
    auto count = errorResults.size();
    auto memorySize = count * sizeof(bool);
    if (memorySize == 0 || memorySize > FOO_MAX_LEN) {
        LOGE("The size of the return value array is abnormal");
        return false;
    }
    *result = (bool *)malloc(memorySize);
    if (*result == nullptr) {
        LOGE("Failed to apply for bool array memory");
        return false;
    }
    for (uint32_t i = 0; i < count; i++) {
        (*result)[i] = errorResults[i];
    }
    return true;
}

static FileManagement_ErrCode ErrorCodeConversion(int32_t errorCode)
{
    FileManagement_ErrCode errCode = ERR_UNKNOWN;
    switch (errorCode) {
        case static_cast<int32_t>(ERR_OK):
            errCode = ERR_OK;
            break;
        case static_cast<int32_t>(ERR_PERMISSION_ERROR):
            errCode = ERR_PERMISSION_ERROR;
            break;
        case static_cast<int32_t>(ERR_PARAMS):
            errCode = ERR_PARAMS;
            break;
        case EPERM:
            errCode = ERR_EPERM;
            break;
        default:
            break;
    }
    return errCode;
}

void OH_FileShare_ReleasePolicyErrorResult(FileShare_PolicyErrorResult *result, unsigned int num);
static FileManagement_ErrCode ExecAction(const FileShare_PolicyInfo *policies,
                                         unsigned int policyNum,
                                         FileShare_PolicyErrorResult **result,
                                         unsigned int *resultNum,
                                         Exec exec)
{
    (*resultNum) = 0;
    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (!ConvertPolicyInfo(policies, policyNum, uriPolicies)) {
        return ERR_PARAMS;
    }
    std::deque<OHOS::AppFileService::PolicyErrorResult> errorResults;
    auto ret = ErrorCodeConversion(exec(uriPolicies, errorResults));
    if (ret == ERR_OK) {
        return ERR_OK;
    }
    if (!ConvertPolicyErrorResult(errorResults, result, *resultNum)) {
        OH_FileShare_ReleasePolicyErrorResult(*result, *resultNum);
        return ERR_ENOMEM;
    }
    return ret;
}

FileManagement_ErrCode OH_FileShare_PersistPermission(const FileShare_PolicyInfo *policies,
                                                      unsigned int policyNum,
                                                      FileShare_PolicyErrorResult **result,
                                                      unsigned int *resultNum)
{
    if (policies == nullptr || result == nullptr || resultNum == nullptr) {
        LOGE("The external input pointer is abnormal");
        return ERR_PARAMS;
    }
    if (policyNum == 0 || policyNum > OHOS::AppFileService::MAX_ARRAY_SIZE) {
        LOGE("The policyNum is abnormal");
        return ERR_PARAMS;
    }
    return ExecAction(policies, policyNum, result, resultNum, OHOS::AppFileService::FilePermission::PersistPermission);
}

FileManagement_ErrCode OH_FileShare_RevokePermission(const FileShare_PolicyInfo *policies,
                                                     unsigned int policyNum,
                                                     FileShare_PolicyErrorResult **result,
                                                     unsigned int *resultNum)
{
    if (policies == nullptr || result == nullptr || resultNum == nullptr) {
        LOGE("The external input pointer is abnormal");
        return ERR_PARAMS;
    }
    if (policyNum == 0 || policyNum > OHOS::AppFileService::MAX_ARRAY_SIZE) {
        LOGE("The policyNum is abnormal");
        return ERR_PARAMS;
    }
    return ExecAction(policies, policyNum, result, resultNum, OHOS::AppFileService::FilePermission::RevokePermission);
}

FileManagement_ErrCode OH_FileShare_ActivatePermission(const FileShare_PolicyInfo *policies,
                                                       unsigned int policyNum,
                                                       FileShare_PolicyErrorResult **result,
                                                       unsigned int *resultNum)
{
    if (policies == nullptr || result == nullptr || resultNum == nullptr) {
        LOGE("The external input pointer is abnormal");
        return ERR_PARAMS;
    }
    if (policyNum == 0 || policyNum > OHOS::AppFileService::MAX_ARRAY_SIZE) {
        LOGE("The policyNum is abnormal");
        return ERR_PARAMS;
    }
    return ExecAction(policies, policyNum, result, resultNum, OHOS::AppFileService::FilePermission::ActivatePermission);
}

FileManagement_ErrCode OH_FileShare_DeactivatePermission(const FileShare_PolicyInfo *policies,
                                                         unsigned int policyNum,
                                                         FileShare_PolicyErrorResult **result,
                                                         unsigned int *resultNum)
{
    if (policies == nullptr || result == nullptr || resultNum == nullptr) {
        LOGE("The external input pointer is abnormal");
        return ERR_PARAMS;
    }
    if (policyNum == 0 || policyNum > OHOS::AppFileService::MAX_ARRAY_SIZE) {
        LOGE("The policyNum is abnormal");
        return ERR_PARAMS;
    }
    return ExecAction(policies, policyNum, result, resultNum,
                      OHOS::AppFileService::FilePermission::DeactivatePermission);
}

FileManagement_ErrCode OH_FileShare_CheckPersistentPermission(const FileShare_PolicyInfo *policies,
                                                              unsigned int policyNum,
                                                              bool **result,
                                                              unsigned int *resultNum)
{
    if (policies == nullptr || result == nullptr || resultNum == nullptr) {
        LOGE("The external input pointer is abnormal");
        return ERR_PARAMS;
    }
    if (policyNum == 0 || policyNum > OHOS::AppFileService::MAX_ARRAY_SIZE) {
        LOGE("The policyNum is abnormal");
        return ERR_PARAMS;
    }
    *resultNum = 0;
    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (!ConvertPolicyInfo(policies, policyNum, uriPolicies)) {
        return ERR_PARAMS;
    }
    std::vector<bool> errorResults;
    auto ret = OHOS::AppFileService::FilePermission::CheckPersistentPermission(uriPolicies, errorResults);
    if (ret != 0) {
        return ErrorCodeConversion(ret);
    }
    if (!ConvertPolicyErrorResultBool(errorResults, result)) {
        return ERR_ENOMEM;
    }
    *resultNum = errorResults.size();
    return ERR_OK;
}

void OH_FileShare_ReleasePolicyErrorResult(FileShare_PolicyErrorResult *result, unsigned int num)
{
    if (result == nullptr) {
        return;
    }
    for (unsigned i = 0; i < num; i++) {
        if (result[i].uri != nullptr) {
            free(result[i].uri);
        }
        if (result[i].message != nullptr) {
            free(result[i].message);
        }
    }
    free(result);
}