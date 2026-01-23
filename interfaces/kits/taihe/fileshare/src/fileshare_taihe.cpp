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

#include "fileshare_taihe.h"
#include <iostream>
#include "ability.h"
#include "accesstoken_kit.h"
#include "ani.h"
#include "datashare_helper.h"
#include "datashare_values_bucket.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "oh_file_share.h"
#include "remote_uri.h"
#include "tokenid_kit.h"
#include "uri_permission_manager_client.h"
#include "want.h"
#include "n_error.h"

namespace ANI::FileShare {
constexpr int32_t E_PERMISSION_DENIED = -1;
constexpr uint32_t READ_MODE = 0x01;
constexpr uint32_t WRITE_MODE = 0x02;
constexpr uint32_t CREATE_MODE = 0x04;
constexpr uint32_t DELETE_MODE = 0x08;
constexpr uint32_t RENAME_MODE = 0x10;
constexpr uint32_t ALL_VALID_MODES = READ_MODE | WRITE_MODE | CREATE_MODE | DELETE_MODE | RENAME_MODE;
const std::string MEDIA_FILE_URI_PHOTO_PREFEX = "file://media/Photo/";
const std::string MEDIA_FILE_URI_AUDIO_PREFEX = "file://media/Audio/";
const std::string MEDIA_FILE_URI_VIDEO_PREFEX = "file://media/video/";
const std::string MEDIA_FILE_URI_IMAGE_PREFEX = "file://media/image/";
const std::string MEDIA_FILE_URI_FILE_PREFEX = "file://media/file/";
const std::string MEDIA_FILE_URI_AUDIO_LOW_PREFEX = "file://media/audio/";
const std::string PERMISSION_BUNDLE_NAME = "bundle_name";
const std::string PERMISSION_FILE_ID = "file_id";
const std::string PERMISSION_MODE = "mode";
const std::string PERMISSION_TABLE_TYPE = "table_type";
const std::string MEDIALIBRARY_DATA_URI = "datashare:///media";
const std::string MEDIA_GRANT_URI_PERMISSION =
    "datashare:///media/bundle_permission_insert_operation/bundle_permission_insert_operation";
const std::string MEDIA_API_VERSION_10 = "?api_version=10";
const std::string MEDIA_AUTHORITY = "media";
const std::string FILE_SCHEME = "file";
const std::string FILE_MANAGER_AUTHORITY = "docs";
const std::string FILE_NOPS = "";
const std::string FILE_READ = "r";
const std::string FILE_WRITE = "w";

static int32_t ErrorCodeConversion(int32_t errorCode)
{
    int32_t errCode = ERR_UNKNOWN;
    switch (errorCode) {
        case static_cast<int32_t>(ERR_OK):
            errCode = ERR_OK;
            break;
        case static_cast<int32_t>(ERR_PERMISSION_ERROR):
            errCode = ERR_PERMISSION_ERROR;
            break;
        case static_cast<int32_t>(ERR_PARAMS):
            errCode = ERR_PARAMS;
            break;
        case static_cast<int32_t>(ERR_DEVICE_NOT_SUPPORTED):
            errCode = ERR_DEVICE_NOT_SUPPORTED;
            break;
        case EPERM:
            errCode = ERR_EPERM;
            break;
        default:
            break;
    }
    return errCode;
}

static std::string GetErrorMessage(int32_t errorCode)
{
    switch (errorCode) {
        case ERR_DEVICE_NOT_SUPPORTED:
            return "The device doesn't support this api";
        case ERR_EPERM:
            return "Operation not permitted";
        case ERR_PERMISSION_ERROR:
            return "Permission verification failed";
        case ERR_PARAMS:
            return "Parameter error";
        default:
            return "Unknown error";
    }
}

static void GetErrorCodeConversion(int32_t errCode)
{
    if (errCode == OHOS::FileManagement::LibN::ERRNO_NOERR) {
        return;
    }
    int32_t code = 0;
    std::string msg;
    auto it = OHOS::FileManagement::LibN::errCodeTable.find(errCode);
    if (it != OHOS::FileManagement::LibN::errCodeTable.end()) {
        code = it->second.first;
        msg = it->second.second;
    } else {
        code = OHOS::FileManagement::LibN::errCodeTable.at(OHOS::FileManagement::LibN::UNKROWN_ERR).first;
        msg = OHOS::FileManagement::LibN::errCodeTable.at(OHOS::FileManagement::LibN::UNKROWN_ERR).second;
    }
    taihe::set_business_error(code, msg);
}

ohos::fileshare::fileShare::PolicyInfo MakePolicyInfo(taihe::string_view uri, int32_t operationMode)
{
    return {uri, operationMode};
}

ohos::fileshare::fileShare::PathPolicyInfo MakePathPolicyInfo(taihe::string_view path,
    ohos::fileshare::fileShare::OperationMode operationMode)
{
    return {path, operationMode};
}

static int32_t GetUriPoliciesArg(taihe::array_view<ohos::fileshare::fileShare::PolicyInfo> policies,
    std::vector<OHOS::AppFileService::UriPolicyInfo> &uriPolicies)
{
    uint32_t count = policies.size();
    if (count > OHOS::AppFileService::MAX_ARRAY_SIZE) {
        LOGE("The length of the array is extra-long");
        return OHOS::FileManagement::LibN::E_PARAMS;
    }
    for (uint32_t i = 0; i < count; i++) {
        OHOS::AppFileService::UriPolicyInfo uriPolicy;
        uriPolicy.uri = policies[i].uri;
        uriPolicy.mode = static_cast<unsigned int>(policies[i].operationMode);
        if (uriPolicy.uri == FILE_NOPS) {
            LOGE("URI is empty");
            return OHOS::FileManagement::LibN::E_PARAMS;
        }
        if ((uriPolicy.mode & ALL_VALID_MODES) != uriPolicy.mode) {
            LOGE("Invalid operation mode");
            return OHOS::FileManagement::LibN::E_PARAMS;
        }
        uriPolicies.emplace_back(uriPolicy);
    }
    return OHOS::FileManagement::LibN::ERRNO_NOERR;
}

static int32_t GetPathPoliciesArg(taihe::array_view<ohos::fileshare::fileShare::PathPolicyInfo> policies,
    std::vector<OHOS::AppFileService::PathPolicyInfo> &pathPolicies)
{
    uint32_t count = policies.size();
    if (count > OHOS::AppFileService::MAX_ARRAY_SIZE) {
        LOGE("The length of the array is extra-long");
        return OHOS::FileManagement::LibN::E_PARAMS;
    }
    for (uint32_t i = 0; i < count; i++) {
        OHOS::AppFileService::PathPolicyInfo pathPolicy;
        pathPolicy.path = policies[i].path;
        pathPolicy.mode = static_cast<unsigned int>(policies[i].operationMode);
        if (pathPolicy.path == FILE_NOPS) {
            LOGE("path is empty");
            return OHOS::FileManagement::LibN::E_PARAMS;
        }
        if ((pathPolicy.mode & ALL_VALID_MODES) != pathPolicy.mode) {
            LOGE("Invalid operation mode");
            return OHOS::FileManagement::LibN::E_PARAMS;
        }
        pathPolicies.emplace_back(pathPolicy);
    }
    return OHOS::FileManagement::LibN::ERRNO_NOERR;
}

void ActivatePermissionSync(taihe::array_view<ohos::fileshare::fileShare::PolicyInfo> policies)
{
    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(policies, uriPolicies)) {
        LOGE("Failed to get URI policies");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_PARAMS),
            GetErrorMessage(OHOS::FileManagement::LibN::E_PARAMS));
        return;
    }

    std::shared_ptr<PolicyErrorArgs> arg = std::make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("PolicyErrorArgs make make_shared failed");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_UNKNOWN_ERROR),
                                  GetErrorMessage(OHOS::FileManagement::LibN::E_UNKNOWN_ERROR));
        return;
    }

    auto errCode = OHOS::AppFileService::FilePermission::ActivatePermission(uriPolicies, arg->errorResults);
    arg->errNo = ErrorCodeConversion(errCode);
    if (arg->errNo) {
        LOGE("Activation failed");
        taihe::set_business_error(arg->errNo, GetErrorMessage(arg->errNo));
    }
}

