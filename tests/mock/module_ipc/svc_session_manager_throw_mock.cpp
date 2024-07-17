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

void SvcSessionManager::VerifyCallerAndScenario(uint32_t clientToken, IServiceReverse::Scenario scenario) const
{
    BackupSvcSessionManager::session->VerifyCallerAndScenario(clientToken, scenario);
}

ErrCode SvcSessionManager::Active(Impl newImpl)
{
    return BackupSvcSessionManager::session->Active(newImpl);
}

void SvcSessionManager::Deactive(const wptr<IRemoteObject> &remoteInAction, bool force)
{
    BackupSvcSessionManager::session->Deactive(remoteInAction, force);
}

void SvcSessionManager::VerifyBundleName(string &bundleName)
{
    BackupSvcSessionManager::session->VerifyBundleName(bundleName);
}

sptr<IServiceReverse> SvcSessionManager::GetServiceReverseProxy()
{
    return BackupSvcSessionManager::session->GetServiceReverseProxy();
}

IServiceReverse::Scenario SvcSessionManager::GetScenario()
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

void SvcSessionManager::InitClient(Impl &newImpl)
{
    BackupSvcSessionManager::session->InitClient(newImpl);
}

void SvcSessionManager::SetExtFileNameRequest(const string &bundleName, const string &fileName)
{
    BackupSvcSessionManager::session->SetExtFileNameRequest(bundleName, fileName);
}

std::set<std::string> SvcSessionManager::GetExtFileNameRequest(const std::string &bundleName)
{
    return BackupSvcSessionManager::session->GetExtFileNameRequest(bundleName);
}

map<BundleName, BackupExtInfo>::iterator SvcSessionManager::GetBackupExtNameMap(const string &bundleName)
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

void SvcSessionManager::AppendBundles(const vector<BundleName> &bundleNames)
{
    BackupSvcSessionManager::session->AppendBundles(bundleNames);
}

sptr<SvcBackupConnection> SvcSessionManager::CreateBackupConnection(BundleName &bundleName)
{
    return BackupSvcSessionManager::session->CreateBackupConnection(bundleName);
}

void SvcSessionManager::Start()
{
    BackupSvcSessionManager::session->Start();
}

void SvcSessionManager::Finish()
{
    BackupSvcSessionManager::session->Finish();
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

void SvcSessionManager::BundleExtTimerStart(const std::string &bundleName, const Utils::Timer::TimerCallback &callback)
{
    BackupSvcSessionManager::session->BundleExtTimerStart(bundleName, callback);
}

bool SvcSessionManager::UpdateTimer(const std::string &bundleName, uint32_t timeOut,
    const Utils::Timer::TimerCallback &callback)
{
    return BackupSvcSessionManager::session->UpdateTimer(bundleName, timeOut, callback);
}

void SvcSessionManager::BundleExtTimerStop(const std::string &bundleName)
{
    BackupSvcSessionManager::session->BundleExtTimerStop(bundleName);
}

void SvcSessionManager::IncreaseSessionCnt()
{
    BackupSvcSessionManager::session->IncreaseSessionCnt();
}

void SvcSessionManager::DecreaseSessionCnt()
{
    BackupSvcSessionManager::session->DecreaseSessionCnt();
}

int32_t SvcSessionManager::GetMemParaCurSize()
{
    return BackupSvcSessionManager::session->GetMemParaCurSize();
}

void SvcSessionManager::SetMemParaCurSize(int32_t size)
{
    BackupSvcSessionManager::session->SetMemParaCurSize(size);
}

void SvcSessionManager::ClearSessionData()
{
    BackupSvcSessionManager::session->ClearSessionData();
}

bool SvcSessionManager::GetIsIncrementalBackup()
{
    return BackupSvcSessionManager::session->GetIsIncrementalBackup();
}

bool SvcSessionManager::ValidRestoreDataType(RestoreTypeEnum restoreDataType)
{
    return BackupSvcSessionManager::session->ValidRestoreDataType(restoreDataType);
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
} // namespace OHOS::FileManagement::Backup
