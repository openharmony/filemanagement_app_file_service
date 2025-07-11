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

#include "module_ipc/svc_session_manager.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>

#include "b_resources/b_constants.h"
#include "ext_extension_mock.h"
#include "../common/include/test_common.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
constexpr int SCHED_NUM = 1;
constexpr int FILE_NUM = 2;
static int32_t g_nFileReadyNum = 0;
static int32_t g_nAllBundlesFinished = 0;
} // namespace

ErrCode SvcSessionManager::VerifyCallerAndScenario(uint32_t clientToken, IServiceReverseType::Scenario scenario) const
{
    GTEST_LOG_(INFO) << "VerifyCallerAndScenario";
    return BError(BError::Codes::OK);
}

ErrCode SvcSessionManager::Active(Impl newImpl, bool force)
{
    GTEST_LOG_(INFO) << "Active";
    extConnectNum_ = 0;
    impl_ = newImpl;
    return BError(BError::Codes::OK);
}

ErrCode SvcSessionManager::Deactive(const wptr<IRemoteObject> &remoteInAction, bool force)
{
    GTEST_LOG_(INFO) << "Deactive";
    return BError(BError::Codes::OK);
}

ErrCode SvcSessionManager::VerifyBundleName(string &bundleName)
{
    GTEST_LOG_(INFO) << "VerifyBundleName " << bundleName;
    return BError(BError::Codes::OK);
}

sptr<IServiceReverse> SvcSessionManager::GetServiceReverseProxy()
{
    GTEST_LOG_(INFO) << "GetServiceReverseProxy, clientProxy is null:" << (impl_.clientProxy == nullptr);
    return impl_.clientProxy;
}

IServiceReverseType::Scenario SvcSessionManager::GetScenario()
{
    GTEST_LOG_(INFO) << "GetScenario";
    return impl_.scenario;
}

bool SvcSessionManager::OnBundleFileReady(const string &bundleName, const string &fileName)
{
    GTEST_LOG_(INFO) << "OnBundleFileReady";
    g_nFileReadyNum++;
    if (g_nFileReadyNum % FILE_NUM == SCHED_NUM) {
        GTEST_LOG_(INFO) << "OnBundleFileReady is true";
        return true;
    }
    GTEST_LOG_(INFO) << "OnBundleFileReady is false";
    return false;
}

UniqueFd SvcSessionManager::OnBundleExtManageInfo(const string &bundleName, UniqueFd fd)
{
    GTEST_LOG_(INFO) << "OnBundleExtManageInfo";
    return UniqueFd(-1);
}

void SvcSessionManager::RemoveExtInfo(const string &bundleName)
{
    GTEST_LOG_(INFO) << "RemoveExtInfo";
    impl_.clientToken = CLEARED_CLIENT_TOKEN;
}

wptr<SvcBackupConnection> SvcSessionManager::GetExtConnection(const BundleName &bundleName)
{
    GTEST_LOG_(INFO) << "GetExtConnection";
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        GTEST_LOG_(INFO) << "GetExtConnection not find bundle";
        return nullptr;
    }
    if (!it->second.backUpConnection) {
        GTEST_LOG_(INFO) << "connection is null, init now!";
        auto callDied = [](const string &&bundleName, bool isCleanCalled = false) {};
        auto callConnected = [](const string &&bundleName) {};
        it->second.backUpConnection = sptr<SvcBackupConnection>(new SvcBackupConnection(callDied, callConnected,
            bundleName));
        sptr<BackupExtExtensionMock> mock = sptr(new BackupExtExtensionMock());
        it->second.backUpConnection->OnAbilityConnectDone({}, mock->AsObject(), 0);
    }
    return wptr(it->second.backUpConnection);
}

sptr<SvcBackupConnection> SvcSessionManager::GetBackupAbilityExt(const string &bundleName)
{
    GTEST_LOG_(INFO) << "GetBackupAbilityExt";
    return sptr<SvcBackupConnection>(new SvcBackupConnection(nullptr, nullptr, bundleName));
}

