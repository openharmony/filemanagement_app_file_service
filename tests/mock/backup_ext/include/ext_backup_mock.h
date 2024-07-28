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

#ifndef OHOS_FILEMGMT_BACKUP_EXT_BACKUP_MOCK_H
#define OHOS_FILEMGMT_BACKUP_EXT_BACKUP_MOCK_H

#include <gmock/gmock.h>

#include "ext_backup.h"

namespace OHOS::FileManagement::Backup {
class BExtBackup {
public:
    virtual void OnStart(const AAFwk::Want &) = 0;
    virtual void Init(const std::shared_ptr<AbilityRuntime::AbilityLocalRecord> &,
              const std::shared_ptr<AbilityRuntime::OHOSApplication> &,
              std::shared_ptr<AbilityRuntime::AbilityHandler> &,
              const sptr<IRemoteObject> &) = 0;
    virtual void OnCommand(const AAFwk::Want &, bool, int) = 0;
    virtual sptr<IRemoteObject> OnConnect(const AAFwk::Want &) = 0;
    virtual void OnDisconnect(const AAFwk::Want &) = 0;
    virtual ExtBackup *Create(const std::unique_ptr<AbilityRuntime::Runtime> &) = 0;
    virtual BConstants::ExtensionAction GetExtensionAction() = 0;
    virtual bool AllowToBackupRestore() = 0;
    virtual bool UseFullBackupOnly(void) = 0;
    virtual std::string GetUsrConfig() = 0;
    virtual ErrCode OnBackup(std::function<void(ErrCode, std::string)>) = 0;
    virtual ErrCode OnBackup(std::function<void(ErrCode, std::string)>,
        std::function<void(ErrCode, const std::string)>) = 0;
    virtual ErrCode OnRestore(std::function<void(ErrCode, std::string)>,
        std::function<void(ErrCode, const std::string)>) = 0;
    virtual ErrCode OnRestore(std::function<void(ErrCode, std::string)>) = 0;
    virtual ErrCode GetBackupInfo(std::function<void(ErrCode, std::string)>) = 0;
    virtual bool WasFromSpecialVersion(void) = 0;
    virtual bool SpecialVersionForCloneAndCloud(void) = 0;
    virtual bool RestoreDataReady() = 0;
    virtual ErrCode InvokeAppExtMethod(ErrCode, const std::string) = 0;
    virtual void SetCreator(const CreatorFunc &) = 0;
    virtual BConstants::ExtensionAction VerifyAndGetAction(const AAFwk::Want &,
        std::shared_ptr<AppExecFwk::AbilityInfo>) = 0;
    virtual ErrCode GetParament(const AAFwk::Want &) = 0;
public:
    virtual std::unique_ptr<NativeReference> LoadSystemModuleByEngine(napi_env, const std::string&, const napi_value*,
        size_t) = 0;
    virtual napi_env GetNapiEnv() const = 0;
public:
    virtual napi_value CreateExtBackupJsContext(napi_env, std::shared_ptr<ExtBackupContext>) = 0;
public:
    BExtBackup() = default;
    virtual ~BExtBackup() = default;
public:
    static inline std::shared_ptr<BExtBackup> extBackup = nullptr;
};

class ExtBackupMock : public BExtBackup {
public:
    MOCK_METHOD(void, OnStart, (const AAFwk::Want &));
    MOCK_METHOD(void, Init, (const std::shared_ptr<AbilityRuntime::AbilityLocalRecord> &,
              const std::shared_ptr<AbilityRuntime::OHOSApplication> &,
              std::shared_ptr<AbilityRuntime::AbilityHandler> &,
              const sptr<IRemoteObject> &));
    MOCK_METHOD(void, OnCommand, (const AAFwk::Want &, bool, int));
    MOCK_METHOD(sptr<IRemoteObject>, OnConnect, (const AAFwk::Want &));
    MOCK_METHOD(void, OnDisconnect, (const AAFwk::Want &));
    MOCK_METHOD(ExtBackup*, Create, (const std::unique_ptr<AbilityRuntime::Runtime> &));
    MOCK_METHOD(BConstants::ExtensionAction, GetExtensionAction, ());
    MOCK_METHOD(bool, AllowToBackupRestore, ());
    MOCK_METHOD(bool, UseFullBackupOnly, ());
    MOCK_METHOD(std::string, GetUsrConfig, ());
    MOCK_METHOD(ErrCode, OnBackup, (std::function<void(ErrCode, std::string)>));
    MOCK_METHOD(ErrCode, OnBackup, (std::function<void(ErrCode, std::string)>,
        std::function<void(ErrCode, const std::string)>));
    MOCK_METHOD(ErrCode, OnRestore, (std::function<void(ErrCode, std::string)>,
        std::function<void(ErrCode, const std::string)>));
    MOCK_METHOD(ErrCode, OnRestore, (std::function<void(ErrCode, std::string)>));
    MOCK_METHOD(ErrCode, GetBackupInfo, (std::function<void(ErrCode, std::string)>));
    MOCK_METHOD(bool, WasFromSpecialVersion, ());
    MOCK_METHOD(bool, SpecialVersionForCloneAndCloud, ());
    MOCK_METHOD(bool, RestoreDataReady, ());
    MOCK_METHOD(ErrCode, InvokeAppExtMethod, (ErrCode, const std::string));
    MOCK_METHOD(void, SetCreator, (const CreatorFunc &));
    MOCK_METHOD(BConstants::ExtensionAction, VerifyAndGetAction, (const AAFwk::Want &,
        std::shared_ptr<AppExecFwk::AbilityInfo>));
    MOCK_METHOD(ErrCode, GetParament, (const AAFwk::Want &));
    MOCK_METHOD(napi_value, CreateExtBackupJsContext, (napi_env, std::shared_ptr<ExtBackupContext>));
public:
    MOCK_METHOD((std::unique_ptr<NativeReference>), LoadSystemModuleByEngine, (napi_env, const std::string&,
        const napi_value*, size_t));
    MOCK_METHOD(napi_env, GetNapiEnv, (), (const));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_EXT_BACKUP_H
