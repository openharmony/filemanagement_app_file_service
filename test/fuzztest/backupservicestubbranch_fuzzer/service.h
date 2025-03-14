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
#include "iservice_reverse.h"
#include "iremote_stub.h"
#include "service_stub.h"
#include "service_common.h"

namespace OHOS::FileManagement::Backup {
class Service : public ServiceStub {
public:
    ErrCode InitRestoreSession(const sptr<IServiceReverse>&) { return BError(BError::Codes::OK); }
    ErrCode InitRestoreSessionWithErrMsg(const sptr<IServiceReverse>&, std::string&) { return BError(BError::Codes::OK); }
    ErrCode InitBackupSession(const sptr<IServiceReverse>&) { return BError(BError::Codes::OK); }
    ErrCode InitBackupSessionWithErrMsg(const sptr<IServiceReverse>&, std::string&) { return BError(BError::Codes::OK); }
    ErrCode Start() { return BError(BError::Codes::OK); }
    ErrCode GetLocalCapabilities(int&) { return BError(BError::Codes::OK); }
    ErrCode GetLocalCapabilitiesForBundleInfos(int&) {  return BError(BError::Codes::OK); }
    ErrCode PublishFile(const BFileInfo&) { return BError(BError::Codes::OK); }
    ErrCode AppFileReady(const std::string&, int, int32_t) { return BError(BError::Codes::OK); }
    ErrCode AppDone(ErrCode) { return BError(BError::Codes::OK); }
    ErrCode ServiceResultReport(const std::string&,
        BackupRestoreScenario, ErrCode) { return BError(BError::Codes::OK); }
    ErrCode GetFileHandle(const std::string&, const std::string&) { return BError(BError::Codes::OK); }
    ErrCode AppendBundlesRestoreSessionDataByDetail(int,
                                        const std::vector<BundleName>&,
                                        const std::vector<std::string>&,
                                        int32_t,
                                        int32_t) { return BError(BError::Codes::OK); }
    ErrCode AppendBundlesRestoreSessionData(int,
                                        const std::vector<BundleName>&,
                                        int32_t,
                                        int32_t) { return BError(BError::Codes::OK); }
    ErrCode AppendBundlesBackupSession(const std::vector<BundleName>&) { return BError(BError::Codes::OK); }
    ErrCode AppendBundlesDetailsBackupSession(const std::vector<BundleName>&,
                                              const std::vector<std::string>&) { return BError(BError::Codes::OK); }
    ErrCode Finish() { return BError(BError::Codes::OK); }
    ErrCode Release() { return BError(BError::Codes::OK); }
    ErrCode Cancel(const std::string&, int32_t&) { return BError(BError::Codes::OK); }
    ErrCode GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData>&, int&) { return BError(BError::Codes::OK); }
    ErrCode GetAppLocalListAndDoIncrementalBackup() { return BError(BError::Codes::OK); }
    ErrCode InitIncrementalBackupSession(const sptr<IServiceReverse>&) { return BError(BError::Codes::OK); }
    ErrCode InitIncrementalBackupSessionWithErrMsg(const sptr<IServiceReverse>&, std::string&) { return BError(BError::Codes::OK); }
    ErrCode AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData>&)
        { return BError(BError::Codes::OK); }
    ErrCode AppendBundlesIncrementalBackupSessionWithBundleInfos(const std::vector<BIncrementalData>&, const std::vector<std::string>&)
        { return BError(BError::Codes::OK); }
    ErrCode PublishIncrementalFile(const BFileInfo&) { return BError(BError::Codes::OK); }
    ErrCode PublishSAIncrementalFile(const BFileInfo&, int) { return BError(BError::Codes::OK); }
    ErrCode AppIncrementalFileReady(const std::string&, int, int, int32_t)
        { return BError(BError::Codes::OK); }
    ErrCode AppIncrementalDone(ErrCode) { return BError(BError::Codes::OK); }
    ErrCode GetIncrementalFileHandle(const std::string&, const std::string&) { return BError(BError::Codes::OK); }
    ErrCode GetBackupInfo(const BundleName&, std::string&) { return BError(BError::Codes::OK); }
    ErrCode UpdateTimer(const BundleName&, uint32_t, bool&) { return BError(BError::Codes::OK); }
    ErrCode UpdateSendRate(const std::string&, int32_t, bool&) { return BError(BError::Codes::OK); }
    ErrCode ReportAppProcessInfo(const std::string&, BackupRestoreScenario) { return BError(BError::Codes::OK); }
    ErrCode StartExtTimer(bool&) { return BError(BError::Codes::OK); }
    ErrCode StartFwkTimer(bool&) { return BError(BError::Codes::OK); }
    ErrCode StopExtTimer(bool&) { return BError(BError::Codes::OK); }
    ErrCode RefreshDataSize(int64_t) { return BError(BError::Codes::OK); }

    ErrCode SAResultReport(const std::string, const std::string,
                           const ErrCode, const BackupRestoreScenario) { return BError(BError::Codes::OK); }
    ErrCode GetBackupDataSize(bool, const std::vector<BIncrementalData>&) { return BError(BError::Codes::OK); }
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_H