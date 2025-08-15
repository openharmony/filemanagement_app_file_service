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

#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <sys/stat.h>
 
namespace OHOS::FileManagement::Backup {
class ScanFileSingleton {
public:
    static ScanFileSingleton &GetInstance();
     
    void AddBigFile(const std::string& key, const struct stat& value);
     
    void AddSmallFile(const std::string& key, size_t value);
     
    std::map<std::string, struct stat> GetAllBigFiles();

    bool GetCompletedFlag();

    void SetCompletedFlag(bool value);

    std::map<std::string, size_t> GetAllSmallFiles();

    void SetIncludeSize(uint32_t includeSize);

    void SetExcludeSize(uint32_t excludeSize);

    uint32_t GetIncludeSize();

    uint32_t GetExcludeSize();

    // 条件等待，等待文件被添加或者扫描完成
    void WaitForFiles();
private:
    // 私有构造函数，防止外部实例化
    ScanFileSingleton() {}
    ~ScanFileSingleton();

    std::queue<std::pair<std::string, struct stat>> bigFileQueue_;
    std::map<std::string, size_t> smallFiles_;
    std::mutex mutexLock_;
    std::condition_variable waitForFilesAddCv_;
    bool isCalculateCompleted_ = false;
    uint32_t includeSize_ = 0;
    uint32_t excludeSize_ = 0;
};
} // namespace OHOS::FileManagement::ScanFileSingleton
#endif // OHOS_FILEMGMT_BACKUP_SCAN_FILE_SINGLETON_H