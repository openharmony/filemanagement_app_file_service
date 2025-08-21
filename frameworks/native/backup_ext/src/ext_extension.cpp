/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include <algorithm>
#include <chrono>
#include <cinttypes>
#include <fstream>
#include <iomanip>
#include <map>
#include <regex>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <directory_ex.h>
#include <sys/stat.h>
#include <unistd.h>

#include <directory_ex.h>
#include <unique_fd.h>

#include "accesstoken_kit.h"
#include "bundle_mgr_client.h"
#include "errors.h"
#include "ipc_skeleton.h"

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
#include "filemgmt_libhilog.h"
#include "hitrace_meter.h"
#include "installd_un_tar_file.h"
#include "iservice.h"
#include "sandbox_helper.h"
#include "service_client.h"
#include "tar_file.h"

namespace OHOS::FileManagement::Backup {
const string INDEX_FILE_BACKUP = string(BConstants::PATH_BUNDLE_BACKUP_HOME).
                                 append(BConstants::SA_BUNDLE_BACKUP_BACKUP).
                                 append(BConstants::EXT_BACKUP_MANAGE);
const string INDEX_FILE_RESTORE = string(BConstants::PATH_BUNDLE_BACKUP_HOME).
                                  append(BConstants::SA_BUNDLE_BACKUP_RESTORE).
                                  append(BConstants::EXT_BACKUP_MANAGE);
const string INDEX_FILE_INCREMENTAL_BACKUP = string(BConstants::PATH_BUNDLE_BACKUP_HOME).
                                             append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
const string MEDIA_LIBRARY_BUNDLE_NAME = "com.ohos.medialibrary.medialibrarydata";
const string FILE_MANAGER_BUNDLE_NAME = "com.ohos.filepicker";
using namespace std;

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

static void RecordDoBackupRes(const std::string &bundleName, const ErrCode errCode, AppRadar::DoBackupInfo &backupInfo)
{
    uint32_t inExcludeNum = backupInfo.includeNum + backupInfo.excludeNum;
    if (inExcludeNum >= BConstants::MAX_INEXCLUDE_SIZE) {
        AppRadar::Info infoInExclude(bundleName, "", string("\"total inExclude\":").append(to_string(inExcludeNum)));
        AppRadar::GetInstance().RecordBackupFuncRes(infoInExclude, "BackupExtExtension::DoBackup",
            AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_DO_BACKUP, ERR_OK);
    }
    if (errCode == ERR_OK && backupInfo.cost >= BConstants::MAX_TIME_COST) {
        std::stringstream ss;
        ss << R"("spendTime": )" << backupInfo.cost << "ms, ";
        ss << R"("totalFilesNum": )" << backupInfo.allFileNum << ", ";
        ss << R"("smallFilesNum": )" << backupInfo.smallFileNum << ", ";
        ss << R"("bigFilesNum": )" << backupInfo.allFileNum - backupInfo.tarFileNum;
        AppRadar::Info info(bundleName, "", ss.str());
        AppRadar::GetInstance().RecordBackupFuncRes(info, "BackupExtExtension::DoBackup",
            AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_DO_BACKUP, errCode);
    }
}

static string GetIndexFileRestorePath(const string &bundleName)
{
    if (BFile::EndsWith(bundleName, BConstants::BUNDLE_FILE_MANAGER) && bundleName.size() == BConstants::FM_LEN) {
        return string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE).
               append(BConstants::EXT_BACKUP_MANAGE);
    } else if (bundleName == BConstants::BUNDLE_MEDIAL_DATA) {
        return string(BConstants::PATH_MEDIALDATA_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE).
               append(BConstants::EXT_BACKUP_MANAGE);
    }
    return INDEX_FILE_RESTORE;
}

static string GetRestoreTempPath(const string &bundleName)
{
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    if (BFile::EndsWith(bundleName, BConstants::BUNDLE_FILE_MANAGER) && bundleName.size() == BConstants::FM_LEN) {
        if (mkdir(string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).data(), S_IRWXU) && errno != EEXIST) {
            string str = string("Failed to create .backup folder. ").append(std::generic_category().message(errno));
            throw BError(BError::Codes::EXT_INVAL_ARG, str);
        }
        path = string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    } else if (bundleName == BConstants::BUNDLE_MEDIAL_DATA) {
        path = string(BConstants::PATH_MEDIALDATA_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    }
    return path;
}

static std::set<std::string> GetIdxFileData(const string &bundleName)
{
    string indexFileRestorePath = GetIndexFileRestorePath(bundleName);
    UniqueFd idxFd(open(indexFileRestorePath.data(), O_RDONLY));
    if (idxFd < 0) {
        HILOGE("Failed to open idxFile = %{private}s, err = %{public}d", indexFileRestorePath.c_str(), errno);
        return std::set<std::string>();
    }
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(std::move(idxFd));
    auto cache = cachedEntity.Structuralize();
    return cache.GetExtManage();
}

std::vector<ExtManageInfo> BackupExtExtension::GetExtManageInfo(bool isSpecialVersion)
{
    string indexFileRestorePath = isSpecialVersion ? INDEX_FILE_RESTORE : GetIndexFileRestorePath(bundleName_);
    string filePath = BExcepUltils::Canonicalize(indexFileRestorePath);
    UniqueFd idxFd(open(filePath.data(), O_RDONLY));
    if (idxFd < 0) {
        HILOGE("Failed to open cano_idxFile = %{private}s, err = %{public}d", filePath.c_str(), errno);
        return {};
    }
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(std::move(idxFd));
    auto cache = cachedEntity.Structuralize();
    return cache.GetExtManageInfo();
}

void BackupExtExtension::VerifyCaller()
{
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
    if (tokenType != Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        AppRadar::Info info(bundleName_, "", "{\"reason\":\"Calling tokenType error\"}");
        AppRadar::GetInstance().RecordDefaultFuncRes(
            info, "BackupExtExtension::VerifyCaller", AppRadar::GetInstance().GetUserId(),
            BizStageBackup::BIZ_STAGE_PERMISSION_CHECK_FAIL, BError(BError::Codes::EXT_BROKEN_IPC).GetCode());
        throw BError(BError::Codes::EXT_BROKEN_IPC,
            string("Calling tokenType is error, token type is ").append(to_string(tokenType)));
    }
    if (IPCSkeleton::GetCallingUid() != BConstants::BACKUP_UID) {
        AppRadar::Info info(bundleName_, "", "{\"reason\":\"Calling uid invalid\"}");
        AppRadar::GetInstance().RecordDefaultFuncRes(
            info, "BackupExtExtension::VerifyCaller", AppRadar::GetInstance().GetUserId(),
            BizStageBackup::BIZ_STAGE_PERMISSION_CHECK_FAIL, BError(BError::Codes::EXT_BROKEN_IPC).GetCode());
        throw BError(BError::Codes::EXT_BROKEN_IPC,
            string("Calling uid is invalid, calling uid is ").append(to_string(IPCSkeleton::GetCallingUid())));
    }
}

