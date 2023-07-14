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

#include <refbase.h>

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "b_json/b_json_entity_extension_config.h"
#include "b_resources/b_constants.h"
#include "bundle_mgr_client.h"
#include "filemgmt_libhilog.h"
#include "install_param.h"
#include "iservice_registry.h"
#include "module_external/inner_receiver_impl.h"
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

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfos(int32_t userId)
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    vector<AppExecFwk::BundleInfo> installedBundles;
    auto bms = GetBundleManager();
    if (!bms->GetBundleInfos(AppExecFwk::GET_BUNDLE_WITH_EXTENSION_INFO, installedBundles, userId)) {
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

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfos(const vector<string> &bundleNames, int32_t userId)
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    auto bms = GetBundleManager();
    for (auto const &bundleName : bundleNames) {
        AppExecFwk::BundleInfo installedBundle;
        if (!bms->GetBundleInfo(bundleName, AppExecFwk::GET_BUNDLE_WITH_EXTENSION_INFO, installedBundle, userId)) {
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

ErrCode BundleMgrAdapter::Install(wptr<InnerReceiverImpl> statusReceiver, const string &bundleFilePath, int32_t userId)
{
    HILOGI("Begin");
    auto bms = GetBundleManager();
    AppExecFwk::BundleInfo bundleInfo;
    if (!bms->GetBundleArchiveInfo(bundleFilePath, AppExecFwk::BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo)) {
        return BError(BError::Codes::SA_BROKEN_IPC, "Failed to get bundle archive info").GetCode();
    }
    auto receiver = statusReceiver.promote();
    if (receiver == nullptr) {
        return BError(BError::Codes::SA_BROKEN_IPC, "Failed to get receiver").GetCode();
    }
    // check bundle name
    if (bundleInfo.name != receiver->GetBundleName()) {
        return BError(BError::Codes::SA_INVAL_ARG, "Bundle name is not match").GetCode();
    }

    auto install = bms->GetBundleInstaller();
    if (!install) {
        return BError(BError::Codes::SA_BROKEN_IPC, "Failed to get bundle installer").GetCode();
    }

    AppExecFwk::InstallParam installParam;
    installParam.installFlag = AppExecFwk::InstallFlag::REPLACE_EXISTING;
    installParam.userId = userId;
    return install->StreamInstall({bundleFilePath}, installParam, receiver);
}
} // namespace OHOS::FileManagement::Backup
