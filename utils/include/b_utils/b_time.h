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

#ifndef OHOS_FILEMGMT_BACKUP_B_TIME_H
#define OHOS_FILEMGMT_BACKUP_B_TIME_H
#include <string>

namespace OHOS::FileManagement::Backup {

constexpr uint32_t MS_TO_US = 1000;
constexpr uint32_t SECOND_TO_MS = 1000;

class TimeUtils {
public:
    static int64_t GetTimeS();
    static int64_t GetTimeMS();
    static int64_t GetTimeUS();
    static uint32_t GetSpendSecond(int64_t startSecond);
    static uint32_t GetSpendMS(int64_t startMS);
    static uint64_t GetSpendUS(int64_t startUS);
    static std::string GetCurrentTime();
};
} // namespace OHOS::FileManagement::TimeUtils
#endif // OHOS_FILEMGMT_BACKUP_B_TIME_H