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

namespace OHOS {
namespace AppFileService {
using namespace std;
typedef enum OperateMode {
    READ_MODE = 1 << 0,
    WRITE_MODE = 1 << 1,
} OperateMode;
typedef enum PolicyFlag {
    ALLOWED_PERSIST = 1 << 0,
    FORBID_PERSIST = 1 << 1,
} PolicyFlag;
enum PolicyErrorCode {
    FORBIDDEN_TO_BE_PERSISTED = 1,
    INVALID_MODE = 2,
    INVALID_PATH = 3,
};
struct UriPolicyInfo {
    string uri = "";
    uint32_t mode = OperateMode::READ_MODE;
};
struct PolicyInfo {
    string path = "";
    uint32_t mode = OperateMode::READ_MODE;
};
struct PolicyErrorResult {
    string uri = "";
    PolicyErrorCode code = PolicyErrorCode::FORBIDDEN_TO_BE_PERSISTED;
    string message = "";
};

class FilePermission {
public:
    static int32_t GrantPermission(uint32_t tokenId, vector<UriPolicyInfo> uriPolicies, uint32_t policyFlag);
    static int32_t PersistPermission(vector<UriPolicyInfo> uriPolicies, deque<struct PolicyErrorResult> &errorResults);
    static int32_t RevokePermission(vector<UriPolicyInfo> uriPolicies,
                                           deque<struct PolicyErrorResult> &errorResults);
    static int32_t ActivatePermission(vector<UriPolicyInfo> uriPolicies,
                                        deque<struct PolicyErrorResult> &errorResults);
    static int32_t DeactivatePermission(vector<UriPolicyInfo> uriPolicies,
                                          deque<struct PolicyErrorResult> &errorResults);

private:
    static void GetErrorResults(const vector<uint32_t> &resultCodes,
                                const vector<string> &resultUris,
                                deque<struct PolicyErrorResult> &errorResults);
    static void GetPolicyInformation(const vector<UriPolicyInfo> &uriPolicies,
                                     vector<string> &resultUris,
                                     deque<struct PolicyErrorResult> &errorResults,
                                     vector<struct PolicyInfo> &policies);
};
} // namespace AppFileService
} // namespace OHOS

#endif