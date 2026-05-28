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
#include <cinttypes>

#include "module_migrate_manager/migrate_manager.h"
#include "module_ipc/enhance_service_manager.h"
#include "module_ipc/service.h"

#include "accesstoken_kit.h"
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
#include "b_hiaudit/hi_audit.h"
#include "b_utils/b_time.h"
#include "b_utils/scan_file_singleton.h"
#include "b_utils/string_utils.h"
#include "filemgmt_libhilog.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {

const uint32_t MAX_FD_GROUP_USE_TIME = 1000; // 每组打开最大时间1000ms

static bool ReplaceBundleName(string &srcPath, const string &bundleName)
{
    std::string bundleBase = BConstants::PATH_BASE.data();
    bundleBase.append(BConstants::BACKSLASH);
    bundleBase.append(bundleName);

    std::string bundleDataBase = BConstants::PATH_DATABASE.data();
    bundleBase.append(BConstants::BACKSLASH);
    bundleBase.append(bundleName);

    std::string bundleDistributeBase = BConstants::PATH_DISTRIBUTE.data();
    bundleBase.append(BConstants::BACKSLASH);
    bundleBase.append(bundleName);

    auto pos = srcPath.find(BConstants::PATH_BASE);
    if (pos != string::npos) {
        srcPath.replace(pos, BConstants::PATH_BASE.lenth(), bundleBase);
        return true;
    }
    pos = srcPath.find(BConstants::PATH_DATABASE);
    if (pos != string::npos) {
        srcPath.replace(pos, BConstants::PATH_DATABASE.lenth(), bundleBase);
        return true;
    }
    pos = srcPath.find(BConstants::PATH_DISTRIBUTE);
    if (pos != string::npos) {
        srcPath.replace(pos, BConstants::PATH_DISTRIBUTE.lenth(), bundleBase);
        return true;
    }
    return false;
}

static vector<string> GetDefaultIncludePath(int userId, const string &bundleName)
{
    vector<string> include = {};
    vector<string> defaultInclude = {BConstants::PATHES_TO_BACKUP.begin(), BConstants::PATHES_TO_BACKUP.end()};
    std::string strEl2 = "/data/app/el2";
    strEl2.append(std::to_string(userId));
    std::string strEl1 = "/data/app/el1";
    strEl1.append(std::to_string(userId));
    for (auto inc : defaultInclude) {
        auto pos = inc.find(BConstants::PATH_RELETIVE_HOME_EL1);
        if (pos != string::npos) {
            inc.replace(pos, BConstants::PATH_RELETIVE_HOME_EL1.length(), strEl1);
            HILOGD("inc %{public}s", inc.c_str());
            if (ReplaceBundleName(inc, bundleName)) {
                include.push_back(inc);
            }
        }
        auto pos = inc.find(BConstants::PATH_RELETIVE_HOME_EL2);
        if (pos != string::npos) {
            inc.replace(pos, BConstants::PATH_RELETIVE_HOME_EL2.length(), strEl2);
            HILOGD("inc %{public}s", inc.c_str());
            if (ReplaceBundleName(inc, bundleName)) {
                include.push_back(inc);
            }
        }
    }
    return include;
}

ErrCode MigrateManager::HandleBackup(bool isClearData, const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    isClearData_ = isClearData;
    curScenario_ = BackupRestoreScenario::FULL_BACKUP;
    AsyncTaskBackup(bundleName);
    return BError(BError::Codes::OK);
}

