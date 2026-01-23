/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HI_AUDIT_H
#define HI_AUDIT_H

#include <cstdint>
#include <mutex>
#include <queue>
#include <string>
#include <sys/stat.h>

#include "b_resources/b_constants.h"
#include "nocopyable.h"

namespace OHOS::FileManagement::Backup {
struct HiAuditConfig {
    std::string logPath;
    std::string logName;
    uint32_t logSize; // kb
    uint32_t fileSize;
    uint32_t fileCount;
};

struct AuditLog {
    bool isUserBehavior;
    std::string cause;
    std::string operationType;
    std::string operationScenario;
    uint32_t operationCount;
    std::string operationStatus;
    std::string extend;
    std::string type;
    std::string path;

    const std::string TitleString() const
    {
        return "happenTime, packageName, isForeground, cause, isUserBehavior, operationType, operationScenario, "
            "operationStatus, operationCount, extend, type, path\n";
    }

    const std::string ToString() const
    {
        return cause + ", " + std::to_string(isUserBehavior) + ", " + operationType + ", " + operationScenario + ", " +
            operationStatus + ", " + std::to_string(operationCount) + ", " + extend + "," + type + "," + path + "\n";
    }
};

class HiAudit : public NoCopyable {
public:
    static HiAudit &GetInstance(bool isSaJob);
    void Write(const AuditLog &auditLog);

private:
    HiAudit(bool isSaJob);
    ~HiAudit();

    void Init();
    void GetWriteFilePath();
    void WriteToFile(const std::string &log);
    uint64_t GetMilliseconds();
    std::string GetFormattedTimestamp(time_t timeStamp, const std::string &format);
    std::string GetFormattedTimestampEndWithMilli();
    void CleanOldAuditFile();
    void ZipAuditLog();
    bool MkLogDirSuccess();

private:
    std::mutex mutex_;
    int writeFd_ = BConstants::INVALID_FD_NUM;
    std::atomic<uint32_t> writeLogSize_ = 0;
    bool isSaJob_ = false;
    HiAuditConfig hiAuditConfig_;
};
} // namespace OHOS::FileManagement::Backup
#endif // HI_AUDIT_H
