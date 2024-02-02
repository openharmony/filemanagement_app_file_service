/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#ifndef INTERFACES_KITS_JS_FILE_URI_FILE_URI_N_EXPOTER_H
#define INTERFACES_KITS_JS_FILE_URI_FILE_URI_N_EXPOTER_H

#include "filemgmt_libn.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileUri {
class FileUriNExporter final : public FileManagement::LibN::NExporter {
public:
    inline static const std::string className = "FileUri";

    bool Export() override;
    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value UriToString(napi_env env, napi_callback_info info);
    static napi_value GetFileUriName(napi_env env, napi_callback_info info);
    static napi_value GetFileUriPath(napi_env env, napi_callback_info info);
    static napi_value GetFullDirectoryUri(napi_env env, napi_callback_info info);
    static napi_value IsRemoteUri(napi_env env, napi_callback_info info);
    static napi_value Normalize(napi_env env, napi_callback_info info);
    static napi_value Equals(napi_env env, napi_callback_info info);
    static napi_value EqualsTo(napi_env env, napi_callback_info info);
    static napi_value IsAbsolute(napi_env env, napi_callback_info info);
    static napi_value GetScheme(napi_env env, napi_callback_info info);
    static napi_value GetAuthority(napi_env env, napi_callback_info info);
    static napi_value GetSsp(napi_env env, napi_callback_info info);
    static napi_value GetUserInfo(napi_env env, napi_callback_info info);
    static napi_value GetHost(napi_env env, napi_callback_info info);
    static napi_value GetPort(napi_env env, napi_callback_info info);
    static napi_value GetQuery(napi_env env, napi_callback_info info);
    static napi_value GetFragment(napi_env env, napi_callback_info info);

    FileUriNExporter(napi_env env, napi_value exports);
    ~FileUriNExporter() override;
};
} // namespace ModuleFileUri
} // namespace AppFileService
} // namespace OHOS
#endif // INTERFACES_KITS_JS_FILE_URI_FILE_URI_N_EXPOTER_H
