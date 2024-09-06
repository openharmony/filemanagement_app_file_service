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

#include "sa_backup_connection_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ErrCode SABackupConnection::ConnectBackupSAExt(std::string bundleName, std::string extension, std::string extInfo)
{
    return BSABackupConnection::saConnect->ConnectBackupSAExt(bundleName, extension, extInfo);
}

ErrCode SABackupConnection::DisconnectBackupSAExt()
{
    return BSABackupConnection::saConnect->DisconnectBackupSAExt();
}

bool SABackupConnection::IsSAExtConnected()
{
    return BSABackupConnection::saConnect->IsSAExtConnected();
}

sptr<ILocalAbilityManager> SABackupConnection::GetBackupSAExtProxy()
{
    return BSABackupConnection::saConnect->GetBackupSAExtProxy();
}

ErrCode SABackupConnection::LoadBackupSAExt()
{
    return BSABackupConnection::saConnect->LoadBackupSAExt();
}

ErrCode SABackupConnection::LoadBackupSAExtInner()
{
    return BSABackupConnection::saConnect->LoadBackupSAExtInner();
}

bool SABackupConnection::InputParaSet(MessageParcel &data)
{
    return BSABackupConnection::saConnect->InputParaSet(data);
}

bool SABackupConnection::OutputParaGet(MessageParcel &reply)
{
    return BSABackupConnection::saConnect->OutputParaGet(reply);
}

ErrCode SABackupConnection::CallBackupSA()
{
    return BSABackupConnection::saConnect->CallBackupSA();
}

ErrCode SABackupConnection::CallRestoreSA(UniqueFd fd)
{
    return BSABackupConnection::saConnect->CallRestoreSA(std::move(fd));
}

void SABackupConnection::SALoadCallback::OnLoadSystemAbilitySuccess(int32_t, const OHOS::sptr<IRemoteObject>&) {}

void SABackupConnection::SALoadCallback::OnLoadSystemAbilityFail(int32_t) {}
} // namespace OHOS::FileManagement::Backup
