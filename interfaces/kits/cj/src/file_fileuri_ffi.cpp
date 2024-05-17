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

#include "file_fileuri_ffi.h"
#include "file_uri.h"
#include "macro.h"
#include "common_func.h"
#include "securec.h"
#include <cinttypes>

using namespace OHOS::FFI;

namespace OHOS {
namespace CJSystemapi {
namespace FileUri {


extern "C" {
char* MallocCString(const std::string& origin)
{
    if (origin.empty()) {
        return nullptr;
    }
    auto length = origin.length() + 1;
    char* res = static_cast<char*>(malloc(sizeof(char) * length));
    if (res == nullptr) {
        return nullptr;
    }
    return std::char_traits<char>::copy(res, origin.c_str(), length);
}

int64_t FfiOHOSFILEUriCreateUri(const char* uriOrPath)
{
    LOGI("FILEURI_TEST::FfiOHOSFILEUriCreateUri");
    auto nativeUri = FFIData::Create<FileUriImpl>(uriOrPath);
    int64_t id = nativeUri->GetID();
    LOGI("FILEURI_TEST::FfiOHOSFILEUriCreateUri %{public}" PRId64, id);
    return id;
}

char* FfiOHOSFILEUriGetPath(int64_t id)
{
    LOGI("FILEURI_TEST::FfiOHOSFILEUriGetPath");
    auto instance = FFIData::GetData<FileUriImpl>(id);
    auto path = instance->GetPath();
    char* result = MallocCString(path);
    return result;
}

char* FfiOHOSFILEUriGetName(int64_t id)
{
    LOGI("FILEURI_TEST::FfiOHOSFILEUriGetName");
    auto instance = FFIData::GetData<FileUriImpl>(id);
    auto name = instance->GetName();
    char* result = MallocCString(name);
    return result;
}

char* FfiOHOSFILEUriToString(int64_t id)
{
    LOGI("FILEURI_TEST::FfiOHOSFILEUriToString");
    auto instance = FFIData::GetData<FileUriImpl>(id);
    auto str = instance->ToString();
    char* result = MallocCString(str);
    return result;
}

char* FfiOHOSFILEUriGetUriFromPath(const char* path)
{
    auto uri = AppFileService::CommonFunc::GetUriFromPath(path);
    char* result = MallocCString(uri);
    return result;
}
}
}
} //namespace CJSystemapi
} //namespace OHOS