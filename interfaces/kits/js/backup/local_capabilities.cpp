/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "local_capabilities.h"

#include "filemgmt_libhilog.h"
#include "filemgmt_libn.h"
#include "service_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace LibN;

napi_value LocalCapabilities::Async(napi_env env, napi_callback_info info)
{
    HILOGI("called LocalCapabilities::Async begin");
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO, NARG_CNT::ONE)) {
        HILOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    auto fd = make_shared<UniqueFd>();
    auto cbExec = [fd]() -> NError {
        HILOGI("called LocalCapabilities::Async cbExec");
        auto proxy = ServiceProxy::GetInstance();
        if (!proxy) {
            HILOGI("called LocalCapabilities::Async cbExec, failed to get proxy");
            return NError(errno);
        }
        *fd = proxy->GetLocalCapabilities();
        return NError(ERRNO_NOERR);
    };
    auto cbCompl = [fd](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        NVal obj = NVal::CreateObject(env);
        obj.AddProp({NVal::DeclareNapiProperty("fd", NVal::CreateInt32(env, fd->Release()).val_)});
        return {obj};
    };

    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ZERO) {
        HILOGI("called LocalCapabilities::Async::promise");
        return NAsyncWorkPromise(env, thisVar).Schedule(PROCEDURE_LOCALCAPABILITIES_NAME, cbExec, cbCompl).val_;
    } else {
        HILOGI("called LocalCapabilities::Async::callback");
        NVal cb(env, funcArg[NARG_POS::FIRST]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(PROCEDURE_LOCALCAPABILITIES_NAME, cbExec, cbCompl).val_;
    }
}
} // namespace OHOS::FileManagement::Backup