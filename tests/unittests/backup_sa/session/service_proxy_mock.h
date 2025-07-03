/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef TEST_UNITTEST_SERVICE_PROXY_MOCK_H
#define TEST_UNITTEST_SERVICE_PROXY_MOCK_H

#include <gmock/gmock.h>

#include "service_proxy.h"

namespace OHOS::FileManagement::Backup {
class ServiceProxyMock : public ServiceProxy {
public:
    explicit ServiceProxyMock(const sptr<IRemoteObject> &impl) : ServiceProxy(impl) {}
    ~ServiceProxyMock() override {}

public:
    MOCK_METHOD1(InitRestoreSession, ErrCode(const sptr<IServiceReverse> &remote));
    MOCK_METHOD3(InitRestoreSessionWithErrMsg, ErrCode(const sptr<IServiceReverse> &remote,
                                                       int32_t &errCodeForMsg, std::string &errMsg));
    MOCK_METHOD1(InitBackupSession, ErrCode(const sptr<IServiceReverse> &remote));
    MOCK_METHOD3(InitBackupSessionWithErrMsg, ErrCode(const sptr<IServiceReverse> &remote,
                                                      int32_t &errCodeForMsg, std::string &errMsg));
    MOCK_METHOD0(Start, ErrCode());
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
    MOCK_METHOD1(GetLocalCapabilitiesForBundleInfos, ErrCode(int32_t &));
    MOCK_METHOD1(PublishFile, ErrCode(const BFileInfo &fileInfo));
    MOCK_METHOD2(AppFileReady, ErrCode(const std::string &fileName, int fd));
    MOCK_METHOD3(AppFileReady, ErrCode(const std::string &fileName, int fd, int32_t errCode));
    MOCK_METHOD1(AppDone, ErrCode(int32_t errCode));

    MOCK_METHOD2(GetFileHandle, ErrCode(const std::string &bundleName, const std::string &fileName));

    MOCK_METHOD1(AppendBundlesBackupSession, ErrCode(const std::vector<BundleName> &bundleNames));
    MOCK_METHOD0(Finish, ErrCode());
    MOCK_METHOD0(Release, ErrCode());
    MOCK_METHOD2(CancelForResult, ErrCode(const std::string &bundleName, int32_t &result));
    MOCK_METHOD2(GetLocalCapabilitiesIncremental, ErrCode(const std::vector<BIncrementalData> &bundleNames, int &fd));
    MOCK_METHOD1(InitIncrementalBackupSession, ErrCode(const sptr<IServiceReverse> &remote));
    MOCK_METHOD3(InitIncrementalBackupSessionWithErrMsg,
                 ErrCode(const sptr<IServiceReverse> &remote, int32_t &errCodeForMsg, std::string &errMsg));
    MOCK_METHOD1(AppendBundlesIncrementalBackupSession, ErrCode(const std::vector<BIncrementalData> &bundlesToBackup));
    MOCK_METHOD2(AppendBundlesIncrementalBackupSessionWithBundleInfos,
                 ErrCode(const std::vector<BIncrementalData> &bundlesToBackup, const std::vector<std::string> &infos));
    MOCK_METHOD1(PublishIncrementalFile, ErrCode(const BFileInfo &fileInfo));
    MOCK_METHOD2(PublishSAIncrementalFile, ErrCode(const BFileInfo &fileInfo, int fd));
    MOCK_METHOD3(AppIncrementalFileReady, ErrCode(const std::string &fileName, int fd, int manifestFd));
    MOCK_METHOD4(AppIncrementalFileReady,
                 ErrCode(const std::string &fileName, int fd, int manifestFd, int32_t errCode));
    MOCK_METHOD2(AppIncrementalFileReadyWithoutFd,
                 ErrCode(const std::string &fileName, int32_t errCode));
    MOCK_METHOD1(AppIncrementalDone, ErrCode(int32_t errCode));
    MOCK_METHOD2(GetIncrementalFileHandle, ErrCode(const std::string &bundleName, const std::string &fileName));
    MOCK_METHOD2(GetBackupInfo, ErrCode(const BundleName &bundleName, std::string &result));
    MOCK_METHOD3(UpdateTimer, ErrCode(const BundleName &bundleName, uint32_t timeout, bool &result));
    MOCK_METHOD0(GetAppLocalListAndDoIncrementalBackup, ErrCode());
    MOCK_METHOD1(StopExtTimer, ErrCode(bool &isExtStop));
    MOCK_METHOD1(RefreshDataSize, ErrCode(int64_t totalsize));
    MOCK_METHOD5(
        AppendBundlesRestoreSessionDataByDetail,
        ErrCode(int32_t, const std::vector<std::string> &, const std::vector<std::string> &, int32_t, int32_t));
    MOCK_METHOD4(AppendBundlesRestoreSessionData,
                 ErrCode(int32_t, const std::vector<std::string> &, int32_t, int32_t));
    MOCK_METHOD2(GetBackupDataSize, ErrCode(bool isPreciseScan, const std::vector<BIncrementalData> &bundleNameList));
    MOCK_METHOD1(CleanBundleTempDir, ErrCode(const std::string &bundleName));
    MOCK_METHOD1(HandleExtDisconnect, ErrCode(bool isIncBackup));
    MOCK_METHOD1(GetExtOnRelease, ErrCode(bool &isExtOnRelease));
    MOCK_METHOD3(GetCompatibilityInfo, ErrCode(const std::string&, const std::string&, std::string&));
};
} // End of namespace OHOS::FileManagement::Backup
#endif // TEST_UNITTEST_SERVICE_PROXY_MOCK_H
