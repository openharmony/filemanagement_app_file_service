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

#ifndef OHOS_FILEMGMT_BACKUP_RADAR_RUNNINGLOCK_STATISTIC_H
#define OHOS_FILEMGMT_BACKUP_RADAR_RUNNINGLOCK_STATISTIC_H

#include "b_resources/b_constants.h"
#include "radar_const.h"

namespace OHOS::FileManagement::Backup {

class RadarRunningLockStatistic {
public:
    RadarRunningLockStatistic() {}
    RadarRunningLockStatistic(ErrCode radarCode) :radarCode_(radarCode) {}
    ~RadarRunningLockStatistic() = default;
    RadarRunningLockStatistic(const RadarRunningLockStatistic &) = delete;
    RadarRunningLockStatistic &operator=(const RadarRunningLockStatistic &) = delete;
    RadarRunningLockStatistic(RadarRunningLockStatistic &&) = delete;
    RadarRunningLockStatistic &operator=(RadarRunningLockStatistic &&) = delete;
    
    void ReportBackupRunningLock(const std::string &func, const std::string &errMsg, ErrCode errCode);
    void ReportRestoreRunningLock(const std::string &func, const std::string &errMsg, ErrCode errCode);

    ErrCode radarCode_ = ERROR_OK;
};
} // namespace OHOS::FileManagement::Backup
#endif