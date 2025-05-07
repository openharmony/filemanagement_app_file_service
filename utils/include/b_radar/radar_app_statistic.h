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

#ifndef OHOS_FILEMGMT_BACKUP_RADAR_APP_STATISTIC_H
#define OHOS_FILEMGMT_BACKUP_RADAR_APP_STATISTIC_H

#include <unordered_map>
#include "b_resources/b_constants.h"
#include "radar_const.h"

namespace OHOS::FileManagement::Backup {

struct ItemInfo {
    uint32_t count = 0;
    uint64_t size = 0;
};

constexpr uint8_t TYPE_DEF_COUNT = 7;
constexpr uint8_t CATEGORY_TYPE_COUNT = 10;

constexpr uint8_t SIZE_DEF_COUNT = 6;

enum FileType : uint8_t {
    TXT,
    PIC,
    AUDIO,
    VEDIO,
    COMPRESS,
    BIN,
    OTHER,
};

const std::unordered_map<std::string, FileType> FileTypeDef = {
    {"txt", TXT}, {"log", TXT}, {"json", TXT}, {"xml", TXT},
    {"jpg", PIC}, {"jpeg", PIC}, {"png", PIC}, {"bmp", PIC}, {"gif", PIC}, {"svg", PIC}, {"webp", PIC}, {"tif", PIC},
    {"raw", PIC},
    {"wav", AUDIO}, {"flac", AUDIO}, {"wma", AUDIO}, {"acc", AUDIO}, {"mp3", AUDIO}, {"ogg", AUDIO}, {"opus", AUDIO},
    {"mov", VEDIO}, {"wmv", VEDIO}, {"rm", VEDIO}, {"rmvb", VEDIO}, {"3gp", VEDIO}, {"m4v", VEDIO}, {"mkv", VEDIO},
    {"rar", COMPRESS}, {"zip", COMPRESS}, {"7z", COMPRESS}, {"gz", COMPRESS}, {"iso", COMPRESS}, {"tar", COMPRESS},
    {"exe", BIN}, {"doc", BIN}, {"docx", BIN}, {"xls", BIN}, {"xlsx", BIN}, {"ppt", BIN}, {"pdf", BIN}
};

constexpr uint64_t ONE_MB = 1024 * 1024;
constexpr uint64_t TWO_MB = 2 * ONE_MB;
constexpr uint64_t TEN_MB = 10 * ONE_MB;
constexpr uint64_t HUNDRED_MB = 100 * ONE_MB;
constexpr uint64_t ONE_GB = 1024 * ONE_MB;

enum FileSize : uint8_t {
    TINY, // [0, 1M)
    SMALL, // [1M, 2M)
    MEDIUM, // [2M, 10M)
    BIG, // [10M, 100M)
    GREAT_BIG, // [100M, 1G)
    GIANT, // >=1G
};

class FileStatList {
public:
    std::string ToJsonString();
    virtual ItemInfo* GetListPtr() = 0;
    virtual uint32_t GetListSize() = 0;
};

class FileTypeStat : public FileStatList {
public:
    uint8_t GetIndexByType(std::string fileExtension);
    void UpdateStat(std::string extension, uint64_t size);
    ItemInfo* GetListPtr() override
    {
        return typeInfoList_;
    }
    uint32_t GetListSize() override
    {
        return TYPE_DEF_COUNT;
    }
private:
    ItemInfo typeInfoList_[TYPE_DEF_COUNT] = {{0, 0}};
};

class FileSizeStat : public FileStatList {
public:
    void UpdateStat(uint64_t fileSize);
    ItemInfo* GetListPtr() override
    {
        return sizeInfoList_;
    }
    uint32_t GetListSize() override
    {
        return SIZE_DEF_COUNT;
    }
private:
    ItemInfo sizeInfoList_[SIZE_DEF_COUNT] = {{0, 0}};
};

class RadarAppStatistic {
public:
    std::string appCaller_; // tool app
    uint32_t smallFileCount_ = 0;
    uint64_t smallFileSize_ = 0;
    uint32_t bigFileCount_ = 0;
    uint64_t bigFileSize_ = 0;
    uint32_t tarFileCount_ = 0;
    uint64_t tarFileSize_ = 0;
    uint32_t dirDepth_ = 0;
    uint64_t tarBoundSize_ = BConstants::BIG_FILE_BOUNDARY;
    uint64_t manageJsonSize_ = 0;
    uint32_t extConnectSpend_ = 0;

    Duration onBackupSpend_ = {0, 0};
    Duration onBackupexSpend_ = {0, 0};
    Duration scanFileSpend_ = {0, 0};
    int64_t sendRateZeroStart_ = 0;
    uint32_t sendRateZeroSpendUS_ = 0;
    uint32_t tarSpend_ = 0;
    uint32_t hashSpendUS_ = 0;
    Duration doBackupSpend_ = {0, 0};

    Duration onRestoreSpend_ = {0, 0};
    Duration onRestoreexSpend_ = {0, 0};
    uint32_t untarSpend_ = 0;
    uint32_t bigFileSpend_ = 0;
    uint32_t doRestoreSpend_ = 0;

    RadarAppStatistic() {};
    ~RadarAppStatistic() = default;
    RadarAppStatistic(const RadarAppStatistic &) = delete;
    RadarAppStatistic &operator=(const RadarAppStatistic &) = delete;
    RadarAppStatistic(RadarAppStatistic &&) = delete;
    RadarAppStatistic &operator=(RadarAppStatistic &&) = delete;

    void SetUniqId(int64_t uniqId) { uniqId_ = uniqId; };
    void UpdateSendRateZeroSpend();
    void UpdateFileDist(std::string fileExtension, uint64_t fileSize);
    void ReportBackup(const std::string &func, int32_t errorCode = ERROR_OK);
    void ReportBackup(const std::string &func, BError errCode);
    void ReportRestore(const std::string &func, int32_t errorCode = ERROR_OK);
    void ReportRestore(const std::string &func, BError errCode);

private:
    FileSizeStat fileSizeDist_;
    FileTypeStat fileTypeDist_;
    int64_t uniqId_ = 0;
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_RADAR_APP_STATISTIC_H
