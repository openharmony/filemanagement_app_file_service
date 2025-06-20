/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILE_GRANT_PERMISSION_UTILS_H
#define OHOS_FILE_GRANT_PERMISSION_UTILS_H

#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#include "cj_ffi/cj_common_ffi.h"
#include "file_permission.h"

namespace OHOS {
namespace CJSystemapi {
namespace FileGrantPermission {

struct CPolicyInfo {
    char* uri;
    uint32_t mode;
};

struct CArrCPolicyInfo {
    CPolicyInfo* head;
    int64_t size;
};

struct PolicyErrorArgs {
    std::deque<AppFileService::PolicyErrorResult> errorResults;
    int32_t errNo = 0;
    ~PolicyErrorArgs() = default;
};

struct PolicyInfoResultArgs {
    std::vector<bool> resultData;
    int32_t errNo = 0;
    ~PolicyInfoResultArgs() = default;
};

struct CPolicyErrorResult {
    char* uri;
    int32_t code;
    char* message;
};

struct CArrCPolicyErrorResult {
    CPolicyErrorResult* head;
    int64_t size;
};

struct CArrBool {
    bool* head;
    int64_t size;
};

struct CJError {
    int32_t errorcode;
    std::string message;
};

enum CommonErrCode {
    E_UNKNOWN_ERROR = 13900042
};

} // namespace FileGrantPermission
} // namespace CJSystemapi
} // namespace OHOS
#endif // OHOS_FILE_GRANT_PERMISSION_UTILS_H