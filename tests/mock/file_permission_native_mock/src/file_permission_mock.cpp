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
#include "file_permission_mock.h"

namespace OHOS {
using namespace AppFileService;
int32_t FilePermission::PersistPermission(const vector<UriPolicyInfo> &uriPolicies,
                                          deque<struct PolicyErrorResult> &errorResults)
{
    if (IFilePermissionMock::filePermissionMock == nullptr) {
        return 0;
    }

    return IFilePermissionMock::filePermissionMock->PersistPermission(uriPolicies, errorResults);
}

int32_t FilePermission::RevokePermission(const vector<UriPolicyInfo> &uriPolicies,
                                         deque<struct PolicyErrorResult> &errorResults)
{
    if (IFilePermissionMock::filePermissionMock == nullptr) {
        return 0;
    }

    return IFilePermissionMock::filePermissionMock->RevokePermission(uriPolicies, errorResults);
}

int32_t FilePermission::ActivatePermission(const vector<UriPolicyInfo> &uriPolicies,
                                           deque<struct PolicyErrorResult> &errorResults)
{
    if (IFilePermissionMock::filePermissionMock == nullptr) {
        return 0;
    }

    return IFilePermissionMock::filePermissionMock->ActivatePermission(uriPolicies, errorResults);
}
int32_t FilePermission::DeactivatePermission(const vector<UriPolicyInfo> &uriPolicies,
                                             deque<struct PolicyErrorResult> &errorResults)
{
    if (IFilePermissionMock::filePermissionMock == nullptr) {
        return 0;
    }

    return IFilePermissionMock::filePermissionMock->DeactivatePermission(uriPolicies, errorResults);
}
int32_t FilePermission::CheckPersistentPermission(const vector<UriPolicyInfo> &uriPolicies, vector<bool> &errorResults)
{
    if (IFilePermissionMock::filePermissionMock == nullptr) {
        return 0;
    }

    return IFilePermissionMock::filePermissionMock->CheckPersistentPermission(uriPolicies, errorResults);
}
}