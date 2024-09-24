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

#ifndef OHOS_FILEMGMT_BACKUP_SA_BACKUP_CONNECTION_MOCK_H
#define OHOS_FILEMGMT_BACKUP_SA_BACKUP_CONNECTION_MOCK_H

#include <gmock/gmock.h>

#include "module_ipc/sa_backup_connection.h"

namespace OHOS::FileManagement::Backup {
class BSABackupConnection {
public:
    virtual bool InputParaSet(MessageParcel&) = 0;
    virtual bool OutputParaGet(MessageParcel&) = 0;
    virtual ErrCode ConnectBackupSAExt(std::string, std::string, std::string) = 0;
    virtual ErrCode DisconnectBackupSAExt() = 0;
    virtual bool IsSAExtConnected() = 0;
    virtual sptr<ILocalAbilityManager> GetBackupSAExtProxy() = 0;
    virtual ErrCode LoadBackupSAExt() = 0;
    virtual ErrCode LoadBackupSAExtInner() = 0;
    virtual ErrCode CallBackupSA() = 0;
    virtual ErrCode CallRestoreSA(UniqueFd) = 0;
public:
    BSABackupConnection() = default;
    virtual ~BSABackupConnection() = default;
public:
    static inline std::shared_ptr<BSABackupConnection> saConnect = nullptr;
};

class SABackupConnectionMock : public BSABackupConnection {
public:
    MOCK_METHOD(bool, InputParaSet, (MessageParcel&));
    MOCK_METHOD(bool, OutputParaGet, (MessageParcel&));
    MOCK_METHOD(ErrCode, ConnectBackupSAExt, (std::string, std::string, std::string));
    MOCK_METHOD(ErrCode, DisconnectBackupSAExt, ());
    MOCK_METHOD(bool, IsSAExtConnected, ());
    MOCK_METHOD((sptr<ILocalAbilityManager>), GetBackupSAExtProxy, ());
    MOCK_METHOD(ErrCode, LoadBackupSAExt, ());
    MOCK_METHOD(ErrCode, LoadBackupSAExtInner, ());
    MOCK_METHOD(ErrCode, CallBackupSA, ());
    MOCK_METHOD(ErrCode, CallRestoreSA, (UniqueFd));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_SA_BACKUP_CONNECTION_MOCK_H