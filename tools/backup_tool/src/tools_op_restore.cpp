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

#include <cerrno>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <mutex>
#include <regex>
#include <set>
#include <string>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>
#include <vector>

#include "b_error/b_error.h"
#include "b_filesystem/b_dir.h"
#include "b_filesystem/b_file.h"
#include "b_json/b_json_entity_caps.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_resources/b_constants.h"
#include "backup_kit_inner.h"
#include "base/hiviewdfx/hitrace/interfaces/native/innerkits/include/hitrace_meter/hitrace_meter.h"
#include "errors.h"
#include "service_proxy.h"
#include "tools_op.h"
#include "tools_op_restore.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class Session {
public:
    void UpdateBundleSendFiles(const BundleName &bundleName, const string &fileName)
    {
        lock_guard<mutex> lk(lock_);
        bundleStatusMap_[bundleName].sendFile.insert(fileName);
    }

    void UpdateBundleSentFiles(const BundleName &bundleName, const string &fileName)
    {
        lock_guard<mutex> lk(lock_);
        bundleStatusMap_[bundleName].sentFile.insert(fileName);
        TryClearBundleOfMap(bundleName);
    }

    void ClearBundleOfMap(const BundleName &bundleName)
    {
        lock_guard<mutex> lk(lock_);
        bundleStatusMap_.erase(bundleName);
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

    unique_ptr<BSessionRestore> session_ = {};

private:
    struct BundleStatus {
        set<string> sendFile;
        set<string> sentFile;
    };

    void TryClearBundleOfMap(const BundleName &bundleName)
    {
        if (bundleStatusMap_[bundleName].sendFile == bundleStatusMap_[bundleName].sentFile) {
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
    return "\t\tThis operation helps to restore application data.\n"
           "\t\t--pathCapFile\t\t This parameter should be the path of the capability file.\n"
           "\t\t--bundle\t\t This parameter is bundleName.";
}

static void OnFileReady(shared_ptr<Session> ctx, const BFileInfo &fileInfo, UniqueFd fd)
{
    printf("FileReady owner = %s, fileName = %s, sn = %u, fd = %d\n", fileInfo.owner.c_str(), fileInfo.fileName.c_str(),
           fileInfo.sn, fd.Get());
    if (fileInfo.fileName.find('/') != string::npos &&
        fileInfo.fileName != BConstants::RESTORE_INSTALL_PATH) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, "Filename is not valid");
    }
    string tmpPath;
    if (fileInfo.fileName == BConstants::RESTORE_INSTALL_PATH) {
        printf("OnFileReady bundle hap\n");
        tmpPath = string(BConstants::BACKUP_TOOL_INSTALL_DIR) + fileInfo.owner + ".hap";
    } else {
        tmpPath = string(BConstants::BACKUP_TOOL_RECEIVE_DIR) + fileInfo.owner + "/" + fileInfo.fileName;
    }
    if (access(tmpPath.data(), F_OK) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    UniqueFd fdLocal(open(tmpPath.data(), O_RDONLY));
    if (fdLocal < 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    BFile::SendFile(fd, fdLocal);
    int ret = ctx->session_->PublishFile(fileInfo);
    if (ret != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, "PublishFile error");
    }
    if (fileInfo.fileName != BConstants::RESTORE_INSTALL_PATH) {
        ctx->UpdateBundleSentFiles(fileInfo.owner, fileInfo.fileName);
    }
    ctx->TryNotify();
}

static void OnBundleStarted(shared_ptr<Session> ctx, ErrCode err, const BundleName name)
{
    printf("BundleStarted errCode = %d, BundleName = %s\n", err, name.c_str());
    if (err != 0) {
        ctx->UpdateBundleFinishedCount();
        ctx->isAllBundelsFinished.store(true);
        ctx->ClearBundleOfMap(name);
        ctx->TryNotify();
    }
}

static void OnBundleFinished(shared_ptr<Session> ctx, ErrCode err, const BundleName name)
{
    printf("BundleFinished errCode = %d, BundleName = %s\n", err, name.c_str());
    ctx->UpdateBundleFinishedCount();
    if (err != 0) {
        ctx->isAllBundelsFinished.store(true);
        ctx->ClearBundleOfMap(name);
    }
    ctx->TryNotify();
}

static void OnAllBundlesFinished(shared_ptr<Session> ctx, ErrCode err)
{
    ctx->isAllBundelsFinished.store(true);
    if (err == 0) {
        printf("all bundles restore finished end\n");
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

static void RestoreApp(shared_ptr<Session> restore, vector<BundleName> &bundleNames)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "RestoreApp");
    if (!restore || !restore->session_) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    for (auto &bundleName : bundleNames) {
        if (bundleName.find('/') != string::npos) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, "Filename is not valid");
        }
        string path = string(BConstants::BACKUP_TOOL_RECEIVE_DIR) + bundleName;
        if (access(path.data(), F_OK) != 0) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
        }
        const auto [err, filePaths] = BDir::GetDirFiles(path);
        if (err != 0) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, "error path");
        }
        // install bundle.hap
        string installPath = string(BConstants::BACKUP_TOOL_INSTALL_DIR) + bundleName + ".hap";
        if (access(installPath.data(), F_OK) == 0) {
            printf("install bundle hap %s\n", installPath.c_str());
            restore->session_->GetFileHandle(bundleName, string(BConstants::RESTORE_INSTALL_PATH));
        }
        for (auto &filePath : filePaths) {
            string fileName = filePath.substr(filePath.rfind("/") + 1);
            restore->session_->GetFileHandle(bundleName, fileName);
            restore->UpdateBundleSendFiles(bundleName, fileName);
        }
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
}

