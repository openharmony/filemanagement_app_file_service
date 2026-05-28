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

#include <directory_ex.h>
#include <sys/stat.h>
#include <unistd.h>

#include "module_migrate_manager/migrate_manager.h"
#include "service_client.h"

#include "b_anony/b_anony.h"
#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_filesystem/b_dir.h"
#include "b_filesystem/b_file.h"
#include "b_filesystem/b_file_hash.h"
#include "b_json/b_json_cached_entity.h"
#include "b_jsonutil/b_jsonutil.h"
#include "b_ohos/startup/backup_para.h"
#include "b_tarball/b_tarball_factory.h"
#include "b_resources/b_constants.h"
#include "b_utils/b_time.h"
#include "b_utils/scan_file_singleton.h"
#include "b_utils/string_utils.h"
#include "filemgmt_libhilog.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {

static void RecordDoRestoreRes(const std::string &bundleName, const std::string &func,
    AppRadar::DoRestoreInfo &restoreInfo)
{
    std::stringstream ss;
    ss << R"("bigFileNums": )" << restoreInfo.bigFileNum << ", ";
    ss << R"("bigFileSize": )" << restoreInfo.bigFileSize << ", ";
    ss << R"("RestoreBigFileTime": )" << restoreInfo.bigFileSpendTime << ", ";
    ss << R"("unTarFileNums": )" << restoreInfo.tarFileNum << ", ";
    ss << R"("unTarFileSize": )" << restoreInfo.tarFileSize << ", ";
    ss << R"("unTarTime": )" << restoreInfo.tarFileSpendTime << ", ";
    ss << R"("totalFileNum": )" << restoreInfo.bigFileNum + restoreInfo.tarFileNum << ", ";
    ss << R"("totalFileSize": )" << restoreInfo.bigFileSize + restoreInfo.tarFileSize << ", ";
    ss << R"("restoreAllFileTime": )" << restoreInfo.totalFileSpendTime;
    int32_t err = static_cast<int32_t>(BError::Codes::OK);
    AppRadar::Info info (bundleName, "", ss.str());
    AppRadar::GetInstance().RecordRestoreFuncRes(info, func, AppRadar::GetInstance().GetUserId(),
        BizStageRestore::BIZ_STAGE_DO_RESTORE, err);
}

ErrCode MigrateManager::HandleRestore(bool isClearData)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        isClearData_ = isClearData;
        curScenario_ = BackupType::FULL_RESTORE;
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to handle restore");
        return BError(BError::Codes::EXT_BROKEN_IPC).GetCode();
    }
}

ErrCode MigrateManager::GetIncrementalFileHandle(const std::string &fileName,
    UniqueFd &fd, UniqueFd &reportFd, int32_t &fdErrCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto [errCode, fdval, reportFdVal] = GetIncrementalFileHandle(fileName);
    fdErrCode = errCode;
    fd = move(fdval);
    reportFd = move(reportFdVal);
    return ERR_OK;
}

tuple<ErrCode, UniqueFd, UniqueFd> MigrateManager::GetIncrementalFileHandle(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (!BDir::IsFilePathValid(fileName)) {
            if (servicePtr_ == nullptr) {
                throw BError(BError::Codes::EXT_BROKEN_IPC, string("Failed to AGetInstance"));
            }
            HILOGE("Check file path : %{public}s err, path is forbidden", GetAnonyPath(fileName).c_str());
            auto ret = servicePtr_->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG));
            if (ret != ERR_OK) {
                HILOGE("Failed to notify app incre done. err = %{public}d", ret);
            }
            return {BError(BError::Codes::EXT_INVAL_ARG).GetCode(), UniqueFd(-1), UniqueFd(-1)};
        }
        return GetIncreFileHandleForNormalVersion(fileName);
    } catch (...) {
        HILOGE("Failed to get incremental file handle");
        DoClear();
        return {BError(BError::Codes::EXT_BROKEN_IPC).GetCode(), UniqueFd(-1), UniqueFd(-1)};
    }
}

tuple<ErrCode, UniqueFd, UniqueFd> MigrateManager::GetIncreFileHandleForNormalVersion(const std::string &fileName)
{
    HILOGI("extension: GetIncrementalFileHandle single to single Name:%{public}s", GetAnonyPath(fileName).c_str());
    int32_t errCode = ERR_OK;
    UniqueFd fd (BConstants::INVALID_FD_NUM);
    UniqueFd reportFd(BConstants::INVALID_FD_NUM);

    string scanResult;
    ErrCode ret = ERR_OK;

    auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
    if (!enhanceService) {
        HILOGW("enhance service is not loaded");
        return {ret, UniqueFd(INVALID_FD), UniqueFd(INVALID_FD)};
    }
    errCode = enhanceService->DefaultAppFileHandle(bundleName_, fileName, fd, reportFd);
    if (errCode != 0) {
        HILOGE("GetFileHandle fail err:%{public}d", errCode);
    }
    return {errCode, move(fd), move(reportFd)};
}

