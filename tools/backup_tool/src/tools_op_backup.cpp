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

#include <atomic>
#include <cerrno>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <regex>
#include <set>
#include <string>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <directory_ex.h>

#include "b_error/b_error.h"
#include "b_filesystem/b_file.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_resources/b_constants.h"
#include "backup_kit_inner.h"
#include "base/hiviewdfx/hitrace/interfaces/native/innerkits/include/hitrace_meter/hitrace_meter.h"
#include "service_proxy.h"
#include "tools_op.h"
#include "tools_op_backup.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class Session {
public:
    void UpdateBundleReceivedFiles(const BundleName &bundleName, const string &fileName)
    {
        lock_guard<mutex> lk(lock_);
        bundleStatusMap_[bundleName].receivedFile.insert(fileName);
        TryClearBundleOfMap(bundleName);
    }

    void SetIndexFiles(const BundleName &bundleName, UniqueFd fd)
    {
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
        auto cache = cachedEntity.Structuralize();
        lock_guard<mutex> lk(lock_);
        bundleStatusMap_[bundleName].indexFile = cache.GetExtManage();
    }

    void TryNotify(bool flag = false)
    {
        if (flag == true) {
            ready_ = true;
            cv_.notify_all();
        } else if (bundleStatusMap_.size() == 0 && cnt_ == 0 && isAllBundelsFinished.load()) {
            ready_ = true;
            cv_.notify_all();
        }
    }

    void UpdateBundleFinishedCount()
    {
        lock_guard<mutex> lk(lock_);
        cnt_--;
    }

    void SetBundleFinishedCount(uint32_t cnt)
    {
        cnt_ = cnt;
    }

    void Wait()
    {
        unique_lock<mutex> lk(lock_);
        cv_.wait(lk, [&] { return ready_; });
    }

    unique_ptr<BSessionBackup> session_ = {};

private:
    struct BundleStatus {
        set<string> receivedFile;
        set<string> indexFile;
    };

    void TryClearBundleOfMap(const BundleName &bundleName)
    {
        if (bundleStatusMap_[bundleName].indexFile == bundleStatusMap_[bundleName].receivedFile) {
            bundleStatusMap_.erase(bundleName);
        }
    }

    map<string, BundleStatus> bundleStatusMap_;
    mutable condition_variable cv_;
    mutex lock_;
    bool ready_ = false;
    uint32_t cnt_ {0};

public:
    std::atomic<bool> isAllBundelsFinished {false};
};

static string GenHelpMsg()
{
    return "\t\tThis operation helps to backup application data.\n"
           "\t\t--isLocal\t\t This parameter should be true or flase; true: local backup false: others.\n"
           "\t\t--pathCapFile\t\t This parameter should be the path of the capability file.\n"
           "\t\t--bundle\t\t This parameter is bundleName.";
}

static void OnFileReady(shared_ptr<Session> ctx, const BFileInfo &fileInfo, UniqueFd fd)
{
    printf("FileReady owner = %s, fileName = %s, sn = %u, fd = %d\n", fileInfo.owner.c_str(), fileInfo.fileName.c_str(),
           fileInfo.sn, fd.Get());
    string tmpPath = string(BConstants::BACKUP_TOOL_RECEIVE_DIR) + fileInfo.owner;
    if (access(tmpPath.data(), F_OK) != 0 && mkdir(tmpPath.data(), S_IRWXU) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    if (fileInfo.fileName.find('/') != string::npos) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, "Filename is not valid");
    }
    UniqueFd fdLocal(open((tmpPath + "/" + fileInfo.fileName).data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU));
    if (fdLocal < 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    BFile::SendFile(fdLocal, fd);
    if (fileInfo.fileName == BConstants::EXT_BACKUP_MANAGE) {
        ctx->SetIndexFiles(fileInfo.owner, move(fd));
    } else {
        ctx->UpdateBundleReceivedFiles(fileInfo.owner, fileInfo.fileName);
    }
    ctx->TryNotify();
}

static void OnBundleStarted(shared_ptr<Session> ctx, ErrCode err, const BundleName name)
{
    printf("BundleStarted errCode = %d, BundleName = %s\n", err, name.c_str());
    if (err != 0) {
        ctx->isAllBundelsFinished.store(true);
        ctx->UpdateBundleFinishedCount();
        ctx->TryNotify();
    }
}

static void OnBundleFinished(shared_ptr<Session> ctx, ErrCode err, const BundleName name)
{
    printf("BundleFinished errCode = %d, BundleName = %s\n", err, name.c_str());
    ctx->UpdateBundleFinishedCount();
    ctx->TryNotify();
}

static void OnAllBundlesFinished(shared_ptr<Session> ctx, ErrCode err)
{
    ctx->isAllBundelsFinished.store(true);
    if (err == 0) {
        printf("backup successful\n");
    } else {
        printf("Failed to Unplanned Abort error: %d\n", err);
        ctx->TryNotify(true);
        return;
    }
    ctx->TryNotify();
}

