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

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_H

#include <vector>
#include <string>

#include "b_error/b_error.h"
#include "i_service_reverse.h"
#include "iremote_stub.h"
#include "service_stub.h"

namespace OHOS::FileManagement::Backup {
class Service : public ServiceStub {
public:
    ErrCode InitRestoreSession(sptr<IServiceReverse>) { return BError(BError::Codes::OK); }
    ErrCode InitRestoreSession(sptr<IServiceReverse>, std::string&) { return BError(BError::Codes::OK); }
    ErrCode InitBackupSession(sptr<IServiceReverse>) { return BError(BError::Codes::OK); }
    ErrCode InitBackupSession(sptr<IServiceReverse>, std::string&) { return BError(BError::Codes::OK); }
    ErrCode Start() { return BError(BError::Codes::OK); }
    UniqueFd GetLocalCapabilities() {  return UniqueFd(-1); }
    UniqueFd GetLocalCapabilitiesForBundleInfos() {  return UniqueFd(-1); }
    ErrCode PublishFile(const BFileInfo&) { return BError(BError::Codes::OK); }
    ErrCode AppFileReady(const std::string&, UniqueFd, int32_t) { return BError(BError::Codes::OK); }
    ErrCode AppDone(ErrCode) { return BError(BError::Codes::OK); }
    ErrCode ServiceResultReport(const std::string,
        BackupRestoreScenario, ErrCode) { return BError(BError::Codes::OK); }
    ErrCode GetFileHandle(const std::string&, const std::string&) { return BError(BError::Codes::OK); }
    ErrCode AppendBundlesRestoreSession(UniqueFd,
                                        const std::vector<BundleName>&,
                                        const std::vector<std::string>&,
                                        RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND,
                                        int32_t userId = DEFAULT_INVAL_VALUE) { return BError(BError::Codes::OK); }
    ErrCode AppendBundlesRestoreSession(UniqueFd,
                                        const std::vector<BundleName>&,
                                        RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND,
                                        int32_t userId = DEFAULT_INVAL_VALUE) { return BError(BError::Codes::OK); }
    ErrCode AppendBundlesBackupSession(const std::vector<BundleName>&) { return BError(BError::Codes::OK); }
    ErrCode AppendBundlesDetailsBackupSession(const std::vector<BundleName>&,
                                              const std::vector<std::string>&) { return BError(BError::Codes::OK); }
    ErrCode Finish() { return BError(BError::Codes::OK); }
    ErrCode Release() { return BError(BError::Codes::OK); }
    ErrCode Cancel(std::string, int32_t&) { return BError(BError::Codes::OK); }
    UniqueFd GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData>&) { return UniqueFd(-1); }
    ErrCode GetAppLocalListAndDoIncrementalBackup() { return BError(BError::Codes::OK); }
    ErrCode InitIncrementalBackupSession(sptr<IServiceReverse>) { return BError(BError::Codes::OK); }
    ErrCode InitIncrementalBackupSession(sptr<IServiceReverse>, std::string&) { return BError(BError::Codes::OK); }
    ErrCode AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData>&)
        { return BError(BError::Codes::OK); }
    ErrCode AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData>&, const std::vector<std::string>&)
        { return BError(BError::Codes::OK); }
    ErrCode PublishIncrementalFile(const BFileInfo&) { return BError(BError::Codes::OK); }
    ErrCode PublishSAIncrementalFile(const BFileInfo&, UniqueFd) { return BError(BError::Codes::OK); }
    ErrCode AppIncrementalFileReady(const std::string&, UniqueFd, UniqueFd, int32_t)
        { return BError(BError::Codes::OK); }
    ErrCode AppIncrementalDone(ErrCode) { return BError(BError::Codes::OK); }
    ErrCode GetIncrementalFileHandle(const std::string&, const std::string&) { return BError(BError::Codes::OK); }
    ErrCode GetBackupInfo(BundleName&, std::string&) { return BError(BError::Codes::OK); }
    ErrCode UpdateTimer(BundleName&, uint32_t, bool&) { return BError(BError::Codes::OK); }
    ErrCode UpdateSendRate(std::string&, int32_t, bool&) { return BError(BError::Codes::OK); }
    ErrCode ReportAppProcessInfo(const std::string, const BackupRestoreScenario) { return BError(BError::Codes::OK); }
    ErrCode StartExtTimer(bool&) { return BError(BError::Codes::OK); }
    ErrCode StartFwkTimer(bool&) { return BError(BError::Codes::OK); }
    ErrCode StopExtTimer(bool&) { return BError(BError::Codes::OK); }
    ErrCode RefreshDataSize(int64_t) { return BError(BError::Codes::OK); }

    ErrCode SAResultReport(const std::string, const std::string,
                           const ErrCode, const BackupRestoreScenario) { return BError(BError::Codes::OK); }
    ErrCode GetBackupDataSize(bool, std::vector<BIncrementalData>) { return BError(BError::Codes::OK); }
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_H