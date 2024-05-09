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
    MOCK_METHOD1(InitRestoreSession, ErrCode(sptr<IServiceReverse> remote));
    MOCK_METHOD1(InitBackupSession, ErrCode(sptr<IServiceReverse> remote));
    MOCK_METHOD0(Start, ErrCode());
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
    MOCK_METHOD0(GetLocalCapabilities, UniqueFd());
    MOCK_METHOD1(PublishFile, ErrCode(const BFileInfo &fileInfo));
    MOCK_METHOD2(AppFileReady, ErrCode(const std::string &fileName, UniqueFd fd));
    MOCK_METHOD3(AppFileReady, ErrCode(const std::string &fileName, UniqueFd fd, int32_t errCode));
    MOCK_METHOD1(AppDone, ErrCode(ErrCode errCode));
    MOCK_METHOD2(ServiceResultReport, ErrCode(const std::string restoreRetInfo, BackupRestoreScenario scenario));
    MOCK_METHOD2(GetFileHandle, ErrCode(const std::string &bundleName, const std::string &fileName));
    MOCK_METHOD5(AppendBundlesRestoreSession, ErrCode(UniqueFd fd, const std::vector<BundleName> &bundleNames,
        const std::vector<std::string> &detailInfos, RestoreTypeEnum restoreType, int32_t userId));
    MOCK_METHOD4(AppendBundlesRestoreSession, ErrCode(UniqueFd fd, const std::vector<BundleName> &bundleNames,
        RestoreTypeEnum restoreType, int32_t userId));
    MOCK_METHOD1(AppendBundlesBackupSession, ErrCode(const std::vector<BundleName> &bundleNames));
    MOCK_METHOD0(Finish, ErrCode());
    MOCK_METHOD0(Release, ErrCode());
    MOCK_METHOD1(GetLocalCapabilitiesIncremental, UniqueFd(const std::vector<BIncrementalData> &bundleNames));
    MOCK_METHOD1(InitIncrementalBackupSession, ErrCode(sptr<IServiceReverse> remote));
    MOCK_METHOD1(AppendBundlesIncrementalBackupSession, ErrCode(const std::vector<BIncrementalData> &bundlesToBackup));
    MOCK_METHOD1(PublishIncrementalFile, ErrCode(const BFileInfo &fileInfo));
    MOCK_METHOD3(AppIncrementalFileReady, ErrCode(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd));
    MOCK_METHOD4(AppIncrementalFileReady, ErrCode(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd,
        int32_t errCode));
    MOCK_METHOD1(AppIncrementalDone, ErrCode(ErrCode errCode));
    MOCK_METHOD2(GetIncrementalFileHandle, ErrCode(const std::string &bundleName, const std::string &fileName));
    MOCK_METHOD2(GetBackupInfo, ErrCode(BundleName &bundleName, std::string &result));
    MOCK_METHOD3(UpdateTimer, ErrCode(BundleName &bundleName, uint32_t timeOut, bool &result));
};
} // End of namespace OHOS::FileManagement::Backup
#endif // TEST_UNITTEST_SERVICE_PROXY_MOCK_H
