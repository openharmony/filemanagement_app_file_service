/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "service_stub.h"
namespace OHOS::FileManagement::Backup {
using namespace std;

int32_t ServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string localDescriptor = GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (localDescriptor != remoteDescriptor) {
        return ERR_TRANSACTION_FAILED;
    }
    switch (static_cast<IServiceIpcCode>(code)) {
        case IServiceIpcCode::COMMAND_INIT_RESTORE_SESSION: {
            sptr<IServiceReverse> reverseIpcRemoteObject;
            ErrCode errCode = InitRestoreSession(reverseIpcRemoteObject);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (reverseIpcRemoteObject == nullptr) {
                    return ERR_INVALID_DATA;
                }
                if (!reply.WriteRemoteObject(reverseIpcRemoteObject->AsObject())) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_INIT_BACKUP_SESSION: {
            sptr<IServiceReverse> reverseIpcRemoteObject;
            ErrCode errCode = InitBackupSession(reverseIpcRemoteObject);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (reverseIpcRemoteObject == nullptr) {
                    return ERR_INVALID_DATA;
                }
                if (!reply.WriteRemoteObject(reverseIpcRemoteObject->AsObject())) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_START: {
            ErrCode errCode = Start();
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_GET_LOCAL_CAPABILITIES: {
            int fd;
            ErrCode errCode = GetLocalCapabilities(fd);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteInt32(fd)) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_PUBLISH_FILE: {
            std::unique_ptr<BFileInfo> fileInfo(data.ReadParcelable<BFileInfo>());
            if (!fileInfo) {
                return ERR_INVALID_DATA;
            }

            ErrCode errCode = PublishFile(*fileInfo);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_GET_FILE_HANDLE: {
            std::string bundleName = Str16ToStr8(data.ReadString16());
            std::string fileName = Str16ToStr8(data.ReadString16());
            ErrCode errCode = GetFileHandle(bundleName, fileName);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_APPEND_BUNDLES_RESTORE_SESSION_DATA_BY_DETAIL: {
            int fd = data.ReadFileDescriptor();
            std::vector<std::string> bundleNames;
            int32_t bundleNamesSize = data.ReadInt32();
            if (bundleNamesSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {
                return ERR_INVALID_DATA;
            }
            for (int32_t i1 = 0; i1 < bundleNamesSize; ++i1) {
                std::string value1 = Str16ToStr8(data.ReadString16());
                bundleNames.push_back(value1);
            }
            std::vector<std::string> detailInfos;
            int32_t detailInfosSize = data.ReadInt32();
            if (detailInfosSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {
                return ERR_INVALID_DATA;
            }
            for (int32_t i2 = 0; i2 < detailInfosSize; ++i2) {
                std::string value2 = Str16ToStr8(data.ReadString16());
                detailInfos.push_back(value2);
            }
            int32_t restoreType = data.ReadInt32();
            int32_t userId = data.ReadInt32();
            ErrCode errCode =
                AppendBundlesRestoreSessionDataByDetail(fd, bundleNames, detailInfos, restoreType, userId);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_APPEND_BUNDLES_RESTORE_SESSION_DATA: {
            int fd = data.ReadFileDescriptor();
            std::vector<std::string> bundleNames;
            int32_t bundleNamesSize = data.ReadInt32();
            if (bundleNamesSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {
                return ERR_INVALID_DATA;
            }
            for (int32_t i3 = 0; i3 < bundleNamesSize; ++i3) {
                std::string value3 = Str16ToStr8(data.ReadString16());
                bundleNames.push_back(value3);
            }
            int32_t restoreType = data.ReadInt32();
            int32_t userId = data.ReadInt32();
            ErrCode errCode = AppendBundlesRestoreSessionData(fd, bundleNames, restoreType, userId);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_APPEND_BUNDLES_BACKUP_SESSION: {
            std::vector<std::string> bundleNames;
            int32_t bundleNamesSize = data.ReadInt32();
            if (bundleNamesSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {
                return ERR_INVALID_DATA;
            }
            for (int32_t i4 = 0; i4 < bundleNamesSize; ++i4) {
                std::string value4 = Str16ToStr8(data.ReadString16());
                bundleNames.push_back(value4);
            }
            ErrCode errCode = AppendBundlesBackupSession(bundleNames);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_APPEND_BUNDLES_DETAILS_BACKUP_SESSION: {
            std::vector<std::string> bundleNames;
            int32_t bundleNamesSize = data.ReadInt32();
            if (bundleNamesSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {
                return ERR_INVALID_DATA;
            }
            for (int32_t i5 = 0; i5 < bundleNamesSize; ++i5) {
                std::string value5 = Str16ToStr8(data.ReadString16());
                bundleNames.push_back(value5);
            }
            std::vector<std::string> bundleInfos;
            int32_t bundleInfosSize = data.ReadInt32();
            if (bundleInfosSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {
                return ERR_INVALID_DATA;
            }
            for (int32_t i6 = 0; i6 < bundleInfosSize; ++i6) {
                std::string value6 = Str16ToStr8(data.ReadString16());
                bundleInfos.push_back(value6);
            }
            ErrCode errCode = AppendBundlesDetailsBackupSession(bundleNames, bundleInfos);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_FINISH: {
            ErrCode errCode = Finish();
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_RELEASE: {
            ErrCode errCode = Release();
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_CANCEL: {
            std::string bundleName = Str16ToStr8(data.ReadString16());
            int32_t cancelResult;
            ErrCode errCode = Cancel(bundleName, cancelResult);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteInt32(cancelResult)) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_GET_APP_LOCAL_LIST_AND_DO_INCREMENTAL_BACKUP: {
            ErrCode errCode = GetAppLocalListAndDoIncrementalBackup();
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_GET_INCREMENTAL_FILE_HANDLE: {
            std::string bundleName = Str16ToStr8(data.ReadString16());
            std::string fileName = Str16ToStr8(data.ReadString16());
            ErrCode errCode = GetIncrementalFileHandle(bundleName, fileName);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_GET_BACKUP_INFO: {
            std::string bundleName;
            std::string getBackupInfoResult;
            ErrCode errCode = GetBackupInfo(bundleName, getBackupInfoResult);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteString16(Str8ToStr16(bundleName))) {
                    return ERR_INVALID_DATA;
                }
                if (!reply.WriteString16(Str8ToStr16(getBackupInfoResult))) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_UPDATE_TIMER: {
            std::string bundleName;
            uint32_t timeout = data.ReadUint32();
            bool updateTimerResult;
            ErrCode errCode = UpdateTimer(bundleName, timeout, updateTimerResult);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteString16(Str8ToStr16(bundleName))) {
                    return ERR_INVALID_DATA;
                }
                if (!reply.WriteInt32(updateTimerResult ? 1 : 0)) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_UPDATE_SEND_RATE: {
            std::string bundleName;
            int32_t sendRate = data.ReadInt32();
            bool updateSendRateResult;
            ErrCode errCode = UpdateSendRate(bundleName, sendRate, updateSendRateResult);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteString16(Str8ToStr16(bundleName))) {
                    return ERR_INVALID_DATA;
                }
                if (!reply.WriteInt32(updateSendRateResult ? 1 : 0)) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_START_EXT_TIMER: {
            bool isExtStart;
            ErrCode errCode = StartExtTimer(isExtStart);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteInt32(isExtStart ? 1 : 0)) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_START_FWK_TIMER: {
            bool isFwkStart;
            ErrCode errCode = StartFwkTimer(isFwkStart);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteInt32(isFwkStart ? 1 : 0)) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_STOP_EXT_TIMER: {
            bool isExtStop;
            ErrCode errCode = StopExtTimer(isExtStop);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteInt32(isExtStop ? 1 : 0)) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_GET_LOCAL_CAPABILITIES_INCREMENTAL: {
            std::vector<BIncrementalData> bundleNames;
            int32_t bundleNamesSize = data.ReadInt32();
            if (bundleNamesSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {
                return ERR_INVALID_DATA;
            }
            for (int32_t i7 = 0; i7 < bundleNamesSize; ++i7) {
                std::unique_ptr<BIncrementalData> value7(data.ReadParcelable<BIncrementalData>());
                if (!value7) {
                    return ERR_INVALID_DATA;
                }

                bundleNames.push_back(*value7);
            }
            int fd;
            ErrCode errCode = GetLocalCapabilitiesIncremental(bundleNames, fd);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteInt32(fd)) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_INIT_INCREMENTAL_BACKUP_SESSION: {
            sptr<IServiceReverse> reverseIpcRemoteObject;
            ErrCode errCode = InitIncrementalBackupSession(reverseIpcRemoteObject);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (reverseIpcRemoteObject == nullptr) {
                    return ERR_INVALID_DATA;
                }
                if (!reply.WriteRemoteObject(reverseIpcRemoteObject->AsObject())) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION: {
            std::vector<BIncrementalData> bundlesToBackup;
            int32_t bundlesToBackupSize = data.ReadInt32();
            if (bundlesToBackupSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {
                return ERR_INVALID_DATA;
            }
            for (int32_t i8 = 0; i8 < bundlesToBackupSize; ++i8) {
                std::unique_ptr<BIncrementalData> value8(data.ReadParcelable<BIncrementalData>());
                if (!value8) {
                    return ERR_INVALID_DATA;
                }

                bundlesToBackup.push_back(*value8);
            }
            ErrCode errCode = AppendBundlesIncrementalBackupSession(bundlesToBackup);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION_WITH_BUNDLE_INFOS: {
            std::vector<BIncrementalData> bundlesToBackup;
            int32_t bundlesToBackupSize = data.ReadInt32();
            if (bundlesToBackupSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {
                return ERR_INVALID_DATA;
            }
            for (int32_t i9 = 0; i9 < bundlesToBackupSize; ++i9) {
                std::unique_ptr<BIncrementalData> value9(data.ReadParcelable<BIncrementalData>());
                if (!value9) {
                    return ERR_INVALID_DATA;
                }

                bundlesToBackup.push_back(*value9);
            }
            std::vector<std::string> bundleInfos;
            int32_t bundleInfosSize = data.ReadInt32();
            if (bundleInfosSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {
                return ERR_INVALID_DATA;
            }
            for (int32_t i10 = 0; i10 < bundleInfosSize; ++i10) {
                std::string value10 = Str16ToStr8(data.ReadString16());
                bundleInfos.push_back(value10);
            }
            ErrCode errCode = AppendBundlesIncrementalBackupSessionWithBundleInfos(bundlesToBackup, bundleInfos);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_PUBLISH_INCREMENTAL_FILE: {
            std::unique_ptr<BFileInfo> fileInfo(data.ReadParcelable<BFileInfo>());
            if (!fileInfo) {
                return ERR_INVALID_DATA;
            }

            ErrCode errCode = PublishIncrementalFile(*fileInfo);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_PUBLISH_S_A_INCREMENTAL_FILE: {
            std::unique_ptr<BFileInfo> fileInfo(data.ReadParcelable<BFileInfo>());
            if (!fileInfo) {
                return ERR_INVALID_DATA;
            }

            int fd = data.ReadFileDescriptor();
            ErrCode errCode = PublishSAIncrementalFile(*fileInfo, fd);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_APP_INCREMENTAL_FILE_READY: {
            std::string fileName = Str16ToStr8(data.ReadString16());
            int fd = data.ReadFileDescriptor();
            int manifestFd = data.ReadFileDescriptor();
            int32_t appIncrementalFileReadyErrCode = data.ReadInt32();
            ErrCode errCode = AppIncrementalFileReady(fileName, fd, manifestFd, appIncrementalFileReadyErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_APP_INCREMENTAL_DONE: {
            int32_t appIncrementalDoneErrCode = data.ReadInt32();
            ErrCode errCode = AppIncrementalDone(appIncrementalDoneErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_REPORT_APP_PROCESS_INFO: {
            std::string processInfo = Str16ToStr8(data.ReadString16());
            BackupRestoreScenario scenario;
            {
                uint64_t enumTmp = 0;
                if (!data.ReadUint64(enumTmp)) {
                    return ERR_INVALID_DATA;
                }
                scenario = static_cast<BackupRestoreScenario>(enumTmp);
            }
            ErrCode errCode = ReportAppProcessInfo(processInfo, scenario);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_REFRESH_DATA_SIZE: {
            int64_t totalDataSize = data.ReadInt64();
            ErrCode errCode = RefreshDataSize(totalDataSize);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_APP_DONE: {
            int32_t appDoneErrCode = data.ReadInt32();
            ErrCode errCode = AppDone(appDoneErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_SERVICE_RESULT_REPORT: {
            std::string restoreRetInfo;
            BackupRestoreScenario scenario;
            {
                uint64_t enumTmp = 0;
                if (!data.ReadUint64(enumTmp)) {
                    return ERR_INVALID_DATA;
                }
                scenario = static_cast<BackupRestoreScenario>(enumTmp);
            }
            int32_t serviceResultReportErrCode = data.ReadInt32();
            ErrCode errCode = ServiceResultReport(restoreRetInfo, scenario, serviceResultReportErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteString16(Str8ToStr16(restoreRetInfo))) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case IServiceIpcCode::COMMAND_APP_FILE_READY: {
            std::string fileName = Str16ToStr8(data.ReadString16());
            int fd = data.ReadFileDescriptor();
            int32_t appFileReadyErrCode = data.ReadInt32();
            ErrCode errCode = AppFileReady(fileName, fd, appFileReadyErrCode);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return ERR_TRANSACTION_FAILED;
}
} // namespace OHOS::FileManagement::Backup
