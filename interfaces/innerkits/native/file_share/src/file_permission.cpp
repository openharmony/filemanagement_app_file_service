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

#include "log.h"
#include "uri.h"

namespace OHOS {
namespace AppFileService {
const std::string MEDIA_AUTHORITY = "media";
const std::string NETWORK_PARA = "?networkid=";
const std::string PERSISTENCE_FORBIDDEN_MESSAGE = "URI forbid to be persisted!";
const std::string INVALID_MODE_MESSAGE = "Invalid operation mode!";
const std::string INVALID_PATH_MESSAGE = "Invalid path!";
const std::string FILE_SCHEME_PREFIX = "file://";

namespace {
bool CheckValidUri(const string &uriStr)
{
    if(uriStr.find(FILE_SCHEME_PREFIX) != 0){
        LOGE("Incorrect URI format!");
        return false;
    }
    Uri uri(uriStr);
    std::string bundleName = uri.GetAuthority();
    if (bundleName == MEDIA_AUTHORITY) {
        LOGE("the URI is media URI");
        return false;
    }
    if (uriStr.find(NETWORK_PARA) != string::npos) {
        LOGE("the URI is not the current device URI");
        return false;
    }
    return true;
}
} // namespace

void FilePermission::GetErrorResults(const vector<uint32_t> &resultCodes,
                                     const vector<PathPolicyInfo> &pathPolicies,
                                     deque<struct PolicyErrorResult> &errorResults)
{
    for (int i = 0; i < resultCodes.size(); i++) {
        PolicyErrorResult result;
        Uri uri(pathPolicies[i].path);
        switch (resultCodes[i]) {
            case static_cast<PolicyErrorCode>(PolicyErrorCode::PERSISTENCE_FORBIDDEN):
                result = {uri.ToString(), PolicyErrorCode::PERSISTENCE_FORBIDDEN, PERSISTENCE_FORBIDDEN_MESSAGE};
                errorResults.emplace_back(result);
                break;
            case static_cast<PolicyErrorCode>(PolicyErrorCode::INVALID_MODE):
                result = {uri.ToString(), PolicyErrorCode::INVALID_MODE, INVALID_MODE_MESSAGE};
                errorResults.emplace_back(result);
                break;
            case static_cast<PolicyErrorCode>(PolicyErrorCode::INVALID_PATH):
                result = {uri.ToString(), PolicyErrorCode::INVALID_PATH, INVALID_PATH_MESSAGE};
                errorResults.emplace_back(result);
                break;
            default:
                break;
        }
    }
}

void FilePermission::GetPathPolicyInfoFromUriPolicyInfo(const vector<UriPolicyInfo> &uriPolicies,
                                                        deque<struct PolicyErrorResult> &errorResults,
                                                        vector<struct PathPolicyInfo> &pathPolicies)
{
    for (auto uriPolicy : uriPolicies) {
        Uri uri(uriPolicy.uri);
        string path = uri.GetPath();
        if (!CheckValidUri(uriPolicy.uri) || access(path.c_str(), F_OK) != 0) {
            LOGE("Not correct uri!");
            PolicyErrorResult result = {uriPolicy.uri, PolicyErrorCode::INVALID_PATH, INVALID_PATH_MESSAGE};
            errorResults.emplace_back(result);
        } else {
            PathPolicyInfo policyInfo = {path, uriPolicy.mode};
            pathPolicies.emplace_back(policyInfo);
        }
    }
}

int32_t FilePermission::GrantPermission(uint32_t tokenId, const vector<UriPolicyInfo> &uriPolicies, uint32_t policyFlag)
{
    vector<PathPolicyInfo> pathPolicies;
    deque<struct PolicyErrorResult> errorResults;
    GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults, pathPolicies);
    // SandboxManager interface call
    if (!errorResults.empty()) {
        LOGE("There are some URI operations that fail");
        return EPERM;
    }
    LOGD("GrantPermission success");
    return 0;
}

int32_t FilePermission::PersistPermission(const vector<UriPolicyInfo> &uriPolicies,
                                          deque<struct PolicyErrorResult> &errorResults)
{
    vector<PathPolicyInfo> pathPolicies;
    vector<uint32_t> resultCodes;
    GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults, pathPolicies);
    // SandboxManager interface call
    GetErrorResults(resultCodes, pathPolicies, errorResults);
    if (!errorResults.empty()) {
        LOGE("There are some URI operations that fail");
        return EPERM;
    }
    LOGD("PersistPermission success");
    return 0;
}

int32_t FilePermission::RevokePermission(const vector<UriPolicyInfo> &uriPolicies,
                                         deque<struct PolicyErrorResult> &errorResults)
{
    vector<PathPolicyInfo> pathPolicies;
    vector<uint32_t> resultCodes;
    GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults, pathPolicies);
    // SandboxManager interface call
    GetErrorResults(resultCodes, pathPolicies, errorResults);
    if (!errorResults.empty()) {
        LOGE("There are some URI operations that fail");
        return EPERM;
    }
    LOGD("RevokePermission success");
    return 0;
}

int32_t FilePermission::ActivatePermission(const vector<UriPolicyInfo> &uriPolicies,
                                           deque<struct PolicyErrorResult> &errorResults)
{
    vector<PathPolicyInfo> pathPolicies;
    vector<uint32_t> resultCodes;
    GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults, pathPolicies);
    // SandboxManager interface call
    GetErrorResults(resultCodes, pathPolicies, errorResults);
    if (!errorResults.empty()) {
        LOGE("There are some URI operations that fail");
        return EPERM;
    }
    LOGD("ActivatePermission success");
    return 0;
}

int32_t FilePermission::DeactivatePermission(const vector<UriPolicyInfo> &uriPolicies,
                                             deque<struct PolicyErrorResult> &errorResults)
{
    vector<PathPolicyInfo> pathPolicies;
    vector<uint32_t> resultCodes;
    GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults, pathPolicies);
    // SandboxManager interface call
    GetErrorResults(resultCodes, pathPolicies, errorResults);
    if (!errorResults.empty()) {
        LOGE("There are some URI operations that fail");
        return EPERM;
    }
    LOGD("DeactivatePermission success");
    return 0;
}
} // namespace AppFileService
} // namespace OHOS