void DeactivatePermissionSync(taihe::array_view<ohos::fileshare::fileShare::PolicyInfo> policies)
{
    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(policies, uriPolicies)) {
        LOGE("Failed to get URI policies");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_PARAMS),
            GetErrorMessage(OHOS::FileManagement::LibN::E_PARAMS));
        return;
    }

    std::shared_ptr<PolicyErrorArgs> arg = std::make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("PolicyErrorArgs make make_shared failed");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_UNKNOWN_ERROR),
                                  GetErrorMessage(OHOS::FileManagement::LibN::E_UNKNOWN_ERROR));
        return;
    }

    auto errCode = OHOS::AppFileService::FilePermission::DeactivatePermission(uriPolicies, arg->errorResults);
    arg->errNo = ErrorCodeConversion(errCode);
    if (arg->errNo) {
        LOGE("Deactivation failed");
        taihe::set_business_error(arg->errNo, GetErrorMessage(arg->errNo));
        return;
    }
}

static bool IsSystemApp()
{
    uint64_t fullTokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
    return OHOS::Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(fullTokenId);
}

static bool CheckTokenIdPermission(uint32_t tokenCaller, const std::string &permission)
{
    return OHOS::Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission) ==
           OHOS::Security::AccessToken::PermissionState::PERMISSION_GRANTED;
}

