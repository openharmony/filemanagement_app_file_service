/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "ext_backup_js.h"
#include "ext_backup_js_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
void ExtBackupJs::Init(const shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
                       const shared_ptr<AppExecFwk::OHOSApplication> &application,
                       shared_ptr<AppExecFwk::AbilityHandler> &handler,
                       const sptr<IRemoteObject> &token)
{
    BExtBackupJs::extBackupJs->Init(record, application, handler, token);
}

void ExtBackupJs::ExportJsContext(void)
{
    BExtBackupJs::extBackupJs->ExportJsContext();
}

ExtBackupJs *ExtBackupJs::Create(const unique_ptr<AbilityRuntime::Runtime> &runtime)
{
    return BExtBackupJs::extBackupJs->Create(runtime);
}

ErrCode ExtBackupJs::OnBackup(function<void(ErrCode, string)> callback,
    function<void(ErrCode, const string)> callbackEx)
{
    return BExtBackupJs::extBackupJs->OnBackup(callback, callbackEx);
}

ErrCode ExtBackupJs::CallJsOnBackupEx()
{
    return BExtBackupJs::extBackupJs->CallJsOnBackupEx();
}

ErrCode ExtBackupJs::CallJsOnBackup()
{
    return BExtBackupJs::extBackupJs->CallJsOnBackup();
}

ErrCode ExtBackupJs::OnRestore(function<void(ErrCode, string)> callback,
    function<void(ErrCode, const string)> callbackEx)
{
    return BExtBackupJs::extBackupJs->OnRestore(callback, callbackEx);
}

ErrCode ExtBackupJs::CallJSRestoreEx()
{
    return BExtBackupJs::extBackupJs->CallJSRestoreEx();
}

ErrCode ExtBackupJs::CallJSRestore()
{
    return BExtBackupJs::extBackupJs->CallJSRestore();
}

ErrCode ExtBackupJs::GetBackupInfo(function<void(ErrCode, const string)> callback)
{
    return BExtBackupJs::extBackupJs->GetBackupInfo(callback);
}

int ExtBackupJs::CallJsMethod(const string &funcName, AbilityRuntime::JsRuntime &jsRuntime, NativeReference *jsObj,
    InputArgsParser argParser, ResultValueParser retParser)
{
    return BExtBackupJs::extBackupJs->CallJsMethod(funcName, jsRuntime, jsObj, argParser, retParser);
}

function<bool(napi_env env, vector<napi_value> &argv)> ExtBackupJs::ParseBackupExInfo()
{
    return BExtBackupJs::extBackupJs->ParseBackupExInfo();
}

function<bool(napi_env env, vector<napi_value> &argv)> ExtBackupJs::ParseRestoreExInfo()
{
    return BExtBackupJs::extBackupJs->ParseRestoreExInfo();
}

function<bool(napi_env env, vector<napi_value> &argv)> ExtBackupJs::ParseRestoreInfo()
{
    return BExtBackupJs::extBackupJs->ParseRestoreInfo();
}

ErrCode ExtBackupJs::InvokeAppExtMethod(ErrCode errCode, const string result)
{
    return BExtBackupJs::extBackupJs->InvokeAppExtMethod(errCode, result);
}

function<bool(napi_env env, vector<napi_value> &argv)> ExtBackupJs::ParseReleaseInfo()
{
    return BExtBackupJs::extBackupJs->ParseReleaseInfo();
}

ErrCode ExtBackupJs::OnRelease(function<void(ErrCode, string)> callback, int32_t scenario)
{
    return BExtBackupJs::extBackupJs->OnRelease(callback, scenario);
}

ErrCode ExtBackupJs::GetBackupCompatibilityInfo(std::function<void(ErrCode, const std::string)> callbackEx,
    std::string extInfo)
{
    return BExtBackupJs::extBackupJs->GetBackupCompatibilityInfo(callbackEx, extInfo);
}

ErrCode ExtBackupJs::GetRestoreCompatibilityInfo(std::function<void(ErrCode, const std::string)> callbackEx,
    std::string extInfo)
{
    return BExtBackupJs::extBackupJs->GetRestoreCompatibilityInfo(callbackEx, extInfo);
}

std::function<bool(napi_env env, std::vector<napi_value> &argv)> ExtBackupJs::ParseCompatibilityInfo()
{
    return BExtBackupJs::extBackupJs->ParseCompatibilityInfo();
}
} // namespace OHOS::FileManagement::Backup
