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

#include "service_client.h"
#include "iservice.h"
#include "service_proxy.h"
namespace OHOS::FileManagement::Backup {
using namespace std;

bool ServiceClient::CheckServiceProxy()
{
    serviceProxy_ = ServiceClient::GetInstance();
    bool isNull = false;
    if (serviceProxy_ != nullptr) {
        isNull = true;
    }
    return isNull;
}
sptr<IService> ServiceClient::GetServiceProxyPointer() { return serviceProxy_; }
sptr<IService> ServiceClient::GetInstance() { return serviceProxy_; }
void ServiceClient::InvaildInstance() {}
void ServiceClient::ServiceProxyLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
                                                                         const OHOS::sptr<IRemoteObject> &remoteObject)
{}
void ServiceClient::ServiceProxyLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId) {}
}
