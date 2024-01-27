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
#include <iomanip>
#include <regex>
#include <string>
#include <vector>

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
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_resources/b_constants.h"
#include "b_tarball/b_tarball_factory.h"
#include "filemgmt_libhilog.h"
#include "service_proxy.h"
#include "hitrace_meter.h"
#include "tar_file.h"
#include "untar_file.h"

namespace OHOS::FileManagement::Backup {
const string DEFAULT_TAR_PKG = "1.tar";
const string INDEX_FILE_BACKUP = string(BConstants::PATH_BUNDLE_BACKUP_HOME).
                                 append(BConstants::SA_BUNDLE_BACKUP_BACKUP).
                                 append(BConstants::EXT_BACKUP_MANAGE);
const string INDEX_FILE_RESTORE = string(BConstants::PATH_BUNDLE_BACKUP_HOME).
                                  append(BConstants::SA_BUNDLE_BACKUP_RESTORE).
                                  append(BConstants::EXT_BACKUP_MANAGE);
using namespace std;

void BackupExtExtension::VerifyCaller()
{
    HILOGI("begin");
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

static bool CheckAndCreateDirectory(const string& filePath)
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
    HILOGE("GetFileHandleForSpecialCloneCloud: fileName is %{public}s", fileName.data());
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
    return UniqueFd(open(fileName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
}

UniqueFd BackupExtExtension::GetFileHandle(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
            HILOGI("Failed to get file handle, because action is %{public}d invalid", extension_->GetExtensionAction());
            throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
        }

        VerifyCaller();

        if (extension_->SpeicalVersionForCloneAndCloud()) {
            return GetFileHandleForSpecialCloneCloud(fileName);
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
        return UniqueFd(open(tarName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
    } catch (...) {
        HILOGE("Failed to get file handle");
        DoClear();
        return UniqueFd(-1);
    }
}

ErrCode BackupExtExtension::GetIncrementalFileHandle(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    return ERR_OK;
}

ErrCode BackupExtExtension::HandleClear()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("begin clear");
    if (extension_->GetExtensionAction() == BConstants::ExtensionAction::INVALID) {
        throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
    }
    VerifyCaller();
    DoClear();
    return ERR_OK;
}

static ErrCode IndexFileReady(const TarMap &pkgInfo, sptr<IService> proxy)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(
        UniqueFd(open(INDEX_FILE_BACKUP.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)));
    auto cache = cachedEntity.Structuralize();
    cache.SetExtManage(pkgInfo);
    cachedEntity.Persist();
    close(cachedEntity.GetFd().Release());

    ErrCode ret =
        proxy->AppFileReady(string(BConstants::EXT_BACKUP_MANAGE), UniqueFd(open(INDEX_FILE_BACKUP.data(), O_RDONLY)));
    if (SUCCEEDED(ret)) {
        HILOGI("The application is packaged successfully");
    } else {
        HILOGI(
            "The application is packaged successfully but the AppFileReady interface fails to be invoked: "
            "%{public}d",
            ret);
    }
    return ret;
}

static ErrCode BigFileReady(sptr<IService> proxy)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(INDEX_FILE_BACKUP.data(), O_RDONLY)));
    auto cache = cachedEntity.Structuralize();
    auto pkgInfo = cache.GetExtManageInfo();

    ErrCode ret {ERR_OK};
    for (auto &item : pkgInfo) {
        if (item.hashName.empty() || item.fileName.empty()) {
            continue;
        }

        UniqueFd fd(open(item.fileName.data(), O_RDONLY));
        if (fd < 0) {
            HILOGE("open file failed, file name is %{public}s, err = %{public}d", item.fileName.c_str(), errno);
            continue;
        }

        ret = proxy->AppFileReady(item.hashName, std::move(fd));
        if (SUCCEEDED(ret)) {
            HILOGI("The application is packaged successfully, package name is %{public}s", item.hashName.c_str());
        } else {
            HILOGI(
                "The application is packaged successfully but the AppFileReady interface fails to be invoked: "
                "%{public}d",
                ret);
        }
    }
    return ret;
}

