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
#include "log.h"
#include "parameter.h"
#include "uri.h"
#include <unistd.h>
#include <unordered_set>
#ifdef SANDBOX_MANAGER
#include "accesstoken_kit.h"
#include "bundle_constants.h"
#include "hap_token_info.h"
#include "ipc_skeleton.h"
#include "n_error.h"
#include "sandbox_helper.h"
#include "sandbox_manager_err_code.h"
#include "tokenid_kit.h"
#endif

namespace OHOS {
namespace AppFileService {
const std::string MEDIA_AUTHORITY = "media";
const std::string NETWORK_PARA = "?networkid=";
const std::string PERSISTENCE_FORBIDDEN_MESSAGE = "URI forbid to be persisted!";
const std::string INVALID_MODE_MESSAGE = "Invalid operation mode!";
const std::string INVALID_PATH_MESSAGE = "Invalid path!";
const std::string PERMISSION_NOT_PERSISTED_MESSAGE = "The policy is no persistent capability!";
const std::string FILE_SCHEME_PREFIX = "file://";
const std::string FILE_MANAGER_AUTHORITY = "docs";
const std::string SANDBOX_STORAGE_PATH = "/storage/Users/currentUser/";
const std::string DOWNLOAD_PATH = "/storage/Users/currentUser/Download";
const std::string DESKTOP_PATH = "/storage/Users/currentUser/Desktop";
const std::string DOCUMENTS_PATH = "/storage/Users/currentUser/Documents";
const std::string READ_WRITE_DOWNLOAD_PERMISSION = "ohos.permission.READ_WRITE_DOWNLOAD_DIRECTORY";
const std::string READ_WRITE_DESKTOP_PERMISSION = "ohos.permission.READ_WRITE_DESKTOP_DIRECTORY";
const std::string READ_WRITE_DOCUMENTS_PERMISSION = "ohos.permission.READ_WRITE_DOCUMENTS_DIRECTORY";
const std::string FILE_ACCESS_MANAGER_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
const std::unordered_map<std::string, std::string> permissionPathMap = {
    {READ_WRITE_DOWNLOAD_PERMISSION, DOWNLOAD_PATH},
    {READ_WRITE_DESKTOP_PERMISSION, DESKTOP_PATH},
    {READ_WRITE_DOCUMENTS_PERMISSION, DOCUMENTS_PATH}};
#ifdef SANDBOX_MANAGER
namespace {
bool CheckValidUri(const string &uriStr)
{
    if (uriStr.find(FILE_SCHEME_PREFIX) != 0) {
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
int32_t ErrorCodeConversion(int32_t sandboxManagerErrorCode,
                            const deque<struct PolicyErrorResult> &errorResults,
                            const vector<uint32_t> &resultCodes)
{
    if (sandboxManagerErrorCode == PERMISSION_DENIED) {
        LOGE("The app does not have the authorization URI permission");
        return FileManagement::LibN::E_PERMISSION;
    }
    if (sandboxManagerErrorCode == INVALID_PARAMTER) {
        if (resultCodes.size() != 0) {
            LOGE("The number of incoming URIs is too many");
            return FileManagement::LibN::E_PARAMS;
        } else {
            LOGE("The incoming URI is invalid");
            return EPERM;
        }
    }
    if (!errorResults.empty()) {
        LOGE("Some of the incoming URIs failed");
        return EPERM;
    }
    for (size_t i = 0; i < resultCodes.size(); i++) {
        if (resultCodes[i] != 0) {
            LOGE("Reason for URI authorization failure");
            return EPERM;
        }
    }
    if (sandboxManagerErrorCode == SANDBOX_MANAGER_OK) {
        return 0;
    }
    return FileManagement::LibN::E_UNKNOWN_ERROR;
}

int32_t ErrorCodeConversion(int32_t sandboxManagerErrorCode)
{
    if (sandboxManagerErrorCode == SANDBOX_MANAGER_OK) {
        return 0;
    }
    if (sandboxManagerErrorCode == PERMISSION_DENIED) {
        LOGE("The app does not have the authorization URI permission");
        return FileManagement::LibN::E_PERMISSION;
    }
    return FileManagement::LibN::E_UNKNOWN_ERROR;
}
} // namespace
void FilePermission::ParseErrorResults(const vector<uint32_t> &resultCodes,
                                       const vector<PolicyInfo> &pathPolicies,
                                       deque<struct PolicyErrorResult> &errorResults)
{
    if (resultCodes.size() != pathPolicies.size()) {
        LOGE("resultCodes size is not equals pathPolicies size");
        return;
    }
    for (size_t i = 0; i < resultCodes.size(); i++) {
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
            case static_cast<PolicyErrorCode>(PolicyErrorCode::PERMISSION_NOT_PERSISTED):
                result = {uri.ToString(), PolicyErrorCode::PERMISSION_NOT_PERSISTED, PERMISSION_NOT_PERSISTED_MESSAGE};
                errorResults.emplace_back(result);
                break;
            default:
                break;
        }
    }
}

void FilePermission::ParseErrorResults(const vector<bool> &resultCodes, vector<bool> &errorResults)
{
    auto resultCodeSize = resultCodes.size();
    if (resultCodeSize == 0) {
        return;
    }
    auto errorResultSize = errorResults.size();
    for (size_t i = 0, j = 0; i < errorResultSize && j < resultCodeSize; i++) {
        if (errorResults[i]) {
            errorResults[i] = resultCodes[j++];
        }
    }
}

vector<PolicyInfo> FilePermission::GetPathPolicyInfoFromUriPolicyInfo(const vector<UriPolicyInfo> &uriPolicies,
                                                                      deque<struct PolicyErrorResult> &errorResults)
{
    vector<PolicyInfo> pathPolicies;
    for (auto uriPolicy : uriPolicies) {
        Uri uri(uriPolicy.uri);
        string path = SandboxHelper::Decode(uri.GetPath());
        if (!CheckValidUri(uriPolicy.uri) || access(path.c_str(), F_OK) != 0) {
            LOGE("Not correct uri!");
            PolicyErrorResult result = {uriPolicy.uri, PolicyErrorCode::INVALID_PATH, INVALID_PATH_MESSAGE};
            errorResults.emplace_back(result);
        } else {
            PolicyInfo policyInfo = {path, uriPolicy.mode};
            pathPolicies.emplace_back(policyInfo);
        }
    }
    return pathPolicies;
}

vector<PolicyInfo> FilePermission::GetPathPolicyInfoFromUriPolicyInfo(const vector<UriPolicyInfo> &uriPolicies,
                                                                      vector<bool> &errorResults)
{
    vector<PolicyInfo> pathPolicies;
    for (const auto &uriPolicy : uriPolicies) {
        Uri uri(uriPolicy.uri);
        string path = SandboxHelper::Decode(uri.GetPath());
        if (!CheckValidUri(uriPolicy.uri) || access(path.c_str(), F_OK) != 0) {
            LOGE("Not correct uri!");
            errorResults.emplace_back(false);
        } else {
            PolicyInfo policyInfo = {path, uriPolicy.mode};
            pathPolicies.emplace_back(policyInfo);
            errorResults.emplace_back(true);
        }
    }
    return pathPolicies;
}

static bool CheckPermission(uint64_t tokenCaller, const string &permission)
{
    return Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission) ==
           Security::AccessToken::PermissionState::PERMISSION_GRANTED;
}

static bool IsFileManagerUri(const string &pathStr)
{
    return pathStr.find(DOWNLOAD_PATH) == 0 || pathStr.find(DESKTOP_PATH) == 0 || pathStr.find(DOCUMENTS_PATH) == 0;
}

static bool CheckFileManagerUriPermission(uint32_t providerTokenId, const string &pathStr)
{
    return (IsFileManagerUri(pathStr) && CheckPermission(providerTokenId, FILE_ACCESS_MANAGER_PERMISSION)) ||
           (pathStr.find(DOWNLOAD_PATH) == 0 && CheckPermission(providerTokenId, READ_WRITE_DOWNLOAD_PERMISSION)) ||
           (pathStr.find(DESKTOP_PATH) == 0 && CheckPermission(providerTokenId, READ_WRITE_DESKTOP_PERMISSION)) ||
           (pathStr.find(DOCUMENTS_PATH) == 0 && CheckPermission(providerTokenId, READ_WRITE_DOCUMENTS_PERMISSION));
}

int32_t FilePermission::CheckUriPersistentPermission(uint32_t tokenId,
                                                     const vector<UriPolicyInfo> &uriPolicies,
                                                     vector<bool> &errorResults)
{
    int errorCode = 0;
    vector<PolicyInfo> pathPolicies = GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults);
    if (pathPolicies.size() == 0) {
        return EPERM;
    }

