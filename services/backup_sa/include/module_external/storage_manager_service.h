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

#ifndef OHOS_FILEMGMT_BACKUP_STORAGE_MGR_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_STORAGE_MGR_SERVICE_H

#include <string>
#include <vector>
#include <unistd.h>

#include "istorage_manager.h"
#include "datashare_abs_result_set.h"
#include "datashare_helper.h"
#include "datashare_predicates.h"

namespace OHOS::FileManagement::Backup {
const uint64_t PATH_MAX_LEN = 4096;
const std::string PHY_APP = "/data/app/";
const std::string WILDCARD_DEFAULT_INCLUDE = "*";
constexpr char FILE_SEPARATOR_CHAR = '/';
const std::string BASE_EL1 = "/data/storage/el1/base/";
const std::string BASE_EL2 = "/data/storage/el2/base/";
const std::string EL1 = "el1";
const std::string EL2 = "el2";
const std::string BASE = "/base/";
const std::string BACKUP_INCLUDE = "INCLUDES";
const std::string BACKUP_EXCLUDE = "EXCLUDES";
const std::string BACKUP_PATH_PREFIX = "/data/service/el2/";
const std::string BACKUP_PATH_SURFFIX = "/backup/backup_sa/";
const std::string BACKUP_INCEXC_SYMBOL = "incExc_";
const std::string BACKUP_STAT_SYMBOL = "stat_";
const std::string DEFAULT_INCLUDE_PATH_IN_HAP_FILES = "files";
const std::string DEFAULT_INCLUDE_PATH_IN_HAP_DATABASE = "database";
const std::string DEFAULT_INCLUDE_PATH_IN_HAP_PREFERENCE = "preferences";
const std::string MEDIA_CLOUD_SAND_PREFIX = "/storage/cloud";
const std::string MEDIA_SAND_PREFIX = "/storage/media";
const std::string URI_PREFIX = "file://";
const std::string NORMAL_SAND_PREFIX = "/data/storage";
const std::string FILE_SAND_PREFIX = "/storage/Users";
const std::string FILE_AUTHORITY = "docs";
const std::string DEFAULT_PATH_WITH_WILDCARD = "haps/*";
const std::string FILE_CONTENT_SEPARATOR = ";";
const char LINE_SEP = '\n';
const std::string VER_10_LINE1 = "version=1.0&attrNum=8";
const std::string VER_10_LINE2 = "path;mode;dir;size;mtime;hash;isIncremental;encodeFlag";
const std::string MEDIALIBRARY_DATA_URI = "datashare:///media";
const std::string MEDIA_QUERYOPRN_QUERYVOLUME = "query_media_volume";
const std::string MEDIA_TYPE = "media";
const std::string FILE_TYPE = "file";
const std::int32_t E_OK = 0;
constexpr int32_t E_ERR = -1;
constexpr int32_t STORAGE_SERVICE_SYS_CAP_TAG = 13600000;
constexpr const char *QUOTA_DEVICE_DATA_PATH = "/data";
constexpr const char *PROC_MOUNTS_PATH = "/proc/mounts";
constexpr const char *DEV_BLOCK_PATH = "/dev/block/";
const std::int32_t E_SA_IS_NULLPTR = STORAGE_SERVICE_SYS_CAP_TAG + 12;
const std::int32_t E_REMOTE_IS_NULLPTR = STORAGE_SERVICE_SYS_CAP_TAG + 13;
const std::int32_t E_MEDIALIBRARY_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 1202;
const std::int32_t E_QUERY = STORAGE_SERVICE_SYS_CAP_TAG + 1206;
const std::int32_t E_GETROWCOUNT = STORAGE_SERVICE_SYS_CAP_TAG + 1207;
const std::int32_t E_SYS_KERNEL_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 8;
const int MEDIA_TYPE_IMAGE = 1;
const int MEDIA_TYPE_VIDEO = 2;
const int MEDIA_TYPE_AUDIO = 3;
const int32_t GET_DATA_SHARE_HELPER_TIMES = 5;
const int UID_FILE_MANAGER = 1006;
const int32_t USER_ID_BASE = 200000;
static std::map<std::string, std::string> mQuotaReverseMounts;


struct FileStat {
    std::string filePath;
    int64_t fileSize;
    int64_t lastUpdateTime;
    int32_t mode;
    bool isDir;
    bool isIncre;
};
struct BundleStatsParas {
    uint32_t userId;
    std::string &bundleName;
    int64_t lastBackupTime;
    int64_t fileSizeSum;
    int64_t incFileSizeSum;
};

class StorageManagerService {
public:
    static StorageManagerService &GetInstance()
    {
        static StorageManagerService instance {};
        return instance;
    }
    /**
    * @brief Get the bundle stats object
    *
    * @param bundleName bundle name
    */
    bool GetBundleStats(const std::string &bundleName, StorageManager::BundleStats &bundleStats);

