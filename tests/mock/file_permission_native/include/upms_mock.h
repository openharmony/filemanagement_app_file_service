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

#ifndef FILEMANAGEMENT_APP_FILE_SERVICE_UPMS_MOCK_H
#define FILEMANAGEMENT_APP_FILE_SERVICE_UPMS_MOCK_H
#ifdef SANDBOX_MANAGER
#include <gmock/gmock.h>

#include "uri_permission_manager_client.h"

namespace OHOS {
namespace AppFileService {
using namespace OHOS::AccessControl::SandboxManager;
class UpmsManagerKitMock {
public:
    virtual ~UpmsManagerKitMock() = default;
    virtual int32_t Active(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result) = 0;
    static inline std::shared_ptr<UpmsManagerKitMock> upmsManagerKitMock = nullptr;
};

class UpmsMock : public UpmsManagerKitMock {
public:
    MOCK_METHOD2(Active, int32_t(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result));
};
} // namespace AppFileService
} // namespace OHOS
#endif
#endif
