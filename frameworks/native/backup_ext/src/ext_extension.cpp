/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

namespace OHOS::FileManagement::Backup {
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

UniqueFd BackupExtExtension::GetFileHandle(const string &fileName)
{
    try {
        if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
            HILOGI("Failed to get file handle, because action is %{public}d invalid", extension_->GetExtensionAction());
            throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
        }

        VerifyCaller();

        if (!regex_match(fileName, regex("^[0-9a-zA-Z_.]+$"))) {
            throw BError(BError::Codes::EXT_INVAL_ARG, "Filename is not alphanumeric");
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

ErrCode BackupExtExtension::HandleClear()
{
    HILOGI("begin clear");
    if (extension_->GetExtensionAction() == BConstants::ExtensionAction::INVALID) {
        throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
    }
    VerifyCaller();
    DoClear();
    return ERR_OK;
}

static ErrCode IndexFileReady(const map<string, pair<string, struct stat>> &pkgInfo, sptr<IService> proxy)
{
    string indexFile = string(BConstants::PATH_BUNDLE_BACKUP_HOME).
                            append(BConstants::SA_BUNDLE_BACKUP_BACKUP).
                            append(BConstants::EXT_BACKUP_MANAGE);
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(
        UniqueFd(open(indexFile.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)));
    auto cache = cachedEntity.Structuralize();
    cache.SetExtManage(pkgInfo);
    cachedEntity.Persist();
    close(cachedEntity.GetFd().Release());

    ErrCode ret =
        proxy->AppFileReady(string(BConstants::EXT_BACKUP_MANAGE), UniqueFd(open(indexFile.data(), O_RDONLY)));
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
    string indexFile = string(BConstants::PATH_BUNDLE_BACKUP_HOME).
                            append(BConstants::SA_BUNDLE_BACKUP_BACKUP).
                            append(BConstants::EXT_BACKUP_MANAGE);
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(indexFile.data(), O_RDONLY)));
    auto cache = cachedEntity.Structuralize();
    map<string, pair<string, struct stat>> pkgInfo = cache.GetExtManageInfo();

    ErrCode ret {ERR_OK};
    for (auto &item : pkgInfo) {
        if (item.first.empty() || item.second.first.empty()) {
            continue;
        }

        UniqueFd fd(open(item.second.first.data(), O_RDONLY));
        if (fd < 0) {
            HILOGE("open file failed, file name is %{public}s", std::get<0>(item.second).c_str());
            continue;
        }

        ret = proxy->AppFileReady(item.first, std::move(fd));
        if (SUCCEEDED(ret)) {
            HILOGI("The application is packaged successfully, package name is %{public}s", item.first.c_str());
        } else {
            HILOGI(
                "The application is packaged successfully but the AppFileReady interface fails to be invoked: "
                "%{public}d",
                ret);
        }
    }
    return ret;
}

ErrCode BackupExtExtension::PublishFile(const string &fileName)
{
    HILOGE("begin publish file. fileName is %{public}s", fileName.data());
    try {
        if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
            throw BError(BError::Codes::EXT_INVAL_ARG, "Action is invalid");
        }
        VerifyCaller();

        string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
        string tarName = path + fileName;
        {
            BExcepUltils::VerifyPath(tarName, true);
            unique_lock<shared_mutex> lock(lock_);
            if (find(tars_.begin(), tars_.end(), fileName) != tars_.end() || access(tarName.data(), F_OK) != 0) {
                throw BError(BError::Codes::EXT_INVAL_ARG, "The file does not exist");
            }
            tars_.push_back(fileName);
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

ErrCode BackupExtExtension::HandleBackup()
{
    string usrConfig = extension_->GetUsrConfig();
    AsyncTaskBackup(usrConfig);
    return 0;
}

static map<string, pair<string, struct stat>> GetBigFileInfo(const vector<string> &includes,
                                                             const vector<string> &excludes)
{
    auto [errCode, files] = BDir::GetBigFiles(includes, excludes);
    if (errCode != 0) {
        return {};
    }

    auto GetStringHash = [](const map<string, pair<string, struct stat>> &m, const string &str) -> string {
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

    map<string, pair<string, struct stat>> bigFiles;
    for (const auto &item : files) {
        string md5Name = GetStringHash(bigFiles, item.first);
        if (!md5Name.empty()) {
            bigFiles.emplace(md5Name, make_pair(item.first, item.second));
        }
    }

    return bigFiles;
}

int BackupExtExtension::DoBackup(const BJsonEntityExtensionConfig &usrConfig)
{
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
    auto bigFileInfo = GetBigFileInfo(includes, excludes);
    for (const auto &item : bigFileInfo) {
        if (!item.second.first.empty()) {
            excludes.push_back(item.second.first);
        }
    }

    string pkgName = "1.tar";
    string tarName = path + pkgName;
    string root = "/";

    // 打包
    auto tarballTar = BTarballFactory::Create("cmdline", tarName);
    (tarballTar->tar)(root, {includes.begin(), includes.end()}, {excludes.begin(), excludes.end()});

    struct stat sta = {};
    if (stat(tarName.data(), &sta) == -1) {
        HILOGE("failed to invoke the system function stat, %{public}s", tarName.c_str());
    }
    bigFileInfo.emplace(pkgName, make_pair(tarName, sta));

    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
    }

    ErrCode ret = ERR_OK;
    ret = IndexFileReady(bigFileInfo, proxy);
    ret = BigFileReady(proxy);
    HILOGE("HandleBackup finish, ret = %{public}d", ret);
    return ret;
}

int BackupExtExtension::DoRestore(const string &fileName)
{
    HILOGI("Do restore");
    if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
        return EPERM;
    }
    // REM: 给定version
    // REM: 解压启动Extension时即挂载好的备份目录中的数据
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    string tarName = path + fileName;

    auto tarballFunc = BTarballFactory::Create("cmdline", tarName);
    if (extension_->WasFromSpeicalVersion()) {
        (tarballFunc->untar)(path);
    } else {
        (tarballFunc->untar)("/");
    }
    HILOGI("Application recovered successfully, package path is %{public}s", tarName.c_str());

    return ERR_OK;
}

void BackupExtExtension::AsyncTaskBackup(const string config)
{
    auto task = [obj {wptr<BackupExtExtension>(this)}, config]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK,
                              "Ext extension handle have been already released");

        try {
            BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(config);
            auto cache = cachedEntity.Structuralize();
            if (!cache.GetAllowToBackupRestore()) {
                HILOGI("Application does not allow backup or restore");
                return;
            }
            BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG,
                                  "extension handle have been already released");

            int ret = ptr->extension_->OnBackup();
            if (ret == ERR_OK) {
                ret = ptr->DoBackup(cache);
            }

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

static bool IsAllFileReceived(vector<string> tars)
{
    // 是否已收到索引文件
    auto it = find(tars.begin(), tars.end(), string(BConstants::EXT_BACKUP_MANAGE));
    if (tars.end() == it) {
        return false;
    }

    // 获取索引文件内容
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    string indexFile = path + string(BConstants::EXT_BACKUP_MANAGE);
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(indexFile.data(), O_RDONLY)));
    auto cache = cachedEntity.Structuralize();
    set<string> info = cache.GetExtManage();

    // 从数量上判断是否已经全部收到
    if (tars.size() <= info.size()) {
        return false;
    }

    // 逐个判断是否收到
    sort(tars.begin(), tars.end());
    if (includes(tars.begin(), tars.end(), info.begin(), info.end())) {
        return true;
    }
    return false;
}

static void RestoreBigFiles()
{
    // 获取索引文件内容
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    string indexFile = path + string(BConstants::EXT_BACKUP_MANAGE);
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(indexFile.data(), O_RDONLY)));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManageInfo();

    for (auto &item : info) {
        if (item.first.empty() || item.first.size() < BConstants::BIG_FILE_NAME_SIZE ||
            item.first.rfind('.') != string::npos) {
            continue;
        }

        string fileName = path + item.first;
        auto [filePath, sta] = item.second;

        if (access(fileName.data(), F_OK) != 0) {
            HILOGI("file dose not exist");
            continue;
        }
        if (filePath.empty()) {
            HILOGE("file path is empty. %{public}s", filePath.c_str());
            continue;
        }
        if (rename(fileName.data(), filePath.data()) != 0) {
            HILOGE("failed to rename the file, try to copy it. err = %{public}d", errno);
            if (!BFile::CopyFile(fileName, filePath)) {
                HILOGE("failed to copy the file. err = %{public}d", errno);
                continue;
            }
            HILOGI("succeed to rename or copy the file");
        }
        set<string> lks = cache.GetHardLinkInfo(item.first);
        for (const auto &lksPath : lks) {
            if (link(filePath.data(), lksPath.data())) {
                HILOGE("failed to create hard link file %{public}s  errno : %{public}d", lksPath.c_str(), errno);
            }
        }

        struct timespec tv[2] = {sta.st_atim, sta.st_mtim};
        UniqueFd fd(open(filePath.data(), O_RDONLY));
        if (futimens(fd.Get(), tv) != 0) {
            HILOGI("failed to change the file time. %{public}s , %{public}d", filePath.c_str(), errno);
        }
    }
}

