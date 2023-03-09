/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_SVC_EXTENSION_PROXY_H
#define OHOS_FILEMGMT_BACKUP_SVC_EXTENSION_PROXY_H

#include "i_extension.h"
#include "iremote_proxy.h"

namespace OHOS::FileManagement::Backup {
class SvcExtensionProxy : public IRemoteProxy<IExtension> {
public:
    UniqueFd GetFileHandle(const std::string &fileName) override;
    ErrCode HandleClear() override;
    ErrCode HandleBackup() override;
    ErrCode PublishFile(const std::string &fileName) override;

public:
    explicit SvcExtensionProxy(const sptr<IRemoteObject> &remote) : IRemoteProxy<IExtension>(remote) {}

private:
    static inline BrokerDelegator<SvcExtensionProxy> delegator_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SVC_EXTENSION_PROXY_H