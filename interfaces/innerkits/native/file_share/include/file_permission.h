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

#ifndef APP_FILE_SERVICE_FILE_PERMISSION
#define APP_FILE_SERVICE_FILE_PERMISSION

#include <deque>
#include <string>
#include <vector>
#ifdef SANDBOX_MANAGER
#include "sandbox_manager_kit.h"
#endif

namespace OHOS {
namespace AppFileService {
using namespace std;
#ifdef SANDBOX_MANAGER
using namespace AccessControl::SandboxManager;
#endif
constexpr const int32_t MAX_ARRAY_SIZE = 500;
typedef enum OperationMode {
    READ_MODE = 1 << 0,
    WRITE_MODE = 1 << 1,
    CREATE_MODE = 1 << 2,
    DELETE_MODE = 1 << 3,
    RENAME_MODE = 1 << 4,
} OperationMode;

typedef enum PolicyFlag {
    ALLOW_PERSISTENCE = 1 << 0,
    FORBID_PERSISTENCE = 1 << 1,
} PolicyFlag;

enum PolicyErrorCode {
    PERSISTENCE_FORBIDDEN = 1,
    INVALID_MODE = 2,
    INVALID_PATH = 3,
    PERMISSION_NOT_PERSISTED = 4,
};

enum PolicyType {
    TEMPORARY_TYPE = 0,
    PERSISTENT_TYPE = 1,
};

struct UriPolicyInfo {
    string uri = "";
    uint32_t mode = OperationMode::READ_MODE;
};

struct PathPolicyInfo {
    string path = "";
    uint32_t mode = OperationMode::READ_MODE;
};

struct PolicyErrorResult {
    string uri = "";
    PolicyErrorCode code = PolicyErrorCode::PERSISTENCE_FORBIDDEN;
    string message = "";
};

class FilePermission {
public:
    static int32_t PersistPermission(const vector<UriPolicyInfo> &uriPolicies,
                                     deque<struct PolicyErrorResult> &errorResults);
    static int32_t RevokePermission(const vector<UriPolicyInfo> &uriPolicies,
                                    deque<struct PolicyErrorResult> &errorResults);
    static int32_t ActivatePermission(const vector<UriPolicyInfo> &uriPolicies,
                                      deque<struct PolicyErrorResult> &errorResults);
    static int32_t DeactivatePermission(const vector<UriPolicyInfo> &uriPolicies,
                                        deque<struct PolicyErrorResult> &errorResults);
    static int32_t CheckPersistentPermission(const vector<UriPolicyInfo> &uriPolicies, vector<bool> &errorResults);
    static string GetPathByPermission(const std::string &userName, const std::string &permission);
    static int32_t CheckUriPersistentPermission(uint32_t tokenId,
                                                const vector<UriPolicyInfo> &uriPolicies,
                                                vector<bool> &errorResults);
    static int32_t CheckPathPermission(uint32_t tokenId,
                                                const vector<PathPolicyInfo> &uriPolicies,
                                                int32_t policyType,
                                                vector<bool> &errorResults);
    static int32_t GrantPermission(const vector<UriPolicyInfo> &uriPolicies, const std::string &bundleName,
        int32_t appCloneIndex, deque<struct PolicyErrorResult> &errorResults);
#ifdef SANDBOX_MANAGER
private:
    static void ParseErrorResults(const vector<uint32_t> &resultCodes,
                                  const vector<PolicyInfo> &pathPolicies,
                                  deque<struct PolicyErrorResult> &errorResults);
    static void ParseErrorResults(const vector<bool> &resultCodes, vector<bool> &errorResults);
    static vector<PolicyInfo> GetPathPolicyInfoFromUriPolicyInfo(const vector<UriPolicyInfo> &uriPolicies,
        deque<struct PolicyErrorResult> &errorResults, bool checkAccess = true);
    static vector<PolicyInfo> GetPathPolicyInfoFromUriPolicyInfo(const vector<UriPolicyInfo> &uriPolicies,
                                                                 vector<bool> &errorResults);
    static vector<PolicyInfo> GetSandboxPolicyInfo(const vector<PathPolicyInfo> &pathPolicies);
#endif
};
} // namespace AppFileService
} // namespace OHOS

#endif