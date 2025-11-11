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

#ifndef OHOS_FILEMGMT_BACKUP_SCAN_FILE_SINGLETON_H
#define OHOS_FILEMGMT_BACKUP_SCAN_FILE_SINGLETON_H

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <string>
#include <sys/stat.h>
#include <unordered_set>
#include <vector>

#include "b_radar/radar_app_statistic.h"
 
namespace OHOS::FileManagement::Backup {
class IFileInfo {
public:
    virtual ~IFileInfo() {};
    virtual std::string GetRestorePath() = 0;
    IFileInfo(const std::string& filename, const std::string& filePath, const struct stat& sta, bool isBigFile)
        : filename_(filename), filePath_(filePath), sta_(sta), isBigFile_(isBigFile) {};
    std::string filename_ = "";
    std::string filePath_ = "";
    struct stat sta_;
    bool isBigFile_ = false;
};

struct FileInfo : public IFileInfo {
    FileInfo(const std::string& filename, const std::string& filePath, const struct stat& sta, bool isBigFile)
        : IFileInfo(filename, filePath, sta, isBigFile) {};
    std::string GetRestorePath() override;
};

struct CompatibleFileInfo : public IFileInfo {
    CompatibleFileInfo(const std::string& filename, const std::string& filePath, const struct stat& sta, bool isBigFile,
        const std::string& restorePath) : IFileInfo(filename, filePath, sta, isBigFile), restorePath_(restorePath) {};
    std::string GetRestorePath() override;
    std::string restorePath_ = "";
};

class ISmallFileInfo {
public:
    virtual ~ISmallFileInfo() {};
    ISmallFileInfo(const std::string& filePath, size_t fileSize) : filePath_(filePath), fileSize_(fileSize) {};
    virtual std::string GetRestorePath() = 0;
    std::string filePath_ = "";
    size_t fileSize_ = 0;
};

struct SmallFileInfo : public ISmallFileInfo {
    std::string GetRestorePath() override;
    SmallFileInfo(const std::string& filePath, size_t fileSize) : ISmallFileInfo(filePath, fileSize) {};
};

struct CompatibleSmallFileInfo : public ISmallFileInfo {
    std::string GetRestorePath() override;
    CompatibleSmallFileInfo(const std::string& filePath, size_t fileSize, const std::string& restorePath)
        : ISmallFileInfo(filePath, fileSize), restorePath_(restorePath) {};
    std::string restorePath_ = "";
};

class ScanFileSingleton {
public:
    static ScanFileSingleton &GetInstance();
     
    void AddBigFile(const std::string& filePath, const struct stat& sta, const std::string& restorePath = "");
    void AddTarFile(const std::string& filename, const std::string& filePath, const struct stat& sta);
    std::shared_ptr<IFileInfo> GetFileInfo();
    bool HasFileReady();

    void AddSmallFile(const std::string& filePath, size_t fileSize, const std::string& restorePath = "");
    std::vector<std::shared_ptr<ISmallFileInfo>> GetAllSmallFiles();

    bool IsProcessCompleted();

    void SetCompletedFlag(bool value);

    // 条件等待，等待文件被添加或者扫描完成
    void WaitForFiles();
    void UpdateSmallFileSizeLimit(uint64_t allSmallFileSize);

    void StartPacket();
    void WaitForPacketFlag();
private:
    // 私有构造函数，防止外部实例化
    ScanFileSingleton() {}
    ~ScanFileSingleton() {};

    std::mutex pendingFileMutex_;
    std::queue<std::shared_ptr<IFileInfo>> pendingFileQueue_;
    std::unordered_set<std::string> hashNameSet_;
    std::mutex smallFileMutex_;
    std::vector<std::shared_ptr<ISmallFileInfo>> smallFiles_;
    std::mutex mutexLock_;
    std::condition_variable waitFilesReady_;
    std::atomic<bool> isProcessCompleted_ = false;
    std::atomic<uint64_t> currentTarSize_ = 0;
    std::atomic<uint64_t> smallFileSizeLimit_ = 0;
    std::atomic<bool> stopPacket_ = false;
    std::mutex mutexPacket_;
    std::condition_variable waitPacketFlag_;
};
} // namespace OHOS::FileManagement::ScanFileSingleton
#endif // OHOS_FILEMGMT_BACKUP_SCAN_FILE_SINGLETON_H