static void OnBackupServiceDied(shared_ptr<Session> ctx)
{
    printf("backupServiceDied\n");
    ctx->TryNotify(true);
}

static void BackupToolDirSoftlinkToBackupDir()
{
    // 判断BConstants::BACKUP_TOOL_LINK_DIR 是否是软链接
    if (access(BConstants::BACKUP_TOOL_LINK_DIR.data(), F_OK) == 0) {
        struct stat inStat = {};
        if (lstat(BConstants::BACKUP_TOOL_LINK_DIR.data(), &inStat) == -1) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
        }

        if ((inStat.st_mode & S_IFMT) == S_IFLNK) {
            return;
        }
        // 非软连接删除重新创建
        if (!ForceRemoveDirectory(BConstants::BACKUP_TOOL_LINK_DIR.data())) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
        }
    }

    if (access(BConstants::GetSaBundleBackupToolDir(BConstants::DEFAULT_USER_ID).data(), F_OK) != 0 &&
        mkdir(BConstants::GetSaBundleBackupToolDir(BConstants::DEFAULT_USER_ID).data(), S_IRWXU) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    if (symlink(BConstants::GetSaBundleBackupToolDir(BConstants::DEFAULT_USER_ID).data(),
                BConstants::BACKUP_TOOL_LINK_DIR.data()) == -1) {
        HILOGE("failed to create soft link file %{public}s  errno : %{public}d",
               BConstants::BACKUP_TOOL_LINK_DIR.data(), errno);
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
}

static int32_t InitPathCapFile(const string &pathCapFile, vector<string> bundleNames)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "InitPathCapFile");
    // SELinux backup_tool工具/data/文件夹下创建文件夹 SA服务因root用户的自定义标签无写入权限 此处调整为软链接形式
    BackupToolDirSoftlinkToBackupDir();

    if (access((BConstants::BACKUP_TOOL_RECEIVE_DIR).data(), F_OK) != 0 &&
        mkdir((BConstants::BACKUP_TOOL_RECEIVE_DIR).data(), S_IRWXU) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }

    UniqueFd fdLocal(open(pathCapFile.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU));
    if (fdLocal < 0) {
        fprintf(stderr, "Failed to open file. error: %d %s\n", errno, strerror(errno));
        return -EPERM;
    }
    auto proxy = ServiceProxy::GetInstance();
    if (!proxy) {
        fprintf(stderr, "Get an empty backup sa proxy\n");
        return -EPERM;
    }
    BFile::SendFile(fdLocal, proxy->GetLocalCapabilities());

    auto ctx = make_shared<Session>();
    ctx->session_ = BSessionBackup::Init(
        BSessionBackup::Callbacks {.onFileReady = bind(OnFileReady, ctx, placeholders::_1, placeholders::_2),
                                   .onBundleStarted = bind(OnBundleStarted, ctx, placeholders::_1, placeholders::_2),
                                   .onBundleFinished = bind(OnBundleFinished, ctx, placeholders::_1, placeholders::_2),
                                   .onAllBundlesFinished = bind(OnAllBundlesFinished, ctx, placeholders::_1),
                                   .onBackupServiceDied = bind(OnBackupServiceDied, ctx)});
    if (ctx->session_ == nullptr) {
        printf("Failed to init backup\n");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return -EPERM;
    }
    int ret = ctx->session_->AppendBundles(bundleNames);
    if (ret != 0) {
        printf("backup append bundles error: %d\n", ret);
        throw BError(BError::Codes::TOOL_INVAL_ARG, "backup append bundles error");
    }

    ctx->SetBundleFinishedCount(bundleNames.size());
    ctx->Wait();
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return 0;
}

static int Exec(map<string, vector<string>> &mapArgToVal)
{
    if (mapArgToVal.find("pathCapFile") == mapArgToVal.end() || mapArgToVal.find("bundles") == mapArgToVal.end() ||
        mapArgToVal.find("isLocal") == mapArgToVal.end()) {
        return -EPERM;
    }
    return InitPathCapFile(*(mapArgToVal["pathCapFile"].begin()), mapArgToVal["bundles"]);
}

bool BackUpRegister()
{
    return ToolsOp::Register(ToolsOp {ToolsOp::Descriptor {
        .opName = {"backup"},
        .argList = {{
                        .paramName = "pathCapFile",
                        .repeatable = false,
                    },
                    {
                        .paramName = "bundles",
                        .repeatable = true,
                    },
                    {
                        .paramName = "isLocal",
                        .repeatable = false,
                    }},
        .funcGenHelpMsg = GenHelpMsg,
        .funcExec = Exec,
    }});
}
} // namespace OHOS::FileManagement::Backup