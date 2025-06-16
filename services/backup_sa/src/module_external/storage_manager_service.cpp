/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "module_external/storage_manager_service.h"

#include <dirent.h>
#include <fstream>
#include <stack>
#include <sys/quota.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <refbase.h>
#include <linux/fs.h>
#include <linux/quota.h>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "sandbox_helper.h"
#include "file_uri.h"

#include <iservice_registry.h>
#include <system_ability_definition.h>

namespace OHOS::FileManagement::Backup {
using namespace std;
std::recursive_mutex mMountsLock;

static bool PathSortFunc(const std::string &path1, const std::string &path2)
{
    return path1 < path2;
}

static std::string GetQuotaSrcMountPath(const std::string &target)
{
    std::lock_guard<std::recursive_mutex> lock(mMountsLock);
    if (mQuotaReverseMounts.find(target) != mQuotaReverseMounts.end()) {
        return mQuotaReverseMounts[target];
    } else {
        return "";
    }
}

bool StorageManagerService::GetBundleStats(const string &bundleName,
    StorageManager::BundleStats &storageStats)
{
    return true;
}

int32_t StorageManagerService::GetUserStorageStatsByType(int32_t userId, StorageManager::StorageStats &storageStats,
    std::string type)
{
    storageStats.video_ = 0;
    storageStats.image_ = 0;
    storageStats.file_ = 0;
    int32_t err = E_ERR;
    if (type == MEDIA_TYPE) {
        HILOGI("GetUserStorageStatsByType media");
        err = GetMediaStorageStats(storageStats);
    } else if (type == FILE_TYPE) {
        HILOGI("GetUserStorageStatsByType file");
        err = GetFileStorageStats(userId, storageStats);
    } else {
        HILOGI("GetUserStorageStatsByType type: %{public}s", type.c_str());
    }
    return err;
}

int32_t StorageManagerService::GetMediaStorageStats(StorageManager::StorageStats &storageStats)
{
    HILOGE("GetMediaStorageStats start");
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        HILOGE("StorageStatusService::GetMediaStorageStats samgr == nullptr");
        return E_SA_IS_NULLPTR;
    }
    auto remoteObj = sam->GetSystemAbility(FILEMANAGEMENT_BACKUP_SERVICE_SA_ID);
    if (remoteObj == nullptr) {
        HILOGE("StorageStatusService::GetMediaStorageStats remoteObj == nullptr");
        return E_REMOTE_IS_NULLPTR;
    }
    int32_t tryCount = 1;
    HILOGE("GetMediaStorageStats start Creator");
    auto dataShareHelper = DataShare::DataShareHelper::Creator(remoteObj, MEDIALIBRARY_DATA_URI);
    while (dataShareHelper == nullptr && tryCount < GET_DATA_SHARE_HELPER_TIMES) {
        HILOGW("dataShareHelper is retrying, attempt %{public}d", tryCount);
        dataShareHelper = DataShare::DataShareHelper::Creator(remoteObj, MEDIALIBRARY_DATA_URI);
        tryCount++;
    }
    if (dataShareHelper == nullptr) {
        HILOGE("dataShareHelper is null!");
        return E_MEDIALIBRARY_ERROR;
    }
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI + "/" + MEDIA_QUERYOPRN_QUERYVOLUME + "/" + MEDIA_QUERYOPRN_QUERYVOLUME);
    DataShare::DataSharePredicates predicates;
    HILOGE("GetMediaStorageStats start Query");
    auto queryResultSet = dataShareHelper->Query(uri, predicates, columns);
    if (queryResultSet == nullptr) {
        HILOGE("queryResultSet is null!");
        return E_QUERY;
    }
    auto count = 0;
    auto ret = queryResultSet->GetRowCount(count);
    if ((ret != E_OK) || (count < 0)) {
        HILOGE("get row count from rdb failed");
        return E_GETROWCOUNT;
    }
    GetMediaTypeAndSize(queryResultSet, storageStats);
    dataShareHelper->Release();
    HILOGE("GetMediaStorageStats end");
    return E_OK;
}

