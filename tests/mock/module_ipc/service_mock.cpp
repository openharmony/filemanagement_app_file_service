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

int32_t Service::GetUserIdDefault()
{
    return 0;
}

void Service::OnStart() {}

void Service::OnStop() {}

ErrCode Service::GetLocalCapabilitiesForBundleInfos(int &fd)
{
    return BError(BError::Codes::OK);
}
ErrCode Service::InitBackupSessionWithErrMsg(const sptr<IServiceReverse> &remote, std::string &errMsg)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::InitRestoreSessionWithErrMsg(const sptr<IServiceReverse> &reverseIpcRemoteObject, std::string &errMsg)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesRestoreSessionDataByDetail(int fd,
                                                         const std::vector<std::string> &bundleNames,
                                                         const std::vector<std::string> &detailInfos,
                                                         int32_t restoreType,
                                                         int32_t userId)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesRestoreSessionData(int fd,
                                                 const std::vector<std::string> &bundleNames,
                                                 int32_t restoreType,
                                                 int32_t userId)
{
    return BError(BError::Codes::OK);
}
ErrCode Service::AppendBundlesIncrementalBackupSessionWithBundleInfos(
    const std::vector<BIncrementalData> &bundlesToBackup,
    const std::vector<std::string> &bundleInfos)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::GetLocalCapabilities(int &fd)
{
    fd = -1;
    return BError(BError::Codes::OK);
}

UniqueFd Service::GetLocalCapabilities()
{
    return UniqueFd(-1);
}

UniqueFd Service::GetLocalCapabilitiesForBundleInfos()
{
    return UniqueFd(-1);
}

void Service::StopAll(const wptr<IRemoteObject> &obj, bool force) {}