    /**
    * @brief Get the user storage stats object
    *
    * @param bundleName bundle name
    * @param userId user id
    */
    int32_t GetUserStorageStatsByType(int32_t userId, StorageManager::StorageStats &storageStats, std::string type);

    /**
    * @brief update memory para
    *
    * @param size para data
    */
    int32_t UpdateMemoryPara(int32_t size, int32_t oldSize);

    /**
    * @brief Get the user storage stats object
    *
    * @param userId user id
    * @param bundleNames
    * @param incrementalBackTimes
    * @param pkgFileSizes bundle backup file size
    * @param incPkgFileSizes Incremental bundle backup file size
    */
    int32_t GetBundleStatsForIncrease(uint32_t userId, const std::vector<std::string> &bundleNames,
        const std::vector<int64_t> &incrementalBackTimes, std::vector<int64_t> &pkgFileSizes,
        std::vector<int64_t> &incPkgFileSizes);
private:
    StorageManagerService() = default;
    ~StorageManagerService() = default;
    void GetBundleStatsForIncreaseEach(uint32_t userId, std::string &bundleName, int64_t lastBackupTime,
    std::vector<int64_t> &pkgFileSizes, std::vector<int64_t> &incPkgFileSizes);
    std::tuple<std::vector<std::string>, std::vector<std::string>> ReadIncludesExcludesPath(
    const std::string &bundleName, const int64_t lastBackupTime, const uint32_t userId);
    void DealWithIncludeFiles(const BundleStatsParas &paras, const std::vector<std::string> &includes,
    std::vector<std::string> &phyIncludes, std::map<std::string, std::string>& pathMap);
    void ConvertSandboxRealPath(const uint32_t userId, const std::string &bundleName,
    const std::string &sandboxPathStr, std::vector<std::string> &realPaths,
    std::map<std::string, std::string>& pathMap);
    void DeduplicationPath(std::vector<std::string> &configPaths);
    void ScanExtensionPath(BundleStatsParas &paras,
    const std::vector<std::string> &includes, const std::vector<std::string> &excludes,
    std::map<std::string, std::string> &pathMap, std::ofstream &statFile);
    void RecognizeSandboxWildCard(const uint32_t userId, const std::string &bundleName,
    const std::string &sandboxPathStr, std::vector<std::string> &phyIncludes,
    std::map<std::string, std::string>& pathMap);
    void SetExcludePathMap(std::string &excludePath, std::map<std::string, bool> &excludesMap);
    std::tuple<bool, bool> CheckIfDirForIncludes(const std::string &path, BundleStatsParas &paras,
    std::map<std::string, std::string> &pathMap, std::ofstream &statFile, std::map<std::string, bool> &excludesMap);
    bool GetIncludesFileStats(const std::string &dir, BundleStatsParas &paras,
    std::map<std::string, std::string> &pathMap,
    std::ofstream &statFile, std::map<std::string, bool> &excludesMap);
    bool GetPathWildCard(uint32_t userId, const std::string &bundleName, const std::string &includeWildCard,
    std::vector<std::string> &includePathList, std::map<std::string, std::string> &pathMap);
    bool ExcludeFilter(std::map<std::string, bool> &excludesMap, const std::string &path);
    void WriteFileList(std::ofstream &statFile, struct FileStat fileStat, BundleStatsParas &paras);
    bool AddOuterDirIntoFileStat(const std::string &dir, BundleStatsParas &paras, const std::string &sandboxDir,
    std::ofstream &statFile, std::map<std::string, bool> &excludesMap);
    std::string PhysicalToSandboxPath(const std::string &dir, const std::string &sandboxDir, const std::string &path);
    void InsertStatFile(const std::string &path, struct FileStat fileStat,
    std::ofstream &statFile, std::map<std::string, bool> &excludesMap, BundleStatsParas &paras);
    bool AddPathMapForPathWildCard(uint32_t userId, const std::string &bundleName, const std::string &phyPath,
    std::map<std::string, std::string> &pathMap);
    uint32_t CheckOverLongPath(const std::string &path);
    int32_t GetMediaStorageStats(StorageManager::StorageStats &storageStats);
    void GetMediaTypeAndSize(const std::shared_ptr<DataShare::DataShareResultSet> &resultSet,
    StorageManager::StorageStats &storageStats);
    int32_t GetFileStorageStats(int32_t userId, StorageManager::StorageStats &storageStats);
    bool InitialiseQuotaMounts();
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_STORAGE_MGR_SERVICE_H