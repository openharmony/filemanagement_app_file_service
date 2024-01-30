/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "b_error/b_error.h"
#include "b_incremental_data.h"
#include "filemgmt_libhilog.h"
#include "filemgmt_libn.h"
#include "incremental_backup_data.h"
#include "service_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace LibN;

static napi_value AsyncCallback(napi_env env, const NFuncArg& funcArg)
{
    HILOGI("called LocalCapabilities::AsyncCallback begin");

    auto fd = make_shared<UniqueFd>();
    auto cbExec = [fd]() -> NError {
        HILOGI("called LocalCapabilities::AsyncCallback cbExec");
        ServiceProxy::InvaildInstance();
        auto proxy = ServiceProxy::GetInstance();
        if (!proxy) {
            HILOGI("called LocalCapabilities::AsyncCallback cbExec, failed to get proxy");
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

static bool CheckDataList(const LibN::NVal &data)
{
    LibN::NVal name = data.GetProp("bundleName");
    if (name.val_ == nullptr) {
        return false;
    }
    auto [succ, str, ignore] = name.ToUTF8String();
    if (!succ) {
        return false;
    }

    LibN::NVal time = data.GetProp("lastIncrementalTime");
    if (time.val_ == nullptr) {
        return false;
    }
    tie(succ, ignore) = time.ToInt64();
    if (!succ) {
        return false;
    }
    return true;
}

static std::tuple<bool, std::vector<BIncrementalData>> ParseDataList(napi_env env, const napi_value& value)
{
    uint32_t size = 0;
    napi_status status = napi_get_array_length(env, value, &size);
    if (status != napi_ok) {
        HILOGE("Get array length failed!");
        return {false, {}};
    }
    if (size == 0) {
        HILOGI("array length is zero!");
        return {true, {}};
    }

    napi_value result;
    std::vector<BIncrementalData> backupData;
    for (uint32_t i = 0; i < size; i++) {
        status = napi_get_element(env, value, i, &result);
        if (status != napi_ok) {
            HILOGE("Get element failed! index is :%{public}u", i);
            return {false, {}};
        } else {
            NVal element(env, result);
            if (!CheckDataList(element)) {
                HILOGE("bundles are invalid!");
                return {false, {}};
            }
            IncrementalBackupData data(element);
            backupData.emplace_back(data.bundleName,
                                    data.lastIncrementalTime,
                                    data.manifestFd,
                                    data.parameters,
                                    data.priority);
        }
    }
    return {true, backupData};
}

static napi_value AsyncDataList(napi_env env, const NFuncArg& funcArg)
{
    HILOGI("called LocalCapabilities::AsyncDataList begin");

    auto [succ, bundles] = ParseDataList(env, funcArg[NARG_POS::FIRST]);
    if (!succ) {
        HILOGE("bundles array invalid.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "bundles array invalid.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    auto fd = make_shared<UniqueFd>();
    auto cbExec = [fd, bundles { move(bundles) }]() -> NError {
        HILOGI("called LocalCapabilities::AsyncDataList cbExec");
        ServiceProxy::InvaildInstance();
        auto proxy = ServiceProxy::GetInstance();
        if (!proxy) {
            HILOGI("called LocalCapabilities::AsyncDataList cbExec, failed to get proxy");
            return NError(errno);
        }
        *fd = proxy->GetLocalCapabilitiesIncremental(bundles);
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

    HILOGI("called LocalCapabilities::Async::promise");
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(PROCEDURE_LOCALCAPABILITIES_NAME, cbExec, cbCompl).val_;
}

napi_value LocalCapabilities::Async(napi_env env, napi_callback_info info)
{
    HILOGI("called LocalCapabilities::Async begin");
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO, NARG_CNT::ONE)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return nullptr;
    }

    if (funcArg.GetArgc() == 1) {
        bool result = 0;
        napi_status status = napi_is_array(env, funcArg[NARG_POS::FIRST], &result);
        if (status == napi_ok && result) {
            return AsyncDataList(env, funcArg);
        }
    }

    return AsyncCallback(env, funcArg);
}
} // namespace OHOS::FileManagement::Backup