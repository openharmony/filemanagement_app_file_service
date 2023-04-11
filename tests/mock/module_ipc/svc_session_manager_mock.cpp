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

#include "module_ipc/svc_session_manager.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <string>

#include "b_resources/b_constants.h"
#include "ext_extension_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
constexpr int SCHED_NUM = 1;
constexpr int FILE_NUM = 2;
static int32_t g_nFileReadyNum = 0;
static int32_t g_nAllBundlesFinished = 0;
} // namespace

void SvcSessionManager::VerifyCallerAndScenario(uint32_t clientToken, IServiceReverse::Scenario scenario) const
{
    GTEST_LOG_(INFO) << "VerifyCallerAndScenario";
}

void SvcSessionManager::Active(Impl newImpl)
{
    GTEST_LOG_(INFO) << "Active";
    extConnectNum_ = 0;
    impl_ = newImpl;
}

void SvcSessionManager::Deactive(const wptr<IRemoteObject> &remoteInAction, bool force)
{
    GTEST_LOG_(INFO) << "Deactive";
}

void SvcSessionManager::VerifyBundleName(string &bundleName)
{
    GTEST_LOG_(INFO) << "VerifyBundleName " << bundleName;
}

sptr<IServiceReverse> SvcSessionManager::GetServiceReverseProxy()
{
    GTEST_LOG_(INFO) << "GetServiceReverseProxy";
    return impl_.clientProxy;
}

IServiceReverse::Scenario SvcSessionManager::GetScenario()
{
    GTEST_LOG_(INFO) << "GetScenario";
    return impl_.scenario;
}

bool SvcSessionManager::OnBunleFileReady(const string &bundleName, const string &fileName)
{
    GTEST_LOG_(INFO) << "OnBunleFileReady";
    g_nFileReadyNum++;
    if (g_nFileReadyNum % FILE_NUM == SCHED_NUM) {
        GTEST_LOG_(INFO) << "OnBunleFileReady is true";
        return true;
    }
    GTEST_LOG_(INFO) << "OnBunleFileReady is false";
    return false;
}

UniqueFd SvcSessionManager::OnBunleExtManageInfo(const string &bundleName, UniqueFd fd)
{
    GTEST_LOG_(INFO) << "OnBunleExtManageInfo";
    return UniqueFd(-1);
}

void SvcSessionManager::RemoveExtInfo(const string &bundleName)
{
    GTEST_LOG_(INFO) << "RemoveExtInfo";
}

wptr<SvcBackupConnection> SvcSessionManager::GetExtConnection(const BundleName &bundleName)
{
    GTEST_LOG_(INFO) << "GetExtConnection";
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return nullptr;
    }
    if (!it->second.backUpConnection) {
        auto callDied = [](const string &&bundleName) {};
        auto callConnDone = [](const string &&bundleName) {};
        it->second.backUpConnection = sptr<SvcBackupConnection>(new SvcBackupConnection(callDied, callConnDone));
        sptr<BackupExtExtensionMock> mock = sptr(new BackupExtExtensionMock());
        it->second.backUpConnection->OnAbilityConnectDone({}, mock->AsObject(), 0);
    }
    return wptr(it->second.backUpConnection);
}

sptr<SvcBackupConnection> SvcSessionManager::GetBackupExtAbility(const string &bundleName)
{
    GTEST_LOG_(INFO) << "GetBackupExtAbility";
    return nullptr;
}

void SvcSessionManager::DumpInfo(const int fd, const std::vector<std::u16string> &args)
{
    GTEST_LOG_(INFO) << "DumpInfo";
}

void SvcSessionManager::InitClient(Impl &newImpl)
{
    GTEST_LOG_(INFO) << "InitClient";
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

map<BundleName, BackupExtInfo>::iterator SvcSessionManager::GetBackupExtNameMap(const string &bundleName)
{
    GTEST_LOG_(INFO) << "GetBackupExtNameMap";
    auto it = impl_.backupExtNameMap.find(bundleName);
    return it;
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
    return "com.example.app2backup";
}

void SvcSessionManager::AppendBundles(const vector<BundleName> &bundleNames)
{
    GTEST_LOG_(INFO) << "AppendBundles";
    BackupExtInfo info {};
    info.backupExtName = "com.example.app2backup";
    impl_.backupExtNameMap.insert(make_pair("com.example.app2backup", info));
}

void SvcSessionManager::Start() {}

void SvcSessionManager::Finish() {}

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

void SvcSessionManager::SetInstallState(const string &bundleName, const string &state) {}

string SvcSessionManager::GetInstallState(const string &bundleName)
{
    return "OK";
}

void SvcSessionManager::SetNeedToInstall(const std::string &bundleName, bool needToInstall) {}

bool SvcSessionManager::GetNeedToInstall(const std::string &bundleName)
{
    return false;
}

bool SvcSessionManager::NeedToUnloadService()
{
    return false;
}
} // namespace OHOS::FileManagement::Backup