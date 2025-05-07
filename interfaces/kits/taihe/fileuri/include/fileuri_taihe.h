/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INTERFACES_KITS_TAIHE_INCLUDE_FILEURI_TAIHE_H
#define INTERFACES_KITS_TAIHE_INCLUDE_FILEURI_TAIHE_H

#include "ohos.file.fileuri.proj.hpp"
#include "ohos.file.fileuri.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include "file_uri.h"

namespace ANI::fileUri {

class FileUriImpl {
public:
    FileUriImpl() = delete;

    FileUriImpl(taihe::string_view name);

    string getName();

private:
    OHOS::Uri uri_;
};

ohos::file::fileuri::FileUri makeFileUri(taihe::string_view name);

string getUriFromPath(taihe::string_view path);

}

#endif // INTERFACES_KITS_TAIHE_INCLUDE_FILEURI_TAIHE_H