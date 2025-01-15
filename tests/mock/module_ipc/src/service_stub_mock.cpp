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

#include "module_ipc/service_stub.h"
#include "service_stub_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ServiceStub::ServiceStub()
{
}

void ServiceStub::ServiceStubSupplement()
{
}

int32_t ServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    return BServiceStub::stub->OnRemoteRequest(code, data, reply, option);
}

int32_t ServiceStub::CmdInitRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdInitRestoreSession(data, reply);
}

int32_t ServiceStub::CmdInitBackupSession(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdInitBackupSession(data, reply);
}

int32_t ServiceStub::CmdStart(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdStart(data, reply);
}

int32_t ServiceStub::CmdGetLocalCapabilities(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdGetLocalCapabilities(data, reply);
}

int32_t ServiceStub::CmdPublishFile(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdPublishFile(data, reply);
}

int32_t ServiceStub::CmdAppFileReady(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdAppFileReady(data, reply);
}

int32_t ServiceStub::CmdAppDone(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdAppDone(data, reply);
}

int32_t ServiceStub::CmdResultReport(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdResultReport(data, reply);
}

int32_t ServiceStub::CmdGetFileHandle(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdGetFileHandle(data, reply);
}

int32_t ServiceStub::CmdAppendBundlesRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdAppendBundlesRestoreSession(data, reply);
}

int32_t ServiceStub::CmdAppendBundlesDetailsRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdAppendBundlesDetailsRestoreSession(data, reply);
}

int32_t ServiceStub::CmdAppendBundlesBackupSession(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdAppendBundlesBackupSession(data, reply);
}

int32_t ServiceStub::CmdFinish(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdFinish(data, reply);
}

int32_t ServiceStub::CmdGetBackupInfo(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdGetBackupInfo(data, reply);
}

int32_t ServiceStub::CmdUpdateTimer(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdUpdateTimer(data, reply);
}

int32_t ServiceStub::CmdUpdateSendRate(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdUpdateSendRate(data, reply);
}

int32_t ServiceStub::CmdRelease(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdRelease(data, reply);
}

int32_t ServiceStub::CmdCancel(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdCancel(data, reply);
}

int32_t ServiceStub::CmdGetLocalCapabilitiesIncremental(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdGetLocalCapabilitiesIncremental(data, reply);
}

int32_t ServiceStub::CmdGetAppLocalListAndDoIncrementalBackup(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdGetAppLocalListAndDoIncrementalBackup(data, reply);
}

int32_t ServiceStub::CmdInitIncrementalBackupSession(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdInitIncrementalBackupSession(data, reply);
}

int32_t ServiceStub::CmdAppendBundlesIncrementalBackupSession(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdAppendBundlesIncrementalBackupSession(data, reply);
}

int32_t ServiceStub::CmdAppendBundlesDetailsIncrementalBackupSession(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdAppendBundlesDetailsIncrementalBackupSession(data, reply);
}

int32_t ServiceStub::CmdPublishIncrementalFile(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdPublishIncrementalFile(data, reply);
}

int32_t ServiceStub::CmdAppIncrementalFileReady(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdAppIncrementalFileReady(data, reply);
}

int32_t ServiceStub::CmdGetIncrementalFileHandle(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdGetIncrementalFileHandle(data, reply);
}

int32_t ServiceStub::CmdStopExtTimer(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdStopExtTimer(data, reply);
}

int32_t ServiceStub::CmdRefreshDataSize(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdRefreshDataSize(data, reply);
}

int32_t ServiceStub::CmdGetLocalCapabilitiesForBdInfos(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdGetLocalCapabilitiesForBdInfos(data, reply);
}

int32_t ServiceStub::CmdGetBackupDataSize(MessageParcel &data, MessageParcel &reply)
{
    return BServiceStub::stub->CmdGetBackupDataSize(data, reply);
}
} // namespace OHOS::FileManagement::Backup
