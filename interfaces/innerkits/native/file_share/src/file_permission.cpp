/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "file_permission.h"
#include <unistd.h>

#include "filemgmt_libn.h"
#include "log.h"
#include "parameter.h"
#include "uri.h"

namespace OHOS {
namespace AppFileService {
const std::string MEDIA_AUTHORITY = "media";
const std::string REMOTE_SHARE_PATH_DIR = "/.remote_share";
const std::string FORBIDDEN_TO_BE_PERSISTED_MESSAGE = "URI refusal to be persisted!";
const std::string INVALID_MODE_MESSAGE = "Invalid operation mode!";
const std::string INVALID_PATH_MESSAGE = "Invalid path!";
const int32_t FULL_MOUNT_ENABLE_SIZE = 6;
const char *FULL_MOUNT_ENABLE_PARAMETER = "const.filemanager.full_mout.enable";

namespace {
bool CheckValidUri(const string &uriStr)
{
    Uri uri(uriStr);
    std::string bundleName = uri.GetAuthority();
    if (bundleName == MEDIA_AUTHORITY) {
        LOGE("the URI is media URI");
        return false;
    }
    if (uriStr.find(REMOTE_SHARE_PATH_DIR) != string::npos) {
        LOGE("the URI is distributed URI");
        return false;
    }
    return true;
}

bool CheckFileManagerFullMountEnable()
{
    char value[FULL_MOUNT_ENABLE_SIZE] = "false";
    int retSystem = GetParameter(FULL_MOUNT_ENABLE_PARAMETER, "false", value, FULL_MOUNT_ENABLE_SIZE);
    if (retSystem > 0 && !strcmp(value, "true")) {
        LOGE("The full mount enable parameter is true");
        return true;
    }
    LOGD("The full mount enable parameter is false");
    return false;
}
} // namespace

void FilePermission::GetErrorResults(const vector<uint32_t> &resultCodes,
                                     const vector<string> &resultUris,
                                     deque<struct PolicyErrorResult> &errorResults)
{
    for (int i = 0; i < resultCodes.size(); i++) {
        PolicyErrorResult result;
        switch (resultCodes[i]) {
            case static_cast<PolicyErrorCode>(PolicyErrorCode::FORBIDDEN_TO_BE_PERSISTED):
                result = {resultUris[i], PolicyErrorCode::FORBIDDEN_TO_BE_PERSISTED, FORBIDDEN_TO_BE_PERSISTED_MESSAGE};
                errorResults.emplace_back(result);
                break;
            case static_cast<PolicyErrorCode>(PolicyErrorCode::INVALID_MODE):
                result = {resultUris[i], PolicyErrorCode::INVALID_MODE, INVALID_MODE_MESSAGE};
                errorResults.emplace_back(result);
                break;
            case static_cast<PolicyErrorCode>(PolicyErrorCode::INVALID_PATH):
                result = {resultUris[i], PolicyErrorCode::INVALID_PATH, INVALID_PATH_MESSAGE};
                errorResults.emplace_back(result);
                break;
            default:
                break;
        }
    }
}

void FilePermission::GetPolicyInformation(const vector<UriPolicyInfo> &uriPolicies,
                                          vector<string> &resultUris,
                                          deque<struct PolicyErrorResult> &errorResults,
                                          vector<struct PolicyInfo> &policies)
{
    for (auto uriPolicy : uriPolicies) {
        Uri uri(uriPolicy.uri);
        string path = uri.GetPath();
        if (!CheckValidUri(uriPolicy.uri) || access(path.c_str(), F_OK) != 0) {
            LOGE("Not the correct uri!");
            PolicyErrorResult result = {uriPolicy.uri, PolicyErrorCode::INVALID_PATH, INVALID_PATH_MESSAGE};
            errorResults.emplace_back(result);
        } else {
            PolicyInfo policyInfo = {path, uriPolicy.mode};
            policies.emplace_back(policyInfo);
            resultUris.emplace_back(uriPolicy.uri);
        }
    }
}

int32_t FilePermission::GrantPermission(uint32_t tokenId, vector<UriPolicyInfo> uriPolicies, uint32_t policyFlag)
{
    if (!CheckFileManagerFullMountEnable()) {
        LOGE("The device doesn't support this apil");
        return E_DEVICENOTSUPPORT;
    }
    vector<PolicyInfo> policies;
    vector<string> resultUris;
    deque<struct PolicyErrorResult> errorResults;
    GetPolicyInformation(uriPolicies, resultUris, errorResults, policies);
    // SandboxManager interface call
    // int32_t SandboxRet = SandboxManager::GrantPermission(tokenId, resultCodes);
    if (!errorResults.empty()) {
        LOGE("There are some URI operations that fail");
        return EPERM;
    }
    LOGD("GrantPermission success");
    return 0;
}

int32_t FilePermission::PersistPermission(vector<UriPolicyInfo> uriPolicies,
                                          deque<struct PolicyErrorResult> &errorResults)
{
    if (!CheckFileManagerFullMountEnable()) {
        LOGE("The device doesn't support this apil");
        return E_DEVICENOTSUPPORT;
    }
    vector<PolicyInfo> policies;
    vector<string> resultUris;
    vector<uint32_t> resultCodes;
    GetPolicyInformation(uriPolicies, resultUris, errorResults, policies);
    // SandboxManager interface call
    // int32_t SandboxRet = SandboxManager::PersistPermission(policies, resultCodes);
    GetErrorResults(resultCodes, resultUris, errorResults);
    if (!errorResults.empty()) {
        LOGE("There are some URI operations that fail");
        return EPERM;
    }
    LOGD("PersistPermission success");
    return 0;
}

int32_t FilePermission::RevokePermission(vector<UriPolicyInfo> uriPolicies,
                                                  deque<struct PolicyErrorResult> &errorResults)
{
    if (!CheckFileManagerFullMountEnable()) {
        LOGE("The device doesn't support this apil");
        return E_DEVICENOTSUPPORT;
    }
    vector<PolicyInfo> policies;
    vector<string> resultUris;
    vector<uint32_t> resultCodes;
    GetPolicyInformation(uriPolicies, resultUris, errorResults, policies);
    // SandboxManager interface call
    // int32_t SandboxRet = SandboxManager::RevokePermission(policies, resultCodes);
    GetErrorResults(resultCodes, resultUris, errorResults);
    if (!errorResults.empty()) {
        LOGE("There are some URI operations that fail");
        return EPERM;
    }
    LOGD("RevokePermission success");
    return 0;
}

int32_t FilePermission::ActivatePermission(vector<UriPolicyInfo> uriPolicies,
                                             deque<struct PolicyErrorResult> &errorResults)
{
    if (!CheckFileManagerFullMountEnable()) {
        LOGE("The device doesn't support this apil");
        return E_DEVICENOTSUPPORT;
    }
    vector<PolicyInfo> policies;
    vector<string> resultUris;
    vector<uint32_t> resultCodes;
    GetPolicyInformation(uriPolicies, resultUris, errorResults, policies);
    // SandboxManager interface call
    // int32_t SandboxRet = SandboxManager::ActivatePermission(policies, resultCodes);
    GetErrorResults(resultCodes, resultUris, errorResults);
    if (!errorResults.empty()) {
        LOGE("There are some URI operations that fail");
        return EPERM;
    }
    LOGD("ActivatePermission success");
    return 0;
}

int32_t FilePermission::DeactivatePermission(vector<UriPolicyInfo> uriPolicies,
                                               deque<struct PolicyErrorResult> &errorResults)
{
    if (!CheckFileManagerFullMountEnable()) {
        LOGE("The device doesn't support this apil");
        return E_DEVICENOTSUPPORT;
    }
    vector<PolicyInfo> policies;
    vector<string> resultUris;
    vector<uint32_t> resultCodes;
    GetPolicyInformation(uriPolicies, resultUris, errorResults, policies);
    // SandboxManager interface call
    // int32_t SandboxRet = SandboxManager::DeactivatePermission(policies, resultCodes);
    GetErrorResults(resultCodes, resultUris, errorResults);
    if (!errorResults.empty()) {
        LOGE("There are some URI operations that fail");
        return EPERM;
    }
    LOGD("DeactivatePermission success");
    return 0;
}

} // namespace AppFileService
} // namespace OHOS