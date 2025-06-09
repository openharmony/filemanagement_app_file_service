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

#ifndef OHOS_FILEMGMT_BACKUP_RADAR_TOTAL_STATISTIC_H
#define OHOS_FILEMGMT_BACKUP_RADAR_TOTAL_STATISTIC_H

#include <atomic>
#include <mutex>
#include "radar_const.h"
#include "b_error/b_error.h"

namespace OHOS::FileManagement::Backup {

class RadarTotalStatistic {
public:
    std::atomic<uint32_t> succBundleCount_ = 0;
    std::atomic<uint32_t> failBundleCount_ = 0;
    Duration getBundleInfoSpend_ = {0, 0};
    Duration totalSpendTime_ = {0, 0};

    RadarTotalStatistic(BizScene bizScene, std::string caller, Mode mode = Mode::FULL);
    RadarTotalStatistic(const RadarTotalStatistic &) = delete;
    RadarTotalStatistic &operator=(const RadarTotalStatistic &) = delete;
    RadarTotalStatistic(RadarTotalStatistic &&) = delete;
    RadarTotalStatistic &operator=(RadarTotalStatistic &&) = delete;
    ~RadarTotalStatistic() = default;

    void Report(const std::string &func, int32_t error, std::string errMsg = "");
    void Report(const std::string &func, uint32_t moduleId, uint32_t moduleErr);
    void Report(const std::string &func, BError errCode, uint32_t moduleId = MODULE_UNKNOWN);
    BizScene GetBizScene() { return bizScene_; }
    int64_t GetUniqId() { return uniqId_; }

private:
    BizScene bizScene_ = BizScene::UNKNOWN;
    std::string hostPkg_ = "";
    Mode mode_ = Mode::FULL;
    int64_t uniqId_ = 0;
    std::mutex lastCntMutex_;
    uint32_t lastSuccCnt_ = 0;
    uint32_t lastFailCnt_ = 0;
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_RADAR_TOTAL_STATISTIC_H