void SvcSessionManager::DumpInfo(const int fd, const std::vector<std::u16string> &args)
{
    GTEST_LOG_(INFO) << "DumpInfo";
}

ErrCode SvcSessionManager::InitClient(Impl &newImpl)
{
    GTEST_LOG_(INFO) << "InitClient";
    return BError(BError::Codes::OK);
}

void SvcSessionManager::SetExtFileNameRequest(const string &bundleName, const string &fileName)
{
    GTEST_LOG_(INFO) << "SetExtFileNameRequest";
}

std::set<std::string> SvcSessionManager::GetExtFileNameRequest(const std::string &bundleName)
{
    GTEST_LOG_(INFO) << "GetExtFileNameRequest";
    std::set<std::string> fileNameInfo;
    fileNameInfo.insert("testName");
    fileNameInfo.insert("fileName");
    return fileNameInfo;
}

std::tuple<bool, map<BundleName, BackupExtInfo>::iterator> SvcSessionManager::GetBackupExtNameMap(
    const string &bundleName)
{
    GTEST_LOG_(INFO) << "GetBackupExtNameMap";
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return {false, impl_.backupExtNameMap.end()};
    }
    return {true, it};
}

bool SvcSessionManager::GetSchedBundleName(string &bundleName)
{
    if (extConnectNum_ == 0) {
        GTEST_LOG_(INFO) << "GetSchedBundleName is zero";
        extConnectNum_++;
        return false;
    } else if (extConnectNum_ == SCHED_NUM || extConnectNum_ == (SCHED_NUM + 1)) {
        GTEST_LOG_(INFO) << "GetSchedBundleName is one two";
        bundleName = "com.example.app2backup";
        extConnectNum_++;
        return true;
    }

    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return false;
    }
    it->second.backupExtName = bundleName;
    GTEST_LOG_(INFO) << "GetSchedBundleName is " << it->second.backupExtName;
    return true;
}

BConstants::ServiceSchedAction SvcSessionManager::GetServiceSchedAction(const std::string &bundleName)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return BConstants::ServiceSchedAction::WAIT;
    }
    GTEST_LOG_(INFO) << "GetServiceSchedAction is " << it->second.schedAction;
    return it->second.schedAction;
}

void SvcSessionManager::SetServiceSchedAction(const string &bundleName, BConstants::ServiceSchedAction action)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return;
    }
    it->second.schedAction = action;
}

string SvcSessionManager::GetBackupExtName(const string &bundleName)
{
    GTEST_LOG_(INFO) << "GetBackupExtName " << bundleName;
    return BUNDLE_NAME;
}

void SvcSessionManager::SetBackupExtInfo(const string &bundleName, const string &extInfo)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return;
    }
    it->second.extInfo = extInfo;
}

std::string SvcSessionManager::GetBackupExtInfo(const string &bundleName)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return "";
    }
    return it->second.extInfo;
}

void SvcSessionManager::SetBackupExtName(const string &bundleName, const string &backupExtName)
{
    GTEST_LOG_(INFO) << "call SetBackupExtName bundleName=" << bundleName << ",extName=" << backupExtName;
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        GTEST_LOG_(INFO) << "not find";
        return;
    }
    it->second.backupExtName = backupExtName;
}

std::weak_ptr<SABackupConnection> SvcSessionManager::GetSAExtConnection(const BundleName &bundleName)
{
    GTEST_LOG_(INFO) << "call GetSAExtConnection";
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return std::weak_ptr<SABackupConnection>();
    }
    if (!it->second.saBackupConnection) {
        auto callDied = [](const string &&bundleName) {};
        auto callConnected = [](const string &&bundleName) {};
        auto callBackup = [](const std::string &&bundleName, const int &&fd, const std::string &&result,
                             const ErrCode &&errCode) {};
        auto callRestore = [](const std::string &&bundleName, const std::string &&result, const ErrCode &&errCode) {};
        it->second.saBackupConnection =
            std::make_shared<SABackupConnection>(callDied, callConnected, callBackup, callRestore);
    }
    return std::weak_ptr<SABackupConnection>(it->second.saBackupConnection);
}