void StorageManagerService::GetMediaTypeAndSize(const std::shared_ptr<DataShare::DataShareResultSet> &resultSet,
    StorageManager::StorageStats &storageStats)
{
    if (resultSet == nullptr) {
        HILOGE("StorageStatusService::GetMediaTypeAndSize, input resultSet is nullptr.");
        return;
    }
    int thumbnailType = -1;
    while (resultSet->GoToNextRow() == E_OK) {
        int32_t index = 0;
        int mediatype = 0;
        int64_t size = 0;
        if (resultSet->GetColumnIndex("media_type", index) || resultSet->GetInt(index, mediatype)) {
            HILOGE("get media_type column index or int value err.");
            continue;
        }
        if (resultSet->GetColumnIndex("size", index) || resultSet->GetLong(index, size)) {
            HILOGE("get size column index or long value err.");
            continue;
        }
        HILOGI("media type: %{public}d, size: %{public}lld", mediatype, static_cast<long long>(size));
        if (mediatype == MEDIA_TYPE_IMAGE || mediatype == thumbnailType) {
            storageStats.image_ += size;
        } else if (mediatype == MEDIA_TYPE_AUDIO) {
            storageStats.audio_ = size;
        } else if (mediatype == MEDIA_TYPE_VIDEO) {
            storageStats.video_ = size;
        } else {
            HILOGW("unsupprted media_type: %{public}d", mediatype);
        }
    }
}

int32_t StorageManagerService::GetFileStorageStats(int32_t userId, StorageManager::StorageStats &storageStats)
{
    int32_t uid = userId * USER_ID_BASE + UID_FILE_MANAGER;
    HILOGE("GetOccupiedSpaceForUid uid:%{public}d", uid);
    if (InitialiseQuotaMounts() != true) {
        HILOGE("Failed to initialise quota mounts");
        return E_SYS_KERNEL_ERR;
    }

    std::string device = "";
    device = GetQuotaSrcMountPath(QUOTA_DEVICE_DATA_PATH);
    if (device.empty()) {
        HILOGE("skip when device no quotas present");
        return E_OK;
    }

    struct dqblk dq;
    if (quotactl(QCMD(Q_GETQUOTA, USRQUOTA), device.c_str(), uid, reinterpret_cast<char*>(&dq)) != 0) {
        HILOGE("Failed to get quotactl, errno : %{public}d", errno);
        return E_SYS_KERNEL_ERR;
    }
    storageStats.file_ = static_cast<int64_t>(dq.dqb_curspace);
    HILOGE("GetOccupiedSpaceForUid size:%{public}s", std::to_string(storageStats.file_).c_str());
    return E_OK;
}

bool StorageManagerService::InitialiseQuotaMounts()
{
    std::lock_guard<std::recursive_mutex> lock(mMountsLock);
    mQuotaReverseMounts.clear();
    std::ifstream in(PROC_MOUNTS_PATH);

    if (!in.is_open()) {
        HILOGE("Failed to open mounts file");
        return false;
    }
    std::string source;
    std::string target;
    std::string ignored;

    while (in.peek() != EOF) {
        std::getline(in, source, ' ');
        std::getline(in, target, ' ');
        std::getline(in, ignored);
        if (source.compare(0, strlen(DEV_BLOCK_PATH), DEV_BLOCK_PATH) == 0
            && target.compare(QUOTA_DEVICE_DATA_PATH) == 0) {
            struct dqblk dq;
            if (quotactl(QCMD(Q_GETQUOTA, USRQUOTA), source.c_str(), 0, reinterpret_cast<char*>(&dq)) == 0) {
                mQuotaReverseMounts[target] = source;
            }
        }
    }
    return true;
}

int32_t StorageManagerService::UpdateMemoryPara(int32_t size, int32_t oldSize)
{
    return E_OK;
}

int32_t StorageManagerService::GetBundleStatsForIncrease(uint32_t userId, const std::vector<std::string> &bundleNames,
    const std::vector<int64_t> &incrementalBackTimes, std::vector<int64_t> &pkgFileSizes,
    std::vector<int64_t> &incPkgFileSizes)
{
    HILOGI("GetBundleStatsForIncrease start");
    if (bundleNames.size() != incrementalBackTimes.size()) {
        HILOGE("Invalid paramters, size of bundleNames should match incrementalBackTimes.");
        return E_ERR;
    }
    for (size_t i = 0; i < bundleNames.size(); i++) {
        std::string bundleName = bundleNames[i];
        int64_t lastBackupTime = incrementalBackTimes[i];
        GetBundleStatsForIncreaseEach(userId, bundleName, lastBackupTime, pkgFileSizes, incPkgFileSizes);
    }
    return E_OK;
}

