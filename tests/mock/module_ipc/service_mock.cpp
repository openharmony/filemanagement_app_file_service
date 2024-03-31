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

#include "module_ipc/service.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void Service::OnStart() {}

void Service::OnStop() {}

UniqueFd Service::GetLocalCapabilities()
{
    return UniqueFd(-1);
}

void Service::StopAll(const wptr<IRemoteObject> &obj, bool force) {}

string Service::VerifyCallerAndGetCallerName()
{
    return "";
}

ErrCode Service::InitRestoreSession(sptr<IServiceReverse> remote)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::InitBackupSession(sptr<IServiceReverse> remote)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::Start()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppFileReady(const string &fileName, UniqueFd fd)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppDone(ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::ServiceResultReport(const std::string &restoreRetInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesDetailsRestoreSession(UniqueFd fd, const std::vector<BundleName> &bundleNames,
    const std::vector<std::string> &detailInfos, RestoreTypeEnum restoreType, int32_t userId)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd,
                                             const std::vector<BundleName> &bundleNames,
                                             RestoreTypeEnum restoreType,
                                             int32_t userId)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesBackupSession(const std::vector<BundleName> &bundleNames)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::Finish()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::LaunchBackupExtension(const BundleName &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::GetFileHandle(const string &bundleName, const string &fileName)
{
    return BError(BError::Codes::OK);
}

void Service::OnBackupExtensionDied(const string &&bundleName) {}

void Service::ExtConnectDied(const string &callName) {}

void Service::ExtStart(const string &bundleName) {}

int Service::Dump(int fd, const vector<u16string> &args)
{
    return 0;
}

void Service::ExtConnectFailed(const string &bundleName, ErrCode ret)
{
    GTEST_LOG_(INFO) << "ExtConnectFailed is OK";
}

void Service::ExtConnectDone(string bundleName) {}

void Service::ClearSessionAndSchedInfo(const string &bundleName) {}

void Service::VerifyCaller() {}

void Service::VerifyCaller(IServiceReverse::Scenario scenario) {}

void Service::OnAllBundlesFinished(ErrCode errCode) {}

void Service::OnStartSched() {}

void Service::SendAppGalleryNotify(const BundleName &bundleName) {}

void Service::SessionDeactive() {}

ErrCode Service::Release()
{
    return BError(BError::Codes::OK);
}

UniqueFd Service::GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames)
{
    return UniqueFd(-1);
}

ErrCode Service::InitIncrementalBackupSession(sptr<IServiceReverse> remote)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishIncrementalFile(const BFileInfo &fileInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppIncrementalFileReady(const string &fileName, UniqueFd fd, UniqueFd manifestFd)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppIncrementalDone(ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::GetIncrementalFileHandle(const string &bundleName, const string &fileName)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::GetBackupInfo(BundleName &bundleName, std::string &result)
{
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup
