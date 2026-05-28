/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_MIGRATE_MANAGER_H
#define OHOS_FILEMGMT_BACKUP_MIGRATE_MANAGER_H

#include <chrono>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_set>
#include <cstdint>
#include <functional>

#include <sys/stat.h>

#include "anco_backup_callback_stub.h"
#include "b_json/b_json_entity_extension_config.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_json/b_report_entity.h"
#include "b_radar/b_radar.h"
#include "b_radar/radar_app_statistic.h"
#include "thread_pool.h"
#include "ipc_skeleton.h"

#include "module_ipc/enhance_service_manager.h"


namespace OHOS::FileManagement::Backup {
const string INDEX_FILE_BACKUP = string(BConstants::PATH_BUNDLE_BACKUP_HOME).
                                 append(BConstants::SA_BUNDLE_BACKUP_BACKUP).
                                 append(BConstants::EXT_BACKUP_MANAGE);
enum BackupType {
    FULL_BACKUP = 0,
    FULL_RESTORE = 1,
    INCREMENTAL_RESTORE = 2,
};
class Service;
class MigrateManager : public RefBase {
public:
    explicit MigrateManager(wptr<Service> servicePtr, const std::string &bundleName, int32_t userId)
    {
        servicePtr_ = servicePtr;
        bundleName_ = bundleName;
        userId_ = userId;
        threadPool_.Start(BConstants::EXTENSION_THREAD_POOL_COUNT);
        appStatistic_ = std::make_shared<RadarAppStatistic>();
        doBackupPool_.Start(BConstants::EXTENSION_THREAD_POOL_COUNT);
    }
    ~MigrateManager()
    {
        threadPool_.Stop();
        doBackupPool_.Stop();
    }
    void DoClear();
    void DoClearInner();
    void AppDone(ErrCode errCode, const std::string &bundleName);
    void ReportAppStatistic(const std::string &func, ErrCode errCode);
    ErrCode VerifyCallerAndGetCallerName(const std::string &bundleName);
    void UpdateFileStat(std::string filePath, uint64_t fileSize);
    void HandleExtOnRelease(bool isAppResultReport, ErrCode errCode);
    ErrCode HandleExtOnDisconnect(BackupType scenario, bool isAppResultReport, ErrCode errCode);
    void HandleCurBundleEndWork(std::string bundleName, const BackupType scenario);
    ErrCode GetExtOnRelease(bool &isExtOnRelease);
    std::shared_ptr<ScanResultManager> GetScanInstance(const string &bundleName);
    void SetDefaultAppTimer(int64_t &appSize, const string &bundleName);
// 备份逻辑
    void WaitToSendFd(std::chrono::system_clock::time_point &startTime, int &fdSendNum);
    ErrCode UpdateFdSendRate(const std::string &bundleName, int32_t sendRate);
    ErrCode HandleBackup(bool isClearData, const std::string &bundleName);
    void AsyncTaskBackup(const std::string &bundleName);
    void ScanAllDirsTask(const std::string &bundleName);
    void UpdateOnStartTime();
    void SetScanTotalSize(int64_t totalSize);
    int64_t GetScanTotalSize();
    void AsyncDoBackup(const std::string &bundleName);
    void DoBackupTask(const std::string &bundleName);
    ErrCode IndexFileReady(const std::string &bundleName);
    ErrCode ReportAppFileReady(const string &bundleName, const string& filename,
        const string& filePath, bool needDelete = false);
    void DoPacket(const std::string &bundleName);
    ErrCode ScanAllDirs(int64_t &totalSize, const std::string &bundleName);
// 恢复逻辑
    ErrCode PublishIncrementalFile(const string &fileName);
    void AsyncTaskIncrementalRestore();
    int DealIncreRestoreBigAndTarFile();
    int DoIncrementalRestore();
    void RestoreBigFiles(bool appendTargetPath);
    void AppIncrementalDone(ErrCode errCode);
    ErrCode GetIncrementalFileHandle(const std::string &fileName, UniqueFd &fd, UniqueFd &reportFd, int32_t &fdErrCode);
    tuple<ErrCode, UniqueFd, UniqueFd> GetIncrementalFileHandle(const string &fileName);
    ErrCode HandleRestore(bool isClearData);
    tuple<ErrCode, UniqueFd, UniqueFd> GetIncreFileHandleForNormalVersion(const std::string &fileName);
    ErrCode CreateDefaultTask(const std::string &bundleName);
    void CloseFileWithFDSan(int fd);
    ErrCode DealIncrementalDone(ErrCode errCode);
    ErrCode HandleCurAppDone(ErrCode errCode, const std::string &bundleName, bool isIncBackup);
public:
    std::shared_ptr<RadarAppStatistic> appStatistic_ = nullptr;
    std::map<std::string, std::shared_ptr<ScanResultManager>> instanceMap_;
private:
    wptr<Service> servicePtr_;
    OHOS::ThreadPool threadPool_;
    OHOS::ThreadPool doBackupPool_;

    bool isClearData_ {true};
    bool isDebug_ {true};
    int32_t userId_ = 0;
    AppRadar::DoRestoreInfo radarRestoreInfo_ { 0 };
    std::chrono::time_point<std::chrono::system_clock> g_onStart;
    std::string bundleName_;
    int32_t sendRate_ = BConstants::DEFAULT_FD_SEND_RATE;
// lock
    std::mutex onStartTimeLock_;
    std::mutex scanSizeLock_;
    std::mutex updateFileStatLock_;
    std::mutex onReleaseLock_;
    std::mutex bundleEndLock_;
    std::mutex updateSendRateLock_;
    std::condition_variable startSendFdRateCon_;
    std::condition_variable waitSendFdCon_;
    std::mutex startSendMutex_;
    std::mutex scanInstanceLock_;
// 统计数据
    int64_t scanTotalSize_ = 0;
    set<std::string> fileNames_ = {};

    BackupType curScenario_ { BackupType::FULL_BACKUP };
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_MIGRATE_MANAGER_H