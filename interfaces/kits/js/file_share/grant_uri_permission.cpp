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
    enum MediaFileTable {
        FILE_TABLE = 0,
        PHOTO_TABLE = 1,
        AUDIO_TABLE = 2,
    };

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

    static string DealWithUriWithName(string str)
    {
        static uint32_t MEET_COUNT = 6;
        uint32_t count = 0;
        uint32_t index;
        for (index = 0; index < str.length(); index++) {
            if (str[index] == '/') {
                count++;
            }
            if (count == MEET_COUNT) {
                break;
            }
        }
        if (count == MEET_COUNT) {
            str = str.substr(0, index);
        }
        return str;
    }

    static string GetIdFromUri(string uri)
    {
        uri = DealWithUriWithName(uri);
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

    static int32_t GetMediaTypeAndApiFromUri(const std::string &uri, bool &isApi10)
    {
        if (uri.find(MEDIA_FILE_URI_PHOTO_PREFEX) == 0) {
            isApi10 = true;
            return MediaFileTable::PHOTO_TABLE;
        } else if (uri.find(MEDIA_FILE_URI_VIDEO_PREFEX) == 0 ||
            uri.find(MEDIA_FILE_URI_IMAGE_PREFEX) == 0) {
            return MediaFileTable::PHOTO_TABLE;
        } else if (uri.find(MEDIA_FILE_URI_AUDIO_PREFEX) == 0) {
            isApi10 = true;
            return MediaFileTable::AUDIO_TABLE;
        } else if (uri.find(MEDIA_FILE_URI_Audio_PREFEX) == 0) {
            return MediaFileTable::AUDIO_TABLE;
        } else if (uri.find(MEDIA_FILE_URI_FILE_PREFEX) == 0) {
            return MediaFileTable::FILE_TABLE;
        }

        return MediaFileTable::FILE_TABLE;
    }

    static napi_value GetJSArgs(napi_env env, const NFuncArg &funcArg,
                                DataShareValuesBucket &valuesBucket, bool &isApi10)
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
        int32_t filesType = GetMediaTypeAndApiFromUri(string(path.get()), isApi10);
        valuesBucket.Put(PERMISSION_FILE_ID, fileId);
        valuesBucket.Put(PERMISSION_BUNDLE_NAME, string(bundleName.get()));
        valuesBucket.Put(PERMISSION_MODE, mode);
        valuesBucket.Put(PERMISSION_TABLE_TYPE, filesType);
        napi_get_boolean(env, true, &result);
        return result;
    }

    static int InsertByDatashare(napi_env env, const DataShareValuesBucket &valuesBucket, bool isApi10)
    {
        int ret = -1;
        std::shared_ptr<DataShare::DataShareHelper> dataShareHelper = nullptr;
        sptr<FileShareGrantToken> remote = new IRemoteStub<FileShareGrantToken>();
        if (remote == nullptr) {
            LOGE("FileShare::InsertByDatashare get remoteObject failed!");
            return -ENOMEM;
        }

        dataShareHelper = DataShare::DataShareHelper::Creator(remote->AsObject(), MEDIALIBRARY_DATA_URI);
        if (!dataShareHelper) {
            LOGE("FileShare::InsertByDatashare connect to datashare failed!");
            return -E_PERMISSION;
        }
        string uriStr = MEDIA_GRANT_URI_PERMISSION;
        if (isApi10) {
            uriStr +=  MEDIA_API_VERSION_10;
        }

        Uri uri(uriStr);
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
        bool isApi10 = false;
        bool result = GetJSArgs(env, funcArg, valuesBucket, isApi10);
        if (!result) {
            LOGE("FileShare::GrantUriPermission GetJSArgsForGrantUriPermission failed!");
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }

        auto cbExec = [valuesBucket, isApi10, env]() -> NError {
            int ret = InsertByDatashare(env, valuesBucket, isApi10);
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