static bool GetRealPath(string &path)
{
    unique_ptr<char[]> absPath = make_unique<char[]>(PATH_MAX + 1);
    if (realpath(path.c_str(), absPath.get()) == nullptr) {
        return false;
    }

    path = absPath.get();
    if (access(path.data(), F_OK) != 0) {
        return false;
    }

    return true;
}

static int32_t InitFd(UniqueFd &fd, const vector<string> &bundleNames)
{
    if (fd < 0) {
        fprintf(stderr, "Failed to open file error: %d %s\n", errno, strerror(errno));
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return -errno;
    }
    if (access(BConstants::BACKUP_TOOL_INSTALL_DIR.data(), F_OK) == 0) {
        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(move(fd));
        auto cache = cachedEntity.Structuralize();
        vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        for (auto name : bundleNames) {
            string installPath = string(BConstants::BACKUP_TOOL_INSTALL_DIR) + name + ".hap";
            if (access(installPath.data(), F_OK) == 0) {
                bool needToInstall = true;
                bundleInfos.emplace_back(BJsonEntityCaps::BundleInfo {.name = name, .needToInstall = needToInstall});
            }
        }
        cache.SetBundleInfos(bundleInfos);
        cachedEntity.Persist();
        fd = move(cachedEntity.GetFd());
    }
    return 0;
}

static int32_t InitRestoreSession(shared_ptr<Session> ctx)
{
    if (!ctx) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
        return -1;
    }
    ctx->session_ = BSessionRestore::Init(
        BSessionRestore::Callbacks {.onFileReady = bind(OnFileReady, ctx, placeholders::_1, placeholders::_2),
                                    .onBundleStarted = bind(OnBundleStarted, ctx, placeholders::_1, placeholders::_2),
                                    .onBundleFinished = bind(OnBundleFinished, ctx, placeholders::_1, placeholders::_2),
                                    .onAllBundlesFinished = bind(OnAllBundlesFinished, ctx, placeholders::_1),
                                    .onBackupServiceDied = bind(OnBackupServiceDied, ctx)});
    if (ctx->session_ == nullptr) {
        printf("Failed to init restore\n");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return -EPERM;
    }
    return 0;
}

static int32_t InitPathCapFile(const string &pathCapFile, vector<string> bundleNames, bool depMode)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Init");
    string realPath = pathCapFile;
    if (!GetRealPath(realPath)) {
        fprintf(stderr, "path to realpath error");
        return -errno;
    }

    UniqueFd fd(open(realPath.data(), O_RDWR, S_IRWXU));
    int32_t ret = InitFd(fd, bundleNames);
    if (ret != 0) {
        printf("Failed to get fd error:%d\n", ret);
        return ret;
    }
    auto ctx = make_shared<Session>();
    ret = InitRestoreSession(ctx);
    if (ret != 0) {
        printf("Failed to init restore session error:%d\n", ret);
        return ret;
    }

    if (depMode) {
        for (auto &bundleName : bundleNames) {
            UniqueFd fd(open(realPath.data(), O_RDWR, S_IRWXU));
            if (fd < 0) {
                fprintf(stderr, "Failed to open file error: %d %s\n", errno, strerror(errno));
                FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
                return -errno;
            }
            int ret = ctx->session_->AppendBundles(move(fd), {bundleName});
            if (ret != 0) {
                printf("restore append bundles error: %d\n", ret);
                return -ret;
            }
        }
    } else {
        ret = ctx->session_->AppendBundles(move(fd), bundleNames);
        if (ret != 0) {
            printf("restore append bundles error: %d\n", ret);
            return -ret;
        }
    }
    ctx->SetBundleFinishedCount(bundleNames.size());
    RestoreApp(ctx, bundleNames);
    ctx->Wait();
    return 0;
}

static int Exec(map<string, vector<string>> &mapArgToVal)
{
    bool depMode = false;
    if (mapArgToVal.find("depMode") != mapArgToVal.end()) {
        string strFlag = *(mapArgToVal["depMode"].begin());
        depMode = (strFlag == "true");
    }

    if (mapArgToVal.find("pathCapFile") == mapArgToVal.end() || mapArgToVal.find("bundles") == mapArgToVal.end()) {
        return -EPERM;
    }
    return InitPathCapFile(*(mapArgToVal["pathCapFile"].begin()), mapArgToVal["bundles"], depMode);
}

bool RestoreRegister()
{
    return ToolsOp::Register(ToolsOp {ToolsOp::Descriptor {
        .opName = {"restore"},
        .argList = {{
                        .paramName = "pathCapFile",
                        .repeatable = false,
                    },
                    {
                        .paramName = "bundles",
                        .repeatable = true,
                    },
                    {
                        .paramName = "depMode",
                        .repeatable = false,
                    }},
        .funcGenHelpMsg = GenHelpMsg,
        .funcExec = Exec,
    }});
}
} // namespace OHOS::FileManagement::Backup