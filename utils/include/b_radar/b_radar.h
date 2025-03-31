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

#ifndef OHOS_FILEMGMT_BACKUP_B_RADAR_H
#define OHOS_FILEMGMT_BACKUP_B_RADAR_H
#include <chrono>
#include <string>
#include "iservice_reverse.h"

namespace OHOS::FileManagement::Backup {
enum class BizStageBackup : int32_t {
    BIZ_STAGE_DEFAULT = 0,
    BIZ_STAGE_BOOT_BACKUP_SA_FAIL,
    BIZ_STAGE_GET_LOCAL_CAPABILITIES_FAIL,
    BIZ_STAGE_CREATE_BACKUP_SESSION_FAIL,
    BIZ_STAGE_APPEND_BUNDLES_FAIL,
    BIZ_STAGE_CONNECT_EXTENSION_FAIL,
    BIZ_STAGE_START_DISPOSE,
    BIZ_STAGE_EXTENSION_ABNORMAL_EXIT_CLEAR_FAIL,
    BIZ_STAGE_GET_BACKUP_INFO_FAIL,
    BIZ_STAGE_ON_BACKUP,
    BIZ_STAGE_DO_BACKUP,
    BIZ_STAGE_CHECK_DATA_FAIL,
    BIZ_STAGE_END_DISPOSE,
    BIZ_STAGE_PERMISSION_CHECK_FAIL,
    BIZ_STAGE_EXECU_FAIL,
    BIZ_STAGE_ACTIVE_SESSION,
    BIZ_STAGE_DEACTIVE_SESSION,
    BIZ_STAGE_RELEASE,
    BIZ_STAGE_ONSTART_DISPOSE,
    BIZ_STAGE_ONSTART_RESIDUAL,
};

enum class BizStageRestore : int32_t {
    BIZ_STAGE_DEFAULT = 0,
    BIZ_STAGE_BOOT_BACKUP_SA_FAIL,
    BIZ_STAGE_GET_LOCAL_CAPABILITIES_FAIL,
    BIZ_STAGE_CREATE_RESTORE_SESSION_FAIL,
    BIZ_STAGE_APPEND_BUNDLES_FAIL,
    BIZ_STAGE_CONNECT_EXTENSION_FAIL,
    BIZ_STAGE_START_DISPOSE,
    BIZ_STAGE_EXTENSION_ABNORMAL_EXIT_CLEAR_FAIL,
    BIZ_STAGE_GET_FILE_HANDLE_FAIL,
    BIZ_STAGE_DO_RESTORE,
    BIZ_STAGE_CHECK_DATA_FAIL,
    BIZ_STAGE_ON_RESTORE,
    BIZ_STAGE_END_DISPOSE,
    BIZ_STAGE_PERMISSION_CHECK_FAIL,
    BIZ_STAGE_EXECU_FAIL,
    BIZ_STAGE_ACTIVE_SESSION,
    BIZ_STAGE_DEACTIVE_SESSION,
    BIZ_STAGE_RELEASE,
    BIZ_STAGE_ONSTART_DISPOSE,
    BIZ_STAGE_ONSTART_RESIDUAL,
};

class AppRadar {
public:
    static AppRadar &GetInstance()
    {
        static AppRadar instance;
        return instance;
    }

public:
    struct Info {
        std::string bundleName;
        std::string status;
        std::string resInfo;

        Info(const std::string &bundleName, const std::string &status, const std::string &resInfo)
            : bundleName(bundleName), status(status), resInfo(resInfo) {}
    };

    struct DoRestoreInfo {
        uint32_t bigFileNum;
        uint64_t bigFileSize;
        int64_t bigFileSpendTime;
        uint32_t tarFileNum;
        uint64_t tarFileSize;
        int64_t tarFileSpendTime;
        int64_t totalFileSpendTime;
    };

    struct DoBackupInfo {
        int64_t cost;
        uint32_t allFileNum;
        uint32_t smallFileNum;
        uint32_t tarFileNum;
        uint32_t includeNum;
        uint32_t excludeNum;
    };

    struct StatInfo {
        std::string callerName;
        std::string resInfo;

        StatInfo(const std::string &callerName, const std::string &resInfo)
            : callerName(callerName), resInfo(resInfo) {}
    };

public:
    int32_t GetUserId();
    void RecordDefaultFuncRes(Info &info, const std::string &func, int32_t userId,
                              enum BizStageBackup bizStage, int32_t resultCode);
    void RecordBackupFuncRes(Info &info, const std::string &func, int32_t userId,
                             enum BizStageBackup bizStage, int32_t resultCode);
    void RecordRestoreFuncRes(Info &info, const std::string &func, int32_t userId,
                              enum BizStageRestore bizStage, int32_t resultCode);
    void RecordStatisticRes(StatInfo &statInfo, int32_t userId, enum IServiceReverseType::Scenario scenario,
                            int32_t succ_cnt, int32_t fail_cnt, int32_t resultCode);
private:
    AppRadar() = default;
    ~AppRadar() = default;
    AppRadar(const AppRadar &) = delete;
    AppRadar &operator=(const AppRadar &) = delete;
    AppRadar(AppRadar &&) = delete;
    AppRadar &operator=(AppRadar &&) = delete;
};
} // namespace OHOS::FileManagement::AppRadar
#endif // OHOS_FILEMGMT_BACKUP_B_RADAR_H
