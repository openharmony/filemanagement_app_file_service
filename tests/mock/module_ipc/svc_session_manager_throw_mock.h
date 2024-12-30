/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#ifndef OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_THROW_MOCK_H
#define OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_THROW_MOCK_H

#include <memory>
#include <string>
#include <gmock/gmock.h>

#include "module_ipc/svc_session_manager.h"

namespace OHOS::FileManagement::Backup {

class BackupSvcSessionManager {
public:
    virtual ~BackupSvcSessionManager() = default;
public:
    virtual ErrCode VerifyCallerAndScenario(uint32_t, IServiceReverse::Scenario) = 0;
    virtual ErrCode Active(SvcSessionManager::Impl) = 0;
    virtual ErrCode Deactive(const wptr<IRemoteObject> &, bool) = 0;
    virtual ErrCode VerifyBundleName(std::string &) = 0;
    virtual sptr<IServiceReverse> GetServiceReverseProxy() = 0;
    virtual IServiceReverse::Scenario GetScenario() = 0;
    virtual bool OnBundleFileReady(const std::string &, const std::string &) = 0;
    virtual UniqueFd OnBundleExtManageInfo(const std::string &, UniqueFd) = 0;
    virtual void RemoveExtInfo(const std::string &) = 0;
    virtual wptr<SvcBackupConnection> GetExtConnection(const BundleName &) = 0;
    virtual sptr<SvcBackupConnection> GetBackupAbilityExt(const std::string &) = 0;
    virtual void DumpInfo(const int, const std::vector<std::u16string> &) = 0;
    virtual ErrCode InitClient(SvcSessionManager::Impl &) = 0;
    virtual void SetExtFileNameRequest(const std::string &, const std::string &) = 0;
    virtual std::set<std::string> GetExtFileNameRequest(const std::string &) = 0;
    virtual std::map<BundleName, BackupExtInfo>::iterator GetBackupExtNameMap(const std::string &) = 0;
    virtual bool GetSchedBundleName(std::string &) = 0;
    virtual BConstants::ServiceSchedAction GetServiceSchedAction(const std::string &) = 0;
    virtual void SetServiceSchedAction(const std::string &, BConstants::ServiceSchedAction) = 0;
    virtual std::string GetBackupExtName(const std::string &) = 0;
    virtual void SetBackupExtInfo(const std::string &, const std::string &) = 0;
    virtual std::string GetBackupExtInfo(const std::string &) = 0;
    virtual void SetBackupExtName(const std::string &, const std::string &) = 0;
    virtual std::weak_ptr<SABackupConnection> GetSAExtConnection(const BundleName &) = 0;
    virtual void AppendBundles(const std::vector<BundleName> &) = 0;
    virtual sptr<SvcBackupConnection> CreateBackupConnection(BundleName &) = 0;
    virtual ErrCode Start() = 0;
    virtual ErrCode Finish() = 0;
    virtual bool IsOnAllBundlesFinished() = 0;
    virtual bool IsOnOnStartSched() = 0;
    virtual bool NeedToUnloadService() = 0;
    virtual int32_t GetSessionUserId() = 0;
    virtual void SetSessionUserId(int32_t) = 0;
    virtual std::string GetSessionCallerName() = 0;
    virtual std::string GetSessionActiveTime() = 0;
    virtual void SetBundleRestoreType(const std::string &, RestoreTypeEnum) = 0;
    virtual RestoreTypeEnum GetBundleRestoreType(const std::string &) = 0;
    virtual void SetBundleVersionCode(const std::string &, int64_t) = 0;
    virtual int64_t GetBundleVersionCode(const std::string &) = 0;
    virtual void SetBundleVersionName(const std::string &, std::string) = 0;
    virtual std::string GetBundleVersionName(const std::string &) = 0;
    virtual void SetBundleDataSize(const std::string &, int64_t) = 0;
    virtual bool StartFwkTimer(const std::string &, const Utils::Timer::TimerCallback &) = 0;
    virtual bool StopFwkTimer(const std::string &) = 0;
    virtual bool StartExtTimer(const std::string &, const Utils::Timer::TimerCallback &) = 0;
    virtual bool StopExtTimer(const std::string &) = 0;
    virtual bool UpdateTimer(const std::string &, uint32_t, const Utils::Timer::TimerCallback &) = 0;
    virtual void IncreaseSessionCnt(const std::string) = 0;
    virtual void DecreaseSessionCnt(const std::string) = 0;
    virtual int32_t GetMemParaCurSize() = 0;
    virtual void SetMemParaCurSize(int32_t) = 0;
    virtual ErrCode ClearSessionData() = 0;
    virtual bool GetIsIncrementalBackup() = 0;
    virtual bool ValidRestoreDataType(RestoreTypeEnum) = 0;
    virtual SvcSessionManager::Impl GetImpl() = 0;
    virtual int GetSessionCnt() = 0;
    virtual void SetClearDataFlag(const std::string &bundleName, bool isNotClear) = 0;
    virtual bool GetClearDataFlag(const std::string &bundleName) = 0;
    virtual void SetIncrementalData(const BIncrementalData &) = 0;
    virtual int32_t GetIncrementalManifestFd(const std::string &) = 0;
    virtual int64_t GetLastIncrementalTime(const std::string &) = 0;
    virtual bool CleanAndCheckIfNeedWait(ErrCode &ret, std::vector<std::string> &bundleNameList) = 0;
    virtual void SetPublishFlag(const std::string &bundleName) = 0;
    virtual void SetImplRestoreType(const RestoreTypeEnum restoreType) = 0;
    virtual void SetIsReadyLaunch(const std::string &bundleName) = 0;
    virtual void SetOldBackupVersion(const std::string &backupVersion) = 0;
    virtual std::string GetOldBackupVersion() = 0;
public:
    static inline std::shared_ptr<BackupSvcSessionManager> session = nullptr;
};

class SvcSessionManagerMock : public BackupSvcSessionManager {
public:
    MOCK_METHOD(ErrCode, VerifyCallerAndScenario, (uint32_t, IServiceReverse::Scenario));
    MOCK_METHOD(ErrCode, Active, (SvcSessionManager::Impl));
    MOCK_METHOD(ErrCode, Deactive, (const wptr<IRemoteObject> &, bool));
    MOCK_METHOD(ErrCode, VerifyBundleName, (std::string &));
    MOCK_METHOD(sptr<IServiceReverse>, GetServiceReverseProxy, ());
    MOCK_METHOD(IServiceReverse::Scenario, GetScenario, ());
    MOCK_METHOD(bool, OnBundleFileReady, (const std::string &, const std::string &));
    MOCK_METHOD(UniqueFd, OnBundleExtManageInfo, (const std::string &, UniqueFd));
    MOCK_METHOD(void, RemoveExtInfo, (const std::string &));
    MOCK_METHOD(wptr<SvcBackupConnection>, GetExtConnection, (const BundleName &));
    MOCK_METHOD(sptr<SvcBackupConnection>, GetBackupAbilityExt, (const std::string &));
    MOCK_METHOD(void, DumpInfo, (const int, const std::vector<std::u16string> &));
    MOCK_METHOD(ErrCode, InitClient, (SvcSessionManager::Impl &));
    MOCK_METHOD(void, SetExtFileNameRequest, (const std::string &, const std::string &));
    MOCK_METHOD(std::set<std::string>, GetExtFileNameRequest, (const std::string &));
    MOCK_METHOD((std::map<BundleName, BackupExtInfo>::iterator), GetBackupExtNameMap, (const std::string &));
    MOCK_METHOD(bool, GetSchedBundleName, (std::string &));
    MOCK_METHOD(BConstants::ServiceSchedAction, GetServiceSchedAction, (const std::string &));
    MOCK_METHOD(void, SetServiceSchedAction, (const std::string &, BConstants::ServiceSchedAction));
    MOCK_METHOD(std::string, GetBackupExtName, (const std::string &));
    MOCK_METHOD(void, SetBackupExtInfo, (const std::string &, const std::string &));
    MOCK_METHOD(std::string, GetBackupExtInfo, (const std::string &));
    MOCK_METHOD(void, SetBackupExtName, (const std::string &, const std::string &));
    MOCK_METHOD(std::weak_ptr<SABackupConnection>, GetSAExtConnection, (const BundleName &));
    MOCK_METHOD(void, AppendBundles, (const std::vector<BundleName> &));
    MOCK_METHOD(sptr<SvcBackupConnection>, CreateBackupConnection, (BundleName &));
    MOCK_METHOD(ErrCode, Start, ());
    MOCK_METHOD(ErrCode, Finish, ());
    MOCK_METHOD(bool, IsOnAllBundlesFinished, ());
    MOCK_METHOD(bool, IsOnOnStartSched, ());
    MOCK_METHOD(bool, NeedToUnloadService, ());
    MOCK_METHOD(int32_t, GetSessionUserId, ());
    MOCK_METHOD(void, SetSessionUserId, (int32_t));
    MOCK_METHOD(std::string, GetSessionCallerName, ());
    MOCK_METHOD(std::string, GetSessionActiveTime, ());
    MOCK_METHOD(void, SetBundleRestoreType, (const std::string &, RestoreTypeEnum));
    MOCK_METHOD(RestoreTypeEnum, GetBundleRestoreType, (const std::string &));
    MOCK_METHOD(void, SetBundleVersionCode, (const std::string &, int64_t));
    MOCK_METHOD(int64_t, GetBundleVersionCode, (const std::string &));
    MOCK_METHOD(void, SetBundleVersionName, (const std::string &, std::string));
    MOCK_METHOD(std::string, GetBundleVersionName, (const std::string &));
    MOCK_METHOD(void, SetBundleDataSize, (const std::string &, int64_t));
    MOCK_METHOD(bool, StartFwkTimer, (const std::string &, const Utils::Timer::TimerCallback &));
    MOCK_METHOD(bool, StopFwkTimer, (const std::string &));
    MOCK_METHOD(bool, StartExtTimer, (const std::string &, const Utils::Timer::TimerCallback &));
    MOCK_METHOD(bool, StopExtTimer, (const std::string &));
    MOCK_METHOD(bool, UpdateTimer, (const std::string &, uint32_t, const Utils::Timer::TimerCallback &));
    MOCK_METHOD(void, IncreaseSessionCnt, (const std::string));
    MOCK_METHOD(void, DecreaseSessionCnt, (const std::string));
    MOCK_METHOD(int32_t, GetMemParaCurSize, ());
    MOCK_METHOD(void, SetMemParaCurSize, (int32_t));
    MOCK_METHOD(ErrCode, ClearSessionData, ());
    MOCK_METHOD(bool, GetIsIncrementalBackup, ());
    MOCK_METHOD(bool, ValidRestoreDataType, (RestoreTypeEnum));
    MOCK_METHOD(SvcSessionManager::Impl, GetImpl, ());
    MOCK_METHOD(int, GetSessionCnt, ());
    MOCK_METHOD(void, SetClearDataFlag, (const std::string &, bool));
    MOCK_METHOD(bool, GetClearDataFlag, (const std::string &));
    MOCK_METHOD(void, SetIncrementalData, (const BIncrementalData &));
    MOCK_METHOD(int32_t, GetIncrementalManifestFd, (const std::string &));
    MOCK_METHOD(int64_t, GetLastIncrementalTime, (const std::string &));
    MOCK_METHOD(bool, CleanAndCheckIfNeedWait, (ErrCode &, std::vector<std::string> &));
    MOCK_METHOD(void, SetPublishFlag, (const std::string &));
    MOCK_METHOD(void, SetImplRestoreType, (const RestoreTypeEnum restoreType));
    MOCK_METHOD(void, SetIsReadyLaunch, (const std::string &));
    MOCK_METHOD(void, SetOldBackupVersion, (const std::string &));
    MOCK_METHOD(std::string, GetOldBackupVersion, ());
};

} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_THROW_MOCK_H