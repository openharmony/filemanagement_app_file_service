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

#include "svc_session_manager_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void SvcSessionManager::VerifyCallerAndScenario(uint32_t, IServiceReverse::Scenario) const {}

ErrCode SvcSessionManager::Active(Impl newImpl, bool force)
{
    return BSvcSessionManager::sessionManager->Active(newImpl, force);
}

void SvcSessionManager::Deactive(const wptr<IRemoteObject> &, bool) {}

void SvcSessionManager::VerifyBundleName(string &bundleName) {}

sptr<IServiceReverse> SvcSessionManager::GetServiceReverseProxy()
{
    return BSvcSessionManager::sessionManager->GetServiceReverseProxy();
}

IServiceReverse::Scenario SvcSessionManager::GetScenario()
{
    return BSvcSessionManager::sessionManager->GetScenario();
}

bool SvcSessionManager::OnBundleFileReady(const string &bundleName, const string &fileName)
{
    return BSvcSessionManager::sessionManager->OnBundleFileReady(bundleName, fileName);
}

UniqueFd SvcSessionManager::OnBundleExtManageInfo(const string &bundleName, UniqueFd fd)
{
    return BSvcSessionManager::sessionManager->OnBundleExtManageInfo(bundleName, std::move(fd));
}

void SvcSessionManager::RemoveExtInfo(const string &) {}

wptr<SvcBackupConnection> SvcSessionManager::GetExtConnection(const BundleName &bundleName)
{
    return BSvcSessionManager::sessionManager->GetExtConnection(bundleName);
}

sptr<SvcBackupConnection> SvcSessionManager::GetBackupAbilityExt(const string &bundleName)
{
    return nullptr;
}

void SvcSessionManager::DumpInfo(const int, const std::vector<std::u16string> &) {}

void SvcSessionManager::InitClient(Impl &) {}

void SvcSessionManager::SetExtFileNameRequest(const string &bundleName, const string &) {}

std::set<std::string> SvcSessionManager::GetExtFileNameRequest(const std::string &bundleName)
{
    return BSvcSessionManager::sessionManager->GetExtFileNameRequest(bundleName);
}

map<BundleName, BackupExtInfo>::iterator SvcSessionManager::GetBackupExtNameMap(const string &)
{
    return {};
}

bool SvcSessionManager::GetSchedBundleName(string &bundleName)
{
    return BSvcSessionManager::sessionManager->GetSchedBundleName(bundleName);
}

BConstants::ServiceSchedAction SvcSessionManager::GetServiceSchedAction(const std::string &bundleName)
{
    return BSvcSessionManager::sessionManager->GetServiceSchedAction(bundleName);
}

void SvcSessionManager::SetServiceSchedAction(const string &, BConstants::ServiceSchedAction) {}

string SvcSessionManager::GetBackupExtName(const string &bundleName)
{
    return BSvcSessionManager::sessionManager->GetBackupExtName(bundleName);
}

void SvcSessionManager::SetBackupExtInfo(const string &, const string &) {}

std::string SvcSessionManager::GetBackupExtInfo(const string &bundleName)
{
    return BSvcSessionManager::sessionManager->GetBackupExtInfo(bundleName);
}

void SvcSessionManager::SetBackupExtName(const string &, const string &) {}

std::weak_ptr<SABackupConnection> SvcSessionManager::GetSAExtConnection(const BundleName &bundleName)
{
    return BSvcSessionManager::sessionManager->GetSAExtConnection(bundleName);
}

void SvcSessionManager::AppendBundles(const vector<BundleName> &) {}

sptr<SvcBackupConnection> SvcSessionManager::CreateBackupConnection(BundleName &bundleName)
{
    return BSvcSessionManager::sessionManager->CreateBackupConnection(bundleName);
}

void SvcSessionManager::Start() {}

void SvcSessionManager::Finish() {}

bool SvcSessionManager::IsOnAllBundlesFinished()
{
    return BSvcSessionManager::sessionManager->IsOnAllBundlesFinished();
}

bool SvcSessionManager::IsOnOnStartSched()
{
    return BSvcSessionManager::sessionManager->IsOnOnStartSched();
}

bool SvcSessionManager::NeedToUnloadService()
{
    return BSvcSessionManager::sessionManager->NeedToUnloadService();
}

int32_t SvcSessionManager::GetSessionUserId()
{
    return BSvcSessionManager::sessionManager->GetSessionUserId();
}

void SvcSessionManager::SetSessionUserId(int32_t) {}

void SvcSessionManager::SetBundleRestoreType(const std::string &, RestoreTypeEnum) {}

