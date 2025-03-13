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

#include "module_ipc/svc_backup_connection.h"

#include <string>

#include "extension_proxy.h"
#include "module_ipc/svc_session_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
static bool g_bExtAbilityConnected = false;
}

void SvcBackupConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
                                               const sptr<IRemoteObject> &remoteObject,
                                               int resultCode)
{
    isConnected_.store(true);
    backupProxy_ = iface_cast<IExtension>(remoteObject);
    string bundleName = "";
    callConnected_(move(bundleName));
}

void SvcBackupConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    backupProxy_ = nullptr;
    isConnected_.store(false);
    string bundleName = "";
    callDied_(move(bundleName), false);
}

ErrCode SvcBackupConnection::ConnectBackupExtAbility(AAFwk::Want &want, int32_t userId, bool isCleanCalled)
{
    return 0;
}

ErrCode SvcBackupConnection::DisconnectBackupExtAbility()
{
    return 0;
}

bool SvcBackupConnection::WaitDisconnectDone()
{
    return true;
}

bool SvcBackupConnection::IsExtAbilityConnected()
{
    bool bFlag = g_bExtAbilityConnected;
    g_bExtAbilityConnected = !bFlag;
    return bFlag;
}

void SvcBackupConnection::SetCallback(function<void(const std::string &&)> callConnected)
{
    callConnected_ = callConnected;
}

void SvcBackupConnection::SetCallDied(function<void(const std::string &&, bool)> callDied)
{
    callDied_ = callDied;
}

sptr<IExtension> SvcBackupConnection::GetBackupExtProxy()
{
    return backupProxy_;
}
} // namespace OHOS::FileManagement::Backup