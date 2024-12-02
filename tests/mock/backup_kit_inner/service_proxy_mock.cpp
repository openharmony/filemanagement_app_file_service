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

#include "service_proxy.h"

#include <cstddef>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "iremote_object_mock.h"
#include "test_manager.h"
#include "utils_mock_global_variable.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

int32_t ServiceProxy::InitRestoreSession(sptr<IServiceReverse> remote)
{
    if (!GetMockInitBackupOrRestoreSession()) {
        return 1;
    }
    return 0;
}

int32_t ServiceProxy::InitBackupSession(sptr<IServiceReverse> remote)
{
    if (!GetMockInitBackupOrRestoreSession()) {
        return 1;
    }
    return 0;
}

ErrCode ServiceProxy::Start()
{
    return BError(BError::Codes::OK);
}

UniqueFd ServiceProxy::GetLocalCapabilities()
{
    TestManager tm("ServiceProxyMock_GetFd_0100");
    string filePath = tm.GetRootDirCurTest().append("tmp");
    UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
    return fd;
}

ErrCode ServiceProxy::PublishFile(const BFileInfo &fileInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppFileReady(const string &fileName, UniqueFd fd, int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppDone(ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::ServiceResultReport(const std::string restoreRetInfo,
    BackupRestoreScenario scenario, ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::GetFileHandle(const string &bundleName, const string &fileName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppendBundlesRestoreSession(UniqueFd fd,
                                                  const vector<BundleName> &bundleNames,
                                                  RestoreTypeEnum restoreType,
                                                  int32_t userId)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppendBundlesRestoreSession(UniqueFd fd,
                                                  const vector<BundleName> &bundleNames,
                                                  const vector<std::string> &detailInfos,
                                                  RestoreTypeEnum restoreType,
                                                  int32_t userId)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppendBundlesBackupSession(const vector<BundleName> &bundleNames)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppendBundlesDetailsBackupSession(const std::vector<BundleName> &bundleNames,
    const std::vector<std::string> &detailInfos)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::Finish()
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::Release()
{
    return BError(BError::Codes::OK);
}

UniqueFd ServiceProxy::GetLocalCapabilitiesIncremental(const vector<BIncrementalData> &bundleNames)
{
    return UniqueFd(-1);
}

ErrCode ServiceProxy::GetAppLocalListAndDoIncrementalBackup()
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::InitIncrementalBackupSession(sptr<IServiceReverse> remote)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppendBundlesIncrementalBackupSession(const vector<BIncrementalData> &bundlesToBackup)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppendBundlesIncrementalBackupSession(const vector<BIncrementalData> &bundlesToBackup,
    const vector<std::string> &infos)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::PublishIncrementalFile(const BFileInfo &fileInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::PublishSAIncrementalFile(const BFileInfo &fileInfo, UniqueFd fd)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppIncrementalFileReady(const string &fileName, UniqueFd fd, UniqueFd manifestFd, int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppIncrementalDone(ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::GetIncrementalFileHandle(const std::string &bundleName, const std::string &fileName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::GetBackupInfo(std::string &bundleName, std::string &result)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::UpdateTimer(BundleName &bundleName, uint32_t timeout, bool &result)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::StartExtTimer(bool &isExtStart)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::StartFwkTimer(bool &isFwkStart)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::StopExtTimer(bool &isExtStop)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::RefreshDataSize(int64_t totalSize)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::UpdateSendRate(std::string &bundleName, int32_t sendRate, bool &result)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::ReportAppProcessInfo(const std::string processInfo, const BackupRestoreScenario sennario)
{
    return BError(BError::Codes::OK);
}

sptr<IService> ServiceProxy::GetServiceProxyPointer()
{
    return serviceProxy_;
}

sptr<IService> ServiceProxy::GetInstance()
{
    if (!GetMockGetInstance()) {
        return nullptr;
    }

    if (!GetMockLoadSystemAbility()) {
        serviceProxy_ = sptr(new ServiceProxy(nullptr));
    } else {
        sptr<IRemoteObject> object = new MockIRemoteObject();
        serviceProxy_ = sptr(new ServiceProxy(object));
    }
    return serviceProxy_;
}

void ServiceProxy::InvaildInstance()
{
    serviceProxy_ = nullptr;
}

void ServiceProxy::ServiceProxyLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
                                                                        const OHOS::sptr<IRemoteObject> &remoteObject)
{
    return;
}

void ServiceProxy::ServiceProxyLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    return;
}
} // namespace OHOS::FileManagement::Backup