/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include <dlfcn.h>
#include <gtest/gtest.h>

#include "backup_para_mock.h"
#include "bms_adapter_mock.h"
#include "b_json/b_json_entity_caps.h"
#include "b_jsonutil_mock.h"
#include "direct_ex_mock.h"
#include "dlfcn_mock.h"
#include "ipc_skeleton_mock.h"
#ifdef POWER_MANAGER_ENABLED
#include "power_mgr_client.h"
#include "running_lock.h"
#endif
#include "sa_backup_connection_mock.h"
#include "service_reverse_proxy_mock.h"
#include "svc_backup_connection_mock.h"
#include "svc_extension_proxy_mock.h"
#include "svc_session_manager_mock.h"

#include "service_incremental.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace OHOS::AppFileService;

class BService {
public:
    virtual ErrCode GetLocalCapabilities(int& fd) = 0;
    virtual ErrCode VerifyCallerAndGetCallerName(std::string&) = 0;
    virtual ErrCode InitRestoreSession(const sptr<IServiceReverse>&) = 0;
    virtual ErrCode InitRestoreSessionWithErrMsg(const sptr<IServiceReverse>&, int32_t&, std::string&) = 0;
    virtual ErrCode InitBackupSession(const sptr<IServiceReverse>&) = 0;
    virtual ErrCode InitBackupSessionWithErrMsg(const sptr<IServiceReverse>&, int32_t&, std::string&) = 0;
    virtual ErrCode Start() = 0;
    virtual ErrCode PublishFile(const BFileInfo&) = 0;
    virtual ErrCode AppFileReady(const std::string &, int, int32_t) = 0;
    virtual ErrCode AppFileReadyWithoutFd(const std::string &, int32_t) = 0;
    virtual ErrCode AppDone(ErrCode) = 0;
    virtual ErrCode ServiceResultReport(const std::string, BackupRestoreScenario, ErrCode) = 0;
    virtual ErrCode AppendBundlesRestoreSessionDataByDetail(int, const std::vector<std::string>&,
                                                            const std::vector<std::string>&, int32_t, int32_t) = 0;
    virtual ErrCode AppendBundlesRestoreSessionData(int, const std::vector<std::string>&, int32_t, int32_t) = 0;
    virtual ErrCode AppendBundlesBackupSession(const std::vector<BundleName>&) = 0;
    virtual ErrCode AppendBundlesDetailsBackupSession(const std::vector<BundleName>&,
                                                      const std::vector<std::string>&) = 0;
    virtual ErrCode Finish() = 0;
    virtual ErrCode LaunchBackupExtension(const BundleName&) = 0;
    virtual ErrCode LaunchBackupSAExtension(const BundleName&) = 0;
    virtual ErrCode GetFileHandle(const string&, const string&) = 0;
    virtual int Dump(int, const vector<u16string>&) = 0;
    virtual ErrCode VerifyCaller() = 0;
    virtual ErrCode VerifyCaller(IServiceReverseType::Scenario) = 0;
    virtual int32_t GetUserIdDefault() = 0;
    virtual ErrCode GetBackupInfo(const BundleName&, std::string&) = 0;
    virtual ErrCode UpdateTimer(const BundleName&, uint32_t, bool&) = 0;
    virtual ErrCode UpdateSendRate(const std::string&, int32_t, bool&) = 0;
    virtual ErrCode ReportAppProcessInfo(const std::string, BackupRestoreScenario) = 0;
    virtual ErrCode StartExtTimer(bool&) = 0;
    virtual ErrCode StartFwkTimer(bool&) = 0;
    virtual ErrCode StopExtTimer(bool&) = 0;
    virtual ErrCode RefreshDataSize(int64_t) = 0;
    virtual std::string GetCallerName() = 0;
    virtual bool IsReportBundleExecFail(const std::string&) = 0;
    virtual bool IsReportFileReadyFail(const std::string&) = 0;
    virtual std::vector<std::string> GetSupportBackupBundleNames(vector<BJsonEntityCaps::BundleInfo>&, bool,
        const vector<string>&) = 0;
    virtual ErrCode HandleCurBundleFileReady(const std::string&, const std::string&, bool) = 0;
    virtual ErrCode HandleCurAppDone(ErrCode, const std::string&, bool) = 0;
    virtual UniqueFd GetLocalCapabilitiesForBundleInfos() = 0;
    virtual ErrCode GetBackupDataSize(bool, const std::vector<BIncrementalData>&) = 0;
    virtual ErrCode CleanBundleTempDir(const std::string& bundleName) = 0;
    virtual ErrCode HandleExtDisconnect(BackupRestoreScenario, bool, ErrCode) = 0;
    virtual ErrCode GetExtOnRelease(bool&) = 0;
    virtual void SetExtOnRelease(const BundleName&, bool) = 0;
    virtual void RemoveExtOnRelease(const BundleName&) = 0;
    virtual ErrCode GetCompatibilityInfo(const std::string&, const std::string&, std::string&) = 0;
    virtual void SetBroadCastInfoMap(const std::string &bundleName,
                                     const std::map<std::string, std::string> &broadCastInfoMap,
                                     int userId) = 0;
    virtual void BroadCastRestore(const std::string &bundleName, const std::string &broadCastType) = 0;
    virtual void BroadCastSingle(const std::string &bundleName, const std::string &broadCastType) = 0;
public:
    virtual bool UpdateToRestoreBundleMap(const string&, const string&) = 0;
#ifdef POWER_MANAGER_ENABLED
    virtual void RunningLockRadarReport(const std::string &func, const std::string &errMsg, ErrCode errCode) = 0;
#endif
    virtual void CreateRunningLock() = 0;
public:
    BService() = default;
    virtual ~BService() = default;
public:
    static inline std::shared_ptr<BService> serviceMock = nullptr;
};

class ServiceMock : public BService {
public:
    MOCK_METHOD(ErrCode, GetLocalCapabilities, (int&));
    MOCK_METHOD(ErrCode, VerifyCallerAndGetCallerName, (std::string&));
    MOCK_METHOD(ErrCode, InitRestoreSession, (const sptr<IServiceReverse>&));
    MOCK_METHOD(ErrCode, InitRestoreSessionWithErrMsg, (const sptr<IServiceReverse>&, int32_t &, std::string&));
    MOCK_METHOD(ErrCode, InitBackupSession, (const sptr<IServiceReverse>&));
    MOCK_METHOD(ErrCode, InitBackupSessionWithErrMsg, (const sptr<IServiceReverse>&, int32_t &, std::string&));
    MOCK_METHOD(ErrCode, Start, ());
    MOCK_METHOD(ErrCode, PublishFile, (const BFileInfo&));
    MOCK_METHOD(ErrCode, AppFileReady, (const string&, int, int32_t));
    MOCK_METHOD(ErrCode, AppFileReadyWithoutFd, (const string&, int32_t));
    MOCK_METHOD(ErrCode, AppDone, (ErrCode));
    MOCK_METHOD(ErrCode, ServiceResultReport, (const std::string, BackupRestoreScenario, ErrCode));
    MOCK_METHOD(ErrCode, AppendBundlesRestoreSessionDataByDetail, (int, (const std::vector<std::string>&),
        (const std::vector<std::string>&), int32_t, int32_t));
    MOCK_METHOD(ErrCode, AppendBundlesRestoreSessionData, (int, (const std::vector<BundleName>&), int32_t,
        int32_t));
    MOCK_METHOD(ErrCode, AppendBundlesBackupSession, (const std::vector<BundleName>&));
    MOCK_METHOD(ErrCode, AppendBundlesDetailsBackupSession, ((const std::vector<BundleName>&),
        (const std::vector<std::string>&)));
    MOCK_METHOD(ErrCode, Finish, ());
    MOCK_METHOD(ErrCode, LaunchBackupExtension, (const BundleName&));
    MOCK_METHOD(ErrCode, LaunchBackupSAExtension, (const BundleName&));
    MOCK_METHOD(ErrCode, GetFileHandle, (const string&, const string&));
    MOCK_METHOD(int, Dump, (int, const vector<u16string>&));
    MOCK_METHOD(ErrCode, VerifyCaller, ());
    MOCK_METHOD(ErrCode, VerifyCaller, (IServiceReverseType::Scenario));
    MOCK_METHOD(int32_t, GetUserIdDefault, ());
    MOCK_METHOD(ErrCode, GetBackupInfo, (const BundleName&, std::string&));
    MOCK_METHOD(ErrCode, UpdateTimer, (const BundleName&, uint32_t, bool&));
    MOCK_METHOD(ErrCode, UpdateSendRate, (const std::string&, int32_t, bool&));
    MOCK_METHOD(ErrCode, ReportAppProcessInfo, (const std::string, BackupRestoreScenario));
    MOCK_METHOD(ErrCode, StartExtTimer, (bool&));
    MOCK_METHOD(ErrCode, StartFwkTimer, (bool&));
    MOCK_METHOD(ErrCode, StopExtTimer, (bool&));
    MOCK_METHOD(ErrCode, RefreshDataSize, (int64_t));
    MOCK_METHOD(std::string, GetCallerName, ());
    MOCK_METHOD(bool, IsReportBundleExecFail, (const std::string&));
    MOCK_METHOD(bool, IsReportFileReadyFail, (const std::string&));
    MOCK_METHOD(std::vector<std::string>, GetSupportBackupBundleNames, ((vector<BJsonEntityCaps::BundleInfo>&), bool,
        (const vector<string>&)));
    MOCK_METHOD(ErrCode, HandleCurBundleFileReady, (const std::string&, const std::string&, bool));
    MOCK_METHOD(ErrCode, HandleCurAppDone, (ErrCode, const std::string&, bool));
    MOCK_METHOD(UniqueFd, GetLocalCapabilitiesForBundleInfos, ());
    MOCK_METHOD(ErrCode, GetBackupDataSize, (bool, const std::vector<BIncrementalData>&));
    MOCK_METHOD(ErrCode, CleanBundleTempDir, (const std::string&));
    MOCK_METHOD(ErrCode, HandleExtDisconnect, (BackupRestoreScenario, bool, ErrCode));
    MOCK_METHOD(ErrCode, GetExtOnRelease, (bool&));
    MOCK_METHOD(void, SetExtOnRelease, (const BundleName&, bool));
    MOCK_METHOD(void, RemoveExtOnRelease, (const BundleName&));
    MOCK_METHOD(ErrCode, GetCompatibilityInfo, (const std::string&, const std::string&, std::string&));
    MOCK_METHOD(void, SetBroadCastInfoMap, (const std::string &, (const std::map<std::string, std::string> &), int));
    MOCK_METHOD(void, BroadCastRestore, (const std::string &, const std::string &));
    MOCK_METHOD(void, BroadCastSingle, (const std::string &, const std::string &));
#ifdef POWER_MANAGER_ENABLED
    MOCK_METHOD(void, RunningLockRadarReport, (const std::string &, const std::string &, ErrCode));
#endif
    MOCK_METHOD(void, CreateRunningLock, ());
public:
    MOCK_METHOD(bool, UpdateToRestoreBundleMap, (const string&, const string&));
};

