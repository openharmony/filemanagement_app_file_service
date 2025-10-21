/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "extension_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ErrCode ExtensionProxy::GetFileHandleWithUniqueFd(const string &fileName, int32_t &errCode, int &fd)
{
    return 0;
}

ErrCode ExtensionProxy::HandleClear()
{
    return 0;
}

ErrCode ExtensionProxy::HandleBackup(bool isClearData)
{
    return 0;
}

ErrCode ExtensionProxy::PublishFile(const string &fileName)
{
    return 0;
}

ErrCode ExtensionProxy::HandleRestore(bool isClearData)
{
    return 0;
}

ErrCode ExtensionProxy::GetBackupInfo(std::string &result)
{
    return 0;
}

ErrCode ExtensionProxy::GetIncrementalFileHandle(const string &fileName,
                                                 int &fd, int &reportFd, int32_t &fdErrCode)
{
    return 0;
}

ErrCode ExtensionProxy::PublishIncrementalFile(const string &fileName)
{
    return 0;
}

ErrCode ExtensionProxy::HandleIncrementalBackup(int incrementalFd, int manifestFd)
{
    return 0;
}

ErrCode ExtensionProxy::IncrementalOnBackup(bool isClearData)
{
    return 0;
}

ErrCode ExtensionProxy::UpdateFdSendRate(const std::string &bundleName, int32_t sendRate)
{
    return 0;
}

ErrCode ExtensionProxy::User0OnBackup()
{
    return 0;
}

ErrCode ExtensionProxy::GetIncrementalBackupFileHandle(int &fd, int &reportFd)
{
    return 0;
}

ErrCode ExtensionProxy::UpdateDfxInfo(int64_t uniqId, uint32_t extConnectSpend, const std::string &bundleName)
{
    return 0;
}

ErrCode ExtensionProxy::CleanBundleTempDir()
{
    return 0;
}

ErrCode ExtensionProxy::HandleOnRelease(int32_t scenario)
{
    return 0;
}

ErrCode ExtensionProxy::HandleGetCompatibilityInfo(const std::string &extInfo, int32_t scenario, bool isExist,
    std::string &compatibilityInfo)
{
    return 0;
}
} // namespace OHOS::FileManagement::Backup