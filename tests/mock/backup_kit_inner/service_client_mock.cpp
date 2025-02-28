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

#include <sstream>
#include "iremote_object.h"
#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_radar/b_radar.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "service_client.h"
#include "system_ability_definition.h"
#include "svc_death_recipient.h"
#include "hitrace_meter.h"
#include "iservice.h"
#include <mutex>
#include "backup_file_info.h"
#include "backup_incremental_data.h"
#include "iservice.h"
#include "iremote_object_mock.h"
#include "utils_mock_global_variable.h"
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

sptr<IService> ServiceClient::GetServiceProxyPointer()
{
    return serviceProxy_;
}

sptr<IService> ServiceClient::GetInstance()
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

void ServiceClient::InvaildInstance()
{
    serviceProxy_ = nullptr;
}

void ServiceClient::ServiceProxyLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
                                                                         const OHOS::sptr<IRemoteObject> &remoteObject)
{
}

void ServiceClient::ServiceProxyLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId) {}
}