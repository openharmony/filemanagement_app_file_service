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
#include "accesstoken_kit.h"
#include "log.h"
#include "parameter.h"
#include "uri.h"
#include <sys/stat.h>
#include <unistd.h>
#include <unordered_set>
#ifdef SANDBOX_MANAGER
#include "sandbox_manager_err_code.h"
#include "uri_permission_manager_client.h"
#endif
#include "bundle_constants.h"
#include "file_uri.h"
#include "hap_token_info.h"
#include "ipc_skeleton.h"
#include "n_error.h"
#include "sandbox_helper.h"
#include "tokenid_kit.h"

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
const std::string CURRENTUSER = "currentUser";
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

bool CheckValidUri(const string &uriStr, const string &path, bool checkAccess)
{
    if (uriStr.find(FILE_SCHEME_PREFIX) != 0) {
        LOGE("Incorrect URI format!");
        return false;
    }
    if (uriStr.find(NETWORK_PARA) != string::npos) {
        LOGE("the URI is not the current device URI");
        return false;
    }
    // Only Media path can skip access check
    Uri uri(uriStr);
    std::string bundleName = uri.GetAuthority();
    if (bundleName == MEDIA_AUTHORITY) {
        LOGI("media path, skip access check");
        return true;
    }
    // check if path can be accessed
    if (checkAccess && (access(path.c_str(), F_OK) != 0)) {
        LOGE("access path failed");
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
    if (resultCodes.size() == 0) {
        LOGE("Sandboxmanager not processed");
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
    if (resultCodes.size() == 0 || resultCodes.size() != pathPolicies.size()) {
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
    deque<struct PolicyErrorResult> &errorResults, bool checkAccess)
{
    vector<PolicyInfo> pathPolicies;
    for (auto uriPolicy : uriPolicies) {
        AppFileService::ModuleFileUri::FileUri fileuri(uriPolicy.uri);
        string path = fileuri.GetRealPath();
        if (!CheckValidUri(uriPolicy.uri, path, checkAccess)) {
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
        AppFileService::ModuleFileUri::FileUri fileuri(uriPolicy.uri);
        string path = fileuri.GetRealPath();
        if (!CheckValidUri(uriPolicy.uri, path, false)) {
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

vector<PolicyInfo> FilePermission::GetSandboxPolicyInfo(const vector<PathPolicyInfo> &pathPolicies)
{
    vector<PolicyInfo> policies;
    for (const auto &policy: pathPolicies) {
        PolicyInfo policyInfo = {policy.path, policy.mode};
        policies.emplace_back(policyInfo);
    }
    return policies;
}

static bool CheckPermission(uint32_t tokenCaller, const string &permission)
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
#endif
int32_t FilePermission::CheckUriPersistentPermission(uint32_t tokenId,
                                                     const vector<UriPolicyInfo> &uriPolicies,
                                                     vector<bool> &errorResults)
{
    int errorCode = 0;
#ifdef SANDBOX_MANAGER
    vector<PolicyInfo> pathPolicies = GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults);
    if (pathPolicies.size() == 0) {
        return EPERM;
    }

    vector<bool> resultCodes;
    LOGI("CheckUriPersistentPermission pathPolicies size: %{public}zu", pathPolicies.size());
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
#endif
    return errorCode;
}

int32_t FilePermission::CheckPathPermission(uint32_t tokenId, const vector<PathPolicyInfo> &pathPolicies,
    int32_t policyType, vector<bool> &errorResults)
{
    LOGI("CheckPathPermission pathPolicies size:%{public}zu policyType:%{public}d", pathPolicies.size(), policyType);
    int errorCode = 0;
#ifdef SANDBOX_MANAGER
    if (tokenId == 0) {
        LOGE("tokenId is invalid");
        return FileManagement::LibN::E_PARAMS;
    }
    if (pathPolicies.size() == 0 || pathPolicies.size() > MAX_ARRAY_SIZE) {
        LOGE("The number of policy is invalid");
        return FileManagement::LibN::E_PARAMS;
    }
    if (policyType < 0 || policyType > PERSISTENT_TYPE) {
        LOGE("The policyType is invalid type:%{public}d", policyType);
        return FileManagement::LibN::E_PARAMS;
    }

    int32_t sandboxManagerErrorCode = 0;
    vector<PolicyInfo> policies = GetSandboxPolicyInfo(pathPolicies);
    if (policyType == TEMPORARY_TYPE) {
        sandboxManagerErrorCode = SandboxManagerKit::CheckPolicy(tokenId, policies, errorResults);
    } else if (policyType == PERSISTENT_TYPE) {
        sandboxManagerErrorCode = SandboxManagerKit::CheckPersistPolicy(tokenId, policies, errorResults);
    } else {
        LOGE("invalid policy type %{public}d", policyType);
    }
    errorCode = ErrorCodeConversion(sandboxManagerErrorCode);
#endif
    return errorCode;
}

int32_t FilePermission::PersistPermission(const vector<UriPolicyInfo> &uriPolicies,
                                          deque<struct PolicyErrorResult> &errorResults)
{
    int errorCode = 0;
#ifdef SANDBOX_MANAGER
    if (uriPolicies.size() == 0 || uriPolicies.size() > MAX_ARRAY_SIZE) {
        LOGE("The number of result codes exceeds the maximum");
        return FileManagement::LibN::E_PARAMS;
    }
    vector<PolicyInfo> pathPolicies = GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults);
    if (pathPolicies.size() == 0) {
        return EPERM;
    }
    vector<uint32_t> resultCodes;
    LOGI("PersistPermission pathPolicies size: %{public}zu", pathPolicies.size());
    int32_t sandboxManagerErrorCode = SandboxManagerKit::PersistPolicy(pathPolicies, resultCodes);
    errorCode = ErrorCodeConversion(sandboxManagerErrorCode, errorResults, resultCodes);
    if (errorCode == EPERM) {
        ParseErrorResults(resultCodes, pathPolicies, errorResults);
    }
#endif
    return errorCode;
}

int32_t FilePermission::GrantPermission(const vector<UriPolicyInfo> &uriPolicies, const std::string &bundleName,
    int32_t appCloneIndex, deque<struct PolicyErrorResult> &errorResults)
{
    int errorCode = 0;
#ifdef SANDBOX_MANAGER
    if (uriPolicies.size() == 0 || uriPolicies.size() > MAX_ARRAY_SIZE) {
        LOGE("The number of result codes exceeds the maximum");
        return EPERM;
    }
    vector<PolicyInfo> pathPolicies = GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults, false);
    if (pathPolicies.size() == 0) {
        return EPERM;
    }
    vector<uint32_t> resultCodes;
    LOGI("GrantDecPermission size:%{public}zu bundleName:%{public}s appCloneIndex:%{public}d",
        pathPolicies.size(), bundleName.c_str(), appCloneIndex);
    
    uint64_t policyFlag = 1; // support persistent
    int32_t sandboxManagerErrorCode = SandboxManagerKit::SetPolicyByBundleName(bundleName,
        appCloneIndex, pathPolicies, policyFlag, resultCodes);
    errorCode = ErrorCodeConversion(sandboxManagerErrorCode, errorResults, resultCodes);
    if (errorCode == EPERM) {
        ParseErrorResults(resultCodes, pathPolicies, errorResults);
    }
#else
    LOGW("Sandbox manager bundle not exist, device not support.");
    return FileManagement::LibN::E_DEVICENOTSUPPORT;
#endif
    return errorCode;
}

int32_t FilePermission::RevokePermission(const vector<UriPolicyInfo> &uriPolicies,
                                         deque<struct PolicyErrorResult> &errorResults)
{
    int errorCode = 0;
#ifdef SANDBOX_MANAGER
    if (uriPolicies.size() == 0 || uriPolicies.size() > MAX_ARRAY_SIZE) {
        LOGE("The number of result codes exceeds the maximum");
        return FileManagement::LibN::E_PARAMS;
    }
    vector<PolicyInfo> pathPolicies = GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults);
    if (pathPolicies.size() == 0) {
        return EPERM;
    }
    vector<uint32_t> resultCodes;
    LOGI("RevokePermission pathPolicies size: %{public}zu", pathPolicies.size());
    int32_t sandboxManagerErrorCode = SandboxManagerKit::UnPersistPolicy(pathPolicies, resultCodes);
    errorCode = ErrorCodeConversion(sandboxManagerErrorCode, errorResults, resultCodes);
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
    if (uriPolicies.size() == 0 || uriPolicies.size() > MAX_ARRAY_SIZE) {
        LOGE("The number of result codes exceeds the maximum");
        return FileManagement::LibN::E_PARAMS;
    }
    vector<PolicyInfo> pathPolicies = GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults);
    if (pathPolicies.size() == 0) {
        return EPERM;
    }
    vector<uint32_t> resultCodes;
    LOGI("ActivatePermission pathPolicies size: %{public}zu", pathPolicies.size());
    auto &uriPermissionClient = AAFwk::UriPermissionManagerClient::GetInstance();
    int32_t sandboxManagerErrorCode = uriPermissionClient.Active(pathPolicies, resultCodes);
    errorCode = ErrorCodeConversion(sandboxManagerErrorCode, errorResults, resultCodes);
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
    if (uriPolicies.size() == 0 || uriPolicies.size() > MAX_ARRAY_SIZE) {
        LOGE("The number of result codes exceeds the maximum");
        return FileManagement::LibN::E_PARAMS;
    }
    vector<PolicyInfo> pathPolicies = GetPathPolicyInfoFromUriPolicyInfo(uriPolicies, errorResults);
    if (pathPolicies.size() == 0) {
        return EPERM;
    }
    vector<uint32_t> resultCodes;
    LOGI("DeactivatePermission pathPolicies size: %{public}zu", pathPolicies.size());
    int32_t sandboxManagerErrorCode = SandboxManagerKit::StopAccessingPolicy(pathPolicies, resultCodes);
    errorCode = ErrorCodeConversion(sandboxManagerErrorCode, errorResults, resultCodes);
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
    if (uriPolicies.size() == 0 || uriPolicies.size() > MAX_ARRAY_SIZE) {
        LOGE("The number of result codes exceeds the maximum");
        return FileManagement::LibN::E_PARAMS;
    }
    auto tokenId = IPCSkeleton::GetCallingTokenID();
    errorCode = CheckUriPersistentPermission(tokenId, uriPolicies, errorResults);
#endif
    return errorCode;
}

string FilePermission::GetPathByPermission(const std::string &userName, const std::string &permission)
{
    if (permissionPathMap.find(permission) != permissionPathMap.end()) {
        string path = permissionPathMap.at(permission);
        if (!userName.empty()) {
            path = path.replace(path.find(CURRENTUSER), CURRENTUSER.length(), userName);
        }
        return path;
    }
    return "";
}
} // namespace AppFileService
} // namespace OHOS