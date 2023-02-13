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
#include "grant_uri_permission.h"

#include "ability.h"
#include "datashare_helper.h"
#include "datashare_values_bucket.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "remote_uri.h"
#include "tokenid_kit.h"
#include "want.h"

using namespace OHOS::DataShare;
using namespace OHOS::FileManagement::LibN;
using namespace OHOS::DistributedFS::ModuleRemoteUri;

namespace OHOS {
namespace AppFileService {
namespace ModuleFileShare {

    static bool IsAllDigits(string idStr)
    {
        for (size_t i = 0; i < idStr.size(); i++) {
            if (!isdigit(idStr[i])) {
                return false;
            }
        }
        return true;
    }

    static bool IsSystemApp()
    {
        uint64_t fullTokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
        return Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(fullTokenId);
    }

    static string GetIdFromUri(string uri)
    {
        string rowNum = "";
        size_t pos = uri.rfind('/');
        if (pos != string::npos) {
            rowNum = uri.substr(pos + 1);
            if (!IsAllDigits(rowNum)) {
                rowNum = "";
            }
        }
        return rowNum;
    }

    static string GetModeFromFlag(unsigned int flag)
    {
        string mode = "";
        if (flag & OHOS::AAFwk::Want::FLAG_AUTH_READ_URI_PERMISSION) {
            mode += "r";
        }
        if (flag & OHOS::AAFwk::Want::FLAG_AUTH_WRITE_URI_PERMISSION) {
            mode += "w";
        }
        return mode;
    }

    static napi_value GetJSArgs(napi_env env, const NFuncArg &funcArg,
                                DataShareValuesBucket &valuesBucket)
    {
        napi_value result = nullptr;
        auto [succPath, path, lenPath] = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
        if (!succPath) {
            LOGE("FileShare::GetJSArgs get path parameter failed!");
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }

        if (!DistributedFS::ModuleRemoteUri::RemoteUri::IsMediaUri(path.get())) {
            LOGE("FileShare::GetJSArgs path parameter format error!");
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }

        auto [succBundleName, bundleName, lenBundleName] = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
        if (!succBundleName) {
            LOGE("FileShare::GetJSArgs get bundleName parameter failed!");
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }

        string mode;
        if (NVal(env, funcArg[NARG_POS::THIRD]).TypeIs(napi_number)) {
            auto [succFlag, flag] = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
            mode = GetModeFromFlag(flag);
        } else if (NVal(env, funcArg[NARG_POS::THIRD]).TypeIs(napi_string)) {
            auto [succFlag, flag, lenFlag] = NVal(env, funcArg[NARG_POS::THIRD]).ToUTF8String();
            mode = string(flag.get());
        } else {
            LOGE("FileShare::GetJSArgs get flag parameter failed!");
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }

        string idStr = GetIdFromUri(string(path.get()));
        if (idStr == "") {
            LOGE("FileShare::GetJSArgs get fileId parameter failed!");
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }

        int32_t fileId = stoi(idStr);
        valuesBucket.Put(PERMISSION_FILE_ID, fileId);
        valuesBucket.Put(PERMISSION_BUNDLE_NAME, string(bundleName.get()));
        valuesBucket.Put(PERMISSION_MODE, mode);

        napi_get_boolean(env, true, &result);
        return result;
    }

    static int InsertByDatashare(napi_env env, const DataShareValuesBucket &valuesBucket)
    {
        int ret = -1;
        std::shared_ptr<DataShare::DataShareHelper> dataShareHelper = nullptr;
        sptr<FileShareGrantToken> remote = new IRemoteStub<FileShareGrantToken>();
        if (remote == nullptr) {
            LOGE("FileShare::InsertByDatashare get remoteObject failed!");
            return ret;
        }

        dataShareHelper = DataShare::DataShareHelper::Creator(remote->AsObject(), MEDIALIBRARY_DATA_URI);
        Uri uri(MEDIA_GRANT_URI_PERMISSION);
        ret = dataShareHelper->Insert(uri, valuesBucket);
        if (ret < 0) {
            LOGE("FileShare::InsertByDatashare insert failed with error code %{public}d!", ret);
            return ret;
        }
        return ret;
    }

    napi_value GrantUriPermission::Async(napi_env env, napi_callback_info info)
    {
        if (!IsSystemApp()) {
            LOGE("FileShare::GrantUriPermission is not System App!");
            NError(E_PERMISSION_SYS).ThrowErr(env);
            return nullptr;
        }
        NFuncArg funcArg(env, info);
        if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
            LOGE("FileShare::GrantUriPermission GetJSArgsForGrantUriPermission Number of arguments unmatched!");
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }

        OHOS::DataShare::DataShareValuesBucket valuesBucket;
        bool result = GetJSArgs(env, funcArg, valuesBucket);
        if (!result) {
            LOGE("FileShare::GrantUriPermission GetJSArgsForGrantUriPermission failed!");
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }

        auto cbExec = [valuesBucket, env]() -> NError {
            int ret = InsertByDatashare(env, valuesBucket);
            if (ret < 0) {
                LOGE("FileShare::GrantUriPermission InsertByDatashare failed!");
                return NError(-ret);
            }
            return NError(ERRNO_NOERR);
        };

        auto cbCompl = [](napi_env env, NError err) -> NVal {
            if (err) {
                return { env, err.GetNapiErr(env) };
            }
            return NVal::CreateUndefined(env);
        };

        NVal thisVar(env, funcArg.GetThisVar());
        if (funcArg.GetArgc() == NARG_CNT::THREE) {
            return NAsyncWorkPromise(env, thisVar).Schedule(GRANT_URI_NAME, cbExec, cbCompl).val_;
        } else {
            NVal cb(env, funcArg[NARG_POS::FOURTH]);
            return NAsyncWorkCallback(env, thisVar, cb).Schedule(GRANT_URI_NAME, cbExec, cbCompl).val_;
        }
    }
} // namespace ModuleFileShare
} // namespace AppFileService
} // namespace OHOS
