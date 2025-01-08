/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_filesystem/b_dir.h"
#include "b_filesystem/b_file.h"
#include "b_filesystem/b_file_hash.h"
#include "b_json/b_json_cached_entity.h"
#include "b_jsonutil/b_jsonutil.h"
#include "b_ohos/startup/backup_para.h"
#include "b_tarball/b_tarball_factory.h"
#include "filemgmt_libhilog.h"
#include "hitrace_meter.h"
#include "i_service.h"
#include "sandbox_helper.h"
#include "service_proxy.h"
#include "tar_file.h"
#include "b_anony/b_anony.h"

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

std::vector<ExtManageInfo> BackupExtExtension::GetExtManageInfo()
{
    string indexFileRestorePath = GetIndexFileRestorePath(bundleName_);
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

static bool CheckAndCreateDirectory(const string &filePath)
{
    size_t pos = filePath.rfind('/');
    if (pos == string::npos) {
        return true;
    }

    string folderPath = "/" + filePath.substr(0, pos);
    if (access(folderPath.c_str(), F_OK) != 0) {
        if (!ForceCreateDirectory(folderPath.data())) {
            return false;
        }
    }
    return true;
}

static UniqueFd GetFileHandleForSpecialCloneCloud(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("GetFileHandleForSpecialCloneCloud: fileName is %{public}s", GetAnonyPath(fileName).c_str());
    string filePath = fileName;
    if (fileName.front() != BConstants::FILE_SEPARATOR_CHAR) {
        filePath = BConstants::FILE_SEPARATOR_CHAR + fileName;
    }
    size_t filePathPrefix = filePath.find_last_of(BConstants::FILE_SEPARATOR_CHAR);
    if (filePathPrefix == string::npos) {
        HILOGE("GetFileHandleForSpecialCloneCloud: Invalid fileName");
        return UniqueFd(-1);
    }
    string path = filePath.substr(0, filePathPrefix);
    if (access(path.c_str(), F_OK) != 0) {
        bool created = ForceCreateDirectory(path.data());
        if (!created) {
            HILOGE("Failed to create restore folder.");
            return UniqueFd(-1);
        }
    }
    UniqueFd fd(open(fileName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
    if (fd < 0) {
        HILOGE("Open file failed, file name is %{private}s, err = %{public}d", fileName.data(), errno);
        return UniqueFd(-1);
    }
    return fd;
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

static string GetReportFileName(const string &fileName)
{
    string reportName = fileName + "." + string(BConstants::REPORT_FILE_EXT);
    return reportName;
}

static ErrCode GetIncreFileHandleForSpecialVersion(const string &fileName)
{
    ErrCode errCode = ERR_OK;
    UniqueFd fd = GetFileHandleForSpecialCloneCloud(fileName);
    if (fd < 0) {
        HILOGE("Failed to open file = %{private}s, err = %{public}d", fileName.c_str(), errno);
        errCode = errno;
    }

    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
        HILOGE("Failed to create restore folder : %{private}s, err = %{public}d", path.c_str(), errno);
        errCode = errno;
    }
    string reportName = path + BConstants::BLANK_REPORT_NAME;
    UniqueFd reportFd(open(reportName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
    if (reportFd < 0) {
        HILOGE("Failed to open report file = %{private}s, err = %{public}d", reportName.c_str(), errno);
        errCode = errno;
    }

    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to get file handle for special version clone");
        return BError(BError::Codes::EXT_BROKEN_BACKUP_SA).GetCode();
    }
    auto ret = proxy->AppIncrementalFileReady(fileName, move(fd), move(reportFd), errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed to AppIncrementalFileReady %{public}d", ret);
    }
    return ERR_OK;
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

ErrCode BackupExtExtension::GetIncreFileHandleForNormalVersion(const std::string &fileName)
{
    HILOGI("extension: GetIncrementalFileHandle single to single Name:%{public}s", GetAnonyPath(fileName).c_str());
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        throw BError(BError::Codes::EXT_BROKEN_IPC, string("Failed to AGetInstance"));
    }
    std::string tarName;
    int32_t errCode = ERR_OK;
    UniqueFd fd(-1);
    UniqueFd reportFd(-1);
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
            HILOGE("The report file already exists, Name = %{private}s, err =%{public}d", reportName.c_str(), errno);
        }
        reportFd = UniqueFd(open(reportName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
        if (reportFd < 0) {
            HILOGE("Failed to open report file = %{private}s, err = %{public}d", reportName.c_str(), errno);
            errCode = errno;
            break;
        }
    } while (0);
    HILOGI("extension: Will notify AppIncrementalFileReady");
    auto ret = proxy->AppIncrementalFileReady(fileName, move(fd), move(reportFd), errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed to AppIncrementalFileReady %{public}d", ret);
    }
    return ERR_OK;
}

ErrCode BackupExtExtension::GetIncrementalFileHandle(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if ((extension_ == nullptr) || (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE)) {
            HILOGE("Failed to get incremental file handle, extension or action is invalid, action %{public}d.",
                extension_->GetExtensionAction());
            throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
        }
        VerifyCaller();
        if (BDir::CheckFilePathInvalid(fileName)) {
            auto proxy = ServiceProxy::GetInstance();
            if (proxy == nullptr) {
                throw BError(BError::Codes::EXT_BROKEN_IPC, string("Failed to AGetInstance"));
            }
            HILOGE("Check file path : %{public}s err, path is forbidden", GetAnonyPath(fileName).c_str());
            auto ret = proxy->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG));
            if (ret != ERR_OK) {
                HILOGE("Failed to notify app incre done. err = %{public}d", ret);
            }
            return BError(BError::Codes::EXT_INVAL_ARG).GetCode();
        }
        if (extension_->SpecialVersionForCloneAndCloud()) {
            return GetIncreFileHandleForSpecialVersion(fileName);
        }
        return GetIncreFileHandleForNormalVersion(fileName);
    } catch (...) {
        HILOGE("Failed to get incremental file handle");
        DoClear();
        return BError(BError::Codes::EXT_BROKEN_IPC).GetCode();
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

static ErrCode IndexFileReady(const TarMap &pkgInfo, sptr<IService> proxy)
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

    ErrCode ret =
        proxy->AppFileReady(string(BConstants::EXT_BACKUP_MANAGE), UniqueFd(open(INDEX_FILE_BACKUP.data(), O_RDONLY)),
            ERR_OK);
    if (SUCCEEDED(ret)) {
        HILOGI("The application is packaged successfully");
    } else {
        HILOGI(
            "The application is packaged successfully but the AppFileReady interface fails to be invoked: "
            "%{public}d",
            ret);
    }
    HILOGI("End notify Appfile Ready");
    return ret;
}

ErrCode BackupExtExtension::BigFileReady(const TarMap &bigFileInfo, sptr<IService> proxy)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("BigFileReady Begin: bigFileInfo file size is: %{public}zu", bigFileInfo.size());
    ErrCode ret {ERR_OK};
    auto startTime = std::chrono::system_clock::now();
    int fdNum = 0;
    for (auto &item : bigFileInfo) {
        WaitToSendFd(startTime, fdNum);
        int32_t errCode = ERR_OK;
        string fllePath = std::get<0>(item.second);
        UniqueFd fd(open(fllePath.data(), O_RDONLY));
        if (fd < 0) {
            HILOGE("open file failed, file name is %{public}s, err = %{public}d", fllePath.c_str(), errno);
            errCode = errno;
        }

        ret = proxy->AppFileReady(item.first, std::move(fd), errCode);
        if (SUCCEEDED(ret)) {
            HILOGI("The application is packaged successfully, package name is %{public}s", item.first.c_str());
        } else {
            HILOGW("Current file execute app file ready interface failed, ret is:%{public}d", ret);
        }
        fdNum++;
        RefreshTimeInfo(startTime, fdNum);
    }
    HILOGI("BigFileReady End");
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

static pair<TarMap, map<string, size_t>> GetFileInfos(const vector<string> &includes, const vector<string> &excludes)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto [errCode, files, smallFiles] = BDir::GetBigFiles(includes, excludes);
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
        string md5Name = getStringHash(bigFiles, item.first);
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
    UniqueFd fd(open(tarPath.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("TarFileReady open file failed, file name is %{public}s, err = %{public}d", tarName.c_str(), errno);
        errCode = errno;
    }
    int ret = proxy->AppFileReady(tarName, std::move(fd), errCode);
    if (SUCCEEDED(ret)) {
        HILOGI("TarFileReady: AppFileReady success for %{public}s", tarName.c_str());
        // 删除文件
        RemoveFile(tarPath);
    } else {
        HILOGE("TarFileReady AppFileReady fail to be invoked for %{public}s: ret = %{public}d", tarName.c_str(), ret);
    }
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
    for (auto small : srcFiles) {
        totalSize += small.second;
        fileCount += 1;
        packFiles.emplace_back(small.first);
        if (totalSize >= BConstants::DEFAULT_SLICE_SIZE || fileCount >= BConstants::MAX_FILE_COUNT) {
            TarMap tarMap {};
            TarFile::GetInstance().Packet(packFiles, "part", path, tarMap, reportCb);
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
        TarFile::GetInstance().Packet(packFiles, "part", path, tarMap, reportCb);
        TarFileReady(tarMap, proxy);
        fdNum = 1;
        WaitToSendFd(startTime, fdNum);
        tar.insert(tarMap.begin(), tarMap.end());
        packFiles.clear();
        RefreshTimeInfo(startTime, fdNum);
    }
}

int BackupExtExtension::DoBackup(const BJsonEntityExtensionConfig &usrConfig)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Start Do backup");
    auto start = std::chrono::system_clock::now();
    if (extension_ == nullptr) {
        HILOGE("Failed to do backup, extension is nullptr");
        throw BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr");
    }
    if (extension_->GetExtensionAction() != BConstants::ExtensionAction::BACKUP) {
        return EPERM;
    }

    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
    if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
        throw BError(errno);
    }

    vector<string> includes = usrConfig.GetIncludes();
    vector<string> excludes = usrConfig.GetExcludes();
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
    }

    // 大文件处理
    HILOGI("Start packet bigfiles and small files");
    auto [bigFileInfo, smallFiles] = GetFileInfos(includes, excludes);
    for (const auto &item : bigFileInfo) {
        auto filePath = std::get<0>(item.second);
        if (!filePath.empty()) {
            excludes.push_back(filePath);
        }
    }

    // 回传大文件
    HILOGI("Will notify BigFileReady");
    auto res = BigFileReady(bigFileInfo, proxy);

    HILOGI("Start packet Tar files");
    // 分片打包， 回传tar包
    TarMap tarMap {};
    DoPacket(smallFiles, tarMap, proxy);
    bigFileInfo.insert(tarMap.begin(), tarMap.end());
    HILOGI("Do backup, DoPacket end");

    HILOGI("Will notify IndexFileReady");
    if (auto ret = IndexFileReady(bigFileInfo, proxy); ret) {
        return ret;
    }

    HILOGI("HandleBackup finish, ret = %{public}d", res);
    auto end = std::chrono::system_clock::now();
    auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    AppRadar::DoBackupInfo doBackupInfo = {cost, bigFileInfo.size(), smallFiles.size(), tarMap.size(),
                                           includes.size(), excludes.size()};
    RecordDoBackupRes(bundleName_, res, doBackupInfo);
    return res;
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

static unordered_map<string, struct ReportFileInfo> GetTarIncludes(const string &tarName)
{
    // 获取简报文件内容
    string reportName = GetReportFileName(tarName);

    UniqueFd fd(open(reportName.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open report file = %{private}s, err = %{public}d", reportName.c_str(), errno);
        return {};
    }

    // 获取简报内容
    BReportEntity rp(move(fd));
    return rp.GetReportInfos();
}

void BackupExtExtension::DealIncreUnPacketResult(const off_t tarFileSize, const std::string &tarFileName,
    const std::tuple<int, EndFileInfo, ErrFileInfo> &result)
{
    if (!isDebug_) {
        return;
    }
    int err = std::get<FIRST_PARAM>(result);
    if (!isRpValid_) {
        if (err != ERR_OK) {
            endFileInfos_[tarFileName] = tarFileSize;
            errFileInfos_[tarFileName] = {err};
        } else {
            EndFileInfo tmpEndInfo = std::get<SECOND_PARAM>(result);
            endFileInfos_.merge(tmpEndInfo);
        }
    }
    ErrFileInfo tmpErrInfo = std::get<THIRD_PARAM>(result);
    errFileInfos_.merge(tmpErrInfo);
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
    std::tuple<int, EndFileInfo, ErrFileInfo> unPacketRes;
    ErrCode err = ERR_OK;
    auto startTime = std::chrono::system_clock::now();
    for (auto item : fileSet) { // 处理要解压的tar文件
        off_t tarFileSize = 0;
        if (ExtractFileExt(item) == "tar" && !IsUserTar(item, extManageInfo, tarFileSize)) {
            if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
                return EPERM;
            }
            auto iter = find_if(extManageInfo.begin(), extManageInfo.end(),
                [&item](const auto &itemManage) { return itemManage.hashName == item; });
            if (iter != extManageInfo.end()) {
                tarFileSize = iter->sta.st_size;
            }
            radarRestoreInfo_.tarFileNum++;
            radarRestoreInfo_.tarFileSize += static_cast<uint64_t>(tarFileSize);
            // REM: 给定version
            // REM: 解压启动Extension时即挂载好的备份目录中的数据
            string path = GetRestoreTempPath(bundleName_);
            string tarName = path + item;

            // 当用户指定fullBackupOnly字段或指定版本的恢复，解压目录当前在/backup/restore
            if (BDir::CheckFilePathInvalid(tarName)) {
                HILOGE("Check incre tarfile path : %{public}s err, path is forbidden", GetAnonyPath(tarName).c_str());
                return ERR_INVALID_VALUE;
            }
            unordered_map<string, struct ReportFileInfo> result = GetTarIncludes(tarName);
            if ((!extension_->SpecialVersionForCloneAndCloud()) && (!extension_->UseFullBackupOnly())) {
                path = "/";
            }
            if (isDebug_) {
                FillEndFileInfos(path, result);
            }
            unPacketRes = UntarFile::GetInstance().IncrementalUnPacket(tarName, path, result);
            err = std::get<FIRST_PARAM>(unPacketRes);
            DealIncreUnPacketResult(tarFileSize, item, unPacketRes);
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
            HILOGI("Do backup, start fwk timer begin.");
            bool isFwkStart;
            ptr->StartFwkTimer(isFwkStart);
            if (!isFwkStart) {
                HILOGE("Do backup, start fwk timer fail.");
                return;
            }
            HILOGI("Do backup, start fwk timer end.");
            BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(config);
            auto cache = cachedEntity.Structuralize();
            auto ret = ptr->DoBackup(cache);
            // REM: 处理返回结果 ret
            ptr->AppDone(ret);
            HILOGI("backup app done %{public}d", ret);
        } catch (const BError &e) {
            HILOGE("extension: AsyncTaskBackup error, err code:%{public}d", e.GetCode());
            ptr->AppDone(e.GetCode());
        } catch (const exception &e) {
            HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        }
        // 清空备份目录
        ptr->DoClear();
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

void BackupExtExtension::RestoreBigFilesForSpecialCloneCloud(const ExtManageInfo &item)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (isDebug_) {
        endFileInfos_[item.hashName] = item.sta.st_size;
    }
    const struct stat &sta = item.sta;
    string fileName = item.hashName;
    if (BDir::CheckFilePathInvalid(fileName)) {
        HILOGE("Check big spec file path : %{public}s err, path is forbidden", GetAnonyPath(fileName).c_str());
        errFileInfos_[fileName].push_back(DEFAULT_INVAL_VALUE);
        return;
    }
    if (chmod(fileName.c_str(), sta.st_mode) != 0) {
        HILOGE("Failed to chmod filePath, err = %{public}d", errno);
        errFileInfos_[fileName].push_back(errno);
    }

    struct timespec tv[2] = {sta.st_atim, sta.st_mtim};
    UniqueFd fd(open(fileName.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open file = %{public}s, err = %{public}d", GetAnonyPath(fileName).c_str(), errno);
        errFileInfos_[fileName].push_back(errno);
        return;
    }
    if (futimens(fd.Get(), tv) != 0) {
        errFileInfos_[fileName].push_back(errno);
        HILOGE("Failed to change the file time. %{public}s , %{public}d", GetAnonyPath(fileName).c_str(), errno);
    }
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
    if (BDir::CheckFilePathInvalid(tarName)) {
        HILOGE("Check spec tarfile hash path : %{public}s err, path is forbidden", GetAnonyPath(tarName).c_str());
        return ERR_INVALID_VALUE;
    }
    if (BDir::CheckFilePathInvalid(untarPath)) {
        HILOGE("Check spec tarfile path : %{public}s err, path is forbidden", GetAnonyPath(untarPath).c_str());
        return ERR_INVALID_VALUE;
    }
    auto [err, fileInfos, errInfos] = UntarFile::GetInstance().UnPacket(tarName, untarPath);
    if (isDebug_) {
        if (err != 0) {
            endFileInfos_[tarName] = item.sta.st_size;
            errFileInfos_[tarName] = { err };
        }
        endFileInfos_.merge(fileInfos);
        errFileInfos_.merge(errInfos);
    }
    if (err != ERR_OK) {
        HILOGE("Failed to untar file = %{public}s, err = %{public}d", tarName.c_str(), err);
        return err;
    }
    if (!RemoveFile(tarName)) {
        HILOGE("Failed to delete the backup tar %{public}s", tarName.c_str());
    }
    return ERR_OK;
}

ErrCode BackupExtExtension::RestoreFilesForSpecialCloneCloud()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    // 获取索引文件内容
    UniqueFd fd(open(INDEX_FILE_RESTORE.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open index json file = %{private}s, err = %{public}d", INDEX_FILE_RESTORE.c_str(), errno);
        return BError::GetCodeByErrno(errno);
    }
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManageInfo();
    HILOGI("Start do restore for SpecialCloneCloud.");
    auto startTime = std::chrono::system_clock::now();
    for (auto &item : info) {
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
            // 待解压tar文件处理
            radarRestoreInfo_.tarFileNum++;
            radarRestoreInfo_.tarFileSize += static_cast<uint64_t>(item.sta.st_size);
            int ret = RestoreTarForSpecialCloneCloud(item);
            if (isDebug_ && ret != ERR_OK) {
                errFileInfos_[item.hashName].push_back(ret);
                endFileInfos_[item.hashName] = item.sta.st_size;
            }
            if (ret != ERR_OK) {
                HILOGE("Failed to restore tar file %{public}s", item.hashName.c_str());
                return ERR_INVALID_VALUE;
            }
        }
    }
    if (!RemoveFile(INDEX_FILE_RESTORE)) {
        HILOGE("Failed to delete the backup index %{public}s", INDEX_FILE_RESTORE.c_str());
    }
    auto endTime = std::chrono::system_clock::now();
    radarRestoreInfo_.totalFileSpendTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    RecordDoRestoreRes(bundleName_, "BackupExtExtension::RestoreFilesForSpecialCloneCloud", radarRestoreInfo_);
    HILOGI("End do restore for SpecialCloneCloud.");
    return ERR_OK;
}

static bool RestoreBigFilePrecheck(string &fileName, const string &path, const string &hashName, const string &filePath)
{
    if (filePath.empty()) {
        HILOGE("file path is empty. %{public}s", filePath.c_str());
        return false;
    }

    // 不带路径的文件名
    if (access(fileName.data(), F_OK) != 0) {
        HILOGE("file does not exist");
        return false;
    }

    // 目录不存在且只有大文件时，不能通过untar创建，需要检查并创建
    if (!CheckAndCreateDirectory(filePath)) {
        HILOGE("failed to create directory %{public}s", filePath.c_str());
        return false;
    }
    return true;
}

void BackupExtExtension::RestoreBigFileAfter(const string &filePath, const struct stat &sta)
{
    if (chmod(filePath.c_str(), sta.st_mode) != 0) {
        errFileInfos_[filePath].push_back(errno);
        HILOGE("Failed to chmod filePath, err = %{public}d", errno);
    }
    struct timespec tv[2] = {sta.st_atim, sta.st_mtim};
    UniqueFd fd(open(filePath.data(), O_RDONLY));
    if (fd < 0) {
        errFileInfos_[filePath].push_back(errno);
        HILOGE("Failed to open file = %{public}s, err = %{public}d", GetAnonyPath(filePath).c_str(), errno);
        return;
    }
    if (futimens(fd.Get(), tv) != 0) {
        errFileInfos_[filePath].push_back(errno);
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
        errFileInfos_[item.hashName].push_back(errno);
        throw BError(BError::Codes::EXT_INVAL_ARG, string("open report file failed"));
    }
    BReportEntity rp(move(fd));
    rp.CheckAndUpdateIfReportLineEncoded(itemFileName);

    string fileName = path + itemHashName;
    string filePath = appendTargetPath ? (path + itemFileName) : itemFileName;
    if (BDir::CheckFilePathInvalid(filePath)) {
        HILOGE("Check big file path : %{public}s err, path is forbidden", GetAnonyPath(filePath).c_str());
        return;
    }
    if (isDebug_) {
        endFileInfos_[filePath] = item.sta.st_size;
    }

    if (!RestoreBigFilePrecheck(fileName, path, item.hashName, filePath)) {
        return;
    }
    if (!BFile::MoveFile(fileName, filePath)) {
        errFileInfos_[filePath].push_back(errno);
        HILOGE("failed to move the file. err = %{public}d", errno);
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
    for (auto &item : info) {
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
            for (auto item : fileSet) { // 处理要解压的tar文件
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
            ptr->StartOnProcessTaskThread(obj, BackupRestoreScenario::FULL_RESTORE);
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
            ptr->StartOnProcessTaskThread(obj, BackupRestoreScenario::INCREMENTAL_RESTORE);
            auto callBackup = ptr->IncreOnRestoreCallback(obj);
            auto callBackupEx = ptr->IncreOnRestoreExCallback(obj);
            ptr->UpdateOnStartTime();
            ErrCode err = ptr->extension_->OnRestore(callBackup, callBackupEx);
            if (err != ERR_OK) {
                HILOGE("OnRestore done, err = %{pubilc}d", err);
                ptr->AppIncrementalDone(BError::GetCodeByErrno(err));
                ptr->DoClear();
            }
        } catch (const BError &e) {
            ptr->AppIncrementalDone(e.GetCode());
            ptr->DoClear();
        } catch (const exception &e) {
            HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
            ptr->AppIncrementalDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
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
        string backupCache = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
        string restoreCache = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
        string specialRestoreCache = GetRestoreTempPath(bundleName_);

        if (!ForceRemoveDirectory(backupCache)) {
            HILOGI("Failed to delete the backup cache %{public}s", backupCache.c_str());
        }

        if (!ForceRemoveDirectory(restoreCache)) {
            HILOGI("Failed to delete the restore cache %{public}s", restoreCache.c_str());
        }

        if (!ForceRemoveDirectory(specialRestoreCache)) {
            HILOGI("Failed to delete cache for filemanager or medialibrary %{public}s", specialRestoreCache.c_str());
        }
        // delete el1 backup/restore
        ForceRemoveDirectory(
            string(BConstants::PATH_BUNDLE_BACKUP_HOME_EL1).append(BConstants::SA_BUNDLE_BACKUP_BACKUP));
        ForceRemoveDirectory(
            string(BConstants::PATH_BUNDLE_BACKUP_HOME_EL1).append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
        // delete special directory
        if (bundleName_.compare(MEDIA_LIBRARY_BUNDLE_NAME) == 0) {
            ForceRemoveDirectory(
                string(BConstants::PATH_MEDIALDATA_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP));
            ForceRemoveDirectory(
                string(BConstants::PATH_MEDIALDATA_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
        }
        if (bundleName_.compare(FILE_MANAGER_BUNDLE_NAME) == 0) {
            ForceRemoveDirectory(
                string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP));
            ForceRemoveDirectory(
                string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
        }
        unique_lock<shared_mutex> lock(lock_);
    } catch (...) {
        HILOGE("Failed to clear");
    }
}

void BackupExtExtension::AppDone(ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("AppDone Begin.");
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to obtain the ServiceProxy handle");
        DoClear();
        return;
    }
    auto ret = proxy->AppDone(errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed to notify the app done. err = %{public}d", ret);
    }
}

void BackupExtExtension::AppResultReport(const std::string restoreRetInfo,
    BackupRestoreScenario scenario, ErrCode errCode)
{
    auto proxy = ServiceProxy::GetInstance();
    BExcepUltils::BAssert(proxy, BError::Codes::EXT_BROKEN_IPC, "Failed to obtain the ServiceProxy handle");
    HILOGI("restoreRetInfo is %{public}s", restoreRetInfo.c_str());
    auto ret = proxy->ServiceResultReport(restoreRetInfo, scenario, errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed notify app restoreResultReport, errCode: %{public}d", ret);
    }
}

void BackupExtExtension::StartExtTimer(bool &isExtStart)
{
    auto proxy = ServiceProxy::GetInstance();
    BExcepUltils::BAssert(proxy, BError::Codes::EXT_BROKEN_IPC, "Failed to obtain the ServiceProxy handle");
    HILOGI("Start ext timer by ipc.");
    auto ret = proxy->StartExtTimer(isExtStart);
    if (ret != ERR_OK) {
        HILOGE("Start ext timer failed, errCode: %{public}d", ret);
    }
}

void BackupExtExtension::StartFwkTimer(bool &isFwkStart)
{
    auto proxy = ServiceProxy::GetInstance();
    BExcepUltils::BAssert(proxy, BError::Codes::EXT_BROKEN_IPC, "Failed to obtain the ServiceProxy handle");
    HILOGI("Start fwk timer by ipc.");
    auto ret = proxy->StartFwkTimer(isFwkStart);
    if (ret != ERR_OK) {
        HILOGE("Start fwk timer failed, errCode: %{public}d", ret);
    }
}

void BackupExtExtension::UpdateOnStartTime()
{
    std::lock_guard<std::mutex> lock(onStartTimeLock_);
    g_onStart = std::chrono::system_clock::now();
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
            ptr->StartOnProcessTaskThread(obj, BackupRestoreScenario::FULL_BACKUP);
            auto callBackup = ptr->OnBackupCallback(obj);
            auto callBackupEx = ptr->OnBackupExCallback(obj);
            ptr->UpdateOnStartTime();
            ErrCode err = ptr->extension_->OnBackup(callBackup, callBackupEx);
            if (err != ERR_OK) {
                HILOGE("OnBackup done, err = %{pubilc}d", err);
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

static bool CheckTar(const string &fileName)
{
    return ExtractFileExt(fileName) == "tar";
}

static bool IfEquality(const ReportFileInfo &info, const ReportFileInfo &infoAd)
{
    return info.filePath < infoAd.filePath;
}

static void AdDeduplication(vector<struct ReportFileInfo> &FilesList)
{
    sort(FilesList.begin(), FilesList.end(), IfEquality);
    auto it = unique(FilesList.begin(), FilesList.end(), [](const ReportFileInfo &info, const ReportFileInfo &infoAd) {
        return info.filePath == infoAd.filePath;
        });
    FilesList.erase(it, FilesList.end());
}

void BackupExtExtension::CompareFiles(UniqueFd incrementalFd,
                                      UniqueFd manifestFd,
                                      vector<struct ReportFileInfo> &allFiles,
                                      vector<struct ReportFileInfo> &smallFiles,
                                      vector<struct ReportFileInfo> &bigFiles)
{
    HILOGI("Begin Compare");
    struct ReportFileInfo storageFiles;
    BReportEntity cloudRp(move(manifestFd));
    unordered_map<string, struct ReportFileInfo> cloudFiles = cloudRp.GetReportInfos();
    BReportEntity storageRp(move(incrementalFd));

    while (storageRp.GetStorageReportInfos(storageFiles)) {
        // 进行文件对比
        const string &path = storageFiles.filePath;
        if (path.empty()) {
            HILOGD("GetStorageReportInfos failed");
            continue;
        }
        auto it = cloudFiles.find(path);
        bool isExist = (it != cloudFiles.end()) ? true : false;
        if (storageFiles.isIncremental == true && storageFiles.isDir == true && !isExist) {
            smallFiles.push_back(storageFiles);
        }
        bool isChange = (isExist && storageFiles.size == it->second.size &&
            storageFiles.mtime == it->second.mtime) ? false : true;
        if (storageFiles.isDir == false && isChange) {
            auto [res, fileHash] = BackupFileHash::HashWithSHA256(path);
            if (fileHash.empty()) {
                continue;
            }
            storageFiles.hash = fileHash;
        } else if (storageFiles.isDir == false) {
            storageFiles.hash = it->second.hash;
        }

        if (storageFiles.isDir == false && CheckTar(path)) {
            storageFiles.userTar = 1;
        }

        allFiles.push_back(storageFiles);
        if (storageFiles.isDir == false && storageFiles.isIncremental == true && (!isExist ||
             cloudFiles.find(path)->second.hash != storageFiles.hash)) {
            // 在云空间简报里不存在或者hash不一致
            if (storageFiles.size <= BConstants::BIG_FILE_BOUNDARY) {
                smallFiles.push_back(storageFiles);
                continue;
            }
            bigFiles.push_back(storageFiles);
        }
    }
    AdDeduplication(allFiles);
    AdDeduplication(smallFiles);
    AdDeduplication(bigFiles);
    HILOGI("End Compare, allfile is %{public}zu, samllfile is %{public}zu, bigfile is %{public}zu",
        allFiles.size(), smallFiles.size(), bigFiles.size());
}

ErrCode BackupExtExtension::HandleIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Start HandleIncrementalBackup");
        if (!IfAllowToBackupRestore()) {
            return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE, "Application does not allow backup or restore")
                .GetCode();
        }
        AsyncTaskDoIncrementalBackup(move(incrementalFd), move(manifestFd));
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to handle incremental backup");
        return BError(BError::Codes::EXT_INVAL_ARG).GetCode();
    }
}

ErrCode BackupExtExtension::IncrementalOnBackup(bool isClearData)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    SetClearDataFlag(isClearData);
    if (!IfAllowToBackupRestore()) {
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE, "Application does not allow backup or restore")
            .GetCode();
    }
    AsyncTaskOnIncrementalBackup();
    return ERR_OK;
}

tuple<UniqueFd, UniqueFd> BackupExtExtension::GetIncrementalBackupFileHandle()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    return {UniqueFd(-1), UniqueFd(-1)};
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
    for (auto item : srcFiles) {
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
 * 获取增量的大文件的信息
 */
static TarMap GetIncrmentBigInfos(const vector<struct ReportFileInfo> &files)
{
    auto getStringHash = [](const TarMap &tarMap, const string &str) -> string {
        ostringstream strHex;
        strHex << hex;

        hash<string> strHash;
        size_t szHash = strHash(str);
        strHex << setfill('0') << setw(BConstants::BIG_FILE_NAME_SIZE) << szHash;
        string name = strHex.str();
        for (int i = 0; tarMap.find(name) != tarMap.end(); ++i, strHex.str("")) {
            szHash = strHash(str + to_string(i));
            strHex << setfill('0') << setw(BConstants::BIG_FILE_NAME_SIZE) << szHash;
            name = strHex.str();
        }

        return name;
    };

    TarMap bigFiles;
    for (const auto &item : files) {
        struct stat sta = {};
        if (stat(item.filePath.c_str(), &sta) != 0) {
            HILOGE("Failed to stat file %{public}s, err = %{public}d", item.filePath.c_str(), errno);
            throw BError(BError::Codes::EXT_INVAL_ARG, "Get file stat failed");
        }
        string md5Name = getStringHash(bigFiles, item.filePath);
        if (!md5Name.empty()) {
            bigFiles.emplace(md5Name, make_tuple(item.filePath, sta, true));
        }
    }

    return bigFiles;
}

/**
 * 增量tar包和简报信息回传
 */
static ErrCode IncrementalTarFileReady(const TarMap &bigFileInfo,
                                       const vector<struct ReportFileInfo> &srcFiles,
                                       sptr<IService> proxy)
{
    string tarFile = bigFileInfo.begin()->first;
    string manageFile = GetReportFileName(tarFile);
    string file = string(INDEX_FILE_INCREMENTAL_BACKUP).append(manageFile);
    WriteFile(file, srcFiles);

    string tarName = string(INDEX_FILE_INCREMENTAL_BACKUP).append(tarFile);
    ErrCode ret = proxy->AppIncrementalFileReady(tarFile, UniqueFd(open(tarName.data(), O_RDONLY)),
                                                 UniqueFd(open(file.data(), O_RDONLY)), ERR_OK);
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
ErrCode BackupExtExtension::IncrementalBigFileReady(const TarMap &pkgInfo,
    const vector<struct ReportFileInfo> &bigInfos, sptr<IService> proxy)
{
    ErrCode ret {ERR_OK};
    HILOGI("IncrementalBigFileReady Begin, pkgInfo size:%{public}zu", pkgInfo.size());
    auto startTime = std::chrono::system_clock::now();
    int fdNum = 0;
    for (auto &item : pkgInfo) {
        if (item.first.empty()) {
            continue;
        }
        auto [path, sta, isBeforeTar] = item.second;
        int32_t errCode = ERR_OK;
        WaitToSendFd(startTime, fdNum);
        UniqueFd fd(open(path.data(), O_RDONLY));
        if (fd < 0) {
            HILOGE("IncrementalBigFileReady open file failed, file name is %{public}s, err = %{public}d", path.c_str(),
                   errno);
            errCode = errno;
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
        ret = proxy->AppIncrementalFileReady(item.first, std::move(fd), UniqueFd(open(file.data(), O_RDONLY)), errCode);
        if (SUCCEEDED(ret)) {
            HILOGI("IncrementalBigFileReady: The application is packaged successfully, package name is %{public}s",
                   item.first.c_str());
            RemoveFile(file);
        } else {
            HILOGE("IncrementalBigFileReady interface fails to be invoked: %{public}d", ret);
        }
        fdNum += BConstants::FILE_AND_MANIFEST_FD_COUNT;
        RefreshTimeInfo(startTime, fdNum);
    }
    HILOGI("IncrementalBigFileReady End");
    return ret;
}

int BackupExtExtension::DoIncrementalBackupTask(UniqueFd incrementalFd, UniqueFd manifestFd)
{
    auto start = std::chrono::system_clock::now();
    vector<struct ReportFileInfo> allFiles;
    vector<struct ReportFileInfo> smallFiles;
    vector<struct ReportFileInfo> bigFiles;
    CompareFiles(move(incrementalFd), move(manifestFd), allFiles, smallFiles, bigFiles);
    auto ret = DoIncrementalBackup(allFiles, smallFiles, bigFiles);
    if (ret == ERR_OK) {
        auto end = std::chrono::system_clock::now();
        auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (cost >= BConstants::MAX_TIME_COST) {
            std::stringstream ss;
            ss << R"("spendTime": )"<< cost << R"(ms, "totalFiles": )" << allFiles.size() << R"(, "smallFiles": )"
                << smallFiles.size() << R"(, "bigFiles": )" << bigFiles.size();
            AppRadar::Info info(bundleName_, "", ss.str());
            AppRadar::GetInstance().RecordBackupFuncRes(info, "BackupExtExtension::DoIncrementalBackupTask",
                AppRadar::GetInstance().GetUserId(), BizStageBackup::BIZ_STAGE_DO_BACKUP, static_cast<int32_t>(ret));
        }
    }
    return ret;
}

void BackupExtExtension::AsyncTaskDoIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd)
{
    HILOGI("Do IncrementalBackup, start fwk timer begin.");
    bool isFwkStart;
    StartFwkTimer(isFwkStart);
    if (!isFwkStart) {
        HILOGE("Do IncrementalBackup, start fwk timer fail.");
        return;
    }
    HILOGI("Do IncrementalBackup, start fwk timer end.");
    int incrementalFdDup = dup(incrementalFd);
    int manifestFdDup = dup(manifestFd);
    if (incrementalFdDup < 0) {
        throw BError(BError::Codes::EXT_INVAL_ARG, "dup failed");
    }
    auto task = [obj {wptr<BackupExtExtension>(this)}, manifestFdDup, incrementalFdDup]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        try {
            UniqueFd incrementalDupFd(dup(incrementalFdDup));
            UniqueFd manifestDupFd(dup(manifestFdDup));
            if (incrementalDupFd < 0) {
                throw BError(BError::Codes::EXT_INVAL_ARG, "dup failed");
            }
            close(incrementalFdDup);
            close(manifestFdDup);
            auto ret = ptr->DoIncrementalBackupTask(move(incrementalDupFd), move(manifestDupFd));
            ptr->AppIncrementalDone(ret);
            HILOGI("Incremental backup app done %{public}d", ret);
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

    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

void BackupExtExtension::AsyncTaskOnIncrementalBackup()
{
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        try {
            ptr->curScenario_ = BackupRestoreScenario::INCREMENTAL_BACKUP;
            ptr->StartOnProcessTaskThread(obj, BackupRestoreScenario::INCREMENTAL_BACKUP);
            auto callBackup = ptr->IncOnBackupCallback(obj);
            auto callBackupEx = ptr->IncOnBackupExCallback(obj);
            ptr->UpdateOnStartTime();
            ErrCode err = ptr->extension_->OnBackup(callBackup, callBackupEx);
            if (err != ERR_OK) {
                HILOGE("OnBackup done, err = %{pubilc}d", err);
                ptr->AppIncrementalDone(BError::GetCodeByErrno(err));
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

    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

static string GetIncrmentPartName()
{
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(duration);

    return to_string(milliseconds.count()) + "_part";
}

void BackupExtExtension::IncrementalPacket(const vector<struct ReportFileInfo> &infos, TarMap &tar,
    sptr<IService> proxy)
{
    HILOGI("IncrementalPacket begin, infos count: %{public}zu", infos.size());
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
    uint64_t totalSize = 0;
    uint32_t fileCount = 0;
    vector<string> packFiles;
    vector<struct ReportFileInfo> tarInfos;
    TarFile::GetInstance().SetPacketMode(true); // 设置下打包模式
    auto startTime = std::chrono::system_clock::now();
    int fdNum = 0;
    string partName = GetIncrmentPartName();
    auto reportCb = ReportErrFileByProc(wptr<BackupExtExtension> {this}, curScenario_);
    for (auto small : infos) {
        totalSize += static_cast<uint64_t>(small.size);
        fileCount += 1;
        packFiles.emplace_back(small.filePath);
        tarInfos.emplace_back(small);
        if (totalSize >= BConstants::DEFAULT_SLICE_SIZE || fileCount >= BConstants::MAX_FILE_COUNT) {
            TarMap tarMap {};
            TarFile::GetInstance().Packet(packFiles, partName, path, tarMap, reportCb);
            tar.insert(tarMap.begin(), tarMap.end());
            // 执行tar包回传功能
            WaitToSendFd(startTime, fdNum);
            IncrementalTarFileReady(tarMap, tarInfos, proxy);
            totalSize = 0;
            fileCount = 0;
            packFiles.clear();
            tarInfos.clear();
            fdNum += BConstants::FILE_AND_MANIFEST_FD_COUNT;
            RefreshTimeInfo(startTime, fdNum);
        }
    }
    if (fileCount > 0) {
        // 打包回传
        TarMap tarMap {};
        TarFile::GetInstance().Packet(packFiles, partName, path, tarMap, reportCb);
        IncrementalTarFileReady(tarMap, tarInfos, proxy);
        fdNum = 1;
        WaitToSendFd(startTime, fdNum);
        tar.insert(tarMap.begin(), tarMap.end());
        packFiles.clear();
        tarInfos.clear();
        RefreshTimeInfo(startTime, fdNum);
    }
}

static ErrCode IncrementalAllFileReady(const TarMap &pkgInfo,
                                       const vector<struct ReportFileInfo> &srcFiles,
                                       sptr<IService> proxy)
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
    UniqueFd fd(open(INDEX_FILE_BACKUP.data(), O_RDONLY));
    UniqueFd manifestFd(open(file.data(), O_RDONLY));
    ErrCode ret =
        proxy->AppIncrementalFileReady(string(BConstants::EXT_BACKUP_MANAGE), std::move(fd), std::move(manifestFd),
            ERR_OK);
    if (SUCCEEDED(ret)) {
        HILOGI("IncrementalAllFileReady successfully");
        RemoveFile(file);
    } else {
        HILOGI("successfully but the IncrementalAllFileReady interface fails to be invoked: %{public}d", ret);
    }
    return ret;
}

int BackupExtExtension::DoIncrementalBackup(const vector<struct ReportFileInfo> &allFiles,
                                            const vector<struct ReportFileInfo> &smallFiles,
                                            const vector<struct ReportFileInfo> &bigFiles)
{
    HILOGI("Do increment backup begin");
    if (extension_ == nullptr) {
        HILOGE("Failed to do incremental backup, extension is nullptr");
        throw BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr");
    }
    if (extension_->GetExtensionAction() != BConstants::ExtensionAction::BACKUP) {
        return EPERM;
    }
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
    if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
        throw BError(errno);
    }
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
    }
    // 获取增量文件和全量数据
    if (smallFiles.size() == 0 && bigFiles.size() == 0) {
        // 没有增量，则不需要上传
        TarMap tMap;
        ErrCode err = IncrementalAllFileReady(tMap, allFiles, proxy);
        HILOGI("Do increment backup, IncrementalAllFileReady end, file empty");
        return err;
    }
    // tar包数据
    TarMap tarMap;
    IncrementalPacket(smallFiles, tarMap, proxy);
    HILOGI("Do increment backup, IncrementalPacket end");
    // 最后回传大文件
    TarMap bigMap = GetIncrmentBigInfos(bigFiles);
    IncrementalBigFileReady(bigMap, bigFiles, proxy);
    HILOGI("Do increment backup, IncrementalBigFileReady end");
    bigMap.insert(tarMap.begin(), tarMap.end());
    // 回传manage.json和全量文件
    ErrCode err = IncrementalAllFileReady(bigMap, allFiles, proxy);
    HILOGI("End, bigFiles num:%{public}zu, smallFiles num:%{public}zu, allFiles num:%{public}zu", bigFiles.size(),
        smallFiles.size(), allFiles.size());
    return err;
}
} // namespace OHOS::FileManagement::Backup