void StorageManagerService::GetBundleStatsForIncreaseEach(uint32_t userId, std::string &bundleName,
    int64_t lastBackupTime, std::vector<int64_t> &pkgFileSizes, std::vector<int64_t> &incPkgFileSizes)
{
    // input parameters
    BundleStatsParas paras = {.userId = userId, .bundleName = bundleName,
                              .lastBackupTime = lastBackupTime, .fileSizeSum = 0, .incFileSizeSum = 0};

    // obtain includes, excludes in backup extension config
    auto [includes, excludes] = ReadIncludesExcludesPath(bundleName, lastBackupTime, userId);
    if (includes.empty()) {
        pkgFileSizes.emplace_back(0);
        incPkgFileSizes.emplace_back(0);
        return;
    }
    // physical paths
    std::vector<std::string> phyIncludes;
    // map about sandbox path to physical path
    std::map<std::string, std::string> pathMap;

    // recognize physical path for include directory
    DealWithIncludeFiles(paras, includes, phyIncludes, pathMap);
    if (phyIncludes.empty()) {
        HILOGE("Incorrect convert for include sandbox path for %{private}s", bundleName.c_str());
        pkgFileSizes.emplace_back(0);
        incPkgFileSizes.emplace_back(0);
        return;
    }

    // recognize physical path for exclude directory
    std::vector<std::string> phyExcludes;
    for (const auto &exclude : excludes) {
        std::string excludeStr = exclude;
        if (excludeStr.front() != FILE_SEPARATOR_CHAR) {
            excludeStr = FILE_SEPARATOR_CHAR + excludeStr;
        }
        // convert sandbox to physical path
        ConvertSandboxRealPath(userId, bundleName, excludeStr, phyExcludes, pathMap);
    }

    std::string filePath = BACKUP_PATH_PREFIX + std::to_string(userId) + BACKUP_PATH_SURFFIX +
        bundleName + FILE_SEPARATOR_CHAR + BACKUP_STAT_SYMBOL + std::to_string(lastBackupTime);
    std::ofstream statFile;
    statFile.open(filePath.data(), std::ios::out | std::ios::trunc);
    if (!statFile.is_open()) {
        HILOGE("creat file fail, errno:%{public}d.", errno);
        pkgFileSizes.emplace_back(0);
        incPkgFileSizes.emplace_back(0);
        return;
    }
    statFile << VER_10_LINE1 << std::endl;
    statFile << VER_10_LINE2 << std::endl;

    DeduplicationPath(phyIncludes);
    ScanExtensionPath(paras, phyIncludes, phyExcludes, pathMap, statFile);
    // calculate summary file sizes
    pkgFileSizes.emplace_back(paras.fileSizeSum);
    incPkgFileSizes.emplace_back(paras.incFileSizeSum);
    HILOGI("bundleName: %{public}s, size: %{public}lld", bundleName.c_str(), static_cast<long long>(paras.fileSizeSum));
    statFile.close();
}

std::tuple<std::vector<std::string>, std::vector<std::string>> StorageManagerService::ReadIncludesExcludesPath(
    const std::string &bundleName, const int64_t lastBackupTime, const uint32_t userId)
{
    if (bundleName.empty()) {
        HILOGE("bundleName is empty");
        return { {}, {} };
    }
    // 保存includeExclude的path
    std::string filePath = BACKUP_PATH_PREFIX + std::to_string(userId) + BACKUP_PATH_SURFFIX +
        bundleName + FILE_SEPARATOR_CHAR + BACKUP_INCEXC_SYMBOL + std::to_string(lastBackupTime);
    std::ifstream incExcFile;
    incExcFile.open(filePath.data());
    if (!incExcFile.is_open()) {
        HILOGE("Cannot open include/exclude file, fail errno:%{public}d", errno);
        return { {}, {} };
    }

    std::vector<std::string> includes;
    std::vector<std::string> excludes;
    bool incOrExt = true;
    while (incExcFile) {
        std::string line;
        std::getline(incExcFile, line);
        if (line.empty()) {
            HILOGI("Read Complete");
            break;
        }
        if (line == BACKUP_INCLUDE) {
            incOrExt = true;
        } else if (line == BACKUP_EXCLUDE) {
            incOrExt = false;
        }
        if (incOrExt && line != BACKUP_INCLUDE) {
            includes.emplace_back(line);
        } else if (!incOrExt && line != BACKUP_EXCLUDE) {
            excludes.emplace_back(line);
        }
    }
    incExcFile.close();
    return {includes, excludes};
}

