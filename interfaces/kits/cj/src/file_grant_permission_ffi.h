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

#ifndef OHOS_FILE_GRANT_PERMISSION_FFI_H
#define OHOS_FILE_GRANT_PERMISSION_FFI_H

#include <cstdint>

#include "file_grant_permission_utils.h"
#include "native/ffi_remote_data.h"

namespace OHOS {
namespace CJSystemapi {
namespace FileGrantPermission {

extern "C" {
FFI_EXPORT CArrCPolicyErrorResult FfiOHOSFileGPPersistPermission(CArrCPolicyInfo policies, int32_t* code);
FFI_EXPORT CArrCPolicyErrorResult FfiOHOSFileGPRevokePermission(CArrCPolicyInfo policies, int32_t* code);
FFI_EXPORT CArrCPolicyErrorResult FfiOHOSFileGPActivatePermission(CArrCPolicyInfo policies, int32_t* code);
FFI_EXPORT CArrCPolicyErrorResult FfiOHOSFileGPDeactivatePermission(CArrCPolicyInfo policies, int32_t* code);
FFI_EXPORT CArrBool FfiOHOSFileGPCheckPersistentPermission(CArrCPolicyInfo policies, int32_t* code);
}
} // namespace FileGrantPermission
} // namespace CJSystemapi
} // namespace OHOS

#endif // OHOS_FILE_GRANT_PERMISSION_FFI_H