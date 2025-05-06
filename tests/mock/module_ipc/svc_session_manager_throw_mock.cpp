/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "module_ipc/svc_session_manager.h"
#include "svc_session_manager_throw_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ErrCode SvcSessionManager::VerifyCallerAndScenario(uint32_t clientToken, IServiceReverseType::Scenario scenario) const
{
    return BackupSvcSessionManager::session->VerifyCallerAndScenario(clientToken, scenario);
}

ErrCode SvcSessionManager::Active(Impl newImpl, bool force)
{
    return BackupSvcSessionManager::session->Active(newImpl);
}

ErrCode SvcSessionManager::Deactive(const wptr<IRemoteObject> &remoteInAction, bool force)
{
    return BackupSvcSessionManager::session->Deactive(remoteInAction, force);
}

ErrCode SvcSessionManager::VerifyBundleName(string &bundleName)
{
    return BackupSvcSessionManager::session->VerifyBundleName(bundleName);
}

sptr<IServiceReverse> SvcSessionManager::GetServiceReverseProxy()
{
    return BackupSvcSessionManager::session->GetServiceReverseProxy();
}

IServiceReverseType::Scenario SvcSessionManager::GetScenario()
{
    return BackupSvcSessionManager::session->GetScenario();
}

bool SvcSessionManager::OnBundleFileReady(const string &bundleName, const string &fileName)
{
    return BackupSvcSessionManager::session->OnBundleFileReady(bundleName, fileName);
}

UniqueFd SvcSessionManager::OnBundleExtManageInfo(const string &bundleName, UniqueFd fd)
{
    return BackupSvcSessionManager::session->OnBundleExtManageInfo(bundleName, move(fd));
}

void SvcSessionManager::RemoveExtInfo(const string &bundleName)
{
    BackupSvcSessionManager::session->RemoveExtInfo(bundleName);
}

wptr<SvcBackupConnection> SvcSessionManager::GetExtConnection(const BundleName &bundleName)
{
    return BackupSvcSessionManager::session->GetExtConnection(bundleName);
}

sptr<SvcBackupConnection> SvcSessionManager::GetBackupAbilityExt(const string &bundleName)
{
    return BackupSvcSessionManager::session->GetBackupAbilityExt(bundleName);
}

void SvcSessionManager::DumpInfo(const int fd, const std::vector<std::u16string> &args)
{
    BackupSvcSessionManager::session->DumpInfo(fd, args);
}

ErrCode SvcSessionManager::InitClient(Impl &newImpl)
{
    return BackupSvcSessionManager::session->InitClient(newImpl);
}

void SvcSessionManager::SetExtFileNameRequest(const string &bundleName, const string &fileName)
{
    BackupSvcSessionManager::session->SetExtFileNameRequest(bundleName, fileName);
}

std::set<std::string> SvcSessionManager::GetExtFileNameRequest(const std::string &bundleName)
{
    return BackupSvcSessionManager::session->GetExtFileNameRequest(bundleName);
}

std::tuple<bool, map<BundleName, BackupExtInfo>::iterator> SvcSessionManager::GetBackupExtNameMap(
    const string &bundleName)
{
    return BackupSvcSessionManager::session->GetBackupExtNameMap(bundleName);
}

bool SvcSessionManager::GetSchedBundleName(string &bundleName)
{
    return BackupSvcSessionManager::session->GetSchedBundleName(bundleName);
}

BConstants::ServiceSchedAction SvcSessionManager::GetServiceSchedAction(const std::string &bundleName)
{
    return BackupSvcSessionManager::session->GetServiceSchedAction(bundleName);
}

void SvcSessionManager::SetServiceSchedAction(const string &bundleName, BConstants::ServiceSchedAction action)
{
    BackupSvcSessionManager::session->SetServiceSchedAction(bundleName, action);
}

void SvcSessionManager::UpdateDfxInfo(const std::string &bundleName, uint64_t uniqId)
{
}

string SvcSessionManager::GetBackupExtName(const string &bundleName)
{
    return BackupSvcSessionManager::session->GetBackupExtName(bundleName);
}

