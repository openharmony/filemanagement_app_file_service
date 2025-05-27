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
#include <filemgmt_libhilog.h>

namespace OHOS::FileManagement::Backup {

ScanFileSingleton& ScanFileSingleton::GetInstance()
{
    static ScanFileSingleton instance;
    return instance;
}

ScanFileSingleton::~ScanFileSingleton()
{
    isCalculateCompleted_ = false;
}

void ScanFileSingleton::AddBigFile(const std::string& key, const struct stat& value)
{
    std::lock_guard<std::mutex> lock(mutexLock_);
    bigFileQueue_.push({key, value});
    waitForFilesAddCv_.notify_all();
}

void ScanFileSingleton::AddSmallFile(const std::string& key, size_t value)
{
    std::lock_guard<std::mutex> lock(mutexLock_);
    smallFiles_[key] = value;
}

std::map<std::string, struct stat> ScanFileSingleton::GetAllBigFiles()
{
    std::lock_guard<std::mutex> lock(mutexLock_);
    std::map<std::string, struct stat> fileMap;
    while (!bigFileQueue_.empty()) {
        fileMap[bigFileQueue_.front().first] = bigFileQueue_.front().second;
        bigFileQueue_.pop();
    }
    return fileMap;
}

bool ScanFileSingleton::GetCompeletedFlag()
{
    std::lock_guard<std::mutex> lock(mutexLock_);
    return isCalculateCompleted_;
}

void ScanFileSingleton::SetCompeletedFlag(bool value)
{
    std::lock_guard<std::mutex> lock(mutexLock_);
    isCalculateCompleted_ = value;
    if (value) {
        waitForFilesAddCv_.notify_all();
    }
}

std::map<std::string, size_t> ScanFileSingleton::GetAllSmallFiles()
{
    std::lock_guard<std::mutex> lock(mutexLock_);
    return smallFiles_;
}

void ScanFileSingleton::SetIncludeSize(uint32_t includeSize)
{
    std::lock_guard<std::mutex> lock(mutexLock_);
    includeSize_ = includeSize;
}

void ScanFileSingleton::SetExcludeSize(uint32_t excludeSize)
{
    std::lock_guard<std::mutex> lock(mutexLock_);
    excludeSize_ = excludeSize;
}

uint32_t ScanFileSingleton::GetIncludeSize()
{
    std::lock_guard<std::mutex> lock(mutexLock_);
    return includeSize_;
}

uint32_t ScanFileSingleton::GetExcludeSize()
{
    std::lock_guard<std::mutex> lock(mutexLock_);
    return excludeSize_;
}

void ScanFileSingleton::WaitForFiles()
{
    HILOGI("calculate is uncompleted, need to wait");
    std::unique_lock<std::mutex> lock(mutexLock_);
    waitForFilesAddCv_.wait(lock, [this] {return !bigFileQueue_.empty() || isCalculateCompleted_; });
}

} // namespace OHOS::FileManagement::Backup