void StorageManagerService::DealWithIncludeFiles(const BundleStatsParas &paras,
    const std::vector<std::string> &includes, std::vector<std::string> &phyIncludes,
    std::map<std::string, std::string>& pathMap)
{
    uint32_t userId = paras.userId;
    std::string bundleName = paras.bundleName;
    for (const auto &include : includes) {
        std::string includeStr = include;
        if (includeStr.front() != FILE_SEPARATOR_CHAR) {
            includeStr = FILE_SEPARATOR_CHAR + includeStr;
        }
        if (includeStr.find(BASE_EL1 + DEFAULT_PATH_WITH_WILDCARD) == 0 ||
            includeStr.find(BASE_EL2 + DEFAULT_PATH_WITH_WILDCARD) == 0) {
            // recognize sandbox path to physical path with wild card
            RecognizeSandboxWildCard(userId, bundleName, includeStr, phyIncludes, pathMap);
            if (phyIncludes.empty()) {
                HILOGE("DealWithIncludeFiles failed to recognize path with wildcard %{private}s", bundleName.c_str());
                continue;
            }
        } else {
            // convert sandbox to physical path
            ConvertSandboxRealPath(userId, bundleName, includeStr, phyIncludes, pathMap);
        }
    }
}

void StorageManagerService::ConvertSandboxRealPath(const uint32_t userId, const std::string &bundleName,
    const std::string &sandboxPathStr, std::vector<std::string> &realPaths,
    std::map<std::string, std::string>& pathMap)
{
    std::string uriString;
    if (sandboxPathStr.find(NORMAL_SAND_PREFIX) == 0) {
        // for normal hap, start with file://bundleName
        uriString = URI_PREFIX + bundleName;
    } else if (sandboxPathStr.find(FILE_SAND_PREFIX) == 0) {
        // for public files, start with file://docs
        uriString = URI_PREFIX + FILE_AUTHORITY;
    } else if (sandboxPathStr.find(MEDIA_SAND_PREFIX) == 0) {
        std::string physicalPath = sandboxPathStr;
        physicalPath.insert(MEDIA_SAND_PREFIX.length(), FILE_SEPARATOR_CHAR + std::to_string(userId));
        realPaths.emplace_back(physicalPath);
        pathMap.insert({physicalPath, sandboxPathStr});
        return;
    } else if (sandboxPathStr.find(MEDIA_CLOUD_SAND_PREFIX) == 0) {
        std::string physicalPath = sandboxPathStr;
        physicalPath.insert(MEDIA_CLOUD_SAND_PREFIX.length(), FILE_SEPARATOR_CHAR + std::to_string(userId));
        realPaths.emplace_back(physicalPath);
        pathMap.insert({physicalPath, sandboxPathStr});
        return;
    }

    if (!uriString.empty()) {
        std::string sandboxPathUriStr = AppFileService::SandboxHelper::Encode(sandboxPathStr);
        uriString += sandboxPathUriStr;
        AppFileService::ModuleFileUri::FileUri uri(uriString);
        // files
        std::string physicalPath;
        int ret = AppFileService::SandboxHelper::GetBackupPhysicalPath(uri.ToString(), std::to_string(userId),
            physicalPath);
        if (ret != 0) {
            HILOGE("Get physical path failed with %{public}d", ret);
            return;
        }
        realPaths.emplace_back(physicalPath);
        pathMap.insert({physicalPath, sandboxPathStr});
    }
}

void StorageManagerService::DeduplicationPath(std::vector<std::string> &configPaths)
{
    sort(configPaths.begin(), configPaths.end(), PathSortFunc);
    auto it = unique(configPaths.begin(), configPaths.end(), [](const std::string &path1, const std::string &path2) {
        return path1 == path2;
    });
    configPaths.erase(it, configPaths.end());
}

void StorageManagerService::ScanExtensionPath(BundleStatsParas &paras,
    const std::vector<std::string> &includes, const std::vector<std::string> &excludes,
    std::map<std::string, std::string> &pathMap, std::ofstream &statFile)
{
    std::map<std::string, bool> excludesMap;
    for (auto exclude : excludes) {
        SetExcludePathMap(exclude, excludesMap);
    }
    // all file with stats in include directory
    for (const auto &includeDir : includes) {
        // Check if includeDir is a file path
        auto [isSucc, isDir] = CheckIfDirForIncludes(includeDir, paras, pathMap, statFile, excludesMap);
        if (!isSucc) {
            continue;
        }
        // recognize all file in include directory
        if (isDir && !GetIncludesFileStats(includeDir, paras, pathMap, statFile, excludesMap)) {
            HILOGE("Faied to get include files for includeDir");
        }
    }
}

void StorageManagerService::RecognizeSandboxWildCard(const uint32_t userId, const std::string &bundleName,
    const std::string &sandboxPathStr, std::vector<std::string> &phyIncludes,
    std::map<std::string, std::string>& pathMap)
{
    if (sandboxPathStr.find(BASE_EL1 + DEFAULT_PATH_WITH_WILDCARD) == 0) {
        std::string physicalPrefix = PHY_APP + EL1 + FILE_SEPARATOR_CHAR + std::to_string(userId) + BASE +
            bundleName + FILE_SEPARATOR_CHAR;
        std::string relatePath = sandboxPathStr.substr(BASE_EL1.size());
        if (!GetPathWildCard(userId, bundleName, physicalPrefix + relatePath, phyIncludes, pathMap)) {
            HILOGE("el1 GetPathWildCard dir path invaild");
        }
    } else if (sandboxPathStr.find(BASE_EL2 + DEFAULT_PATH_WITH_WILDCARD) == 0) {
        std::string physicalPrefix = PHY_APP + EL2 + FILE_SEPARATOR_CHAR + std::to_string(userId) + BASE +
            bundleName + FILE_SEPARATOR_CHAR;
        std::string relatePath = sandboxPathStr.substr(BASE_EL2.size());
        if (!GetPathWildCard(userId, bundleName, physicalPrefix + relatePath, phyIncludes, pathMap)) {
            HILOGE("el2 GetPathWildCard dir path invaild");
        }
    }
}

void StorageManagerService::SetExcludePathMap(std::string &excludePath, std::map<std::string, bool> &excludesMap)
{
    if (excludePath.empty()) {
        HILOGE("SetExcludePathMap Param failed");
        return;
    }
    struct stat fileStatInfo = {0};
    if (stat(excludePath.c_str(), &fileStatInfo) != 0) {
        HILOGE("SetExcludePathMap call stat error %{private}s, errno:%{public}d", excludePath.c_str(), errno);
        return;
    }
    if (S_ISDIR(fileStatInfo.st_mode)) {
        if (excludePath.back() != FILE_SEPARATOR_CHAR) {
            excludePath.push_back(FILE_SEPARATOR_CHAR);
        }
        excludesMap.insert({excludePath, true});
    } else {
        excludesMap.insert({excludePath, false});
    }
}

