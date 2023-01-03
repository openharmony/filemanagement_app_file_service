/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef FILE_URI_N_EXPOTER_H
#define FILE_URI_N_EXPOTER_H

#include "filemgmt_libn.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileUri {
    napi_value FileUriExport(napi_env env, napi_value exports);
} // namespace ModuleFileUri
} // namespace AppFileService
} // namespace OHOS
#endif // FILE_URI_N_EXPOTER_H
