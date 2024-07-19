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
#include "b_tarball/b_tarball_factory.h"
#include "filemgmt_libhilog.h"
#include "hitrace_meter.h"
#include "i_service.h"
#include "sandbox_helper.h"
#include "service_proxy.h"
#include "tar_file.h"
#include "untar_file.h"
#include "b_anony/b_anony.h"

namespace OHOS::FileManagement::Backup {
const string DEFAULT_TAR_PKG = "1.tar";
const string INDEX_FILE_BACKUP = string(BConstants::PATH_BUNDLE_BACKUP_HOME).
                                 append(BConstants::SA_BUNDLE_BACKUP_BACKUP).
                                 append(BConstants::EXT_BACKUP_MANAGE);
const string INDEX_FILE_RESTORE = string(BConstants::PATH_BUNDLE_BACKUP_HOME).
                                  append(BConstants::SA_BUNDLE_BACKUP_RESTORE).
                                  append(BConstants::EXT_BACKUP_MANAGE);
const string INDEX_FILE_INCREMENTAL_BACKUP = string(BConstants::PATH_BUNDLE_BACKUP_HOME).
                                             append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
using namespace std;

namespace {
const int64_t DEFAULT_SLICE_SIZE = 100 * 1024 * 1024; // 分片文件大小为100M
const uint32_t MAX_FILE_COUNT = 6000;                 // 单个tar包最多包含6000个文件
const uint32_t MAX_FD_GROUP_USE_TIME = 1000;          // 每组打开最大时间1000ms
const int FILE_AND_MANIFEST_FD_COUNT = 2;          // 每组文件和简报数量统计
}

static std::set<std::string> GetIdxFileData()
{
    UniqueFd idxFd(open(INDEX_FILE_RESTORE.data(), O_RDONLY));
    if (idxFd < 0) {
        HILOGE("Failed to open idxFile = %{private}s, err = %{public}d", INDEX_FILE_RESTORE.c_str(), errno);
        return std::set<std::string>();
    }
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(std::move(idxFd));
    auto cache = cachedEntity.Structuralize();
    return cache.GetExtManage();
}

static std::vector<ExtManageInfo> GetExtManageInfo()
{
    string filePath = BExcepUltils::Canonicalize(INDEX_FILE_RESTORE);
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
        throw BError(BError::Codes::EXT_BROKEN_IPC,
                     string("Calling tokenType is error, token type is ").append(to_string(tokenType)));
    }
    if (IPCSkeleton::GetCallingUid() != BConstants::BACKUP_UID) {
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
        throw BError(BError::Codes::EXT_INVAL_ARG, fileName);
    }
    string path = filePath.substr(0, filePathPrefix);
    if (access(path.c_str(), F_OK) != 0) {
        bool created = ForceCreateDirectory(path.data());
        if (!created) {
            string str = string("Failed to create restore folder.");
            throw BError(BError::Codes::EXT_INVAL_ARG, str);
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

        string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
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
    UniqueFd fd = GetFileHandleForSpecialCloneCloud(fileName);
    if (fd < 0) {
        HILOGE("Failed to open file = %{private}s, err = %{public}d", fileName.c_str(), errno);
        throw BError(BError::Codes::EXT_INVAL_ARG, string("open tar file failed"));
    }

    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
        string str = string("Failed to create restore folder. ").append(std::generic_category().message(errno));
        throw BError(BError::Codes::EXT_INVAL_ARG, str);
    }
    string reportName = path + BConstants::BLANK_REPORT_NAME;
    UniqueFd reportFd(open(reportName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
    if (reportFd < 0) {
        HILOGE("Failed to open report file = %{private}s, err = %{public}d", reportName.c_str(), errno);
        throw BError(BError::Codes::EXT_INVAL_ARG, string("open report file failed"));
    }

    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
    }
    auto ret = proxy->AppIncrementalFileReady(fileName, move(fd), move(reportFd), ERR_OK);
    if (ret != ERR_OK) {
        HILOGE("Failed to AppIncrementalFileReady %{public}d", ret);
    }
    return ERR_OK;
}

static string GetIncrementalFileHandlePath()
{
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
        string str = string("Failed to create restore folder. ").append(std::generic_category().message(errno));
        throw BError(BError::Codes::EXT_INVAL_ARG, str);
    }
    return path;
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
        HILOGI("extension: Start GetIncrementalFileHandle");
        VerifyCaller();

        if (extension_->SpecialVersionForCloneAndCloud()) {
            return GetIncreFileHandleForSpecialVersion(fileName);
        }
        HILOGI("extension: single to single fileName:%{public}s", fileName.c_str());
        string path = GetIncrementalFileHandlePath();
        string tarName = path + fileName;
        if (access(tarName.c_str(), F_OK) == 0) {
            throw BError(BError::Codes::EXT_INVAL_ARG, string("The file already exists"));
        }
        int32_t errCode = ERR_OK;
        UniqueFd fd(open(tarName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
        if (fd < 0) {
            HILOGE("Failed to open tar file = %{private}s, err = %{public}d", tarName.c_str(), errno);
            errCode = errno;
        }
        // 对应的简报文件
        string reportName = GetReportFileName(tarName);
        if (access(reportName.c_str(), F_OK) == 0) {
            throw BError(BError::Codes::EXT_INVAL_ARG, string("The report file already exists"));
        }
        UniqueFd reportFd(open(reportName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
        if (reportFd < 0) {
            HILOGE("Failed to open report file = %{private}s, err = %{public}d", reportName.c_str(), errno);
            errCode = errno;
        }
        HILOGI("extension: Will notify AppIncrementalFileReady");
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            throw BError(BError::Codes::EXT_BROKEN_IPC, string("Failed to AGetInstance"));
        }
        auto ret = proxy->AppIncrementalFileReady(fileName, move(fd), move(reportFd), errCode);
        if (ret != ERR_OK) {
            HILOGE("Failed to AppIncrementalFileReady %{public}d", ret);
        }
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to get incremental file handle");
        DoClear();
        return BError(BError::Codes::EXT_INVAL_ARG).GetCode();
    }
}

ErrCode BackupExtExtension::HandleClear()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
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

ErrCode BackupExtExtension::BigFileReady(sptr<IService> proxy)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    UniqueFd fd(open(INDEX_FILE_BACKUP.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open index json file = %{private}s, err = %{public}d", INDEX_FILE_BACKUP.c_str(), errno);
        return BError::GetCodeByErrno(errno);
    }
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto pkgInfo = cache.GetExtManageInfo();
    HILOGI("BigFileReady Begin: pkgInfo file size is: %{public}zu", pkgInfo.size());
    ErrCode ret {ERR_OK};
    auto startTime = std::chrono::system_clock::now();
    int fdNum = 0;
    for (auto &item : pkgInfo) {
        if (item.hashName.empty() || item.fileName.empty()) {
            continue;
        }
        WaitToSendFd(startTime, fdNum);
        int32_t errCode = ERR_OK;
        UniqueFd fd(open(item.fileName.data(), O_RDONLY));
        if (fd < 0) {
            HILOGE("open file failed, file name is %{public}s, err = %{public}d", item.fileName.c_str(), errno);
            errCode = errno;
        }

        ret = proxy->AppFileReady(item.hashName, std::move(fd), errCode);
        if (SUCCEEDED(ret)) {
            HILOGI("The application is packaged successfully, package name is %{public}s", item.hashName.c_str());
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
            AsyncTaskRestore(GetIdxFileData(), GetExtManageInfo());
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

ErrCode BackupExtExtension::HandleBackup()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (extension_ == nullptr) {
        HILOGE("Failed to handle backup, extension is nullptr");
        return BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr").GetCode();
    }
    string usrConfig = extension_->GetUsrConfig();
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(usrConfig);
    auto cache = cachedEntity.Structuralize();
    if (!cache.GetAllowToBackupRestore()) {
        HILOGE("Application does not allow backup or restore");
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE, "Application does not allow backup or restore")
            .GetCode();
    }
    AsyncTaskOnBackup();
    return 0;
}

static bool IsUserTar(const string &tarFile, const std::vector<ExtManageInfo> &extManageInfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (tarFile.empty()) {
        return false;
    }
    auto iter = find_if(extManageInfo.begin(), extManageInfo.end(),
        [&tarFile](const auto &item) { return item.hashName == tarFile; });
    if (iter != extManageInfo.end()) {
        HILOGI("tarFile:%{public}s isUserTar:%{public}d", tarFile.data(), iter->isUserTar);
        return iter->isUserTar;
    }
    HILOGE("Can not find tarFile %{public}s", tarFile.data());
    return false;
}

static pair<TarMap, vector<string>> GetFileInfos(const vector<string> &includes, const vector<string> &excludes)
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

int BackupExtExtension::DoBackup(const BJsonEntityExtensionConfig &usrConfig)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Start Do backup");
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

    // 大文件处理
    HILOGI("Start packet bigfiles and small files");
    auto [bigFileInfo, smallFiles] = GetFileInfos(includes, excludes);
    for (const auto &item : bigFileInfo) {
        auto filePath = std::get<0>(item.second);
        if (!filePath.empty()) {
            excludes.push_back(filePath);
        }
    }

    HILOGI("Start packet Tar files");
    // 分片打包
    TarMap tarMap {};
    TarFile::GetInstance().Packet(smallFiles, "part", path, tarMap);
    bigFileInfo.insert(tarMap.begin(), tarMap.end());
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
    }
    HILOGI("Will notify IndexFileReady");
    if (auto ret = IndexFileReady(bigFileInfo, proxy); ret) {
        return ret;
    }
    HILOGI("Will notify BigFileReady");
    auto res = BigFileReady(proxy);
    HILOGI("HandleBackup finish, ret = %{public}d", res);
    return res;
}

int BackupExtExtension::DoRestore(const string &fileName)
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
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    string tarName = path + fileName;

    // 当用户指定fullBackupOnly字段或指定版本的恢复，解压目录当前在/backup/restore
    if (extension_->SpecialVersionForCloneAndCloud() || extension_->UseFullBackupOnly()) {
        UntarFile::GetInstance().UnPacket(tarName, path);
    } else {
        UntarFile::GetInstance().UnPacket(tarName, "/");
    }
    HILOGI("Application recovered successfully, package path is %{public}s", tarName.c_str());

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

int BackupExtExtension::DoIncrementalRestore()
{
    HILOGI("Do incremental restore");
    if (extension_ == nullptr) {
        HILOGE("Failed to do incremental restore, extension is nullptr");
        throw BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr");
    }
    auto fileSet = GetIdxFileData();
    auto extManageInfo = GetExtManageInfo();
    for (auto item : fileSet) { // 处理要解压的tar文件
        if (ExtractFileExt(item) == "tar" && !IsUserTar(item, extManageInfo)) {
            if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
                return EPERM;
            }
            // REM: 给定version
            // REM: 解压启动Extension时即挂载好的备份目录中的数据
            string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
            string tarName = path + item;

            // 当用户指定fullBackupOnly字段或指定版本的恢复，解压目录当前在/backup/restore
            if (extension_->SpecialVersionForCloneAndCloud() || extension_->UseFullBackupOnly()) {
                UntarFile::GetInstance().IncrementalUnPacket(tarName, path, GetTarIncludes(tarName));
            } else {
                UntarFile::GetInstance().IncrementalUnPacket(tarName, "/", GetTarIncludes(tarName));
            }
            HILOGI("Application recovered successfully, package path is %{public}s", tarName.c_str());
        }
    }
    return ERR_OK;
}

void BackupExtExtension::AsyncTaskBackup(const string config)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto task = [obj {wptr<BackupExtExtension>(this)}, config]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        try {
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

static void RestoreBigFilesForSpecialCloneCloud(ExtManageInfo item)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    struct stat &sta = item.sta;
    string fileName = item.hashName;
    if (chmod(fileName.c_str(), sta.st_mode) != 0) {
        HILOGE("Failed to chmod filePath, err = %{public}d", errno);
    }

    struct timespec tv[2] = {sta.st_atim, sta.st_mtim};
    UniqueFd fd(open(fileName.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open file = %{public}s, err = %{public}d", GetAnonyPath(fileName).c_str(), errno);
        return;
    }
    if (futimens(fd.Get(), tv) != 0) {
        HILOGE("Failed to change the file time. %{public}s , %{public}d", GetAnonyPath(fileName).c_str(), errno);
    }
}

static ErrCode RestoreTarForSpecialCloneCloud(ExtManageInfo item)
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
    UntarFile::GetInstance().UnPacket(tarName, untarPath);

    if (!RemoveFile(tarName)) {
        HILOGE("Failed to delete the backup tar %{public}s", tarName.c_str());
    }
    return ERR_OK;
}

static ErrCode RestoreFilesForSpecialCloneCloud()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    // 获取索引文件内容
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    UniqueFd fd(open(INDEX_FILE_RESTORE.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open index json file = %{private}s, err = %{public}d", INDEX_FILE_RESTORE.c_str(), errno);
        return BError::GetCodeByErrno(errno);
    }
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManageInfo();
    HILOGI("Start do restore for SpecialCloneCloud.");
    for (auto &item : info) {
        if (item.hashName.empty()) {
            continue;
        }
        if (item.isUserTar || item.isBigFile) {
            // 大文件处理
            RestoreBigFilesForSpecialCloneCloud(item);
        } else {
            // 待解压tar文件处理
            if (RestoreTarForSpecialCloneCloud(item) != ERR_OK) {
                HILOGE("Failed to restore tar file %{public}s", item.hashName.c_str());
                return ERR_INVALID_VALUE;
            }
        }
    }
    if (!RemoveFile(INDEX_FILE_RESTORE)) {
        HILOGE("Failed to delete the backup index %{public}s", INDEX_FILE_RESTORE.c_str());
    }
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

static void RestoreBigFileAfter(const string &fileName,
                                const string &filePath,
                                const struct stat &sta)
{
    if (chmod(filePath.c_str(), sta.st_mode) != 0) {
        HILOGE("Failed to chmod filePath, err = %{public}d", errno);
    }

    if (fileName != filePath) {
        auto resolvedFileName = make_unique<char[]>(PATH_MAX);
        auto resolvedFilePath = make_unique<char[]>(PATH_MAX);
        bool allOk = true;
        if (!realpath(fileName.data(), resolvedFileName.get())) {
            HILOGE("failed to real path for fileName");
            allOk = false;
        }
        if (!realpath(filePath.data(), resolvedFilePath.get())) {
            HILOGE("failed to real path for filePath");
            allOk = false;
        }
        if (allOk && string_view(resolvedFileName.get()) != string_view(resolvedFilePath.get())) {
            if (!RemoveFile(fileName)) {
                HILOGE("Failed to delete the big file");
            }
        }
    }

    struct timespec tv[2] = {sta.st_atim, sta.st_mtim};
    UniqueFd fd(open(filePath.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open file = %{public}s, err = %{public}d", GetAnonyPath(filePath).c_str(), errno);
        return;
    }
    if (futimens(fd.Get(), tv) != 0) {
        HILOGE("failed to change the file time. %{public}s , %{public}d", GetAnonyPath(filePath).c_str(), errno);
    }
}

static void RestoreBigFiles(bool appendTargetPath)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    // 获取索引文件内容
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    UniqueFd fd(open(INDEX_FILE_RESTORE.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open index json file = %{private}s, err = %{public}d", INDEX_FILE_RESTORE.c_str(), errno);
        return;
    }
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManageInfo();
    HILOGI("Start Restore Big Files");
    for (auto &item : info) {
        if (item.hashName.empty() || (!item.isUserTar && !item.isBigFile)) {
            continue;
        }

        string itemHashName = item.hashName;
        string itemFileName = item.fileName;
        // check if item.hasName and fileName need decode by report item attribute
        string reportPath = GetReportFileName(path + item.hashName);
        UniqueFd fd(open(reportPath.data(), O_RDONLY));
        if (fd < 0) {
            HILOGE("Failed to open report file = %{public}s, err = %{public}d", reportPath.c_str(), errno);
            throw BError(BError::Codes::EXT_INVAL_ARG, string("open report file failed"));
        }
        BReportEntity rp(move(fd));
        rp.CheckAndUpdateIfReportLineEncoded(itemFileName);

        string fileName = path + itemHashName;
        string filePath = appendTargetPath ? (path + itemFileName) : itemFileName;

        if (!RestoreBigFilePrecheck(fileName, path, item.hashName, filePath)) {
            continue;
        }

        if (!BFile::CopyFile(fileName, filePath)) {
            HILOGE("failed to copy the file. err = %{public}d", errno);
            continue;
        }

        RestoreBigFileAfter(fileName, filePath, item.sta);
    }
    HILOGI("End Restore Big Files");
}

static void DeleteBackupTars()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    UniqueFd fd(open(INDEX_FILE_RESTORE.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open index json file = %{private}s, err = %{public}d", INDEX_FILE_RESTORE.c_str(), errno);
        return;
    }
    // The directory include tars and manage.json which would be deleted
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManage();
    auto path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    auto extManageInfo = GetExtManageInfo();
    for (auto &item : info) {
        if (ExtractFileExt(item) != "tar" || IsUserTar(item, extManageInfo)) {
            continue;
        }
        string tarPath = path + item;
        if (!RemoveFile(tarPath)) {
            HILOGE("Failed to delete the backup tar %{public}s", tarPath.c_str());
        }
    }
    if (!RemoveFile(INDEX_FILE_RESTORE)) {
        HILOGE("Failed to delete the backup index %{public}s", INDEX_FILE_RESTORE.c_str());
    }
    HILOGI("End execute DeleteBackupTars");
}

static void DeleteBackupIncrementalTars()
{
    UniqueFd fd(open(INDEX_FILE_RESTORE.data(), O_RDONLY));
    if (fd < 0) {
        HILOGE("Failed to open index json file = %{private}s, err = %{public}d", INDEX_FILE_RESTORE.c_str(), errno);
        return;
    }
    // The directory include tars and manage.json which would be deleted
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManage();
    auto path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    auto extManageInfo = GetExtManageInfo();
    for (auto &item : info) {
        if (ExtractFileExt(item) != "tar" || IsUserTar(item, extManageInfo)) {
            continue;
        }
        string tarPath = path + item;
        if (!RemoveFile(tarPath)) {
            HILOGE("Failed to delete the backup tar %{private}s, err = %{public}d", tarPath.c_str(), errno);
        }
        // 删除简报文件
        string reportPath = GetReportFileName(tarPath);
        if (!RemoveFile(reportPath)) {
            HILOGE("Failed to delete the backup report %{private}s, err = %{public}d", reportPath.c_str(), errno);
        }
    }
    if (!RemoveFile(INDEX_FILE_RESTORE)) {
        HILOGE("Failed to delete the backup index %{public}s", INDEX_FILE_RESTORE.c_str());
    }
    string reportManagePath = GetReportFileName(INDEX_FILE_RESTORE); // GetIncrementalFileHandle创建的空fd
    if (!RemoveFile(reportManagePath)) {
        HILOGE("Failed to delete the backup report index %{public}s", reportManagePath.c_str());
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
                ret = RestoreFilesForSpecialCloneCloud();
                if (ret == ERR_OK) {
                    ptr->AsyncTaskRestoreForUpgrade();
                } else {
                    ptr->AppDone(ret);
                    ptr->DoClear();
                }
                return;
            }
            // 解压
            for (auto item : fileSet) { // 处理要解压的tar文件
                if (ExtractFileExt(item) == "tar" && !IsUserTar(item, extManageInfo)) {
                    ret = ptr->DoRestore(item);
                }
            }
            // 恢复用户tar包以及大文件
            // 目的地址是否需要拼接path(临时目录)，FullBackupOnly为true并且非特殊场景
            bool appendTargetPath =
                ptr->extension_->UseFullBackupOnly() && !ptr->extension_->SpecialVersionForCloneAndCloud();
            RestoreBigFiles(appendTargetPath);
            DeleteBackupTars();
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

void BackupExtExtension::AsyncTaskIncrementalRestore()
{
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        try {
            // 解压
            int ret = ptr->DoIncrementalRestore();
            // 恢复用户tar包以及大文件
            // 目的地址是否需要拼接path(临时目录)，FullBackupOnly为true并且非特殊场景
            bool appendTargetPath =
                ptr->extension_->UseFullBackupOnly() && !ptr->extension_->SpecialVersionForCloneAndCloud();
            RestoreBigFiles(appendTargetPath);

            // delete 1.tar/manage.json
            DeleteBackupIncrementalTars();

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
            int ret = RestoreFilesForSpecialCloneCloud();
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
        auto callBackup = [obj](ErrCode errCode, std::string errMsg) {
            auto extensionPtr = obj.promote();
            if (extensionPtr == nullptr) {
                HILOGE("Ext extension handle have been released");
                return;
            }
            HILOGI("Current bundle will execute app done");
            if (errMsg.empty()) {
                extensionPtr->AppDone(errCode);
            } else {
                std::string errInfo;
                BJsonUtil::BuildRestoreErrInfo(errInfo, errCode, errMsg);
                extensionPtr->AppResultReport(errInfo, BackupRestoreScenario::FULL_RESTORE, errCode);
            }
            extensionPtr->DoClear();
        };
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        try {
            auto callBackupEx = ptr->RestoreResultCallbackEx(obj);
            ErrCode err = ptr->extension_->OnRestore(callBackup, callBackupEx);
            HILOGI("OnRestore done err = %{public}d", err);
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
        auto callBackup = [obj](ErrCode errCode, std::string errMsg) {
            auto extensionPtr = obj.promote();
            if (extensionPtr == nullptr) {
                HILOGE("Ext extension handle have been released");
                return;
            }
            HILOGI("Current bundle will execute app done");
            if (errMsg.empty()) {
                extensionPtr->AppIncrementalDone(errCode);
            } else {
                std::string errInfo;
                BJsonUtil::BuildRestoreErrInfo(errInfo, errCode, errMsg);
                extensionPtr->AppResultReport(errInfo, BackupRestoreScenario::INCREMENTAL_RESTORE, errCode);
            }
            extensionPtr->DoClear();
        };
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        try {
            auto callBackupEx = ptr->IncRestoreResultCallbackEx(obj);
            ErrCode err = ptr->extension_->OnRestore(callBackup, callBackupEx);
            HILOGI("OnRestore done err = %{public}d", err);
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
        string backupCache = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
        string restoreCache = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);

        if (!ForceRemoveDirectory(backupCache)) {
            HILOGI("Failed to delete the backup cache %{public}s", backupCache.c_str());
        }

        if (!ForceRemoveDirectory(restoreCache)) {
            HILOGI("Failed to delete the restore cache %{public}s", restoreCache.c_str());
        }
        // delete el1 backup/restore
        ForceRemoveDirectory(
            string(BConstants::PATH_BUNDLE_BACKUP_HOME_EL1).append(BConstants::SA_BUNDLE_BACKUP_BACKUP));
        ForceRemoveDirectory(
            string(BConstants::PATH_BUNDLE_BACKUP_HOME_EL1).append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
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
    BExcepUltils::BAssert(proxy, BError::Codes::EXT_BROKEN_IPC, "Failed to obtain the ServiceProxy handle");
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

void BackupExtExtension::AsyncTaskOnBackup()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
            auto callBackup = [obj](ErrCode errCode, std::string errMsg) {
            HILOGI("begin call backup");
            auto extensionPtr = obj.promote();
            if (extensionPtr == nullptr) {
                HILOGE("Ext extension handle have been released");
                return;
            }
            if (extensionPtr->extension_ == nullptr) {
                HILOGE("Extension handle have been released");
                return;
            }
            extensionPtr->AsyncTaskBackup(extensionPtr->extension_->GetUsrConfig());
        };
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        try {
            auto callBackupEx = ptr->HandleTaskBackupEx(obj);
            ptr->extension_->OnBackup(callBackup, callBackupEx);
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

ErrCode BackupExtExtension::HandleRestore()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    VerifyCaller();
    if (extension_ == nullptr) {
        HILOGE("Failed to handle restore, extension is nullptr");
        throw BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr");
    }
    if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
        HILOGE("Failed to get file handle, because action is %{public}d invalid", extension_->GetExtensionAction());
        throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
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

    return 0;
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
    HILOGI("Start HandleIncrementalBackup");
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (extension_ == nullptr) {
        HILOGE("Failed to handle incremental backup, extension is nullptr");
        return BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr").GetCode();
    }
    string usrConfig = extension_->GetUsrConfig();
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(usrConfig);
    auto cache = cachedEntity.Structuralize();
    if (!cache.GetAllowToBackupRestore()) {
        HILOGE("Application does not allow backup or restore");
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE, "Application does not allow backup or restore")
            .GetCode();
    }
    AsyncTaskDoIncrementalBackup(move(incrementalFd), move(manifestFd));
    return 0;
}

ErrCode BackupExtExtension::IncrementalOnBackup()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (extension_ == nullptr) {
        HILOGE("Failed to handle incremental onBackup, extension is nullptr");
        return BError(BError::Codes::EXT_INVAL_ARG, "Extension is nullptr").GetCode();
    }
    string usrConfig = extension_->GetUsrConfig();
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(usrConfig);
    auto cache = cachedEntity.Structuralize();
    if (!cache.GetAllowToBackupRestore()) {
        HILOGE("Application does not allow backup or restore");
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE, "Application does not allow backup or restore")
            .GetCode();
    }
    AsyncTaskOnIncrementalBackup();
    return 0;
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
        fdNum += FILE_AND_MANIFEST_FD_COUNT;
        RefreshTimeInfo(startTime, fdNum);
    }
    HILOGI("IncrementalBigFileReady End");
    return ret;
}

void BackupExtExtension::AsyncTaskDoIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd)
{
    HILOGI("Start AsyncTaskDoIncrementalBackup");
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
            vector<struct ReportFileInfo> allFiles;
            vector<struct ReportFileInfo> smallFiles;
            vector<struct ReportFileInfo> bigFiles;
            ptr->CompareFiles(move(incrementalDupFd), move(manifestDupFd), allFiles, smallFiles, bigFiles);
            auto ret = ptr->DoIncrementalBackup(allFiles, smallFiles, bigFiles);
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
        auto callBackup = [obj](ErrCode errCode, std::string errMsg) {
            HILOGI("App onbackup end");
            auto proxy = ServiceProxy::GetInstance();
            if (proxy == nullptr) {
                throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
            }
            HILOGI("Start GetAppLocalListAndDoIncrementalBackup");
            proxy->GetAppLocalListAndDoIncrementalBackup();
        };
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have been released");
        BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG, "Extension handle have been released");
        try {
            auto callBackupEx = ptr->HandleBackupEx(obj);
            ptr->extension_->OnBackup(callBackup, callBackupEx);
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
    int64_t totalSize = 0;
    uint32_t fileCount = 0;
    vector<string> packFiles;
    vector<struct ReportFileInfo> tarInfos;
    TarFile::GetInstance().SetPacketMode(true); // 设置下打包模式
    auto startTime = std::chrono::system_clock::now();
    int fdNum = 0;
    string partName = GetIncrmentPartName();
    for (auto small : infos) {
        totalSize += small.size;
        fileCount += 1;
        packFiles.emplace_back(small.filePath);
        tarInfos.emplace_back(small);
        if (totalSize >= DEFAULT_SLICE_SIZE || fileCount >= MAX_FILE_COUNT) {
            TarMap tarMap {};
            TarFile::GetInstance().Packet(packFiles, partName, path, tarMap);
            tar.insert(tarMap.begin(), tarMap.end());
            // 执行tar包回传功能
            WaitToSendFd(startTime, fdNum);
            IncrementalTarFileReady(tarMap, tarInfos, proxy);
            totalSize = 0;
            fileCount = 0;
            packFiles.clear();
            tarInfos.clear();
            fdNum += FILE_AND_MANIFEST_FD_COUNT;
            RefreshTimeInfo(startTime, fdNum);
        }
    }
    if (fileCount > 0) {
        // 打包回传
        TarMap tarMap {};
        TarFile::GetInstance().Packet(packFiles, partName, path, tarMap);
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
        IncrementalAllFileReady(tMap, allFiles, proxy);
        HILOGI("Do increment backup, IncrementalAllFileReady end, file empty");
        return ERR_OK;
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
    IncrementalAllFileReady(bigMap, allFiles, proxy);
    HILOGI("End, bigFiles num:%{public}zu, smallFiles num:%{public}zu, allFiles num:%{public}zu", bigFiles.size(),
        smallFiles.size(), allFiles.size());
    return ERR_OK;
}

void BackupExtExtension::AppIncrementalDone(ErrCode errCode)
{
    HILOGI("Begin");
    auto proxy = ServiceProxy::GetInstance();
    BExcepUltils::BAssert(proxy, BError::Codes::EXT_BROKEN_IPC, "Failed to obtain the ServiceProxy handle");
    auto ret = proxy->AppIncrementalDone(errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed to notify the app done. err = %{public}d", ret);
    }
}

ErrCode BackupExtExtension::GetBackupInfo(std::string &result)
{
    auto obj = wptr<BackupExtExtension>(this);
    auto ptr = obj.promote();
    if (ptr == nullptr) {
        HILOGE("Failed to get ext extension.");
        return BError(BError::Codes::EXT_INVAL_ARG, "extension getBackupInfo exception").GetCode();
    }
    if (ptr->extension_ == nullptr) {
        HILOGE("Failed to get extension.");
        return BError(BError::Codes::EXT_INVAL_ARG, "extension getBackupInfo exception").GetCode();
    }
    auto callBackup = [ptr](ErrCode errCode, const std::string result) {
        if (ptr == nullptr) {
            HILOGE("Failed to get ext extension.");
            return;
        }
        HILOGI("GetBackupInfo callBackup start. result = %{public}s", result.c_str());
        ptr->backupInfo_ = result;
    };
    auto ret = ptr->extension_->GetBackupInfo(callBackup);
    if (ret != ERR_OK) {
        HILOGE("Failed to get backupInfo. err = %{public}d", ret);
        return BError(BError::Codes::EXT_INVAL_ARG, "extension getBackupInfo exception").GetCode();
    }
    HILOGD("backupInfo = %s", backupInfo_.c_str());
    result = backupInfo_;
    backupInfo_.clear();

    return ERR_OK;
}

ErrCode BackupExtExtension::UpdateFdSendRate(std::string &bundleName, int32_t sendRate)
{
    std::lock_guard<std::mutex> lock(updateSendRateLock_);
    HILOGI("Update SendRate, bundleName:%{public}s, sendRate:%{public}d", bundleName.c_str(), sendRate);
    VerifyCaller();
    bundleName_ = bundleName;
    sendRate_ = sendRate;
    if (sendRate > 0) {
        startSendFdRateCon_.notify_one();
    }
    return ERR_OK;
}

std::function<void(ErrCode, std::string)> BackupExtExtension::RestoreResultCallbackEx(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get callbackEx");
    return [obj](ErrCode errCode, const std::string restoreRetInfo) {
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        extensionPtr->extension_->CallExtRestore(errCode, restoreRetInfo);
        if (errCode == ERR_OK && restoreRetInfo.size()) {
            HILOGI("Will notify restore result report");
            extensionPtr->AppResultReport(restoreRetInfo, BackupRestoreScenario::FULL_RESTORE);
            return;
        }
        if (restoreRetInfo.empty()) {
            extensionPtr->AppDone(errCode);
            extensionPtr->DoClear();
        } else {
            std::string errInfo;
            BJsonUtil::BuildRestoreErrInfo(errInfo, errCode, restoreRetInfo);
            extensionPtr->AppResultReport(errInfo, BackupRestoreScenario::FULL_RESTORE, errCode);
            extensionPtr->DoClear();
        }
    };
}

std::function<void(ErrCode, std::string)> BackupExtExtension::AppDoneCallbackEx(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get callback for appDone");
    return [obj](ErrCode errCode, std::string errMsg) {
        HILOGI("begin call callBackupExAppDone");
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        extensionPtr->AppDone(errCode);
        extensionPtr->DoClear();
    };
}

std::function<void(ErrCode, std::string)> BackupExtExtension::IncRestoreResultCallbackEx(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get callback for onRestore");
    return [obj](ErrCode errCode, const std::string restoreRetInfo) {
        HILOGI("begin call restoreEx");
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        extensionPtr->extension_->CallExtRestore(errCode, restoreRetInfo);
        if (errCode == ERR_OK && restoreRetInfo.size()) {
            extensionPtr->AppResultReport(restoreRetInfo, BackupRestoreScenario::INCREMENTAL_RESTORE);
            return;
        }
        if (restoreRetInfo.empty()) {
            extensionPtr->AppIncrementalDone(errCode);
            extensionPtr->DoClear();
        } else {
            std::string errInfo;
            BJsonUtil::BuildRestoreErrInfo(errInfo, errCode, restoreRetInfo);
            extensionPtr->AppResultReport(errInfo, BackupRestoreScenario::INCREMENTAL_RESTORE, errCode);
            extensionPtr->DoClear();
        }
    };
}

std::function<void(ErrCode, const std::string)> BackupExtExtension::HandleBackupEx(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get HandleBackupEx");
    return [obj](ErrCode errCode, const std::string backupExRetInfo) {
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
        }
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        extensionPtr->extension_->CallExtRestore(errCode, backupExRetInfo);
        if (backupExRetInfo.size()) {
            HILOGI("Start GetAppLocalListAndDoIncrementalBackup");
            proxy->GetAppLocalListAndDoIncrementalBackup();
            HILOGI("Will notify backup result report");
            extensionPtr->AppResultReport(backupExRetInfo, BackupRestoreScenario::INCREMENTAL_BACKUP);
        }
    };
}

std::function<void(ErrCode, const std::string)> BackupExtExtension::HandleTaskBackupEx(wptr<BackupExtExtension> obj)
{
    HILOGI("Begin get HandleTaskBackupEx");
    return [obj](ErrCode errCode, const std::string backupExRetInfo) {
        HILOGI("begin call backup");
        auto extensionPtr = obj.promote();
        if (extensionPtr == nullptr) {
            HILOGE("Ext extension handle have been released");
            return;
        }
        if (extensionPtr->extension_ == nullptr) {
            HILOGE("Extension handle have been released");
            return;
        }
        extensionPtr->extension_->CallExtRestore(errCode, backupExRetInfo);
        if (backupExRetInfo.size()) {
            extensionPtr->AsyncTaskBackup(extensionPtr->extension_->GetUsrConfig());
            HILOGI("Will notify backup result report");
            extensionPtr->AppResultReport(backupExRetInfo, BackupRestoreScenario::FULL_BACKUP);
        }
    };
}

void BackupExtExtension::WaitToSendFd(std::chrono::system_clock::time_point &startTime, int &fdSendNum)
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

void BackupExtExtension::RefreshTimeInfo(std::chrono::system_clock::time_point &startTime, int &fdSendNum)
{
    auto currentTime = std::chrono::system_clock::now();
    auto useTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
    if (useTime >= MAX_FD_GROUP_USE_TIME) {
        HILOGI("RefreshTimeInfo Begin, fdSendNum is:%{public}d", fdSendNum);
        startTime = std::chrono::system_clock::now();
        fdSendNum = 0;
    }
}
} // namespace OHOS::FileManagement::Backup
