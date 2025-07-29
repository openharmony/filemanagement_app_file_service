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

#ifndef OHOS_FILEMGMT_BACKUP_SVC_BACKUP_CONNECTION_MOCK_H
#define OHOS_FILEMGMT_BACKUP_SVC_BACKUP_CONNECTION_MOCK_H

#include <gmock/gmock.h>

#include <iremote_proxy.h>
#include "iextension.h"

namespace OHOS::FileManagement::Backup {
class SvcExtensionProxyMock : public IRemoteProxy<IExtension> {
public:
    SvcExtensionProxyMock() : IRemoteProxy<IExtension>(nullptr) {};
    virtual ~SvcExtensionProxyMock() = default;
public:
MOCK_METHOD(ErrCode, GetFileHandleWithUniqueFd, (const std::string &, int32_t &, int32_t &));
MOCK_METHOD(ErrCode, HandleClear, ());
MOCK_METHOD(ErrCode, HandleBackup, (bool));
MOCK_METHOD(ErrCode, PublishFile, (const std::string &));
MOCK_METHOD(ErrCode, HandleRestore, (bool));
MOCK_METHOD(ErrCode, GetIncrementalFileHandle, (const std::string &, int &, int &, int32_t &));
MOCK_METHOD(ErrCode, PublishIncrementalFile, (const std::string &));
MOCK_METHOD(ErrCode, HandleIncrementalBackup, (int32_t, int32_t));
MOCK_METHOD(ErrCode, IncrementalOnBackup, (bool));
MOCK_METHOD(ErrCode, GetIncrementalBackupFileHandle, (int &, int &));
MOCK_METHOD(ErrCode, GetBackupInfo, (std::string &));
MOCK_METHOD(ErrCode, UpdateFdSendRate, (const std::string &, int32_t));
MOCK_METHOD(ErrCode, User0OnBackup, ());
MOCK_METHOD(ErrCode, UpdateDfxInfo, (int64_t, uint32_t, const std::string &));
MOCK_METHOD(ErrCode, CleanBundleTempDir, ());
MOCK_METHOD(ErrCode, HandleOnRelease, (int32_t));
MOCK_METHOD(ErrCode, HandleGetCompatibilityInfo, (const std::string &, int32_t, std::string &));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_SVC_BACKUP_CONNECTION_MOCK_H