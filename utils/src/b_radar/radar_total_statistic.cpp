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

#include "b_radar/radar_total_statistic.h"
#include "b_utils/b_time.h"
#include "filemgmt_libhilog.h"
#include "hisysevent.h"

namespace OHOS::FileManagement::Backup {

RadarTotalStatistic::RadarTotalStatistic(BizScene bizScene, std::string callerName, Mode mode)
    : bizScene_(bizScene), hostPkg_(callerName), mode_(mode)
{
    uniqId_ = TimeUtils::GetTimeUS();
}

void RadarTotalStatistic::Report(const std::string &func, int32_t error, std::string errMsg)
{
    std::lock_guard<std::mutex> lastCntLock(lastCntMutex_);
    uint32_t succCount = succBundleCount_.load();
    uint32_t failCount = failBundleCount_.load();
    HiSysEventWrite(
        DOMAIN,
        BACKUP_RESTORE_STATISTIC,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        GET_BUNDLE_INFO_SPEND, getBundleInfoSpend_.GetSpan(),
        TOTAL_SPEND, totalSpendTime_.GetSpan(),
        SUCC_BUNDLE_CNT, succCount - lastSuccCnt_,
        FAIL_BUNDLE_CNT, failCount - lastFailCnt_,
        ORG_PKG, DOMAIN_NAME,
        FUNC, func,
        CONCURRENT_ID, uniqId_,
        BIZ_SCENE, static_cast<int32_t>(bizScene_),
        HOST_PKG, hostPkg_,
        MODE, static_cast<uint32_t>(mode_),
        ERROR_MSG, errMsg,
        ERROR_CODE, error,
        BIZ_STAGE, DEFAULT_STAGE,
        STAGE_RES, error == 0 ? STAGE_RES_SUCCESS : STAGE_RES_FAIL);
        lastSuccCnt_ = succCount;
        lastFailCnt_ = failCount;
}

void RadarTotalStatistic::Report(const std::string &func, uint32_t moduleId, uint32_t moduleErr)
{
    RadarError err(moduleId, moduleErr);
    Report(func, err.GenCode());
}

void RadarTotalStatistic::Report(const std::string &func, BError errCode, uint32_t moduleId)
{
    RadarError err(moduleId, errCode);
    Report(func, err.GenCode(), err.errMsg_);
}
} // namespace OHOS::FileManagement::Backup