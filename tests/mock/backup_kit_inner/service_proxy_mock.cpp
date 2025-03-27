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

int32_t ServiceProxy::InitRestoreSession(const sptr<IServiceReverse>& remote)
{
    if (!GetMockInitBackupOrRestoreSession()) {
        return 1;
    }
    return 0;
}

int32_t ServiceProxy::InitRestoreSessionWithErrMsg(const sptr<IServiceReverse>& remote, std::string &errMsg)
{
    if (!GetMockInitBackupOrRestoreSession()) {
        return 1;
    }
    return 0;
}

int32_t ServiceProxy::InitBackupSession(const sptr<IServiceReverse>& remote)
{
    if (!GetMockInitBackupOrRestoreSession()) {
        return 1;
    }
    return 0;
}

int32_t ServiceProxy::InitBackupSessionWithErrMsg(const sptr<IServiceReverse>& remote, std::string &errMsg)
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

ErrCode ServiceProxy::GetLocalCapabilities(int &fd)
{
    TestManager tm("ServiceProxyMock_GetFd_0100");
    string filePath = tm.GetRootDirCurTest().append("tmp");
    UniqueFd fd_OpenData(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
    fd = fd_OpenData.Release();
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::PublishFile(const BFileInfo &fileInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppFileReady(const string &fileName, int fd, int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppDone(ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::ServiceResultReport(const std::string& restoreRetInfo,
    BackupRestoreScenario scenario, ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::GetFileHandle(const string &bundleName, const string &fileName)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppendBundlesRestoreSessionDataByDetail(int fd,
                                                              const std::vector<std::string> &bundleNames,
                                                              const std::vector<std::string> &detailInfos,
                                                              int32_t restoreType,
                                                              int32_t userId)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppendBundlesRestoreSessionData(int fd,
                                                      const std::vector<std::string> &bundleNames,
                                                      int32_t restoreType,
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

ErrCode ServiceProxy::Cancel(const std::string &bundleName, int32_t &result)
{
    result = BError(BError::Codes::OK);
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames, int &fd)
{
    fd = UniqueFd(-1).Get();
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::GetAppLocalListAndDoIncrementalBackup()
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::InitIncrementalBackupSession(const sptr<IServiceReverse>& remote)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::InitIncrementalBackupSessionWithErrMsg(const sptr<IServiceReverse> &remote, std::string &errMsg)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppendBundlesIncrementalBackupSessionWithBundleInfos(
    const std::vector<BIncrementalData> &bundlesToBackup,
    const std::vector<std::string> &bundleInfos)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::PublishIncrementalFile(const BFileInfo &fileInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::PublishSAIncrementalFile(const BFileInfo &fileInfo, int fd)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppIncrementalFileReady(const string &fileName, int fd, int manifestFd, int32_t errCode)
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

ErrCode ServiceProxy::GetBackupInfo(const std::string &bundleName, std::string &result)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::UpdateTimer(const BundleName &bundleName, uint32_t timeout, bool &result)
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

ErrCode ServiceProxy::UpdateSendRate(const std::string &bundleName, int32_t sendRate, bool &result)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::ReportAppProcessInfo(const std::string& processInfo, BackupRestoreScenario scenario)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::GetLocalCapabilitiesForBundleInfos(int& fd)
{
    fd=-1;
    return fd;
}

ErrCode ServiceProxy::GetBackupDataSize(bool isPreciseScan, const vector<BIncrementalData>& bundleNameList)
{
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup