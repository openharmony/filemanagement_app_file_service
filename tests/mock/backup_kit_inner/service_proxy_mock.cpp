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

int32_t ServiceProxy::InitRestoreSession(sptr<IServiceReverse> remote, const vector<BundleName> &bundleNames)
{
    if (!GetMockInitBackupOrRestoreSession()) {
        return 1;
    }
    return 0;
}

int32_t ServiceProxy::InitBackupSession(sptr<IServiceReverse> remote,
                                        UniqueFd fd,
                                        const vector<BundleName> &bundleNames)
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

ErrCode ServiceProxy::AppFileReady(const string &fileName, UniqueFd fd)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::AppDone(ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode ServiceProxy::GetExtFileName(string &bundleName, string &fileName)
{
    return BError(BError::Codes::OK);
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