void SvcSessionManager::AppendBundles(const vector<BundleName> &bundleNames, vector<BundleName> &failedBundles)
{
    GTEST_LOG_(INFO) << "AppendBundles";
    BackupExtInfo info {};
    info.backupExtName = BUNDLE_NAME;
    impl_.backupExtNameMap.insert(make_pair(BUNDLE_NAME, info));
    for (auto bundleName : bundleNames) {
        if (bundleName == BUNDLE_NAME_FALSE) {
            failedBundles.push_back(BUNDLE_NAME_FALSE);
        }
    }
}

sptr<SvcBackupConnection> SvcSessionManager::CreateBackupConnection(const BundleName &bundleName)
{
    GTEST_LOG_(INFO) << "CreateBackupConnection";
    return GetBackupAbilityExt(bundleName);
}

ErrCode SvcSessionManager::Start()
{
    if (sessionCnt_.load() == START_RETURN_FALSE) {
        GTEST_LOG_(INFO) << "call Start false";
        sessionCnt_.store(0);
        return BError(BError::Codes::EXT_INVAL_ARG);
    }
    GTEST_LOG_(INFO) << "call Start ok";
    return BError(BError::Codes::OK);
}

ErrCode SvcSessionManager::Finish()
{
    return BError(BError::Codes::OK);
}

bool SvcSessionManager::IsOnAllBundlesFinished()
{
    GTEST_LOG_(INFO) << "IsOnAllBundlesFinished";
    g_nAllBundlesFinished++;
    if (g_nAllBundlesFinished % FILE_NUM == SCHED_NUM) {
        GTEST_LOG_(INFO) << "IsOnAllBundlesFinished is true";
        return true;
    }
    GTEST_LOG_(INFO) << "IsOnAllBundlesFinished is false";
    return false;
}

bool SvcSessionManager::IsOnOnStartSched()
{
    return true;
}

bool SvcSessionManager::NeedToUnloadService()
{
    return false;
}

int32_t SvcSessionManager::GetSessionUserId()
{
    return impl_.userId;
}

void SvcSessionManager::SetSessionUserId(int32_t userId)
{
    impl_.userId = userId;
}

string SvcSessionManager::GetSessionCallerName()
{
    return impl_.callerName;
}

string SvcSessionManager::GetSessionActiveTime()
{
    return impl_.activeTime;
}

void SvcSessionManager::SetBundleRestoreType(const std::string &bundleName, RestoreTypeEnum restoreType)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return;
    }
    it->second.restoreType = restoreType;
}

RestoreTypeEnum SvcSessionManager::GetBundleRestoreType(const std::string &bundleName)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return RestoreTypeEnum::RESTORE_DATA_READDY;
    }
    return it->second.restoreType;
}

void SvcSessionManager::SetBundleVersionCode(const std::string &bundleName, int64_t versionCode)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return;
    }
    it->second.versionCode = versionCode;
}

int64_t SvcSessionManager::GetBundleVersionCode(const std::string &bundleName)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return 0;
    }
    return it->second.versionCode;
}

void SvcSessionManager::SetBundleVersionName(const std::string &bundleName, std::string versionName)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return;
    }
    it->second.versionName = versionName;
}

std::string SvcSessionManager::GetBundleVersionName(const std::string &bundleName)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return "";
    }
    return it->second.versionName;
}

void SvcSessionManager::SetBundleUserId(const std::string &bundleName, const int32_t userId)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return;
    }
    it->second.userId = userId;
}

