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
            HILOGE("inc %{public}s", inc.c_str());
            if (ReplaceBundleName(inc, bundleName)) {
                include.push_back(inc);
            }
        }
        auto pos = inc.find(BConstants::PATH_RELETIVE_HOME_EL2);
        if (pos != string::npos) {
            inc.replace(pos, BConstants::PATH_RELETIVE_HOME_EL2.length(), strEl2);
            HILOGE("inc %{public}s", inc.c_str());
            if (ReplaceBundleName(inc, bundleName)) {
                include.push_back(inc);
            }
        }
    }
    return include;
}

ErrCode MigrateManager::HandleBackup(bool isClearData)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    isClearData_ = isClearData; // ext公用函数
    curScenario_ = BackupRestoreScenario::FULL_BACKUP;
    AsyncTaskBackup();
    return BError(BError::Codes::OK);
}

void MigrateManager::AsyncTaskBackup()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto task = [obj {wptr<MigrateManager>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        try {
            ptr->ScanAllDirsTask(); // ext公用函数
            ptr->DoPacket(); // 关注点：大文件的传输如果速度较慢，会导致tar包挤压，手机空间会有所增加
            ScanFileSingleton::GetInstance().SetCompletedFlag(true);
        } catch (const BError &e) {
            HILOGE("extension: AsyncTaskBackup error, err code:%{public}d", e.GetCode());
            ScanFileSingleton::GetInstance().SetCompletedFlag(true);
            ptr->AppDone(e.GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ScanFileSingleton::GetInstance().SetCompletedFlag(true);
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        }
    };

    threadPool_.AddTask([task]() { // REM: 这里异步化了，需要做并发控制
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
    AsyncDoBackup(); // 单独起线程处理准备好的大文件/tar包，通知给上游工具应用
}

void MigrateManager::AsyncDoBackup()
{
    auto dobackupTask = [obj {wptr<MigrateManager>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        ptr->CreateDefaultTask();
        ptr->DoBackupTask();
        ptr->DoClear();
    };
    doBackupPool_.AddTask([dobackupTask = std::move(dobackupTask)]() {
        try {
            dobackupTask();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

ErrCode MigrateManager::CreateDefaultTask()
{
    try {
        ErrCode ret = VerifyCallerAndGetCallerName(bundleName_);
        if (ret != ERR_OK) {
            HILOGE("CreateDefaultTask error, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("CreateDefaultTask, enhance service is not loaded");
            return BError(BError::Codes::OK);
        }
        return enhanceService->CreateDefaultTask(bundleName_, userId_, appStatistic_);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("CreateDefaultTask, Unexpected exception");
        return EPERM;
    }
}

void MigrateManager::DoBackupTask()
{
    int ret = ERR_OK;
    int fdNum = 0;
    std::vector<std::shared_ptr<IFileInfo>> allFiles;
    while (!ScanFileSingleton::GetInstance().IsProcessCompleted() || ScanFileSingleton::GetInstance().HasFileReady()) {
        ScanFileSingleton::GetInstance().WaitForFiles();
        std::shared_ptr<IFileInfo> fileInfo = ScanFileSingleton::GetInstance().GetFileInfo();
        if (fileInfo == nullptr) {
            HILOGE("Get null file info!!");
            continue;
        }
        int subRet = ERR_OK;
        ScanFileSingleton::GetInstance().AddAllFile(fileInfo);
        if (fileInfo->isBigFile_) {
            // 原逻辑：这里需要把filename open，生成fd，然后通过ipc返回到service
            // 现逻辑：输入path，返回一个fd
            subRet = ReportAppFileReady(fileInfo->filename_, fileInfo->filePath_, fileInfo->GetFd());
            appStatistic_->bigFileCount_++;
            UpdateFileStat(fileInfo->filePath_, fileInfo->sta_.st_size);
            fdNum++;
        } else {
            subRet = ReportAppFileReady(fileInfo->filename_, fileInfo->filePath_, fileInfo->GetFd(), true);
            fdNum += BConstants::FILE_AND_MANIFEST_FD_COUNT;
        }
        if (subRet != ERR_OK) { // 后续错误码上报DFX
            HILOGE("report file ready fail,filename=%{public}s, err=%{public}d", fileInfo->filename_.c_str(), subRet);
            ret = static_cast<int>(BError::Codes::EXT_REPORT_FILE_READY_FAIL);
        }
    }
    int indexRet = IndexFileReady(allFiles);
    if (indexRet != ERR_OK) {
        HILOGE("report app file ready fail, err=%{public}d", indexRet);
        ret = static_cast<int>(BError::Codes::EXT_REPORT_FILE_READY_FAIL);
    }
    ScanFileSingleton::GetInstance().SetCompletedFlag(false);
    AppDone(ret);
    HILOGI("backup app done ret=%{public}d", ret);
}

ErrCode MigrateManager::ReportAppFileReady(
    const string &filename, const string &filePath, int &fdval, bool needDelete)
{
    int32_t errCode = ERR_OK;
    std::string newPath = BExcepUltils::Canonicalize(filePath);
    if (servicePtr_ == nullptr) {
        HILOGE("ServiceClient is null");
        CloseFileWithFDSan(fdval);
        return static_cast<int32_t>(BError::Codes::EXT_CLIENT_IS_NULL);
    }
    int reportRs = 0;
    if (fileName == BConstants::EXT_BACKUP_MANAGE) {
        reportRs =
            fdval < 0 ? servicePtr_->AppFileReadyWithoutFd(filename, filePath, errCode, fileName_) :
            servicePtr_->AppFileReady(filename, filePath, fdval, errCode, fileName_);
        fileName_.clear();
    } else {
        fileName_.insert(fileName);
        reportRs =
            fdval < 0 ? servicePtr_->AppFileReadyWithoutFd(filename, filePath, errCode, fileName_) :
            servicePtr_->AppFileReady(filename, filePath, fdval, errCode, fileName_);
    }
    CloseFileWithFDSan(fdval);
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
    int size = 0;
    string managePath = BConstants::GetBundleBackupDir(userId_, bundleName).append(
        BConstants::SA_BUNDLE_BACKUP_BACKUP).append(BConstants::EXT_BACKUP_MANAGE);
    auto err = enhanceService->GetIndexFile(bundleName, managePath, size, fd);
    if (err != 0) {
        HILOGE("get index size fail err:%{public}d", err);
    }
    appStatistic_->manageJsonSize_ = size;
    auto fdVal = move(fd.Get());
    return ReportAppFileReady(string(BConstants::EXT_BACKUP_MANAGE), managePath, fdVal, true);
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
    enhanceService->StartDefaultPacket(bundleName);
    HILOGI("TarSpend: %{public}u ms", appStatistic_->tarSpend_);
}

void MigrateManager::ScanAllDirsTask(const string &bundleName)
{
    int64_t totalSize = 0;
    int32_t err = ScanAllDirs(totalSize);
    if (err != ERR_OK) {
        throw BError(BError::Codes::EXT_INVAL_ARG, "Failed to ScanAllDirs");
    }
    SetScanTotalSize(totalSize);
}

ErrCode MigrateManager::ScanAllDirs(int64_t &totalSize)
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
    AncoRestoreResult scanResult;
    ret = enhanceService->StartDefaultScanAllDirs(bundleName, expandIncludes, excludes, scanResult);
    auto bigFileSize = scanResult.bigFileSize;
    auto smallFileSize = scanResult.smallFileSize;
    appStatistic_->scanFileSpend_.End();

    appStatistic_->bigFileSize_ = static_cast<uint64_t>(bigFileSize);
    appStatistic_->smallFileSize_ = static_cast<uint64_t>(smallFileSize);
    totalSize = bigFileSize + smallFileSize;
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

std::function<void(std::string, int)> MigrateManager::ReportErrFileByProc(std::string msg, int err)
{
    return [obj {wptr<MigrateManager>(this)}, scenario {curScenario_}](
        std::string msg, int err) {
        auto ptr = obj.promote();
        if (ptr == nullptr) {
            HILOGE("ReportErr ptr is empty.");
            return;
        }
        ptr->appStatistic_->UpdateErrorFileList(msg, err);
        string jsonInfo;
        BJsonUtil::BuildOnProcessErrInfo(jsonInfo, msg, err);
        HILOGI("ReportErr Will notify err info.");
        ptr->servicePtr_->ReportAppProcessInfo(jsonInfo, scenario);
    };
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

ErrCode MigrateManager::VerifyCallerAndGetCallerName(std::string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        Security::AccessToken::NativeTokenInfo nativeTokenInfo;
        if (Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(tokenCaller, nativeTokenInfo) != 0) {
            PermissionCheckFailRadar("Get hap token info failed", "VerifyCallerAndGetCallerName");
            HILOGE("Verify and get caller name failed, Get hap token info failed");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        if (nativeTokenInfo.processName != "backup_sa") {
            HILOGE("Verify bundlename failed, %{public}s", bundleName);
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        ErrCode ret = session_->VerifyBundleName(bundleName);
        if (ret != ERR_OK) {
            HILOGE("Verify bundle name failed, bundleNameIndexInfo:%{public}s", bundleNameIndexInfo.c_str());
            return ret;
        }
        return BError(BError::Codes::OK);
    } else {
        string str = to_string(tokenCaller);
        HILOGE("tokenID = %{private}s", GetAnonyString(str).c_str());
        std::string info = string("Invalid token type").append(to_string(tokenType)).append(string("\"}"));
        PermissionCheckFailRadar(info, "VerifyCallerAndGetCallerName");
        HILOGE("Verify and get caller name failed, Invalid token type");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
}
}