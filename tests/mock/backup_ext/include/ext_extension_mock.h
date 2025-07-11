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

#ifndef OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_MOCK_H
#define OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_MOCK_H

#include <gmock/gmock.h>

#include "ext_extension.h"

namespace OHOS::FileManagement::Backup {
class BExtExtension {
public:
    virtual std::tuple<ErrCode, UniqueFd, UniqueFd> GetIncrementalFileHandle(const string &) = 0;
    virtual ErrCode GetIncrementalFileHandle(const string &fileName, int &fd,
        int &reportFd, int32_t &fdErrCode) = 0;
    virtual UniqueFd GetFileHandle(const string &, int32_t &) = 0;
    virtual ErrCode HandleClear() = 0;
    virtual ErrCode BigFileReady(TarMap &, sptr<IService>, int) = 0;
    virtual ErrCode PublishFile(const std::string &) = 0;
    virtual ErrCode PublishIncrementalFile(const string &) = 0;
    virtual ErrCode HandleBackup(bool) = 0;
    virtual int DoBackup(TarMap &, TarMap &, map<string, size_t> &, uint32_t, uint32_t) = 0;
    virtual int DoRestore(const string &, const off_t) = 0;
    virtual int DoIncrementalRestore() = 0;
    virtual void AsyncTaskBackup(const string) = 0;
    virtual void AsyncTaskRestore(std::set<std::string>, const std::vector<ExtManageInfo>) = 0;
    virtual void AsyncTaskIncrementalRestore() = 0;
    virtual void ExtClear() = 0;
    virtual void AsyncTaskIncrementalRestoreForUpgrade() = 0;
    virtual void DoClear() = 0;
    virtual void AppDone(ErrCode) = 0;
    virtual void AppResultReport(const std::string, BackupRestoreScenario, ErrCode) = 0;
    virtual void AsyncTaskOnBackup() = 0;
    virtual ErrCode HandleRestore(bool) = 0;
    virtual ErrCode IncrementalBigFileReady(TarMap&, const std::vector<struct ReportFileInfo>&,
        sptr<IService>) = 0;
    virtual ErrCode GetFileHandleWithUniqueFd(const std::string &fileName, int32_t &errCode, int& fd) = 0;
    virtual std::string GetBackupInfo() = 0;
    virtual void UpdateOnStartTime() = 0;
    virtual ErrCode IncrementalTarFileReady(const TarMap &bigFileInfo, const vector<struct ReportFileInfo> &srcFiles,
        sptr<IService> proxy) = 0;
    virtual void FillFileInfos(UniqueFd incrementalFd, UniqueFd manifestFd, vector<struct ReportFileInfo> &allFiles,
        vector<struct ReportFileInfo> &smallFiles, vector<struct ReportFileInfo> &bigFiles) = 0;
    virtual void ReportAppStatistic(const std::string &func, ErrCode errCode) = 0;
    virtual ErrCode IncrementalAllFileReady(const TarMap &pkgInfo, const vector<struct ReportFileInfo> &srcFiles,
        sptr<IService> proxy) = 0;
    virtual void VerifyCaller() = 0;
    virtual std::function<void(std::string, int)> ReportErrFileByProc(wptr<BackupExtExtension> obj,
        BackupRestoreScenario scenario) = 0;
    virtual void DoClearInner() = 0;
    virtual void StartFwkTimer(bool &isFwkStart) = 0;
    virtual ErrCode CleanBundleTempDir() = 0;
    virtual void AsyncTaskIncreRestoreSpecialVersion() = 0;
public:
    BExtExtension() = default;
    virtual ~BExtExtension() = default;
public:
    static inline std::shared_ptr<BExtExtension> extExtension = nullptr;
};

class ExtExtensionMock : public BExtExtension {
public:
    MOCK_METHOD(UniqueFd, GetFileHandle, (const string &, int32_t &));
    MOCK_METHOD(ErrCode, GetIncrementalFileHandle, (const string &fileName, int &fd,
        int &reportFd, int32_t &fdErrCode));
    MOCK_METHOD(ErrCode, HandleClear, ());
    MOCK_METHOD(ErrCode, BigFileReady, (TarMap &, sptr<IService>, int));
    MOCK_METHOD(ErrCode, PublishFile, (const std::string &));
    MOCK_METHOD(ErrCode, PublishIncrementalFile, (const string &));
    MOCK_METHOD(ErrCode, HandleBackup, (bool));
    MOCK_METHOD(int, DoBackup, (TarMap &, TarMap &, (map<string, size_t> &), uint32_t, uint32_t));
    MOCK_METHOD(int, DoRestore, (const string &, const off_t));
    MOCK_METHOD(int, DoIncrementalRestore, ());
    MOCK_METHOD(void, AsyncTaskBackup, (const string));
    MOCK_METHOD(void, AsyncTaskRestore, (std::set<std::string>, const std::vector<ExtManageInfo>));
    MOCK_METHOD(void, AsyncTaskIncrementalRestore, ());
    MOCK_METHOD(void, AsyncTaskIncreRestoreSpecialVersion, ());
    MOCK_METHOD(void, AsyncTaskRestoreForUpgrade, ());
    MOCK_METHOD(void, ExtClear, ());
    MOCK_METHOD(void, AsyncTaskIncrementalRestoreForUpgrade, ());
    MOCK_METHOD(void, DoClear, ());
    MOCK_METHOD(void, AppDone, (ErrCode));
    MOCK_METHOD(void, AppResultReport, (const std::string, BackupRestoreScenario, ErrCode));
    MOCK_METHOD(void, AsyncTaskOnBackup, ());
    MOCK_METHOD(ErrCode, HandleRestore, (bool));
    MOCK_METHOD(void, FillFileInfos, (UniqueFd, UniqueFd, (std::vector<struct ReportFileInfo>&),
        (std::vector<struct ReportFileInfo>&), (std::vector<struct ReportFileInfo>&)));
    MOCK_METHOD(ErrCode, HandleIncrementalBackup, (UniqueFd, UniqueFd));
    MOCK_METHOD(ErrCode, IncrementalOnBackup, ());
    MOCK_METHOD((std::tuple<UniqueFd, UniqueFd>), GetIncrementalBackupFileHandle, ());
    MOCK_METHOD(ErrCode, IncrementalBigFileReady, (TarMap&, const std::vector<struct ReportFileInfo>&,
        sptr<IService>));
    MOCK_METHOD(ErrCode, CleanBundleTempDir, ());
    MOCK_METHOD((std::tuple<ErrCode, UniqueFd, UniqueFd>), GetIncrementalFileHandle,
        (const string &));
    MOCK_METHOD(ErrCode, GetFileHandleWithUniqueFd, (const std::string &, int32_t &, int&));
    MOCK_METHOD(std::string, GetBackupInfo, ());
    MOCK_METHOD(void, UpdateOnStartTime, ());
    MOCK_METHOD(ErrCode, IncrementalTarFileReady, (const TarMap &, const vector<struct ReportFileInfo> &,
        sptr<IService>));
    MOCK_METHOD(void, ReportAppStatistic, (const std::string &, ErrCode));
    MOCK_METHOD(ErrCode, IncrementalAllFileReady, (const TarMap &, const vector<struct ReportFileInfo> &,
        sptr<IService>));
    MOCK_METHOD(void, VerifyCaller, ());
    MOCK_METHOD((std::function<void(std::string, int)>), ReportErrFileByProc, (wptr<BackupExtExtension>,
        BackupRestoreScenario));
    MOCK_METHOD(void, DoClearInner, ());
    MOCK_METHOD(void, StartFwkTimer, (bool &));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_MOCK_H