    vector<bool> resultCodes;
    int32_t sandboxManagerErrorCode = SandboxManagerKit::CheckPersistPolicy(tokenId, pathPolicies, resultCodes);
    for (size_t i = resultCodes.size(); i < pathPolicies.size(); i++) {
        resultCodes.emplace_back(false);
    }
    for (size_t i = 0; i < pathPolicies.size(); i++) {
        if (!resultCodes[i]) {
            resultCodes[i] = CheckFileManagerUriPermission(tokenId, pathPolicies[i].path);
        }
    }

    errorCode = ErrorCodeConversion(sandboxManagerErrorCode);
    ParseErrorResults(resultCodes, errorResults);
    return errorCode;
}
#endif

int32_t FilePermission::PersistPermission(const vector<UriPolicyInfo> &uriPolicies,
                                          deque<struct PolicyErrorResult> &errorResults)
{
    int errorCode = 0;
#ifdef SANDBOX_MANAGER
    vector<PolicyInfo> pathPolicies = GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults);
    vector<uint32_t> resultCodes;
    int32_t sandboxManagerErrorCode = SandboxManagerKit::PersistPolicy(pathPolicies, resultCodes);
    errorCode = ErrorCodeConversion(sandboxManagerErrorCode, errorResults, resultCodes);
    if (resultCodes.size() > MAX_ARRAY_SIZE) {
        LOGE("The number of result codes exceeds the maximum");
        return FileManagement::LibN::E_PARAMS;
    }
    if (errorCode == EPERM) {
        ParseErrorResults(resultCodes, pathPolicies, errorResults);
    }
#endif
    return errorCode;
}

