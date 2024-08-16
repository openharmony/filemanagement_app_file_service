/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
const uint32_t MAX_FD_GROUP_USE_TIME = 1000;          // 每组打开最大时间1000ms

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

void BackupExtExtension::SetClearDataFlag(bool isClearData)
{
    isClearData_ = isClearData;
    HILOGI("set clear data flag:%{public}d", isClearData);
    if (extension_ == nullptr) {
        HILOGE("Extension handle have been released");
        return;
    }
    extension_->SetClearDataFlag(isClearData);
    if (!extension_->WasFromSpecialVersion() && !extension_->RestoreDataReady()) {
        DoClear();
    }
}

std::map<std::string, off_t> BackupExtExtension::GetIdxFileInfos(bool isSpecialVersion)
{
    string restoreDir = isSpecialVersion ? "" :
        string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    auto extManageInfo = GetExtManageInfo();
    std::map<std::string, off_t> idxFileInfos;
    for (size_t i = 0; i < extManageInfo.size(); ++i) {
        std::string realPath = restoreDir + extManageInfo[i].hashName;
        idxFileInfos[realPath] = extManageInfo[i].sta.st_size;
    }
    return idxFileInfos;
}

void BackupExtExtension::CheckTmpDirFileInfos(bool isSpecialVersion)
{
    ErrFileInfo errFiles;
    auto idxFileInfos = GetIdxFileInfos(isSpecialVersion);
    struct stat attr;
    for (auto it : idxFileInfos) {
        if (it.first.size() >= PATH_MAX || stat(it.first.data(), &attr) == -1) {
            HILOGE("(Debug) Failed to get stat of %{public}s, errno = %{public}d",
                GetAnonyPath(it.first).c_str(), errno);
            errFiles[it.first].push_back(errno);
        } else if (it.second != attr.st_size) {
            HILOGE("(Debug) RecFile:%{public}s size err, recSize: %{public}" PRId64 ", idxSize: %{public}" PRId64 "",
                GetAnonyPath(it.first).c_str(), attr.st_size, it.second);
            errFiles[it.first] = std::vector<int>();
        }
    }
    HILOGE("(Debug) Temp file check result: Total file: %{public}zu, err file: %{public}zu",
        idxFileInfos.size(), errFiles.size());
    if (!errFiles.empty()) {
        HILOGE("(Debug) The received file and idx is not same");
    } else {
        HILOGI("(Debug) The received file and idx is same");
    }
}

tuple<bool, vector<string>> BackupExtExtension::CheckRestoreFileInfos()
{
    vector<string> errFiles;
    struct stat curFileStat {};
    for (const auto& it : endFileInfos_) {
        if (lstat(it.first.c_str(), &curFileStat) != 0) {
            HILOGE("(Debug) Failed to lstat, err = %{public}d", errno);
            errFiles.emplace_back(it.first);
            errFileInfos_[it.first].push_back(errno);
        } else if (curFileStat.st_size != it.second) {
            HILOGE("(Debug) File size check error, file: %{public}s", GetAnonyPath(it.first).c_str());
            errFiles.emplace_back(it.first);
            errFileInfos_[it.first].push_back(errno);
        }
    }
    for (const auto& it : errFileInfos_) {
        for (const auto &codeIt : it.second) {
            HILOGE("(Debug)  errfileInfos file = %{public}s -> %{public}d", GetAnonyPath(it.first).c_str(), codeIt);
        }
    }
    HILOGE("(Debug) End file check result Total file: %{public}zu, err file: %{public}zu",
        endFileInfos_.size(), errFileInfos_.size());
    if (errFiles.size()) {
        return {false, errFiles};
    }
    return {true, errFiles};
}
} // namespace OHOS::FileManagement::Backup