static int32_t GetIdFromUri(std::string uri)
{
    std::replace(uri.begin(), uri.end(), '/', ' ');
    std::stringstream ss(uri);
    std::string tmp;
    int32_t fileId = E_PERMISSION_DENIED;
    ss >> tmp >> tmp >> tmp >> fileId;
    return fileId;
}

static std::string GetModeFromFlag(uint32_t flag)
{
    std::string mode = FILE_NOPS;
    if (flag & OHOS::AAFwk::Want::FLAG_AUTH_READ_URI_PERMISSION) {
        mode += FILE_READ;
    }
    if (flag & OHOS::AAFwk::Want::FLAG_AUTH_WRITE_URI_PERMISSION) {
        mode += FILE_WRITE;
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
    } else if (uri.find(MEDIA_FILE_URI_AUDIO_LOW_PREFEX) == 0) {
        return MediaFileTable::AUDIO_TABLE;
    } else if (uri.find(MEDIA_FILE_URI_FILE_PREFEX) == 0) {
        return MediaFileTable::FILE_TABLE;
    }

    return MediaFileTable::FILE_TABLE;
}

static int32_t InitValuesBucket(const UriPermissionInfo &uriPermInfo, Uri &uri, bool &isApi10,
    OHOS::DataShare::DataShareValuesBucket &valuesBucket)
{
    int32_t fileId = GetIdFromUri(uriPermInfo.uri);
    if (fileId == E_PERMISSION_DENIED) {
        LOGE("FileShare::InitValuesBucket get fileId parameter failed!");
        return -EINVAL;
    }

    int32_t filesType = GetMediaTypeAndApiFromUri(uri.ToString(), isApi10);
    valuesBucket.Put(PERMISSION_FILE_ID, fileId);
    valuesBucket.Put(PERMISSION_BUNDLE_NAME, uriPermInfo.bundleName);
    valuesBucket.Put(PERMISSION_MODE, uriPermInfo.mode);
    valuesBucket.Put(PERMISSION_TABLE_TYPE, filesType);
    return OHOS::FileManagement::LibN::ERRNO_NOERR;
}

