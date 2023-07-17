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
#include "get_uri_from_path.h"

#include "status_receiver_host.h"

#include "common_func.h"
#include "log.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileUri {
using namespace OHOS::FileManagement::LibN;

napi_value GetUriFromPath::Sync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("GetUriFromPath::Sync Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto [succPath, path, ignore] = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succPath) {
        LOGE("GetUriFromPath::Sync get path parameter failed!");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    std::string uri = CommonFunc::GetUriFromPath(path.get());
    if (uri == "") {
        LOGE("GetUriFromPath failed!");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateUTF8String(env, uri).val_;
}

} // namespace ModuleFileUri
} // namespace AppFileService
} // namespace OHOS