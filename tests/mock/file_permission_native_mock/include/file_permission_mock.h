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

#ifndef FILEMANAGEMENT_FILE_PERMISSION_MOCK_MOCK_H
#define FILEMANAGEMENT_FILE_PERMISSION_MOCK_MOCK_H
#include <gmock/gmock.h>

#include "file_permission.h"
namespace OHOS {
namespace AppFileService {
class IFilePermissionMock {
public:
    virtual ~IFilePermissionMock() = default;
    virtual int32_t PersistPermission(const vector<UriPolicyInfo> &uriPolicies,
                                      deque<struct PolicyErrorResult> &errorResults) = 0;
    virtual int32_t RevokePermission(const vector<UriPolicyInfo> &uriPolicies,
                                     deque<struct PolicyErrorResult> &errorResults) = 0;
    virtual int32_t ActivatePermission(const vector<UriPolicyInfo> &uriPolicies,
                                       deque<struct PolicyErrorResult> &errorResults) = 0;
    virtual int32_t DeactivatePermission(const vector<UriPolicyInfo> &uriPolicies,
                                         deque<struct PolicyErrorResult> &errorResults) = 0;
    virtual int32_t CheckPersistentPermission(const vector<UriPolicyInfo> &uriPolicies,
                                              vector<bool> &errorResults) = 0;
public:
    static inline std::shared_ptr<IFilePermissionMock> filePermissionMock = nullptr;
};

class FilePermissionMock : public IFilePermissionMock {
public:
    MOCK_METHOD2(PersistPermission,
                 int32_t(const vector<UriPolicyInfo> &uriPolicies, deque<struct PolicyErrorResult> &errorResults));
    MOCK_METHOD2(RevokePermission, int32_t(const vector<UriPolicyInfo> &uriPolicies,
                                           deque<struct PolicyErrorResult> &errorResults));
    MOCK_METHOD2(ActivatePermission, int32_t(const vector<UriPolicyInfo> &uriPolicies,
                                             deque<struct PolicyErrorResult> &errorResults));
    MOCK_METHOD2(DeactivatePermission, int32_t(const vector<UriPolicyInfo> &uriPolicies,
                                               deque<struct PolicyErrorResult> &errorResults));
    MOCK_METHOD2(CheckPersistentPermission, int32_t(const vector<UriPolicyInfo> &uriPolicies,
                                                    vector<bool> &errorResults));
};
}
}
#endif
