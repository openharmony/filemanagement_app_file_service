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

#include "b_radar/radar_runninglock_statistic.h"
#include "b_radar/radar_const_inner.h"
#include "filemgmt_libhilog.h"
#include "hisysevent.h"

namespace OHOS::FileManagement::Backup {

void ReportBackupRunningLock(const std::string &func, const std::string &errMsg, ErrCode errCode)
{
    radarCode_ = errCode;
    if (radarCode_ == ERROR_OK) {
        return;
    }
    RadarError err(MODULE_BACKUP, radarCode_);
    radarCode_ = err.GenCode();
    HiSysEventWrite(
        DOMAIN,
        BACKUP_RESTORE_STATISTIC,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        BIZ_SCENE, "BIZ_SCENE_BACKUP",
        ORG_PKG, DOMAIN_NAME,
        FUNC, func,
        ERROR_MSG, errMsg,
        ERROR_CODE, radarCode_,
        BIZ_STAGE, "BIZ_STAGE_BACKUP",
        STAGE_RES, radarCode_ == 0 ? STAGE_RES_SUCCESS : STAGE_RES_FAIL);
    );
}

void ReportRestoreRunningLock(const std::string &func, const std::string &errMsg, ErrCode errCode)
{
    radarCode_ = errCode;
    if (radarCode_ == ERROR_OK) {
        return;
    }
    RadarError err(MODULE_BACKUP, radarCode_);
    radarCode_ = err.GenCode();
    HiSysEventWrite(
        DOMAIN,
        BACKUP_RESTORE_STATISTIC,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        BIZ_SCENE, "BIZ_SCENE_RESTORE",
        ORG_PKG, DOMAIN_NAME,
        FUNC, func,
        ERROR_MSG, errMsg,
        ERROR_CODE, radarCode_,
        BIZ_STAGE, "BIZ_STAGE_RESTORE",
        STAGE_RES, radarCode_ == 0 ? STAGE_RES_SUCCESS : STAGE_RES_FAIL);
    );
}

}