void MigrateManager::AsyncTaskBackup(const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto task = [obj {wptr<MigrateManager>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        try {
            ptr->CreateDefaultTask(bundleName);
            ptr->ScanAllDirsTask();
            ptr->DoPacket(); // 关注点：大文件的传输如果速度较慢，会导致tar包挤压，手机空间会有所增加
            ptr->GetScanInstance(bundleName)->SetCompletedFlag(true);
        } catch (const BError &e) {
            HILOGE("extension: AsyncTaskBackup error, err code:%{public}d", e.GetCode());
            ptr->GetScanInstance(bundleName)->SetCompletedFlag(true);
            ptr->AppDone(e.GetCode(), bundleName);
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->GetScanInstance(bundleName)->SetCompletedFlag(true);
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode(), bundleName);
        }
    };

    threadPool_.AddTask([task]() { // REM: 这里异步化了，需要做并发控制
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
    AsyncDoBackup(bundleName); // 单独起线程处理准备好的大文件/tar包，通知给上游工具应用
}

void MigrateManager::AsyncDoBackup(const string &bundleName)
{
    auto dobackupTask = [obj {wptr<MigrateManager>(this)}](const string &bundleName) {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        ptr->CreateDefaultTask(bundleName);
        ptr->DoBackupTask(bundleName);
        ptr->DoClear(bundleName);
    };
    doBackupPool_.AddTask([dobackupTask = std::move(dobackupTask)]() {
        try {
            dobackupTask();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

std::shared_ptr<ScanResultManager> MigrateManager::GetScanInstance(const string &bundleName)
{
    std::lock_guard<std::mutex> lock(scanInstanceLock_);
    auto it = instanceMap_.find(bundleName);
    if (it == instanceMap_.end()) {
        auto newInstance = std::make_shared<ScanResultManager>();
        if (newInstance == nullptr) {
            HILOGE("Failed to create ScanResultManager instance");
            return nullptr;
        }
        instanceMap_[bundleName] = newInstance;
        return newInstance;
    }
    return it->second;
}

ErrCode MigrateManager::CreateDefaultTask(const string &bundleName)
{
    try {
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("CreateDefaultTask, enhance service is not loaded");
            return BError(BError::Codes::OK);
        }
        return enhanceService->CreateDefaultTask(bundleName, userId_);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("CreateDefaultTask, Unexpected exception");
        return EPERM;
    }
}

void MigrateManager::WaitToSendFd(std::chrono::system_clock::time_point &startTime, int &fdSendNum)
{
    HILOGD("WaitToSendFd Begin");
    std::unique_lock<std::mutex> lock(startSendMutex_);
    startSendFdRateCon_.wait(lock, [this] { return sendRate_ > 0; });
    if (fdSendNum >= sendRate_) {
        HILOGI("current time fd num is max rate, bundle name:%{public}s, rate:%{public}d", bundleName_.c_str(),
            sendRate_);
        auto curTime = std::chrono::system_clock::now();
        auto useTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count();
        if (useTimeMs < MAX_FD_GROUP_USE_TIME) {
            int32_t sleepTime = MAX_FD_GROUP_USE_TIME - useTimeMs;
            HILOGI("will wait time:%{public}d ms", sleepTime);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        } else {
            HILOGW("current fd send num exceeds one second");
        }
        fdSendNum = 0;
        startTime = std::chrono::system_clock::now();
    }
    HILOGD("WaitToSendFd End");
}

ErrCode MigrateManager::UpdateFdSendRate(const std::string &bundleName, int32_t sendRate)
{
    try {
        std::lock_guard<std::mutex> lock(updateSendRateLock_);
        HILOGI("Update SendRate, bundleName:%{public}s, sendRate:%{public}d", bundleName.c_str(), sendRate);
        sendRate_ = sendRate;
        if (appStatistic_ == nullptr) {
            HILOGW("appStatistic_ is null");
            return ERR_OK;
        }
        if (sendRate > 0) {
            appStatistic_->UpdateSendRateZeroSpend();
            startSendFdRateCon_.notify_one();
        } else {
            appStatistic_->sendRateZeroStart_ = TimeUtils::GetTimeUS();
        }
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to UpdateFdSendRate");
        return BError(BError::Codes::EXT_BROKEN_IPC).GetCode();
    }
}

void MigrateManager::DoBackupTask(const std::string &bundleName)
{
    int ret = ERR_OK;
    int fdNum = 0;
    std::vector<std::shared_ptr<IFileInfo>> allFiles;
    while (!GetScanInstance(bundleName)->IsProcessCompleted() || GetScanInstance(bundleName)->HasFileReady()) {
        GetScanInstance(bundleName)->WaitForCompleted();
        std::shared_ptr<IFileInfo> fileInfo = GetScanInstance(bundleName)->GetFileInfo();
        if (fileInfo == nullptr) {
            HILOGE("Get null file info!!");
            continue;
        }
        int subRet = ERR_OK;
        GetScanInstance(bundleName)->AddAllFile(fileInfo);
        if (fileInfo->isBigFile_) {
            subRet = ReportAppFileReady(bundleName, fileInfo->filename_, fileInfo->filePath_);
            appStatistic_->bigFileCount_++;
            UpdateFileStat(fileInfo->filePath_, fileInfo->sta_.st_size);
            fdNum++;
        } else {
            subRet = ReportAppFileReady(bundleName, fileInfo->filename_, fileInfo->filePath_, true);
            fdNum += BConstants::FILE_AND_MANIFEST_FD_COUNT;
        }
        if (subRet != ERR_OK) { // 后续错误码上报DFX
            HILOGE("report file ready fail,filename=%{public}s, err=%{public}d", fileInfo->filename_.c_str(), subRet);
            ret = static_cast<int>(BError::Codes::EXT_REPORT_FILE_READY_FAIL);
        }
    }
    int indexRet = IndexFileReady(allFiles, bundleName);
    if (indexRet != ERR_OK) {
        HILOGE("report app file ready fail, err=%{public}d", indexRet);
        ret = static_cast<int>(BError::Codes::EXT_REPORT_FILE_READY_FAIL);
    }
    GetScanInstance(bundleName)->SetCompletedFlag(false);
    AppDone(ret);
    HILOGI("backup app done ret=%{public}d", ret);
}

ErrCode MigrateManager::ReportAppFileReady(const std::string &bundleName,
    const string &filename, const string &filePath, bool needDelete)
{
    int32_t errCode = ERR_OK;
    UniqueFd fd(INVALID_FD);
    std::string newPath = BExcepUltils::Canonicalize(filePath);
    if (servicePtr_ == nullptr) {
        HILOGE("ServiceClient is null");
        return static_cast<int32_t>(BError::Codes::EXT_CLIENT_IS_NULL);
    }
    auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
    if (!enhanceService) {
        HILOGW("enhance service is not loaded");
        return errCode;
    }
    errCode = enhanceService->DefaultOpenFile(bundleName, newPath, fd);
    if (errCode != 0) {
        HILOGW("GetFileHandle fail err:%{public}d", errCode);
    }
    int reportRs =
        fd.Get() < 0 ? servicePtr_->AppFileReadyWithoutFd(filename, newPath, errCode) :
        servicePtr_->AppFileReady(filename, newPath, move(fd), errCode);
    if (SUCCEEDED(reportRs)) {
        HILOGD("Report app file ready success, filename: %{public}s", filename.c_str());
        if (needDelete) {
            auto ret = RemoveFile(newPath);
            HILOGD("RemoveFile result:%{public}d, newPath:%{public}s", ret, newPath.c_str());
        }
    } else {
        HILOGW("Report app file ready failed, ret: %{public}d, filename: %{public}s", reportRs, filename.c_str());
    }
    return reportRs;
}

ErrCode MigrateManager::IndexFileReady(const std::string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    UniqueFd fd(BConstants::INVALID_FD_NUM);

    auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
    if (!enhanceService) {
        HILOGW("enhance service is not loaded");
        return BError(BError::Codes::OK);
    }
    string managePath = BConstants::GetBundleBackupDir(userId_, bundleName).append(
        BConstants::SA_BUNDLE_BACKUP_BACKUP).append(BConstants::EXT_BACKUP_MANAGE);
    auto err = enhanceService->GetIndexFile(bundleName, managePath, size, fd);
    appStatistic_->manageJsonSize_ = BFile::GetFileSize(managePath, err);
    if (err != 0) {
        HILOGE("get index size fail err:%{public}d", err);
    }
    return ReportAppFileReady(bundleName, string(BConstants::EXT_BACKUP_MANAGE), managePath, true);
}

void MigrateManager::DoPacket(const string &bundleName)
{
    string callerName;
    ErrCode ret = VerifyCallerAndGetCallerName(bundleName);
    if (ret != ERR_OK) {
        HILOGE("error, Get bundle name failed, ret:%{public}d", ret);
        return ret;
    }
    auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
    if (!enhanceService) {
        HILOGW("enhance service is not loaded");
        return;
    }
    auto instance = GetScanInstance(bundleName);
    enhanceService->StartDefaultPacket(bundleName, instance);
    HILOGI("TarSpend: %{public}u ms", appStatistic_->tarSpend_);
}

void MigrateManager::ScanAllDirsTask(const string &bundleName)
{
    int64_t totalSize = 0;
    int32_t err = ScanAllDirs(totalSize, bundleName);
    if (err != ERR_OK) {
        throw BError(BError::Codes::EXT_INVAL_ARG, "Failed to ScanAllDirs");
    }
    SetScanTotalSize(totalSize);
}

ErrCode MigrateManager::ScanAllDirs(int64_t &totalSize, const string &bundleName)
{
    HILOGI("Start scanning files and calculate datasize");
    vector<string> includes = GetDefaultIncludePath(userId_, bundleName);
    vector<string> excludes = {};
    set<string> expandIncludes = BDir::ExpandPathWildcard(includes, true);

    ErrCode ret = VerifyCallerAndGetCallerName(bundleName);
    if (ret != ERR_OK) {
        HILOGE("error, Get bundle name failed, ret:%{public}d", ret);
        return ret;
    }
    auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
    if (!enhanceService) {
        HILOGW("enhance service is not loaded");
        return BError(BError::Codes::OK);
    }
    ret = enhanceService->MakeDir(bundleName);
    auto instance = GetScanInstance(bundleName);
    enhanceService->StartDefaultPacket(bundleName, instance);

    auto [errCode, bigFileSize, smallFileSize] = BDir::DefaultScanAllDirs(expandIncludes, excludes, instance);

    appStatistic_->scanFileSpend_.End();

    appStatistic_->bigFileSize_ = static_cast<uint64_t>(bigFileSize);
    appStatistic_->smallFileSize_ = static_cast<uint64_t>(smallFileSize);
    totalSize = bigFileSize + smallFileSize;
    // start timeout
    SetDefaultAppTimer(totalSize);
    HILOGI("Scan end, size=%{public}" PRId64 ", spend=%{public}u", totalSize, appStatistic_->scanFileSpend_.GetSpan());
    return ret;
}

void MigrateManager::SetScanTotalSize(int64_t totalSize)
{
    std::lock_guard<std::mutex> lock(scanSizeLock_);
    scanTotalSize_ = totalSize;
}

int64_t MigrateManager::GetScanTotalSize()
{
    std::lock_guard<std::mutex> lock(scanSizeLock_);
    return scanTotalSize_;
}

void MigrateManager::UpdateOnStartTime()
{
    std::lock_guard<std::mutex> lock(onStartTimeLock_);
    g_onStart = std::chrono::system_clock::now();
}

void MigrateManager::CloseFileWithFDSan(int fd)
{
    if (fd >= 0) {
        fdsan_close_with_tag(fd, BConstants::FDSAN_EXT_TAG);
    }
}

ErrCode MigrateManager::VerifyCallerAndGetCallerName(std::string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        Security::AccessToken::NativeTokenInfo nativeTokenInfo;
        if (Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(tokenCaller, nativeTokenInfo) != 0) {
            HILOGE("Verify and get caller name failed, Get token info failed");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        if (nativeTokenInfo.processName != BConstants::BACKUP_SA_NAME) {
            HILOGE("Verify processName failed, %{public}s", nativeTokenInfo.processName.c_str());
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        return BError(BError::Codes::OK);
    } else {
        string str = to_string(tokenCaller);
        HILOGE("Verify and get caller name failed, Invalid token type = %{private}s", GetAnonyString(str).c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
}
void MigrateManager::UpdateFileStat(std::string filePath, uint64_t fileSize)
{
    std::lock_guard<std::mutex> lock(updateFileStatLock_);
    appStatistic_->UpdateFileDist(ExtractFileExt(filePath), fileSize);
    uint32_t dirDepth = 0;
    const char* pstr = filePath.c_str();
    char pre = '-';
    uint32_t pathLen = filePath.size();
    for (uint32_t i = 0; i < pathLen; i++) {
        if (pstr[i] == '/' && pre != '/') {
            dirDepth++;
        }
        pre = pstr[i];
    }
    if (dirDepth > appStatistic_->dirDepth_) {
        appStatistic_->dirDepth_ = dirDepth;
    }
}

void MigrateManager::SetDefaultAppTimer(int64_t &appSize)
{
    const int64_t minTimeout = 900;      /* 900 second */
    const int64_t defaultTimeout = 30;           /* 30 second */
    const int64_t processRate = 3 * 1024 * 1024; /* 3M/s */
    const int64_t multiple = 3;
    const int64_t invertMillisecond = 1000;
    if (servicePtr_ == nullptr || servicePtr_->session_ == nullptr) {
        HILOGE("servicePtr or session is null");
        return;
    }
    auto timeout = defaultTimeout + (appSize / processRate) * multiple;
    timeout = timeout < minTimeout ? minTimeout : timeout;
    uint32_t resTimeoutMs = (uint32_t)(timeout * invertMillisecond % UINT_MAX);
    servicePtr_->session_->SetTimeoutValue(bundleName, resTimeoutMs);
    auto timeoutCallback = servicePtr_->TimeOutCallback(servicePtr_, bundleName);
    servicePtr_->session_->StartExtTimer(bundleName, timeoutCallback);
}
}