static bool IsAllFileReceived(vector<string> tars, bool isSpeicalVersion)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    // 是否已收到索引文件
    if (isSpeicalVersion) {
        HILOGI("Check if manage.json is received for SpeicalVersion.");
        string manageFileName = INDEX_FILE_RESTORE;
        if (manageFileName.front() == BConstants::FILE_SEPARATOR_CHAR) {
            manageFileName = manageFileName.substr(1);
        }
        if (find(tars.begin(), tars.end(), manageFileName) == tars.end()) {
            return false;
        }
    } else {
        HILOGI("Check if manage.json is received.");
        if (find(tars.begin(), tars.end(), string(BConstants::EXT_BACKUP_MANAGE)) == tars.end()) {
            return false;
        }
    }

    // 获取索引文件内容
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(INDEX_FILE_RESTORE.data(), O_RDONLY)));
    auto cache = cachedEntity.Structuralize();
    set<string> info = cache.GetExtManage();

    // 从数量上判断是否已经全部收到
    if (tars.size() <= info.size()) {
        return false;
    }

    // 逐个判断是否收到
    sort(tars.begin(), tars.end());
    return includes(tars.begin(), tars.end(), info.begin(), info.end());
}

ErrCode BackupExtExtension::PublishFile(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGE("begin publish file. fileName is %{public}s", fileName.data());
    try {
        if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
            throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
        }
        VerifyCaller();
        // 是否指定克隆模式
        bool isSpeicalVersion = extension_->SpeicalVersionForCloneAndCloud();

        string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
        string tarName = isSpeicalVersion ? fileName : path + fileName;
        {
            BExcepUltils::VerifyPath(tarName, !isSpeicalVersion);
            unique_lock<shared_mutex> lock(lock_);
            if (find(tars_.begin(), tars_.end(), fileName) != tars_.end() || access(tarName.data(), F_OK) != 0) {
                throw BError(BError::Codes::EXT_INVAL_ARG, "The file does not exist");
            }
            tars_.push_back(fileName);
            if (!IsAllFileReceived(tars_, isSpeicalVersion)) {
                return ERR_OK;
            }
        }
        // 异步执行解压操作
        if (extension_->AllowToBackupRestore()) {
            AsyncTaskRestore();
        }

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
    return ERR_OK;
}

ErrCode BackupExtExtension::HandleBackup()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    string usrConfig = extension_->GetUsrConfig();
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(usrConfig);
    auto cache = cachedEntity.Structuralize();
    if (!cache.GetAllowToBackupRestore()) {
        HILOGI("Application does not allow backup or restore");
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE, "Application does not allow backup or restore")
            .GetCode();
    }
    AsyncTaskOnBackup();
    return 0;
}

static bool IsUserTar(const string &tarFile, const string &indexFile)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (tarFile.empty()) {
        return false;
    }
    string filePath = BExcepUltils::Canonicalize(indexFile);
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(filePath.data(), O_RDONLY)));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManageInfo();
    auto iter = find_if(info.begin(), info.end(),
    [&tarFile](const auto& item) { return item.hashName == tarFile; });
    if (iter != info.end()) {
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
    HILOGI("Do backup");
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
    auto [bigFileInfo, smallFiles] = GetFileInfos(includes, excludes);
    for (const auto &item : bigFileInfo) {
        auto filePath = std::get<0>(item.second);
        if (!filePath.empty()) {
            excludes.push_back(filePath);
        }
    }

    // 分片打包
    TarMap tarMap {};
    TarFile::GetInstance().Packet(smallFiles, "part", path, tarMap);
    bigFileInfo.insert(tarMap.begin(), tarMap.end());

    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
    }

    if (auto ret = IndexFileReady(bigFileInfo, proxy); ret) {
        return ret;
    }
    auto res = BigFileReady(proxy);
    HILOGI("HandleBackup finish, ret = %{public}d", res);
    return res;
}

int BackupExtExtension::DoRestore(const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Do restore");
    if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
        return EPERM;
    }
    // REM: 给定version
    // REM: 解压启动Extension时即挂载好的备份目录中的数据
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    string tarName = path + fileName;

    // 当用户指定fullBackupOnly字段或指定版本的恢复，解压目录当前在/backup/restore
    if (extension_->SpeicalVersionForCloneAndCloud() || extension_->UseFullBackupOnly()) {
        UntarFile::GetInstance().UnPacket(tarName, path);
    } else {
        UntarFile::GetInstance().UnPacket(tarName, "/");
    }
    HILOGI("Application recovered successfully, package path is %{public}s", tarName.c_str());

    return ERR_OK;
}

