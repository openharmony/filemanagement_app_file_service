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
    MOCK_METHOD(int, SendRequest, (uint32_t, MessageParcel&, MessageParcel&, MessageOption&));
    MOCK_METHOD(void, BackupOnFileReady, (std::string, std::string, int, int32_t));
    MOCK_METHOD(void, BackupOnBundleStarted, (int32_t, std::string));
    MOCK_METHOD(void, BackupOnResultReport, (std::string, std::string));;
    MOCK_METHOD(void, BackupOnBundleFinished, (int32_t, std::string));
    MOCK_METHOD(void, BackupOnAllBundlesFinished, (int32_t));
    MOCK_METHOD(void, BackupOnProcessInfo, (std::string, std::string));
    MOCK_METHOD(void, BackupOnScanningInfo, (std::string));

    MOCK_METHOD(void, RestoreOnBundleStarted, (int32_t, std::string));
    MOCK_METHOD(void, RestoreOnBundleFinished, (int32_t, std::string));
    MOCK_METHOD(void, RestoreOnAllBundlesFinished, (int32_t));
    MOCK_METHOD(void, RestoreOnFileReady, (std::string, std::string, int, int32_t));
    MOCK_METHOD(void, RestoreOnResultReport, (std::string, std::string, ErrCode));
    MOCK_METHOD(void, RestoreOnProcessInfo, (std::string, std::string));

    MOCK_METHOD(void, IncrementalBackupOnFileReady, (std::string, std::string, int, int, int32_t));
    MOCK_METHOD(void, IncrementalBackupOnBundleStarted, (int32_t, std::string));
    MOCK_METHOD(void, IncrementalBackupOnResultReport, (std::string, std::string));
    MOCK_METHOD(void, IncrementalBackupOnBundleFinished, (int32_t, std::string));
    MOCK_METHOD(void, IncrementalBackupOnAllBundlesFinished, (int32_t));
    MOCK_METHOD(void, IncrementalBackupOnProcessInfo, (std::string, std::string));
    MOCK_METHOD(void, IncrementalBackupOnScanningInfo, (std::string));

    MOCK_METHOD(void, IncrementalRestoreOnBundleStarted, (int32_t, std::string));
    MOCK_METHOD(void, IncrementalRestoreOnBundleFinished, (int32_t, std::string));
    MOCK_METHOD(void, IncrementalRestoreOnAllBundlesFinished, (int32_t));
    MOCK_METHOD(void, IncrementalRestoreOnFileReady, (std::string, std::string, int, int, int32_t));
    MOCK_METHOD(void, IncrementalRestoreOnResultReport, (std::string, std::string, ErrCode));;
    MOCK_METHOD(void, IncrementalRestoreOnProcessInfo, (std::string, std::string));

public:
    ServiceReverseProxyMock() : IRemoteProxy<IServiceReverse>(nullptr) {}
    virtual ~ServiceReverseProxyMock() = default;
};
} // namespace OHOS::FileManagement::Backup
#endif // MOCK_SERVICE_REVERSE_PROXY_MOCK_H