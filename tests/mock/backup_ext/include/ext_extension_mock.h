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
    virtual ErrCode GetIncrementalFileHandle(const string &) = 0;
    virtual UniqueFd GetFileHandle(const string &, int32_t &) = 0;
    virtual ErrCode HandleClear() = 0;
    virtual ErrCode BigFileReady(sptr<IService>) = 0;
    virtual ErrCode PublishFile(const std::string &) = 0;
    virtual ErrCode PublishIncrementalFile(const string &) = 0;
    virtual ErrCode HandleBackup() = 0;
    virtual int DoBackup(const BJsonEntityExtensionConfig &) = 0;
    virtual int DoRestore(const string &) = 0;
    virtual int DoIncrementalRestore() = 0;
    virtual void AsyncTaskBackup(const string) = 0;
    virtual void AsyncTaskRestore(std::set<std::string>, const std::vector<ExtManageInfo>) = 0;
    virtual void AsyncTaskIncrementalRestore() = 0;
    virtual void AsyncTaskIncreRestoreSpecialVersion() = 0;
    virtual void AsyncTaskRestoreForUpgrade() = 0;
    virtual void ExtClear() = 0;
    virtual void AsyncTaskIncrementalRestoreForUpgrade() = 0;
    virtual void DoClear() = 0;
    virtual void AppDone(ErrCode) = 0;
    virtual void AppResultReport(const std::string, BackupRestoreScenario, ErrCode) = 0;
    virtual void AsyncTaskOnBackup() = 0;
    virtual ErrCode HandleRestore() = 0;
    virtual void PreparaBackupFiles(UniqueFd, UniqueFd, std::vector<struct ReportFileInfo>&,
        std::vector<struct ReportFileInfo>&, std::vector<struct ReportFileInfo>&) = 0;
    virtual ErrCode HandleIncrementalBackup(UniqueFd, UniqueFd) = 0;
    virtual ErrCode IncrementalOnBackup() = 0;
    virtual tuple<UniqueFd, UniqueFd> GetIncrementalBackupFileHandle() = 0;
    virtual ErrCode IncrementalBigFileReady(const TarMap&, const std::vector<struct ReportFileInfo>&,
        sptr<IService>) = 0;
    virtual void AsyncTaskDoIncrementalBackup(UniqueFd, UniqueFd) = 0;
    virtual void AsyncTaskOnIncrementalBackup() = 0;
    virtual void IncrementalPacket(const std::vector<struct ReportFileInfo>&, TarMap&, sptr<IService>) = 0;
    virtual int DoIncrementalBackup(const std::vector<struct ReportFileInfo>&,
        const std::vector<struct ReportFileInfo>&, const std::vector<struct ReportFileInfo>&) = 0;
    virtual void AppIncrementalDone(ErrCode) = 0;
    virtual ErrCode GetBackupInfo(std::string&) = 0;
    virtual ErrCode UpdateFdSendRate(std::string&, int32_t) = 0;
    virtual std::function<void(ErrCode, std::string)> RestoreResultCallbackEx(wptr<BackupExtExtension>) = 0;
    virtual std::function<void(ErrCode, std::string)> AppDoneCallbackEx(wptr<BackupExtExtension>) = 0;
    virtual std::function<void(ErrCode, std::string)> IncRestoreResultCallbackEx(wptr<BackupExtExtension>) = 0;
    virtual std::function<void(ErrCode, const std::string)> HandleBackupEx(wptr<BackupExtExtension>) = 0;
    virtual std::function<void(ErrCode, const std::string)> HandleTaskBackupEx(wptr<BackupExtExtension>) = 0;
    virtual void WaitToSendFd(std::chrono::system_clock::time_point&, int&) = 0;
    virtual void RefreshTimeInfo(std::chrono::system_clock::time_point&, int&) = 0;
public:
    BExtExtension() = default;
    virtual ~BExtExtension() = default;
public:
    static inline std::shared_ptr<BExtExtension> extExtension = nullptr;
};

class ExtExtensionMock : public BExtExtension {
public:
    MOCK_METHOD(UniqueFd, GetFileHandle, (const string &, int32_t &));
    MOCK_METHOD(std::tuple<ErrCode, UniqueFd, UniqueFd>, GetIncrementalFileHandle, (const string &));
    MOCK_METHOD(ErrCode, HandleClear, ());
    MOCK_METHOD(ErrCode, BigFileReady, (sptr<IService>));
    MOCK_METHOD(ErrCode, PublishFile, (const std::string &));
    MOCK_METHOD(ErrCode, PublishIncrementalFile, (const string &));
    MOCK_METHOD(ErrCode, HandleBackup, ());
    MOCK_METHOD(int, DoBackup, (const BJsonEntityExtensionConfig &));
    MOCK_METHOD(int, DoRestore, (const string &));
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
    MOCK_METHOD(ErrCode, HandleRestore, ());
    MOCK_METHOD(void, FillFileInfos, (UniqueFd, UniqueFd, (std::vector<struct ReportFileInfo>&),
        (std::vector<struct ReportFileInfo>&), (std::vector<struct ReportFileInfo>&)));
    MOCK_METHOD(ErrCode, HandleIncrementalBackup, (UniqueFd, UniqueFd));
    MOCK_METHOD(ErrCode, IncrementalOnBackup, ());
    MOCK_METHOD((std::tuple<UniqueFd, UniqueFd>), GetIncrementalBackupFileHandle, ());
    MOCK_METHOD(ErrCode, IncrementalBigFileReady, (const TarMap&, (const std::vector<struct ReportFileInfo>&),
        sptr<IService>));
    MOCK_METHOD(void, AsyncTaskDoIncrementalBackup, (UniqueFd, UniqueFd));
    MOCK_METHOD(void, AsyncTaskOnIncrementalBackup, ());
    MOCK_METHOD(void, IncrementalPacket, ((const std::vector<struct ReportFileInfo>&), TarMap&, sptr<IService>));
    MOCK_METHOD(int, DoIncrementalBackup, ((const std::vector<struct ReportFileInfo>&),
        (const std::vector<struct ReportFileInfo>&), (const std::vector<struct ReportFileInfo>&)));
    MOCK_METHOD(void, AppIncrementalDone, (ErrCode));
    MOCK_METHOD(ErrCode, GetBackupInfo, (std::string&));
    MOCK_METHOD(ErrCode, UpdateFdSendRate, (std::string&, int32_t));
    MOCK_METHOD((std::function<void(ErrCode, std::string)>), RestoreResultCallbackEx, (wptr<BackupExtExtension>));
    MOCK_METHOD((std::function<void(ErrCode, std::string)>), AppDoneCallbackEx, (wptr<BackupExtExtension>));
    MOCK_METHOD((std::function<void(ErrCode, std::string)>), IncRestoreResultCallbackEx, (wptr<BackupExtExtension>));
    MOCK_METHOD((std::function<void(ErrCode, const std::string)>), HandleBackupEx, (wptr<BackupExtExtension>));
    MOCK_METHOD((std::function<void(ErrCode, const std::string)>), HandleTaskBackupEx, (wptr<BackupExtExtension>));
    MOCK_METHOD(void, WaitToSendFd, ((std::chrono::system_clock::time_point&), int&));
    MOCK_METHOD(void, RefreshTimeInfo, ((std::chrono::system_clock::time_point&), int&));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_MOCK_H