static UniqueFd GetFileHandleForSpecialCloneCloud(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    string filePath = fileName;
    if (fileName.front() != BConstants::FILE_SEPARATOR_CHAR) {
        filePath = BConstants::FILE_SEPARATOR_CHAR + fileName;
    }
    size_t filePathPrefix = filePath.find_last_of(BConstants::FILE_SEPARATOR_CHAR);
    if (filePathPrefix == string::npos) {
        HILOGE("GetFileHandleForSpecialCloneCloud: Invalid fileName");
        return UniqueFd(BConstants::INVALID_FD_NUM);
    }
    string path = filePath.substr(0, filePathPrefix);
    if (access(path.c_str(), F_OK) != 0) {
        bool created = ForceCreateDirectory(path.data());
        if (!created) {
            HILOGE("Failed to create restore folder.");
            return UniqueFd(BConstants::INVALID_FD_NUM);
        }
    }
    UniqueFd fd(open(fileName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
    if (fd < 0) {
        HILOGE("Open file failed, file name is %{public}s, err = %{public}d", GetAnonyPath(fileName).c_str(), errno);
        return UniqueFd(BConstants::INVALID_FD_NUM);
    }
    return fd;
}

ErrCode BackupExtExtension::GetFileHandleWithUniqueFd(const std::string &fileName,
                                                      int32_t &getFileHandleErrCode,
                                                      int &fd)
{
    UniqueFd fileHandleFd(GetFileHandle(fileName, getFileHandleErrCode));
    fd = dup(fileHandleFd.Get());
    return ERR_OK;
}

UniqueFd BackupExtExtension::GetFileHandle(const string &fileName, int32_t &errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (extension_ == nullptr) {
            HILOGE("Failed to get file handle, extension is nullptr");
            throw BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr");
        }
        if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
            HILOGE("Failed to get file handle, because action is %{public}d invalid", extension_->GetExtensionAction());
            throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
        }

        VerifyCaller();

        if (extension_->SpecialVersionForCloneAndCloud()) {
            UniqueFd fd = GetFileHandleForSpecialCloneCloud(fileName);
            if (fd < 0) {
                errCode = errno;
            }
            return fd;
        }

        string path = GetRestoreTempPath(bundleName_);
        if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
            string str = string("Failed to create restore folder. ").append(std::generic_category().message(errno));
            throw BError(BError::Codes::EXT_INVAL_ARG, str);
        }

        string tarName = path + fileName;
        if (access(tarName.c_str(), F_OK) == 0) {
            throw BError(BError::Codes::EXT_INVAL_ARG, string("The file already exists"));
        }
        UniqueFd tarFd(open(tarName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
        if (tarFd < 0) {
            HILOGE("Open file failed, file name is %{private}s, err = %{public}d", tarName.data(), errno);
            errCode = errno;
        }
        return tarFd;
    } catch (...) {
        HILOGE("Failed to get file handle");
        DoClear();
        errCode = -1;
        return UniqueFd(-1);
    }
}

string BackupExtExtension::GetReportFileName(const string &fileName)
{
    string reportName = fileName + "." + string(BConstants::REPORT_FILE_EXT);
    return reportName;
}

tuple<ErrCode, UniqueFd, UniqueFd> BackupExtExtension::GetIncreFileHandleForSpecialVersion(const string &fileName)
{
    ErrCode errCode = ERR_OK;
    HILOGI("Begin GetFileHandleForSpecialCloneCloud: fileName is %{public}s", GetAnonyPath(fileName).c_str());
    UniqueFd fd = GetFileHandleForSpecialCloneCloud(fileName);
    if (fd < 0) {
        HILOGE("Failed to open file = %{public}s, err = %{public}d", GetAnonyPath(fileName).c_str(), errno);
        errCode = errno;
    }

    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
        HILOGE("Failed to create restore folder : %{public}s, err = %{public}d", path.c_str(), errno);
        errCode = errno;
    }
    size_t reportParentPathPos = fileName.find_last_of(BConstants::FILE_SEPARATOR_CHAR);
    if (reportParentPathPos == std::string::npos) {
        HILOGE("Get current file parent path error");
        UniqueFd invalidFd(BConstants::INVALID_FD_NUM);
        return { errCode, move(fd), move(invalidFd) };
    }
    string reportFullFileName = GetReportFileName(fileName);
    string reportFileName = reportFullFileName.substr(reportParentPathPos + 1);
    if (reportFileName.size() > BConstants::LONG_FILE_NAME_BOUNDARY_VAL) {
        string reportHashName = BackupFileHash::HashFilePath(reportFullFileName);
        HILOGI("FileName is too long, report HashName is:%{public}s", reportHashName.c_str());
        std::string reportParentPath = fileName.substr(0, reportParentPathPos);
        std::string reportFullHashName = reportParentPath + BConstants::FILE_SEPARATOR_CHAR + reportHashName;
        UniqueFd reportHashFd(open(reportFullHashName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
        if (reportHashFd < 0) {
            HILOGE("Failed open report file = %{public}s, err = %{public}d",
                GetAnonyPath(reportFullHashName).c_str(), errno);
            errCode = errno;
        }
        std::unique_lock<std::mutex> lock(reportHashLock_);
        reportHashSrcPathMap_.emplace(fileName, reportFullHashName);
        return { errCode, move(fd), move(reportHashFd) };
    }
    UniqueFd reportFd(open(reportFullFileName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
    if (reportFd < 0) {
        HILOGE("Failed to open report file = %{public}s, err = %{public}d",
            GetAnonyPath(reportFullFileName).c_str(), errno);
        errCode = errno;
    }
    return { errCode, move(fd), move(reportFd) };
}

static ErrCode GetIncrementalFileHandlePath(const string &fileName, const string &bundleName, std::string &tarName)
{
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    if (BFile::EndsWith(bundleName, BConstants::BUNDLE_FILE_MANAGER) && bundleName.size() == BConstants::FM_LEN) {
        if (mkdir(string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).data(), S_IRWXU) && errno != EEXIST) {
            string errMsg = string("Failed to create .backup folder. ").append(std::generic_category().message(errno));
            HILOGE("%{public}s, errno = %{public}d", errMsg.c_str(), errno);
            return errno;
        }
        path = string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    } else if (bundleName == BConstants::BUNDLE_MEDIAL_DATA) {
        path = string(BConstants::PATH_MEDIALDATA_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    }
    if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
        string errMsg = string("Failed to create restore folder. ").append(std::generic_category().message(errno));
        HILOGE("%{public}s, errno = %{public}d", errMsg.c_str(), errno);
        return errno;
    }
    tarName = path + fileName;
    return ERR_OK;
}

tuple<ErrCode, UniqueFd, UniqueFd> BackupExtExtension::GetIncreFileHandleForNormalVersion(const std::string &fileName)
{
    HILOGI("extension: GetIncrementalFileHandle single to single Name:%{public}s", GetAnonyPath(fileName).c_str());
    std::string tarName;
    int32_t errCode = ERR_OK;
    UniqueFd fd(BConstants::INVALID_FD_NUM);
    UniqueFd reportFd(BConstants::INVALID_FD_NUM);
    do {
        errCode = GetIncrementalFileHandlePath(fileName, bundleName_, tarName);
        if (errCode != ERR_OK) {
            HILOGE("GetIncrementalFileHandlePath failed, err = %{public}d", errCode);
            break;
        }
        if (access(tarName.c_str(), F_OK) == 0) {
            HILOGE("The file already exists, tarname = %{public}s, err =%{public}d",
                GetAnonyPath(tarName).c_str(), errno);
        }
        fd = UniqueFd(open(tarName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
        if (fd < 0) {
            HILOGE("Failed to open tar file = %{public}s, err = %{public}d", GetAnonyPath(tarName).c_str(), errno);
            errCode = errno;
            break;
        }
        // 对应的简报文件
        string reportName = GetReportFileName(tarName);
        if (access(reportName.c_str(), F_OK) == 0) {
            HILOGE("The report file already exists, Name = %{public}s, err =%{public}d",
                GetAnonyPath(reportName).c_str(), errno);
        }
        reportFd = UniqueFd(open(reportName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
        if (reportFd < 0) {
            HILOGE("Failed to open report file = %{public}s, err = %{public}d",
                GetAnonyPath(reportName).c_str(), errno);
            errCode = errno;
            break;
        }
    } while (0);
    return {errCode, move(fd), move(reportFd)};
}

ErrCode BackupExtExtension::GetIncrementalFileHandle(const std::string &fileName,
                                                     int &fd, int &reportFd, int32_t &fdErrCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto [errCode, fdval, reportFdVal] = GetIncrementalFileHandle(fileName);
    fdErrCode = errCode;
    fd = dup(fdval.Get());
    reportFd = dup(reportFdVal.Get());
    return ERR_OK;
}

tuple<ErrCode, UniqueFd, UniqueFd> BackupExtExtension::GetIncrementalFileHandle(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (extension_ == nullptr) {
            HILOGE("Failed to get incremental file handle, extension  is invalid");
            throw BError(BError::Codes::EXT_INVAL_ARG, "extension is invalid");
        }
        if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
            HILOGE("Failed to get incremental file handle, action is invalid, action %{public}d.",
                extension_->GetExtensionAction());
            throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
        }
        VerifyCaller();
        if (!BDir::IsFilePathValid(fileName)) {
            auto proxy = ServiceClient::GetInstance();
            if (proxy == nullptr) {
                throw BError(BError::Codes::EXT_BROKEN_IPC, string("Failed to AGetInstance"));
            }
            HILOGE("Check file path : %{public}s err, path is forbidden", GetAnonyPath(fileName).c_str());
            auto ret = proxy->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG));
            if (ret != ERR_OK) {
                HILOGE("Failed to notify app incre done. err = %{public}d", ret);
            }
            return {BError(BError::Codes::EXT_INVAL_ARG).GetCode(), UniqueFd(-1), UniqueFd(-1)};
        }
        if (extension_->SpecialVersionForCloneAndCloud()) {
            return GetIncreFileHandleForSpecialVersion(fileName);
        }
        return GetIncreFileHandleForNormalVersion(fileName);
    } catch (...) {
        HILOGE("Failed to get incremental file handle");
        DoClear();
        return {BError(BError::Codes::EXT_BROKEN_IPC).GetCode(), UniqueFd(-1), UniqueFd(-1)};
    }
}

ErrCode BackupExtExtension::HandleClear()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("begin clear");
        if (extension_ == nullptr) {
            HILOGE("Failed to handle clear, extension is nullptr");
            return BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr").GetCode();
        }
        if (extension_->GetExtensionAction() == BConstants::ExtensionAction::INVALID) {
            return BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid").GetCode();
        }
        VerifyCaller();
        DoClear();
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to handle clear");
        return BError(BError::Codes::EXT_BROKEN_IPC).GetCode();
    }
}

ErrCode BackupExtExtension::IndexFileReady(const TarMap &pkgInfo, sptr<IService> proxy)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    UniqueFd fd(open(INDEX_FILE_BACKUP.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR));
    if (fd < 0) {
        HILOGE("Failed to open index json file = %{private}s, err = %{public}d", INDEX_FILE_BACKUP.c_str(), errno);
        return BError::GetCodeByErrno(errno);
    }
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    cache.SetExtManage(pkgInfo);
    cachedEntity.Persist();
    close(cachedEntity.GetFd().Release());
    int32_t err = 0;
    appStatistic_->manageJsonSize_ = BFile::GetFileSize(INDEX_FILE_BACKUP, err);
    if (err != 0) {
        HILOGE("get index size fail err:%{public}d", err);
    }
    int fdval = open(INDEX_FILE_BACKUP.data(), O_RDONLY);
    ErrCode ret = fdval < 0 ?
            proxy->AppFileReadyWithoutFd(string(BConstants::EXT_BACKUP_MANAGE), ERR_OK) :
            proxy->AppFileReady(string(BConstants::EXT_BACKUP_MANAGE), fdval, ERR_OK);
    if (SUCCEEDED(ret)) {
        HILOGI("The application is packaged successfully");
    } else {
        HILOGI(
            "The application is packaged successfully but the AppFileReady interface fails to be invoked: "
            "%{public}d",
            ret);
        close(fdval);
    }
    HILOGI("End notify Appfile Ready");
    return ret;
}

void BackupExtExtension::ClearNoPermissionFiles(TarMap &pkgInfo, vector<std::string> &noPermissionFiles)
{
    HILOGI("start ClearNoPermissionFiles;");
    for (const auto &item : noPermissionFiles) {
        auto it = pkgInfo.find(item);
        if (it != pkgInfo.end()) {
            HILOGI("noPermissionFile, don't need to backup, path = %{public}s",
                GetAnonyString(std::get<0>(it->second)).c_str());
            pkgInfo.erase(it);
        }
    }
}

