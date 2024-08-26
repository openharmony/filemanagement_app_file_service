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

#ifndef OHOS_FILEMGMT_BACKUP_B_RADAR_H
#define OHOS_FILEMGMT_BACKUP_B_RADAR_H
#include <chrono>
#include <string>

namespace OHOS::FileManagement::Backup {
enum class BizScene : int32_t {
    BACKUP = 1,
    RESTORE = 2
};

enum class BizStage : int32_t {
    BIZ_STAGE_GET_LOCAL_CAPABILITIES = 1,
    BIZ_STAGE_GET_BACKUP_INFO,
    BIZ_STAGE_CREATE_SESSION_BACKUP,
    BIZ_STAGE_BACKUP_SA,
    BIZ_STAGE_APPEND_BUNDLES,
    BIZ_STAGE_BACKUP_EXTENSION,
    BIZ_STAGE_APPLICATION,
    BIZ_STAGE_EXTENSION_STATUS,
    BIZ_STAGE_ON_BACKUP,
    BIZ_STAGE_DO_BACKUP,
    BIZ_STAGE_DATA_CONSISTENCY,
    BIZ_STAGE_STATUS_CLEAR,
    BIZ_STAGE_CLIENT_STATUS,
    BIZ_STAGE_PERMISSION_CHECK
};

enum class BizStageRestore : int32_t {
    BIZ_STAGE_GET_LOCAL_CAPABILITIES = 1,
    BIZ_STAGE_CREATE_SESSION_RESTORE,
    BIZ_STAGE_BACKUP_SA,
    BIZ_STAGE_APPEND_BUNDLES,
    BIZ_STAGE_CONNECT_BACKUP_EXTENSION,
    BIZ_STAGE_APPLICATION,
    BIZ_STAGE_EXTENSION_STATUS,
    BIZ_STAGE_GET_FILE_HANDLE,
    BIZ_STAGE_DO_RESTORE,
    BIZ_STAGE_DATA_CONSISTENCY,
    BIZ_STAGE_EXEC_ON_RESTORE,
    BIZ_STAGE_STATUS_CLEAR,
    BIZ_STAGE_CLIENT_STATUS,
    BIZ_STAGE_PERMISSION_CHECK
};

class AppRadar {
public:
    static AppRadar &GetInstance()
    {
        static AppRadar instance;
        return instance;
    }

public:
    int32_t GetUserId();
    void RecordBackUpFuncResWithBundle(const std::string &func, const std::string &bundleName, int32_t userId,
                                       enum BizStage bizStage, int32_t resultCode,
                                       const std::string &resultInfo = "{\"reason\":\"failed\"}");
    void RecordBackUpFuncResWithoutBundle(const std::string &func, int32_t userId,
                                          enum BizStage bizStage, int32_t resultCode,
                                          const std::string &resultInfo = "{\"reason\":\"failed\"}");
    void RecordDoBackUpRes(const std::string &func, int32_t userId, int32_t resultCode, int32_t exportDuration);
    void RecordRestoreFuncRes(int32_t userId, const std::string &func, enum BizStageRestore bizStage,
                              int32_t resultCode);
    void RecordRestoreFuncResWithBundle(const std::string &bundleName, int32_t userId, const std::string &func,
                                        enum BizStageRestore bizStage, int32_t resultCode);
    void RecordRestoreFuncResWithResult(const std::string &bundleName, int32_t userId, const std::string &func,
                                        enum BizStageRestore bizStage, int32_t resultCode,
                                        const std::string &resultInfo);
    void RecordRestoreFuncResWithStatus(const std::string &bundleName, int32_t userId, const std::string &func,
                                        enum BizStageRestore bizStage, const std::string &execStatus,
                                        int32_t resultCode);
    void RecordGetFileHandleRes(const std::string &bundleName, int32_t userId, int32_t resultCode,
                                const std::string &resultInfo);
    void RecordOnRestoreRes(const std::string &bundleName, int32_t userId, const std::string &func,
                            const std::string &execStatus, int32_t resultCode);
    void RecordBackupSARes(int32_t userId, const std::string &func, int32_t resultCode, const std::string &resultInfo);
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