int32_t SvcSessionManager::GetBundleUserId(const std::string &bundleName)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return 0;
    }
    return it->second.userId;
}

void SvcSessionManager::SetBundleDataSize(const std::string &bundleName, int64_t dataSize)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return;
    }
    it->second.dataSize = dataSize;
}

bool SvcSessionManager::StartFwkTimer(const std::string &bundleName, const Utils::Timer::TimerCallback &callback)
{
    return true;
}

bool SvcSessionManager::StopFwkTimer(const std::string &bundleName)
{
    return true;
}

bool SvcSessionManager::StartExtTimer(const std::string &bundleName, const Utils::Timer::TimerCallback &callback)
{
    return true;
}

bool SvcSessionManager::StopExtTimer(const std::string &bundleName)
{
    return true;
}

bool SvcSessionManager::UpdateTimer(const std::string &bundleName, uint32_t timeout,
    const Utils::Timer::TimerCallback &callback)
{
    return true;
}

void SvcSessionManager::IncreaseSessionCnt(const std::string funcName) {}

void SvcSessionManager::DecreaseSessionCnt(const std::string funcName) {}

int32_t SvcSessionManager::GetMemParaCurSize()
{
    return 0;
}

void SvcSessionManager::SetMemParaCurSize(int32_t size) {}

ErrCode SvcSessionManager::ClearSessionData()
{
    return 0;
}

bool SvcSessionManager::GetIsIncrementalBackup()
{
    GTEST_LOG_(INFO) << "call GetIsIncrementalBackup, callerName: " << impl_.callerName;
    if (impl_.callerName == BUNDLE_NAME_FALSE) {
        return false;
    }
    return true;
}

bool SvcSessionManager::ValidRestoreDataType(RestoreTypeEnum restoreDataType)
{
    return impl_.restoreDataType == restoreDataType;
}

uint32_t SvcSessionManager::GetTimeoutValue(const std::string &bundleName)
{
    return 0;
}

SvcSessionManager::Impl SvcSessionManager::GetImpl()
{
    return impl_;
}

int SvcSessionManager::GetSessionCnt()
{
    return sessionCnt_.load();
}

void SvcSessionManager::SetClearDataFlag(const std::string &bundleName, bool isNotClear) {}

bool SvcSessionManager::GetClearDataFlag(const std::string &bundleName)
{
    if (bundleName == BUNDLE_NAME_FALSE) {
        return false;
    }
    return true;
}

void SvcSessionManager::SetIncrementalData(const BIncrementalData &incrementalData) {}

int32_t SvcSessionManager::GetIncrementalManifestFd(const string &bundleName)
{
    return 0;
}

int64_t SvcSessionManager::GetLastIncrementalTime(const string &bundleName)
{
    return 0;
}

bool SvcSessionManager::CleanAndCheckIfNeedWait(ErrCode &ret, std::vector<std::string> &bundleNameList)
{
    return false;
}

void SvcSessionManager::SetPublishFlag(const std::string &bundleName) {}

void SvcSessionManager::SetImplRestoreType(const RestoreTypeEnum restoreType) {}

void SvcSessionManager::SetIsReadyLaunch(const std::string &bundleName) {}

void SvcSessionManager::SetOldBackupVersion(const std::string &backupVersion)
{
    impl_.oldBackupVersion = backupVersion;
}

std::string SvcSessionManager::GetOldBackupVersion()
{
    return impl_.oldBackupVersion;
}

void SvcSessionManager::UpdateDfxInfo(const std::string &bundleName, uint64_t uniqId)
{
}

void SvcSessionManager::HandleOnRelease(sptr<IExtension> proxy)
{
}

void SvcSessionManager::SetIsRestoreEnd(const std::string &bundleName)
{
}

bool SvcSessionManager::GetIsRestoreEnd(const std::string &bundleName)
{
    return false;
}
} // namespace OHOS::FileManagement::Backup