int32_t FilePermission::RevokePermission(const vector<UriPolicyInfo> &uriPolicies,
                                         deque<struct PolicyErrorResult> &errorResults)
{
    int errorCode = 0;
#ifdef SANDBOX_MANAGER
    vector<PolicyInfo> pathPolicies = GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults);
    vector<uint32_t> resultCodes;
    int32_t sandboxManagerErrorCode = SandboxManagerKit::UnPersistPolicy(pathPolicies, resultCodes);
    errorCode = ErrorCodeConversion(sandboxManagerErrorCode, errorResults, resultCodes);
    if (resultCodes.size() > MAX_ARRAY_SIZE) {
        LOGE("The number of result codes exceeds the maximum");
        return FileManagement::LibN::E_PARAMS;
    }
    if (errorCode == EPERM) {
        ParseErrorResults(resultCodes, pathPolicies, errorResults);
    }
#endif
    return errorCode;
}

int32_t FilePermission::ActivatePermission(const vector<UriPolicyInfo> &uriPolicies,
                                           deque<struct PolicyErrorResult> &errorResults)
{
    int errorCode = 0;
#ifdef SANDBOX_MANAGER
    vector<PolicyInfo> pathPolicies = GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults);
    vector<uint32_t> resultCodes;
    int32_t sandboxManagerErrorCode = SandboxManagerKit::StartAccessingPolicy(pathPolicies, resultCodes);
    errorCode = ErrorCodeConversion(sandboxManagerErrorCode, errorResults, resultCodes);
    if (resultCodes.size() > MAX_ARRAY_SIZE) {
        LOGE("The number of result codes exceeds the maximum");
        return FileManagement::LibN::E_PARAMS;
    }
    if (errorCode == EPERM) {
        ParseErrorResults(resultCodes, pathPolicies, errorResults);
    }
#endif
    return errorCode;
}

int32_t FilePermission::DeactivatePermission(const vector<UriPolicyInfo> &uriPolicies,
                                             deque<struct PolicyErrorResult> &errorResults)
{
    int errorCode = 0;
#ifdef SANDBOX_MANAGER
    vector<PolicyInfo> pathPolicies = GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults);
    vector<uint32_t> resultCodes;
    int32_t sandboxManagerErrorCode = SandboxManagerKit::StopAccessingPolicy(pathPolicies, resultCodes);
    errorCode = ErrorCodeConversion(sandboxManagerErrorCode, errorResults, resultCodes);
    if (resultCodes.size() > MAX_ARRAY_SIZE) {
        LOGE("The number of result codes exceeds the maximum");
        return FileManagement::LibN::E_PARAMS;
    }
    if (errorCode == EPERM) {
        ParseErrorResults(resultCodes, pathPolicies, errorResults);
    }
#endif
    return errorCode;
}

int32_t FilePermission::CheckPersistentPermission(const vector<UriPolicyInfo> &uriPolicies, vector<bool> &errorResults)
{
    int errorCode = 0;
#ifdef SANDBOX_MANAGER
    auto tokenId = IPCSkeleton::GetCallingTokenID();
    errorCode = CheckUriPersistentPermission(tokenId, uriPolicies, errorResults);
#endif
    return errorCode;
}

string FilePermission::GetPathByPermission(const std::string &permission)
{
#ifdef SANDBOX_MANAGER
    if (permissionPathMap.find(permission) != permissionPathMap.end()) {
        return permissionPathMap.at(permission);
    }
#endif
    return "";
}
} // namespace AppFileService
} // namespace OHOS