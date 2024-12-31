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

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_PROXY_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_PROXY_H

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>

#include "i_service.h"
#include "iremote_proxy.h"
#include "system_ability_load_callback_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceProxy : public IRemoteProxy<IService> {
public:
    ErrCode InitRestoreSession(sptr<IServiceReverse> remote) override;
    ErrCode InitRestoreSession(sptr<IServiceReverse> remote, std::string &errMsg) override;
    ErrCode InitBackupSession(sptr<IServiceReverse> remote) override;
    ErrCode InitBackupSession(sptr<IServiceReverse> remote, std::string &errMsg) override;
    ErrCode Start() override;
    UniqueFd GetLocalCapabilities() override;
    ErrCode PublishFile(const BFileInfo &fileInfo) override;
    ErrCode AppFileReady(const std::string &fileName, UniqueFd fd, int32_t errCode) override;
    ErrCode AppDone(ErrCode errCode) override;
    ErrCode ServiceResultReport(const std::string restoreRetInfo,
        BackupRestoreScenario scenario, ErrCode errCode) override;
    ErrCode GetFileHandle(const std::string &bundleName, const std::string &fileName) override;
    ErrCode AppendBundlesRestoreSession(UniqueFd fd,
                                        const std::vector<BundleName> &bundleNames,
                                        const std::vector<std::string> &detailInfos,
                                        RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND,
                                        int32_t userId = DEFAULT_INVAL_VALUE) override;
    ErrCode AppendBundlesRestoreSession(UniqueFd fd,
                                        const std::vector<BundleName> &bundleNames,
                                        RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND,
                                        int32_t userId = DEFAULT_INVAL_VALUE) override;
    ErrCode AppendBundlesBackupSession(const std::vector<BundleName> &bundleNames) override;
    ErrCode AppendBundlesDetailsBackupSession(const std::vector<BundleName> &bundleNames,
                                              const std::vector<std::string> &detailInfos) override;
    ErrCode Finish() override;
    ErrCode Release() override;
    ErrCode Cancel(std::string bundleName, int32_t &result) override;
    UniqueFd GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames) override;
    ErrCode GetAppLocalListAndDoIncrementalBackup() override;
    ErrCode InitIncrementalBackupSession(sptr<IServiceReverse> remote) override;
    ErrCode InitIncrementalBackupSession(sptr<IServiceReverse> remote, std::string &errMsg) override;
    ErrCode AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup) override;
    ErrCode AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup,
        const std::vector<std::string> &infos) override;

    ErrCode PublishIncrementalFile(const BFileInfo &fileInfo) override;
    ErrCode PublishSAIncrementalFile(const BFileInfo &fileInfo, UniqueFd fd) override;
    ErrCode AppIncrementalFileReady(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd,
        int32_t errCode) override;
    ErrCode AppIncrementalDone(ErrCode errCode) override;
    ErrCode GetIncrementalFileHandle(const std::string &bundleName, const std::string &fileName) override;
    ErrCode GetBackupInfo(BundleName &bundleName, std::string &result) override;
    ErrCode UpdateTimer(BundleName &bundleName, uint32_t timeout, bool &result) override;
    ErrCode UpdateSendRate(std::string &bundleName, int32_t sendRate, bool &result) override;
    ErrCode ReportAppProcessInfo(const std::string processInfo, const BackupRestoreScenario sennario) override;
    ErrCode StartExtTimer(bool &isExtStart) override;
    ErrCode StartFwkTimer(bool &isFwkStart) override;
    ErrCode StopExtTimer(bool &isExtStop) override;
    ErrCode RefreshDataSize(int64_t totalSize) override;

public:
    explicit ServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IService>(impl) {}
    ~ServiceProxy() override {}

public:
    template <typename T>
    bool WriteParcelableVector(const std::vector<T> &parcelableVector, Parcel &data);
    static sptr<IService> GetServiceProxyPointer();
    static sptr<IService> GetInstance();
    static void InvaildInstance();

public:
    class ServiceProxyLoadCallback : public SystemAbilityLoadCallbackStub {
    public:
        void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject) override;
        void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;

    public:
        std::condition_variable proxyConVar_;
        std::atomic<bool> isLoadSuccess_ = {false};
    };

private:
    static inline std::mutex proxyMutex_;
    static inline std::mutex getInstanceMutex_;
    static inline sptr<IService> serviceProxy_ = nullptr;
    static inline BrokerDelegator<ServiceProxy> delegator_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_PROXY_H