/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "file_uri_n_exporter.h"

#include <string>
#include <vector>

#include "file_uri_entity.h"
#include "file_utils.h"
#include "log.h"
#include "uri.h"
#include "sandbox_helper.h"

using namespace std;
namespace OHOS {
namespace AppFileService {
namespace ModuleFileUri {
using namespace FileManagement;
using namespace FileManagement::LibN;
const std::string MEDIA_AUTHORITY = "media";
napi_value FileUriNExporter::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto [succPath, path, ignore] = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succPath) {
        LOGE("Failed to get path");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = CreateUniquePtr<FileUriEntity>(string(path.get()));
    if (fileuriEntity == nullptr) {
        LOGE("Failed to request heap memory.");
        NError(ENOMEM).ThrowErr(env);
        return nullptr;
    }
    if (!NClass::SetEntityFor<FileUriEntity>(env, funcArg.GetThisVar(), move(fileuriEntity))) {
        LOGE("Failed to set file entity");
        NError(EIO).ThrowErr(env);
        return nullptr;
    }
    return funcArg.GetThisVar();
}

napi_value FileUriNExporter::UriToString(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NVal::CreateUTF8String(env, fileuriEntity->fileUri_.ToString()).val_;
}

napi_value FileUriNExporter::GetFileUriName(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NVal::CreateUTF8String(env, fileuriEntity->fileUri_.GetName()).val_;
}

napi_value FileUriNExporter::GetFullDirectoryUri(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    string uri = fileuriEntity->fileUri_.GetFullDirectoryUri();
    if (uri == "") {
        LOGE("No such file or directory!");
        NError(ENOENT).ThrowErr(env);
    }
    return NVal::CreateUTF8String(env, uri).val_;
}

napi_value FileUriNExporter::IsRemoteUri(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    bool isRemoteUri = fileuriEntity->fileUri_.IsRemoteUri();
    return NVal::CreateBool(env, isRemoteUri).val_;
}

napi_value FileUriNExporter::GetFileUriPath(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NVal::CreateUTF8String(env, fileuriEntity->fileUri_.GetRealPath()).val_;
}

static std::string Split(const std::string &path, Uri &uri)
{
    std::string normalizeUri = "";
    if (!uri.GetScheme().empty()) {
        normalizeUri += uri.GetScheme() + ":";
    }
    if (uri.GetPath().empty()) {
        normalizeUri += uri.GetSchemeSpecificPart();
    } else {
        if (!uri.GetHost().empty()) {
            normalizeUri += "//";
            if (!uri.GetUserInfo().empty()) {
                normalizeUri += uri.GetUserInfo() + "@";
            }
            normalizeUri += uri.GetHost();
            if (uri.GetPort() != -1) {
                normalizeUri += ":" + std::to_string(uri.GetPort());
            }
        } else if (!uri.GetAuthority().empty()) {
            normalizeUri += "//" + uri.GetAuthority();
        }
        normalizeUri += path;
    }
    if (!uri.GetQuery().empty()) {
        normalizeUri += "?" + uri.GetQuery();
    }
    if (!uri.GetFragment().empty()) {
        normalizeUri += "#" + uri.GetFragment();
    }
    return normalizeUri;
}

static std::string NormalizeUri(Uri &uri)
{
    std::vector<std::string> temp;
    size_t pathLen = uri.GetPath().size();
    if (pathLen == 0) {
        return uri.ToString();
    }
    size_t pos = 0;
    size_t left = 0;
    while ((pos = uri.GetPath().find('/', left)) != std::string::npos) {
        temp.push_back(uri.GetPath().substr(left, pos - left));
        left = pos + 1;
    }
    if (left != pathLen) {
        temp.push_back(uri.GetPath().substr(left));
    }
    size_t tempLen = temp.size();
    std::vector<std::string> normalizeTemp;
    for (size_t i = 0; i < tempLen; ++i) {
        if (!temp[i].empty() && !(temp[i] == ".") && !(temp[i] == "..")) {
            normalizeTemp.push_back(temp[i]);
        }
        if (temp[i] == "..") {
            if (!normalizeTemp.empty() && normalizeTemp.back() != "..") {
                normalizeTemp.pop_back();
            } else {
                normalizeTemp.push_back(temp[i]);
            }
        }
    }
    std::string normalizePath = "";
    tempLen = normalizeTemp.size();
    if (tempLen == 0) {
        normalizePath = "/";
    } else {
        for (size_t i = 0; i < tempLen; ++i) {
            normalizePath += "/" + normalizeTemp[i];
        }
    }
    return Split(normalizePath, uri);
}

napi_value FileUriNExporter::Normalize(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    napi_value uriObj = NClass::InstantiateClass(env, FileUriNExporter::className,
        {NVal::CreateUTF8String(env, NormalizeUri(fileuriEntity->fileUri_.uri_)).val_});
    if (!uriObj) {
        LOGE("Failed to construct FileUriNExporter.");
        NError(E_UNKNOWN_ERROR).ThrowErr(env);
        return nullptr;
    }

    return uriObj;
}

napi_value FileUriNExporter::Equals(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto thisEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!thisEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    auto otherEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg[NARG_POS::FIRST]);
    if (!otherEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateBool(env, thisEntity->fileUri_.uri_.Equals(otherEntity->fileUri_.uri_)).val_;
}

napi_value FileUriNExporter::EqualsTo(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    auto thisEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!thisEntity) {
        LOGE("Failed to get file entity");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }

    auto otherEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg[NARG_POS::FIRST]);
    if (!otherEntity) {
        LOGE("Failed to get file entity");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    return NVal::CreateBool(env, thisEntity->fileUri_.uri_.Equals(otherEntity->fileUri_.uri_)).val_;
}

napi_value FileUriNExporter::IsAbsolute(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NVal::CreateBool(env, fileuriEntity->fileUri_.uri_.IsAbsolute()).val_;
}

napi_value FileUriNExporter::GetScheme(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateUTF8String(env, fileuriEntity->fileUri_.uri_.GetScheme()).val_;
}

napi_value FileUriNExporter::GetAuthority(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateUTF8String(env, fileuriEntity->fileUri_.uri_.GetAuthority()).val_;
}

napi_value FileUriNExporter::GetSsp(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateUTF8String(env, fileuriEntity->fileUri_.uri_.GetSchemeSpecificPart()).val_;
}

napi_value FileUriNExporter::GetUserInfo(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateUTF8String(env, fileuriEntity->fileUri_.uri_.GetUserInfo()).val_;
}

napi_value FileUriNExporter::GetHost(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateUTF8String(env, fileuriEntity->fileUri_.uri_.GetHost()).val_;
}

napi_value FileUriNExporter::GetPort(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateUTF8String(env, to_string(fileuriEntity->fileUri_.uri_.GetPort())).val_;
}

napi_value FileUriNExporter::GetQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateUTF8String(env, fileuriEntity->fileUri_.uri_.GetQuery()).val_;
}

napi_value FileUriNExporter::GetFragment(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto fileuriEntity = NClass::GetEntityOf<FileUriEntity>(env, funcArg.GetThisVar());
    if (!fileuriEntity) {
        LOGE("Failed to get file entity");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateUTF8String(env, fileuriEntity->fileUri_.uri_.GetFragment()).val_;
}

bool FileUriNExporter::Export()
{
    vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("toString", UriToString),
        NVal::DeclareNapiGetter("name", GetFileUriName),
        NVal::DeclareNapiGetter("path", GetFileUriPath),
        NVal::DeclareNapiFunction("getFullDirectoryUri", GetFullDirectoryUri),
        NVal::DeclareNapiFunction("isRemoteUri", IsRemoteUri),
        NVal::DeclareNapiFunction("normalize", Normalize),
        NVal::DeclareNapiFunction("equals", Equals),
        NVal::DeclareNapiFunction("equalsTo", EqualsTo),
        NVal::DeclareNapiFunction("checkIsAbsolute", IsAbsolute),
        NVal::DeclareNapiGetter("scheme", GetScheme),
        NVal::DeclareNapiGetter("authority", GetAuthority),
        NVal::DeclareNapiGetter("ssp", GetSsp),
        NVal::DeclareNapiGetter("userInfo", GetUserInfo),
        NVal::DeclareNapiGetter("host", GetHost),
        NVal::DeclareNapiGetter("port", GetPort),
        NVal::DeclareNapiGetter("query", GetQuery),
        NVal::DeclareNapiGetter("fragment", GetFragment),
    };

    auto [succ, classValue] = NClass::DefineClass(exports_.env_, className, Constructor, std::move(props));
    if (!succ) {
        LOGE("Failed to define class");
        NError(EIO).ThrowErr(exports_.env_);
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, classValue);
    if (!succ) {
        LOGE("Failed to save class");
        NError(EIO).ThrowErr(exports_.env_);
        return false;
    }

    return exports_.AddProp(className, classValue);
}

string FileUriNExporter::GetClassName()
{
    return FileUriNExporter::className;
}

FileUriNExporter::FileUriNExporter(napi_env env, napi_value exports) : NExporter(env, exports) {}

FileUriNExporter::~FileUriNExporter() {}
} // namespace ModuleFileUri
} // namespace AppFileService
} // namespace OHOS