void BackupExtExtension::AsyncTaskRestore()
{
    auto task = [obj {wptr<BackupExtExtension>(this)}, tars {tars_}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK,
                              "Ext extension handle have been already released");

        try {
            if (!IsAllFileReceived(tars)) {
                return;
            }

            // 解压
            int ret = ERR_OK;
            for (auto item : tars) {
                if (ExtractFileExt(item) == "tar") {
                    ret = ptr->DoRestore(item);
                }
            }
            // 恢复大文件
            RestoreBigFiles();

            if (ret == ERR_OK) {
                HILOGI("after extra, do restore.");
                BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG,
                                      "extension handle have been already released");
                ret = ptr->extension_->OnRestore();
            }

            // 处理返回结果
            ptr->AppDone(ret);
        } catch (const BError &e) {
            ptr->AppDone(e.GetCode());
        } catch (const exception &e) {
            HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        }
        // 清空恢复目录
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

void BackupExtExtension::AsyncTaskRestoreForUpgrade()
{
    auto task = [obj {wptr<BackupExtExtension>(this)}]() {
        auto ptr = obj.promote();
        BExcepUltils::BAssert(ptr, BError::Codes::EXT_BROKEN_FRAMEWORK,
                              "Ext extension handle have been already released");
        try {
            BExcepUltils::BAssert(ptr->extension_, BError::Codes::EXT_INVAL_ARG,
                                  "extension handle have been already released");
            ptr->AppDone(ptr->extension_->OnRestore());
        } catch (const BError &e) {
            ptr->AppDone(e.GetCode());
        } catch (const exception &e) {
            HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        } catch (...) {
            HILOGE("Failed to restore the ext bundle");
            ptr->AppDone(BError(BError::Codes::EXT_INVAL_ARG).GetCode());
        }
        // 清空恢复目录
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

void BackupExtExtension::DoClear()
{
    try {
        string backupCache = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
        string restoreCache = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);

        if (!ForceRemoveDirectory(backupCache)) {
            HILOGI("Failed to delete the backup cache %{public}s", backupCache.c_str());
        }

        if (!ForceRemoveDirectory(restoreCache)) {
            HILOGI("Failed to delete the restore cache %{public}s", restoreCache.c_str());
        }
        unique_lock<shared_mutex> lock(lock_);
        tars_.clear();
    } catch (...) {
        HILOGI("Failed to clear");
    }
}

void BackupExtExtension::AppDone(ErrCode errCode)
{
    auto proxy = ServiceProxy::GetInstance();
    BExcepUltils::BAssert(proxy, BError::Codes::EXT_BROKEN_IPC, "Failed to obtain the ServiceProxy handle");
    auto ret = proxy->AppDone(errCode);
    if (ret != ERR_OK) {
        HILOGE("Failed to notify the app done. err = %{public}d", ret);
    }
}
} // namespace OHOS::FileManagement::Backup
