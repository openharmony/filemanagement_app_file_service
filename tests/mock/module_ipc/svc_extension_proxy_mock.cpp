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

#include "module_ipc/svc_extension_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

UniqueFd SvcExtensionProxy::GetFileHandle(const string &fileName, int32_t &errCode)
{
    return UniqueFd(-1);
}

ErrCode SvcExtensionProxy::HandleClear()
{
    return 0;
}

ErrCode SvcExtensionProxy::HandleBackup(bool isClearData)
{
    return 0;
}

ErrCode SvcExtensionProxy::PublishFile(const string &fileName)
{
    return 0;
}

ErrCode SvcExtensionProxy::HandleRestore(bool isClearData)
{
    return 0;
}

ErrCode SvcExtensionProxy::GetBackupInfo(std::string &result)
{
    return 0;
}

ErrCode SvcExtensionProxy::GetIncrementalFileHandle(const string &fileName)
{
    return 0;
}

ErrCode SvcExtensionProxy::PublishIncrementalFile(const string &fileName)
{
    return 0;
}

ErrCode SvcExtensionProxy::HandleIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd)
{
    return 0;
}

ErrCode SvcExtensionProxy::IncrementalOnBackup(bool isClearData)
{
    return 0;
}

ErrCode SvcExtensionProxy::UpdateFdSendRate(std::string &bundleName, int32_t sendRate)
{
    return 0;
}

ErrCode SvcExtensionProxy::User0OnBackup()
{
    return 0;
}

tuple<UniqueFd, UniqueFd> SvcExtensionProxy::GetIncrementalBackupFileHandle()
{
    return {UniqueFd(-1), UniqueFd(-1)};
}
} // namespace OHOS::FileManagement::Backup