ErrCode MigrateManager::PublishIncrementalFile(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("begin publish incremental file. fileName is %{private}s", fileName.data());
    try {
        // 异步执行解压操作
        if (BackupPara::GetBackupDebugState()) {
            isDebug_ = true;
        }
        curScenario_ = BackupType::INCREMENTAL_RESTORE;
        HILOGI("Create task for Incremental Restore");
        AsyncTaskIncrementalRestore();
        HILOGI("End publish incremental file");
        return ERR_OK;
    } catch (const BError &e) {
        DoClear();
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        DoClear();
        return BError(BError::Codes::EXT_BROKEN_FRAMEWORK).GetCode();
    } catch (...) {
        HILOGE("Unexpected exception");
        DoClear();
        return BError(BError::Codes::EXT_BROKEN_FRAMEWORK).GetCode();
    }
}

void MigrateManager::AsyncTaskIncrementalRestore()
{
    auto task = [obj {wptr<MigrateManager>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        try {
            int ret = ptr->DealIncreRestoreBigAndTarFile();
            ptr->AppIncrementalDone(ret);
            ptr->DoClear();
        } catch (const BError &e) {
            ptr->AppIncrementalDone(e.GetCode());
        } catch (const exception &e) {
            HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
            ptr->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        }
    };
    // REM: 这里异步化了，需要做并发控制
    // 在往线程池中投入任务之前将需要的数据拷贝副本到参数中，保证不发生读写竞争，
    // 由于拷贝参数时尚运行在主线程中，故在参数拷贝过程中是线程安全的。
    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

int MigrateManager::DealIncreRestoreBigAndTarFile()
{
    auto startTime = std::chrono::system_clock::now();
    // 解压
    int ret = CreateDefaultTask(bundleName_);
    ret = DoIncrementalRestore();
    if (ret != ERR_OK) {
        HILOGE("Do incremental restore err");
        return ret;
    }

    RestoreBigFiles(true);
    
    auto endTime = std::chrono::system_clock::now();
    radarRestoreInfo_.totalFileSpendTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    RecordDoRestoreRes(bundleName_, "BackupExtExtension::AsyncTaskIncrementalRestore",
        radarRestoreInfo_);
    return ret;
}

int MigrateManager::DoIncrementalRestore()
{
    HILOGI("Do incremental restore");
    auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
    if (!enhanceService) {
        HILOGW("enhance service is not loaded");
        return BError(BError::Codes::OK);
    }
    auto startTime = std::chrono::system_clock::now();
    ret = enhanceService->StartDefaultAppUnPack(bundleName_);

    ErrCode err = ERR_OK;
    auto endTime = std::chrono::system_clock::now();
    radarRestoreInfo_.tarFileSpendTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    return err;
}

void MigrateManager::RestoreBigFiles(bool appendTargetPath)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    // 获取索引文件内容

    auto start = std::chrono::system_clock::now();
    auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
    if (!enhanceService) {
        HILOGW("enhance service is not loaded");
        return;
    }
    ret = enhanceService->DefaultAppRestoreBigFiles(bundleName_, appendTargetPath);
    auto end = std::chrono::system_clock::now();
    radarRestoreInfo_.bigFileSpendTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    HILOGI("End Restore Big Files");
}

void MigrateManager::AppIncrementalDone(ErrCode errCode)
{
    HILOGI("Begin");
    ReportAppStatistic("AppIncrementalDone", errCode);
    if (servicePtr_ == nullptr) {
        HILOGE("Failed to obtain the servicePtr_");
        DoClear();
        return;
    }
    auto ret = DealIncrementalDone(errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed to notify the app done. err = %{public}d", ret);
    }
    bool isExtOnRelease = false;
    ret = GetExtOnRelease(isExtOnRelease);
    if (ret != ERR_OK) {
        HILOGE("Failed to GetExtOnRelease err = %{public}d", ret);
    }
    if (isExtOnRelease) {
        HandleExtOnRelease(false, errCode);
    }
}

ErrCode MigrateManager::DealIncrementalDone(ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (servicePtr_->session_ == nullptr) {
            HILOGE("AppIncrementalDone error, session is null");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        HILOGI("Service AppIncrementalDone start bundlename is %{public}s, errCode is :%{public}d",
            bundleName);
        if (servicePtr_->session_->OnBundleFileReady(bundleName_) || errCode != BError(BError::Codes::OK)) {
            servicePtr_->SetExtOnRelease(bundleName_, true);
            return BError(BError::Codes::OK);
        }
        servicePtr_->RemoveExtensionMutex(bundleName_);
        servicePtr_->OnAllBundlesFinished(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        servicePtr_->ReleaseOnException();
        HILOGI("AppIncrementalDone error, err code is:%{public}d", e.GetCode());
        return e.GetCode();
    } catch (...) {
        servicePtr_->ReleaseOnException();
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

void MigrateManager::GetFileHandleWithUniqueFds(const string &bundleName, std::set<std::string> &fileNameVec)
{
    for (const auto &fileName : fileNameVec) {
        auto err = servicePtr_->SendIncrementalFileHandle(bundleName, fileName);
        if (err != ERR_OK) {
            HILOGE("SendIncrementalFileHandle failed code: %{public}d", err);
        }
    }
}

ErrCode MigrateManager::GetFileHandleWithUniqueFd(const std::string &fileName,
    int32_t &getFileHandleErrCode, int &fd)
{
    UniqueFd fileHandleFd(GetFileHandle(fileName, getFileHandleErrCode));
    fd = dup(fileHandleFd.Get());
    return ERR_OK;
}

UniqueFd MigrateManager::GetFileHandle(const string &fileName, int32_t &errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        UniqueFd tarFd(-1);

        return tarFd;
    } catch (...) {
        HILOGE("Failed to get file handle");
        DoClear();
        errCode = -1;
        return UniqueFd(-1);
    }
}

void MigrateManager::DoClear()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (!isClearData_) {
            HILOGI("configured not clear data.");
            return;
        }
        DoClearInner();
    } catch (...) {
        HILOGE("Failed to clear");
    }
}

void MigrateManager::DoClearInner()
{
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName);
        if (ret != ERR_OK) {
            HILOGE("error, Get bundle name failed, ret:%{public}d", ret);
            return;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("enhance service is not loaded");
            return;
        }
        enhanceService->StartDefaultAppClear(bundleName_);
        enhanceService->DestroyDefaultTask(bundleName_);
    } catch (...) {
        HILOGE("Failed to clear");
    }
}

