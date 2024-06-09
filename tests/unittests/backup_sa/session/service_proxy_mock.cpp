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

#include "b_error/b_error.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

int32_t ServiceProxy::InitRestoreSession(sptr<IServiceReverse> remote)
{
    return 0;
}

int32_t ServiceProxy::InitBackupSession(sptr<IServiceReverse> remote)
{
    return 0;
}

ErrCode ServiceProxy::Start()
{
    return BError(BError::Codes::OK);
}

UniqueFd ServiceProxy::GetLocalCapabilities()
{
    return UniqueFd(-1);
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

ErrCode ServiceProxy::ServiceResultReport(const std::string restoreRetInfo, BackupRestoreScenario scenario)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::GetFileHandle(const string &bundleName, const string &fileName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppendBundlesRestoreSession(UniqueFd fd, const vector<BundleName> &bundleNames,
    const vector<std::string> &detailInfos, RestoreTypeEnum restoreType, int32_t userId)
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

ErrCode ServiceProxy::AppendBundlesBackupSession(const vector<BundleName> &bundleNames)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppendBundlesDetailsBackupSession(const std::vector<BundleName>&, const std::vector<std::string>&)
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
    const std::vector<std::string> &infos)
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

ErrCode ServiceProxy::UpdateTimer(BundleName &bundleName, uint32_t timeOut, bool &result)
{
    return BError(BError::Codes::OK);
}

sptr<IService> ServiceProxy::GetInstance()
{
    return serviceProxy_;
}

void ServiceProxy::InvaildInstance() {}

void ServiceProxy::ServiceProxyLoadCallback::OnLoadSystemAbilitySuccess(int32_t, const OHOS::sptr<IRemoteObject>&) {}

void ServiceProxy::ServiceProxyLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId) {}
} // namespace OHOS::FileManagement::Backup