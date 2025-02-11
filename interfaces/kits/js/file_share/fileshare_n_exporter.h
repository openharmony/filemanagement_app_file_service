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
#ifndef FILESHARE_N_EXPOTER_H
#define FILESHARE_N_EXPOTER_H

#include "filemgmt_libn.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileShare {
void InitOperationMode(napi_env env, napi_value exports);
void InitPolicyFlag(napi_env env, napi_value exports);
void InitPolicyType(napi_env env, napi_value exports);
void InitPolicyInfo(napi_env env, napi_value exports);
void InitPathPolicyInfo(napi_env env, napi_value exports);
void InitPolicyErrorResult(napi_env env, napi_value exports);
void InitPolicyErrorCode(napi_env env, napi_value exports);
napi_value FileShareExport(napi_env env, napi_value exports);
} // namespace ModuleFileShare
} // namespace AppFileService
} // namespace OHOS
#endif // FILESHARE_N_EXPOTER_H
