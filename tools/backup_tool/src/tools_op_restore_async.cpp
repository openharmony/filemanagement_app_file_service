/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <memory>
#include <mutex>
#include <regex>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_filesystem/b_dir.h"
#include "b_filesystem/b_file.h"
#include "b_json/b_json_entity_caps.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_resources/b_constants.h"
#include "backup_kit_inner.h"
#include "errors.h"
#include "hitrace_meter.h"
#include "service_proxy.h"
#include "tools_op.h"
#include "tools_op_restore_async.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class SessionAsync {
public:
    void TryNotify(bool flag = false)
    {
        if (flag == true) {
            ready_ = true;
            cv_.notify_all();
        } else if (cnt_ == 0) {
            ready_ = true;
            cv_.notify_all();
        }
    }

    void Wait()
    {
        unique_lock<mutex> lk(lock_);
        cv_.wait(lk, [&] { return ready_; });
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

    shared_ptr<BSessionRestoreAsync> session_ = {};

private:
    mutable condition_variable cv_;
    mutex lock_;
    bool ready_ = false;
    uint32_t cnt_ {0};
};

static string GenHelpMsg()
{
    return "\tThis operation helps to restore application data.\n"
           "\t\t--pathCapFile\t\t This parameter should be the path of the capability file.\n"
           "\t\t--bundle\t\t This parameter is bundleName.\n"
           "\t\t--userId\t\t This parameter is userId.\n"
           "\t\t--restoreType\t\t The parameter is a bool variable. true Simulates the upgrade service scenario; false "
           "simulates the application recovery scenario.\n";
}

static void OnFileReady(shared_ptr<SessionAsync> ctx, const BFileInfo &fileInfo, UniqueFd fd)
{
    printf("FileReady owner = %s, fileName = %s, sn = %u, fd = %d\n", fileInfo.owner.c_str(), fileInfo.fileName.c_str(),
           fileInfo.sn, fd.Get());
    if (fileInfo.fileName.find('/') != string::npos) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, "Filename is not valid");
    }
    string tmpPath = string(BConstants::BACKUP_TOOL_RECEIVE_DIR) + fileInfo.owner + "/" + fileInfo.fileName;
    if (access(tmpPath.data(), F_OK) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    BExcepUltils::VerifyPath(tmpPath, false);
    UniqueFd fdLocal(open(tmpPath.data(), O_RDONLY));
    if (fdLocal < 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    BFile::SendFile(fd, fdLocal);
    int ret = ctx->session_->PublishFile(fileInfo);
    if (ret != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, "PublishFile error");
    }
}

static void OnBundleStarted(shared_ptr<SessionAsync> ctx, ErrCode err, const BundleName name)
{
    printf("BundleStarted errCode = %d, BundleName = %s\n", err, name.c_str());
    if (err != 0) {
        ctx->UpdateBundleFinishedCount();
        ctx->TryNotify();
    }
}

static void OnBundleFinished(shared_ptr<SessionAsync> ctx, ErrCode err, const BundleName name)
{
    printf("BundleFinished errCode = %d, BundleName = %s\n", err, name.c_str());
    if (err != 0) {
        ctx->UpdateBundleFinishedCount();
        ctx->TryNotify();
    }
}

static void OnAllBundlesFinished(shared_ptr<SessionAsync> ctx, ErrCode err)
{
    printf("all bundles finished end\n");
    ctx->TryNotify(true);
}

static void OnBackupServiceDied(shared_ptr<SessionAsync> ctx)
{
    printf("backupServiceDied\n");
    ctx->TryNotify(true);
}

static void RestoreApp(shared_ptr<SessionAsync> restore, vector<BundleName> &bundleNames)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "RestoreApp");
    if (!restore || !restore->session_) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
    }
    for (auto &bundleName : bundleNames) {
        if (bundleName.find('/') != string::npos) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, "bundleName is not valid");
        }
        string path = string(BConstants::BACKUP_TOOL_RECEIVE_DIR) + bundleName;
        if (access(path.data(), F_OK) != 0) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, generic_category().message(errno));
        }
        const auto [err, filePaths] = BDir::GetDirFiles(path);
        if (err != 0) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, "error path");
        }
        for (auto &filePath : filePaths) {
            string fileName = filePath.substr(filePath.rfind("/") + 1);
            restore->session_->GetFileHandle(bundleName, fileName);
        }
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
}

