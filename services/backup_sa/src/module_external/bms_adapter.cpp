/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "module_external/sms_adapter.h"

#include <fstream>
#include <iostream>
#include <refbase.h>

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "b_json/b_json_entity_extension_config.h"
#include "b_resources/b_constants.h"
#include "b_sa/b_sa_utils.h"
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
#include "if_system_ability_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
enum { APP = 0, LOCAL, DISTRIBUTED, DATABASE, CACHE };
const string HMOS_HAP_CODE_PATH = "1";
const string LINUX_HAP_CODE_PATH = "2";
const string MEDIA_LIBRARY_HAP = "com.ohos.medialibrary.medialibrarydata";
const string EXTERNAL_FILE_HAP = "com.ohos.UserFile.ExternalFileManager";
const int E_ERR = -1;
const int SINGLE_BUNDLE_NUM = 1;
const vector<string> dataDir = {"app", "local", "distributed", "database", "cache"};
} // namespace

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

static tuple<bool, bool, string, string, string, Json::Value> GetAllowAndExtName(
    const vector<AppExecFwk::ExtensionAbilityInfo> &extensionInfos)
{
    for (auto &&ext : extensionInfos) {
        if (ext.type != AppExecFwk::ExtensionAbilityType::BACKUP) {
            continue;
        }
        vector<string> out;
        AppExecFwk::BundleMgrClient client;
        if (!client.GetResConfigFile(ext, "ohos.extension.backup", out) || out.size() == 0) {
            HILOGE("Failed to get resconfigfile of bundle, bundle name is:%{public}s", ext.bundleName.c_str());
            continue;
        }
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(out[0], ext.bundleName);
        auto cache = cachedEntity.Structuralize();
        return {cache.GetAllowToBackupRestore(), cache.GetFullBackupOnly(), ext.name, cache.GetRestoreDeps(),
            cache.GetSupportScene(), cache.GetExtraInfo()};
    }
    return {false, false, "", "", "", Json::Value()};
}

static int64_t GetBundleStats(const string &bundleName, int32_t userId)
{
    HILOGI("Begin bundleName:%{public}s", bundleName.c_str());
    if (bundleName == MEDIA_LIBRARY_HAP || bundleName == EXTERNAL_FILE_HAP) {
        return StorageMgrAdapter::GetUserStorageStats(bundleName, userId);
    }
    auto bms = GetBundleManager();
    vector<int64_t> bundleStats;
    bool res = bms->GetBundleStats(bundleName, userId, bundleStats);
    if (!res || bundleStats.size() != dataDir.size()) {
        HILOGE("An error occurred in querying bundle stats. name:%{public}s", bundleName.c_str());
        return 0;
    }
    for (uint i = 0; i < bundleStats.size(); i++) {
        if (bundleStats[i] == E_ERR) {
            HILOGE("Failed to query %{public}s data. name:%{public}s", dataDir[i].c_str(), bundleName.c_str());
            bundleStats[i] = 0;
        }
    }
    int64_t dataSize = bundleStats[LOCAL] + bundleStats[DISTRIBUTED] + bundleStats[DATABASE];
    return dataSize;
}

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfos(const vector<string> &bundleNames, int32_t userId)
{
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    auto bms = GetBundleManager();
    HILOGI("Start, bundleNames size:%{public}zu", bundleNames.size());
    for (auto const &bundleName : bundleNames) {
        HILOGI("Begin Get bundleName:%{public}s", bundleName.c_str());
        if (SAUtils::IsSABundleName(bundleName)) {
            GetBundleInfoForSA(bundleName, bundleInfos);
            continue;
        }
        AppExecFwk::BundleInfo installedBundle;
        if (!bms->GetBundleInfo(bundleName, AppExecFwk::GET_BUNDLE_WITH_EXTENSION_INFO, installedBundle, userId)) {
            if (bundleNames.size() != SINGLE_BUNDLE_NUM) {
                HILOGE("bundleName:%{public}s, current bundle info for backup/restore is empty", bundleName.c_str());
                continue;
            }
            throw BError(BError::Codes::SA_BUNDLE_INFO_EMPTY, "Failed to get bundle info");
        }
        if (installedBundle.applicationInfo.codePath == HMOS_HAP_CODE_PATH ||
            installedBundle.applicationInfo.codePath == LINUX_HAP_CODE_PATH) {
            HILOGI("Unsupported applications, name : %{public}s", installedBundle.name.data());
            continue;
        }
        auto [allToBackup, fullBackupOnly, extName, restoreDeps, supportScene, extraInfo] =
            GetAllowAndExtName(installedBundle.extensionInfos);
        int64_t dataSize = 0;
        if (allToBackup) {
            dataSize = GetBundleStats(installedBundle.name, userId);
        }
        bundleInfos.emplace_back(BJsonEntityCaps::BundleInfo {installedBundle.name, installedBundle.versionCode,
                                                              installedBundle.versionName, dataSize, 0, allToBackup,
                                                              fullBackupOnly, extName, restoreDeps, supportScene,
                                                              extraInfo});
    }
    HILOGI("End, bundleInfos size:%{public}zu", bundleInfos.size());
    return bundleInfos;
}

