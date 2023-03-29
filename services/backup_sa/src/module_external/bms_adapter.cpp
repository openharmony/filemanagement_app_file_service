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

#include <algorithm>
#include <fstream>
#include <memory>
#include <refbase.h>

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "b_json/b_json_entity_extension_config.h"
#include "b_resources/b_constants.h"
#include "bundle_mgr_client.h"
#include "filemgmt_libhilog.h"
#include "install_param.h"
#include "iservice_registry.h"
#include "module_external/sms_adapter.h"
#include "module_ipc/service.h"
#include "module_ipc/svc_session_manager.h"
#include "module_sched/sched_scheduler.h"
#include "status_receiver_host.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static sptr<AppExecFwk::IBundleMgr> GetBundleManager()
{
    auto saMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saMgr == nullptr) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to get system ability manager");
    }

    auto bundleObj = saMgr->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleObj == nullptr) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to get bundle manager service");
    }

    return iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
}

static tuple<bool, string> GetAllowAndExtName(const vector<AppExecFwk::ExtensionAbilityInfo> &extensionInfos)
{
    for (auto &&ext : extensionInfos) {
        if (ext.type != AppExecFwk::ExtensionAbilityType::BACKUP) {
            continue;
        }
        vector<string> out;
        AppExecFwk::BundleMgrClient client;
        if (!client.GetResConfigFile(ext, "ohos.extension.backup", out) || out.size() == 0) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Failed to get resconfigfile of bundle " + ext.bundleName);
        }
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(out[0], ext.bundleName);
        auto cache = cachedEntity.Structuralize();
        return {cache.GetAllowToBackupRestore(), ext.name};
    }
    return {false, ""};
}

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfos()
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    vector<AppExecFwk::BundleInfo> installedBundles;
    auto bms = GetBundleManager();
    if (!bms->GetBundleInfos(AppExecFwk::GET_BUNDLE_WITH_EXTENSION_INFO, installedBundles,
                             AppExecFwk::Constants::START_USERID)) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to get bundle infos");
    }
    for (auto const &installedBundle : installedBundles) {
        auto [allToBackup, extName] = GetAllowAndExtName(installedBundle.extensionInfos);
        auto bundleStats = StorageMgrAdapter::GetBundleStats(installedBundle.name);
        bundleInfos.emplace_back(BJsonEntityCaps::BundleInfo {installedBundle.name, installedBundle.versionCode,
                                                              installedBundle.versionName, bundleStats.dataSize_,
                                                              allToBackup, extName});
    }
    return bundleInfos;
}

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfos(const vector<string> &bundleNames)
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    auto bms = GetBundleManager();
    for (auto const &bundleName : bundleNames) {
        AppExecFwk::BundleInfo installedBundle;
        if (!bms->GetBundleInfo(bundleName, AppExecFwk::GET_BUNDLE_WITH_EXTENSION_INFO, installedBundle,
                                AppExecFwk::Constants::START_USERID)) {
            throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to get bundle info");
        }
        auto [allToBackup, extName] = GetAllowAndExtName(installedBundle.extensionInfos);
        auto bundleStats = StorageMgrAdapter::GetBundleStats(installedBundle.name);
        bundleInfos.emplace_back(BJsonEntityCaps::BundleInfo {installedBundle.name, installedBundle.versionCode,
                                                              installedBundle.versionName, bundleStats.dataSize_,
                                                              allToBackup, extName});
    }
    return bundleInfos;
}
} // namespace OHOS::FileManagement::Backup