void SvcSessionManager::SetBackupExtInfo(const string &bundleName, const string &extInfo)
{
    BackupSvcSessionManager::session->SetBackupExtInfo(bundleName, extInfo);
}

std::string SvcSessionManager::GetBackupExtInfo(const string &bundleName)
{
    return BackupSvcSessionManager::session->GetBackupExtInfo(bundleName);
}

void SvcSessionManager::SetBackupExtName(const string &bundleName, const string &backupExtName)
{
    BackupSvcSessionManager::session->SetBackupExtName(bundleName, backupExtName);
}

std::weak_ptr<SABackupConnection> SvcSessionManager::GetSAExtConnection(const BundleName &bundleName)
{
    return BackupSvcSessionManager::session->GetSAExtConnection(bundleName);
}

int32_t SvcSessionManager::GetBundleUserId(const string &bundleName)
{
    return BackupSvcSessionManager::session->GetBundleUserId(bundleName);
}

void SvcSessionManager::SetBundleUserId(const string &bundleName, const int32_t userId)
{
    BackupSvcSessionManager::session->SetBundleUserId(bundleName, userId);
}

void SvcSessionManager::AppendBundles(const vector<BundleName> &bundleNames, vector<BundleName> &failedBundles)
{
    BackupSvcSessionManager::session->AppendBundles(bundleNames, failedBundles);
}

sptr<SvcBackupConnection> SvcSessionManager::CreateBackupConnection(const BundleName &bundleName)
{
    return BackupSvcSessionManager::session->CreateBackupConnection(bundleName);
}

ErrCode SvcSessionManager::Start()
{
    return BackupSvcSessionManager::session->Start();
}

ErrCode SvcSessionManager::Finish()
{
    return BackupSvcSessionManager::session->Finish();
}

bool SvcSessionManager::IsOnAllBundlesFinished()
{
    return BackupSvcSessionManager::session->IsOnAllBundlesFinished();
}

bool SvcSessionManager::IsOnOnStartSched()
{
    return BackupSvcSessionManager::session->IsOnOnStartSched();
}

bool SvcSessionManager::NeedToUnloadService()
{
    return BackupSvcSessionManager::session->NeedToUnloadService();
}

int32_t SvcSessionManager::GetSessionUserId()
{
    return BackupSvcSessionManager::session->GetSessionUserId();
}

void SvcSessionManager::SetSessionUserId(int32_t userId)
{
    BackupSvcSessionManager::session->SetSessionUserId(userId);
}

string SvcSessionManager::GetSessionCallerName()
{
    return BackupSvcSessionManager::session->GetSessionCallerName();
}

string SvcSessionManager::GetSessionActiveTime()
{
    return BackupSvcSessionManager::session->GetSessionActiveTime();
}

void SvcSessionManager::SetBundleRestoreType(const std::string &bundleName, RestoreTypeEnum restoreType)
{
    BackupSvcSessionManager::session->SetBundleRestoreType(bundleName, restoreType);
}

RestoreTypeEnum SvcSessionManager::GetBundleRestoreType(const std::string &bundleName)
{
    return BackupSvcSessionManager::session->GetBundleRestoreType(bundleName);
}

void SvcSessionManager::SetBundleVersionCode(const std::string &bundleName, int64_t versionCode)
{
    BackupSvcSessionManager::session->SetBundleVersionCode(bundleName, versionCode);
}

int64_t SvcSessionManager::GetBundleVersionCode(const std::string &bundleName)
{
    return BackupSvcSessionManager::session->GetBundleVersionCode(bundleName);
}

void SvcSessionManager::SetBundleVersionName(const std::string &bundleName, std::string versionName)
{
    BackupSvcSessionManager::session->SetBundleVersionName(bundleName, versionName);
}

std::string SvcSessionManager::GetBundleVersionName(const std::string &bundleName)
{
    return BackupSvcSessionManager::session->GetBundleVersionName(bundleName);
}

void SvcSessionManager::SetBundleDataSize(const std::string &bundleName, int64_t dataSize)
{
    BackupSvcSessionManager::session->SetBundleDataSize(bundleName, dataSize);
}