string BundleMgrAdapter::GetAppGalleryBundleName()
{
    auto bms = GetBundleManager();

    string bundleName = "";
    auto ret = bms->QueryAppGalleryBundleName(bundleName);
    if (!ret) {
        HILOGI("Get App Gallery BundleName fail!");
    } else {
        HILOGI("App Gallery BundleName: %{public}s", bundleName.c_str());
    }
    return bundleName;
}

static bool GetBackupExtConfig(const vector<AppExecFwk::ExtensionAbilityInfo> &extensionInfos,
    BJsonEntityCaps::BundleBackupConfigPara &backupPara)
{
    for (auto &&ext : extensionInfos) {
        if (ext.type != AppExecFwk::ExtensionAbilityType::BACKUP) {
            continue;
        }
        vector<string> out;
        AppExecFwk::BundleMgrClient client;
        if (!client.GetResConfigFile(ext, "ohos.extension.backup", out) || out.size() == 0) {
            HILOGE("Failed to get resconfigfile of bundle, bundle name is:%{public}s", ext.bundleName.c_str());
            continue;
        }
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(out[0], ext.bundleName);
        auto cache = cachedEntity.Structuralize();
        backupPara.allToBackup = cache.GetAllowToBackupRestore();
        backupPara.fullBackupOnly = cache.GetFullBackupOnly();
        backupPara.extensionName = ext.name;
        backupPara.restoreDeps = cache.GetRestoreDeps();
        backupPara.supportScene = cache.GetSupportScene();
        backupPara.extraInfo = cache.GetExtraInfo();
        backupPara.includes = cache.GetIncludes();
        backupPara.excludes = cache.GetExcludes();
        return true;
    }
    return false;
}

static bool CreateIPCInteractionFiles(int32_t userId, const string &bundleName, int64_t lastIncrementalTime,
    const vector<string> &includes, const vector<string> &excludes)
{
    // backup_sa bundle path
    string backupSaBundleDir = BConstants::BACKUP_PATH_PREFIX + to_string(userId) + BConstants::BACKUP_PATH_SURFFIX +
        bundleName + BConstants::FILE_SEPARATOR_CHAR;
    if (access(backupSaBundleDir.data(), F_OK) != 0) {
        int32_t err = mkdir(backupSaBundleDir.data(), S_IRWXU | S_IRWXG);
        if (err != 0) {
            HILOGE("Failed to create folder in backup_sa bundleName:%{public}s, sys err:%{public}d",
                bundleName.c_str(), errno);
            return false;
        }
    }
    // backup_sa include/exclude
    string incExFilePath = backupSaBundleDir + BConstants::BACKUP_INCEXC_SYMBOL + to_string(lastIncrementalTime);
    ofstream incExcFile;
    incExcFile.open(incExFilePath.data(), ios::out | ios::trunc);
    if (!incExcFile.is_open()) {
        HILOGE("Cannot create incexc file, err = %{public}d", errno);
        return false;
    }
    incExcFile << BConstants::BACKUP_INCLUDE << endl;
    for (const auto &include : includes) {
        incExcFile << include << endl;
    }
    incExcFile << BConstants::BACKUP_EXCLUDE << endl;
    for (const auto &exclude : excludes) {
        incExcFile << exclude << endl;
    }
    incExcFile.close();

    // backup_sa stat
    string statFilePath = backupSaBundleDir + BConstants::BACKUP_STAT_SYMBOL + to_string(lastIncrementalTime);
    ofstream statFile;
    statFile.open(statFilePath.data(), ios::out | ios::trunc);
    if (!statFile.is_open()) {
        HILOGE("Cannot create stat file");
        return false;
    }
    statFile.close();

    return true;
}

