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
#include <string>
#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "service_client.h"
#include "service_reverse.h"
#include "b_session_restore.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class ServiceClientTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};

public:
    static inline sptr<IService> proxy = nullptr;
};

void ServiceClientTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "begin SetUpTestCase";
    proxy = ServiceClient::GetInstance();
    if (proxy == nullptr) {
        GTEST_LOG_(INFO) << "Failed to get ServiceClient instance";
    }
}

void ServiceClientTest::TearDownTestCase()
{
    if (proxy != nullptr) {
        ServiceClient::InvaildInstance();
        proxy = nullptr;
    }
    ServiceClient::InvaildInstance();
}

/**
 * @tc.number: SUB_service_client_test_0100
 * @tc.name: SUB_service_client_test_0100
 * @tc.desc: 测试 InitIncrementalBackupSession 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(ServiceClientTest, SUB_service_client_test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_0100";
    EXPECT_NE(proxy, nullptr);
    ServiceClient::InvaildInstance();
    proxy=ServiceClient::serviceProxy_;
    EXPECT_EQ(proxy, nullptr);
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_0100";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_0200";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    ErrCode ret = proxy->Start();
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_PERM));
    ret = proxy->Finish();
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_PERM));
    ret = proxy->Release();
    EXPECT_NE(ret, BError(BError::BackupErrorCode::E_PERM));
    ret = proxy->GetAppLocalListAndDoIncrementalBackup();
    EXPECT_NE(ret, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_0200";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_0300";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    std::string bundleName;
    int32_t result = -1;
    ErrCode ret = proxy->Cancel(bundleName, result);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK));
    bundleName = "";
    ret = proxy->Cancel(bundleName, result);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK));
    bundleName = "test";
    ret = proxy->Cancel(bundleName, result);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_CANCEL_UNSTARTED_TASK));
    EXPECT_NE(result, 0);
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_0300";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_0400";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    ErrCode err = -1;
    ErrCode ret = proxy->AppIncrementalDone(err);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    err = 0;
    ret = proxy->AppIncrementalDone(err);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    err = 1;
    ret = proxy->AppIncrementalDone(err);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_0400";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_0500";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    ErrCode err = -1;
    ErrCode ret = proxy->AppDone(err);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    err = 0;
    ret = proxy->AppDone(err);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    err = 1;
    ret = proxy->AppDone(err);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_0500";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_0600";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    int fd = -1;
    ErrCode ret = proxy->GetLocalCapabilities(fd);
    EXPECT_EQ(ret, BError(BError::Codes::OK));
    fd = -2;
    ret = proxy->GetLocalCapabilitiesForBundleInfos(fd);
    EXPECT_EQ(ret, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_0600";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_0700";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    std::string bundleName;
    std::string fileName;
    ErrCode ret = proxy->GetIncrementalFileHandle(bundleName, fileName);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    bundleName = "";
    fileName = "";
    ret = proxy->GetIncrementalFileHandle(bundleName, fileName);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    bundleName = "test";
    fileName = "test";
    ret = proxy->GetIncrementalFileHandle(bundleName, fileName);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_0700";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_0800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_0800";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    std::string bundleName;
    std::string fileName;
    bool booleanValue = false;
    ErrCode ret = proxy->GetBackupInfo(bundleName, fileName);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_BEF));
    EXPECT_EQ(bundleName, "");
    EXPECT_EQ(fileName, "");
    booleanValue = true;
    ret = proxy->StartExtTimer(booleanValue);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    booleanValue = false;
    ret = proxy->StartExtTimer(booleanValue);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    booleanValue = true;
    ret = proxy->StartFwkTimer(booleanValue);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    booleanValue = false;
    ret = proxy->StartFwkTimer(booleanValue);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    booleanValue = true;
    ret = proxy->StopExtTimer(booleanValue);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    booleanValue = false;
    ret = proxy->StopExtTimer(booleanValue);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    bool isExt = true;
    bundleName = "";
    ret = proxy->UpdateTimer(bundleName, 0, isExt);
    EXPECT_NE(ret, BError(BError::BackupErrorCode::E_INVAL));
    EXPECT_EQ(isExt, false);
    bundleName = "test";
    ret = proxy->UpdateTimer(bundleName, 10, isExt);
    EXPECT_EQ(ret, BError(BError::Codes::OK));
    EXPECT_EQ(isExt, false);
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_0800";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_0900";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    std::string stringVal = "";
    ErrCode ret = proxy->ReportAppProcessInfo(stringVal, BackupRestoreScenario::FULL_BACKUP);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    stringVal = "";
    ret = proxy->ReportAppProcessInfo(stringVal, BackupRestoreScenario::FULL_BACKUP);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    stringVal = "test";
    ret = proxy->ReportAppProcessInfo(stringVal, BackupRestoreScenario::FULL_BACKUP);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    int64_t val = -1;
    ret = proxy->RefreshDataSize(val);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    val = 1;
    ret = proxy->RefreshDataSize(val);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    stringVal = "";
    ret = proxy->ServiceResultReport(stringVal, BackupRestoreScenario::FULL_BACKUP, 0);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    stringVal = "";
    ret = proxy->ServiceResultReport(stringVal, BackupRestoreScenario::FULL_BACKUP, 0);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_0900";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_1000, testing::ext::TestSize.Level1)
{
    std::string bundleName = "";
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_1000";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    BFileInfo *fileInfo = nullptr;
    BFileInfo bf {bundleName, "", 0};
    fileInfo = &bf;
    ErrCode ret = proxy->PublishFile(*fileInfo);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->PublishIncrementalFile(*fileInfo);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->PublishSAIncrementalFile(*fileInfo, 0);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_1000";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_1100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_1100";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    int fd = 0;
    int32_t restoreType = -1;
    int32_t userid = -1;
    std::vector<BundleName> bundleNames;
    std::vector<BundleName> detailInfos;
    std::vector<BIncrementalData> bundlesToBackup;
    ErrCode ret = proxy->AppendBundlesRestoreSessionDataByDetail(fd, bundleNames, detailInfos, restoreType, userid);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->AppendBundlesRestoreSessionData(fd, bundleNames, restoreType, userid);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->AppendBundlesIncrementalBackupSessionWithBundleInfos(bundlesToBackup, detailInfos);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    ret = proxy->AppendBundlesBackupSession(bundleNames);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    ret = proxy->AppendBundlesDetailsBackupSession(bundleNames, detailInfos);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    bundleNames.push_back("test");
    ret = proxy->AppendBundlesRestoreSessionDataByDetail(fd, bundleNames, detailInfos, restoreType, userid);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->AppendBundlesRestoreSessionData(fd, bundleNames, restoreType, userid);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->AppendBundlesIncrementalBackupSessionWithBundleInfos(bundlesToBackup, detailInfos);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    ret = proxy->AppendBundlesBackupSession(bundleNames);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    ret = proxy->AppendBundlesDetailsBackupSession(bundleNames, detailInfos);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    detailInfos.push_back("test");
    ret = proxy->AppendBundlesRestoreSessionDataByDetail(fd, bundleNames, detailInfos, restoreType, userid);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->AppendBundlesRestoreSessionData(fd, bundleNames, restoreType, userid);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->AppendBundlesIncrementalBackupSessionWithBundleInfos(bundlesToBackup, detailInfos);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    ret = proxy->AppendBundlesDetailsBackupSession(bundleNames, detailInfos);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    restoreType = 1;
    userid = 1;
    ret = proxy->AppendBundlesRestoreSessionDataByDetail(fd, bundleNames, detailInfos, restoreType, userid);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->AppendBundlesRestoreSessionData(fd, bundleNames, restoreType, userid);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->AppendBundlesIncrementalBackupSessionWithBundleInfos(bundlesToBackup, detailInfos);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_1100";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_1200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_1200";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    sptr<IServiceReverse> srptr = nullptr;
    ErrCode ret = proxy->InitRestoreSession(srptr);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->InitBackupSession(srptr);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->InitIncrementalBackupSession(srptr);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    std::string errMsg = "";
    std::string result = "err";
    ret = proxy->InitRestoreSessionWithErrMsg(srptr, errMsg);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->InitBackupSessionWithErrMsg(srptr, errMsg);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->InitIncrementalBackupSessionWithErrMsg(srptr, errMsg);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    BSessionRestore::Callbacks callback;
    srptr = sptr<IServiceReverse>(new ServiceReverse(callback));
    ret = proxy->InitRestoreSession(srptr);
    EXPECT_EQ(ret, BError(BError::Codes::OK));
    ret = proxy->InitBackupSession(srptr);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_CONFLICT));
    ret = proxy->InitIncrementalBackupSession(srptr);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_CONFLICT));

    ret = proxy->InitRestoreSessionWithErrMsg(srptr, errMsg);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_CONFLICT));
    EXPECT_EQ(errMsg, "");
    ret = proxy->InitBackupSessionWithErrMsg(srptr, errMsg);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_CONFLICT));
    EXPECT_EQ(errMsg, "");
    ret = proxy->InitIncrementalBackupSessionWithErrMsg(srptr, errMsg);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_CONFLICT));
    EXPECT_EQ(errMsg, "");

    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_1200";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_1300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_1300";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);

    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_1300";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_1400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_1400";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    std::string bundleName;
    std::string fileName;
    bool isExt = true;
    bundleName = "";
    ErrCode ret = proxy->UpdateSendRate(bundleName, 0, isExt);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    EXPECT_TRUE(isExt);
    bundleName = "test";
    ret = proxy->UpdateSendRate(bundleName, 10, isExt);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    EXPECT_TRUE(isExt);
    bundleName = "";
    ret = proxy->AppIncrementalFileReady(bundleName, 0, 0, 0);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    bundleName = "test";
    ret = proxy->AppIncrementalFileReady(bundleName, 1, 1, 0);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    fileName = "";
    ret = proxy->AppFileReady(fileName, 0, 0);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    fileName = "name";
    ret = proxy->AppFileReady(fileName, -1, 0);
    EXPECT_NE(ret, BError(BError::Codes::OK));
    ret = proxy->AppFileReady(fileName, 0, 0);
    EXPECT_NE(ret, BError(BError::BackupErrorCode::E_INVAL));
    bundleName = "";
    fileName = "";
    ret = proxy->GetFileHandle(bundleName, fileName);
    EXPECT_EQ(ret, BError(BError::Codes::OK));
    EXPECT_EQ(fileName, "");
    bundleName = "test";
    ret = proxy->GetFileHandle(bundleName, fileName);
    EXPECT_EQ(ret, BError(BError::Codes::OK));
    EXPECT_EQ(fileName, "");
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_1400";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_1500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_1500";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    ErrCode ret = proxy->HandleExtDisconnect(false);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    ret = proxy->HandleExtDisconnect(true);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_1500";
}

HWTEST_F(ServiceClientTest, SUB_service_client_test_1600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceClientTest-begin SUB_service_client_test_1600";
    proxy = ServiceClient::GetInstance();
    EXPECT_NE(proxy, nullptr);
    bool isExtOnRelease = false;
    ErrCode ret = proxy->GetExtOnRelease(isExtOnRelease);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    EXPECT_EQ(isExtOnRelease, false);
    isExtOnRelease = true;
    ret = proxy->GetExtOnRelease(isExtOnRelease);
    EXPECT_EQ(ret, BError(BError::BackupErrorCode::E_INVAL));
    EXPECT_EQ(isExtOnRelease, true);
    GTEST_LOG_(INFO) << "ServiceClientTest-end SUB_service_client_test_1600";
}
} // namespace OHOS::FileManagement::Backup