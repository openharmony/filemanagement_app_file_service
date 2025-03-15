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

#ifndef TEST_UNITTEST_SERVICE_CLIENT_MOCK_H
#define TEST_UNITTEST_SERVICE_CLIENT_MOCK_H

#include <gmock/gmock.h>
#include "service_client.h"
 
namespace OHOS::FileManagement::Backup {
class ServiceClientMock : public ServiceClient {
 
public:
    MOCK_METHOD0(GetServiceProxyPointer, bool());
    MOCK_METHOD0(GetInstance, sptr<IService>());
    MOCK_METHOD0(InvaildInstance, sptr<IService>());
    MOCK_METHOD0(InvaildInstance, void());
    MOCK_METHOD0(Start, ErrCode());
};
} // End of namespace OHOS::FileManagement::Backup
#endif // TEST_UNITTEST_SERVICE_PROXY_MOCK_H 
