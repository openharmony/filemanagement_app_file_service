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

#ifndef FILEMANAGEMENT_APP_FILE_SERVICE_SANDBOX_MOCK_H
#define FILEMANAGEMENT_APP_FILE_SERVICE_SANDBOX_MOCK_H

#include <gmock/gmock.h>
#include <memory>

#include "accesstoken_kit.h"
#include "bundle_constants.h"
#include "hap_token_info.h"
#include "ipc_skeleton.h"
#include "n_error.h"
#include "sandbox_helper.h"
#include "sandbox_manager_err_code.h"
#include "sandbox_manager_kit.h"
#include "tokenid_kit.h"

namespace OHOS::AccessControl::SandboxManager {
class SandboxManagerKit {
public:
    virtual ~SandboxManagerKit() = default;
    virtual int32_t CheckPersistPolicy(uint64_t tokenId,
                                       const std::vector<PolicyInfo> &policy,
                                       std::vector<bool> &result) = 0;
    virtual int32_t PersistPolicy(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result) = 0;
    virtual int32_t UnPersistPolicy(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result) = 0;
    virtual int32_t StartAccessingPolicy(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result) = 0;
    virtual int32_t StopAccessingPolicy(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result) = 0;
    static inline std::shared_ptr<SandboxManagerKit> sandboxManagerKit = nullptr;
};

class SandboxMock : public SandboxManagerKit {
public:
    MOCK_METHOD3(CheckPersistPolicy,
                 int32_t(uint64_t tokenId, const std::vector<PolicyInfo> &policy, std::vector<bool> &result));
    MOCK_METHOD2(PersistPolicy, int32_t(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result));
    MOCK_METHOD2(UnPersistPolicy, int32_t(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result));
    MOCK_METHOD2(StartAccessingPolicy, int32_t(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result));
    MOCK_METHOD2(StopAccessingPolicy, int32_t(const std::vector<PolicyInfo> &policy, std::vector<uint32_t> &result));
};
} // namespace OHOS::AccessControl::SandboxManager
#endif