void Service::OnStart() {}

void Service::OnStop() {}

ErrCode Service::GetLocalCapabilitiesForBundleInfos(int &fd)
{
    return BError(BError::Codes::OK);
}
ErrCode Service::InitBackupSessionWithErrMsg(const sptr<IServiceReverse> &remote,
                                             int32_t& errCodeForMsg, std::string &errMsg)
{
    errCodeForMsg = BError(BError::Codes::OK);
    return BError(BError::Codes::OK);
}

ErrCode Service::GetLocalCapabilities(int &fd)
{
    return BService::serviceMock->GetLocalCapabilities(fd);
}

void Service::StopAll(const wptr<IRemoteObject>&, bool) {}

ErrCode Service::VerifyCallerAndGetCallerName(std::string &bundleName)
{
    return BService::serviceMock->VerifyCallerAndGetCallerName(bundleName);
}

ErrCode Service::InitRestoreSession(const sptr<IServiceReverse> &remote)
{
    return BService::serviceMock->InitRestoreSession(remote);
}

ErrCode Service::InitRestoreSessionWithErrMsg(const sptr<IServiceReverse> &remote,
                                              int32_t& errCodeForMsg, std::string &errMsg)
{
    return BService::serviceMock->InitRestoreSessionWithErrMsg(remote, errCodeForMsg, errMsg);
}

ErrCode Service::InitBackupSession(const sptr<IServiceReverse> &remote)
{
    return BService::serviceMock->InitBackupSession(remote);
}

ErrCode Service::Start()
{
    return BService::serviceMock->Start();
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    return BService::serviceMock->PublishFile(fileInfo);
}

ErrCode Service::AppFileReady(const std::string &fileName, int fd, int32_t appFileReadyErrCode)
{
    return BService::serviceMock->AppFileReady(fileName, fd, appFileReadyErrCode);
}

ErrCode Service::AppFileReadyWithoutFd(const std::string &fileName, int32_t appFileReadyErrCode)
{
    return BService::serviceMock->AppFileReadyWithoutFd(fileName, appFileReadyErrCode);
}

ErrCode Service::AppDone(int32_t appDoneErrCode)
{
    return BService::serviceMock->AppDone(appDoneErrCode);
}

ErrCode Service::ServiceResultReport(const std::string& restoreRetInfo,
    BackupRestoreScenario sennario, ErrCode errCode)
{
    return BService::serviceMock->ServiceResultReport(restoreRetInfo, sennario, errCode);
}

ErrCode Service::AppendBundlesRestoreSessionDataByDetail(int fd,
                                                         const std::vector<std::string> &bundleNames,
                                                         const std::vector<std::string> &detailInfos,
                                                         int32_t restoreType,
                                                         int32_t userId)
{
    return BService::serviceMock->AppendBundlesRestoreSessionDataByDetail(fd, bundleNames, detailInfos,
                                                                          restoreType, userId);
}

ErrCode Service::AppendBundlesRestoreSessionData(int fd,
                                                 const std::vector<std::string> &bundleNames,
                                                 int32_t restoreType,
                                                 int32_t userId)
{
    return BService::serviceMock->AppendBundlesRestoreSessionData(fd, bundleNames, restoreType, userId);
}

ErrCode Service::AppendBundlesBackupSession(const std::vector<BundleName> &bundleNames)
{
    return BService::serviceMock->AppendBundlesBackupSession(bundleNames);
}

ErrCode Service::AppendBundlesDetailsBackupSession(const std::vector<BundleName> &bundleNames,
                                                   const std::vector<std::string> &bundleInfos)
{
    return BService::serviceMock->AppendBundlesDetailsBackupSession(bundleNames, bundleInfos);
}

ErrCode Service::Finish()
{
    return BService::serviceMock->Finish();
}

ErrCode Service::LaunchBackupExtension(const BundleName &bundleName)
{
    return BService::serviceMock->LaunchBackupExtension(bundleName);
}

ErrCode Service::LaunchBackupSAExtension(const BundleName &bundleName)
{
    return BService::serviceMock->LaunchBackupSAExtension(bundleName);
}

ErrCode Service::GetFileHandle(const string &bundleName, const string &fileName)
{
    return BService::serviceMock->GetFileHandle(bundleName, fileName);
}

void Service::OnBackupExtensionDied(const string&&, bool) {}

void Service::ExtConnectDied(const string&) {}

void Service::ExtStart(const string&) {}

int Service::Dump(int fd, const vector<u16string> &args)
{
    return BService::serviceMock->Dump(fd, args);
}

void Service::ExtConnectFailed(const string&, ErrCode) {}

void Service::ExtConnectDone(string) {}

void Service::ClearSessionAndSchedInfo(const string&) {}

ErrCode Service::VerifyCaller()
{
    return BService::serviceMock->VerifyCaller();
}

ErrCode Service::VerifyCaller(IServiceReverseType::Scenario scenario)
{
    return BService::serviceMock->VerifyCaller(scenario);
}

int32_t Service::GetUserIdDefault()
{
    return BService::serviceMock->GetUserIdDefault();
}

void Service::OnAllBundlesFinished(ErrCode) {}

void Service::OnStartSched() {}

void Service::SendStartAppGalleryNotify(const BundleName&) {}

void Service::SessionDeactive() {}

ErrCode Service::GetBackupInfo(const BundleName &bundleName, std::string &result)
{
    return BService::serviceMock->GetBackupInfo(bundleName, result);
}

ErrCode Service::UpdateTimer(const BundleName &bundleName, uint32_t timeOut, bool &result)
{
    return BService::serviceMock->UpdateTimer(bundleName, timeOut, result);
}

ErrCode Service::UpdateSendRate(const std::string &bundleName, int32_t sendRate, bool &result)
{
    return BService::serviceMock->UpdateSendRate(bundleName, sendRate,  result);
}

void Service::OnSABackup(const std::string&, const int&, const std::string&, const ErrCode&) {}

void Service::OnSARestore(const std::string&, const std::string&, const ErrCode&) {}

void Service::NotifyCallerCurAppDone(ErrCode, const std::string&) {}

void Service::SendEndAppGalleryNotify(const BundleName&) {}

void Service::NoticeClientFinish(const string&, ErrCode) {}

ErrCode Service::ReportAppProcessInfo(const std::string &processInfo, BackupRestoreScenario sennario)
{
    return BService::serviceMock->ReportAppProcessInfo(processInfo, sennario);
}

ErrCode Service::StartExtTimer(bool &isExtStart)
{
    return BService::serviceMock->StartExtTimer(isExtStart);
}

ErrCode Service::StartFwkTimer(bool &isFwkStart)
{
    return BService::serviceMock->StartFwkTimer(isFwkStart);
}

ErrCode Service::StopExtTimer(bool &isExtStop)
{
    return BService::serviceMock->StopExtTimer(isExtStop);
}

ErrCode Service::RefreshDataSize(int64_t totalsize)
{
    return BService::serviceMock->RefreshDataSize(totalsize);
}

std::function<void()> Service::TimeOutCallback(wptr<Service> ptr, std::string bundleName)
{
    return []() {};
}

void Service::SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo>&, std::vector<std::string>&,
    RestoreTypeEnum, std::string &) {}

void Service::SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo>&, std::vector<std::string>&,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>>&, std::map<std::string, bool>&,
    RestoreTypeEnum, std::string &) {}

void Service::SetCurrentSessProperties(BJsonEntityCaps::BundleInfo&, std::map<std::string, bool>&,
    const std::string&) {}