ErrCode Service::VerifyCallerAndGetCallerName(std::string &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::InitRestoreSession(const sptr<IServiceReverse> &remote)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::InitBackupSession(const sptr<IServiceReverse> &remote)
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

ErrCode Service::AppFileReady(const string &fileName, int fd, int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppDone(ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::ServiceResultReport(const std::string &restoreRetInfo, BackupRestoreScenario sennario, ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd,
                                             const std::vector<BundleName> &bundleNames,
                                             const std::vector<std::string> &detailInfos,
                                             RestoreTypeEnum restoreType,
                                             int32_t userId)
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

ErrCode Service::AppendBundlesDetailsBackupSession(const std::vector<BundleName> &bundleNames,
                                                   const std::vector<std::string> &bundleInfos)
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

ErrCode Service::LaunchBackupSAExtension(const BundleName &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::GetFileHandle(const string &bundleName, const string &fileName)
{
    return BError(BError::Codes::OK);
}

void Service::OnBackupExtensionDied(const string &&bundleName, bool isCleanCalled) {}

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

ErrCode Service::VerifyCaller()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::VerifyCaller(IServiceReverse::Scenario scenario)
{
    return BError(BError::Codes::OK);
}

void Service::OnAllBundlesFinished(ErrCode errCode) {}

void Service::OnStartSched() {}

void Service::SendStartAppGalleryNotify(const BundleName &bundleName) {}

void Service::SessionDeactive() {}

ErrCode Service::Release()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::Cancel(const std::string &bundleName, int32_t &result)
{
    result = BError(BError::Codes::OK);
    return BError(BError::Codes::OK);
}

ErrCode Service::GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames, int &fd)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::GetAppLocalListAndDoIncrementalBackup()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::InitIncrementalBackupSession(const sptr<IServiceReverse> &remote)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::InitIncrementalBackupSessionWithErrMsg(const sptr<IServiceReverse> &remote, std::string &errMsg)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup,
                                                       const std::vector<std::string> &infos)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishIncrementalFile(const BFileInfo &fileInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishSAIncrementalFile(const BFileInfo &fileInfo, int fd)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppIncrementalFileReady(const std::string &fileName,
                                         int fd,
                                         int manifestFd,
                                         int32_t appIncrementalFileReadyErrCode)
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

ErrCode Service::GetBackupInfo(const BundleName &bundleName, std::string &result)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::StartExtTimer(bool &isExtStart)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::StartFwkTimer(bool &isFwkStart)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::StopExtTimer(bool &isExtStop)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::RefreshDataSize(int64_t totalDatasize)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::UpdateTimer(const BundleName &bundleName, uint32_t timeout, bool &result)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::UpdateSendRate(const std::string &bundleName, int32_t sendRate, bool &result)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::ReportAppProcessInfo(const std::string &processInfo, const BackupRestoreScenario sennario)
{
    return BError(BError::Codes::OK);
}

void Service::OnSABackup(const std::string &bundleName,
                         const int &fd,
                         const std::string &result,
                         const ErrCode &errCode)
{
}

void Service::OnSARestore(const std::string &bundleName, const std::string &result, const ErrCode &errCode) {}

ErrCode Service::ClearResidualBundleData(const std::string &bundleName)
{
    return BError(BError::Codes::OK);
}

std::shared_ptr<ExtensionMutexInfo> Service::GetExtensionMutex(const BundleName &bundleName)
{
    return make_shared<ExtensionMutexInfo>(bundleName);
}

void Service::RemoveExtensionMutex(const BundleName &bundleName) {}

void Service::OnBundleStarted(BError error, sptr<SvcSessionManager> session, const BundleName &bundleName) {}

void Service::HandleExceptionOnAppendBundles(sptr<SvcSessionManager> session,
                                             const vector<BundleName> &appendBundleNames,
                                             const vector<BundleName> &restoreBundleNames)
{
}

void Service::BundleBeginRadarReport(const std::string &bundleName,
                                     const ErrCode errCode,
                                     const IServiceReverse::Scenario scenario)
{
}

void Service::BundleEndRadarReport(const std::string &bundleName,
                                   ErrCode errCode,
                                   const IServiceReverse::Scenario scenario)
{
}

void Service::FileReadyRadarReport(const std::string &bundleName,
                                   const std::string &fileName,
                                   const ErrCode errCode,
                                   const IServiceReverse::Scenario scenario)
{
}

void Service::ExtensionConnectFailRadarReport(const std::string &bundleName,
                                              const ErrCode errCode,
                                              const IServiceReverse::Scenario scenario)
{
}

void Service::PermissionCheckFailRadar(const std::string &info, const std::string &func) {}

void Service::OnStartResRadarReport(const std::vector<std::string> &bundleNameList, int32_t stage) {}

std::string Service::GetCallerName()
{
    return "";
}

bool Service::IsReportBundleExecFail(const std::string &bundleName)
{
    return true;
}

void Service::ClearBundleRadarReport() {}

void Service::UpdateBundleRadarReport(const std::string &bundleName) {}

bool Service::IsReportFileReadyFail(const std::string &bundleName)
{
    return true;
}

void Service::ClearFileReadyRadarReport() {}

void Service::UpdateFailedBundles(const std::string &bundleName, BundleTaskInfo taskInfo) {}

void Service::ClearFailedBundles() {}

void Service::GetOldDeviceBackupVersion() {}

void Service::CreateDirIfNotExist(const std::string &path) {}

std::vector<std::string> Service::GetSupportBackupBundleNames(vector<BJsonEntityCaps::BundleInfo> &,
                                                              bool,
                                                              const vector<string> &)
{
    return {};
}

void Service::StartRunningTimer(const std::string &bundleName) {}

void Service::HandleNotSupportBundleNames(const vector<string> &, vector<string> &, bool) {}

void Service::SetBundleIncDataInfo(const std::vector<BIncrementalData> &, std::vector<std::string> &) {}

void Service::CancelTask(std::string bundleName, wptr<Service> ptr) {}

void SetUserIdAndRestoreType(RestoreTypeEnum restoreType, int32_t userId) {}

void Service::CallOnBundleEndByScenario(const std::string &bundleName, BackupRestoreScenario scenario, ErrCode errCode)
{
}

void Service::SetUserIdAndRestoreType(RestoreTypeEnum restoreType, int32_t userId) {}

ErrCode Service::GetBackupDataSize(bool isPreciseScan, const std::vector<BIncrementalData> &bundleNameList)
{
    return BError(BError::Codes::OK);
}

void Service::GetDataSizeStepByStep(bool isPreciseScan, vector<BIncrementalData> bundleNameList, string &scanning) {}

void Service::GetPresumablySize(vector<BIncrementalData> bundleNameList, string &scanning) {}

void Service::GetPrecisesSize(vector<BIncrementalData> bundleNameList, string &scanning) {}

void Service::WriteToList(BJsonUtil::BundleDataSize bundleDataSize) {}

void Service::DeleteFromList(size_t scannedSize) {}

void Service::WriteScannedInfoToList(const string &bundleName, int64_t dataSize, int64_t incDataSize) {}

void Service::SendScannedInfo(const string &scannendInfos, sptr<SvcSessionManager> session) {}

void Service::CyclicSendScannedInfo(bool isPreciseScan, vector<BIncrementalData> bundleNameList) {}

bool Service::GetScanningInfo(wptr<Service> obj, size_t scannedSize, string &scanning)
{
    return true;
}

void Service::SetScanningInfo(string &scanning, string name) {}
} // namespace OHOS::FileManagement::Backup
