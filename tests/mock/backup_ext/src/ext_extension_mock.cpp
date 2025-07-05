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

ErrCode BackupExtExtension::GetIncrementalFileHandle(const string &fileName,
    int &fd, int &reportFd, int32_t &fdErrCode)
{
    return BExtExtension::extExtension->GetIncrementalFileHandle(fileName, fd, reportFd, fdErrCode);
}

ErrCode BackupExtExtension::HandleClear()
{
    return BExtExtension::extExtension->HandleClear();
}

ErrCode BackupExtExtension::BigFileReady(TarMap &bigFileInfo, sptr<IService> proxy, int backupedFileSize)
{
    return BExtExtension::extExtension->BigFileReady(bigFileInfo, proxy, backupedFileSize);
}

ErrCode BackupExtExtension::PublishFile(const std::string &fileName)
{
    return BExtExtension::extExtension->PublishFile(fileName);
}

ErrCode BackupExtExtension::PublishIncrementalFile(const string &fileName)
{
    return BExtExtension::extExtension->PublishIncrementalFile(fileName);
}

ErrCode BackupExtExtension::HandleBackup(bool isClearData)
{
    return BExtExtension::extExtension->HandleBackup(isClearData);
}

int BackupExtExtension::DoBackup(TarMap &bigFileInfo, TarMap &bigFileInfoBackuped,
    map<string, size_t> &smallFiles, uint32_t includesNum, uint32_t excludesNum)
{
    return BExtExtension::extExtension->DoBackup(bigFileInfo, bigFileInfoBackuped, smallFiles,
        includesNum, excludesNum);
}

int BackupExtExtension::DoRestore(const string &fileName, const off_t fileSize)
{
    return BExtExtension::extExtension->DoRestore(fileName, fileSize);
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

ErrCode BackupExtExtension::HandleRestore(bool isClearData)
{
    return BExtExtension::extExtension->HandleRestore(isClearData);
}

ErrCode BackupExtExtension::IncrementalBigFileReady(TarMap &pkgInfo,
    const vector<struct ReportFileInfo> &bigInfos, sptr<IService> proxy)
{
    return BExtExtension::extExtension->IncrementalBigFileReady(pkgInfo, bigInfos, proxy);
}

void BackupExtExtension::StartFwkTimer(bool &isFwkStart)
{
}

ErrCode BackupExtExtension::GetFileHandleWithUniqueFd(const std::string &fileName, int32_t &errCode, int& fd)
{
    return BExtExtension::extExtension->GetFileHandleWithUniqueFd(fileName, errCode, fd);
}

void BackupExtExtension::FillFileInfos(UniqueFd incrementalFd,
                                       UniqueFd manifestFd,
                                       vector<struct ReportFileInfo> &allFiles,
                                       vector<struct ReportFileInfo> &smallFiles,
                                       vector<struct ReportFileInfo> &bigFiles)
{
}

void BackupExtExtension::ReportAppStatistic(const std::string &func, ErrCode errCode)
{
}

void BackupExtExtension::UpdateOnStartTime()
{
}

ErrCode BackupExtExtension::IncrementalTarFileReady(const TarMap &bigFileInfo,
    const vector<struct ReportFileInfo> &srcFiles, sptr<IService> proxy)
{
    return BExtExtension::extExtension->IncrementalTarFileReady(bigFileInfo, srcFiles, proxy);
}

ErrCode BackupExtExtension::IncrementalAllFileReady(const TarMap &pkgInfo,
    const vector<struct ReportFileInfo> &srcFiles, sptr<IService> proxy)
{
    return BExtExtension::extExtension->IncrementalAllFileReady(pkgInfo, srcFiles, proxy);
}

std::function<void(std::string, int)> BackupExtExtension::ReportErrFileByProc(wptr<BackupExtExtension> obj,
    BackupRestoreScenario scenario)
{
    return BExtExtension::extExtension->ReportErrFileByProc(obj, scenario);
}

void BackupExtExtension::DoClearInner()
{
}

ErrCode BackupExtExtension::CleanBundleTempDir()
{
    return BExtExtension::extExtension->CleanBundleTempDir();
}
} // namespace OHOS::FileManagement::Backup
