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

#include "ext_backup_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void ExtBackup::SetCreator(const CreatorFunc &creator)
{
    BExtBackup::extBackup->SetCreator(creator);
}

void ExtBackup::Init(const shared_ptr<AbilityRuntime::AbilityLocalRecord> &record,
                     const shared_ptr<AbilityRuntime::OHOSApplication> &application,
                     shared_ptr<AbilityRuntime::AbilityHandler> &handler,
                     const sptr<IRemoteObject> &token)
{
    BExtBackup::extBackup->Init(record, application, handler, token);
}

ExtBackup *ExtBackup::Create(const unique_ptr<AbilityRuntime::Runtime> &runtime)
{
    return BExtBackup::extBackup->Create(runtime);
}

void ExtBackup::OnStart(const AAFwk::Want &want)
{
    BExtBackup::extBackup->OnStart(want);
}

void ExtBackup::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    BExtBackup::extBackup->OnCommand(want, restart, startId);
}

string ExtBackup::GetUsrConfig() const
{
    return BExtBackup::extBackup->GetUsrConfig();
}

bool ExtBackup::AllowToBackupRestore()
{
    return BExtBackup::extBackup->AllowToBackupRestore();
}

bool ExtBackup::UseFullBackupOnly(void) const
{
    return BExtBackup::extBackup->UseFullBackupOnly();
}

BConstants::ExtensionAction ExtBackup::GetExtensionAction() const
{
    return BExtBackup::extBackup->GetExtensionAction();
}

BConstants::ExtensionAction ExtBackup::VerifyAndGetAction(const AAFwk::Want &want,
                                                          std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo)
{
    return BExtBackup::extBackup->VerifyAndGetAction(want, abilityInfo);
}

ErrCode ExtBackup::GetParament(const AAFwk::Want &want)
{
    return BExtBackup::extBackup->GetParament(want);
}

sptr<IRemoteObject> ExtBackup::OnConnect(const AAFwk::Want &want)
{
    return BExtBackup::extBackup->OnConnect(want);
}

void ExtBackup::OnDisconnect(const AAFwk::Want &want)
{
    BExtBackup::extBackup->OnDisconnect(want);
}

bool ExtBackup::WasFromSpecialVersion(void)
{
    return BExtBackup::extBackup->WasFromSpecialVersion();
}

bool ExtBackup::SpecialVersionForCloneAndCloud(void)
{
    return BExtBackup::extBackup->SpecialVersionForCloneAndCloud();
}

bool ExtBackup::RestoreDataReady()
{
    return BExtBackup::extBackup->RestoreDataReady();
}

ErrCode ExtBackup::OnBackup(function<void(ErrCode, std::string)> callback)
{
    return BExtBackup::extBackup->OnBackup(callback);
}

ErrCode ExtBackup::OnBackup(std::function<void(ErrCode, std::string)> callback,
    std::function<void(ErrCode, const std::string)> callbackEx)
{
    return BExtBackup::extBackup->OnBackup(callback, callbackEx);
}

ErrCode ExtBackup::OnRestore(function<void(ErrCode, std::string)> callback,
    std::function<void(ErrCode, const std::string)> callbackEx)
{
    return BExtBackup::extBackup->OnRestore(callback, callbackEx);
}

ErrCode ExtBackup::OnRestore(function<void(ErrCode, std::string)> callback)
{
    return BExtBackup::extBackup->OnRestore(callback);
}

ErrCode ExtBackup::GetBackupInfo(function<void(ErrCode, std::string)> callback)
{
    return BExtBackup::extBackup->GetBackupInfo(callback);
}

ErrCode ExtBackup::InvokeAppExtMethod(ErrCode errCode, const std::string result)
{
    return BExtBackup::extBackup->InvokeAppExtMethod(errCode, result);
}

ErrCode ExtBackup::OnProcess(function<void(ErrCode, std::string)> callback)
{
    return BExtBackup::extBackup->OnProcess(callback);
}
} // namespace OHOS::FileManagement::Backup
