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

#ifndef OHOS_FILE_FILEURI_FILEURI_FFI_H
#define OHOS_FILE_FILEURI_FILEURI_FFI_H

#include "native/ffi_remote_data.h"
#include "cj_ffi/cj_common_ffi.h"

#include <cstdint>

extern "C" {
    FFI_EXPORT int64_t FfiOHOSFILEUriCreateUri(const char* uriOrPath);
    FFI_EXPORT char* FfiOHOSFILEUriGetPath(int64_t id);
    FFI_EXPORT char* FfiOHOSFILEUriGetName(int64_t id);
    FFI_EXPORT char* FfiOHOSFILEUriToString(int64_t id);
    FFI_EXPORT char* FfiOHOSFILEUriGetUriFromPath(const char* path);
}

#endif // OHOS_FILE_FILEURI_FILEURI_FFI_H