static bool GenerateBundleStatsIncrease(int32_t userId, const vector<string> &bundleNames,
    const vector<int64_t> &lastBackTimes, vector<BJsonEntityCaps::BundleInfo> &bundleInfos,
    vector<BJsonEntityCaps::BundleInfo> &newBundleInfos)
{
    vector<int64_t> pkgFileSizes {};
    vector<int64_t> incPkgFileSizes {};
    int32_t err = StorageMgrAdapter::GetBundleStatsForIncrease(userId, bundleNames, lastBackTimes,
        pkgFileSizes, incPkgFileSizes);
    if (err != 0) {
        HILOGE("Failed to get bundleStats result from storage, err = %{public}d", err);
        return false;
    }
    HILOGI("bundleNames size:%{public}zu, pkgFileSizes size:%{public}zu, bundleInfos size:%{public}zu",
        bundleNames.size(), pkgFileSizes.size(), bundleInfos.size());
    if (bundleInfos.size() != pkgFileSizes.size()) {
        HILOGE("The number of bundle is not equal to the number of data records");
        return false;
    }
    for (size_t i = 0; i < bundleInfos.size(); i++) {
        std::string curBundleName = bundleInfos[i].name;
        HILOGD("BundleMgrAdapter name for %{public}s", curBundleName.c_str());
        BJsonEntityCaps::BundleInfo newBundleInfo = {.name = curBundleName,
                                                     .versionCode = bundleInfos[i].versionCode,
                                                     .versionName = bundleInfos[i].versionName,
                                                     .spaceOccupied = pkgFileSizes[i],
                                                     .increSpaceOccupied = incPkgFileSizes[i],
                                                     .allToBackup = bundleInfos[i].allToBackup,
                                                     .fullBackupOnly = bundleInfos[i].fullBackupOnly,
                                                     .extensionName = bundleInfos[i].extensionName,
                                                     .restoreDeps = bundleInfos[i].restoreDeps,
                                                     .supportScene = bundleInfos[i].supportScene,
                                                     .extraInfo = bundleInfos[i].extraInfo};
        newBundleInfos.emplace_back(newBundleInfo);
    }
    return true;
}

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfosForIncremental(
    const vector<BIncrementalData> &incrementalDataList, int32_t userId)
{
    vector<std::string> bundleNames;
    vector<int64_t> incrementalBackTimes;
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    auto bms = GetBundleManager();
    for (auto const &bundleNameTime : incrementalDataList) {
        auto bundleName = bundleNameTime.bundleName;
        HILOGD("Begin get bundleName:%{private}s", bundleName.c_str());
        AppExecFwk::BundleInfo installedBundle;
        if (!bms->GetBundleInfo(bundleName, AppExecFwk::GET_BUNDLE_WITH_EXTENSION_INFO, installedBundle, userId)) {
            throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to get bundle info");
        }
        if (installedBundle.applicationInfo.codePath == HMOS_HAP_CODE_PATH ||
            installedBundle.applicationInfo.codePath == LINUX_HAP_CODE_PATH) {
            HILOGI("Unsupported applications, name : %{private}s", installedBundle.name.data());
            continue;
        }
        struct BJsonEntityCaps::BundleBackupConfigPara backupPara;
        if (!GetBackupExtConfig(installedBundle.extensionInfos, backupPara)) {
            HILOGE("No backup extension ability found");
            continue;
        }
        if (!CreateIPCInteractionFiles(userId, bundleName, bundleNameTime.lastIncrementalTime, backupPara.includes,
            backupPara.excludes)) {
            continue;
        }
        bundleInfos.emplace_back(BJsonEntityCaps::BundleInfo {installedBundle.name, installedBundle.versionCode,
                                                              installedBundle.versionName, 0, 0,
                                                              backupPara.allToBackup, backupPara.fullBackupOnly,
                                                              backupPara.extensionName,
                                                              backupPara.restoreDeps, backupPara.supportScene,
                                                              backupPara.extraInfo});
        bundleNames.emplace_back(bundleName);
        incrementalBackTimes.emplace_back(bundleNameTime.lastIncrementalTime);
    }
    vector<BJsonEntityCaps::BundleInfo> newBundleInfos {};
    if (!GenerateBundleStatsIncrease(userId, bundleNames, incrementalBackTimes, bundleInfos, newBundleInfos)) {
        HILOGE("Failed to get bundleStats result");
        return {};
    }

    HILOGI("BundleMgrAdapter GetBundleInfosForIncremental end ");
    return newBundleInfos;
}

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfosForIncremental(int32_t userId,
    const std::vector<BIncrementalData> &extraIncreData)
{
    vector<AppExecFwk::BundleInfo> installedBundles;
    HILOGI("Begin get bundle infos");
    auto bms = GetBundleManager();
    if (!bms->GetBundleInfos(AppExecFwk::GET_BUNDLE_WITH_EXTENSION_INFO, installedBundles, userId)) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to get bundle infos");
    }

    vector<BIncrementalData> bundleNames;
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    HILOGI("Begin get bundle infos");
    for (auto const &installedBundle : installedBundles) {
        if (installedBundle.applicationInfo.codePath == HMOS_HAP_CODE_PATH ||
            installedBundle.applicationInfo.codePath == LINUX_HAP_CODE_PATH) {
            HILOGI("Unsupported applications, name : %{public}s", installedBundle.name.data());
            continue;
        }
        auto [allToBackup, fullBackupOnly, extName, restoreDeps, supportScene, extraInfo] =
            GetAllowAndExtName(installedBundle.extensionInfos);
        if (!allToBackup) {
            bundleInfos.emplace_back(BJsonEntityCaps::BundleInfo {installedBundle.name, installedBundle.versionCode,
                installedBundle.versionName, 0, 0, allToBackup, fullBackupOnly, extName, restoreDeps, supportScene,
                extraInfo});
            continue;
        }
        auto it = std::find_if(extraIncreData.begin(), extraIncreData.end(),
            [installedBundle](const BIncrementalData &info)->bool {
                return installedBundle.name == info.bundleName;
            });
        if (it == extraIncreData.end()) {
            bundleNames.emplace_back(BIncrementalData {installedBundle.name, 0});
        } else {
            bundleNames.emplace_back(*it);
        }
    }
    auto bundleInfosNew = BundleMgrAdapter::GetBundleInfosForIncremental(bundleNames, userId);
    auto bundleInfosSA = BundleMgrAdapter::GetBundleInfosForSA();
    copy(bundleInfosNew.begin(), bundleInfosNew.end(), back_inserter(bundleInfos));
    copy(bundleInfosSA.begin(), bundleInfosSA.end(), back_inserter(bundleInfos));
    HILOGI("End get bundle infos, bundleInfos size: %{public}zu", bundleInfos.size());
    return bundleInfos;
}

vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetFullBundleInfos(int32_t userId)
{
    vector<AppExecFwk::BundleInfo> installedBundles;
    HILOGI("Begin GetFullBundleInfos");
    auto bms = GetBundleManager();
    if (!bms->GetBundleInfos(AppExecFwk::GET_BUNDLE_WITH_EXTENSION_INFO, installedBundles, userId)) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to get bundle infos");
    }
    vector<string> bundleNames;
    vector<BJsonEntityCaps::BundleInfo> bundleInfos;
    for (auto const &installedBundle : installedBundles) {
        HILOGI("Begin get bundle infos, bundleName = %{public}s", installedBundle.name.data());
        if (installedBundle.applicationInfo.codePath == HMOS_HAP_CODE_PATH ||
            installedBundle.applicationInfo.codePath == LINUX_HAP_CODE_PATH) {
            HILOGI("Unsupported applications, name : %{public}s", installedBundle.name.data());
            continue;
        }
        auto [allToBackup, fullBackupOnly, extName, restoreDeps, supportScene, extraInfo] =
            GetAllowAndExtName(installedBundle.extensionInfos);
        if (!allToBackup) {
            HILOGI("Not allToBackup, bundleName = %{public}s", installedBundle.name.data());
            bundleInfos.emplace_back(BJsonEntityCaps::BundleInfo {installedBundle.name, installedBundle.appIndex,
                installedBundle.versionCode, installedBundle.versionName, 0, 0, allToBackup, fullBackupOnly, extName,
                restoreDeps, supportScene, extraInfo});
            continue;
        }
        bundleNames.emplace_back(installedBundle.name);
    }
    auto bundleInfosNew = BundleMgrAdapter::GetBundleInfos(bundleNames, userId);
    auto bundleInfosSA = BundleMgrAdapter::GetBundleInfosForSA();
    copy(bundleInfosNew.begin(), bundleInfosNew.end(), back_inserter(bundleInfos));
    copy(bundleInfosSA.begin(), bundleInfosSA.end(), back_inserter(bundleInfos));
    HILOGI("End GetFullBundleInfos, bundleInfos size: %{public}zu", bundleInfos.size());
    return bundleInfos;
}