ErrCode BackupExtExtension::BigFileReady(TarMap &bigFileInfo, sptr<IService> proxy, int backupedFileSize)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("BigFileReady Begin: bigFileInfo file size is: %{public}zu, backupedFileSize is %{public}d",
           bigFileInfo.size(), backupedFileSize);
    ErrCode ret {ERR_OK};
    auto startTime = std::chrono::system_clock::now();
    int fdNum = backupedFileSize;
    vector<string> noPermissionFiles;
    for (auto &item : bigFileInfo) {
        WaitToSendFd(startTime, fdNum);
        int32_t errCode = ERR_OK;
        string filePath = std::get<0>(item.second);
        int fdval = open(filePath.data(), O_RDONLY);
        if (fdval < 0) {
            HILOGE("open file failed, file name is %{public}s, err = %{public}d", GetAnonyString(filePath).c_str(),
                errno);
            errCode = errno;
            if (errCode == ERR_NO_PERMISSION) {
                noPermissionFiles.emplace_back(item.first.c_str());
                continue;
            }
        }
        ret = fdval < 0 ? proxy->AppFileReadyWithoutFd(item.first, errCode) :
                          proxy->AppFileReady(item.first, fdval, errCode);
        if (SUCCEEDED(ret)) {
            HILOGI("The application is packaged successfully, package name is %{public}s", item.first.c_str());
        } else {
            HILOGW("Current file execute app file ready interface failed, ret is:%{public}d", ret);
        }
        close(fdval);
        fdNum++;
        RefreshTimeInfo(startTime, fdNum);
    }
    ClearNoPermissionFiles(bigFileInfo, noPermissionFiles);
    HILOGI("BigFileReady End, fdNum is %{public}d", fdNum);
    return ret;
}

ErrCode BackupExtExtension::PublishFile(const std::string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Begin publish file. fileName is %{public}s", GetAnonyPath(fileName).c_str());
    try {
        if (extension_ == nullptr) {
            HILOGE("Failed to publish file, extension is nullptr");
            throw BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr");
        }
        if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
            HILOGE("Failed to publish file, action is invalid");
            throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
        }
        VerifyCaller();
        // 异步执行解压操作
        if (extension_->AllowToBackupRestore()) {
            AsyncTaskRestore(GetIdxFileData(bundleName_), GetExtManageInfo());
        }
        HILOGI("End publish file");
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

ErrCode BackupExtExtension::PublishIncrementalFile(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("begin publish incremental file. fileName is %{private}s", fileName.data());
    try {
        if (extension_ == nullptr) {
            HILOGE("Failed to publish incremental file, extension is nullptr");
            throw BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr");
        }
        if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
            HILOGE("Failed to publish incremental file, action is invalid");
            throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
        }
        VerifyCaller();
        // 异步执行解压操作
        if (BackupPara::GetBackupDebugState()) {
            isDebug_ = true;
        }
        if (extension_->AllowToBackupRestore()) {
            if (extension_->SpecialVersionForCloneAndCloud()) {
                HILOGI("Create task for Incremental SpecialVersion");
                AsyncTaskIncreRestoreSpecialVersion();
            } else {
                HILOGI("Create task for Incremental Restore");
                AsyncTaskIncrementalRestore();
            }
        }
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

ErrCode BackupExtExtension::HandleBackup(bool isClearData)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    SetClearDataFlag(isClearData);
    if (!IfAllowToBackupRestore()) {
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE, "Application does not allow backup or restore")
            .GetCode();
    }
    AsyncTaskOnBackup();
    return ERR_OK;
}

static bool IsUserTar(const string &tarFile, const std::vector<ExtManageInfo> &extManageInfo, off_t &tarFileSize)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (tarFile.empty()) {
        return false;
    }
    auto iter = find_if(extManageInfo.begin(), extManageInfo.end(),
        [&tarFile](const auto &item) { return item.hashName == tarFile; });
    if (iter != extManageInfo.end()) {
        HILOGI("tarFile:%{public}s isUserTar:%{public}d", tarFile.data(), iter->isUserTar);
        tarFileSize = iter->sta.st_size;
        return iter->isUserTar;
    }
    HILOGE("Can not find tarFile %{public}s", tarFile.data());
    return false;
}

pair<TarMap, map<string, size_t>> BackupExtExtension::GetFileInfos(const vector<string> &includes,
    const vector<string> &excludes)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    appStatistic_->scanFileSpend_.Start();
    auto [errCode, files, smallFiles] = BDir::GetBigFiles(includes, excludes);
    appStatistic_->scanFileSpend_.End();
    if (errCode != 0) {
        return {};
    }

    auto getStringHash = [](const TarMap &m, const string &str) -> string {
        ostringstream strHex;
        strHex << hex;

        hash<string> strHash;
        size_t szHash = strHash(str);
        strHex << setfill('0') << setw(BConstants::BIG_FILE_NAME_SIZE) << szHash;
        string name = strHex.str();
        for (int i = 0; m.find(name) != m.end(); ++i, strHex.str("")) {
            szHash = strHash(str + to_string(i));
            strHex << setfill('0') << setw(BConstants::BIG_FILE_NAME_SIZE) << szHash;
            name = strHex.str();
        }

        return name;
    };

    TarMap bigFiles;
    for (const auto &item : files) {
        int64_t hashStart = TimeUtils::GetTimeUS();
        string md5Name = getStringHash(bigFiles, item.first);
        UpdateFileStat(item.first, item.second.st_size);
        appStatistic_->hashSpendUS_ += TimeUtils::GetSpendUS(hashStart);
        if (!md5Name.empty()) {
            bigFiles.emplace(md5Name, make_tuple(item.first, item.second, true));
        }
    }
    return {bigFiles, smallFiles};
}

/**
 * 全量tar包回传
 */
static ErrCode TarFileReady(const TarMap &tarFileInfo, sptr<IService> proxy)
{
    if (tarFileInfo.empty()) {
        HILOGI("TarFileReady: No tar file found");
        return ERR_OK;
    }
    string tarName = tarFileInfo.begin()->first;
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
    string tarPath = path + tarName;
    int32_t errCode = ERR_OK;
    int fdval = open(tarPath.data(), O_RDONLY);
    if (fdval < 0) {
        HILOGE("TarFileReady open file failed, file name is %{public}s, err = %{public}d", tarName.c_str(), errno);
        errCode = errno;
    }
    int ret = fdval < 0 ? proxy->AppFileReadyWithoutFd(tarName, errCode) :
              proxy->AppFileReady(tarName, fdval, errCode);
    if (SUCCEEDED(ret)) {
        HILOGI("TarFileReady: AppFileReady success for %{public}s", tarName.c_str());
        // 删除文件
        RemoveFile(tarPath);
    } else {
        HILOGE("TarFileReady AppFileReady fail to be invoked for %{public}s: ret = %{public}d", tarName.c_str(), ret);
    }
    close(fdval);
    return ret;
}

std::function<void(std::string, int)> BackupExtExtension::ReportErrFileByProc(wptr<BackupExtExtension> obj,
    BackupRestoreScenario scenario)
{
    return [obj, scenario](std::string msg, int err) {
        auto extPtr = obj.promote();
        if (extPtr == nullptr) {
            HILOGE("ReportErr ExtPtr is empty.");
            return;
        }
        string jsonInfo;
        BJsonUtil::BuildOnProcessErrInfo(jsonInfo, msg, err);
        HILOGI("ReportErr Will notify err info.");
        extPtr->ReportAppProcessInfo(jsonInfo, scenario);
    };
}

void BackupExtExtension::DoPacket(const map<string, size_t> &srcFiles, TarMap &tar, sptr<IService> proxy)
{
    HILOGI("DoPacket begin, infos count: %{public}zu", srcFiles.size());
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
    uint64_t totalSize = 0;
    uint32_t fileCount = 0;
    vector<string> packFiles;
    TarFile::GetInstance().SetPacketMode(true); // 设置下打包模式
    auto startTime = std::chrono::system_clock::now();
    int fdNum = 0;
    auto reportCb = ReportErrFileByProc(wptr<BackupExtExtension> {this}, curScenario_);
    uint64_t totalTarUs = 0;
    for (const auto &small : srcFiles) {
        totalSize += small.second;
        fileCount += 1;
        packFiles.emplace_back(small.first);
        if (totalSize >= BConstants::DEFAULT_SLICE_SIZE || fileCount >= BConstants::MAX_FILE_COUNT) {
            TarMap tarMap {};
            int64_t tarStartUs = TimeUtils::GetTimeUS();
            TarFile::GetInstance().Packet(packFiles, "part", path, tarMap, reportCb);
            totalTarUs += TimeUtils::GetSpendUS(tarStartUs);
            appStatistic_->tarFileSize_ += TarFile::GetInstance().GetTarFileSize();
            appStatistic_->tarFileCount_++;
            tar.insert(tarMap.begin(), tarMap.end());
            // 执行tar包回传功能
            WaitToSendFd(startTime, fdNum);
            TarFileReady(tarMap, proxy);
            totalSize = 0;
            fileCount = 0;
            packFiles.clear();
            fdNum += BConstants::FILE_AND_MANIFEST_FD_COUNT;
            RefreshTimeInfo(startTime, fdNum);
        }
    }
    if (fileCount > 0) {
        // 打包回传
        TarMap tarMap {};
        int64_t tarStartUs = TimeUtils::GetTimeUS();
        TarFile::GetInstance().Packet(packFiles, "part", path, tarMap, reportCb);
        totalTarUs += TimeUtils::GetSpendUS(tarStartUs);
        appStatistic_->tarFileSize_ += TarFile::GetInstance().GetTarFileSize();
        appStatistic_->tarFileCount_++;
        TarFileReady(tarMap, proxy);
        fdNum = 1;
        WaitToSendFd(startTime, fdNum);
        tar.insert(tarMap.begin(), tarMap.end());
        packFiles.clear();
        RefreshTimeInfo(startTime, fdNum);
    }
    appStatistic_->tarSpend_ = static_cast<uint32_t>(totalTarUs / MS_TO_US);
}

