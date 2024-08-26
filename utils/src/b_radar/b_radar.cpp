/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "b_radar/b_radar.h"

#include <iomanip>
#include <sstream>
#include <unistd.h>

#include "b_process/b_multiuser.h"
#include "b_resources/b_constants.h"
#include "hisysevent.h"

namespace {
constexpr uint8_t INDEX = 3;
constexpr int32_t MS_1000 = 1000;
const std::string FILE_BACKUP_EVENTS = "FILE_BACKUP_EVENTS";
}
namespace OHOS::FileManagement::Backup {
static std::string GetCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    std::stringstream strTime;
    strTime << (std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S:")) << (std::setfill('0'))
            << (std::setw(INDEX)) << (ms.count() % MS_1000);
    return strTime.str();
}

int32_t AppRadar::GetUserId()
{
    auto multiuser = BMultiuser::ParseUid(getuid());
    if ((multiuser.userId == BConstants::SYSTEM_UID) || (multiuser.userId == BConstants::XTS_UID)) {
        return BConstants::DEFAULT_USER_ID;
    }
    return multiuser.userId;
}

void AppRadar::RecordBackUpFuncResWithBundle(const std::string &func, const std::string &bundleName,
                                             int32_t userId, enum BizStage bizStage, int32_t resultCode,
                                             const std::string &resultInfo)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        FILE_BACKUP_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "BUNDLE_NAME", bundleName,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", func,
        "TIME", GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BizScene::BACKUP),
        "BIZ_STAGE", static_cast<int32_t>(bizStage),
        "RESULT_CODE", resultCode,
        "RESULT_INFO", resultInfo);
}

void AppRadar::RecordBackUpFuncResWithoutBundle(const std::string &func, int32_t userId, enum BizStage bizStage,
                                                int32_t resultCode, const std::string &resultInfo)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        FILE_BACKUP_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", func,
        "TIME", GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BizScene::BACKUP),
        "BIZ_STAGE", static_cast<int32_t>(bizStage),
        "RESULT_CODE", resultCode,
        "RESULT_INFO", resultInfo);
}

void AppRadar::RecordDoBackUpRes(const std::string &func, int32_t userId, int32_t resultCode,
                                 int32_t exportDuration)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        FILE_BACKUP_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", func,
        "TIME", GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BizScene::BACKUP),
        "BIZ_STAGE", static_cast<int32_t>(BizStage::BIZ_STAGE_DO_BACKUP),
        "EXEC_STATUS", "{\"spend_time\":" + std::to_string(exportDuration) + "ms\"}",
        "RESULT_CODE", resultCode);
}

void AppRadar::RecordRestoreFuncRes(int32_t userId, const std::string &func, enum BizStageRestore bizStage,
                                    int32_t resultCode)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        FILE_BACKUP_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", func,
        "TIME", GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BizScene::RESTORE),
        "BIZ_STAGE", static_cast<int32_t>(bizStage),
        "RESULT_CODE", resultCode);
}

void AppRadar::RecordRestoreFuncResWithBundle(const std::string &bundleName, int32_t userId, const std::string &func,
                                              enum BizStageRestore bizStage, int32_t resultCode)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        FILE_BACKUP_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "BUNDLE_NAME", bundleName,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", func,
        "TIME", GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BizScene::RESTORE),
        "BIZ_STAGE", static_cast<int32_t>(bizStage),
        "RESULT_CODE", resultCode);
}

void AppRadar::RecordRestoreFuncResWithResult(const std::string &bundleName, int32_t userId, const std::string &func,
                                              enum BizStageRestore bizStage, int32_t resultCode,
                                              const std::string &resultInfo)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        FILE_BACKUP_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "BUNDLE_NAME", bundleName,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", func,
        "TIME", GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BizScene::RESTORE),
        "BIZ_STAGE", static_cast<int32_t>(bizStage),
        "RESULT_CODE", resultCode,
        "RESULT_INFO", resultInfo);
}

void AppRadar::RecordRestoreFuncResWithStatus(const std::string &bundleName, int32_t userId, const std::string &func,
                                              enum BizStageRestore bizStage, const std::string &execStatus,
                                              int32_t resultCode)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        FILE_BACKUP_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "BUNDLE_NAME", bundleName,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", func,
        "TIME", GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BizScene::RESTORE),
        "BIZ_STAGE", static_cast<int32_t>(bizStage),
        "EXEC_STATUS", execStatus,
        "RESULT_CODE", resultCode);
}

void AppRadar::RecordGetFileHandleRes(const std::string &bundleName, int32_t userId, int32_t resultCode,
                                      const std::string &resultInfo)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        FILE_BACKUP_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "BUNDLE_NAME", bundleName,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", "GetFileHandle",
        "TIME", GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BizScene::RESTORE),
        "BIZ_STAGE", static_cast<int32_t>(BizStageRestore::BIZ_STAGE_GET_FILE_HANDLE),
        "RESULT_CODE", resultCode,
        "RESULT_INFO", resultInfo);
}

void AppRadar::RecordOnRestoreRes(const std::string &bundleName, int32_t userId, const std::string &func,
                                  const std::string &execStatus, int32_t resultCode)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        FILE_BACKUP_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "BUNDLE_NAME", bundleName,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", func,
        "TIME", GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BizScene::RESTORE),
        "BIZ_STAGE", static_cast<int32_t>(BizStageRestore::BIZ_STAGE_EXEC_ON_RESTORE),
        "EXEC_STATUS", execStatus,
        "RESULT_CODE", resultCode);
}

void AppRadar::RecordBackupSARes(int32_t userId, const std::string &func, int32_t resultCode,
                                 const std::string &resultInfo)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        FILE_BACKUP_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", func,
        "TIME", GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BizScene::RESTORE),
        "BIZ_STAGE", static_cast<int32_t>(BizStageRestore::BIZ_STAGE_BACKUP_SA),
        "RESULT_CODE", resultCode,
        "RESULT_INFO", resultInfo);
}
} // namespace OHOS::FileManagement::Backup