std::tuple<bool, bool> StorageManagerService::CheckIfDirForIncludes(const std::string &path, BundleStatsParas &paras,
    std::map<std::string, std::string> &pathMap, std::ofstream &statFile, std::map<std::string, bool> &excludesMap)
{
    if (!statFile.is_open() || path.empty()) {
        HILOGE("CheckIfDirForIncludes Param failed");
        return {false, false};
    }
    // check whether the path exists
    struct stat fileStatInfo = {0};
    if (stat(path.c_str(), &fileStatInfo) != 0) {
        HILOGE("CheckIfDirForIncludes call stat error %{public}s, fail errno:%{public}d", path.c_str(), errno);
        return {false, false};
    }
    if (S_ISDIR(fileStatInfo.st_mode)) {
        HILOGI("%{public}s exists and is a directory", path.c_str());
        return {true, true};
    } else {
        std::string sandboxPath = path;
        auto it = pathMap.find(path);
        if (it != pathMap.end()) {
            sandboxPath = it->second;
        }

        struct FileStat fileStat;
        fileStat.filePath = sandboxPath;
        fileStat.fileSize = fileStatInfo.st_size;
        // mode
        fileStat.mode = static_cast<int32_t>(fileStatInfo.st_mode);
        fileStat.isDir = false;
        int64_t lastUpdateTime = static_cast<int64_t>(fileStatInfo.st_mtime);
        fileStat.lastUpdateTime = lastUpdateTime;
        if (paras.lastBackupTime == 0 || lastUpdateTime > paras.lastBackupTime) {
            fileStat.isIncre = true;
        }
        if (ExcludeFilter(excludesMap, path) == false) {
            WriteFileList(statFile, fileStat, paras);
        }
        return {true, false};
    }
}

bool StorageManagerService::GetIncludesFileStats(const std::string &dir, BundleStatsParas &paras,
    std::map<std::string, std::string> &pathMap,
    std::ofstream &statFile, std::map<std::string, bool> &excludesMap)
{
    std::string sandboxDir = dir;
    auto it = pathMap.find(dir);
    if (it != pathMap.end()) {
        sandboxDir = it->second;
    }
    // stat current directory info
    AddOuterDirIntoFileStat(dir, paras, sandboxDir, statFile, excludesMap);

    std::stack<std::string> folderStack;
    std::string filePath;
    folderStack.push(dir);
    // stat files and sub-directory in current directory info
    while (!folderStack.empty()) {
        filePath = folderStack.top();
        folderStack.pop();
        DIR *dirPtr = opendir(filePath.c_str());
        if (dirPtr == nullptr) {
            HILOGE("GetIncludesFileStats open file dir:%{private}s fail, errno:%{public}d", filePath.c_str(), errno);
            continue;
        }
        if (filePath.back() != FILE_SEPARATOR_CHAR) {
            filePath.push_back(FILE_SEPARATOR_CHAR);
        }

        struct dirent *entry = nullptr;
        while ((entry = readdir(dirPtr)) != nullptr) {
            if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
                continue;
            }
            std::string path = filePath + entry->d_name;
            struct stat fileInfo = {0};
            if (stat(path.c_str(), &fileInfo) != 0) {
                HILOGE("GetIncludesFileStats call stat error %{private}s, errno:%{public}d", path.c_str(), errno);
                fileInfo.st_size = 0;
            }
            struct FileStat fileStat = {};
            fileStat.filePath = PhysicalToSandboxPath(dir, sandboxDir, path);
            fileStat.fileSize = fileInfo.st_size;
            CheckOverLongPath(fileStat.filePath);
            // mode
            fileStat.mode = static_cast<int32_t>(fileInfo.st_mode);
            int64_t lastUpdateTime = static_cast<int64_t>(fileInfo.st_mtime);
            fileStat.lastUpdateTime = lastUpdateTime;
            fileStat.isIncre = (paras.lastBackupTime == 0 || lastUpdateTime > paras.lastBackupTime) ? true : false;
            if (entry->d_type == DT_DIR) {
                fileStat.isDir = true;
                folderStack.push(path);
            }
            InsertStatFile(path, fileStat, statFile, excludesMap, paras);
        }
        closedir(dirPtr);
    }
    return true;
}

