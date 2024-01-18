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

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_PROXY_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_PROXY_H

#include "i_service_reverse.h"
#include "iremote_proxy.h"

namespace OHOS::FileManagement::Backup {
class ServiceReverseProxy final : public IRemoteProxy<IServiceReverse>, protected NoCopyable {
public:
    void BackupOnFileReady(std::string bundleName, std::string fileName, int fd) override;
    void BackupOnBundleStarted(int32_t errCode, std::string bundleName) override;
    void BackupOnBundleFinished(int32_t errCode, std::string bundleName) override;
    void BackupOnAllBundlesFinished(int32_t errCode) override;

    void RestoreOnBundleStarted(int32_t errCode, std::string bundleName) override;
    void RestoreOnBundleFinished(int32_t errCode, std::string bundleName) override;
    void RestoreOnAllBundlesFinished(int32_t errCode) override;
    void RestoreOnFileReady(std::string bundleName, std::string fileName, int fd) override;

    void IncrementalBackupOnFileReady(std::string bundleName, std::string fileName, int fd, int manifestFd) override;
    void IncrementalBackupOnBundleStarted(int32_t errCode, std::string bundleName) override;
    void IncrementalBackupOnBundleFinished(int32_t errCode, std::string bundleName) override;
    void IncrementalBackupOnAllBundlesFinished(int32_t errCode) override;

    void IncrementalRestoreOnBundleStarted(int32_t errCode, std::string bundleName) override;
    void IncrementalRestoreOnBundleFinished(int32_t errCode, std::string bundleName) override;
    void IncrementalRestoreOnAllBundlesFinished(int32_t errCode) override;
    void IncrementalRestoreOnFileReady(std::string bundleName, std::string fileName, int fd, int manifestFd) override;

public:
    explicit ServiceReverseProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IServiceReverse>(impl) {}
    ~ServiceReverseProxy() override = default;

private:
    static inline BrokerDelegator<ServiceReverseProxy> delegator_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_PROXY_H