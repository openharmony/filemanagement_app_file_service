/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_EXT_BACKUP_JS_MOCK_H
#define OHOS_FILEMGMT_BACKUP_EXT_BACKUP_JS_MOCK_H

#include <gmock/gmock.h>

#include "ability_info.h"
#include "ext_backup_js.h"

namespace OHOS::FileManagement::Backup {
class BExtBackupJs {
public:
    BExtBackupJs() = default;
    virtual ~BExtBackupJs() = default;
public:
    virtual void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &,
              const std::shared_ptr<AppExecFwk::OHOSApplication> &,
              std::shared_ptr<AppExecFwk::AbilityHandler> &,
              const sptr<IRemoteObject> &) = 0;
    virtual ExtBackupJs *Create(const std::unique_ptr<AbilityRuntime::Runtime> &) = 0;
    virtual ErrCode OnBackup(std::function<void(ErrCode, std::string)>,
        std::function<void(ErrCode, const std::string)>) = 0;
    virtual ErrCode OnRestore(std::function<void(ErrCode, std::string)>,
        std::function<void(ErrCode, const std::string)>) = 0;
    virtual ErrCode GetBackupInfo(std::function<void(ErrCode, const std::string)>);
    virtual ErrCode InvokeAppExtMethod(ErrCode, const std::string) = 0;
    virtual int CallJsMethod(const std::string &, AbilityRuntime::JsRuntime &, NativeReference *, InputArgsParser,
        ResultValueParser) = 0;
    virtual std::tuple<ErrCode, napi_value> CallObjectMethod(std::string_view, const std::vector<napi_value> &) = 0;
    virtual std::function<bool(napi_env env, std::vector<napi_value> &argv)> ParseRestoreExInfo() = 0;
    virtual std::function<bool(napi_env env, std::vector<napi_value> &argv)> ParseRestoreInfo() = 0;
    virtual std::function<bool(napi_env env, std::vector<napi_value> &argv)> ParseBackupExInfo() = 0;
    virtual std::function<bool(napi_env env, std::vector<napi_value> &argv)> ParseBackupInfo() = 0;
    virtual ErrCode CallJSRestoreEx() = 0;
    virtual ErrCode CallJSRestore() = 0;
    virtual ErrCode CallJsOnBackupEx() = 0;
    virtual ErrCode CallJsOnBackup() = 0;
    virtual void ExportJsContext() = 0;
    virtual ErrCode OnRelease(std::function<void(ErrCode, std::string)>, int32_t scenario) = 0;
    virtual std::function<bool(napi_env, std::vector<napi_value> &argv) ParseReleaseInfo() = 0;
    virtual ErrCode GetBackupCompatibilityInfo(std::function<void(ErrCode, const std::string)> , std::string) = 0;
    virtual ErrCode GetRestoreCompatibilityInfo(std::function<void(ErrCode, const std::string)> , std::string) = 0;
    virtual std::function<bool(napi_env, std::vector<napi_value> &argv) ParseCompatibilityInfo() = 0;
public:
    virtual bool GetProfileFromAbility(const OHOS::AppExecFwk::AbilityInfo &, const std::string &,
        std::vector<std::string> &) const = 0;
public:
    static inline std::shared_ptr<BExtBackupJs> extBackupJs = nullptr;
};

class ExtBackupJsMock : public BExtBackupJs {
public:
    MOCK_METHOD(void, Init, (const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &,
              const std::shared_ptr<AppExecFwk::OHOSApplication> &,
              std::shared_ptr<AppExecFwk::AbilityHandler> &,
              const sptr<IRemoteObject> &));
    MOCK_METHOD(ExtBackupJs *, Create, (const std::unique_ptr<AbilityRuntime::Runtime> &));
    MOCK_METHOD(ErrCode, OnBackup, (std::function<void(ErrCode, std::string)>,
        std::function<void(ErrCode, const std::string)>));
    MOCK_METHOD(ErrCode, OnRestore, (std::function<void(ErrCode, std::string)>,
        std::function<void(ErrCode, const std::string)>));
    MOCK_METHOD(ErrCode, GetBackupInfo, (std::function<void(ErrCode, const std::string)>));
    MOCK_METHOD(ErrCode, InvokeAppExtMethod, (ErrCode, const std::string));
    MOCK_METHOD(int, CallJsMethod, (const std::string&, AbilityRuntime::JsRuntime&, NativeReference*, InputArgsParser,
        ResultValueParser));
    MOCK_METHOD((std::tuple<ErrCode, napi_value>), CallObjectMethod,
        (std::string_view, const std::vector<napi_value> &));
    MOCK_METHOD((std::function<bool(napi_env env, std::vector<napi_value> &argv)>), ParseRestoreExInfo, ());
    MOCK_METHOD((std::function<bool(napi_env env, std::vector<napi_value> &argv)>), ParseRestoreInfo, ());
    MOCK_METHOD((std::function<bool(napi_env env, std::vector<napi_value> &argv)>), ParseBackupExInfo, ());
    MOCK_METHOD((std::function<bool(napi_env env, std::vector<napi_value> &argv)>), ParseBackupInfo, ());
    MOCK_METHOD(ErrCode, CallJSRestoreEx, ());
    MOCK_METHOD(ErrCode, CallJSRestore, ());
    MOCK_METHOD(ErrCode, CallJsOnBackupEx, ());
    MOCK_METHOD(ErrCode, CallJsOnBackup, ());
    MOCK_METHOD(void, ExportJsContext, ());
public:
    MOCK_METHOD(bool, GetProfileFromAbility, (const OHOS::AppExecFwk::AbilityInfo&, const std::string&,
        std::vector<std::string>&), (const));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_EXT_BACKUP_JS_MOCK_H