bool StorageManagerService::GetPathWildCard(uint32_t userId, const std::string &bundleName,
    const std::string &includeWildCard, std::vector<std::string> &includePathList,
    std::map<std::string, std::string> &pathMap)
{
    size_t pos = includeWildCard.rfind(WILDCARD_DEFAULT_INCLUDE);
    if (pos == std::string::npos) {
        HILOGE("GetPathWildCard: path should include *");
        return false;
    }
    std::string pathBeforeWildCard = includeWildCard.substr(0, pos);
    DIR *dirPtr = opendir(pathBeforeWildCard.c_str());
    if (dirPtr == nullptr) {
        HILOGE("GetPathWildCard open file dir:%{public}s fail, errno:%{public}d", pathBeforeWildCard.c_str(), errno);
        return false;
    }
    struct dirent *entry = nullptr;
    std::vector<std::string> subDirs;
    while ((entry = readdir(dirPtr)) != nullptr) {
        if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
            continue;
        }
        std::string path = pathBeforeWildCard + entry->d_name;
        if (entry->d_type == DT_DIR) {
            subDirs.emplace_back(path);
        }
    }
    closedir(dirPtr);
    for (auto &subDir : subDirs) {
        DIR *subDirPtr = opendir(subDir.c_str());
        if (subDirPtr == nullptr) {
            HILOGE("GetPathWildCard open file dir:%{private}s fail, errno:%{public}d", subDir.c_str(), errno);
            return false;
        }
        while ((entry = readdir(subDirPtr)) != nullptr) {
            if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
                continue;
            }
            std::string dirName = std::string(entry->d_name);

            std::string path = subDir + FILE_SEPARATOR_CHAR + entry->d_name;
            if (entry->d_type == DT_DIR && (dirName == DEFAULT_INCLUDE_PATH_IN_HAP_FILES ||
                dirName == DEFAULT_INCLUDE_PATH_IN_HAP_DATABASE ||
                dirName == DEFAULT_INCLUDE_PATH_IN_HAP_PREFERENCE)) {
                includePathList.emplace_back(path);
                AddPathMapForPathWildCard(userId, bundleName, path, pathMap);
            }
        }
        closedir(subDirPtr);
    }
    return true;
}

bool StorageManagerService::ExcludeFilter(std::map<std::string, bool> &excludesMap, const std::string &path)
{
    if (path.empty()) {
        HILOGE("ExcludeFilter Param failed");
        return true;
    }
    std::string formatPath = path;
    for (auto exclude = excludesMap.begin(); exclude != excludesMap.end(); exclude++) {
        if (exclude->second != true) {
            if (formatPath.compare(exclude->first) == 0) {
                return true;
            }
        } else {
            if (formatPath.compare(0, exclude->first.size(), exclude->first) == 0 &&
                (formatPath.size() == exclude->first.size() || formatPath[exclude->first.size()] == '/')) {
                return true;
            }
        }
    }
    return false;
}

void StorageManagerService::WriteFileList(std::ofstream &statFile, struct FileStat fileStat, BundleStatsParas &paras)
{
    if (!statFile.is_open() || fileStat.filePath.empty()) {
        HILOGE("WriteFileList Param failed");
        return;
    }
    std::string fileLine = "";
    bool encodeFlag = false;
    if (fileStat.filePath.find(LINE_SEP) != std::string::npos) {
        fileLine += AppFileService::SandboxHelper::Encode(fileStat.filePath) + FILE_CONTENT_SEPARATOR;
        encodeFlag = true;
    } else {
        fileLine += fileStat.filePath + FILE_CONTENT_SEPARATOR;
    }
    fileLine += std::to_string(fileStat.mode) + FILE_CONTENT_SEPARATOR;
    if (fileStat.isDir) {
        fileLine += std::to_string(1) + FILE_CONTENT_SEPARATOR;
    } else {
        fileLine += std::to_string(0) + FILE_CONTENT_SEPARATOR;
    }
    fileLine += std::to_string(fileStat.fileSize) + FILE_CONTENT_SEPARATOR;
    fileLine += std::to_string(fileStat.lastUpdateTime) + FILE_CONTENT_SEPARATOR;
    fileLine += FILE_CONTENT_SEPARATOR;
    if (fileStat.isIncre) {
        fileLine += std::to_string(1);
    } else {
        fileLine += std::to_string(0);
    }
    fileLine += FILE_CONTENT_SEPARATOR;
    if (encodeFlag) {
        fileLine += std::to_string(1);
    } else {
        fileLine += std::to_string(0);
    }
    // te file line
    statFile << fileLine << std::endl;
    if (fileStat.isIncre) {
        paras.incFileSizeSum += fileStat.fileSize;
    }
    paras.fileSizeSum += fileStat.fileSize;
}

