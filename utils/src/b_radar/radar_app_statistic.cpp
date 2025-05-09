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

#include "b_radar/radar_app_statistic.h"
#include "filemgmt_libhilog.h"
#include "hisysevent.h"

namespace OHOS::FileManagement::Backup {

std::string FileStatList::ToJsonString()
{
    std::string result = "[";
    ItemInfo* list = GetListPtr();
    uint32_t size = GetListSize();
    for (uint32_t i = 0; i < size; i++) {
        if (i != 0) {
            result += ",";
        }
        result += "{\"count\":" + std::to_string(list[i].count) + ", \"size\":" + std::to_string(list[i].size) + "}";
    }
    result += "]";
    return result;
}

uint8_t FileTypeStat::GetIndexByType(std::string fileExtension)
{
    auto it = FileTypeDef.find(fileExtension);
    if (fileExtension.size() > 0 && it != FileTypeDef.end()) {
        return static_cast<uint8_t>(it->second);
    }
    return static_cast<uint8_t>(FileType::OTHER);
}

void FileTypeStat::UpdateStat(std::string extension, uint64_t size)
{
    uint8_t idx = GetIndexByType(extension);
    typeInfoList_[idx].count++;
    typeInfoList_[idx].size += size;
}

void FileSizeStat::UpdateStat(uint64_t fileSize)
{
    if (fileSize < ONE_MB) {
        sizeInfoList_[TINY].count++;
        sizeInfoList_[TINY].size += fileSize;
    } else if (fileSize < TWO_MB) {
        sizeInfoList_[SMALL].count++;
        sizeInfoList_[SMALL].size += fileSize;
    } else if (fileSize < TEN_MB) {
        sizeInfoList_[MEDIUM].count++;
        sizeInfoList_[MEDIUM].size += fileSize;
    } else if (fileSize < HUNDRED_MB) {
        sizeInfoList_[BIG].count++;
        sizeInfoList_[BIG].size += fileSize;
    } else if (fileSize < ONE_GB) {
        sizeInfoList_[GREAT_BIG].count++;
        sizeInfoList_[GREAT_BIG].size += fileSize;
    } else {
        sizeInfoList_[GIANT].count++;
        sizeInfoList_[GIANT].size += fileSize;
    }
}

void RadarAppStatistic::ReportBackup(const std::string &func, int32_t errorCode)
{
    HiSysEventWrite(
        DOMAIN,
        BACKUP_RESTORE_APP_STATISTIC,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,
        ORG_PKG, DOMAIN_NAME,
        FUNC, func,
        UNIQUE_ID, uniqId_,
        BIZ_SCENE, static_cast<int32_t>(BizScene::BACKUP),
        APP_CALLER, appCaller_,
        FILE_SIZE_DIST, fileSizeDist_.ToJsonString(),
        FILE_TYPE_DIST, fileTypeDist_.ToJsonString(),
        SMALL_FILE_COUNT, smallFileCount_,
        SMALL_FILE_SIZE, smallFileSize_,
        BIG_FILE_COUNT, bigFileCount_,
        BIG_FILE_SIZE, bigFileSize_,
        TAR_FILE_COUNT, tarFileCount_,
        TAR_FILE_SIZE, tarFileSize_,
        TAR_BOUND_SIZE, tarBoundSize_,
        DIR_DEPTH, dirDepth_,
        MANAGE_JSON_SIZE, manageJsonSize_,
        EXTENSION_CONNECT_SPEND, extConnectSpend_,
        ON_BACKUP_SPEND, onBackupSpend_.GetSpan(),
        ON_BACKUPEX_SPEND, onBackupexSpend_.GetSpan(),
        TAR_SPEND, tarSpend_,
        HASH_SPEND, hashSpendUS_ / MS_TO_US,
        SCAN_FILE_SPEND, scanFileSpend_.GetSpan(),
        SEND_RATE_ZERO_SPAN, sendRateZeroSpendUS_ / MS_TO_US,
        DO_BACKUP_SPEND, doBackupSpend_.GetSpan(),
        ERROR_CODE, errorCode);
}

void RadarAppStatistic::ReportBackup(const std::string &func, BError errCode)
{
    ReportBackup(func, RadarErrorCode(MODULE_BACKUP, errCode).GenCode());
}

void RadarAppStatistic::ReportRestore(const std::string &func, int32_t errorCode)
{
    HiSysEventWrite(
        DOMAIN,
        BACKUP_RESTORE_APP_STATISTIC,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,
        ORG_PKG, DOMAIN_NAME,
        FUNC, func,
        UNIQUE_ID, uniqId_,
        BIZ_SCENE, static_cast<int32_t>(BizScene::RESTORE),
        APP_CALLER, appCaller_,
        BIG_FILE_COUNT, bigFileCount_,
        BIG_FILE_SIZE, bigFileSize_,
        TAR_FILE_COUNT, tarFileCount_,
        TAR_FILE_SIZE, tarFileSize_,
        TAR_BOUND_SIZE, tarBoundSize_,
        EXTENSION_CONNECT_SPEND, extConnectSpend_,
        ON_RESTORE_SPEND, onRestoreSpend_.GetSpan(),
        ON_RESTOREEX_SPEND, onRestoreexSpend_.GetSpan(),
        UNTAR_SPEND, untarSpend_,
        BIG_FILE_SPEND, bigFileSpend_,
        DO_RESTORE_SPEND, doRestoreSpend_,
        ERROR_CODE, errorCode);
}

void RadarAppStatistic::ReportRestore(const std::string &func, BError errCode)
{
    ReportRestore(func, RadarErrorCode(MODULE_RESTORE, errCode).GenCode());
}

void RadarAppStatistic::UpdateSendRateZeroSpend()
{
    sendRateZeroSpendUS_ += TimeUtils::GetSpendUS(sendRateZeroStart_);
    sendRateZeroStart_ = 0;
}

void RadarAppStatistic::UpdateFileDist(std::string fileExtension, uint64_t fileSize)
{
    fileSizeDist_.UpdateStat(fileSize);
    fileTypeDist_.UpdateStat(fileExtension, fileSize);
}

} // namespace OHOS::FileManagement::Backup