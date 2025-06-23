/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 , (the "License");
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

#ifndef MOCK_SERVICE_REVERSE_PROXY_MOCK_H
#define MOCK_SERVICE_REVERSE_PROXY_MOCK_H

#include <gmock/gmock.h>

#include "iservice_reverse.h"
#include "iremote_proxy.h"

namespace OHOS::FileManagement::Backup {
class ServiceReverseProxyMock : public IRemoteProxy<IServiceReverse> {
public:
    MOCK_METHOD(int, SendRequest, (uint32_t, MessageParcel &, MessageParcel &, MessageOption &));
    MOCK_METHOD(ErrCode, BackupOnFileReady, (const std::string &, const std::string &, int, int32_t));
    MOCK_METHOD(ErrCode, BackupOnFileReadyWithoutFd, (const std::string &, const std::string &, int32_t));
    MOCK_METHOD(ErrCode, BackupOnBundleStarted, (int32_t, const std::string &));
    MOCK_METHOD(ErrCode, BackupOnResultReport, (const std::string &, const std::string &));
    ;
    MOCK_METHOD(ErrCode, BackupOnBundleFinished, (int32_t, const std::string &));
    MOCK_METHOD(ErrCode, BackupOnAllBundlesFinished, (int32_t));
    MOCK_METHOD(ErrCode, BackupOnProcessInfo, (const std::string &, const std::string &));
    MOCK_METHOD(ErrCode, BackupOnScanningInfo, (const std::string &));

    MOCK_METHOD(ErrCode, RestoreOnBundleStarted, (int32_t, const std::string &));
    MOCK_METHOD(ErrCode, RestoreOnBundleFinished, (int32_t, const std::string &));
    MOCK_METHOD(ErrCode, RestoreOnAllBundlesFinished, (int32_t));
    MOCK_METHOD(ErrCode, RestoreOnFileReady, (const std::string &, const std::string &, int32_t, int32_t));
    MOCK_METHOD(ErrCode, RestoreOnFileReadyWithoutFd, (const std::string &, const std::string &, int32_t));
    MOCK_METHOD(ErrCode, RestoreOnResultReport, (const std::string &, const std::string &, ErrCode));
    MOCK_METHOD(ErrCode, RestoreOnProcessInfo, (const std::string &, const std::string &));

    MOCK_METHOD(ErrCode, IncrementalBackupOnFileReady, (const std::string &, const std::string &, int, int, int32_t));
    MOCK_METHOD(ErrCode, IncrementalSaBackupOnFileReady, (const std::string &, const std::string &, int, int32_t));
    MOCK_METHOD(ErrCode, IncrementalBackupOnFileReadyWithoutFd, (const std::string &, const std::string &, int32_t));
    MOCK_METHOD(ErrCode, IncrementalBackupOnBundleStarted, (int32_t, const std::string &));
    MOCK_METHOD(ErrCode, IncrementalBackupOnResultReport, (const std::string &, const std::string &));
    MOCK_METHOD(ErrCode, IncrementalBackupOnBundleFinished, (int32_t, const std::string &));
    MOCK_METHOD(ErrCode, IncrementalBackupOnAllBundlesFinished, (int32_t));
    MOCK_METHOD(ErrCode, IncrementalBackupOnProcessInfo, (const std::string &, const std::string &));
    MOCK_METHOD(ErrCode, IncrementalBackupOnScanningInfo, (const std::string &));

    MOCK_METHOD(ErrCode, IncrementalRestoreOnBundleStarted, (int32_t, const std::string &));
    MOCK_METHOD(ErrCode, IncrementalRestoreOnBundleFinished, (int32_t, const std::string &));
    MOCK_METHOD(ErrCode, IncrementalRestoreOnAllBundlesFinished, (int32_t));
    MOCK_METHOD(ErrCode, IncrementalRestoreOnFileReady, (const std::string &, const std::string &, int, int, int32_t));
    MOCK_METHOD(ErrCode, IncrementalRestoreOnFileReadyWithoutFd, (const std::string &, const std::string &, int32_t));
    MOCK_METHOD(ErrCode, IncrementalRestoreOnResultReport, (const std::string &, const std::string &, ErrCode));
    ;
    MOCK_METHOD(ErrCode, IncrementalRestoreOnProcessInfo, (const std::string &, const std::string &));

public:
    ServiceReverseProxyMock() : IRemoteProxy<IServiceReverse>(nullptr) {}
    virtual ~ServiceReverseProxyMock() = default;
};
} // namespace OHOS::FileManagement::Backup
#endif // MOCK_SERVICE_REVERSE_PROXY_MOCK_H