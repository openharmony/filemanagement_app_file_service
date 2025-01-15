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

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_STUB_MOCK_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_STUB_MOCK_H

#include <gmock/gmock.h>

namespace OHOS::FileManagement::Backup {
class BServiceStub {
public:
    virtual int32_t OnRemoteRequest(uint32_t, MessageParcel&, MessageParcel&, MessageOption&) = 0;
    virtual int32_t CmdInitRestoreSession(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdInitBackupSession(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdStart(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdGetLocalCapabilities(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdGetLocalCapabilitiesForBdInfos(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdPublishFile(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdAppFileReady(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdAppDone(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdResultReport(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdGetFileHandle(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdAppendBundlesRestoreSession(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdAppendBundlesDetailsRestoreSession(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdAppendBundlesBackupSession(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdAppendBundlesDetailsBackupSession(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdFinish(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdRelease(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdCancel(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdGetLocalCapabilitiesIncremental(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdGetAppLocalListAndDoIncrementalBackup(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdInitIncrementalBackupSession(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdAppendBundlesIncrementalBackupSession(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdAppendBundlesDetailsIncrementalBackupSession(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdPublishIncrementalFile(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdPublishSAIncrementalFile(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdAppIncrementalFileReady(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdAppIncrementalDone(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdGetIncrementalFileHandle(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdGetBackupInfo(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdUpdateTimer(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdUpdateSendRate(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdStopExtTimer(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdRefreshDataSize(MessageParcel&, MessageParcel&) = 0;
    virtual int32_t CmdGetBackupDataSize(MessageParcel&, MessageParcel&) = 0;
    virtual void ServiceStubSupplement() = 0;
    virtual void ServiceStubSuppAppendBundles() = 0;

public:
    BServiceStub() = default;
    virtual ~BServiceStub() = default;
public:
    static inline std::shared_ptr<BServiceStub> stub = nullptr;
};

class ServiceStubMock : public BServiceStub {
public:
    MOCK_METHOD(int32_t, OnRemoteRequest, (uint32_t, MessageParcel&, MessageParcel&, MessageOption&));
    MOCK_METHOD(int32_t, CmdInitRestoreSession, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdInitBackupSession, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdStart, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdGetLocalCapabilities, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdGetLocalCapabilitiesForBdInfos, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdPublishFile, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdAppFileReady, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdAppDone, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdResultReport, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdGetFileHandle, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdAppendBundlesRestoreSession, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdAppendBundlesDetailsRestoreSession, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdAppendBundlesBackupSession, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdAppendBundlesDetailsBackupSession, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdFinish, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdRelease, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdGetLocalCapabilitiesIncremental, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdGetAppLocalListAndDoIncrementalBackup, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdInitIncrementalBackupSession, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdAppendBundlesIncrementalBackupSession, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdAppendBundlesDetailsIncrementalBackupSession, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdPublishIncrementalFile, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdPublishSAIncrementalFile, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdAppIncrementalFileReady, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdAppIncrementalDone, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdGetIncrementalFileHandle, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdGetBackupInfo, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdUpdateTimer, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdUpdateSendRate, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdStopExtTimer, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdRefreshDataSize, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(int32_t, CmdGetBackupDataSize, (MessageParcel&, MessageParcel&));
    MOCK_METHOD(void, ServiceStubSupplement, ());
    MOCK_METHOD(void, ServiceStubSuppAppendBundles, ());
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_SERVICE_STUB_MOCK_H