void BackupExtExtension::AsyncTaskBackup(const string config)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto task = [obj {wptr<BackupExtExtension>(this)}, config]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK,
                              "Ext extension handle have been already released");
        try {
            BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(config);
            auto cache = cachedEntity.Structuralize();
            auto ret = ptr->DoBackup(cache);
            // REM: 处理返回结果 ret
            ptr->AppDone(ret);
            HILOGE("backup app done %{public}d", ret);
        } catch (const BError &e) {
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
    struct stat& sta = item.sta;
    string fileName = item.hashName;
    if (chmod(fileName.c_str(), sta.st_mode) != 0) {
        HILOGE("Failed to chmod filePath, err = %{public}d", errno);
    }

    struct timespec tv[2] = {sta.st_atim, sta.st_mtim};
    UniqueFd fd(open(fileName.data(), O_RDONLY));
    if (futimens(fd.Get(), tv) != 0) {
        HILOGE("Failed to change the file time. %{public}s , %{public}d", fileName.c_str(), errno);
    }
}

static ErrCode RestoreTarForSpeicalCloneCloud(ExtManageInfo item)
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
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(INDEX_FILE_RESTORE.data(), O_RDONLY)));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManageInfo();

    for (auto &item : info) {
        if (item.hashName.empty()) {
            continue;
        }
        if (item.isUserTar || item.isBigFile) {
            // 大文件处理
            RestoreBigFilesForSpecialCloneCloud(item);
        } else {
            // 待解压tar文件处理
            if (RestoreTarForSpeicalCloneCloud(item) != ERR_OK) {
                HILOGE("Failed to restore tar file %{public}s", item.hashName.c_str());
                return ERR_INVALID_VALUE;
            }
        }
    }
    if (!RemoveFile(INDEX_FILE_RESTORE)) {
        HILOGE("Failed to delete the backup index %{public}s", INDEX_FILE_RESTORE.c_str());
    }
    HILOGI("after extra, do restore for SpecialCloneCloud.");
    return ERR_OK;
}

static bool RestoreBigFilePrecheck(string& fileName, const string& path,
                                   const string& hashName, const string& filePath)
{
    if (filePath.empty()) {
        HILOGE("file path is empty. %{public}s", filePath.c_str());
        return false;
    }

    // 不带路径的文件名
    if (access(fileName.data(), F_OK) != 0) {
        HILOGI("file does not exist");
        return false;
    }

    // 目录不存在且只有大文件时，不能通过untar创建，需要检查并创建
    if (!CheckAndCreateDirectory(filePath)) {
        HILOGE("failed to create directory %{public}s", filePath.c_str());
        return false;
    }
    return true;
}

static void RestoreBigFileAfter(const string& fileName, const string& filePath, const struct stat& sta,
                                const set<string>& lks)
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

    for (const auto &lksPath : lks) {
        if (link(filePath.data(), lksPath.data())) {
            HILOGE("failed to create hard link file, errno : %{public}d", errno);
        }
    }

    struct timespec tv[2] = {sta.st_atim, sta.st_mtim};
    UniqueFd fd(open(filePath.data(), O_RDONLY));
    if (futimens(fd.Get(), tv) != 0) {
        HILOGI("failed to change the file time. %{public}s , %{public}d", filePath.c_str(), errno);
    }
}

static void RestoreBigFiles(bool appendTargetPath)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    // 获取索引文件内容
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(INDEX_FILE_RESTORE.data(), O_RDONLY)));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManageInfo();

    for (auto &item : info) {
        if (item.hashName.empty() || (!item.isUserTar && !item.isBigFile)) {
            continue;
        }

        string fileName = path + item.hashName;
        string filePath = appendTargetPath ? (path + item.fileName) : item.fileName;

        if (!RestoreBigFilePrecheck(fileName, path, item.hashName, filePath)) {
            continue;
        }

        if (!BFile::CopyFile(fileName, filePath)) {
            HILOGE("failed to copy the file. err = %{public}d", errno);
            continue;
        }
        
        RestoreBigFileAfter(fileName, filePath, item.sta, cache.GetHardLinkInfo(item.hashName));
    }
}

