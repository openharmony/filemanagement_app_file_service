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
#include "uri_permission_manager_client.h"
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

    struct UriPermissionInfo {
        unsigned int flag;
        string mode;
        string bundleName;
        string uri;
    };

    static bool IsSystemApp()
    {
        uint64_t fullTokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
        return Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(fullTokenId);
    }

    static int32_t GetIdFromUri(string uri)
    {
        std::replace(uri.begin(), uri.end(), '/', ' ');
        stringstream ss(uri);
        string tmp;
        int fileId = -1;
        ss >> tmp >> tmp >> tmp >> fileId;
        return fileId;
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

    static unsigned int GetFlagFromMode(const string &mode)
    {
        unsigned int flag = AAFwk::Want::FLAG_AUTH_READ_URI_PERMISSION;
        if (mode.find("w") != string::npos) {
            flag = AAFwk::Want::FLAG_AUTH_WRITE_URI_PERMISSION;
        }
        return flag;
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

    static int InsertByDatashare(const DataShareValuesBucket &valuesBucket, bool isApi10)
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

    static int InitValuesBucket(const UriPermissionInfo &uriPermInfo, Uri &uri, bool &isApi10,
                                DataShareValuesBucket &valuesBucket)
    {
        int32_t fileId = GetIdFromUri(uriPermInfo.uri);
        if (fileId == -1) {
            LOGE("FileShare::InitValuesBucket get fileId parameter failed!");
            return -EINVAL;
        }

        int32_t filesType = GetMediaTypeAndApiFromUri(uri.ToString(), isApi10);
        valuesBucket.Put(PERMISSION_FILE_ID, fileId);
        valuesBucket.Put(PERMISSION_BUNDLE_NAME, uriPermInfo.bundleName);
        valuesBucket.Put(PERMISSION_MODE, uriPermInfo.mode);
        valuesBucket.Put(PERMISSION_TABLE_TYPE, filesType);
        return 0;
    }

    static int GrantInMediaLibrary(const UriPermissionInfo &uriPermInfo, Uri &uri)
    {
        bool isApi10 = false;
        DataShareValuesBucket valuesBucket;
        int ret = InitValuesBucket(uriPermInfo, uri, isApi10, valuesBucket);
        if (ret < 0) {
            return ret;
        }

        ret = InsertByDatashare(valuesBucket, isApi10);
        if (ret < 0) {
            return ret;
        }
        return 0;
    }

    static int DoGrantUriPermission(const UriPermissionInfo &uriPermInfo)
    {
        Uri uri(uriPermInfo.uri);
        string authority = uri.GetAuthority();
        string path = uri.GetPath();
        if (authority == MEDIA_AUTHORITY && path.find(".") == string::npos) {
            return GrantInMediaLibrary(uriPermInfo, uri);
        } else {
            auto& uriPermissionClient = AAFwk::UriPermissionManagerClient::GetInstance();
            int ret =  uriPermissionClient.GrantUriPermission(uri, uriPermInfo.flag,
                                                              uriPermInfo.bundleName);
            if (ret != 0) {
                LOGD("uriPermissionClient.GrantUriPermission by uri permission client failed!");
                return GrantInMediaLibrary(uriPermInfo, uri);
            }
        }

        return 0;
    }

    static bool CheckValidPublicUri(const string &inputUri)
    {
        Uri uri(inputUri);
        string scheme = uri.GetScheme();
        if (scheme != FILE_SCHEME) {
            return false;
        }

        string authority = uri.GetAuthority();
        if (authority != MEDIA_AUTHORITY && authority != FILE_MANAGER_AUTHORITY) {
            return false;
        }

        return true;
    }

    static bool GetJSArgs(napi_env env, const NFuncArg &funcArg, UriPermissionInfo &uriPermInfo)
    {
        auto [succUri, uri, lenUri] = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
        if (!succUri) {
            LOGE("FileShare::GetJSArgs get uri parameter failed!");
            NError(EINVAL).ThrowErr(env);
            return false;
        }

        uriPermInfo.uri = string(uri.get());
        if (!CheckValidPublicUri(uriPermInfo.uri)) {
            LOGE("FileShare::GetJSArgs uri = %{private}s parameter format error!", uriPermInfo.uri.c_str());
            NError(EINVAL).ThrowErr(env);
            return false;
        }

        LOGD("FileShare::GetJSArgs uri = %{private}s", uriPermInfo.uri.c_str());

        auto [succBundleName, bundleName, lenBundleName] = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
        if (!succBundleName) {
            LOGE("FileShare::GetJSArgs get bundleName parameter failed!");
            NError(EINVAL).ThrowErr(env);
            return false;
        }
        uriPermInfo.bundleName = string(bundleName.get());

        if (NVal(env, funcArg[NARG_POS::THIRD]).TypeIs(napi_number)) {
            auto [succFlag, flag] = NVal(env, funcArg[NARG_POS::THIRD]).ToUint32();
            uriPermInfo.flag = flag;
            uriPermInfo.mode = GetModeFromFlag(flag);
        } else if (NVal(env, funcArg[NARG_POS::THIRD]).TypeIs(napi_string)) {
            auto [succFlag, flag, lenFlag] = NVal(env, funcArg[NARG_POS::THIRD]).ToUTF8String();
            uriPermInfo.mode = string(flag.get());
            uriPermInfo.flag = GetFlagFromMode(uriPermInfo.mode);
        } else {
            LOGE("FileShare::GetJSArgs get flag parameter failed!");
            NError(EINVAL).ThrowErr(env);
            return false;
        }

        return true;
    }

    napi_value GrantUriPermission::Async(napi_env env, napi_callback_info info)
    {
        LOGD("FileShare::GrantUriPermission begin!");
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

        UriPermissionInfo uriPermInfo;
        bool result = GetJSArgs(env, funcArg, uriPermInfo);
        if (!result) {
            LOGE("FileShare::GrantUriPermission GetJSArgs failed!");
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }

        auto cbExec = [uriPermInfo, env]() -> NError {
            int ret = DoGrantUriPermission(uriPermInfo);
            if (ret < 0) {
                LOGE("FileShare::GrantUriPermission DoGrantUriPermission failed with %{public}d", ret);
                return NError(-ret);
            }
            LOGD("FileShare::GrantUriPermission DoGrantUriPermission successfully!");
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