RestoreTypeEnum SvcSessionManager::GetBundleRestoreType(const std::string &bundleName)
{
    return BSvcSessionManager::sessionManager->GetBundleRestoreType(bundleName);
}

void SvcSessionManager::SetBundleVersionCode(const std::string &, int64_t) {}

int64_t SvcSessionManager::GetBundleVersionCode(const std::string &bundleName)
{
    return BSvcSessionManager::sessionManager->GetBundleVersionCode(bundleName);
}

void SvcSessionManager::SetBundleVersionName(const std::string &, std::string) {}

std::string SvcSessionManager::GetBundleVersionName(const std::string &bundleName)
{
    return BSvcSessionManager::sessionManager->GetBundleVersionName(bundleName);
}

void SvcSessionManager::SetBundleDataSize(const std::string &, int64_t) {}

bool SvcSessionManager::StartFwkTimer(const std::string &bundleName, const Utils::Timer::TimerCallback &callback)
{
    return BSvcSessionManager::sessionManager->StartFwkTimer(bundleName, callback);
}

bool SvcSessionManager::StopFwkTimer(const std::string &bundleName)
{
    return BSvcSessionManager::sessionManager->StopFwkTimer(bundleName);
}

bool SvcSessionManager::StartExtTimer(const std::string &bundleName, const Utils::Timer::TimerCallback &callback)
{
    return BSvcSessionManager::sessionManager->StartExtTimer(bundleName, callback);
}

bool SvcSessionManager::StopExtTimer(const std::string &bundleName)
{
    return BSvcSessionManager::sessionManager->StopExtTimer(bundleName);
}

bool SvcSessionManager::UpdateTimer(const std::string &bundleName, uint32_t timeOut,
    const Utils::Timer::TimerCallback &callback)
{
    return BSvcSessionManager::sessionManager->UpdateTimer(bundleName, timeOut, callback);
}

int32_t SvcSessionManager::GetMemParaCurSize()
{
    return BSvcSessionManager::sessionManager->GetMemParaCurSize();
}

void SvcSessionManager::SetMemParaCurSize(int32_t) {}

bool SvcSessionManager::GetIsIncrementalBackup()
{
    return BSvcSessionManager::sessionManager->GetIsIncrementalBackup();
}

bool SvcSessionManager::ValidRestoreDataType(RestoreTypeEnum restoreDataType)
{
    return BSvcSessionManager::sessionManager->ValidRestoreDataType(restoreDataType);
}

void SvcSessionManager::SetIncrementalData(const BIncrementalData &) {}

int32_t SvcSessionManager::GetIncrementalManifestFd(const string &bundleName)
{
    return BSvcSessionManager::sessionManager->GetIncrementalManifestFd(bundleName);
}

int64_t SvcSessionManager::GetLastIncrementalTime(const string &bundleName)
{
    return BSvcSessionManager::sessionManager->GetLastIncrementalTime(bundleName);
}

void SvcSessionManager::SetClearDataFlag(const std::string &, bool) {}

bool SvcSessionManager::GetClearDataFlag(const std::string &bundleName)
{
    return BSvcSessionManager::sessionManager->GetClearDataFlag(bundleName);
}

void SvcSessionManager::IncreaseSessionCnt(const std::string) {}

void SvcSessionManager::DecreaseSessionCnt(const std::string) {}

void SvcSessionManager::SetPublishFlag(const std::string&) {}

bool SvcSessionManager::CleanAndCheckIfNeedWait(ErrCode &ret, std::vector<std::string> &bundleNameList)
{
    return BSvcSessionManager::sessionManager->CleanAndCheckIfNeedWait(ret, bundleNameList);
}

ErrCode SvcSessionManager::ClearSessionData()
{
    return BSvcSessionManager::sessionManager->ClearSessionData();
}

int SvcSessionManager::GetSessionCnt()
{
    return BSvcSessionManager::sessionManager->GetSessionCnt();
}

SvcSessionManager::Impl SvcSessionManager::GetImpl()
{
    return BSvcSessionManager::sessionManager->GetImpl();
}

void SvcSessionManager::SetImplRestoreType(const RestoreTypeEnum restoreType) {}

void SvcSessionManager::SetIsReadyLaunch(const std::string&) {}

void SvcSessionManager::SetOldBackupVersion(const std::string &backupVersion) {}

std::string SvcSessionManager::GetOldBackupVersion()
{
    return BSvcSessionManager::sessionManager->GetOldBackupVersion();
}
} // namespace OHOS::FileManagement::Backup