static int32_t InsertByDatashare(const OHOS::DataShare::DataShareValuesBucket &valuesBucket, bool isApi10)
{
    int32_t ret = E_PERMISSION_DENIED;
    std::shared_ptr<OHOS::DataShare::DataShareHelper> dataShareHelper = nullptr;
    OHOS::sptr<FileShareGrantToken> remote = new OHOS::IRemoteStub<FileShareGrantToken>();
    if (remote == nullptr) {
        LOGE("FileShare::InsertByDatashare get remoteObject failed!");
        return -ENOMEM;
    }

    dataShareHelper = OHOS::DataShare::DataShareHelper::Creator(remote->AsObject(), MEDIALIBRARY_DATA_URI);
    if (!dataShareHelper) {
        LOGE("FileShare::InsertByDatashare connect to datashare failed!");
        return -OHOS::FileManagement::LibN::E_PERMISSION;
    }
    std::string uriStr = MEDIA_GRANT_URI_PERMISSION;
    if (isApi10) {
        uriStr +=  MEDIA_API_VERSION_10;
    }

    OHOS::Uri uri(uriStr);
    ret = dataShareHelper->Insert(uri, valuesBucket);
    if (ret < 0) {
        LOGE("FileShare::InsertByDatashare insert failed with error code %{public}d!", ret);
        return ret;
    }
    return ret;
}

static int32_t GrantInMediaLibrary(const UriPermissionInfo &uriPermInfo, Uri &uri)
{
    bool isApi10 = false;
    OHOS::DataShare::DataShareValuesBucket valuesBucket;
    int32_t ret = InitValuesBucket(uriPermInfo, uri, isApi10, valuesBucket);
    if (ret < 0) {
        return ret;
    }

    ret = InsertByDatashare(valuesBucket, isApi10);
    if (ret < 0) {
        return ret;
    }
    return OHOS::FileManagement::LibN::ERRNO_NOERR;
}

static int32_t DoGrantUriPermission(const UriPermissionInfo &uriPermInfo)
{
    Uri uri(uriPermInfo.uri);
    std::string authority = uri.GetAuthority();
    std::string path = uri.GetPath();
    if (authority == MEDIA_AUTHORITY && path.find(".") == std::string::npos) {
        return GrantInMediaLibrary(uriPermInfo, uri);
    } else {
        auto& uriPermissionClient = OHOS::AAFwk::UriPermissionManagerClient::GetInstance();
        int32_t ret = uriPermissionClient.GrantUriPermission(uri, uriPermInfo.flag,
            uriPermInfo.bundleName);
        if (ret != OHOS::FileManagement::LibN::ERRNO_NOERR) {
            LOGD("uriPermissionClient.GrantUriPermission by uri permission client failed!");
            return GrantInMediaLibrary(uriPermInfo, uri);
        }
    }
    return OHOS::FileManagement::LibN::ERRNO_NOERR;
}

void GrantUriPermissionSync(taihe::string_view uri, taihe::string_view bundleName, uintptr_t flag)
{
    LOGD("fileShare::GrantUriPermission begin!");
    if (!IsSystemApp()) {
        LOGE("fileShare::GrantUriPermission is not System App!");
        taihe::set_business_error(OHOS::FileManagement::LibN::E_PERMISSION_SYS,
            "fileShare::GrantUriPermissionSync is not System App!");
        return;
    }

    std::string uri_ = std::string(uri);
    OHOS::Uri inputUri(uri_);
    std::string scheme = inputUri.GetScheme();
    if (scheme != FILE_SCHEME) {
        return;
    }
    std::string authority = inputUri.GetAuthority();
    if (authority != MEDIA_AUTHORITY && authority != FILE_MANAGER_AUTHORITY) {
        return;
    }

    ani_env *env = taihe::get_env();
    ani_enum_item enumItem = reinterpret_cast<ani_enum_item>(flag);
    ani_int result;
    env->EnumItem_GetValue_Int(enumItem, &result);

    UriPermissionInfo uriPermInfo;
    uriPermInfo.uri = uri;
    uriPermInfo.bundleName = bundleName;
    uriPermInfo.flag = static_cast<unsigned int>(result);
    uriPermInfo.mode = GetModeFromFlag((int32_t)result);

    int ret = DoGrantUriPermission(uriPermInfo);
    if (ret < 0) {
        LOGE("fileShare::GrantUriPermission DoGrantUriPermission failed with %{public}d", ret);
        taihe::set_business_error(ErrorCodeConversion(-ret), GetErrorMessage(-ret));
    }
}

