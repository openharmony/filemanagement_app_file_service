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

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_STUB_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_STUB_H

#include <map>

#include "i_service_reverse.h"
#include "iremote_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceReverseStub : public IRemoteStub<IServiceReverse> {
public:
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

public:
    ServiceReverseStub();
    ~ServiceReverseStub() override = default;

private:
    using ServiceInterface = int32_t (ServiceReverseStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, ServiceInterface> opToInterfaceMap_;

    int32_t CmdBackupOnFileReady(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBackupOnBundleStarted(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBackupOnBundleFinished(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBackupOnAllBundlesFinished(MessageParcel &data, MessageParcel &reply);

    int32_t CmdRestoreOnBundleStarted(MessageParcel &data, MessageParcel &reply);
    int32_t CmdRestoreOnBundleFinished(MessageParcel &data, MessageParcel &reply);
    int32_t CmdRestoreOnAllBundlesFinished(MessageParcel &data, MessageParcel &reply);
    int32_t CmdRestoreOnFileReady(MessageParcel &data, MessageParcel &reply);
    int32_t CmdRestoreOnResultReport(MessageParcel &data, MessageParcel &reply);

    int32_t CmdIncrementalBackupOnFileReady(MessageParcel &data, MessageParcel &reply);
    int32_t CmdIncrementalBackupOnBundleStarted(MessageParcel &data, MessageParcel &reply);
    int32_t CmdIncrementalBackupOnBundleFinished(MessageParcel &data, MessageParcel &reply);
    int32_t CmdIncrementalBackupOnAllBundlesFinished(MessageParcel &data, MessageParcel &reply);

    int32_t CmdIncrementalRestoreOnBundleStarted(MessageParcel &data, MessageParcel &reply);
    int32_t CmdIncrementalRestoreOnBundleFinished(MessageParcel &data, MessageParcel &reply);
    int32_t CmdIncrementalRestoreOnAllBundlesFinished(MessageParcel &data, MessageParcel &reply);
    int32_t CmdIncrementalRestoreOnFileReady(MessageParcel &data, MessageParcel &reply);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_STUB_H
