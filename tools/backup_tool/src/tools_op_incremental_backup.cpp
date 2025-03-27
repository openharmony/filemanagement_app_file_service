/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "tools_op_incremental_backup.h"

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
#include <sys/resource.h>
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
#include "service_client.h"
#include "tools_op.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class SessionBckup {
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
        TryClearBundleOfMap(bundleName);
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

    unique_ptr<BIncrementalBackupSession> session_ = {};

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
    vector<BIncrementalData> lastIncrementalData {};
};

static string GenHelpMsg()
{
    return "\t\tThis operation helps to backup application data.\n"
           "\t\t--isLocal\t\t This parameter should be true or flase; true: local backup false: others.\n"
           "\t\t--pathCapFile\t\t This parameter should be the path of the capability file.\n"
           "\t\t--bundle\t\t This parameter is bundleName.";
}

static void OnFileReady(shared_ptr<SessionBckup> ctx, const BFileInfo &fileInfo, UniqueFd fd, UniqueFd manifestFd)
{
    printf("FileReady owner = %s, fileName = %s, fd = %d, manifestFd = %d\n", fileInfo.owner.c_str(),
           fileInfo.fileName.c_str(), fd.Get(), manifestFd.Get());
    string tmpPath = string(BConstants::BACKUP_TOOL_INCREMENTAL_RECEIVE_DIR) + fileInfo.owner;
    if (access(tmpPath.data(), F_OK) != 0 && mkdir(tmpPath.data(), S_IRWXU) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    auto iter = find_if(ctx->lastIncrementalData.begin(), ctx->lastIncrementalData.end(),
                        [bundleName {fileInfo.owner}](const auto &data) { return bundleName == data.bundleName; });
    if (iter == ctx->lastIncrementalData.end()) {
        throw BError(BError::Codes::TOOL_INVAL_ARG);
    }
    tmpPath = tmpPath + "/" + to_string(iter->lastIncrementalTime);
    if (access(tmpPath.data(), F_OK) != 0 && mkdir(tmpPath.data(), S_IRWXU) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    // 数据文件保存 路径拼接方式: /data/backup/incrementalreceived/bundleName/时间戳/incremental/文件名
    string tmpDataPath = tmpPath + string(BConstants::BACKUP_TOOL_INCREMENTAL);
    if (access(tmpDataPath.data(), F_OK) != 0 && mkdir(tmpDataPath.data(), S_IRWXU) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    UniqueFd fdLocal(open((tmpDataPath + "/" + fileInfo.fileName).data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU));
    if (fdLocal < 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    BFile::SendFile(fdLocal, fd);

    // 简报文件保存 路径拼接方式: /data/backup/incrementalreceived/bundleName/时间戳/manifest/文件名.rp
    string tmpmanifestPath = tmpPath + string(BConstants::BACKUP_TOOL_MANIFEST);
    if (access(tmpmanifestPath.data(), F_OK) != 0 && mkdir(tmpmanifestPath.data(), S_IRWXU) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    UniqueFd fdManifest(
        open((tmpmanifestPath + "/" + fileInfo.fileName + ".rp").data(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU));
    if (fdManifest < 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    BFile::SendFile(fdManifest, manifestFd);
    if (fileInfo.fileName == BConstants::EXT_BACKUP_MANAGE) {
        string path = string(BConstants::BACKUP_TOOL_INCREMENTAL_RECEIVE_DIR) + fileInfo.owner +
                                string(BConstants::BACKUP_TOOL_MANIFEST).append(".rp");
        UniqueFd fullDatFd(open(path.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU));
        if (fullDatFd < 0) {
            HILOGE("Failed to open rp file = %{private}s, err = %{public}d", path.c_str(), errno);
            return;
        }
        BFile::SendFile(fullDatFd, fdManifest);
        ctx->SetIndexFiles(fileInfo.owner, move(fd));
    } else {
        ctx->UpdateBundleReceivedFiles(fileInfo.owner, fileInfo.fileName);
    }
    ctx->TryNotify();
}

static void OnBundleStarted(shared_ptr<SessionBckup> ctx, ErrCode err, const BundleName name)
{
    printf("BundleStarted errCode = %d, BundleName = %s\n", err, name.c_str());
    if (err != 0) {
        ctx->isAllBundelsFinished.store(true);
        ctx->UpdateBundleFinishedCount();
        ctx->TryNotify();
    }
}

static void OnBundleFinished(shared_ptr<SessionBckup> ctx, ErrCode err, const BundleName name)
{
    printf("BundleFinished errCode = %d, BundleName = %s\n", err, name.c_str());
    ctx->UpdateBundleFinishedCount();
    ctx->TryNotify();
}

static void OnAllBundlesFinished(shared_ptr<SessionBckup> ctx, ErrCode err)
{
    ctx->isAllBundelsFinished.store(true);
    if (err == 0) {
        printf("all bundles backup finished end\n");
    } else {
        printf("Failed to Unplanned Abort error: %d\n", err);
        ctx->TryNotify(true);
        return;
    }
    ctx->TryNotify();
}

static void OnResultReport(shared_ptr<SessionBckup> ctx, const std::string &bundleName, const std::string &resultInfo)
{
    printf("OnResultReport bundleName = %s, resultInfo = %s\n", bundleName.c_str(), resultInfo.c_str());
}

static void OnBackupServiceDied(shared_ptr<SessionBckup> ctx)
{
    printf("backupServiceDied\n");
    ctx->TryNotify(true);
}

static void OnProcess(shared_ptr<SessionBckup> ctx, const std::string &bundleName, const std::string &processInfo)
{
    printf("OnProcess bundleName = %s, processInfo = %s\n", bundleName.c_str(), processInfo.c_str());
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
        if (!ForceRemoveDirectoryBMS(BConstants::BACKUP_TOOL_LINK_DIR.data())) {
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

static int GetLocalCapabilitiesIncremental(shared_ptr<SessionBckup> ctx,
                                           const string &pathCapFile,
                                           const vector<string> &bundleNames,
                                           const vector<string> &times)
{
    if (bundleNames.size() != times.size()) {
        fprintf(stderr, "Inconsistent amounts of bundles and incrementalTime!\n");
        return -EPERM;
    }
    UniqueFd fdLocal(open(pathCapFile.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU));
    if (fdLocal < 0) {
        fprintf(stderr, "Failed to open file. error: %d %s\n", errno, strerror(errno));
        return -EPERM;
    }
    auto proxy = ServiceClient::GetInstance();
    if (!proxy) {
        fprintf(stderr, "Get an empty backup sa proxy\n");
        return -EPERM;
    }
    int num = 0;
    for (const auto &bundleName : bundleNames) {
        BIncrementalData data;
        data.bundleName = bundleName;
        data.lastIncrementalTime = atoi(times[num].c_str());
        ctx->lastIncrementalData.push_back(data);
        num++;
    }
    int fdVal = -1;
    proxy->GetLocalCapabilitiesIncremental(ctx->lastIncrementalData, fdVal);
    UniqueFd fd(fdVal);
    if (fd < 0) {
        fprintf(stderr, "error GetLocalCapabilitiesIncremental");
    } else {
        BFile::SendFile(fdLocal, std::move(fd));
    }
    return 0;
}

static int32_t Init(const string &pathCapFile, const vector<string>& bundleNames, const vector<string>& times)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Init");
    // SELinux backup_tool工具/data/文件夹下创建文件夹 SA服务因root用户的自定义标签无写入权限 此处调整为软链接形式
    BackupToolDirSoftlinkToBackupDir();

    if (access((BConstants::BACKUP_TOOL_INCREMENTAL_RECEIVE_DIR).data(), F_OK) != 0 &&
        mkdir((BConstants::BACKUP_TOOL_INCREMENTAL_RECEIVE_DIR).data(), S_IRWXU) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }

    auto ctx = make_shared<SessionBckup>();
    ctx->session_ = BIncrementalBackupSession::Init(BIncrementalBackupSession::Callbacks {
        .onFileReady = bind(OnFileReady, ctx, placeholders::_1, placeholders::_2, placeholders::_3),
        .onBundleStarted = bind(OnBundleStarted, ctx, placeholders::_1, placeholders::_2),
        .onBundleFinished = bind(OnBundleFinished, ctx, placeholders::_1, placeholders::_2),
        .onAllBundlesFinished = bind(OnAllBundlesFinished, ctx, placeholders::_1),
        .onResultReport = bind(OnResultReport, ctx, placeholders::_1, placeholders::_2),
        .onBackupServiceDied = bind(OnBackupServiceDied, ctx),
        .onProcess = bind(OnProcess, ctx, placeholders::_1, placeholders::_2)});
    if (ctx->session_ == nullptr) {
        printf("Failed to init backup\n");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return -EPERM;
    }

    if (int ret = GetLocalCapabilitiesIncremental(ctx, pathCapFile, bundleNames, times); ret != 0) {
        return ret;
    }
    for (auto &data : ctx->lastIncrementalData) {
        string tmpPath = string(BConstants::BACKUP_TOOL_INCREMENTAL_RECEIVE_DIR) + data.bundleName;
        if (access(tmpPath.data(), F_OK) != 0 && mkdir(tmpPath.data(), S_IRWXU) != 0) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
        }
        tmpPath = tmpPath + string(BConstants::BACKUP_TOOL_MANIFEST).append(".rp");
        data.manifestFd = open(tmpPath.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    }
    int ret = ctx->session_->AppendBundles(ctx->lastIncrementalData);
    if (ret != 0) {
        printf("backup append bundles error: %d\n", ret);
        throw BError(BError::Codes::TOOL_INVAL_ARG, "backup append bundles error");
    }

    ctx->SetBundleFinishedCount(bundleNames.size());
    ctx->Wait();
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    ctx->session_->Release();
    return 0;
}

static int g_exec(map<string, vector<string>> &mapArgToVal)
{
    if (mapArgToVal.find("pathCapFile") == mapArgToVal.end() || mapArgToVal.find("bundles") == mapArgToVal.end() ||
        mapArgToVal.find("incrementalTime") == mapArgToVal.end()) {
        return -EPERM;
    }
    return Init(*(mapArgToVal["pathCapFile"].begin()), mapArgToVal["bundles"], mapArgToVal["incrementalTime"]);
}

bool IncrementalBackUpRegister()
{
    return ToolsOp::Register(ToolsOp {ToolsOp::Descriptor {
        .opName = {"incrementalbackup"},
        .argList = {{
                        .paramName = "pathCapFile",
                        .repeatable = false,
                    },
                    {
                        .paramName = "bundles",
                        .repeatable = true,
                    },
                    {
                        .paramName = "incrementalTime",
                        .repeatable = true,
                    }},
        .funcGenHelpMsg = GenHelpMsg,
        .funcExec = g_exec,
    }});
}
} // namespace OHOS::FileManagement::Backup