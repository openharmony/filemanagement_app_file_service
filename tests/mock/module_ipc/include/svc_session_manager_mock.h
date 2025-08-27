/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_MOCK_H
#define OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_MOCK_H

#include <gmock/gmock.h>

#include "module_ipc/svc_session_manager.h"

namespace OHOS::FileManagement::Backup {
class BSvcSessionManager {
public:
    virtual ErrCode Active(struct SvcSessionManager::Impl, bool) = 0;
    virtual sptr<IServiceReverse> GetServiceReverseProxy() = 0;
    virtual IServiceReverseType::Scenario GetScenario() = 0;
    virtual int32_t GetSessionUserId() = 0;
    virtual std::string GetSessionCallerName() = 0;
    virtual std::string GetSessionActiveTime() = 0;
    virtual bool OnBundleFileReady(const std::string&, const std::string&) = 0;
    virtual UniqueFd OnBundleExtManageInfo(const std::string&, UniqueFd) = 0;
    virtual wptr<SvcBackupConnection> GetExtConnection(const BundleName&) = 0;
    virtual std::weak_ptr<SABackupConnection> GetSAExtConnection(const BundleName&) = 0;
    virtual std::set<std::string> GetExtFileNameRequest(const std::string&) = 0;
    virtual BConstants::ServiceSchedAction GetServiceSchedAction(const std::string&) = 0;
    virtual bool GetSchedBundleName(std::string&) = 0;
    virtual std::string GetBackupExtName(const std::string&) = 0;
    virtual std::string GetBackupExtInfo(const std::string&) = 0;
    virtual sptr<SvcBackupConnection> CreateBackupConnection(const BundleName&) = 0;
    virtual bool IsOnAllBundlesFinished() = 0;
    virtual bool IsOnOnStartSched() = 0;
    virtual bool NeedToUnloadService() = 0;
    virtual RestoreTypeEnum GetBundleRestoreType(const std::string&) = 0;
    virtual int64_t GetBundleVersionCode(const std::string&) = 0;
    virtual std::string GetBundleVersionName(const std::string&) = 0;
    virtual bool StartFwkTimer(const std::string&, const Utils::Timer::TimerCallback&) = 0;
    virtual bool StopFwkTimer(const std::string&) = 0;
    virtual bool StartExtTimer(const std::string&, const Utils::Timer::TimerCallback&) = 0;
    virtual bool StopExtTimer(const std::string&) = 0;
    virtual bool UpdateTimer(const std::string&, uint32_t, const Utils::Timer::TimerCallback&) = 0;
    virtual bool GetIsIncrementalBackup() = 0;
    virtual int32_t GetIncrementalManifestFd(const std::string&) = 0;
    virtual int64_t GetLastIncrementalTime(const std::string&) = 0;
    virtual int32_t GetMemParaCurSize() = 0;
    virtual bool ValidRestoreDataType(RestoreTypeEnum) = 0;
    virtual uint32_t GetTimeoutValue(const std::string &) = 0;
    virtual bool GetClearDataFlag(const std::string&) = 0;
    virtual bool CleanAndCheckIfNeedWait(ErrCode &, std::vector<std::string> &) = 0;
    virtual ErrCode ClearSessionData() = 0;
    virtual int GetSessionCnt() = 0;
    virtual SvcSessionManager::Impl GetImpl() = 0;
    virtual std::string GetOldBackupVersion() = 0;
    virtual void UpdateDfxInfo(const BundleName &bundleName, uint64_t uniqId) = 0;
    virtual void HandleOnRelease(sptr<IExtension> proxy) = 0;
    virtual void SetIsRestoreEnd(const std::string &bundleName) = 0;
    virtual bool GetIsRestoreEnd(const std::string &bundleName) = 0;
    virtual std::string GetBackupScene(const std::string &bundleName) = 0;
    virtual void SetBackupScene(const std::string &bundleName, const std::string &backupScene) = 0;
public:
    BSvcSessionManager() = default;
    virtual ~BSvcSessionManager() = default;
public:
    static inline std::shared_ptr<BSvcSessionManager> sessionManager = nullptr;
};

class SvcSessionManagerMock : public BSvcSessionManager {
public:
    MOCK_METHOD(ErrCode, Active, (struct SvcSessionManager::Impl, bool));
    MOCK_METHOD((sptr<IServiceReverse>), GetServiceReverseProxy, ());
    MOCK_METHOD(IServiceReverseType::Scenario, GetScenario, ());
    MOCK_METHOD(int32_t, GetSessionUserId, ());
    MOCK_METHOD(std::string, GetSessionCallerName, ());
    MOCK_METHOD(std::string, GetSessionActiveTime, ());
    MOCK_METHOD(bool, OnBundleFileReady, (const std::string&, const std::string&));
    MOCK_METHOD(UniqueFd, OnBundleExtManageInfo, (const std::string&, UniqueFd));
    MOCK_METHOD((wptr<SvcBackupConnection>), GetExtConnection, (const BundleName&));
    MOCK_METHOD((std::weak_ptr<SABackupConnection>), GetSAExtConnection, (const BundleName&));
    MOCK_METHOD((std::set<std::string>), GetExtFileNameRequest, (const std::string&));
    MOCK_METHOD(BConstants::ServiceSchedAction, GetServiceSchedAction, (const std::string&));
    MOCK_METHOD(bool, GetSchedBundleName, (std::string&));
    MOCK_METHOD(std::string, GetBackupExtName, (const std::string&));
    MOCK_METHOD(std::string, GetBackupExtInfo, (const std::string&));
    MOCK_METHOD(sptr<SvcBackupConnection>, CreateBackupConnection, (const BundleName&));
    MOCK_METHOD(bool, IsOnAllBundlesFinished, ());
    MOCK_METHOD(bool, IsOnOnStartSched, ());
    MOCK_METHOD(bool, NeedToUnloadService, ());
    MOCK_METHOD(RestoreTypeEnum, GetBundleRestoreType, (const std::string&));
    MOCK_METHOD(int64_t, GetBundleVersionCode, (const std::string&));
    MOCK_METHOD(std::string, GetBundleVersionName, (const std::string&));
    MOCK_METHOD(bool, StartFwkTimer, (const std::string&, const Utils::Timer::TimerCallback&));
    MOCK_METHOD(bool, StopFwkTimer, (const std::string&));
    MOCK_METHOD(bool, StartExtTimer, (const std::string&, const Utils::Timer::TimerCallback&));
    MOCK_METHOD(bool, StopExtTimer, (const std::string&));
    MOCK_METHOD(bool, UpdateTimer, (const std::string&, uint32_t, const Utils::Timer::TimerCallback&));
    MOCK_METHOD(bool, GetIsIncrementalBackup, ());
    MOCK_METHOD(int32_t, GetIncrementalManifestFd, (const std::string&));
    MOCK_METHOD(int64_t, GetLastIncrementalTime, (const std::string&));
    MOCK_METHOD(int32_t, GetMemParaCurSize, ());
    MOCK_METHOD(bool, ValidRestoreDataType, (RestoreTypeEnum));
    MOCK_METHOD(uint32_t, GetTimeoutValue, (const std::string &));
    MOCK_METHOD(bool, GetClearDataFlag, (const std::string&));
    MOCK_METHOD(bool, CleanAndCheckIfNeedWait, (ErrCode &, std::vector<std::string> &));
    MOCK_METHOD(ErrCode, ClearSessionData, ());
    MOCK_METHOD(int, GetSessionCnt, ());
    MOCK_METHOD(SvcSessionManager::Impl, GetImpl, ());
    MOCK_METHOD(std::string, GetOldBackupVersion, ());
    MOCK_METHOD(void, UpdateDfxInfo, (const std::string &, uint64_t));
    MOCK_METHOD(void, HandleOnRelease, (sptr<IExtension>));
    MOCK_METHOD(void, SetIsRestoreEnd, (const std::string &));
    MOCK_METHOD(bool, GetIsRestoreEnd, (const std::string &));
    MOCK_METHOD(void, SetBackupScene, (const std::string &, const std::string &));
    MOCK_METHOD(std::string, GetBackupScene, (const std::string &));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_MOCK_H
