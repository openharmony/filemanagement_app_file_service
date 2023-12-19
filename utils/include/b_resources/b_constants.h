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

enum class ExtensionAction {
    INVALID = 0,
    BACKUP = 1,
    RESTORE = 2,
};

enum ServiceSchedAction {
    WAIT = 0,
    INSTALLING = 1,
    START,
    RUNNING,
    FINISH,
};

constexpr int SPAN_USERID_UID = 20000000;
constexpr int SYSTEM_UID = 0;
constexpr int XTS_UID = 1;
constexpr int DEFAULT_USER_ID = 100;
constexpr int BACKUP_UID = 1089;
constexpr int EXTENSION_THREAD_POOL_COUNT = 1;
constexpr int BACKUP_LOADSA_TIMEOUT_MS = 4000;

constexpr int DECIMAL_BASE = 10; // 十进制基数

constexpr off_t BIG_FILE_BOUNDARY = 2 * 1024 * 1024; // 大文件边界
constexpr unsigned long BIG_FILE_NAME_SIZE = 16;     // 大文件名长度(hash处理)

constexpr int PATHES_TO_BACKUP_SIZE = 13; // 应用默认备份的目录个数
constexpr uint32_t BACKUP_PARA_VALUE_MAX = 5; // 读取backup.para字段值的最大长度
constexpr int SA_THREAD_POOL_COUNT = 1; // SA THREAD_POOL 最大线程数
constexpr int EXT_CONNECT_MAX_COUNT = 3; // extension 最大启动数
constexpr int EXT_CONNECT_MAX_TIME = 15000; // SA 启动 extension 等待连接最大时间

constexpr int IPC_MAX_WAIT_TIME = 3000; // IPC通讯最大等待时间(s)

// backup.para内配置项的名称，该配置项值为true时可在不更新hap包的情况下，可以读取包管理元数据配置文件的内容
static inline std::string BACKUP_DEBUG_OVERRIDE_EXTENSION_CONFIG_KEY = "backup.debug.overrideExtensionConfig";

// backup.para内配置项的名称，该配置项AccountConfig为true时存在时，可以按照配置的AccountNumber备份恢复
static inline std::string BACKUP_DEBUG_OVERRIDE_ACCOUNT_CONFIG_KEY = "backup.debug.overrideAccountConfig";
static inline std::string BACKUP_DEBUG_OVERRIDE_ACCOUNT_NUMBER_KEY = "backup.debug.overrideAccountNumber";

// 应用备份数据暂存路径
static inline std::string_view SA_BUNDLE_BACKUP_BACKUP = "/backup/";
static inline std::string_view SA_BUNDLE_BACKUP_RESTORE = "/restore/";
static inline std::string_view SA_BUNDLE_BACKUP_TMP_DIR = "/tmp/";
static inline std::string_view BACKUP_TOOL_RECEIVE_DIR = "/data/backup/received/";
static inline std::string_view PATH_BUNDLE_BACKUP_HOME_EL1 = "/data/storage/el1/backup";
static inline std::string_view PATH_BUNDLE_BACKUP_HOME = "/data/storage/el2/backup";
static inline std::string_view BACKUP_TOOL_LINK_DIR = "/data/backup";
static inline std::string_view BACKUP_TOOL_INSTALL_DIR = "/data/backup/install/";

// 多用户场景应用备份数据路径
static inline std::string GetSaBundleBackupDir(int32_t userId)
{
    std::string str;
    str.append("/data/service/el2/");
    str.append(std::to_string(userId));
    str.append("/backup/bundles/");
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
static inline std::string_view BACKUP_CONFIG_EXTENSION_PATH = "/data/storage/el2/base/temp/";

// 应用备份恢复所需的索引文件
static inline std::string_view EXT_BACKUP_MANAGE = "manage.json";

// 包管理元数据配置文件
static inline std::string_view BACKUP_CONFIG_JSON = "backup_config.json";

// 恢复应用安装包URL判断
static inline std::string_view RESTORE_INSTALL_PATH = "/data/storage/el2/restore/bundle.hap";

// 特殊版本信息
constexpr int DEFAULT_VERSION_CODE = 0;
static inline std::string_view DEFAULT_VERSION_NAME = "0.0.0.0";
static inline std::string_view DEFAULT_VERSION_NAME_CLONE = "99.99.99.999";
static inline std::string_view DEFAULT_VERSION_NAME_CLONE_2 = "99.99.99.998";
static inline std::string_view DEFAULT_VERSION_NAME_CLONE_3 = "99.99.99.997";
static inline std::string_view DEFAULT_VERSION_NAME_PC = "99.99.99.996";
static inline std::string_view DEFAULT_VERSION_NAME_CLOUD = "99.99.99.995";
static inline std::vector<std::string_view> DEFAULT_VERSION_NAMES_VEC = {
    DEFAULT_VERSION_NAME,
    DEFAULT_VERSION_NAME_CLONE,
    DEFAULT_VERSION_NAME_CLONE_2,
    DEFAULT_VERSION_NAME_CLONE_3,
    DEFAULT_VERSION_NAME_PC,
    DEFAULT_VERSION_NAME_CLOUD,
};

// 应用默认备份的目录，其均为相对根路径的路径。为避免模糊匹配，务必以斜线为结尾。
static inline std::array<std::string_view, PATHES_TO_BACKUP_SIZE> PATHES_TO_BACKUP = {
    "data/storage/el1/database/",
    "data/storage/el1/base/files/",
    "data/storage/el1/base/preferences/",
    "data/storage/el1/base/haps/*/database/",
    "data/storage/el1/base/haps/*/files/",
    "data/storage/el1/base/haps/*/preferences/",
    "data/storage/el2/database/",
    "data/storage/el2/base/files/",
    "data/storage/el2/base/preferences/",
    "data/storage/el2/base/haps/*/database/",
    "data/storage/el2/base/haps/*/files/",
    "data/storage/el2/base/haps/*/preferences/",
    "data/storage/el2/distributedfiles/",
};
} // namespace OHOS::FileManagement::Backup::BConstants

#endif // OHOS_FILEMGMT_BACKUP_B_CONSTANTS_H