void MigrateManager::AppDone(ErrCode errCode, const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("AppDone Begin.");
    ReportAppStatistic("AppDone", errCode);
    if (servicePtr_ == nullptr) {
        HILOGE("Failed to obtain the ServiceClient handle");
        DoClear();
        return;
    }
    auto ret = servicePtr_->AppDone(errCode, bundleName);
    if (ret != ERR_OK) {
        HILOGE("Failed to notify the app done. err = %{public}d", ret);
    }
    bool isExtOnRelease = false;
    ret = GetExtOnRelease(isExtOnRelease)
    if (ret != ERR_OK) {
        HILOGE("Failed to GetExtOnRelease err = %{public}d", ret);
    }
    if (isExtOnRelease) {
        HandleExtOnRelease(false, errCode);
    }
}

void MigrateManager::ReportAppStatistic(const std::string &func, ErrCode errCode)
{
    if (curScenario_ == BackupType::FULL_BACKUP) {
        appStatistic_->ReportBackup(func, RadarError(MODULE_BACKUP, errCode).GenCode());
    } else {
        appStatistic_->untarSpend_ = static_cast<uint32_t>(radarRestoreInfo_.tarFileSpendTime);
        appStatistic_->bigFileSpend_ = static_cast<uint32_t>(radarRestoreInfo_.bigFileSpendTime);
        appStatistic_->doRestoreSpend_ = static_cast<uint32_t>(radarRestoreInfo_.totalFileSpendTime);
        appStatistic_->bigFileCount_ = radarRestoreInfo_.bigFileNum;
        appStatistic_->tarFileCount_ = radarRestoreInfo_.tarFileNum;
        appStatistic_->bigFileSize_ = radarRestoreInfo_.bigFileSize;
        appStatistic_->tarFileSize_ = radarRestoreInfo_.tarFileSize;
        appStatistic_->ReportRestore(func, RadarError(MODULE_RESTORE, errCode).GenCode());
    }
}

