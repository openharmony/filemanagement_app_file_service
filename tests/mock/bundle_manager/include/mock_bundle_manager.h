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

#ifndef MOCK_BUNDLE_MANAGER_H
#define MOCK_BUNDLE_MANAGER_H

#include <gmock/gmock.h>
#include <vector>

#include "ability_info.h"
#include "application_info.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
namespace AppExecFwk {
class BundleMgrProxy : public IRemoteProxy<IBundleMgr> {
public:
    explicit BundleMgrProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBundleMgr>(impl) {}
    virtual ~BundleMgrProxy() {}

    bool GetBundleInfo(const std::string &bundleName,
                       const BundleFlag flag,
                       BundleInfo &bundleInfo,
                       int32_t userId) override;
};

class BundleMgrStub : public IRemoteStub<IBundleMgr> {
public:
    BundleMgrStub() {};
    virtual ~BundleMgrStub() {}
    virtual int OnRemoteRequest(uint32_t code,
                                MessageParcel &data,
                                MessageParcel &reply,
                                MessageOption &option) override;

    virtual bool GetBundleInfo(const std::string &bundleName,
                               const BundleFlag flag,
                               BundleInfo &bundleInfo,
                               int32_t userId = Constants::UNSPECIFIED_USERID) override;

    virtual bool GetBundleInfo(const std::string &bundleName,
                               int32_t flags,
                               BundleInfo &bundleInfo,
                               int32_t userId = Constants::UNSPECIFIED_USERID) override;
};
} // namespace AppExecFwk
} // namespace OHOS
#endif // MOCK_BUNDLE_MANAGER_H