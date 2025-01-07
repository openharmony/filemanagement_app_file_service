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

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_STUB_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_STUB_H

#include <map>

#include "i_service.h"
#include "iremote_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceStub : public IRemoteStub<IService> {
public:
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

public:
    ServiceStub();
    ~ServiceStub() override = default;

private:
    using ServiceInterface = int32_t (ServiceStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, ServiceInterface> opToInterfaceMap_;

    int32_t CmdInitRestoreSession(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInitRestoreSessionMsg(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInitBackupSession(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInitBackupSessionMsg(MessageParcel &data, MessageParcel &reply);
    int32_t CmdStart(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetLocalCapabilities(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetLocalCapabilitiesForBdInfos(MessageParcel &data, MessageParcel &reply);
    int32_t CmdPublishFile(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAppFileReady(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAppDone(MessageParcel &data, MessageParcel &reply);
    int32_t CmdResultReport(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetFileHandle(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAppendBundlesRestoreSession(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAppendBundlesDetailsRestoreSession(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAppendBundlesBackupSession(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAppendBundlesDetailsBackupSession(MessageParcel &data, MessageParcel &reply);
    int32_t CmdFinish(MessageParcel &data, MessageParcel &reply);
    int32_t CmdRelease(MessageParcel &data, MessageParcel &reply);
    int32_t CmdCancel(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetLocalCapabilitiesIncremental(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetAppLocalListAndDoIncrementalBackup(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInitIncrementalBackupSession(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInitIncrementalBackupSessionMsg(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAppendBundlesIncrementalBackupSession(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAppendBundlesDetailsIncrementalBackupSession(MessageParcel &data, MessageParcel &reply);
    int32_t CmdPublishIncrementalFile(MessageParcel &data, MessageParcel &reply);
    int32_t CmdPublishSAIncrementalFile(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAppIncrementalFileReady(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAppIncrementalDone(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetIncrementalFileHandle(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetBackupInfo(MessageParcel &data, MessageParcel &reply);
    int32_t CmdUpdateTimer(MessageParcel &data, MessageParcel &reply);
    int32_t CmdUpdateSendRate(MessageParcel &data, MessageParcel &reply);
    int32_t CmdReportAppProcessInfo(MessageParcel &data, MessageParcel &reply);
    int32_t CmdStartExtTimer(MessageParcel &data, MessageParcel &reply);
    int32_t CmdStartFwkTimer(MessageParcel &data, MessageParcel &reply);
    int32_t CmdStopExtTimer(MessageParcel &data, MessageParcel &reply);
    int32_t CmdRefreshDataSize(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetBackupDataSize(MessageParcel &data, MessageParcel &reply);
    void ServiceStubSupplement();
    void ServiceStubSuppAppendBundles();

public:
    template <typename T>
    bool ReadParcelableVector(std::vector<T> &parcelableInfos, MessageParcel &data);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_STUB_H