int BackupExtExtension::DoBackup(TarMap &bigFileInfo, TarMap &fileBackupedInfo, map<string, size_t> &smallFiles,
    uint32_t includesNum, uint32_t excludesNum)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("bigFileInfo size: %{public}zu, fileBackupedInfo size %{public}zu, smallFiles size: %{public}zu",
           bigFileInfo.size(), fileBackupedInfo.size(), smallFiles.size());
    auto start = std::chrono::system_clock::now();
    if (extension_ == nullptr) {
        HILOGE("Failed to do backup, extension is nullptr");
        throw BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr");
    }
    if (extension_->GetExtensionAction() != BConstants::ExtensionAction::BACKUP) {
        return EPERM;
    }

    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
    }

    // 回传大文件
    HILOGI("Will notify BigFileReady");
    auto res = BigFileReady(bigFileInfo, proxy, fileBackupedInfo.size());

    HILOGI("Start packet Tar files");
    // 分片打包， 回传tar包
    TarMap tarMap {};
    DoPacket(smallFiles, tarMap, proxy);
    bigFileInfo.insert(tarMap.begin(), tarMap.end());
    fileBackupedInfo.insert(bigFileInfo.begin(), bigFileInfo.end());
    HILOGI("Do backup, DoPacket end");

    HILOGI("Will notify IndexFileReady");
    if (auto ret = IndexFileReady(fileBackupedInfo, proxy); ret) {
        return ret;
    }

    HILOGI("HandleBackup finish, ret = %{public}d", res);
    auto end = std::chrono::system_clock::now();
    auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    AppRadar::DoBackupInfo doBackupInfo = {cost, fileBackupedInfo.size(), smallFiles.size(), tarMap.size(),
                                           includesNum, excludesNum};
    RecordDoBackupRes(bundleName_, res, doBackupInfo);
    return res;
}

tuple<ErrCode, uint32_t, uint32_t> BackupExtExtension::CalculateDataSize(const BJsonEntityExtensionConfig &usrConfig,
    int64_t &totalSize, TarMap &bigFileInfo, map<string, size_t> &smallFiles)
{
    HILOGI("Start scanning files and calculate datasize");
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
    if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
        HILOGE("mkdir failed path :%{public}s, err = %{public}d", path.c_str(), errno);
        return {errno, 0, 0};
    }

    vector<string> includes = usrConfig.GetIncludes();
    vector<string> excludes = usrConfig.GetExcludes();

    // 扫描文件计算数据量
    tie(bigFileInfo, smallFiles) = GetFileInfos(includes, excludes);
    ScanFileSingleton::GetInstance().SetCompeletedFlag(true);
    appStatistic_->smallFileCount_ = smallFiles.size();
    appStatistic_->bigFileCount_ = bigFileInfo.size();
    for (const auto &item : bigFileInfo) {
        int64_t fileSize = static_cast<int64_t>(std::get<1>(item.second).st_size);
        HILOGD("bigfile size = %{public}" PRId64 "", fileSize);
        totalSize += fileSize;
    }
    appStatistic_->bigFileSize_ = static_cast<uint64_t>(totalSize);
    HILOGI("bigfile size = %{public}" PRId64 "", totalSize);
    for (const auto &item : smallFiles) {
        UpdateFileStat(item.first, item.second);
        totalSize += static_cast<int64_t>(item.second);
    }
    appStatistic_->smallFileSize_ = static_cast<uint64_t>(totalSize - appStatistic_->bigFileSize_);
    HILOGI("scanning end, Datasize = %{public}" PRId64 "", totalSize);
    return {ERR_OK, static_cast<uint32_t>(includes.size()), static_cast<uint32_t>(excludes.size())};
}

bool BackupExtExtension::RefreshDataSize(int64_t totalSize)
{
    HILOGI("RefreshDataSize start");
    if (totalSize == 0) {
        HILOGI("no backup datasize, don't need to refresh");
        return true;
    }
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to obtain the ServiceClient handle");
        return false;
    }
    HILOGI("start RefreshDatasize by ipc");
    auto ret = proxy->RefreshDataSize(totalSize);
    if (ret != ERR_OK) {
        HILOGE("RefreshDataSize failed, ret = %{public}d", ret);
        return false;
    }
    HILOGI("RefreshDataSize end");
    return true;
}

int BackupExtExtension::DoRestore(const string &fileName, const off_t fileSize)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Do restore");
    if (extension_ == nullptr) {
        HILOGE("Failed to do restore, extension is nullptr");
        throw BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr");
    }
    if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
        return EPERM;
    }
    // REM: 给定version
    // REM: 解压启动Extension时即挂载好的备份目录中的数据
    string path = GetRestoreTempPath(bundleName_);
    string tarName = path + fileName;

    // 当用户指定fullBackupOnly字段或指定版本的恢复，解压目录当前在/backup/restore
    if (!extension_->SpecialVersionForCloneAndCloud() && !extension_->UseFullBackupOnly()) {
        path = "/";
    }
    auto [ret, fileInfos, errInfos] = UntarFile::GetInstance().UnPacket(tarName, path);
    if (isDebug_) {
        if (ret != 0) {
            endFileInfos_[tarName] = fileSize;
            errFileInfos_[tarName] = { ret };
        }
        endFileInfos_.merge(fileInfos);
        errFileInfos_.merge(errInfos);
    }
    if (BDir::CheckAndRmSoftLink(tarName) || BDir::CheckAndRmSoftLink(fileInfos)) {
        HILOGE("File soft links are forbidden");
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE).GetCode();
    }
    if (ret != 0) {
        HILOGE("Failed to untar file = %{public}s, err = %{public}d", tarName.c_str(), ret);
        return ret;
    }
    HILOGI("Application recovered successfully, package path is %{public}s", tarName.c_str());
    if (!isClearData_) {
        HILOGI("configured not clear data");
        return ERR_OK;
    }
    if (!RemoveFile(tarName)) {
        HILOGE("Failed to delete the backup tar %{public}s", tarName.c_str());
    }
    return ERR_OK;
}

void BackupExtExtension::GetTarIncludes(const string &tarName, unordered_map<string, struct ReportFileInfo> &infos)
{
    // 获取简报文件内容
    string reportName = GetReportFileName(tarName);

    UniqueFd fd(open(reportName.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open report file = %{private}s, err = %{public}d", reportName.c_str(), errno);
        return;
    }

    // 获取简报内容
    BReportEntity rp(move(fd));
    rp.GetReportInfos(infos);
}

int BackupExtExtension::DealIncreUnPacketResult(const off_t tarFileSize, const std::string &tarFileName,
    const std::tuple<int, EndFileInfo, ErrFileInfo> &result)
{
    int err = std::get<FIRST_PARAM>(result);
    EndFileInfo tmpEndInfo = std::get<SECOND_PARAM>(result);
    if (!isRpValid_) {
        if (err != ERR_OK) {
            endFileInfos_[tarFileName] = tarFileSize;
            errFileInfos_[tarFileName] = {err};
        }
        endFileInfos_.merge(tmpEndInfo);
    }
    if (BDir::CheckAndRmSoftLink(tmpEndInfo)) {
        HILOGE("File soft links are forbidden");
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE).GetCode();
    }
    ErrFileInfo tmpErrInfo = std::get<THIRD_PARAM>(result);
    errFileInfos_.merge(tmpErrInfo);
    return ERR_OK;
}

int BackupExtExtension::DoIncrementalRestore()
{
    HILOGI("Do incremental restore");
    if (extension_ == nullptr) {
        HILOGE("Failed to do incremental restore, extension is nullptr");
        throw BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr");
    }
    auto fileSet = GetIdxFileData(bundleName_);
    auto extManageInfo = GetExtManageInfo();
    ErrCode err = ERR_OK;
    auto startTime = std::chrono::system_clock::now();
    for (const auto &item : fileSet) { // 处理要解压的tar文件
        off_t tarFileSize = 0;
        if (ExtractFileExt(item) == "tar" && !IsUserTar(item, extManageInfo, tarFileSize)) {
            if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
                return EPERM;
            }
            radarRestoreInfo_.tarFileNum++;
            radarRestoreInfo_.tarFileSize += static_cast<uint64_t>(tarFileSize);
            // REM: 给定version
            // REM: 解压启动Extension时即挂载好的备份目录中的数据
            string path = GetRestoreTempPath(bundleName_);
            string tarName = path + item;

            // 当用户指定fullBackupOnly字段或指定版本的恢复，解压目录当前在/backup/restore
            if (!BDir::IsFilePathValid(tarName) || BDir::CheckAndRmSoftLink(tarName)) {
                HILOGE("Check incre tarfile path : %{public}s err, path is forbidden", GetAnonyPath(tarName).c_str());
                return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE).GetCode();
            }
            tarName = TarFile::GetInstance().DecompressTar(tarName);
            unordered_map<string, struct ReportFileInfo> result;
            GetTarIncludes(tarName, result);
            if ((!extension_->SpecialVersionForCloneAndCloud()) && (!extension_->UseFullBackupOnly())) {
                path = "/";
            }
            if (isDebug_) {
                FillEndFileInfos(path, result);
            }
            std::tuple<int, EndFileInfo, ErrFileInfo> unPacketRes =
                UntarFile::GetInstance().IncrementalUnPacket(tarName, path, result);
            err = std::get<FIRST_PARAM>(unPacketRes);
            if (int tmpErr = DealIncreUnPacketResult(tarFileSize, item, unPacketRes); tmpErr != ERR_OK) {
                return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE).GetCode();
            }
            HILOGI("Application recovered successfully, package path is %{public}s", tarName.c_str());
            DeleteBackupIncrementalTars(tarName);
        }
    }
    auto endTime = std::chrono::system_clock::now();
    radarRestoreInfo_.tarFileSpendTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    return err;
}

