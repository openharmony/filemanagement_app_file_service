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

#include "file_uri_entity.h"
#include "file_utils.h"
#include "log.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileUri {
using namespace std;
using namespace FileManagement;
using namespace FileManagement::LibN;

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

napi_value FileUriNExporter::GetDirectoryUri(napi_env env, napi_callback_info info)
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
    return NVal::CreateUTF8String(env, fileuriEntity->fileUri_.GetDirectoryUri()).val_;
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
    return NVal::CreateUTF8String(env, fileuriEntity->fileUri_.GetPath()).val_;
}

bool FileUriNExporter::Export()
{
    vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("toString", UriToString),
        NVal::DeclareNapiGetter("name", GetFileUriName),
        NVal::DeclareNapiGetter("path", GetFileUriPath),
        NVal::DeclareNapiFunction("getDirectoryUri", GetDirectoryUri),
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