static int32_t ChangeBundleInfo(const string &pathCapFile, const vector<string> &bundleNames, const string &type)
{
    BExcepUltils::VerifyPath(pathCapFile, false);
    UniqueFd fd(open(pathCapFile.data(), O_RDWR, S_IRWXU));
    if (fd < 0) {
        fprintf(stderr, "Failed to open file error: %d %s\n", errno, strerror(errno));
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return -errno;
    }
    BJsonCachedEntity<BJsonEntityCaps> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    auto cacheBundleInfos = cache.GetBundleInfos();
    for (auto name : bundleNames) {
        string versionName = string(BConstants::DEFAULT_VERSION_NAME);
        uint32_t versionCode = static_cast<uint32_t>(BConstants::DEFAULT_VERSION_CODE);
        if (type == "false") {
            versionName = string(BConstants::DEFAULT_VERSION_NAME_CLONE);
            versionCode = static_cast<uint32_t>(BConstants::DEFAULT_VERSION_CODE);
        }
        for (auto &&bundleInfo : cacheBundleInfos) {
            if (bundleInfo.name != name) {
                continue;
            }
            bundleInfos.emplace_back(BJsonEntityCaps::BundleInfo {.name = name,
                                                                  .versionCode = versionCode,
                                                                  .versionName = versionName,
                                                                  .spaceOccupied = bundleInfo.spaceOccupied,
                                                                  .allToBackup = bundleInfo.allToBackup,
                                                                  .extensionName = bundleInfo.extensionName});
        }
    }
    cache.SetBundleInfos(bundleInfos);
    cachedEntity.Persist();

    return 0;
}

static int32_t AppendBundles(shared_ptr<SessionAsync> restore,
                             const string &pathCapFile,
                             vector<string> bundleNames,
                             const string &type,
                             const string &userId)
{
    BExcepUltils::VerifyPath(pathCapFile, false);
    UniqueFd fd(open(pathCapFile.data(), O_RDWR, S_IRWXU));
    if (fd < 0) {
        fprintf(stderr, "Failed to open file error: %d %s\n", errno, strerror(errno));
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return -errno;
    }
    RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND;
    if (type == "true") {
        restoreType = RestoreTypeEnum::RESTORE_DATA_READDY;
    }
    try {
        int ret = restore->session_->AppendBundles(move(fd), bundleNames, restoreType, atoi(userId.data()));
        if (ret != 0) {
            printf("restore append bundles error: %d\n", ret);
            return -ret;
        }
        if (type == "false") {
            RestoreApp(restore, bundleNames);
        }
    } catch (const BError &e) {
        printf("restore append bundles error: %d\n", e.GetCode());
        return -1;
    } catch (...) {
        printf("Unexpected exception");
        return -1;
    }
    restore->Wait();
    return 0;
}

static int32_t InitArg(const string &pathCapFile,
                       const vector<string> &bundleNames,
                       const string &type,
                       const string &userId)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Init");
    BExcepUltils::VerifyPath(pathCapFile, false);

    if (ChangeBundleInfo(pathCapFile, bundleNames, type)) {
        fprintf(stderr, "ChangeBundleInfo error");
        return -errno;
    }

    auto ctx = make_shared<SessionAsync>();
    ctx->session_ = BSessionRestoreAsync::Init(BSessionRestoreAsync::Callbacks {
        .onFileReady = bind(OnFileReady, ctx, placeholders::_1, placeholders::_2),
        .onBundleStarted = bind(OnBundleStarted, ctx, placeholders::_1, placeholders::_2),
        .onBundleFinished = bind(OnBundleFinished, ctx, placeholders::_1, placeholders::_2),
        .onAllBundlesFinished = bind(OnAllBundlesFinished, ctx, placeholders::_1),
        .onBackupServiceDied = bind(OnBackupServiceDied, ctx)});
    if (ctx->session_ == nullptr) {
        printf("Failed to init restore\n");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return -EPERM;
    }
    ctx->SetBundleFinishedCount(bundleNames.size());
    return AppendBundles(ctx, pathCapFile, bundleNames, type, userId);
}

static int Exec(map<string, vector<string>> &mapArgToVal)
{
    if (mapArgToVal.find("pathCapFile") == mapArgToVal.end() || mapArgToVal.find("bundles") == mapArgToVal.end() ||
        mapArgToVal.find("restoreType") == mapArgToVal.end() || mapArgToVal.find("userId") == mapArgToVal.end()) {
        return -EPERM;
    }
    return InitArg(*(mapArgToVal["pathCapFile"].begin()), mapArgToVal["bundles"], *(mapArgToVal["restoreType"].begin()),
                   *(mapArgToVal["userId"].begin()));
}

bool RestoreAsyncRegister()
{
    return ToolsOp::Register(ToolsOp {ToolsOp::Descriptor {
        .opName = {"restoreAsync"},
        .argList = {{
                        .paramName = "pathCapFile",
                        .repeatable = false,
                    },
                    {
                        .paramName = "bundles",
                        .repeatable = true,
                    },
                    {
                        .paramName = "restoreType",
                        .repeatable = true,
                    },
                    {
                        .paramName = "userId",
                        .repeatable = true,
                    }},
        .funcGenHelpMsg = GenHelpMsg,
        .funcExec = Exec,
    }});
}
} // namespace OHOS::FileManagement::Backup