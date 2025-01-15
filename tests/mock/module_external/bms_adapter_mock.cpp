/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "module_external/bms_adapter.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <refbase.h>

#include "b_json/b_json_entity_extension_config.h"
#include "bundle_mgr_client.h"
#include "module_ipc/service.h"
#include "module_ipc/svc_session_manager.h"
#include "module_sched/sched_scheduler.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfos(const vector<string> &bundleNames, int32_t userId)
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    bundleInfos.emplace_back(
        BJsonEntityCaps::BundleInfo {"com.example.app2backup", 0, {}, {}, 0, 0, true, false,
            "com.example.app2backup"});
    return bundleInfos;
}

string BundleMgrAdapter::GetAppGalleryBundleName()
{
    return "";
}

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfosForIncremental(
    const vector<BIncrementalData> &incrementalDataList, int32_t userId)
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    bundleInfos.emplace_back(
        BJsonEntityCaps::BundleInfo {"com.example.app2backup", 0, {}, {}, 0, 0, true, false,
            "com.example.app2backup"});
    return bundleInfos;
}

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfosForIncremental(int32_t userId,
    const vector<BIncrementalData> &extraIncreData)
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    bundleInfos.emplace_back(
        BJsonEntityCaps::BundleInfo {"com.example.app2backup", 0, {}, {}, 0, 0, true, false,
            "com.example.app2backup"});
    return bundleInfos;
}


vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetFullBundleInfos(int32_t userId)
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    bundleInfos.emplace_back(
        BJsonEntityCaps::BundleInfo {"com.example.app2backup", 0, {}, {}, 0, 0, true, false,
            "com.example.app2backup"});
    return bundleInfos;
}

string BundleMgrAdapter::GetExtName(string bundleName, int32_t userId)
{
    return "BackupExtensionAbility";
}

bool BundleMgrAdapter::IsUser0BundleName(std::string bundleName, int32_t userId)
{
    return true;
}

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfosForAppend(
    const vector<BIncrementalData> &incrementalDataList, int32_t userId)
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    bundleInfos.emplace_back(
        BJsonEntityCaps::BundleInfo {"com.example.app2backup", 0, {}, {}, 0, 0, true, false,
            "com.example.app2backup"});
    return bundleInfos;
}

std::vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfosForLocalCapabilities(int32_t userId)
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    bundleInfos.emplace_back(
        BJsonEntityCaps::BundleInfo {"com.example.app2backup", 0, {}, {}, 0, 0, true, false,
            "com.example.app2backup"});
    return bundleInfos;
}

std::vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfosForIndex(
    const vector<string> &bundleNames, int32_t userId)
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    bundleInfos.emplace_back(
        BJsonEntityCaps::BundleInfo {"com.example.app2backup", 0, {}, {}, 0, 0, true, false,
            "com.example.app2backup"});
    return bundleInfos;
}

int64_t BundleMgrAdapter::GetBundleDataSize(const std::string &bundleName, int32_t userId)
{
    return 0;
}

void BundleMgrAdapter::CreatBackupEnv(const std::vector<BIncrementalData> &bundleNameList, int32_t userId) {}
} // namespace OHOS::FileManagement::Backup
