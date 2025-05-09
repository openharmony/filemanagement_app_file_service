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

#ifndef GRANT_PERMISSIONS_H
#define GRANT_PERMISSIONS_H

#include <deque>
#include <functional>

#include "file_permission.h"
#include "filemgmt_libn.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileShare {
using ErrParam = std::function<std::tuple<uint32_t, std::string>()>;
napi_value PersistPermission(napi_env env, napi_callback_info info);
napi_value RevokePermission(napi_env env, napi_callback_info info);
napi_value ActivatePermission(napi_env env, napi_callback_info info);
napi_value DeactivatePermission(napi_env env, napi_callback_info info);
napi_value CheckPersistentPermission(napi_env env, napi_callback_info info);
napi_value CheckPathPermission(napi_env env, napi_callback_info info);

struct PolicyErrorArgs {
    deque<PolicyErrorResult> errorResults;
    int32_t errNo = 0;
    ~PolicyErrorArgs() = default;
};

struct PolicyInfoResultArgs {
    vector<bool> resultData;
    int32_t errNo = 0;
    ~PolicyInfoResultArgs() = default;
};
} // namespace ModuleFileShare
} // namespace AppFileService
} // namespace OHOS
#endif // _GRANT_PERMISSIONS_H_
