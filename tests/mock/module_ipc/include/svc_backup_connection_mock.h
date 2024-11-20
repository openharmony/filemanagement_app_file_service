/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License") = 0;
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

#ifndef OHOS_FILEMGMT_BACKUP_SVC_EXTENSION_PROXY_MOCK_H
#define OHOS_FILEMGMT_BACKUP_SVC_EXTENSION_PROXY_MOCK_H

#include <gmock/gmock.h>

#include "module_ipc/svc_backup_connection.h"

namespace OHOS::FileManagement::Backup {
class BSvcBackupConnection : public RefBase {
public:
    virtual ErrCode ConnectBackupExtAbility(AAFwk::Want&, int32_t, bool) = 0;
    virtual ErrCode DisconnectBackupExtAbility() = 0;
    virtual bool IsExtAbilityConnected() = 0;
    virtual sptr<IExtension> GetBackupExtProxy() = 0;
    virtual bool WaitDisconnectDone() = 0;
public:
    BSvcBackupConnection() = default;
    virtual ~BSvcBackupConnection() = default;
public:
    static inline sptr<BSvcBackupConnection> connect = nullptr;
};

class SvcBackupConnectionMock : public BSvcBackupConnection {
public:
    MOCK_METHOD(ErrCode, ConnectBackupExtAbility, (AAFwk::Want&, int32_t, bool));
    MOCK_METHOD(ErrCode, DisconnectBackupExtAbility, ());
    MOCK_METHOD(bool, IsExtAbilityConnected, ());
    MOCK_METHOD((sptr<IExtension>), GetBackupExtProxy, ());
    MOCK_METHOD(bool, WaitDisconnectDone, ());
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_SVC_EXTENSION_PROXY_MOCK_H