taihe::array<bool> CheckPathPermissionSync(int32_t tokenID,
    taihe::array_view<ohos::fileshare::fileShare::PathPolicyInfo> policies,
    ohos::fileshare::fileShare::PolicyType policyType)
{
    if (!IsSystemApp()) {
        LOGE("fileShare::CheckPathPermissionSync is not System App!");
        taihe::set_business_error(OHOS::FileManagement::LibN::E_PERMISSION_SYS,
            "fileShare::CheckPathPermissionSync is not System App!");
        return taihe::array<bool>::make(0);
    }

    int32_t callerTokenId = static_cast<int32_t>(OHOS::IPCSkeleton::GetCallingTokenID());
    if (tokenID != callerTokenId) {
        if (!CheckTokenIdPermission(callerTokenId, "ohos.permission.CHECK_SANDBOX_POLICY")) {
            taihe::set_business_error(OHOS::FileManagement::LibN::E_PERMISSION,
                "fileShare::CheckPathPermissionSync checkPermission failed!");
            return taihe::array<bool>::make(0);
        }
    }

    std::vector<OHOS::AppFileService::PathPolicyInfo> pathPolicies;
    if (GetPathPoliciesArg(policies, pathPolicies)) {
        LOGE("Failed to get pathPolicies.");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_PARAMS),
                                  GetErrorMessage(OHOS::FileManagement::LibN::E_PARAMS));
        return taihe::array<bool>::make(0);
    }

    std::shared_ptr<PolicyInfoResultArgs> arg = std::make_shared<PolicyInfoResultArgs>();
    if (arg == nullptr) {
        LOGE("PolicyInfoResultArgs make make_shared failed.");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_UNKNOWN_ERROR),
                                  GetErrorMessage(OHOS::FileManagement::LibN::E_UNKNOWN_ERROR));
        return taihe::array<bool>::make(0);
    }

    arg->errNo = OHOS::AppFileService::FilePermission::CheckPathPermission(tokenID,
        pathPolicies, policyType, arg->resultData);
    if (arg->errNo) {
        LOGE("Activation failed.");
        taihe::set_business_error(arg->errNo, GetErrorMessage(arg->errNo));
        return taihe::array<bool>::make(0);
    }
    taihe::array<bool> result(taihe::copy_data_t{}, arg->resultData.begin(), arg->resultData.size());
    return result;
}

void GrantDecUriPermissionSync(taihe::array_view<ohos::fileshare::fileShare::PolicyInfo> policies,
    taihe::string_view targetBundleName, int32_t appCloneIndex)
{
    if (!IsSystemApp()) {
        LOGE("fileShare::GrantDecUriPermissionSync is not System App!");
        taihe::set_business_error(OHOS::FileManagement::LibN::E_PERMISSION_SYS,
            "fileShare::GrantDecUriPermissionSync is not System App!");
        return;
    }

    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(policies, uriPolicies)) {
        LOGE("Failed to get uriPolicies");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_PERM),
                                  GetErrorMessage(OHOS::FileManagement::LibN::E_PERM));
        return;
    }

    std::shared_ptr<PolicyErrorArgs> arg = std::make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("PolicyErrorArgs make make_shared failed");
        taihe::set_business_error(OHOS::FileManagement::LibN::E_NOMEM,
            "fileShare::GrantDecUriPermissionSync Out of memory!");
        return;
    }

    std::string targetBundleName_ = std::string(targetBundleName);

    auto errCode = OHOS::AppFileService::FilePermission::GrantPermission(uriPolicies,
        targetBundleName_, appCloneIndex, arg->errorResults);
    arg->errNo = ErrorCodeConversion(errCode);
    if (arg->errNo) {
        LOGE("GrantDecUriPermission failed");
        taihe::set_business_error(arg->errNo, GetErrorMessage(arg->errNo));
    }
}

