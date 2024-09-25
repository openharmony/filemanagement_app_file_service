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

#include "module_ipc/svc_backup_connection.h"

#include "svc_backup_connection_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void SvcBackupConnection::OnAbilityConnectDone(const AppExecFwk::ElementName&, const sptr<IRemoteObject>&, int) {}

void SvcBackupConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName&, int) {}

ErrCode SvcBackupConnection::ConnectBackupExtAbility(AAFwk::Want &want, int32_t userId)
{
    return BSvcBackupConnection::connect->ConnectBackupExtAbility(want, userId);
}

ErrCode SvcBackupConnection::DisconnectBackupExtAbility()
{
    return BSvcBackupConnection::connect->DisconnectBackupExtAbility();
}

bool SvcBackupConnection::IsExtAbilityConnected()
{
    return BSvcBackupConnection::connect->IsExtAbilityConnected();
}

void SvcBackupConnection::SetCallback(function<void(const std::string &&)>) {}

void SvcBackupConnection::SetCallDied(function<void(const std::string &&)>) {}

sptr<IExtension> SvcBackupConnection::GetBackupExtProxy()
{
    return BSvcBackupConnection::connect->GetBackupExtProxy();
}

bool SvcBackupConnection::WaitDisconnectDone()
{
    return BSvcBackupConnection::connect->WaitDisconnectDone();
}
} // namespace OHOS::FileManagement::Backup