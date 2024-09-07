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

#ifndef OHOS_FILEMGMT_BACKUP_B_CONSTANTS_H
#define OHOS_FILEMGMT_BACKUP_B_CONSTANTS_H

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <unistd.h>

namespace OHOS::FileManagement::Backup::BConstants {

static inline const char *EXTENSION_ACTION_PARA = "extensionAction";
static inline const char *EXTENSION_RESTORE_TYPE_PARA = "restoreType";
static inline const char *EXTENSION_VERSION_CODE_PARA = "versionCode";
static inline const char *EXTENSION_VERSION_NAME_PARA = "versionName";
static inline const char *EXTENSION_RESTORE_EXT_INFO_PARA = "restoreExtInfo";
static inline const char *EXTENSION_BACKUP_EXT_INFO_PARA = "backupExtInfo";
static inline const char *EXTENSION_APP_CLONE_INDEX_PARA = "ohos.extra.param.key.appCloneIndex";

enum class ExtensionAction {
    INVALID = 0,
    BACKUP = 1,
    RESTORE = 2,
};

enum ServiceSchedAction {
    WAIT = 0,
    START = 1,
    RUNNING = 2,
    FINISH = 3,
    CLEAN = 4,
};

constexpr int SPAN_USERID_UID = 200000;
constexpr int SYSTEM_UID = 0;
constexpr int XTS_UID = 1;
constexpr int DEFAULT_USER_ID = 100;
constexpr int BACKUP_UID = 1089;
constexpr int EXTENSION_THREAD_POOL_COUNT = 1;
constexpr int BACKUP_LOADSA_TIMEOUT_MS = 4000;

constexpr int DECIMAL_BASE = 10; // 十进制基数

constexpr off_t BIG_FILE_BOUNDARY = 2 * 1024 * 1024; // 大文件边界
constexpr unsigned long BIG_FILE_NAME_SIZE = 16;     // 大文件名长度(hash处理)

constexpr int PATHES_TO_BACKUP_SIZE = 13;     // 应用默认备份的目录个数
constexpr uint32_t BACKUP_PARA_VALUE_MAX = 5; // 读取backup.para字段值的最大长度
constexpr int SA_THREAD_POOL_COUNT = 1;       // SA THREAD_POOL 最大线程数
constexpr int EXT_CONNECT_MAX_COUNT = 3;      // extension 最大启动数
constexpr int EXT_CONNECT_MAX_TIME = 15000;   // SA 启动 extension 等待连接最大时间

constexpr int IPC_MAX_WAIT_TIME = 3000; // IPC通讯最大等待时间(s)
constexpr int MAX_PARCELABLE_VECTOR_NUM = 10000;
constexpr char FILE_SEPARATOR_CHAR = '/';

// 分片打包常量
const uint64_t DEFAULT_SLICE_SIZE = 100 * 1024 * 1024; // 分片文件大小为100M
const uint32_t MAX_FILE_COUNT = 6000; // 单个tar包最多包含6000个文件
const int FILE_AND_MANIFEST_FD_COUNT = 2; // 每组文件和简报数量统计

constexpr int DEFAULT_VFS_CACHE_PRESSURE = 100; // 默认内存回收参数
constexpr int BACKUP_VFS_CACHE_PRESSURE = 10000; // 备份过程修改参数

constexpr int32_t INVALID_FD_NUM = -1;

constexpr int MAX_FD_SEND_RATE = 800; // 允许应用申请的最大FD数量
constexpr int MIN_FD_SEND_RATE = 0; // 允许应用申请的最小FD数量
constexpr int DEFAULT_FD_SEND_RATE = 60; // 框架默认的FD数量
constexpr int32_t PARAM_STARING_MAX_MEMORY = 2 * 1024 * 1024;
constexpr uint32_t H2MS = 60 * 60 * 1000;

constexpr int CALL_APP_ON_PROCESS_TIME_INTERVAL = 5; // 框架每隔5s去调用应用的onProcess
constexpr int APP_ON_PROCESS_MAX_TIMEOUT = 1000; // 应用的onProcess接口最大超时时间为1秒
constexpr int APP_ON_PROCESS_TIMEOUT_MAX_COUNT = 3; // 应用的onProcess接口超时的上限次数

// backup.para内配置项的名称，该配置项值为true时可在不更新hap包的情况下，可以读取包管理元数据配置文件的内容
static inline std::string BACKUP_DEBUG_OVERRIDE_EXTENSION_CONFIG_KEY = "backup.debug.overrideExtensionConfig";

// backup.para内配置项的名称，该配置项AccountConfig为true时存在时，可以按照配置的AccountNumber备份恢复
static inline std::string BACKUP_DEBUG_OVERRIDE_ACCOUNT_CONFIG_KEY = "backup.debug.overrideAccountConfig";
static inline std::string BACKUP_DEBUG_OVERRIDE_ACCOUNT_NUMBER_KEY = "backup.debug.overrideAccountNumber";

// 增量备份相关处理目录
static const std::string BACKUP_PATH_PREFIX = "/data/service/el2/";
static const std::string BACKUP_PATH_SURFFIX = "/backup/backup_sa/";
static const std::string BACKUP_INCEXC_SYMBOL = "incExc_";
static const std::string BACKUP_STAT_SYMBOL = "stat_";
static const std::string BACKUP_INCLUDE = "INCLUDES";
static const std::string BACKUP_EXCLUDE = "EXCLUDES";

// backup.para内配置项的名称，该配置项为true时备份恢复支持Release接口调用
static inline std::string BACKUP_OVERRIDE_BACKUP_SA_RELEASE_KEY = "backup.overrideBackupSARelease";

// backup.para内配置项的名称，该配置项为true时备份恢复支持增量恢复
static inline std::string BACKUP_OVERRIDE_INCREMENTAL_KEY = "backup.overrideIncrementalRestore";

// 应用备份数据暂存路径
static inline std::string_view SA_BUNDLE_BACKUP_BACKUP = "/backup/";
static inline std::string_view SA_BUNDLE_BACKUP_RESTORE = "/restore/";
static inline std::string_view SA_BUNDLE_BACKUP_TMP_DIR = "/tmp/";
static inline std::string_view BACKUP_TOOL_RECEIVE_DIR = "/data/backup/received/";
static inline std::string_view PATH_BUNDLE_BACKUP_HOME_EL1 = "/data/storage/el1/base/.backup";
static inline std::string_view PATH_BUNDLE_BACKUP_HOME = "/data/storage/el2/base/.backup";
static inline std::string_view PATH_FILEMANAGE_BACKUP_HOME = "/storage/Users/currentUser/.backup";
static inline std::string_view PATH_MEDIALDATA_BACKUP_HOME = "/storage/media/local/files/.backup";
static inline std::string_view BACKUP_TOOL_LINK_DIR = "/data/backup";
static inline std::string_view BACKUP_TOOL_INCREMENTAL_RECEIVE_DIR = "/data/backup/incrementalreceived/";
static inline std::string_view BACKUP_TOOL_MANIFEST = "/manifest";
static inline std::string_view BACKUP_TOOL_INCREMENTAL = "/incremental";
static inline std::string BACKUP_DIR_PRE = "/data/storage/";
static inline std::string CONTEXT_ELS[] = {"el1", "el2"};
static inline std::string BACKUP_DIR_END = "/base/.backup/";

// 文管bundleName
static inline std::string BUNDLE_FILE_MANAGER = "hmos.filemanager";
// 文管bundleNameSize
constexpr size_t FM_LEN = 27;
// 媒体库数据bundleName
static inline std::string BUNDLE_MEDIAL_DATA = "com.ohos.medialibrary.medialibrarydata";
// SA Ext
constexpr int BACKUP_DEFAULT_SA_ID = -1;
constexpr int BACKUP_SA_RELOAD_MAX = 2;
static inline std::string EXTENSION_BACKUP = "backup";
static inline std::string EXTENSION_RESTORE = "restore";

// 多用户场景应用备份数据路径
static inline std::string GetSaBundleBackupDir(int32_t userId)
{
    std::string str;
    str.append("/data/app/el2/");
    str.append(std::to_string(userId));
    str.append("/base/.backup/");
    return str;
}

static inline std::string GetSaBundleBackupRootDir(int32_t userId)
{
    std::string str;
    str.append("/data/service/el2/");
    str.append(std::to_string(userId));
    str.append("/backup/backup_sa/");
    return str;
}

static inline std::string GetSaBundleBackupToolDir(int32_t userId)
{
    std::string str;
    str.append("/data/service/el2/");
    str.append(std::to_string(userId));
    str.append("/backup/backup_tool/");
    return str;
}

// 备份恢复配置文件暂存路径
static inline std::string_view BACKUP_CONFIG_EXTENSION_PATH = "/data/storage/el2/base/cache/";

// 应用备份恢复所需的索引文件
static inline std::string_view EXT_BACKUP_MANAGE = "manage.json";

// 包管理元数据配置文件
static inline std::string_view BACKUP_CONFIG_JSON = "backup_config.json";

// 简报文件名后缀
static inline std::string_view REPORT_FILE_EXT = "rp";

// 空简报
static inline std::string BLANK_REPORT_NAME = "blankReport.rp";

// 特殊版本信息
constexpr int DEFAULT_VERSION_CODE = 0;
constexpr char VERSION_NAME_SEPARATOR_CHAR = '-';
static inline std::string_view DEFAULT_VERSION_NAME = "0.0.0.0";
static inline std::string_view DEFAULT_VERSION_NAME_CLONE = "99.99.99.999";
static inline std::string_view DEFAULT_VERSION_NAME_CLONE_2 = "99.99.99.998";
static inline std::string_view DEFAULT_VERSION_NAME_CLONE_3 = "99.99.99.997";
static inline std::string_view DEFAULT_VERSION_NAME_PC = "99.99.99.996";
static inline std::string_view DEFAULT_VERSION_NAME_CLOUD = "99.99.99.995";
static inline std::vector<std::string_view> DEFAULT_VERSION_NAMES_VEC = {
    DEFAULT_VERSION_NAME,         DEFAULT_VERSION_NAME_CLONE, DEFAULT_VERSION_NAME_CLONE_2,
    DEFAULT_VERSION_NAME_CLONE_3, DEFAULT_VERSION_NAME_PC,    DEFAULT_VERSION_NAME_CLOUD,
};

// 应用默认备份的目录，其均为相对根路径的路径。为避免模糊匹配，务必以斜线为结尾。
static inline std::array<std::string_view, PATHES_TO_BACKUP_SIZE> PATHES_TO_BACKUP = {
    "data/storage/el1/database/",          "data/storage/el1/base/files/",
    "data/storage/el1/base/preferences/",  "data/storage/el1/base/haps/*/database/",
    "data/storage/el1/base/haps/*/files/", "data/storage/el1/base/haps/*/preferences/",
    "data/storage/el2/database/",          "data/storage/el2/base/files/",
    "data/storage/el2/base/preferences/",  "data/storage/el2/base/haps/*/database/",
    "data/storage/el2/base/haps/*/files/", "data/storage/el2/base/haps/*/preferences/",
    "data/storage/el2/distributedfiles/",
};

// napi 层代码引用到的常量，对应js接口声明(@ohos.file.backup.d.ts)中的属性字段
static inline std::string BUNDLE_NAME = "bundleName";
static inline std::string URI = "uri";
static inline std::string FD = "fd";
static inline std::string MANIFEST_FD = "manifestFd";
static inline std::string LAST_INCREMENTAL_TIME = "lastIncrementalTime";
static inline std::string PARAMETERS = "parameters";
static inline std::string PRIORITY = "priority";

// 雷达打点引用到的常量
constexpr int32_t MS_1000 = 1000;
constexpr uint8_t INDEX = 3;
static inline std::string FILE_BACKUP_RESTORE_EVENTS = "FILE_BACKUP_RESTORE_EVENTS";
} // namespace OHOS::FileManagement::Backup::BConstants

#endif // OHOS_FILEMGMT_BACKUP_B_CONSTANTS_H
