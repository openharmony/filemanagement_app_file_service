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
#include "b_error/b_error.h"
#include "b_utils/storage_manager_helper.h"
#include <filemgmt_libhilog.h>

namespace OHOS::FileManagement::Backup {

constexpr uint32_t MEGA_BYTE = 1048576; // 1M包含多少字节
constexpr uint64_t FIVE_GB = 5ULL * 1024 * MEGA_BYTE; // 5GB
constexpr uint64_t FOUR_GB = 4ULL * 1024 * MEGA_BYTE; // 4GB
constexpr uint64_t ONE_HUNDRED_FIFTY_MB = 150ULL * MEGA_BYTE; // 150MB

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

std::string AncoFileInfo::GetRestorePath()
{
    return "";
}

std::string AncoCompatibleFileInfo::GetRestorePath()
{
    return restorePath_;
}

std::string CompatibleSmallFileInfo::GetRestorePath()
{
    return restorePath_;
}

ScanFileSingleton& ScanFileSingleton::GetInstance()
{
    static ScanFileSingleton instance;
    static std::once_flag initFlag;
    std::call_once(initFlag, [&]() {
        uint64_t freeSize = StorageManagerHelper::GetInstance().GetFreeSize();
        if (freeSize == 0) {
            HILOGE("get freeSize fail!");
        }
        if (freeSize < FIVE_GB) {
            instance.maxTarSize_.store(ONE_HUNDRED_FIFTY_MB);
        } else {
            instance.maxTarSize_.store(freeSize - FOUR_GB);
        }
    });
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
    if (sta.st_size < 0) {
        HILOGE("st_size is negative, fileName:%{public}s!", filename.c_str());
        return;
    }
    pendingFileQueue_.push(std::make_shared<FileInfo>(filename, filePath, sta, false));
    currentTarSize_.fetch_add(sta.st_size);
    if (currentTarSize_.load() > maxTarSize_.load()) {
        HILOGW("meet max tar size, stop scan. tarSize=%{public}uM",
            static_cast<uint32_t>(currentTarSize_.load() / MEGA_BYTE));
        stopPacket_.store(true);
    }
    waitFilesReady_.notify_all();
}

void ScanFileSingleton::AddAncoBigFile(
    const std::string &filePath, const std::string &restorePath, const struct stat &sta)
{
    std::lock_guard<std::mutex> lock(pendingFileMutex_);
    std::string hashName = StringUtils::GenHashName(filePath);
    for (size_t i = 0; hashNameSet_.find(hashName) != hashNameSet_.end(); i++) {
        hashName = StringUtils::GenHashName(filePath + std::to_string(i));
    }
    hashNameSet_.emplace(hashName);
    hashName += BConstants::ANCO_TAG;
    if (restorePath.empty()) {
        pendingFileQueue_.push(std::make_shared<AncoFileInfo>(hashName, filePath, sta, true));
    } else {
        pendingFileQueue_.push(
            std::make_shared<AncoCompatibleFileInfo>(hashName, filePath, sta, true, restorePath));
    }
    waitFilesReady_.notify_all();
}

void ScanFileSingleton::AddAncoTarFile(const std::string &filename, const std::string &filePath, const struct stat &sta)
{
    std::lock_guard<std::mutex> lock(pendingFileMutex_);
    pendingFileQueue_.push(std::make_shared<AncoFileInfo>(filename, filePath, sta, false));
    currentTarSize_.fetch_add(sta.st_size);
    if (currentTarSize_.load() > maxTarSize_.load()) {
        HILOGW("meet max tar size, stop scan. tarSize=%{public}uM",
            static_cast<uint32_t>(currentTarSize_.load() / MEGA_BYTE));
        stopPacket_.store(true);
    }
    waitFilesReady_.notify_all();
}

std::shared_ptr<IFileInfo> ScanFileSingleton::GetFileInfo()
{
    std::lock_guard<std::mutex> lock(pendingFileMutex_);
    if (!pendingFileQueue_.empty()) {
        std::shared_ptr<IFileInfo> fileInfo = pendingFileQueue_.front();
        pendingFileQueue_.pop();
        if (fileInfo != nullptr && !fileInfo->isBigFile_) {
            currentTarSize_.fetch_sub(fileInfo->sta_.st_size);
            if (currentTarSize_.load() < maxTarSize_.load()) {
                StartPacket();
            }
        }
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
    std::unique_lock<std::mutex> lock(mutexLock_);
    isProcessCompleted_.store(isCompleted);
    if (isCompleted) {
        waitFilesReady_.notify_all();
    }
}

void ScanFileSingleton::WaitForFiles()
{
    std::unique_lock<std::mutex> lock(mutexLock_);
    waitFilesReady_.wait(lock, [this] {return HasFileReady() || IsProcessCompleted(); });
}

void ScanFileSingleton::StartPacket()
{
    stopPacket_.store(false);
    waitPacketFlag_.notify_all();
}

void ScanFileSingleton::WaitForPacketFlag()
{
    std::unique_lock<std::mutex> lock(mutexPacket_);
    waitPacketFlag_.wait(lock, [this] {return !HasFileReady() || !stopPacket_.load(); });
}

} // namespace OHOS::FileManagement::Backup