/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "b_utils/b_time.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <unistd.h>

#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "parameters.h"

namespace OHOS::FileManagement::Backup {
namespace {
    constexpr int32_t MAX_RATIO = 50;
    constexpr const char* TIMEOUT_UNIT_TIME_RATIO = "persist.sys.abilityms.timeout_unit_time_ratio";
    constexpr int32_t AFS_RESERVE_SECOND = 5;
}

int64_t TimeUtils::GetTimeS()
{
    std::chrono::seconds nowS = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch());
        return nowS.count();
}

int64_t TimeUtils::GetTimeMS()
{
    std::chrono::milliseconds nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
        return nowMs.count();
}

int64_t TimeUtils::GetTimeUS()
{
    std::chrono::microseconds nowUs = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch());
        return nowUs.count();
}

uint32_t TimeUtils::GetSpendSecond(int64_t startTime)
{
    if (startTime == 0) {
        return 0;
    }
    int64_t endTime = GetTimeS();
    if (endTime < startTime) {
        return 0;
    }
    return static_cast<uint32_t>(endTime - startTime);
}

uint32_t TimeUtils::GetSpendMS(int64_t startTime)
{
    if (startTime == 0) {
        return 0;
    }
    int64_t endTime = GetTimeMS();
    if (endTime < startTime) {
        return 0;
    }
    return static_cast<uint32_t>(endTime - startTime);
}

uint64_t TimeUtils::GetSpendUS(int64_t startTime)
{
    if (startTime == 0) {
        return 0;
    }
    int64_t endTime = GetTimeUS();
    if (endTime < startTime) {
        return 0;
    }
    return endTime - startTime;
}

std::string TimeUtils::GetCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    std::stringstream strTime;
    strTime << (std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S:")) << (std::setfill('0'))
            << (std::setw(BConstants::INDEX)) << (ms.count() % BConstants::MS_1000);
    return strTime.str();
}

int32_t TimeUtils::GetAmsTimeout()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (amsTimeoutRatio_ == INVALID_RATIO) {
        amsTimeoutRatio_ = OHOS::system::GetIntParameter<int32_t>(TIMEOUT_UNIT_TIME_RATIO, 1);
    }
    if (amsTimeoutRatio_ < 0 || amsTimeoutRatio_ > MAX_RATIO) {
        HILOGE("timeout ratio is invalid:%{public}d", amsTimeoutRatio_);
        amsTimeoutRatio_ = MAX_RATIO;
    }
    return amsTimeoutRatio_ * CONNECT_EXTENSION_TIMEOUT;
}

int32_t TimeUtils::GenAfsTimeout()
{
    return GetAmsTimeout() + AFS_RESERVE_SECOND;
}
} // namespace OHOS::FileManagement::Backup