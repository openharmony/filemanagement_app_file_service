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
};

enum EntryKey {
    SUPER_LONG_PATH = 0,
    SUPER_LONG_LINK_PATH,
    SUPER_LONG_SIZE,
};

constexpr int SPAN_USERID_UID = 20000000;
constexpr int SYSTEM_UID = 0;
constexpr int DEFAULT_USER_ID = 100;
constexpr int BACKUP_UID = 1089;
constexpr int EXTENSION_THREAD_POOL_COUNT = 1;
constexpr int BACKUP_LOADSA_TIMEOUT_MS = 4000;

constexpr int DECIMAL_BASE = 10; // 十进制基数

constexpr int HEADER_SIZE = 512;         // 打包文件头部Header结构体大小
constexpr int BLOCK_SIZE = 512;          // 打包文件数据段尾部补充的全零字节块上限大小
constexpr int BLOCK_PADDING_SIZE = 1024; // 打包文件尾部追加的全零字节块大小
constexpr off_t BIG_FILE_BOUNDARY = 1024 * 1024 * 1024; // 大文件边界
constexpr unsigned long BIG_FILE_NAME_SIZE = 16;        // 大文件名长度(hash处理)

// 打包文件头部Header结构体各字段数组/字符串大小。
constexpr int PATHNAME_MAX_SIZE = 100;
constexpr int MODE_MAX_SIZE = 8;
constexpr int UGID_MAX_SIZE = 8;
constexpr int FILESIZE_MAX_SIZE = 12;
constexpr int TIME_MAX_SIZE = 12;
constexpr int CHKSUM_MAX_SIZE = 8;
constexpr int LINKNAME_MAX_SIZE = 100;
constexpr int MAGIC_SIZE = 6;
constexpr int VERSION_SIZE = 2;
constexpr int UGNAME_MAX_SIZE = 32;
constexpr int DEV_MAX_SIZE = 8;
constexpr int PREFIX_SIZE = 155;
constexpr int PADDING_SIZE = 12;

// 读取backup.para字段值的最大长度
constexpr uint32_t BACKUP_PARA_VALUE_MAX = 5;

// SA THREAD_POOL 最大线程数
constexpr int SA_THREAD_POOL_COUNT = 1;
// extension 最大启动数
constexpr int EXT_CONNECT_MAX_COUNT = 3;
// SA 启动 extension 等待连接最大时间
constexpr int EXT_CONNECT_MAX_TIME = 15000;

// 打包文件头部Header结构体fileSize字段最大值。
constexpr off_t FILESIZE_MAX = 077777777777;

// 打包文件头部Header结构体typeFlag字段值。
constexpr char TYPEFLAG_REGULAR_FILE = '0';
constexpr char TYPEFLAG_SYMBOLIC_LINK = '2';
constexpr char TYPEFLAG_DIRECTORY = '5';
constexpr char TYPEFLAG_EXTENDED = 'x';

// 打包文件扩展数据段字段值。
static inline std::string ENTRY_NAME_LINKPATH = "linkpath";
static inline std::string ENTRY_NAME_PATH = "path";
static inline std::string ENTRY_NAME_SIZE = "size";

// backup.para内配置项的名称，改配置项值为true时可在不更新hap包的情况下，可以读取包管理元数据配置文件的内容
static inline std::string BACKUP_DEBUG_OVERRIDE_EXTENSION_CONFIG_KEY = "backup.debug.overrideExtensionConfig";

// 应用备份数据暂存路径。
static inline std::string_view SA_BUNDLE_BACKUP_DIR = "/data/service/el2/100/backup/bundles/";
static inline std::string_view SA_BUNDLE_BACKUP_BACKUP = "/backup/";
static inline std::string_view SA_BUNDLE_BACKUP_RESTORE = "/restore/";
static inline std::string_view SA_BUNDLE_BACKUP_ROOT_DIR = "/data/service/el2/100/backup/backup_sa/";
static inline std::string_view SA_BUNDLE_BACKUP_TMP_DIR = "/tmp/";
static inline std::string_view BACKUP_TOOL_RECEIVE_DIR = "/data/backup/received/";
static inline std::string_view PATH_BUNDLE_BACKUP_HOME = "/data/storage/el2/backup";
static inline std::string_view SA_BUNDLE_BACKUP_TOOL_DIR = "/data/service/el2/100/backup/backup_tool/";
static inline std::string_view BACKUP_TOOL_LINK_DIR = "/data/backup";

// 备份恢复配置文件暂存路径
static inline std::string_view BACKUP_CONFIG_EXTENSION_PATH = "/data/storage/el2/base/temp/";

// 应用备份恢复所需的索引文件
static inline std::string_view EXT_BACKUP_MANAGE = "manage.json";

// 包管理元数据配置文件
static inline std::string_view BACKUP_CONFIG_JSON = "backup_config.json";

// 应用默认备份的目录，其均为相对根路径的路径。为避免模糊匹配，务必以斜线为结尾。
static inline std::array<std::string_view, 6> PATHES_TO_BACKUP = {
    "data/storage/el2/database/",
    "data/storage/el2/base/files/",
    "data/storage/el2/base/preferences/",
    "data/storage/el2/base/haps/*/database/",
    "data/storage/el2/base/haps/*/base/files/",
    "data/storage/el2/base/haps/*/base/preferences/",
};
} // namespace OHOS::FileManagement::Backup::BConstants

#endif // OHOS_FILEMGMT_BACKUP_B_CONSTANTS_H