string BundleMgrAdapter::GetExtName(string bundleName, int32_t userId)
{
    vector<AppExecFwk::BundleInfo> installedBundles;
    auto bms = GetBundleManager();
    if (!bms->GetBundleInfos(AppExecFwk::GET_BUNDLE_WITH_EXTENSION_INFO, installedBundles, userId)) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to get bundle infos");
    }
    for (auto const &installedBundle : installedBundles) {
        if (installedBundle.applicationInfo.codePath == HMOS_HAP_CODE_PATH ||
            installedBundle.applicationInfo.codePath == LINUX_HAP_CODE_PATH) {
            HILOGI("Unsupported applications, name : %{public}s", installedBundle.name.data());
            continue;
        }
        for (auto ext : installedBundle.extensionInfos) {
            if (ext.bundleName != bundleName) {
                continue;
            }
            if (ext.type != AppExecFwk::ExtensionAbilityType::BACKUP) {
                continue;
            }
            HILOGI("bundleName: %{public}s, find extName: %{public}s", bundleName.c_str(), ext.name.c_str());
            return ext.name;
        }
    }
    HILOGI("bundleName: %{public}s , find extName failed", bundleName.c_str());
    return "BackupExtensionAbility";
}

std::vector<BJsonEntityCaps::BundleInfo> BundleMgrAdapter::GetBundleInfosForSA()
{
    std::vector<int32_t> saIds;
    vector<BJsonEntityCaps::BundleInfo> saBundleInfos;
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgrProxy) {
        HILOGE("SamgrProxy is nullptr");
        return saBundleInfos;
    }
    int32_t ret = samgrProxy->GetExtensionSaIds(BConstants::EXTENSION_BACKUP, saIds);
    HILOGI("GetExtensionSaIds ret: %{public}d", ret);
    for (auto saId : saIds) {
        saBundleInfos.emplace_back(BJsonEntityCaps::BundleInfo {std::to_string(saId), 0, "", 0, 0, true, false,
            "", "", "", ""});
    }
    return saBundleInfos;
}

void BundleMgrAdapter::GetBundleInfoForSA(std::string bundleName, std::vector<BJsonEntityCaps::BundleInfo>& bundleInfos)
{
    HILOGI("SA %{public}s GetBundleInfo begin.", bundleName.c_str());
    std::vector<int32_t> saIds;
    vector<BJsonEntityCaps::BundleInfo> saBundleInfos;
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgrProxy) {
        HILOGE("SamgrProxy is nullptr");
        return;
    }
    int32_t ret = samgrProxy->GetExtensionSaIds(BConstants::EXTENSION_BACKUP, saIds);
    if (ret != ERR_OK) {
        HILOGE("GetExtensionSaIds err,ret %{public}d", ret);
        return;
    }
    if (saIds.empty()) {
        HILOGE("GetExtensionSaIds result is empty");
        return;
    }
    int32_t saId = std::atoi(bundleName.c_str());
    if (std::find(saIds.begin(), saIds.end(), saId) == saIds.end()) {
        HILOGE("SA %{public}d is not surport backup.", saId);
        return;
    }
    bundleInfos.emplace_back(BJsonEntityCaps::BundleInfo {bundleName, 0, "", 0, 0, true, false, "", "", "", ""});
    HILOGI("SA %{public}s GetBundleInfo end.", bundleName.c_str());
}
} // namespace OHOS::FileManagement::Backup