bool SvcSessionManager::StartFwkTimer(const std::string &bundleName, const Utils::Timer::TimerCallback &callback)
{
    return BackupSvcSessionManager::session->StartFwkTimer(bundleName, callback);
}

bool SvcSessionManager::StopFwkTimer(const std::string &bundleName)
{
    return BackupSvcSessionManager::session->StopFwkTimer(bundleName);
}

bool SvcSessionManager::StartExtTimer(const std::string &bundleName, const Utils::Timer::TimerCallback &callback)
{
    return BackupSvcSessionManager::session->StartExtTimer(bundleName, callback);
}

bool SvcSessionManager::StopExtTimer(const std::string &bundleName)
{
    return BackupSvcSessionManager::session->StopExtTimer(bundleName);
}

bool SvcSessionManager::UpdateTimer(const std::string &bundleName, uint32_t timeout,
    const Utils::Timer::TimerCallback &callback)
{
    return BackupSvcSessionManager::session->UpdateTimer(bundleName, timeout, callback);
}

void SvcSessionManager::IncreaseSessionCnt(const std::string funcName)
{
    BackupSvcSessionManager::session->IncreaseSessionCnt(funcName);
}

void SvcSessionManager::DecreaseSessionCnt(const std::string funcName)
{
    BackupSvcSessionManager::session->DecreaseSessionCnt(funcName);
}

int32_t SvcSessionManager::GetMemParaCurSize()
{
    return BackupSvcSessionManager::session->GetMemParaCurSize();
}

void SvcSessionManager::SetMemParaCurSize(int32_t size)
{
    BackupSvcSessionManager::session->SetMemParaCurSize(size);
}

ErrCode SvcSessionManager::ClearSessionData()
{
    return BackupSvcSessionManager::session->ClearSessionData();
}

bool SvcSessionManager::GetIsIncrementalBackup()
{
    return BackupSvcSessionManager::session->GetIsIncrementalBackup();
}

bool SvcSessionManager::ValidRestoreDataType(RestoreTypeEnum restoreDataType)
{
    return BackupSvcSessionManager::session->ValidRestoreDataType(restoreDataType);
}

uint32_t SvcSessionManager::GetTimeoutValue(const std::string &bundleName)
{
    return BackupSvcSessionManager::session->GetTimeoutValue(bundleName);
}

SvcSessionManager::Impl SvcSessionManager::GetImpl()
{
    return BackupSvcSessionManager::session->GetImpl();
}

int SvcSessionManager::GetSessionCnt()
{
    return BackupSvcSessionManager::session->GetSessionCnt();
}

void SvcSessionManager::SetClearDataFlag(const std::string &bundleName, bool isNotClear)
{
    BackupSvcSessionManager::session->SetClearDataFlag(bundleName, isNotClear);
}

bool SvcSessionManager::GetClearDataFlag(const std::string &bundleName)
{
    return BackupSvcSessionManager::session->GetClearDataFlag(bundleName);
}

void SvcSessionManager::SetIncrementalData(const BIncrementalData &incrementalData)
{
    BackupSvcSessionManager::session->SetIncrementalData(incrementalData);
}

int32_t SvcSessionManager::GetIncrementalManifestFd(const string &bundleName)
{
    return BackupSvcSessionManager::session->GetIncrementalManifestFd(bundleName);
}

int64_t SvcSessionManager::GetLastIncrementalTime(const string &bundleName)
{
    return BackupSvcSessionManager::session->GetLastIncrementalTime(bundleName);
}

bool SvcSessionManager::CleanAndCheckIfNeedWait(ErrCode &ret, std::vector<std::string> &bundleNameList)
{
    return false;
}

void SvcSessionManager::SetPublishFlag(const std::string &bundleName) {}

void SvcSessionManager::SetImplRestoreType(const RestoreTypeEnum restoreType) {}

void SvcSessionManager::SetIsReadyLaunch(const std::string &bundleName) {}

void SvcSessionManager::SetOldBackupVersion(const std::string &backupVersion) {}

std::string SvcSessionManager::GetOldBackupVersion()
{
    return BackupSvcSessionManager::session->GetOldBackupVersion();
}
} // namespace OHOS::FileManagement::Backup