void Service::ReleaseOnException() {}

bool SvcRestoreDepsManager::UpdateToRestoreBundleMap(const string &bundleName, const string &fileName)
{
    return BService::serviceMock->UpdateToRestoreBundleMap(bundleName, fileName);
}

void Service::OnBundleStarted(BError, sptr<SvcSessionManager>, const BundleName&) {}

void Service::HandleExceptionOnAppendBundles(sptr<SvcSessionManager>, const vector<BundleName>&,
    const vector<BundleName>&) {}

void Service::BundleBeginRadarReport(const std::string&, const ErrCode, const IServiceReverseType::Scenario) {}

void Service::BundleEndRadarReport(const std::string&, const ErrCode, const IServiceReverseType::Scenario) {}

void Service::FileReadyRadarReport(const std::string&, const std::string&, const ErrCode,
    const IServiceReverseType::Scenario) {}

void Service::ExtensionConnectFailRadarReport(const std::string&, const ErrCode, const IServiceReverseType::Scenario) {}

void Service::PermissionCheckFailRadar(const std::string&, const std::string&) {}

void Service::OnStartResRadarReport(const std::vector<std::string>&, int32_t) {}

std::string Service::GetCallerName()
{
    return BService::serviceMock->GetCallerName();
}

bool Service::IsReportBundleExecFail(const std::string &bundleName)
{
    return BService::serviceMock->IsReportBundleExecFail(bundleName);
}

void Service::ClearBundleRadarReport() {}

void Service::UpdateBundleRadarReport(const std::string&) {}

bool Service::IsReportFileReadyFail(const std::string &bundleName)
{
    return BService::serviceMock->IsReportFileReadyFail(bundleName);
}

void Service::ClearFileReadyRadarReport() {}

void Service::UpdateFailedBundles(const std::string&, BundleTaskInfo) {}

void Service::ClearFailedBundles() {}

void Service::StartRunningTimer(const std::string&) {}

std::vector<std::string> Service::GetSupportBackupBundleNames(vector<BJsonEntityCaps::BundleInfo> &bundleInfos,
    bool isIncBackup, const vector<std::string> &srcBundleNames)
{
    return BService::serviceMock->GetSupportBackupBundleNames(bundleInfos, isIncBackup, srcBundleNames);
}

ErrCode Service::HandleCurBundleFileReady(const std::string &bundleName, const std::string &fileName, bool isIncBackup)
{
    return BService::serviceMock->HandleCurBundleFileReady(bundleName, fileName, isIncBackup);
}

ErrCode Service::HandleCurAppDone(ErrCode errCode, const std::string &bundleName, bool isIncBackup)
{
    return BService::serviceMock->HandleCurAppDone(errCode, bundleName, isIncBackup);
}

void Service::StartCurBundleBackupOrRestore(const std::string&) {}

UniqueFd Service::GetLocalCapabilitiesForBundleInfos()
{
    return BService::serviceMock->GetLocalCapabilitiesForBundleInfos();
}

void Service::AppendBundles(const std::vector<std::string> &bundleNames) {}
ErrCode Service::GetBackupDataSize(bool isPreciseScan, const std::vector<BIncrementalData>& bundleNameList)
{
    return BService::serviceMock->GetBackupDataSize(isPreciseScan, bundleNameList);
}

ErrCode Service::CleanBundleTempDir(const std::string& bundleName)
{
    return BService::serviceMock->CleanBundleTempDir(bundleName);
}

ErrCode Service::HandleExtDisconnect(BackupRestoreScenario scenario, bool isAppResultReport, ErrCode errCode)
{
    return BService::serviceMock->HandleExtDisconnect(scenario, isAppResultReport, errCode);
}

ErrCode Service::GetExtOnRelease(bool &isExtOnRelease)
{
    return BService::serviceMock->GetExtOnRelease(isExtOnRelease);
}

void Service::SetExtOnRelease(const BundleName &bundleName, bool isOnRelease)
{
    return BService::serviceMock->SetExtOnRelease(bundleName, isOnRelease);
}

void Service::RemoveExtOnRelease(const BundleName &bundleName)
{
    return BService::serviceMock->RemoveExtOnRelease(bundleName);
}

ErrCode Service::GetCompatibilityInfo(const std::string &bundleName, const std::string &extInfo,
    std::string &compatInfo)
{
    return BService::serviceMock->GetCompatibilityInfo(bundleName, extInfo, compatInfo);
}

void Service::SetBroadCastInfoMap(const std::string &bundleName,
                                  const std::map<std::string, std::string> &broadCastInfoMap,
                                  int userId)
{
    BService::serviceMock->SetBroadCastInfoMap(bundleName, broadCastInfoMap, userId);
}
void Service::BroadCastRestore(const std::string &bundleName, const std::string &broadCastType)
{
    BService::serviceMock->BroadCastRestore(bundleName, broadCastType);
}
void Service::BroadCastSingle(const std::string &bundleName, const std::string &broadCastType)
{
    BService::serviceMock->BroadCastSingle(bundleName, broadCastType);
}

void Service::TotalStatStart(BizScene bizScene, std::string caller, uint64_t startTime, Mode mode)
{
    std::unique_lock<std::shared_mutex> lock(totalStatMutex_);
    totalStatistic_ = std::make_shared<RadarTotalStatistic>(bizScene, caller, mode);
    totalStatistic_->totalSpendTime_.startMilli_ = startTime;
}

void Service::TotalStatEnd(ErrCode errCode)
{
    std::unique_lock<std::shared_mutex> lock(totalStatMutex_);
    if (totalStatistic_ != nullptr) {
        totalStatistic_->totalSpendTime_.End();
        if (errCode != ERROR_OK) {
            totalStatistic_->innerErr_ = errCode;
        }
    }
}

void Service::UpdateHandleCnt(ErrCode errCode)
{
    std::unique_lock<std::shared_mutex> lock(totalStatMutex_);
    if (totalStatistic_ != nullptr) {
        if (errCode == ERROR_OK) {
            totalStatistic_->succBundleCount_++;
        } else {
            totalStatistic_->failBundleCount_++;
        }
    }
}

#ifdef POWER_MANAGER_ENABLED
void Service::RunningLockRadarReport(const std::string &func, const std::string &errMsg, ErrCode errCode)
{
    BService::serviceMock->RunningLockRadarReport(func, errMsg, errCode);
}
#endif

void Service::CreateRunningLock()
{
    BService::serviceMock->CreateRunningLock();
}
} // namespace OHOS::FileManagement::Backup

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace testing::ext;
using CallbackFunc = std::function<int (int, int, unsigned int, unsigned int)>;
typedef int (*CallDeviceTaskRequest)(int, uint32_t, uint32_t, CallbackFunc);

constexpr int32_t SERVICE_ID = 5203;

class ServiceIncrementalTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp()
    {
        dlFuncMock = std::make_shared<DlfcnMock>();
        DlfcnMock::dlFunc_ = dlFuncMock;
    };
    void TearDown()
    {
        dlFuncMock = nullptr;
        DlfcnMock::dlFunc_ = nullptr;
    };

    static inline shared_ptr<DlfcnMock> dlFuncMock = nullptr;
    static inline sptr<Service> service = nullptr;
    static inline shared_ptr<BackupParaMock> param = nullptr;
    static inline shared_ptr<BJsonUtilMock> jsonUtil = nullptr;
    static inline shared_ptr<SvcSessionManagerMock> session = nullptr;
    static inline shared_ptr<BundleMgrAdapterMock> bms = nullptr;
    static inline sptr<SvcExtensionProxyMock> svcProxy = nullptr;
    static inline sptr<SvcBackupConnectionMock> connect = nullptr;
    static inline shared_ptr<SABackupConnectionMock> saConnect = nullptr;
    static inline shared_ptr<IPCSkeletonMock> skeleton = nullptr;
    static inline sptr<ServiceReverseProxyMock> srProxy = nullptr;
    static inline shared_ptr<ServiceMock> srvMock = nullptr;
    static inline shared_ptr<DirectoryFuncMock> directMock = nullptr;

    static int gcFuncMock1(int argv1, unsigned int argv2, unsigned int argv3, CallbackFunc argv4);
    static int gcFuncMock2(int argv1, unsigned int argv2, unsigned int argv3, CallbackFunc argv4);
};

void ServiceIncrementalTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    srvMock = make_shared<ServiceMock>();
    ServiceMock::serviceMock = srvMock;
    service = sptr(new Service(SERVICE_ID));
    param = make_shared<BackupParaMock>();
    BackupParaMock::backupPara = param;
    jsonUtil = make_shared<BJsonUtilMock>();
    BJsonUtilMock::jsonUtil = jsonUtil;
    session = make_shared<SvcSessionManagerMock>();
    SvcSessionManagerMock::sessionManager = session;
    svcProxy = sptr(new SvcExtensionProxyMock());
    bms = make_shared<BundleMgrAdapterMock>();
    BundleMgrAdapterMock::bms = bms;
    connect = sptr(new SvcBackupConnectionMock());
    SvcBackupConnectionMock::connect = connect;
    saConnect = make_shared<SABackupConnectionMock>();
    SABackupConnectionMock::saConnect = saConnect;
    skeleton = make_shared<IPCSkeletonMock>();
    IPCSkeletonMock::skeleton = skeleton;
    srProxy = sptr(new ServiceReverseProxyMock());
    directMock = make_shared<DirectoryFuncMock>();
    DirectoryFuncMock::directoryFunc_ = directMock;
}

void ServiceIncrementalTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    service = nullptr;
    BackupParaMock::backupPara = nullptr;
    param = nullptr;
    BJsonUtilMock::jsonUtil = nullptr;
    jsonUtil = nullptr;
    SvcSessionManagerMock::sessionManager = nullptr;
    session = nullptr;
    svcProxy = nullptr;
    BundleMgrAdapterMock::bms = nullptr;
    bms = nullptr;
    SvcBackupConnectionMock::connect = nullptr;
    connect = nullptr;
    SABackupConnectionMock::saConnect = nullptr;
    saConnect = nullptr;
    IPCSkeletonMock::skeleton = nullptr;
    skeleton = nullptr;
    srProxy = nullptr;
    ServiceMock::serviceMock = nullptr;
    srvMock = nullptr;
    DirectoryFuncMock::directoryFunc_ = nullptr;
    directMock = nullptr;
}

int ServiceIncrementalTest::gcFuncMock1(int argv1, unsigned int argv2, unsigned int argv3, CallbackFunc argv4)
{
    thread mockFuncThread([argv4]() {
        int testStatus = 0;
        int testErrcode = 1;
        unsigned int testPercent = 2;
        unsigned int testGap = 3;
        argv4(testStatus, testErrcode, testPercent, testGap);
    });
    mockFuncThread.detach();
    return 0;
}

