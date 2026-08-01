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

#ifndef OHOS_FILE_FILEURI_URI_FFI_H
#define OHOS_FILE_FILEURI_URI_FFI_H

#include <string>

#include "uri.h"
#include "native/ffi_remote_data.h"
#include "cj_ffi/cj_common_ffi.h"

namespace OHOS {
namespace CJSystemapi {
namespace FileUri {

class FileUriImpl : public OHOS::FFI::FFIData {
public:
    explicit FileUriImpl(const std::string &uriOrPath);

    std::string GetName();
    std::string GetPath();
    std::string ToString();

    OHOS::FFI::RuntimeType* GetRuntimeType() override { return GetClassType(); }
private:
    Uri uri_;

    friend class OHOS::FFI::RuntimeType;
    friend class OHOS::FFI::TypeBase;
    static OHOS::FFI::RuntimeType* GetClassType()
    {
        static OHOS::FFI::RuntimeType runtimeType = OHOS::FFI::RuntimeType::Create<OHOS::FFI::FFIData>("FileUriImpl");
        return &runtimeType;
    }
};

}
}  // namespace CJSystemapi
}  // namespace OHOS
#endif  // OHOS_FILE_FILEURI_URI_FFI_H