ErrCode MigrateManager::GetExtOnRelease(bool &isExtOnRelease)
{
    std::shared_lock<std::shared_mutex> lock(onReleaseLock_);
    auto ret = ERR_OK;
    auto it = servicePtr_->backupExtOnReleaseMap_.find(bundleName_);
    if (it == servicePtr_->backupExtOnReleaseMap_.end()) {
        HILOGI("BackupExtOnReleaseMap not contain %{public}s", bundleName_.c_str());
        servicePtr_->backupExtOnReleaseMap_[bundleName_] = false;
        isExtOnRelease = servicePtr_->backupExtOnReleaseMap_[bundleName_].load();
        return ret;
    }
    HILOGI("BackupExtOnReleaseMap contain %{public}s", bundleName_.c_str());
    isExtOnRelease = servicePtr_->backupExtOnReleaseMap_[bundleName_].load();
    return ret;
}

void MigrateManager::HandleExtOnRelease(bool isAppResultReport, ErrCode errCode)
{
    HILOGI("HandleExtOnRelease begin");
    int32_t scenario = static_cast<int32_t>(BConstants::ExtensionScenario::RESTORE);
    auto ret = HandleExtDisconnect(curScenario_, isAppResultReport, errCode);
    if (ret != ERR_OK) {
        HILOGI("failed to HandleExtOnRelease, err = %{public}d", ret);
    }
}

void MigrateManager::HandleExtDisconnect(BackupType scenario, bool isAppResultReport, ErrCode errCode)
{
    try {
        HILOGI("Begin, scenario:%{public}d, isAppResultReport:%{public}d, errCode:%{public}d", scenario,
            isAppResultReport, errCode);
        std::string callerName;
        auto ret = VerifyCallerAndGetCallerName(callerName);
        if (ret != ERR_OK) {
            HILOGE("HandleExtDisconnect VerifyCaller failed, get bundle failed, ret:%{public}d", ret);
            if (isAppResultReport) {
                HandleCurBundleEndWork(callerName, scenario);
                servicePtr_->OnAllBundlesFinished(BError(BError::Codes::OK));
            }
            return ret;
        }
        if (isAppResultReport && (scenario == BackupType::FULL_RESTORE ||
            scenario == BackupType::INCREMENTAL_RESTORE)) {
            HandleCurBundleEndWork(bundleName_, scenario);
            servicePtr_->OnAllBundlesFinished(BError(BError::Codes::OK));
        } else if (!isAppResultReport) {
            bool isIncBackup = true;
            if (scenario == BackupType::FULL_BACKUP || scenario == BackupType::FULL_RESTORE) {
                isIncBackup = false;
            }
            std::lock_guard<std::mutex> lock(bundleEndLock_);
            ret = HandleCurAppDone(errCode, bundleName_, isIncBackup);
            if (ret != ERR_OK) {
                HILOGE("Handle current app done error, bundleName:%{public}s", bundleName_.c_str());
                return ret;
            }
            servicePtr_->RemoveExtensionMutex(bundleName_);
            servicePtr_->RemoveExtOnRelease(bundleName_);
            servicePtr_->OnAllBundlesFinished(BError(BError::Codes::OK));
        }
        return ret;
    } catch (const BError &e) {
        HILOGE("AppIncrementalFileReady exception");
        return e.GetCode();
    } catch (...) {
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

void MigrateManager::HandleCurBundleEndWork(std::string bundleName, const BackupType scenario)
{
    HILOGI("Begin");
    try {
        if (scenario != BackupType::FULL_RESTORE &&
            scenario != BackupType::INCREMENTAL_RESTORE) {
            return;
        }
        if (servicePtr_->session_->OnBundleFileReady(bundleName)) {
            std::lock_guard<std::mutex> lock(bundleEndLock_);
            servicePtr_->session_->StopFwkTimer(bundleName);
            servicePtr_->session_->StopExtTimer(bundleName);
            ClearSessionAndSchedInfo(bundleName);
        }
        servicePtr_->RemoveExtensionMutex(bundleName);
    } catch (...) {
        HILOGE("Unexpected exception");
    }
}

ErrCode MigrateManager::HandleCurAppDone(ErrCode errCode, const std::string &bundleName, bool isIncBackup)
{
    servicePtr_->session_->StopFwkTimer(bundleName);
    servicePtr_->session_->StopExtTimer(bundleName);
    servicePtr_->SendEndAppGalleryNotify(bundleName);
    servicePtr_->ClearSessionAndSchedInfo(bundleName);
    if (isIncBackup) {
        HILOGI("Incremental backup or restore app done, bundleName:%{public}s, errCode:%{public}d",
            bundleName.c_str(), errCode);
        servicePtr_->NotifyCallerCurAppIncrementDone(errCode, bundleName);
    } else {
        HILOGI("Full backup or restore app done, bundleName:%{public}s, errCode:%{public}d",
            bundleName.c_str(), errCode);
        servicePtr_->NotifyCallerCurAppDone(errCode, bundleName);
    }
    return BError(BError::Codes::OK);
}
}