int ServiceIncrementalTest::gcFuncMock2(int argv1, unsigned int argv2, unsigned int argv3, CallbackFunc argv4)
{
    thread mockFuncThread([argv4]() {
        int testStatus = 0;
        int testErrcode = 1;
        unsigned int testPercent = 2;
        unsigned int testGap = 3;
        argv4(testStatus, testErrcode, testPercent, testGap);
    });
    mockFuncThread.detach();
    int mockRes = -1;
    return mockRes;
}
/**
 * @tc.number: SUB_ServiceIncremental_GetLocalCapabilitiesIncremental_0000
 * @tc.name: SUB_ServiceIncremental_GetLocalCapabilitiesIncremental_0000
 * @tc.desc: 测试 GetLocalCapabilitiesIncremental 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_GetLocalCapabilitiesIncremental_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_GetLocalCapabilitiesIncremental_0000";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        int fd = -1;
        service->GetLocalCapabilitiesIncremental({}, fd);
        service->session_ = session_;
        EXPECT_EQ(static_cast<int>(fd), BConstants::INVALID_FD_NUM);

        service->isOccupyingSession_ = true;
        fd = service->GetLocalCapabilitiesIncremental({});
        EXPECT_EQ(static_cast<int>(fd), -ENOENT);

        service->isOccupyingSession_ = false;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::EXT_INVAL_ARG).GetCode()));
        fd = service->GetLocalCapabilitiesIncremental({});
        EXPECT_EQ(static_cast<int>(fd), -ENOENT);

        vector<BJsonEntityCaps::BundleInfo> infos;
        infos.emplace_back(BJsonEntityCaps::BundleInfo{.name = "bundleName", .appIndex = 0});
        EXPECT_CALL(*directMock, ForceRemoveDirectoryBMS(_)).WillRepeatedly(Return(true));
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfosForIncremental(An<int32_t>(), An<const vector<BIncrementalData>&>()))
            .WillOnce(Return(infos));

        BJsonUtil::BundleDetailInfo bundleInfo;
        bundleInfo.bundleIndex = 1;
        bundleInfo.bundleName = "bundleName";
        fd = service->GetLocalCapabilitiesIncremental({});

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfosForIncremental(An<int32_t>(), An<const vector<BIncrementalData>&>()))
            .WillOnce(Return(infos));
        service->isCreatingIncreaseFile_.store(2);
        fd = service->GetLocalCapabilitiesIncremental({});
        service->isCreatingIncreaseFile_.store(0);
        EXPECT_TRUE(static_cast<int>(fd) > 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetLocalCapabilitiesIncremental.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetLocalCapabilitiesIncremental_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_StartGetFdTask_0000
 * @tc.name: SUB_ServiceIncremental_StartGetFdTask_0000
 * @tc.desc: 测试 StartGetFdTask 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_StartGetFdTask_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_StartGetFdTask_0000";
    try {
        service->StartGetFdTask("", nullptr);
        EXPECT_TRUE(true);

        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_THROW(service->StartGetFdTask("", service), BError);

        service->session_ = session_;
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        EXPECT_THROW(service->StartGetFdTask("", service), BError);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        EXPECT_THROW(service->StartGetFdTask("", service), BError);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*svcProxy, User0OnBackup()).WillOnce(Return(BError(BError::Codes::EXT_INVAL_ARG).GetCode()));
        service->StartGetFdTask("", service);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*svcProxy, User0OnBackup()).WillOnce(Return(ERR_OK));
        service->StartGetFdTask("", service);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by StartGetFdTask.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_StartGetFdTask_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_StartGetFdTask_0100
 * @tc.name: SUB_ServiceIncremental_StartGetFdTask_0100
 * @tc.desc: 测试 StartGetFdTask 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_StartGetFdTask_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_StartGetFdTask_0100";
    try {
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(false));
        EXPECT_THROW(service->StartGetFdTask("", service), BError);

        BJsonUtil::BundleDetailInfo bundleInfo;
        vector<BJsonEntityCaps::BundleInfo> info;
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetLastIncrementalTime(_)).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfosForIncremental(An<const vector<BIncrementalData>&>(), An<int32_t>()))
            .WillOnce(Return(info));
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(bundleInfo));
        EXPECT_THROW(service->StartGetFdTask("", service), BError);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by StartGetFdTask.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_StartGetFdTask_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_RefreshBundleDataSize_0000
 * @tc.name: SUB_ServiceIncremental_RefreshBundleDataSize_0000
 * @tc.desc: 测试 RefreshBundleDataSize 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_RefreshBundleDataSize_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_RefreshBundleDataSize_0000";
    try {
        service->RefreshBundleDataSize({}, "", nullptr);
        EXPECT_TRUE(true);

        auto session_ = service->session_;
        service->session_ = nullptr;
        service->RefreshBundleDataSize({}, "", service);
        EXPECT_TRUE(true);

        service->session_ = session_;
        BJsonUtil::BundleDetailInfo bundleInfo;
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(bundleInfo));
        service->RefreshBundleDataSize({}, "", service);
        EXPECT_TRUE(true);

        bundleInfo.bundleIndex = 0;
        bundleInfo.bundleName = "bundleName";
        vector<BJsonEntityCaps::BundleInfo> infos;
        infos.emplace_back(BJsonEntityCaps::BundleInfo{.name = "bundleName", .appIndex = 0});
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(bundleInfo));
        service->RefreshBundleDataSize(infos, "", service);
        EXPECT_TRUE(true);

        bundleInfo.bundleIndex = 1;
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(bundleInfo));
        service->RefreshBundleDataSize(infos, "", service);
        EXPECT_TRUE(true);

        bundleInfo.bundleName = "appName";
        EXPECT_CALL(*jsonUtil, ParseBundleNameIndexStr(_)).WillOnce(Return(bundleInfo));
        service->RefreshBundleDataSize(infos, "", service);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by RefreshBundleDataSize.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_RefreshBundleDataSize_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_GetAppLocalListAndDoIncrementalBackup_0000
 * @tc.name: SUB_ServiceIncremental_GetAppLocalListAndDoIncrementalBackup_0000
 * @tc.desc: 测试 GetAppLocalListAndDoIncrementalBackup 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_GetAppLocalListAndDoIncrementalBackup_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) <<
        "ServiceIncrementalTest-begin SUB_ServiceIncremental_GetAppLocalListAndDoIncrementalBackup_0000";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_EQ(service->GetAppLocalListAndDoIncrementalBackup(), BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        service->isOccupyingSession_ = true;
        EXPECT_EQ(service->GetAppLocalListAndDoIncrementalBackup(), BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->isOccupyingSession_ = false;
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_EQ(service->GetAppLocalListAndDoIncrementalBackup(), BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        EXPECT_EQ(service->GetAppLocalListAndDoIncrementalBackup(), BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetAppLocalListAndDoIncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetAppLocalListAndDoIncrementalBackup_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_InitIncrementalBackupSession_0000
 * @tc.name: SUB_ServiceIncremental_InitIncrementalBackupSession_0000
 * @tc.desc: 测试 InitIncrementalBackupSession 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_InitIncrementalBackupSession_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_InitIncrementalBackupSession_0000";
    try {
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_CALL(*srvMock, GetCallerName()).WillRepeatedly(Return(""));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr), BError(BError::Codes::SA_INVAL_ARG).GetCode());
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        sptr<IServiceReverse> srPrt = static_cast<sptr<IServiceReverse>>(srProxy);
        srPrt = nullptr;
        EXPECT_EQ(service->InitIncrementalBackupSession(srPrt), BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        EXPECT_EQ(service->InitIncrementalBackupSession(srPrt), BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::SA_SESSION_CONFLICT)));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr), BError(BError::Codes::SA_SESSION_CONFLICT).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        EXPECT_EQ(service->InitIncrementalBackupSession(nullptr), BError(BError::Codes::SA_INVAL_ARG).GetCode());

    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by InitIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_InitIncrementalBackupSession_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_InitIncrementalBackupSession_0100
 * @tc.name: SUB_ServiceIncremental_InitIncrementalBackupSession_0100
 * @tc.desc: 测试 InitIncrementalBackupSession with errMsg的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_InitIncrementalBackupSession_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_InitIncrementalBackupSession_0100";
    try {
        std::string errMsg;
        sptr<IServiceReverse> reverseNUll = nullptr;
        ErrCode errCode;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_CALL(*srvMock, GetCallerName()).WillRepeatedly(Return(""));
        service->InitIncrementalBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
        EXPECT_EQ(errCode, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        service->InitIncrementalBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
        EXPECT_EQ(errCode, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::OK)));
        service->InitIncrementalBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
        EXPECT_EQ(errCode, BError(BError::Codes::OK).GetCode());
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::SA_SESSION_CONFLICT)));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        EXPECT_CALL(*session, GetSessionCallerName()).WillOnce(Return(""));
        EXPECT_CALL(*session, GetSessionActiveTime()).WillOnce(Return(""));
        EXPECT_CALL(*jsonUtil, BuildInitSessionErrInfo(_, _, _, _)).WillOnce(Return(""));
        service->InitIncrementalBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
        EXPECT_EQ(errCode, BError(BError::Codes::SA_SESSION_CONFLICT).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*skeleton, GetCallingTokenID()).WillOnce(Return(0));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        EXPECT_CALL(*session, Active(_, _)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG)));
        service->InitIncrementalBackupSessionWithErrMsg(reverseNUll, errCode, errMsg);
        EXPECT_EQ(errCode, BError(BError::Codes::SA_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by InitIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_InitIncrementalBackupSession_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0000
 * @tc.name: SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0000
 * @tc.desc: 测试 AppendBundlesIncrementalBackupSession 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) <<
        "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0000";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_EQ(service->AppendBundlesIncrementalBackupSession({}), BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        service->isOccupyingSession_ = true;
        EXPECT_EQ(service->AppendBundlesIncrementalBackupSession({}), BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->isOccupyingSession_ = false;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_EQ(service->AppendBundlesIncrementalBackupSession({}), BError(BError::Codes::SA_INVAL_ARG).GetCode());

        std::vector<std::string> supportBackupNames;
        vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*bms, GetBundleInfosForAppendBundles(_, _)).WillOnce(Return(bundleInfos));
        EXPECT_CALL(*srvMock, GetSupportBackupBundleNames(_, _, _)).WillOnce(Return(supportBackupNames));
        EXPECT_EQ(service->AppendBundlesIncrementalBackupSession({}), BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppendBundlesIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0100
 * @tc.name: SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0100
 * @tc.desc: 测试 AppendBundlesIncrementalBackupSession 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) <<
        "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0100";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto ret = service->AppendBundlesIncrementalBackupSession({}, {});
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        service->isOccupyingSession_ = true;
        ret = service->AppendBundlesIncrementalBackupSession({}, {});
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->isOccupyingSession_ = false;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = service->AppendBundlesIncrementalBackupSession({}, {});
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        std::vector<std::string> supportBackupNames;
        vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*jsonUtil, BuildBundleInfos(_, _, _, _, _)).WillOnce(Return(bundleNameDetailMap));
        EXPECT_CALL(*bms, GetBundleInfosForAppendBundles(_, _)).WillOnce(Return(bundleInfos));
        EXPECT_CALL(*srvMock, GetSupportBackupBundleNames(_, _, _)).WillOnce(Return(supportBackupNames));
        ret = service->AppendBundlesIncrementalBackupSession({}, {});
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppendBundlesIncrementalBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppendBundlesIncrementalBackupSession_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_PublishIncrementalFile_0000
 * @tc.name: SUB_ServiceIncremental_PublishIncrementalFile_0000
 * @tc.desc: 测试 PublishIncrementalFile 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_PublishIncrementalFile_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_PublishIncrementalFile_0000";
    try {
        BFileInfo fileInfo;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        auto ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        fileInfo.fileName = "test";
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, EPERM);

        fileInfo.fileName.clear();
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->session_ = session_;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, PublishIncrementalFile(_))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, PublishIncrementalFile(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->PublishIncrementalFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by PublishIncrementalFile.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_PublishIncrementalFile_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_PublishSAIncrementalFile_0000
 * @tc.name: SUB_ServiceIncremental_PublishSAIncrementalFile_0000
 * @tc.desc: 测试 PublishSAIncrementalFile 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_PublishSAIncrementalFile_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_PublishSAIncrementalFile_0000";
    try {
        BFileInfo fileInfo;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        auto ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::SA_EXT_ERR_CALL).GetCode());

        fileInfo.owner = "abc";
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::SA_EXT_ERR_CALL).GetCode());

        fileInfo.owner = "123";
        shared_ptr<SABackupConnection> sa = nullptr;
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        sa = make_shared<SABackupConnection>(nullptr, nullptr, nullptr, nullptr);
        EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetSAExtConnection(_)).WillOnce(Return(sa));
        EXPECT_CALL(*saConnect, CallRestoreSA(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->PublishSAIncrementalFile(fileInfo, UniqueFd(-1));
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by PublishSAIncrementalFile.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_PublishSAIncrementalFile_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppIncrementalFileReady_0000
 * @tc.name: SUB_ServiceIncremental_AppIncrementalFileReady_0000
 * @tc.desc: 测试 AppIncrementalFileReady 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppIncrementalFileReady_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppIncrementalFileReady_0000";
    try {
        string bundleName;
        string fileName;
        int32_t errCode = 0;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        auto ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        fileName = BConstants::EXT_BACKUP_MANAGE;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, OnBundleExtManageInfo(_, _)).WillOnce(Return(UniqueFd(-1)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppIncrementalFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppIncrementalFileReady_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppIncrementalFileReady_0100
 * @tc.name: SUB_ServiceIncremental_AppIncrementalFileReady_0100
 * @tc.desc: 测试 AppIncrementalFileReady 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppIncrementalFileReady_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppIncrementalFileReady_0100";
    try {
        string bundleName;
        string fileName;
        int32_t errCode = 0;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        auto ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        fileName = BConstants::EXT_BACKUP_MANAGE;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, OnBundleExtManageInfo(_, _)).WillOnce(Return(UniqueFd(-1)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*srvMock, HandleCurBundleFileReady(_, _, _)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppIncrementalFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppIncrementalFileReady_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppIncrementalFileReady_0200
 * @tc.name: SUB_ServiceIncremental_AppIncrementalFileReady_0200
 * @tc.desc: 测试 AppIncrementalFileReady 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppIncrementalFileReady_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppIncrementalFileReady_0200";
    try {
        string fileName;
        int32_t errCode = 0;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        auto ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
            EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        fileName = BConstants::EXT_BACKUP_MANAGE;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, OnBundleExtManageInfo(_, _)).WillOnce(Return(UniqueFd(-1)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppIncrementalFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppIncrementalFileReady_0200";
}

/**
* @tc.number: SUB_ServiceIncremental_AppIncrementalFileReady_0300
* @tc.name: SUB_ServiceIncremental_AppIncrementalFileReady_0300
* @tc.desc: 测试 AppIncrementalFileReady 的正常/异常分支
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: issueIAKC3I
*/
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppIncrementalFileReady_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppIncrementalFileReady_0300";
    try {
        string fileName;
        int32_t errCode = 0;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*srvMock, HandleCurBundleFileReady(_, _, _))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        auto ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        fileName = BConstants::EXT_BACKUP_MANAGE;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, OnBundleExtManageInfo(_, _)).WillOnce(Return(UniqueFd(-1)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*srvMock, HandleCurBundleFileReady(_, _, _)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = service->AppIncrementalFileReady(fileName, UniqueFd(-1), UniqueFd(-1), errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppIncrementalFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppIncrementalFileReady_0300";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppIncrementalFileReadyWithoutFd_0000
 * @tc.name: SUB_ServiceIncremental_AppIncrementalFileReadyWithoutFd_0000
 * @tc.desc: 测试 AppIncrementalFileReady 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppIncrementalFileReadyWithoutFd_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppIncrementalFileReadyWithoutFd_0000";
    try {
        string bundleName;
        string fileName;
        int32_t errCode = 0;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        auto ret = service->AppIncrementalFileReadyWithoutFd(fileName, errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalFileReadyWithoutFd(fileName, errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        fileName = BConstants::EXT_BACKUP_MANAGE;
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, OnBundleExtManageInfo(_, _)).WillOnce(Return(UniqueFd(-1)));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalFileReadyWithoutFd(fileName, errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppIncrementalFileReadyWithoutFd.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppIncrementalFileReadyWithoutFd_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_AppIncrementalDone_0000
 * @tc.name: SUB_ServiceIncremental_AppIncrementalDone_0000
 * @tc.desc: 测试 AppIncrementalDone 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_AppIncrementalDone_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_AppIncrementalDone_0000";
    try {
        int32_t errCode = BError(BError::Codes::OK).GetCode();
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillRepeatedly(Return(100));
        BJsonUtil::BundleDetailInfo bundleInfo;
        bundleInfo.bundleIndex = 0;
        bundleInfo.bundleName = "bundleName";
        auto ret = service->AppIncrementalDone(errCode);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = service->AppIncrementalDone(errCode);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalDone(errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        service->backupExtMutexMap_.clear();
        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(false));
        ret = service->AppIncrementalDone(BError(BError::Codes::SA_INVAL_ARG).GetCode());
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCallerAndGetCallerName(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        ret = service->AppIncrementalDone(errCode);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by AppIncrementalDone.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_AppIncrementalDone_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_GetIncrementalFileHandle_0000
 * @tc.name: SUB_ServiceIncremental_GetIncrementalFileHandle_0000
 * @tc.desc: 测试 GetIncrementalFileHandle 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_GetIncrementalFileHandle_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_GetIncrementalFileHandle_0000";
    try {
        string bundleName;
        string fileName;
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
        service->session_ = session_;

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::UNKNOWN));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetIncrementalFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetIncrementalFileHandle_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_GetIncrementalFileHandle_0100
 * @tc.name: SUB_ServiceIncremental_GetIncrementalFileHandle_0100
 * @tc.desc: 测试 GetIncrementalFileHandle 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_GetIncrementalFileHandle_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_GetIncrementalFileHandle_0100";
    try {
        string bundleName;
        string fileName;
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalFileHandle(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*session, OnBundleFileReady(_, _)).WillOnce(Return(true));
        EXPECT_CALL(*srvMock, HandleCurBundleFileReady(_, _, _))
            .WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
        EXPECT_CALL(*srvMock, GetUserIdDefault()).WillOnce(Return(0));
        auto ret = service->GetIncrementalFileHandle(bundleName, fileName);
            EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, GetIncrementalFileHandle(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::WAIT));
        EXPECT_CALL(*srvMock, UpdateToRestoreBundleMap(_, _)).WillOnce(Return(true));
        ret = service->GetIncrementalFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetIncrementalFileHandle.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetIncrementalFileHandle_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_IncrementalBackup_0000
 * @tc.name: SUB_ServiceIncremental_IncrementalBackup_0000
 * @tc.desc: 测试 IncrementalBackup 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_IncrementalBackup_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_IncrementalBackup_0000";
    try {
        string bundleName;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        EXPECT_TRUE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        EXPECT_TRUE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_FALSE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, IncrementalOnBackup(_)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnBundleStarted(_, _)).WillOnce(Return(0));
        EXPECT_TRUE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, IncrementalOnBackup(_)).WillOnce(Return(1));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnBundleStarted(_, _)).WillOnce(Return(0));
        EXPECT_TRUE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(false));
        EXPECT_FALSE(service->IncrementalBackup(bundleName));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by IncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_IncrementalBackup_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_IncrementalBackup_0100
 * @tc.name: SUB_ServiceIncremental_IncrementalBackup_0100
 * @tc.desc: 测试 IncrementalBackup 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_IncrementalBackup_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_IncrementalBackup_0100";
    try {
        string bundleName;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(false));
        EXPECT_FALSE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(false));
        EXPECT_FALSE(service->IncrementalBackup(bundleName));

        set<string> fileNameVec { "fileName" };
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE))
        .WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, HandleRestore(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy)).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleStarted(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetOldBackupVersion()).WillOnce(Return(""));
        EXPECT_CALL(*session, GetExtFileNameRequest(_)).WillOnce(Return(fileNameVec));
        EXPECT_CALL(*svcProxy, GetIncrementalFileHandle(_, _, _, _)).WillOnce(Return(0));
        EXPECT_TRUE(service->IncrementalBackup(bundleName));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by IncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_IncrementalBackup_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_IncrementalBackup_0200
 * @tc.name: SUB_ServiceIncremental_IncrementalBackup_0200
 * @tc.desc: 测试 IncrementalBackup 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_IncrementalBackup_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_IncrementalBackup_0200";
    try {
        string bundleName;
        set<string> fileNameVec { "fileName" };
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE))
        .WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*param, GetBackupOverrideIncrementalRestore()).WillOnce(Return(true));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(true));
        EXPECT_CALL(*session, GetClearDataFlag(_)).WillOnce(Return(false));
        EXPECT_CALL(*svcProxy, HandleRestore(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy)).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleStarted(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*session, GetOldBackupVersion()).WillOnce(Return("1.0.0"));
        EXPECT_CALL(*session, GetExtFileNameRequest(_)).WillOnce(Return(fileNameVec));
        EXPECT_CALL(*svcProxy, GetIncrementalFileHandle(_, _, _, _)).WillOnce(Return(0));
        EXPECT_TRUE(service->IncrementalBackup(bundleName));

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(connect));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_FALSE(service->IncrementalBackup(bundleName));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by IncrementalBackup.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_IncrementalBackup_0200";
}