static void DeleteBackupTars()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    // The directory include tars and manage.json which would be deleted
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(INDEX_FILE_RESTORE.data(), O_RDONLY)));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManage();
    auto path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    for (auto &item : info) {
        if (ExtractFileExt(item) != "tar" || IsUserTar(item, INDEX_FILE_RESTORE)) {
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
}

void BackupExtExtension::AsyncTaskRestore()
{
    auto task = [obj {wptr<BackupExtExtension>(this)}, tars {tars_}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK, "Ext extension handle have already released");
        try {
            int ret = ERR_OK;
            if (ptr->extension_->SpeicalVersionForCloneAndCloud()) {
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
            for (auto item : tars) { // 处理要解压的tar文件
                if (ExtractFileExt(item) == "tar" && !IsUserTar(item, INDEX_FILE_RESTORE)) {
                    ret = ptr->DoRestore(item);
                }
            }
            // 恢复用户tar包以及大文件
            // 目的地址是否需要拼接path(临时目录)，FullBackupOnly为true并且非特殊场景
            bool appendTargetPath = ptr->extension_->UseFullBackupOnly() &&
                                    !ptr->extension_->SpeicalVersionForCloneAndCloud();
            RestoreBigFiles(appendTargetPath);

            // delete 1.tar/manage.json
            DeleteBackupTars();

            if (ret == ERR_OK) {
                HILOGI("after extra, do restore.");
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

void BackupExtExtension::AsyncTaskRestoreForUpgrade()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
        auto ptr = obj.promote();
        try {
            BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK,
                                  "Ext extension handle have been already released");
            BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG,
                                  "extension handle have been already released");

            auto callBackup = [obj]() {
                HILOGI("begin call restore");
                auto extensionPtr = obj.promote();
                BExcepUltils::BAssert(extensionPtr, BError::Codes::EXT_BROKEN_FRAMEWORK,
                                      "Ext extension handle have been already released");
                extensionPtr->AppDone(BError(BError::Codes::OK));
                // 清空恢复目录
                extensionPtr->DoClear();
            };
            ptr->extension_->OnRestore(callBackup);
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

void BackupExtExtension::ExtClear()
{
    HILOGI("ext begin clear");
    DoClear();
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
        tars_.clear();
    } catch (...) {
        HILOGI("Failed to clear");
    }
}

void BackupExtExtension::AppDone(ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto proxy = ServiceProxy::GetInstance();
    BExcepUltils::BAssert(proxy, BError::Codes::EXT_BROKEN_IPC, "Failed to obtain the ServiceProxy handle");
    auto ret = proxy->AppDone(errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed to notify the app done. err = %{public}d", ret);
    }
}

void BackupExtExtension::AsyncTaskOnBackup()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
        auto ptr = obj.promote();
        try {
            BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK,
                                  "Ext extension handle have been already released");
            BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG,
                                  "extension handle have been already released");

            auto callBackup = [obj]() {
                HILOGI("begin call backup");
                auto extensionPtr = obj.promote();
                BExcepUltils::BAssert(extensionPtr, BError::Codes::EXT_BROKEN_FRAMEWORK,
                                      "Ext extension handle have been already released");
                BExcepUltils::BAssert(extensionPtr->extension_, BError::Codes::EXT_INVAL_ARG,
                                      "extension handle have been already released");
                extensionPtr->AsyncTaskBackup(extensionPtr->extension_->GetUsrConfig());
            };

            ptr->extension_->OnBackup(callBackup);
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
    if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
        HILOGI("Failed to get file handle, because action is %{public}d invalid", extension_->GetExtensionAction());
        throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
    }
    // read backup_config is allow to backup or restore
    if (!extension_->AllowToBackupRestore()) {
        HILOGI("Application does not allow backup or restore");
        return BError(BError::Codes::EXT_FORBID_BACKUP_RESTORE, "Application does not allow backup or restore")
            .GetCode();
    }

    // async do restore.
    if (extension_->WasFromSpeicalVersion() && extension_->RestoreDataReady()) {
        HILOGI("Restore directly when upgrading.");
        AsyncTaskRestoreForUpgrade();
    }

    return 0;
}

ErrCode BackupExtExtension::HandleIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    return 0;
}

tuple<UniqueFd, UniqueFd> BackupExtExtension::GetIncrementalBackupFileHandle()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    return {UniqueFd(-1), UniqueFd(-1)};
}
} // namespace OHOS::FileManagement::Backup
