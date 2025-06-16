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

#include "ext_extension.h"
#include "ext_extension_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
void BackupExtExtension::VerifyCaller()
{
}

UniqueFd BackupExtExtension::GetFileHandle(const string &fileName, int32_t &errCode)
{
    return BExtExtension::extExtension->GetFileHandle(fileName, errCode);
}

ErrCode BackupExtExtension::GetIncrementalFileHandle(const string &fileName)
{
    return BExtExtension::extExtension->GetIncrementalFileHandle(fileName);
}

ErrCode BackupExtExtension::HandleClear()
{
    return BExtExtension::extExtension->HandleClear();
}

ErrCode BackupExtExtension::BigFileReady(sptr<IService> proxy)
{
    return BExtExtension::extExtension->BigFileReady(proxy);
}

ErrCode BackupExtExtension::PublishFile(const std::string &fileName)
{
    return BExtExtension::extExtension->PublishFile(fileName);
}

ErrCode BackupExtExtension::PublishIncrementalFile(const string &fileName)
{
    return BExtExtension::extExtension->PublishIncrementalFile(fileName);
}

ErrCode BackupExtExtension::HandleBackup()
{
    return BExtExtension::extExtension->HandleBackup();
}

int BackupExtExtension::DoBackup(const BJsonEntityExtensionConfig &usrConfig)
{
    return BExtExtension::extExtension->DoBackup(usrConfig);
}

int BackupExtExtension::DoRestore(const string &fileName)
{
    return BExtExtension::extExtension->DoRestore(fileName);
}

int BackupExtExtension::DoIncrementalRestore()
{
    return BExtExtension::extExtension->DoIncrementalRestore();
}

void BackupExtExtension::AsyncTaskBackup(const string config)
{
}

void BackupExtExtension::AsyncTaskRestore(std::set<std::string> fileSet,
    const std::vector<ExtManageInfo> extManageInfo)
{
}

void BackupExtExtension::AsyncTaskIncrementalRestore()
{
}

void BackupExtExtension::AsyncTaskIncreRestoreSpecialVersion()
{
}

void BackupExtExtension::AsyncTaskRestoreForUpgrade()
{
}

void BackupExtExtension::ExtClear()
{
}

void BackupExtExtension::AsyncTaskIncrementalRestoreForUpgrade()
{
}

void BackupExtExtension::DoClear()
{
}

void BackupExtExtension::AppDone(ErrCode errCode)
{
}

void BackupExtExtension::AppResultReport(const std::string restoreRetInfo,
    BackupRestoreScenario scenario, ErrCode errCode)
{
}

void BackupExtExtension::AsyncTaskOnBackup()
{
}

ErrCode BackupExtExtension::HandleRestore()
{
    return BExtExtension::extExtension->HandleRestore();
}

void BackupExtExtension::PreparaBackupFiles(UniqueFd incrementalFd,
                                            UniqueFd manifestFd,
                                            vector<struct ReportFileInfo> &allFiles,
                                            vector<struct ReportFileInfo> &smallFiles,
                                            vector<struct ReportFileInfo> &bigFiles)
{
}

ErrCode BackupExtExtension::HandleIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd)
{
    return BExtExtension::extExtension->HandleIncrementalBackup(std::move(incrementalFd), std::move(manifestFd));
}

ErrCode BackupExtExtension::IncrementalOnBackup()
{
    return BExtExtension::extExtension->IncrementalOnBackup();
}

tuple<UniqueFd, UniqueFd> BackupExtExtension::GetIncrementalBackupFileHandle()
{
    return BExtExtension::extExtension->GetIncrementalBackupFileHandle();
}

ErrCode BackupExtExtension::IncrementalBigFileReady(const TarMap &pkgInfo,
    const vector<struct ReportFileInfo> &bigInfos, sptr<IService> proxy)
{
    return BExtExtension::extExtension->IncrementalBigFileReady(pkgInfo, bigInfos, proxy);
}

void BackupExtExtension::AsyncTaskDoIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd)
{
}

void BackupExtExtension::AsyncTaskOnIncrementalBackup()
{
}

void BackupExtExtension::IncrementalPacket(const vector<struct ReportFileInfo> &, TarMap &, sptr<IService>)
{
}

int BackupExtExtension::DoIncrementalBackup(const vector<struct ReportFileInfo> &allFiles,
                                            const vector<struct ReportFileInfo> &smallFiles,
                                            const vector<struct ReportFileInfo> &bigFiles)
{
    return BExtExtension::extExtension->DoIncrementalBackup(allFiles, smallFiles, bigFiles);
}

void BackupExtExtension::AppIncrementalDone(ErrCode errCode)
{
}

ErrCode BackupExtExtension::GetBackupInfo(std::string &result)
{
    return BExtExtension::extExtension->GetBackupInfo(result);
}

ErrCode BackupExtExtension::UpdateFdSendRate(std::string &bundleName, int32_t sendRate)
{
    return BExtExtension::extExtension->UpdateFdSendRate(bundleName, sendRate);
}

std::function<void(ErrCode, std::string)> BackupExtExtension::RestoreResultCallbackEx(wptr<BackupExtExtension> obj)
{
    return BExtExtension::extExtension->RestoreResultCallbackEx(obj);
}

std::function<void(ErrCode, std::string)> BackupExtExtension::AppDoneCallbackEx(wptr<BackupExtExtension> obj)
{
    return BExtExtension::extExtension->AppDoneCallbackEx(obj);
}

std::function<void(ErrCode, std::string)> BackupExtExtension::IncRestoreResultCallbackEx(wptr<BackupExtExtension> obj)
{
    return BExtExtension::extExtension->IncRestoreResultCallbackEx(obj);
}

std::function<void(ErrCode, const std::string)> BackupExtExtension::HandleBackupEx(wptr<BackupExtExtension> obj)
{
    return BExtExtension::extExtension->HandleBackupEx(obj);
}

std::function<void(ErrCode, const std::string)> BackupExtExtension::HandleTaskBackupEx(wptr<BackupExtExtension> obj)
{
    return BExtExtension::extExtension->HandleTaskBackupEx(obj);
}

void BackupExtExtension::WaitToSendFd(std::chrono::system_clock::time_point &startTime, int &fdSendNum)
{
}

void BackupExtExtension::RefreshTimeInfo(std::chrono::system_clock::time_point &startTime, int &fdSendNum)
{
}

ErrCode BackupExtExtension::CleanBundleTempDir()
{
    return BExtExtension::extExtension->CleanBundleTempDir();
}

ErrCode BackupExtExtension::HandleOnRelease(int32_t scenario)
{
    return BExtExtension::extExtension->HandleOnRelease(scenario);
}
} // namespace OHOS::FileManagement::Backup
