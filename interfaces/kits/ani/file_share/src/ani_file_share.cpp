 /*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ani_file_share.h"

#include <ani.h>
#include <iostream>
#include <vector>
#include "ability.h"
#include "datashare_helper.h"
#include "datashare_values_bucket.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "n_error.h"
#include "remote_uri.h"
#include "tokenid_kit.h"
#include "uri.h"
#include "uri_permission_manager_client.h"
#include "want.h"

using namespace OHOS::DataShare;
using namespace OHOS::DistributedFS::ModuleRemoteUri;
using namespace OHOS::FileManagement::LibN;

namespace OHOS {
namespace AppFileService {
namespace ModuleFileShare {

static std::string ParseObjToStr(ani_env *env, ani_string stringObj)
{
    ani_size  strSize;
    env->String_GetUTF8Size(stringObj, &strSize);
    std::vector<char> buffer(strSize + 1);
    char* utf8Buffer = buffer.data();

    ani_size byteswritten = 0;
    env->String_GetUTF8(stringObj, utf8Buffer, strSize + 1, &byteswritten);

    if (byteswritten <= strSize) {
        utf8Buffer[byteswritten] = '\0';
    } else {
        utf8Buffer[strSize] = '\0';
    }
    std::string path = std::string(utf8Buffer);
    return path;
}

static ani_int ParseEnumToInt(ani_env *env, ani_enum_item enumItem)
{
    ani_int intValue = -1;
    if (ANI_OK != env->EnumItem_GetValue_Int(enumItem, &intValue)) {
        LOGE("%{public}s: EnumItem_GetValue_Int FAILD.", __func__);
        return -1;
    }
    LOGD("%{public}s: Enum Value: %{public}d.", __func__, intValue);
    return intValue;
}

static void ThrowBusinessError(ani_env *env, int errCode, std::string&& errMsg)
{
    LOGD("Begin ThrowBusinessError.");
    static const char *errorClsName = "L@ohos/base/BusinessError;";
    ani_class cls {};
    if (ANI_OK != env->FindClass(errorClsName, &cls)) {
        LOGE("find class BusinessError %{public}s failed", errorClsName);
        return;
    }
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ":V", &ctor)) {
        LOGE("find method BusinessError.constructor failed");
        return;
    }
    ani_object errorObject;
    if (ANI_OK != env->Object_New(cls, ctor, &errorObject)) {
        LOGE("create BusinessError object failed");
        return;
    }
    ani_double aniErrCode = static_cast<ani_double>(errCode);
    ani_string errMsgStr;
    if (ANI_OK != env->String_NewUTF8(errMsg.c_str(), errMsg.size(), &errMsgStr)) {
        LOGE("convert errMsg to ani_string failed");
        return;
    }
    if (ANI_OK != env->Object_SetFieldByName_Double(errorObject, "code", aniErrCode)) {
        LOGE("set error code failed");
        return;
    }
    if (ANI_OK != env->Object_SetPropertyByName_Ref(errorObject, "message", errMsgStr)) {
        LOGE("set error message failed");
        return;
    }
    env->ThrowError(static_cast<ani_error>(errorObject));
    return;
}

static bool IsSystemApp()
{
    uint64_t fullTokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
    return OHOS::Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(fullTokenId);
}

static int32_t GetIdFromUri(string uri)
{
    std::replace(uri.begin(), uri.end(), '/', ' ');
    std::stringstream ss(uri);
    std::string tmp;
    int fileId = -1;
    ss >> tmp >> tmp >> tmp >> fileId;
    return fileId;
}

static bool CheckValidPublicUri(const std::string &inputUri)
{
    Uri uri(inputUri);
    std::string scheme = uri.GetScheme();
    if (scheme != FILE_SCHEME) {
        return false;
    }

    std::string authority = uri.GetAuthority();
    if (authority != MEDIA_AUTHORITY && authority != FILE_MANAGER_AUTHORITY) {
        return false;
    }

    return true;
}

static std::string GetModeFromFlag(unsigned int flag)
{
    std::string mode = "";
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
    if (uri.find(MEDIA_FILE_URI_PHOTO_PREFIX) == 0) {
        isApi10 = true;
        return MediaFileTable::PHOTO_TABLE;
    } else if (uri.find(MEDIA_FILE_URI_VIDEO_PREFIX) == 0 ||
        uri.find(MEDIA_FILE_URI_IMAGE_PREFIX) == 0) {
        return MediaFileTable::PHOTO_TABLE;
    } else if (uri.find(MEDIA_FILE_URI_AUDIO_PREFIX) == 0) {
        isApi10 = true;
        return MediaFileTable::AUDIO_TABLE;
    } else if (uri.find(MEDIA_FILE_URI_AUDIO_PREFIX_L) == 0) {
        return MediaFileTable::AUDIO_TABLE;
    } else if (uri.find(MEDIA_FILE_URI_FILE_PREFIX) == 0) {
        return MediaFileTable::FILE_TABLE;
    }

    return MediaFileTable::FILE_TABLE;
}

static int InitValuesBucket(const UriPermissionInfo &uriPermInfo, Uri &uri, bool &isApi10,
    DataShareValuesBucket &valuesBucket)
{
    int32_t fileId = GetIdFromUri(uriPermInfo.uri);
    if (fileId == -1) {
        LOGE("%{public}s: get fileId parameter failed!", __func__);
        return -EINVAL;
    }

    int32_t filesType = GetMediaTypeAndApiFromUri(uri.ToString(), isApi10);
    valuesBucket.Put(PERMISSION_FILE_ID, fileId);
    valuesBucket.Put(PERMISSION_BUNDLE_NAME, uriPermInfo.bundleName);
    valuesBucket.Put(PERMISSION_MODE, uriPermInfo.mode);
    valuesBucket.Put(PERMISSION_TABLE_TYPE, filesType);
    return 0;
}

static int InsertByDatashare(const DataShareValuesBucket &valuesBucket, bool isApi10)
{
    int ret = -1;
    std::shared_ptr<DataShare::DataShareHelper> dataShareHelper = nullptr;
    sptr<FileShareGrantToken> remote = new (std::nothrow) IRemoteStub<FileShareGrantToken>();
    if (remote == nullptr) {
        LOGE("%{public}s: get remoteObject failed!", __func__);
        return -ret;
    }

    dataShareHelper = DataShare::DataShareHelper::Creator(remote->AsObject(), MEDIALIBRARY_DATA_URI);
    if (!dataShareHelper) {
        LOGE("%{public}s: connect to datashare failed!", __func__);
        delete remote;
        return ret;
    }
    string uriStr = MEDIA_GRANT_URI_PERMISSION;
    if (isApi10) {
        uriStr +=  MEDIA_API_VERSION_10;
    }

    Uri uri(uriStr);
    ret = dataShareHelper->Insert(uri, valuesBucket);
    if (ret < 0) {
        LOGE("%{public}s: insert failed with error code %{public}d!", __func__, ret);
        return ret;
    }
    return ret;
}

static int GrantInMediaLibrary(const UriPermissionInfo &uriPermInfo, Uri &uri)
{
    bool isApi10 = false;
    DataShareValuesBucket valuesBucket;
    int ret = InitValuesBucket(uriPermInfo, uri, isApi10, valuesBucket);
    if (ret < 0) {
        LOGE("%{public}s: InitValuesBucket failed!", __func__);
        return ret;
    }

    ret = InsertByDatashare(valuesBucket, isApi10);
    if (ret < 0) {
        LOGE("%{public}s: InsertByDatashare failed!", __func__);
        return ret;
    }
    return 0;
}

static int DoGrantUriPermission(const UriPermissionInfo &uriPermInfo)
{
    Uri uri(uriPermInfo.uri);
    std::string authority = uri.GetAuthority();
    std::string path = uri.GetPath();
    if (authority == MEDIA_AUTHORITY && path.find(".") == string::npos) {
        return GrantInMediaLibrary(uriPermInfo, uri);
    } else {
        auto& uriPermissionClient = OHOS::AAFwk::UriPermissionManagerClient::GetInstance();
        int ret =  uriPermissionClient.GrantUriPermission(uri, uriPermInfo.flag, uriPermInfo.bundleName);
        if (ret != 0) {
            LOGD("%{public}s: uriPermissionClient.GrantUriPermission by uri permission client failed!", __func__);
            return GrantInMediaLibrary(uriPermInfo, uri);
        }
    }

    return 0;
}

static void GrantUriPermission(ani_env *env, ani_string uri, ani_string bundleName, ani_object enumIndex)
{
    LOGD("Enter GrantUriPermission.");
    if (!IsSystemApp()) {
        LOGE("%{public}s: GrantUriPermission is not System App!", __func__);
        ThrowBusinessError(env, E_PERMISSION_SYS, "FileShare::GrantUriPermission is not System App!");
        return;
    }

    UriPermissionInfo uriPermInfo;
    std::string uriStr = ParseObjToStr(env, uri);
    if (!CheckValidPublicUri(uriStr)) {
        LOGE("%{public}s: GrantUriPermission uri is not valid!", __func__);
        ThrowBusinessError(env, EINVAL, "GrantUriPermission uri is not valid!");
        return;
    }
    uriPermInfo.uri = uriStr;
    std::string bundleNameStr = ParseObjToStr(env, bundleName);
    uriPermInfo.bundleName = bundleNameStr;

    ani_int wantConstantFlag = ParseEnumToInt(env, static_cast<ani_enum_item>(enumIndex));
    if (wantConstantFlag < 0) {
        LOGE("%{public}s: GrantUriPermission ParseEnumToInt Faild!", __func__);
        ThrowBusinessError(env, EINVAL, "GrantUriPermission is not System App!");
        return;
    }
    uriPermInfo.flag = static_cast<uint32_t>(wantConstantFlag);
    uriPermInfo.mode = GetModeFromFlag(wantConstantFlag);
    LOGD("GrantUriPermission uri: %{public}s, bundleName: %{public}s, flag: %{public}d, mode: %{public}s",
        uriStr.c_str(), bundleNameStr.c_str(), wantConstantFlag, uriPermInfo.mode.c_str());

    int ret = DoGrantUriPermission(uriPermInfo);
    if (ret < 0) {
        LOGE("FileShare::GrantUriPermission DoGrantUriPermission failed with %{public}d", ret);
        ThrowBusinessError(env, -ret, "GrantUriPermission failed");
        return;
    }
    LOGD("FileShare::GrantUriPermission DoGrantUriPermission successfully!");
}
} // namespace ModuleFileShare
} // namespace AppFileService
} // namespace OHOS

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    LOGD("Enter ANI_Constructor.");
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        LOGE("Unsupported ANI_VERSION_1");
        return ANI_ERROR;
    }

    static const char *nsName = "L@ohos/fileshare/fileShare;";
    ani_namespace ns;
    if (ANI_OK != env->FindNamespace(nsName, &ns)) {
        LOGE("Not found namespace %{public}s.", nsName);
        return ANI_NOT_FOUND;
    }
    std::array nsMethods = {
        ani_native_function {"grantUriPermissionInner", nullptr,
            reinterpret_cast<void *>(OHOS::AppFileService::ModuleFileShare::GrantUriPermission)},
    };
    if (ANI_OK != env->Namespace_BindNativeFunctions(ns, nsMethods.data(), nsMethods.size())) {
        LOGE("Cannot bind native methods to namespace %{public}s.", nsName);
        return ANI_ERROR;
    };

    *result = ANI_VERSION_1;
    return ANI_OK;
}
