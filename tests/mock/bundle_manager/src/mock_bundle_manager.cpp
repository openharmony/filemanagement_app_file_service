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

#include "mock_bundle_manager.h"

#include <fstream>
#include <sstream>
#include <string>

#include "ability_config.h"
#include "ability_info.h"
#include "application_info.h"
#include "bundle_mgr_client.h"

namespace OHOS {
namespace AppExecFwk {
using namespace std;
using namespace OHOS::AAFwk;

bool BundleMgrProxy::GetBundleInfo(const string &bundleName,
                                   const BundleFlag flag,
                                   BundleInfo &bundleInfo,
                                   int32_t userId)
{
    GTEST_LOG_(INFO) << "BundleMgrProxy:GetBundleInfo is ok";
    ExtensionAbilityInfo info;
    info.type = AppExecFwk::ExtensionAbilityType::BACKUP;
    info.name = "com.example.app2backup";
    bundleInfo.extensionInfos.push_back(info);
    return true;
}

int BundleMgrStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    return 0;
}

bool BundleMgrStub::GetBundleInfo(const string &bundleName,
                                  const BundleFlag flag,
                                  BundleInfo &bundleInfo,
                                  int32_t userId)
{
    GTEST_LOG_(INFO) << "BundleMgrStub:GetBundleInfo is ok";
    ExtensionAbilityInfo info;
    info.type = AppExecFwk::ExtensionAbilityType::BACKUP;
    info.name = "com.example.app2backup";
    bundleInfo.extensionInfos.push_back(info);
    return true;
}

bool BundleMgrStub::GetBundleInfo(const string &bundleName, int32_t flags, BundleInfo &bundleInfo, int32_t userId)
{
    GTEST_LOG_(INFO) << "GetBundleInfo is ok";
    ExtensionAbilityInfo info;
    info.type = AppExecFwk::ExtensionAbilityType::BACKUP;
    info.name = "com.example.app2backup";
    bundleInfo.extensionInfos.push_back(info);
    return true;
}

bool BundleMgrStub::GetApplicationInfos(const ApplicationFlag flag, int userId, vector<ApplicationInfo> &appInfos)
{
    GTEST_LOG_(INFO) << "GetApplicationInfos is ok";
    ApplicationInfo appInfo {};
    appInfo.bundleName = "com.example.app2backup";
    appInfo.versionName = "1.0.0";
    appInfos.push_back(appInfo);
    return true;
}

BundleMgrClient::BundleMgrClient() {}

BundleMgrClient::~BundleMgrClient() {}

bool BundleMgrClient::GetResConfigFile(const ExtensionAbilityInfo &extensionInfo,
                                       const string &metadataName,
                                       vector<string> &profileInfos) const
{
    GTEST_LOG_(INFO) << "GetResConfigFile is ok";
    string str = "{\"allowToBackupRestore\":true}";
    profileInfos.push_back(str);
    return true;
}
} // namespace AppExecFwk
} // namespace OHOS