::taihe::array<bool> CheckPersistentPermissionSync(
    ::taihe::array_view<::ohos::fileshare::fileShare::PolicyInfo> policies)
{
    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(policies, uriPolicies)) {
        LOGE("Failed to get uriPolicies.");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_PARAMS),
            GetErrorMessage(OHOS::FileManagement::LibN::E_PARAMS));
        return taihe::array<bool>::make(0);
    }
    std::shared_ptr<PolicyInfoResultArgs> arg = std::make_shared<PolicyInfoResultArgs>();
    if (arg == nullptr) {
        LOGE("PolicyInfoResultArgs make make_shared failed.");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_UNKNOWN_ERROR),
            GetErrorMessage(OHOS::FileManagement::LibN::E_UNKNOWN_ERROR));
        return taihe::array<bool>::make(0);
    }
    arg->errNo = OHOS::AppFileService::FilePermission::CheckPersistentPermission(uriPolicies, arg->resultData);
    if (arg->errNo) {
        LOGE("CheckPersistentPermission failed.");
        taihe::set_business_error(arg->errNo, GetErrorMessage(arg->errNo));
        return taihe::array<bool>::make(0);
    }
    return taihe::array<bool>(taihe::copy_data_t{}, arg->resultData.begin(), arg->resultData.size());
}

void RevokePermissionSync(::taihe::array_view<::ohos::fileshare::fileShare::PolicyInfo> policies)
{
    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(policies, uriPolicies)) {
        LOGE("Failed to get uriPolicies.");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_PARAMS),
            GetErrorMessage(OHOS::FileManagement::LibN::E_PARAMS));
        return;
    }
    std::shared_ptr<PolicyErrorArgs> arg = std::make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("PolicyErrorArgs make make_shared failed.");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_UNKNOWN_ERROR),
            GetErrorMessage(OHOS::FileManagement::LibN::E_UNKNOWN_ERROR));
        return;
    }
    arg->errNo = OHOS::AppFileService::FilePermission::RevokePermission(uriPolicies, arg->errorResults);
    if (arg->errNo) {
        LOGE("RevokePermission failed.");
        GetErrorCodeConversion(arg->errNo);
        return;
    }
}

void PersistPermissionSync(::taihe::array_view<::ohos::fileshare::fileShare::PolicyInfo> policies)
{
    std::vector<OHOS::AppFileService::UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(policies, uriPolicies)) {
        LOGE("Failed to get uriPolicies.");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_PARAMS),
            GetErrorMessage(OHOS::FileManagement::LibN::E_PARAMS));
        return;
    }
    std::shared_ptr<PolicyErrorArgs> arg = std::make_shared<PolicyErrorArgs>();
    if (arg == nullptr) {
        LOGE("PolicyErrorArgs make make_shared failed.");
        taihe::set_business_error(ErrorCodeConversion(OHOS::FileManagement::LibN::E_UNKNOWN_ERROR),
            GetErrorMessage(OHOS::FileManagement::LibN::E_UNKNOWN_ERROR));
        return;
    }
    arg->errNo = OHOS::AppFileService::FilePermission::PersistPermission(uriPolicies, arg->errorResults);
    if (arg->errNo) {
        LOGE("PersistPermission failed.");
        GetErrorCodeConversion(arg->errNo);
        return;
    }
}
} // namespace

// NOLINTBEGIN
TH_EXPORT_CPP_API_MakePolicyInfo(ANI::FileShare::MakePolicyInfo);
TH_EXPORT_CPP_API_MakePathPolicyInfo(ANI::FileShare::MakePathPolicyInfo);
TH_EXPORT_CPP_API_ActivatePermissionSync(ANI::FileShare::ActivatePermissionSync);
TH_EXPORT_CPP_API_DeactivatePermissionSync(ANI::FileShare::DeactivatePermissionSync);
TH_EXPORT_CPP_API_GrantUriPermissionSync(ANI::FileShare::GrantUriPermissionSync);
TH_EXPORT_CPP_API_CheckPathPermissionSync(ANI::FileShare::CheckPathPermissionSync);
TH_EXPORT_CPP_API_GrantDecUriPermissionSync(ANI::FileShare::GrantDecUriPermissionSync);
TH_EXPORT_CPP_API_CheckPersistentPermissionSync(ANI::FileShare::CheckPersistentPermissionSync);
TH_EXPORT_CPP_API_RevokePermissionSync(ANI::FileShare::RevokePermissionSync);
TH_EXPORT_CPP_API_PersistPermissionSync(ANI::FileShare::PersistPermissionSync);
 // NOLINTEND