bool StorageManagerService::AddOuterDirIntoFileStat(const std::string &dir, BundleStatsParas &paras,
    const std::string &sandboxDir, std::ofstream &statFile, std::map<std::string, bool> &excludesMap)
{
    if (!statFile.is_open() || dir.empty()) {
        HILOGE("AddOuterDirIntoFileStat Param failed");
        return false;
    }
    struct stat fileInfo = {0};
    if (stat(dir.c_str(), &fileInfo) != 0) {
        HILOGE("AddOuterDirIntoFileStat call stat error %{private}s, fail errno:%{public}d", dir.c_str(), errno);
        return false;
    }
    struct FileStat fileStat = {};
    fileStat.filePath = PhysicalToSandboxPath(dir, sandboxDir, dir);
    fileStat.fileSize = fileInfo.st_size;
    // mode
    fileStat.mode = static_cast<int32_t>(fileInfo.st_mode);
    int64_t lastUpdateTime = static_cast<int64_t>(fileInfo.st_mtime);
    fileStat.lastUpdateTime = lastUpdateTime;
    fileStat.isIncre = (paras.lastBackupTime == 0 || lastUpdateTime > paras.lastBackupTime) ? true : false;
    fileStat.isDir = true;
    std::string formatPath = dir;
    if (formatPath.back() != FILE_SEPARATOR_CHAR) {
        formatPath.push_back(FILE_SEPARATOR_CHAR);
    }
    if (ExcludeFilter(excludesMap, formatPath) == false) {
        WriteFileList(statFile, fileStat, paras);
    }
    return true;
}

std::string StorageManagerService::PhysicalToSandboxPath(const std::string &dir, const std::string &sandboxDir,
    const std::string &path)
{
    std::size_t dirPos = dir.size();
    std::string pathSurffix = path.substr(dirPos);
    return sandboxDir + pathSurffix;
}

void StorageManagerService::InsertStatFile(const std::string &path, struct FileStat fileStat,
    std::ofstream &statFile, std::map<std::string, bool> &excludesMap, BundleStatsParas &paras)
{
    if (!statFile.is_open() || path.empty()) {
        HILOGE("InsertStatFile Param failed");
        return;
    }
    std::string formatPath = path;
    if (fileStat.isDir == true && formatPath.back() != FILE_SEPARATOR_CHAR) {
        formatPath.push_back(FILE_SEPARATOR_CHAR);
    }
    if (!ExcludeFilter(excludesMap, formatPath)) {
        WriteFileList(statFile, fileStat, paras);
    }
}

bool StorageManagerService::AddPathMapForPathWildCard(uint32_t userId, const std::string &bundleName,
    const std::string &phyPath, std::map<std::string, std::string> &pathMap)
{
    std::string physicalPrefixEl1 = PHY_APP + EL1 + FILE_SEPARATOR_CHAR + std::to_string(userId) + BASE +
        bundleName + FILE_SEPARATOR_CHAR;
    std::string physicalPrefixEl2 = PHY_APP + EL2 + FILE_SEPARATOR_CHAR + std::to_string(userId) + BASE +
        bundleName + FILE_SEPARATOR_CHAR;
    if (phyPath.find(physicalPrefixEl1) == 0) {
        std::string relatePath = phyPath.substr(physicalPrefixEl1.size());
        pathMap.insert({phyPath, BASE_EL1 + relatePath});
    } else if (phyPath.find(physicalPrefixEl2) == 0) {
        std::string relatePath = phyPath.substr(physicalPrefixEl2.size());
        pathMap.insert({phyPath, BASE_EL2 + relatePath});
    } else {
        HILOGE("Invalid phyiscal path");
        return false;
    }
    return true;
}

uint32_t StorageManagerService::CheckOverLongPath(const std::string &path)
{
    uint32_t len = path.length();
    if (len >= PATH_MAX_LEN) {
        size_t found = path.find_last_of('/');
        std::string sub = path.substr(found + 1);
        HILOGE("Path over long, length:%{public}d, fileName:%{public}s.", len, sub.c_str());
    }
    return len;
}

} // namespace OHOS::FileManagement::Backup