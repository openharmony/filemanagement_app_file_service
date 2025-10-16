/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#include "b_utils/scan_file_singleton.h"
#include "b_resources/b_constants.h"
#include "b_utils/string_utils.h"
#include <filemgmt_libhilog.h>

namespace OHOS::FileManagement::Backup {

std::string FileInfo::GetRestorePath()
{
    return "";
}

std::string CompatibleFileInfo::GetRestorePath()
{
    return restorePath_;
}

std::string SmallFileInfo::GetRestorePath()
{
    return "";
}

std::string CompatibleSmallFileInfo::GetRestorePath()
{
    return restorePath_;
}

ScanFileSingleton& ScanFileSingleton::GetInstance()
{
    static ScanFileSingleton instance;
    return instance;
}

void ScanFileSingleton::AddBigFile(const std::string& filePath, const struct stat& sta, const std::string& restorePath)
{
    std::lock_guard<std::mutex> lock(pendingFileMutex_);
    std::string hashName = StringUtils::GenHashName(filePath);
    for (size_t i = 0; hashNameSet_.find(hashName) != hashNameSet_.end(); i++) {
        hashName = StringUtils::GenHashName(filePath + std::to_string(i));
    }
    hashNameSet_.emplace(hashName);
    if (restorePath.empty()) {
        pendingFileQueue_.push(std::make_shared<FileInfo>(hashName, filePath, sta, true));
    } else {
        pendingFileQueue_.push(std::make_shared<CompatibleFileInfo>(hashName, filePath, sta, true, restorePath));
    }
    waitFilesReady_.notify_all();
}

void ScanFileSingleton::AddTarFile(const std::string& filename, const std::string& filePath, const struct stat& sta)
{
    std::lock_guard<std::mutex> lock(pendingFileMutex_);
    pendingFileQueue_.push(std::make_shared<FileInfo>(filename, filePath, sta, false));
    waitFilesReady_.notify_all();
}

std::shared_ptr<IFileInfo> ScanFileSingleton::GetFileInfo()
{
    std::lock_guard<std::mutex> lock(pendingFileMutex_);
    if (!pendingFileQueue_.empty()) {
        std::shared_ptr<IFileInfo> fileInfo = pendingFileQueue_.front();
        pendingFileQueue_.pop();
        return fileInfo;
    }
    return nullptr;
}

bool ScanFileSingleton::HasFileReady()
{
    std::lock_guard<std::mutex> lock(pendingFileMutex_);
    return !pendingFileQueue_.empty();
}

void ScanFileSingleton::AddSmallFile(const std::string& filePath, size_t fileSize, const std::string& restorePath)
{
    std::lock_guard<std::mutex> lock(smallFileMutex_);
    if (restorePath.empty()) {
        smallFiles_.push_back(std::make_shared<SmallFileInfo>(filePath, fileSize));
    } else {
        smallFiles_.push_back(std::make_shared<CompatibleSmallFileInfo>(filePath, fileSize, restorePath));
    }
}

std::vector<std::shared_ptr<ISmallFileInfo>> ScanFileSingleton::GetAllSmallFiles()
{
    std::lock_guard<std::mutex> lock(smallFileMutex_);
    if (!smallFiles_.empty()) {
        auto allSmallInfo = move(smallFiles_);
        smallFiles_ = {};
        return allSmallInfo;
    }
    return {};
}

bool ScanFileSingleton::IsProcessCompleted()
{
    return isProcessCompleted_.load();
}

void ScanFileSingleton::SetCompletedFlag(bool isCompleted)
{
    isProcessCompleted_.store(isCompleted);
    if (isCompleted) {
        waitFilesReady_.notify_all();
    }
}

void ScanFileSingleton::WaitForFiles()
{
    HILOGI("calculate is uncompleted, need to wait");
    std::unique_lock<std::mutex> lock(mutexLock_);
    waitFilesReady_.wait(lock, [this] {return HasFileReady() || IsProcessCompleted(); });
}

} // namespace OHOS::FileManagement::Backup