/**
 * @tc.number: SUB_ServiceIncremental_NotifyCallerCurAppIncrementDone_0000
 * @tc.name: SUB_ServiceIncremental_NotifyCallerCurAppIncrementDone_0000
 * @tc.desc: 测试 NotifyCallerCurAppIncrementDone 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_NotifyCallerCurAppIncrementDone_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_NotifyCallerCurAppIncrementDone_0000";
    try {
        ErrCode errCode = 0;
        string callerName;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::UNDEFINED));
        service->NotifyCallerCurAppIncrementDone(errCode, callerName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalBackupOnBundleFinished(_, _)).WillOnce(Return(0));
        service->NotifyCallerCurAppIncrementDone(errCode, callerName);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillOnce(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleFinished(_, _)).WillOnce(Return(0));
        service->NotifyCallerCurAppIncrementDone(errCode, callerName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by NotifyCallerCurAppIncrementDone.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_NotifyCallerCurAppIncrementDone_0200";
}

/**
 * @tc.number: SUB_ServiceIncremental_SendUserIdToApp_0000
 * @tc.name: SUB_ServiceIncremental_SendUserIdToApp_0000
 * @tc.desc: 测试 SendUserIdToApp 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_SendUserIdToApp_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_SendUserIdToApp_0000";
    try {
        string bundleName;
        int32_t userId = 100;
        auto session_ = service->session_;
        service->session_ = nullptr;
        service->SendUserIdToApp(bundleName, userId);
        service->session_ = session_;
        EXPECT_TRUE(true);

        EXPECT_CALL(*jsonUtil, BuildBundleInfoJson(_, _)).WillOnce(Return(false));
        service->SendUserIdToApp(bundleName, userId);
        EXPECT_TRUE(true);

        EXPECT_CALL(*jsonUtil, BuildBundleInfoJson(_, _)).WillOnce(Return(true));
        service->SendUserIdToApp(bundleName, userId);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by SendUserIdToApp.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_SendUserIdToApp_0200";
}

/**
 * @tc.number: SUB_ServiceIncremental_SetCurrentBackupSessProperties_0000
 * @tc.name: SUB_ServiceIncremental_SetCurrentBackupSessProperties_0000
 * @tc.desc: 测试 SetCurrentBackupSessProperties 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_SetCurrentBackupSessProperties_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_SetCurrentBackupSessProperties_0000";
    try {
        vector<string> bundleNames { "bundleName" };
        int32_t userId = 100;
        vector<BJsonEntityCaps::BundleInfo> bundleInfos;
        auto session_ = service->session_;
        service->session_ = nullptr;
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        service->SetCurrentBackupSessProperties(bundleNames, userId, bundleInfos, true);
        EXPECT_TRUE(true);

        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(true));
        service->SetCurrentBackupSessProperties(bundleNames, userId, bundleInfos, true);
        EXPECT_TRUE(true);

        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        service->SetCurrentBackupSessProperties(bundleNames, userId, bundleInfos, false);
        EXPECT_TRUE(true);

        BJsonEntityCaps::BundleInfo bundleInfo;
        bundleInfo.name = "bundleName";
        bundleInfos.push_back(bundleInfo);
        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        service->SetCurrentBackupSessProperties(bundleNames, userId, bundleInfos, false);
        EXPECT_TRUE(true);

        EXPECT_CALL(*jsonUtil, BuildBundleNameIndexInfo(_, _)).WillOnce(Return("bundleName"));
        EXPECT_CALL(*bms, IsUser0BundleName(_, _)).WillOnce(Return(false));
        service->SetCurrentBackupSessProperties(bundleNames, userId, bundleInfos, true);
        EXPECT_TRUE(true);
        service->session_ = session_;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by SetCurrentBackupSessProperties.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_SetCurrentBackupSessProperties_0200";
}

/**
 * @tc.number: SUB_ServiceIncremental_Release_0000
 * @tc.name: SUB_ServiceIncremental_Release_0000
 * @tc.desc: 测试 Release 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_Release_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_Release_0000";
    try {
        auto session_ = service->session_;
        service->session_ = nullptr;
        auto ret = service->Release();
        EXPECT_EQ(ret, BError(BError::Codes::SA_INVAL_ARG).GetCode());

        service->session_ = session_;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        ret = service->Release();
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetSessionUserId()).WillOnce(Return(0));
        ret = service->Release();
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::UNDEFINED));
        ret = service->Release();
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by Release.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_Release_0200";
}

/**
 * @tc.number: SUB_ServiceIncremental_SetBundleIncDataInfo_0000
 * @tc.name: SUB_ServiceIncremental_SetBundleIncDataInfo_0000
 * @tc.desc: 测试 SetBundleIncDataInfo 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_SetBundleIncDataInfo_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_SetBundleIncDataInfo_0000";
    try {
        vector<BIncrementalData> bundlesToBackup;
        vector<string> supportBundleNames;
        service->SetBundleIncDataInfo(bundlesToBackup, supportBundleNames);
        EXPECT_TRUE(true);

        BIncrementalData data;
        data.bundleName = "bundleName";
        bundlesToBackup.push_back(data);
        service->SetBundleIncDataInfo(bundlesToBackup, supportBundleNames);
        EXPECT_TRUE(true);

        supportBundleNames.emplace_back("bundleName");
        service->SetBundleIncDataInfo(bundlesToBackup, supportBundleNames);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by SetBundleIncDataInfo.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_SetBundleIncDataInfo_0200";
}

/**
 * @tc.number: SUB_ServiceIncremental_CancelTask_0000
 * @tc.name: SUB_ServiceIncremental_CancelTask_0000
 * @tc.desc: 测试 CancelTask 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_CancelTask_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_CancelTask_0000";
    try {
        service->CancelTask("", nullptr);
        EXPECT_TRUE(true);

        auto session_ = service->session_;
        service->session_ = nullptr;
        service->CancelTask("", service);
        service->session_ = session_;
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(nullptr));
        service->CancelTask("", service);
        EXPECT_TRUE(true);

        std::string bundleName = "123";
        auto callDied = [](const string &&bundleName, bool isCleanCalled) {};
        auto callConnected = [](const string &&bundleName) {};
        auto connectPtr = sptr(new SvcBackupConnection(callDied, callConnected, bundleName));

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(wptr(connectPtr)));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(nullptr));
        service->CancelTask("", service);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(wptr(connectPtr)));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, HandleClear()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::UNDEFINED));
        service->CancelTask("", service);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(wptr(connectPtr)));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, HandleClear()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(false));
        service->CancelTask("", service);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by CancelTask.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_CancelTask_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_CancelTask_0100
 * @tc.name: SUB_ServiceIncremental_CancelTask_0100
 * @tc.desc: 测试 CancelTask 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_CancelTask_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_CancelTask_0100";
    try {
        std::string bundleName = "123";
        auto callDied = [](const string &&bundleName, bool isCleanCalled) {};
        auto callConnected = [](const string &&bundleName) {};
        auto connectPtr = sptr(new SvcBackupConnection(callDied, callConnected, bundleName));

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(wptr(connectPtr)));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, HandleClear()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::BACKUP))
            .WillOnce(Return(IServiceReverseType::Scenario::UNDEFINED));
        EXPECT_CALL(*session, GetIsIncrementalBackup()).WillOnce(Return(true));
        EXPECT_CALL(*session, GetServiceReverseProxy()).WillRepeatedly(Return(srProxy));
        EXPECT_CALL(*srProxy, IncrementalRestoreOnBundleFinished(_, _)).WillRepeatedly(Return(0));
        service->CancelTask("", service);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(wptr(connectPtr)));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, HandleClear()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(false));
        service->CancelTask("", service);
        EXPECT_TRUE(true);

        EXPECT_CALL(*session, GetExtConnection(_)).WillOnce(Return(wptr(connectPtr))).WillOnce(Return(nullptr));
        EXPECT_CALL(*connect, GetBackupExtProxy()).WillOnce(Return(svcProxy));
        EXPECT_CALL(*svcProxy, HandleClear()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, StopFwkTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*session, StopExtTimer(_)).WillOnce(Return(false));
        EXPECT_CALL(*connect, DisconnectBackupExtAbility()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE))
            .WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*session, ValidRestoreDataType(_)).WillOnce(Return(true));
        service->CancelTask("", service);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by CancelTask.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_CancelTask_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_Cancel_0000
 * @tc.name: SUB_ServiceIncremental_Cancel_0000
 * @tc.desc: 测试 Cancel 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_Cancel_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_Cancel_0000";
    try {
        string bundleName = "com.example.app2backup";
        BackupExtInfo info {};
        info.backupExtName = "com.example.app2backup";
        SvcSessionManager::Impl impl;
        int32_t result;

        auto session_ = service->session_;
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetImpl()).WillOnce(Return(impl));
        service->CancelForResult(bundleName, result);
        EXPECT_EQ(result, BError(BError::BackupErrorCode::E_CANCEL_NO_TASK).GetCode());

        impl.backupExtNameMap.insert(make_pair(bundleName, info));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetImpl()).WillOnce(Return(impl));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::UNKNOWN));
        service->CancelForResult(bundleName, result);
        EXPECT_EQ(result, BError(BError::BackupErrorCode::E_CANCEL_NO_TASK).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetImpl()).WillOnce(Return(impl));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::CLEAN));
        service->CancelForResult(bundleName, result);
        EXPECT_EQ(result, BError(BError::BackupErrorCode::E_CANCEL_NO_TASK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by Cancel.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_Cancel_0000";
}

/**
* @tc.number: SUB_ServiceIncremental_Cancel_0100
* @tc.name: SUB_ServiceIncremental_Cancel_0100
* @tc.desc: 测试 Cancel 的正常/异常分支
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: issueIAKC3I
*/
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_Cancel_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_Cancel_0000";
    try {
        string bundleName = "com.example.app2backup";
        BackupExtInfo info {};
        info.backupExtName = "com.example.app2backup";
        SvcSessionManager::Impl impl;
        int32_t result;

        impl.backupExtNameMap.insert(make_pair(bundleName, info));
        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetImpl()).WillOnce(Return(impl));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::START));
        service->CancelForResult(bundleName, result);
        EXPECT_EQ(result, BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK).GetCode());

        EXPECT_CALL(*session, GetScenario()).WillOnce(Return(IServiceReverseType::Scenario::RESTORE));
        EXPECT_CALL(*srvMock, VerifyCaller(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        EXPECT_CALL(*session, GetImpl()).WillOnce(Return(impl));
        EXPECT_CALL(*session, GetServiceSchedAction(_)).WillOnce(Return(BConstants::ServiceSchedAction::RUNNING));
        service->CancelForResult(bundleName, result);
        EXPECT_EQ(result, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by Cancel.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_Cancel_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_GetExtensionMutex_0000
 * @tc.name: SUB_ServiceIncremental_GetExtensionMutex_0000
 * @tc.desc: 测试 GetExtensionMutex 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_GetExtensionMutex_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_GetExtensionMutex_0000";
    try {
        BundleName bundleName = "bundleName";
        service->backupExtMutexMap_.clear();
        auto ret = service->GetExtensionMutex(bundleName);
        EXPECT_TRUE(ret != nullptr);

        ret = service->GetExtensionMutex(bundleName);
        EXPECT_TRUE(ret != nullptr);
        service->backupExtMutexMap_.clear();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetExtensionMutex.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetExtensionMutex_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_RemoveExtensionMutex_0000
 * @tc.name: SUB_ServiceIncremental_RemoveExtensionMutex_0000
 * @tc.desc: 测试 RemoveExtensionMutex 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_RemoveExtensionMutex_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_RemoveExtensionMutex_0000";
    try {
        BundleName bundleName = "bundleName";
        service->backupExtMutexMap_.clear();
        service->RemoveExtensionMutex(bundleName);
        EXPECT_TRUE(true);

        service->backupExtMutexMap_[bundleName] = nullptr;
        service->RemoveExtensionMutex(bundleName);
        EXPECT_TRUE(true);
        service->backupExtMutexMap_.clear();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by RemoveExtensionMutex.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_RemoveExtensionMutex_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_ClearIncrementalStatFile_0000
 * @tc.name: SUB_ServiceIncremental_ClearIncrementalStatFile_0000
 * @tc.desc: 测试 ClearIncrementalStatFile 分身与一般应用分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_ClearIncrementalStatFile_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_ClearIncrementalStatFile_0000";
    try {
        
        int userId = BConstants::DEFAULT_USER_ID;
        ASSERT_TRUE(service != nullptr);
        BJsonUtil::BundleDetailInfo bundleInfo;
        bundleInfo.bundleIndex = 1;
        bundleInfo.bundleName = "com.example.app2backup";
        service->ClearIncrementalStatFile(userId, bundleInfo.bundleName);

        bundleInfo.bundleIndex = 0;
        service->ClearIncrementalStatFile(userId, bundleInfo.bundleName);
        EXPECT_EQ(bundleInfo.bundleIndex, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by ClearIncrementalStatFile.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_ClearIncrementalStatFile_0000";
}

/**
 * @tc.number: SUB_ServiceIncremental_ClearIncrementalStatFile_0100
 * @tc.name: SUB_ServiceIncremental_ClearIncrementalStatFile_0100
 * @tc.desc: 测试 ClearIncrementalStatFile 的文件删除异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_ClearIncrementalStatFile_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_ClearIncrementalStatFile_0100";
    try {
        
        int userId = BConstants::DEFAULT_USER_ID;
        ASSERT_TRUE(service != nullptr);
        BJsonUtil::BundleDetailInfo bundleInfo;
        bundleInfo.bundleIndex = 0;
        bundleInfo.bundleName = "com.example.app2backup";
        string path = BConstants::GetSaBundleBackupRootDir(userId).data() + bundleInfo.bundleName;
        string cmdMkdir = string("mkdir -p ") + path;
        int ret = system(cmdMkdir.c_str());
        EXPECT_EQ(ret, 0);

        service->ClearIncrementalStatFile(userId, bundleInfo.bundleName);
        
        EXPECT_CALL(*directMock, ForceRemoveDirectoryBMS(_)).WillOnce(Return(false));
        service->ClearIncrementalStatFile(userId, bundleInfo.bundleName);
        EXPECT_EQ(bundleInfo.bundleIndex, 0);
        string cmdRmdir = string("rm -r ") + path;
        ret = system(cmdRmdir.c_str());
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by ClearIncrementalStatFile.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_ClearIncrementalStatFile_0100";
}

/**
 * @tc.number: SUB_ServiceIncremental_UpdateGcprogress_0101
 * @tc.name: SUB_ServiceIncremental_UpdateGcprogress_0101
 * @tc.desc: 测试 UpdateGcprogress 函数
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_UpdateGcprogress_0101, TestSize.Level1)
{
    std::shared_ptr<GcProgressInfo> testGCProgressInfo = std::make_shared<GcProgressInfo>();
    int testStatus = 0;
    int testErrcode = 1;
    unsigned int testPercent = 2;
    unsigned int testGap = 3;
    GcProgressInfoUpdate testData{testStatus, testErrcode, testPercent, testGap};
    service->UpdateGcProgress(testGCProgressInfo, testData);
    EXPECT_EQ(testGCProgressInfo->status.load(), testStatus);
    EXPECT_EQ(testGCProgressInfo->errcode.load(), testErrcode);
    EXPECT_EQ(testGCProgressInfo->percent.load(), testPercent);
    EXPECT_EQ(testGCProgressInfo->gap.load(), testGap);
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_UpdateGcprogress_0101";
}

/**
 * @tc.number: SUB_ServiceIncremental_DealWithGcErrcode_0101
 * @tc.name: SUB_ServiceIncremental_DealWithGcErrcode_0101
 * @tc.desc: 测试 DealWithGcErrcode 函数
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_DealWithGcErrcode_0101, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_DealWithGcErrcode_0101";
    try {
        int testCode = BConstants::GC_DEVICE_OK;
        bool testStatus = true;
        auto res = service->DealWithGcErrcode(testStatus, testCode);
        EXPECT_EQ(res, ERROR_OK);

        testCode = BConstants::GC_DEVICE_INCOMPATIBLE;
        res = service->DealWithGcErrcode(testStatus, testCode);
        EXPECT_EQ(res, static_cast<ErrCode> (BError::BackupErrorCode::E_INCOMPATIBLE));
        
        testCode = BConstants::GC_TASK_TIMEOUT;
        res = service->DealWithGcErrcode(testStatus, testCode);
        EXPECT_EQ(res, static_cast<ErrCode> (BError::BackupErrorCode::E_MISSION_TIMEOUT));

        testCode = 1; // 其余错误码
        res = service->DealWithGcErrcode(testStatus, testCode);
        EXPECT_EQ(res, static_cast<ErrCode> (BError::BackupErrorCode::E_GC_FAILED));

        testCode = BConstants::GC_DEVICE_OK;
        testStatus = false;
        res = service->DealWithGcErrcode(testStatus, testCode);
        EXPECT_EQ(res, static_cast<ErrCode> (BError::BackupErrorCode::E_MISSION_TIMEOUT));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by DealWithGcErrcode.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_DealWithGcErrcode_0101";
}

/**
 * @tc.number: SUB_ServiceIncremental_StartCleanData_0101
 * @tc.name: SUB_ServiceIncremental_StartCleanData_0101
 * @tc.desc: 测试 StartCleanData 函数 dlopen failed
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_StartCleanData_0101, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_StartCleanData_0101";
    int testTriggerType = 0;
    unsigned int testWriteSize = 1000;
    unsigned int testWaitTime = 180;
    
    EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
    EXPECT_CALL(*srvMock, GetCallerName()).WillOnce(Return(BConstants::BUNDLE_DATA_CLONE));
    EXPECT_CALL(*dlFuncMock, dlopen(_, _)).WillOnce(Return(nullptr));
    auto res = service->StartCleanData(testTriggerType, testWriteSize, testWaitTime);
    EXPECT_EQ(res, static_cast<ErrCode> (BError::BackupErrorCode::E_INVAL));
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_StartCleanData_0101";
}

/**
 * @tc.number: SUB_ServiceIncremental_StartCleanData_0102
 * @tc.name: SUB_ServiceIncremental_StartCleanData_0102
 * @tc.desc: 测试 StartCleanData 函数 Caller wrong
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_StartCleanData_0102, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_StartCleanData_0102";
    int testTriggerType = 0;
    unsigned int testWriteSize = 1000;
    unsigned int testWaitTime = 180;
    
    EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
    EXPECT_CALL(*srvMock, GetCallerName()).WillOnce(Return("wrong caller"));
    auto res = service->StartCleanData(testTriggerType, testWriteSize, testWaitTime);
    EXPECT_EQ(res, static_cast<ErrCode> (BError::BackupErrorCode::E_PERM));
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_StartCleanData_0102";
}

/**
 * @tc.number: SUB_ServiceIncremental_StartCleanData_0103
 * @tc.name: SUB_ServiceIncremental_StartCleanData_0103
 * @tc.desc: 测试 StartCleanData 函数 dlsym failed
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_StartCleanData_0103, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_StartCleanData_0103";
    int testTriggerType = 0;
    unsigned int testWriteSize = 1000;
    unsigned int testWaitTime = 180;
    void *handle = &testTriggerType;
    EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
    EXPECT_CALL(*srvMock, GetCallerName()).WillOnce(Return(BConstants::BUNDLE_DATA_CLONE));
    EXPECT_CALL(*dlFuncMock, dlopen(_, _)).WillOnce(Return(handle));
    EXPECT_CALL(*dlFuncMock, dlsym(_, _)).WillOnce(Return(nullptr));
    auto res = service->StartCleanData(testTriggerType, testWriteSize, testWaitTime);
    EXPECT_EQ(res, static_cast<ErrCode> (BError::BackupErrorCode::E_INVAL));
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_StartCleanData_0103";
}

/**
 * @tc.number: SUB_ServiceIncremental_StartCleanData_0104
 * @tc.name: SUB_ServiceIncremental_StartCleanData_0104
 * @tc.desc: 测试 StartCleanData 函数 garbage collection failed
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_StartCleanData_0104, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_StartCleanData_0104";
    int testTriggerType = 0;
    unsigned int testWriteSize = 1000;
    unsigned int testWaitTime = 180;
    void *handle = &testTriggerType;
    EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
    EXPECT_CALL(*srvMock, GetCallerName()).WillOnce(Return(BConstants::BUNDLE_DATA_CLONE));
    EXPECT_CALL(*dlFuncMock, dlopen(_, _)).WillOnce(Return(handle));
    EXPECT_CALL(*dlFuncMock, dlsym(_, _)).WillOnce(Return(reinterpret_cast<void *>(gcFuncMock1)));
    auto res = service->StartCleanData(testTriggerType, testWriteSize, testWaitTime);
    EXPECT_EQ(res, static_cast<ErrCode> (BError::BackupErrorCode::E_GC_FAILED));
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_StartCleanData_0104";
}

/**
 * @tc.number: SUB_ServiceIncremental_StartCleanData_0105
 * @tc.name: SUB_ServiceIncremental_StartCleanData_0105
 * @tc.desc: 测试 StartCleanData 函数 execute garbage collection failed
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_StartCleanData_0105, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_StartCleanData_0105";
    int testTriggerType = 0;
    unsigned int testWriteSize = 1000;
    unsigned int testWaitTime = 180;
    void *handle = &testTriggerType;
    EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
    EXPECT_CALL(*srvMock, GetCallerName()).WillOnce(Return(BConstants::BUNDLE_DATA_CLONE));
    EXPECT_CALL(*dlFuncMock, dlopen(_, _)).WillOnce(Return(handle));
    EXPECT_CALL(*dlFuncMock, dlsym(_, _)).WillOnce(Return(reinterpret_cast<void *>(gcFuncMock2)));
    auto res = service->StartCleanData(testTriggerType, testWriteSize, testWaitTime);
    EXPECT_EQ(res, static_cast<ErrCode> (BError::BackupErrorCode::E_GC_FAILED));
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_StartCleanData_0105";
}

/**
 * @tc.number: SUB_ServiceIncremental_StartCleanData_0106
 * @tc.name: SUB_ServiceIncremental_StartCleanData_0106
 * @tc.desc: 测试 StartCleanData 函数 VerifyCaller failed
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_StartCleanData_0106, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_StartCleanData_0106";
    int testTriggerType = 0;
    unsigned int testWriteSize = 1000;
    unsigned int testWaitTime = 180;
    EXPECT_CALL(*srvMock, VerifyCaller()).WillOnce(Return(BError(BError::Codes::SA_INVAL_ARG).GetCode()));
    auto res = service->StartCleanData(testTriggerType, testWriteSize, testWaitTime);
    EXPECT_EQ(res, BError(BError::Codes::SA_INVAL_ARG).GetCode());
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_StartCleanData_0106";
}
}