void BackupExtExtension::AsyncTaskBackup(const string config)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto task = [obj {wptr<BackupExtExtension>(this)}, config]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        try {
            ptr->CalculateDataSizeTask(config);
        } catch (const BError &e) {
            HILOGE("extension: AsyncTaskBackup error, err code:%{public}d", e.GetCode());
            ScanFileSingleton::GetInstance().SetCompeletedFlag(true);
            ptr->AppDone(e.GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ScanFileSingleton::GetInstance().SetCompeletedFlag(true);
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
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

    auto dobackupTask = [obj {wptr<BackupExtExtension>(this)}, config]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        try {
            ptr->DoBackUpTask(config);
        } catch (const BError &e) {
            HILOGE("extension: AsyncTaskBackup error, err code:%{public}d", e.GetCode());
            ptr->AppDone(e.GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        }
        ptr->DoClear();
    };

    doBackupPool_.AddTask([dobackupTask]() {
        try {
            dobackupTask();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

void BackupExtExtension::RestoreBigFilesForSpecialCloneCloud(const ExtManageInfo &item)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (isDebug_) {
        endFileInfos_[item.hashName] = item.sta.st_size;
    }
    const struct stat &sta = item.sta;
    string fileName = item.hashName;
    if (!BDir::IsFilePathValid(fileName)) {
        HILOGE("Check big special file path : %{public}s err, path is forbidden", GetAnonyPath(fileName).c_str());
        errFileInfos_[fileName].emplace_back(DEFAULT_INVAL_VALUE);
        if (!RemoveFile(fileName)) {
            HILOGE("Failed to delete the backup bigFile %{public}s", GetAnonyPath(fileName).c_str());
        }
        return;
    }
    if (BDir::CheckAndRmSoftLink(fileName)) {
        HILOGE("File soft links are forbidden");
        return;
    }
    if (chmod(fileName.c_str(), sta.st_mode) != 0) {
        HILOGE("Failed to chmod filePath, err = %{public}d", errno);
        errFileInfos_[fileName].emplace_back(errno);
    }
    struct timespec tv[2] = {sta.st_atim, sta.st_mtim};
    UniqueFd fd(open(fileName.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open file = %{public}s, err = %{public}d", GetAnonyPath(fileName).c_str(), errno);
        errFileInfos_[fileName].emplace_back(errno);
        return;
    }
    if (futimens(fd.Get(), tv) != 0) {
        errFileInfos_[fileName].emplace_back(errno);
        HILOGE("Failed to change the file time. %{public}s , %{public}d", GetAnonyPath(fileName).c_str(), errno);
    }
    RmBigFileReportForSpecialCloneCloud(fileName);
}

ErrCode BackupExtExtension::RestoreTarForSpecialCloneCloud(const ExtManageInfo &item)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    string tarName = item.hashName;
    if (item.fileName.empty()) {
        HILOGE("Invalid untar path info for tar %{public}s", tarName.c_str());
        return ERR_INVALID_VALUE;
    }
    string untarPath = item.fileName;
    if (untarPath.back() != BConstants::FILE_SEPARATOR_CHAR) {
        untarPath += BConstants::FILE_SEPARATOR_CHAR;
    }
    HILOGI("Start to untar file = %{public}s, untarPath = %{public}s", GetAnonyPath(item.hashName).c_str(),
        GetAnonyPath(untarPath).c_str());
    if (!BDir::IsFilePathValid(tarName)) {
        HILOGE("Check spec tarfile hash path : %{public}s err, path is forbidden", GetAnonyPath(tarName).c_str());
        return ERR_INVALID_VALUE;
    }
    if (BDir::CheckAndRmSoftLink(tarName)) {
        HILOGE("File soft links are forbidden");
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE).GetCode();
    }
    if (!BDir::IsFilePathValid(untarPath)) {
        HILOGE("Check spec tarfile path : %{public}s err, path is forbidden", GetAnonyPath(untarPath).c_str());
        return ERR_INVALID_VALUE;
    }
    if (IfCloudSpecialRestore(tarName)) {
        auto ret = CloudSpecialRestore(tarName, untarPath, item.sta.st_size);
        return ret;
    }
    auto [err, fileInfos, errInfos] = UntarFile::GetInstance().UnPacket(tarName, untarPath);
    if (isDebug_) {
        endFileInfos_.merge(fileInfos);
        errFileInfos_.merge(errInfos);
    }
    if (BDir::CheckAndRmSoftLink(fileInfos)) {
        HILOGE("File soft links are forbidden");
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE).GetCode();
    }
    DeleteBackupIncrementalTars(tarName);
    if (err != ERR_OK) {
        HILOGE("Failed to untar file = %{public}s, err = %{public}d", tarName.c_str(), err);
        return err;
    }
    return ERR_OK;
}

ErrCode BackupExtExtension::RestoreTarListForSpecialCloneCloud(const std::vector<const ExtManageInfo> &tarList)
{
    if (tarList.empty()) {
        HILOGI("no tar files, bundle: %{public}s", bundleName_.c_str());
        return ERR_OK;
    }
    bool isSplit = CheckIsSplitTarList(tarList);
    if (!isSplit) {
        HILOGI("tar list unsplit, bundle: %{public}s", bundleName_.c_str());
        return RestoreUnSplitTarListForSpecialCloneCloud(tarList);
    }

    HILOGI("tar list split, bundle: %{public}s", bundleName_.c_str());
    return RestoreSplitTarListForSpecialCloneCloud(tarList);
}

bool BackupExtExtension::CheckIsSplitTarList(const std::vector<const ExtManageInfo> &tarList)
{
    auto *unSplitTar = new installd::UnTarFile(nullptr);
    bool isSplit = false;
    for (const auto &item : tarList) {
        // reset untar object
        unSplitTar->Reset();

        HILOGI("check if split tar, filename: %{public}s, path: %{public}s", GetAnonyPath(item.hashName).c_str(),
            GetAnonyPath(item.fileName).c_str());
        // check if tar is split
        isSplit = unSplitTar->CheckIsSplitTar(item.hashName, item.fileName);
        if (isSplit) {
            HILOGI("check is split tar, filename: %{public}s, path: %{public}s", GetAnonyPath(item.hashName).c_str(),
                GetAnonyPath(item.fileName).c_str());
            break;
        }
    }
    // destory untar object
    delete unSplitTar;
    return isSplit;
}

ErrCode BackupExtExtension::RestoreUnSplitTarListForSpecialCloneCloud(const std::vector<const ExtManageInfo> &tarList)
{
    for (const auto &item : tarList) {
        // 待解压tar文件处理
        HILOGI("untar unsplit, filename: %{public}s, path: %{public}s", GetAnonyPath(item.hashName).c_str(),
            GetAnonyPath(item.fileName).c_str());
        radarRestoreInfo_.tarFileNum++;
        radarRestoreInfo_.tarFileSize += static_cast<uint64_t>(item.sta.st_size);
        int ret = RestoreTarForSpecialCloneCloud(item);
        if (isDebug_ && ret != ERR_OK) {
            errFileInfos_[item.hashName].emplace_back(ret);
            endFileInfos_[item.hashName] = item.sta.st_size;
        }
        if (ret != ERR_OK) {
            HILOGE("Failed to restore tar file %{public}s", item.hashName.c_str());
            return ERR_INVALID_VALUE;
        }
    }
    return ERR_OK;
}

ErrCode BackupExtExtension::RestoreSplitTarListForSpecialCloneCloud(const std::vector<const ExtManageInfo> &tarList)
{
    auto *unSplitTar = new installd::UnTarFile(nullptr);
    ErrCode errCode = ERR_OK;
    for (const auto &item : tarList) {
        radarRestoreInfo_.tarFileNum++;
        radarRestoreInfo_.tarFileSize += static_cast<uint64_t>(item.sta.st_size);
        // reset untar object
        unSplitTar->Reset();

        // do untar with root path
        int ret = unSplitTar->UnSplitTar(item.hashName, item.fileName);
        if (isDebug_ && ret != ERR_OK) {
            errFileInfos_[item.hashName].emplace_back(ret);
            endFileInfos_[item.hashName] = item.sta.st_size;
        }
        if (ret != ERR_OK) {
            HILOGE("Failed to restore tar file %{public}s", item.hashName.c_str());
            errCode = ERR_INVALID_VALUE;
            break;
        }
        if (!RemoveFile(item.hashName)) {
            HILOGE("Failed to delete the backup split tar %{public}s", item.hashName.c_str());
        }
    }
    // destory untar object
    delete unSplitTar;
    return errCode;
}

ErrCode BackupExtExtension::RestoreFilesForSpecialCloneCloud()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    // 获取索引文件内容
    UniqueFd fd(open(INDEX_FILE_RESTORE.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open index json file = %{private}s, err = %{public}d", INDEX_FILE_RESTORE.c_str(), errno);
        return errno;
    }
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManageInfo();
    HILOGI("Start do restore for SpecialCloneCloud.");
    auto startTime = std::chrono::system_clock::now();
    auto tarList = std::vector<const ExtManageInfo>();
    for (const auto &item : info) {
        if (item.hashName.empty()) {
            HILOGE("Hash name empty");
            continue;
        }
        if (item.isUserTar || item.isBigFile) {
            // 大文件处理
            radarRestoreInfo_.bigFileNum++;
            radarRestoreInfo_.bigFileSize += static_cast<uint64_t>(item.sta.st_size);
            RestoreBigFilesForSpecialCloneCloud(item);
        } else {
            tarList.emplace_back(item);
        }
    }

    int ret = RestoreTarListForSpecialCloneCloud(tarList);
    if (ret != ERR_OK) {
        HILOGE("Failed to restore tar file %{public}s", bundleName_.c_str());
        return ERR_INVALID_VALUE;
    }

    DeleteIndexAndRpFile();
    auto endTime = std::chrono::system_clock::now();
    radarRestoreInfo_.totalFileSpendTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    RecordDoRestoreRes(bundleName_, "BackupExtExtension::RestoreFilesForSpecialCloneCloud", radarRestoreInfo_);
    HILOGI("End do restore for SpecialCloneCloud.");
    return ERR_OK;
}

void BackupExtExtension::DeleteIndexAndRpFile()
{
    if (!RemoveFile(INDEX_FILE_RESTORE)) {
        HILOGE("Failed to delete the backup index %{public}s", INDEX_FILE_RESTORE.c_str());
    }
    string reportManagePath = GetReportFileName(INDEX_FILE_RESTORE); // GetIncrementalFileHandle创建的空fd
    if (!RemoveFile(reportManagePath)) {
        HILOGE("Failed to delete the backup report index %{public}s", reportManagePath.c_str());
    }
}

static bool RestoreBigFilePrecheck(string &fileName, const string &path, const string &hashName, const string &filePath)
{
    if (filePath.empty()) {
        HILOGE("file path is empty. %{public}s", GetAnonyString(filePath).c_str());
        return false;
    }

    // 不带路径的文件名
    if (access(fileName.data(), F_OK) != 0) {
        HILOGE("file does not exist");
        return false;
    }

    // 目录不存在且只有大文件时，不能通过untar创建，需要检查并创建
    if (!BDir::CheckAndCreateDirectory(filePath)) {
        HILOGE("failed to create directory %{public}s", GetAnonyString(filePath).c_str());
        return false;
    }
    return true;
}

void BackupExtExtension::RestoreBigFileAfter(const string &filePath, const struct stat &sta)
{
    if (chmod(filePath.c_str(), sta.st_mode) != 0) {
        errFileInfos_[filePath].emplace_back(errno);
        HILOGE("Failed to chmod filePath, err = %{public}d", errno);
    }
    struct timespec tv[2] = {sta.st_atim, sta.st_mtim};
    UniqueFd fd(open(filePath.data(), O_RDONLY));
    if (fd < 0) {
        errFileInfos_[filePath].emplace_back(errno);
        HILOGE("Failed to open file = %{public}s, err = %{public}d", GetAnonyPath(filePath).c_str(), errno);
        return;
    }
    if (futimens(fd.Get(), tv) != 0) {
        errFileInfos_[filePath].emplace_back(errno);
        HILOGE("failed to change the file time. %{public}s , %{public}d", GetAnonyPath(filePath).c_str(), errno);
    }
}

void BackupExtExtension::RestoreOneBigFile(const std::string &path,
    const ExtManageInfo &item, const bool appendTargetPath)
{
    radarRestoreInfo_.bigFileNum++;
    radarRestoreInfo_.bigFileSize += static_cast<uint64_t>(item.sta.st_size);
    string itemHashName = item.hashName;
    string itemFileName = item.fileName;
    // check if item.hasName and fileName need decode by report item attribute
    string reportPath = GetReportFileName(path + item.hashName);
    UniqueFd fd(open(reportPath.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open report file = %{public}s, err = %{public}d", reportPath.c_str(), errno);
        errFileInfos_[item.hashName].emplace_back(errno);
        throw BError(BError::Codes::EXT_INVAL_ARG, string("open report file failed"));
    }
    BReportEntity rp(move(fd));
    rp.CheckAndUpdateIfReportLineEncoded(itemFileName);

    string fileName = path + itemHashName;
    string filePath = appendTargetPath ? (path + itemFileName) : itemFileName;
    if (!BDir::IsFilePathValid(filePath)) {
        HILOGE("Check big file path : %{public}s err, path is forbidden", GetAnonyPath(filePath).c_str());
        return;
    }
    if (BDir::CheckAndRmSoftLink(fileName)) {
        HILOGE("File soft links are forbidden");
        return;
    }
    if (isDebug_) {
        endFileInfos_[filePath] = item.sta.st_size;
    }

    if (!RestoreBigFilePrecheck(fileName, path, item.hashName, filePath)) {
        return;
    }
    if (!BFile::MoveFile(fileName, filePath)) {
        errFileInfos_[filePath].emplace_back(errno);
        HILOGE("failed to move the file. err = %{public}d", errno);
        return;
    }
    if (BDir::CheckAndRmSoftLink(filePath)) {
        HILOGE("File soft links are forbidden");
        return;
    }
    RestoreBigFileAfter(filePath, item.sta);
}

void BackupExtExtension::RestoreBigFiles(bool appendTargetPath)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    // 获取索引文件内容
    string path = GetRestoreTempPath(bundleName_);
    string indexFileRestorePath = GetIndexFileRestorePath(bundleName_);
    UniqueFd fd(open(indexFileRestorePath.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open index json file = %{private}s, err = %{public}d", indexFileRestorePath.c_str(), errno);
        return;
    }
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManageInfo();
    HILOGI("Start Restore Big Files");
    auto start = std::chrono::system_clock::now();
    for (const auto &item : info) {
        if (item.hashName.empty() || (!item.isUserTar && !item.isBigFile)) {
            continue;
        }
        RestoreOneBigFile(path, item, appendTargetPath);
    }
    auto end = std::chrono::system_clock::now();
    radarRestoreInfo_.bigFileSpendTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    HILOGI("End Restore Big Files");
}

void BackupExtExtension::FillEndFileInfos(const std::string &path,
    const unordered_map<string, struct ReportFileInfo> &result)
{
    isRpValid_ = result.size() > 0;
    if (!isRpValid_) {
        return;
    }
    for (const auto &it : result) {
        std::string filePath = it.first;
        if (!filePath.empty() && filePath.size() <= PATH_MAX) {
            endFileInfos_[path + filePath] = it.second.size;
        } else {
            HILOGE("File name : %{public}s check error", GetAnonyPath(path + filePath).c_str());
        }
    }
}

void BackupExtExtension::DeleteBackupIdxFile()
{
    if (!isClearData_) {
        HILOGI("configured not clear data.");
        return;
    }
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    string indexFileRestorePath = GetIndexFileRestorePath(bundleName_);
    if (!RemoveFile(indexFileRestorePath)) {
        HILOGE("Failed to delete the backup index %{public}s", indexFileRestorePath.c_str());
    }
    HILOGI("End execute DeleteBackupIdxFile");
}

void BackupExtExtension::DeleteBackupIncrementalIdxFile()
{
    if (!isClearData_) {
        HILOGI("configured not clear data.");
        return;
    }
    string indexFileRestorePath = GetIndexFileRestorePath(bundleName_);
    if (!RemoveFile(indexFileRestorePath)) {
        HILOGE("Failed to delete the backup index %{public}s", indexFileRestorePath.c_str());
    }
    string reportManagePath = GetReportFileName(indexFileRestorePath); // GetIncrementalFileHandle创建的空fd
    if (!RemoveFile(reportManagePath)) {
        HILOGE("Failed to delete the backup report index %{public}s", reportManagePath.c_str());
    }
}

void BackupExtExtension::DeleteBackupIncrementalTars(const string &tarName)
{
    if (!isClearData_) {
        HILOGI("configured not need clear data");
        return;
    }
    if (!RemoveFile(tarName)) {
        HILOGE("Failed to delete the backup tar %{private}s, err = %{public}d", tarName.c_str(), errno);
    }
    // 删除简报文件
    string reportPath = GetReportFileName(tarName);
    if (!RemoveFile(reportPath)) {
        HILOGE("Failed to delete backup report %{private}s, err = %{public}d", reportPath.c_str(), errno);
    }
}

void BackupExtExtension::HandleSpecialVersionRestore()
{
    auto ret = RestoreFilesForSpecialCloneCloud();
    if (ret == ERR_OK) {
        AsyncTaskRestoreForUpgrade();
    } else {
        AppDone(ret);
        DoClear();
    }
}

void BackupExtExtension::AsyncTaskRestore(std::set<std::string> fileSet,
    const std::vector<ExtManageInfo> extManageInfo)
{
    auto task = [obj {wptr<BackupExtExtension>(this)}, fileSet {fileSet}, extManageInfo {extManageInfo}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        try {
            int ret = ERR_OK;
            if (ptr->extension_->SpecialVersionForCloneAndCloud()) {
                ptr->HandleSpecialVersionRestore();
                return;
            }
            // 解压
            for (const auto &item : fileSet) { // 处理要解压的tar文件
                off_t tarFileSize = 0;
                if (ExtractFileExt(item) == "tar" && !IsUserTar(item, extManageInfo, tarFileSize)) {
                    ret = ptr->DoRestore(item, tarFileSize);
                }
            }
            // 恢复用户tar包以及大文件
            // 目的地址是否需要拼接path(临时目录)，FullBackupOnly为true并且非特殊场景
            bool appendTargetPath =
                ptr->extension_->UseFullBackupOnly() && !ptr->extension_->SpecialVersionForCloneAndCloud();
            ptr->RestoreBigFiles(appendTargetPath);
            ptr->DeleteBackupIdxFile();
            if (ret == ERR_OK) {
                ptr->AsyncTaskRestoreForUpgrade();
            } else {
                ptr->AppDone(ret);
                ptr->DoClear();
            }
        } catch (const BError &e) {
            ptr->AppDone(e.GetCode());
        } catch (const exception &e) {
            HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
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

int BackupExtExtension::DealIncreRestoreBigAndTarFile()
{
    if (isDebug_) {
        CheckTmpDirFileInfos();
    }
    auto startTime = std::chrono::system_clock::now();
    // 解压
    int ret = ERR_OK;
    ret = DoIncrementalRestore();
    if (ret != ERR_OK) {
        HILOGE("Do incremental restore err");
        return ret;
    }
    // 恢复用户tar包以及大文件
    // 目的地址是否需要拼接path(临时目录)，FullBackupOnly为true并且非特殊场景
    bool appendTargetPath =
        extension_->UseFullBackupOnly() && !extension_->SpecialVersionForCloneAndCloud();
    RestoreBigFiles(appendTargetPath);
    // delete 1.tar/manage.json
    DeleteBackupIncrementalIdxFile();
    if (isDebug_) {
        CheckRestoreFileInfos();
    }
    auto endTime = std::chrono::system_clock::now();
    radarRestoreInfo_.totalFileSpendTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    RecordDoRestoreRes(bundleName_, "BackupExtExtension::AsyncTaskIncrementalRestore",
        radarRestoreInfo_);
    return ret;
}

void BackupExtExtension::AsyncTaskIncrementalRestore()
{
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        try {
            int ret = ptr->DealIncreRestoreBigAndTarFile();
            if (ret == ERR_OK) {
                HILOGI("after extra, do incremental restore.");
                ptr->AsyncTaskIncrementalRestoreForUpgrade();
            } else {
                ptr->AppIncrementalDone(ret);
                ptr->DoClear();
            }
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

void BackupExtExtension::AsyncTaskIncreRestoreSpecialVersion()
{
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        try {
            if (ptr != nullptr && ptr->isDebug_) {
                ptr->CheckTmpDirFileInfos(true);
            }
            int ret = ptr->RestoreFilesForSpecialCloneCloud();
            if (ptr != nullptr && ptr->isDebug_) {
                ptr->CheckRestoreFileInfos();
            }
            if (ret == ERR_OK) {
                ptr->AsyncTaskIncrementalRestoreForUpgrade();
            } else {
                ptr->AppIncrementalDone(ret);
                ptr->DoClear();
            }
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

void BackupExtExtension::AsyncTaskRestoreForUpgrade()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        try {
            HILOGI("On restore, start ext timer begin.");
            bool isExtStart;
            ptr->StartExtTimer(isExtStart);
            if (!isExtStart) {
                HILOGE("On restore, start ext timer fail.");
                return;
            }
            HILOGI("On restore, start ext timer end.");
            ptr->curScenario_ = BackupRestoreScenario::FULL_RESTORE;
            if ((ptr->StartOnProcessTaskThread(obj, BackupRestoreScenario::FULL_RESTORE)) != ERR_OK) {
                HILOGE("Call onProcess result is timeout");
                return;
            }
            auto callBackup = ptr->OnRestoreCallback(obj);
            auto callBackupEx = ptr->OnRestoreExCallback(obj);
            ptr->UpdateOnStartTime();
            ErrCode err = ptr->extension_->OnRestore(callBackup, callBackupEx);
            if (err != ERR_OK) {
                ptr->AppDone(BError::GetCodeByErrno(err));
                ptr->DoClear();
            }
        } catch (const BError &e) {
            ptr->AppDone(e.GetCode());
            ptr->DoClear();
        } catch (const exception &e) {
            HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
            ptr->DoClear();
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
            ptr->DoClear();
        }
    };
    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

void BackupExtExtension::ExtClear()
{
    HILOGI("ext begin clear");
    DoClear();
}

void BackupExtExtension::AsyncTaskIncrementalRestoreForUpgrade()
{
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        try {
            HILOGI("On incrementalRestore, start ext timer begin.");
            bool isExtStart;
            ptr->StartExtTimer(isExtStart);
            if (!isExtStart) {
                HILOGE("On incrementalRestore, start ext timer fail.");
                return;
            }
            HILOGI("On incrementalRestore, start ext timer end.");
            ptr->curScenario_ = BackupRestoreScenario::INCREMENTAL_RESTORE;
            if ((ptr->StartOnProcessTaskThread(obj, BackupRestoreScenario::INCREMENTAL_RESTORE)) != ERR_OK) {
                HILOGE("Call onProcess result is timeout");
                return;
            }
            auto callBackup = ptr->IncreOnRestoreCallback(obj);
            auto callBackupEx = ptr->IncreOnRestoreExCallback(obj);
            ptr->UpdateOnStartTime();
            ErrCode err = ptr->extension_->OnRestore(callBackup, callBackupEx);
            if (err != ERR_OK) {
                HILOGE("OnRestore done, err = %{public}d", err);
                ptr->AppIncrementalDone(BError::GetCodeByErrno(err));
                ptr->DoClear();
            }
        } catch (const BError &e) {
            ptr->AppIncrementalDone(e.GetCode());
            ptr->DoClear();
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
            ptr->DoClear();
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

void BackupExtExtension::DoClear()
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

void BackupExtExtension::DoClearInner()
{
    try {
        string backupCache = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
        string restoreCache = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
        string specialRestoreCache = GetRestoreTempPath(bundleName_);

        if (!ForceRemoveDirectoryBMS(backupCache)) {
            HILOGE("Failed to delete the backup cache %{public}s", backupCache.c_str());
        }

        if (!ForceRemoveDirectoryBMS(restoreCache)) {
            HILOGE("Failed to delete the restore cache %{public}s", restoreCache.c_str());
        }

        if (!ForceRemoveDirectoryBMS(specialRestoreCache)) {
            HILOGE("Failed to delete cache for filemanager or medialibrary %{public}s", specialRestoreCache.c_str());
        }
        // delete el1 backup/restore
        ForceRemoveDirectoryBMS(
            string(BConstants::PATH_BUNDLE_BACKUP_HOME_EL1).append(BConstants::SA_BUNDLE_BACKUP_BACKUP));
        ForceRemoveDirectoryBMS(
            string(BConstants::PATH_BUNDLE_BACKUP_HOME_EL1).append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
        // delete special directory
        if (bundleName_.compare(MEDIA_LIBRARY_BUNDLE_NAME) == 0) {
            ForceRemoveDirectoryBMS(
                string(BConstants::PATH_MEDIALDATA_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP));
            ForceRemoveDirectoryBMS(
                string(BConstants::PATH_MEDIALDATA_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
        }
        if (bundleName_.compare(FILE_MANAGER_BUNDLE_NAME) == 0) {
            ForceRemoveDirectoryBMS(
                string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP));
            ForceRemoveDirectoryBMS(
                string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
        }
    } catch (...) {
        HILOGE("Failed to clear");
    }
}

void BackupExtExtension::ReportAppStatistic(const std::string &func, ErrCode errCode)
{
    if (curScenario_ == BackupRestoreScenario::FULL_BACKUP ||
        curScenario_ == BackupRestoreScenario::INCREMENTAL_BACKUP) {
        appStatistic_->ReportBackup(func, errCode);
    } else {
        appStatistic_->untarSpend_ = static_cast<uint32_t>(radarRestoreInfo_.tarFileSpendTime);
        appStatistic_->bigFileSpend_ = static_cast<uint32_t>(radarRestoreInfo_.bigFileSpendTime);
        appStatistic_->doRestoreSpend_ = static_cast<uint32_t>(radarRestoreInfo_.totalFileSpendTime);
        appStatistic_->bigFileCount_ = radarRestoreInfo_.bigFileNum;
        appStatistic_->tarFileCount_ = radarRestoreInfo_.tarFileNum;
        appStatistic_->bigFileSize_ = radarRestoreInfo_.bigFileSize;
        appStatistic_->tarFileSize_ = radarRestoreInfo_.tarFileSize;
        appStatistic_->ReportRestore(func, errCode);
    }
}

void BackupExtExtension::AppDone(ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("AppDone Begin.");
    ReportAppStatistic("AppDone", errCode);
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to obtain the ServiceClient handle");
        DoClear();
        return;
    }
    auto ret = proxy->AppDone(errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed to notify the app done. err = %{public}d", ret);
    }
    if (HandleGetExtOnRelease()) {
        HandleExtOnRelease();
    }
}

void BackupExtExtension::AppResultReport(const std::string restoreRetInfo,
    BackupRestoreScenario scenario, ErrCode errCode)
{
    auto proxy = ServiceClient::GetInstance();
    BExcepUltils::BAssert(proxy, BError::Codes::EXT_BROKEN_IPC, "Failed to obtain the ServiceClient handle");
    HILOGI("restoreRetInfo is %{public}s", restoreRetInfo.c_str());
    auto ret = proxy->ServiceResultReport(restoreRetInfo, scenario, errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed notify app restoreResultReport, errCode: %{public}d", ret);
    }
}

void BackupExtExtension::StartExtTimer(bool &isExtStart)
{
    auto proxy = ServiceClient::GetInstance();
    BExcepUltils::BAssert(proxy, BError::Codes::EXT_BROKEN_IPC, "Failed to obtain the ServiceClient handle");
    HILOGI("Start ext timer by ipc.");
    auto ret = proxy->StartExtTimer(isExtStart);
    if (ret != ERR_OK) {
        HILOGE("Start ext timer failed, errCode: %{public}d", ret);
    }
}

void BackupExtExtension::StartFwkTimer(bool &isFwkStart)
{
    HILOGI("Do backup, start fwk timer begin.");
    auto proxy = ServiceClient::GetInstance();
    BExcepUltils::BAssert(proxy, BError::Codes::EXT_BROKEN_IPC, "Failed to obtain the ServiceClient handle");
    HILOGI("Start fwk timer by ipc.");
    auto ret = proxy->StartFwkTimer(isFwkStart);
    if (ret != ERR_OK) {
        HILOGE("Start fwk timer failed, errCode: %{public}d", ret);
    }
    HILOGI("Do backup, start fwk timer end.");
}

bool BackupExtExtension::StopExtTimer()
{
    HILOGI("StopExtTimer start");
    bool isExtStop;
    auto proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to obtain the ServiceClient handle");
        return false;
    }
    HILOGI("StopExtTimer by ipc");
    auto ret = proxy->StopExtTimer(isExtStop);
    if (ret != ERR_OK) {
        HILOGE("StopExtTimer failed, errcode :%{public}d", ret);
        return false;
    }
    if (!isExtStop) {
        HILOGE("StopExtTimer failed");
        return false;
    }
    HILOGI("StopExtTimer end");
    return true;
}

void BackupExtExtension::UpdateOnStartTime()
{
    std::lock_guard<std::mutex> lock(onStartTimeLock_);
    g_onStart = std::chrono::system_clock::now();
    appStatistic_->onBackupSpend_.Start();
    appStatistic_->onBackupexSpend_.Start();
}

void BackupExtExtension::AsyncTaskOnBackup()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        try {
            ptr->curScenario_ = BackupRestoreScenario::FULL_BACKUP;
            if ((ptr->StartOnProcessTaskThread(obj, BackupRestoreScenario::FULL_BACKUP)) != ERR_OK) {
                HILOGE("Call onProcess result is timeout");
                return;
            }
            auto callBackup = ptr->OnBackupCallback(obj);
            auto callBackupEx = ptr->OnBackupExCallback(obj);
            ptr->UpdateOnStartTime();
            ErrCode err = ptr->extension_->OnBackup(callBackup, callBackupEx);
            if (err != ERR_OK) {
                HILOGE("OnBackup done, err = %{public}d", err);
                ptr->AppDone(BError::GetCodeByErrno(err));
            }
        } catch (const BError &e) {
            ptr->AppDone(e.GetCode());
        } catch (const exception &e) {
            HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        }
    };

    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

ErrCode BackupExtExtension::HandleRestore(bool isClearData)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        VerifyCaller();
        SetClearDataFlag(isClearData);
        if (extension_ == nullptr) {
            HILOGE("Failed to handle restore, extension is nullptr");
            return BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr").GetCode();
        }
        if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
            HILOGE("Failed to get file handle, because action is %{public}d invalid", extension_->GetExtensionAction());
            return BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid").GetCode();
        }
        // read backup_config is allow to backup or restore
        if (!extension_->AllowToBackupRestore()) {
            HILOGE("Application does not allow backup or restore");
            return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE, "Application does not allow backup or restore")
                .GetCode();
        }

        // async do restore.
        if (extension_->WasFromSpecialVersion() && extension_->RestoreDataReady()) {
            HILOGI("Restore directly when upgrading.");
            AsyncTaskRestoreForUpgrade();
        }
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to handle restore");
        return BError(BError::Codes::EXT_BROKEN_IPC).GetCode();
    }
}

static bool IfEquality(const ReportFileInfo &info, const ReportFileInfo &infoAd)
{
    return info.filePath < infoAd.filePath;
}

static void AdDeduplication(vector<struct ReportFileInfo> &filesList)
{
    sort(filesList.begin(), filesList.end(), IfEquality);
    auto it = unique(filesList.begin(), filesList.end(), [](const ReportFileInfo &info, const ReportFileInfo &infoAd) {
        return info.filePath == infoAd.filePath;
        });
    filesList.erase(it, filesList.end());
}

void BackupExtExtension::FillFileInfos(UniqueFd incrementalFd,
                                       UniqueFd manifestFd,
                                       vector<struct ReportFileInfo> &allFiles,
                                       vector<struct ReportFileInfo> &smallFiles,
                                       vector<struct ReportFileInfo> &bigFiles)
{
    HILOGI("Begin Compare");
    BReportEntity cloudRp(move(manifestFd));
    unordered_map<string, struct ReportFileInfo> cloudFiles;
    cloudRp.GetReportInfos(cloudFiles);
    appStatistic_->scanFileSpend_.Start();
    if (cloudFiles.empty()) {
        FillFileInfosWithoutCmp(allFiles, smallFiles, bigFiles, move(incrementalFd));
    } else {
        FillFileInfosWithCmp(allFiles, smallFiles, bigFiles, cloudFiles, move(incrementalFd));
    }

    AdDeduplication(allFiles);
    AdDeduplication(smallFiles);
    AdDeduplication(bigFiles);
    appStatistic_->smallFileCount_ = smallFiles.size();
    appStatistic_->bigFileCount_ = bigFiles.size();
    appStatistic_->scanFileSpend_.End();
    HILOGI("End Compare, allfile is %{public}zu, smallfile is %{public}zu, bigfile is %{public}zu",
        allFiles.size(), smallFiles.size(), bigFiles.size());
}

static void WriteFile(const string &filename, const vector<struct ReportFileInfo> &srcFiles)
{
    fstream f;
    f.open(filename.data(), ios::out);
    if (!f) {
        HILOGE("Failed to open file = %{private}s", filename.c_str());
        return;
    }

    // 前面2行先填充进去
    f << "version=1.0&attrNum=8" << endl;
    f << "path;mode;dir;size;mtime;hash;usertar;encodeFlag" << endl;
    for (const auto &item : srcFiles) {
        string path = BReportEntity::EncodeReportItem(item.filePath, item.encodeFlag);
        string str = path + ";" + item.mode + ";" + to_string(item.isDir) + ";" + to_string(item.size);
        str += ";" + to_string(item.mtime) + ";" + item.hash + ";" + to_string(item.userTar)+ ";";
        if (item.encodeFlag) {
            str += std::to_string(1);
        } else {
            str += std::to_string(0);
        }
        f << str << endl;
    }
    f.close();
}

/**
 * 增量tar包和简报信息回传
 */
ErrCode BackupExtExtension::IncrementalTarFileReady(const TarMap &bigFileInfo,
                                                    const vector<struct ReportFileInfo> &srcFiles,
                                                    sptr<IService> proxy)
{
    string tarFile = bigFileInfo.begin()->first;
    string manageFile = GetReportFileName(tarFile);
    string file = string(INDEX_FILE_INCREMENTAL_BACKUP).append(manageFile);
    WriteFile(file, srcFiles);

    string tarName = string(INDEX_FILE_INCREMENTAL_BACKUP).append(tarFile);
    int fd = open(tarName.data(), O_RDONLY);
    int manifestFd = open(file.data(), O_RDONLY);
    ErrCode ret = (fd < 0 || manifestFd < 0) ?
                  proxy->AppIncrementalFileReadyWithoutFd(tarFile, ERR_OK) :
                  proxy->AppIncrementalFileReady(tarFile, fd, manifestFd, ERR_OK);
    if (SUCCEEDED(ret)) {
        HILOGI("IncrementalTarFileReady: The application is packaged successfully");
        // 删除文件
        RemoveFile(file);
        RemoveFile(tarName);
    } else {
        HILOGE("IncrementalTarFileReady interface fails to be invoked: %{public}d", ret);
    }
    return ret;
}

/**
 * 增量大文件和简报信息回传
 */
ErrCode BackupExtExtension::IncrementalBigFileReady(TarMap &pkgInfo,
    const vector<struct ReportFileInfo> &bigInfos, sptr<IService> proxy)
{
    ErrCode ret {ERR_OK};
    HILOGI("IncrementalBigFileReady Begin, pkgInfo size:%{public}zu", pkgInfo.size());
    auto startTime = std::chrono::system_clock::now();
    int fdNum = 0;
    vector<string> noPermissionFiles;
    for (const auto &item : pkgInfo) {
        if (item.first.empty()) {
            continue;
        }
        auto [path, sta, isBeforeTar] = item.second;
        int32_t errCode = ERR_OK;
        WaitToSendFd(startTime, fdNum);
        int fdval = open(path.data(), O_RDONLY);
        if (fdval < 0) {
            HILOGE("IncrementalBigFileReady open file failed, file name is %{public}s, err = %{public}d",
                GetAnonyString(path).c_str(), errno);
            errCode = errno;
            if (errCode == ERR_NO_PERMISSION) {
                noPermissionFiles.emplace_back(item.first.c_str());
                continue;
            }
        }
        vector<struct ReportFileInfo> bigInfo;
        for (const auto &tempFile : bigInfos) {
            if (tempFile.filePath == path) {
                bigInfo.emplace_back(tempFile);
                break;
            }
        }
        string file = GetReportFileName(string(INDEX_FILE_INCREMENTAL_BACKUP).append(item.first));
        WriteFile(file, bigInfo);
        int manifestFdval = open(file.data(), O_RDONLY);
        ErrCode ret = (fdval < 0 || manifestFdval < 0) ? proxy->AppIncrementalFileReadyWithoutFd(item.first, errCode) :
                      proxy->AppIncrementalFileReady(item.first, fdval, manifestFdval, errCode);
        CheckAppIncrementalFileReadyResult(ret, item.first, file);
        if (fdval >= 0) {
            close(fdval);
        }
        if (manifestFdval >=0) {
            close(manifestFdval);
        }
        fdNum += BConstants::FILE_AND_MANIFEST_FD_COUNT;
        RefreshTimeInfo(startTime, fdNum);
    }
    ClearNoPermissionFiles(pkgInfo, noPermissionFiles);
    HILOGI("IncrementalBigFileReady End");
    return ret;
}

void BackupExtExtension::CheckAppIncrementalFileReadyResult(int32_t ret, std::string packageName, std::string file)
{
    if (SUCCEEDED(ret)) {
        HILOGI("IncreBigFileReady: The app is packaged success, package name is %{public}s", packageName.c_str());
        RemoveFile(file);
    } else {
        HILOGE("IncrementalBigFileReady interface fails to be invoked: %{public}d", ret);
    }
}

ErrCode BackupExtExtension::IncrementalAllFileReady(const TarMap &pkgInfo,
    const vector<struct ReportFileInfo> &srcFiles, sptr<IService> proxy)
{
    UniqueFd fdIndex(open(INDEX_FILE_BACKUP.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR));
    if (fdIndex < 0) {
        HILOGE("Failed to open index json file = %{private}s, err = %{public}d", INDEX_FILE_BACKUP.c_str(), errno);
        return BError::GetCodeByErrno(errno);
    }
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(std::move(fdIndex));
    auto cache = cachedEntity.Structuralize();
    cache.SetExtManage(pkgInfo);
    cachedEntity.Persist();
    close(cachedEntity.GetFd().Release());

    string file = GetReportFileName(string(INDEX_FILE_INCREMENTAL_BACKUP).append("all"));
    WriteFile(file, srcFiles);
    int fdval = open(INDEX_FILE_BACKUP.data(), O_RDONLY);
    int manifestFdval = open(file.data(), O_RDONLY);
    ErrCode ret = (fdval < 0 || manifestFdval < 0) ?
            proxy->AppIncrementalFileReadyWithoutFd(string(BConstants::EXT_BACKUP_MANAGE), ERR_OK) :
            proxy->AppIncrementalFileReady(string(BConstants::EXT_BACKUP_MANAGE), fdval, manifestFdval, ERR_OK);
    if (SUCCEEDED(ret)) {
        HILOGI("IncrementalAllFileReady successfully");
        RemoveFile(file);
    } else {
        HILOGI("successfully but the IncrementalAllFileReady interface fails to be invoked: %{public}d", ret);
    }
    close(fdval);
    close(manifestFdval);
    return ret;
}

ErrCode BackupExtExtension::CleanBundleTempDir()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("BackupExtExtension::CleanBundleTempDir begin");
    if (extension_ == nullptr) {
        HILOGE("Failed to CleanBundleTempDir, extension is nullptr");
        return BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr").GetCode();
    }
    if (extension_->GetExtensionAction() == BConstants::ExtensionAction::INVALID) {
        return BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid").GetCode();
    }
    try {
        VerifyCaller();
        DoClearInner();
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to CleanBundleTempDir");
        return BError(BError::Codes::EXT_BROKEN_IPC).GetCode();
    }
}
} // namespace OHOS::FileManagement::Backup
