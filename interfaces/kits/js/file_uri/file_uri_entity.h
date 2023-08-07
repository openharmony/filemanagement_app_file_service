/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_KITS_JS_FILE_URI_FILE_URI_ENTITY_H
#define INTERFACES_KITS_JS_FILE_URI_FILE_URI_ENTITY_H

#include "file_uri.h"

namespace OHOS::AppFileService::ModuleFileUri {
struct FileUriEntity {
    FileUri fileUri_;
    explicit FileUriEntity(const std::string &uriOrPath): fileUri_(FileUri(uriOrPath)){};
};
} // OHOS::AppFileService::ModuleFileUri
#endif // INTERFACES_KITS_JS_FILE_URI_FILE_URI_ENTITY_H