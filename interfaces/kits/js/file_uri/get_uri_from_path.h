/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef GET_URI_FROM_PATH_H
#define GET_URI_FROM_PATH_H

#include <string>
#include "filemgmt_libn.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileUri {
using namespace std;

const string SCHEME = "file";
const char SCHEME_SEPARATOR = ':';
const string PATH_SYMBOLS = "//";
const string FRAGMENT_SYMBOLS = "#";

class GetUriFromPath final {
public:
    static napi_value Sync(napi_env env, napi_callback_info info);
};
} // namespace ModuleFileUri
} // namespace AppFileService
} // namespace OHOS

#endif // GET_URI_FROM_PATH_H