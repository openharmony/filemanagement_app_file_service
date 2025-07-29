/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_RADAR_CONST_INNER_H
#define OHOS_FILEMGMT_BACKUP_RADAR_CONST_INNER_H

#include <string>

namespace OHOS::FileManagement::Backup {

constexpr int32_t DEFAULT_STAGE = 1;
constexpr int32_t STAGE_RES_IDLE = 0;
constexpr int32_t STAGE_RES_SUCCESS = 1;
constexpr int32_t STAGE_RES_FAIL = 2;
constexpr int32_t STAGE_RES_CANCEL = 3;

constexpr char DOMAIN[] = "APP_FILE_SVC";
const std::string DOMAIN_NAME = "APP_FILE_SVC";
const std::string BACKUP_RESTORE_APP_STATISTIC = "BACKUP_RESTORE_APP_STATISTIC";
const std::string BACKUP_RESTORE_STATISTIC = "BACKUP_RESTORE_STATISTIC";

const std::string ORG_PKG = "ORG_PKG";
const std::string FUNC = "FUNC";
const std::string BIZ_SCENE = "BIZ_SCENE";
const std::string BIZ_STAGE = "BIZ_STAGE";
const std::string STAGE_RES = "STAGE_RES";
const std::string CONCURRENT_ID = "CONCURRENT_ID";
const std::string ERROR_CODE = "ERROR_CODE";
const std::string ERROR_MSG = "ERROR_MSG";

const std::string HOST_PKG = "HOST_PKG";
const std::string APP_CALLER = "APP_CALLER";
const std::string MODE = "MODE";
const std::string FILE_SIZE_DIST = "FILE_SIZE_DIST";
const std::string FILE_TYPE_DIST = "FILE_TYPE_DIST";
const std::string SMALL_FILE_COUNT = "SMALL_FILE_COUNT";
const std::string SMALL_FILE_SIZE = "SMALL_FILE_SIZE";
const std::string BIG_FILE_COUNT = "BIG_FILE_COUNT";
const std::string BIG_FILE_SIZE = "BIG_FILE_SIZE";
const std::string TAR_FILE_COUNT = "TAR_FILE_COUNT";
const std::string TAR_FILE_SIZE = "TAR_FILE_SIZE";
const std::string TAR_BOUND_SIZE = "TAR_BOUND_SIZE";
const std::string DIR_DEPTH = "DIR_DEPTH";
const std::string MANAGE_JSON_SIZE = "MANAGE_JSON_SIZE";
const std::string EXTENSION_CONNECT_SPEND = "EXTENSION_CONNECT_SPEND";
const std::string GET_EXT_INFO_SPEND = "GET_EXT_INFO_SPEND";

const std::string ON_BACKUP_SPEND = "ON_BACKUP_SPEND";
const std::string ON_BACKUPEX_SPEND = "ON_BACKUPEX_SPEND";
const std::string DO_BACKUP_SPEND = "DO_BACKUP_SPEND";
const std::string TAR_SPEND = "TAR_SPEND";
const std::string HASH_SPEND = "HASH_SPEND";
const std::string SCAN_FILE_SPEND = "SCAN_FILE_SPEND";
const std::string SEND_RATE_ZERO_SPAN = "SEND_RATE_ZERO_SPAN";

const std::string ON_RESTORE_SPEND = "ON_RESTORE_SPEND";
const std::string ON_RESTOREEX_SPEND = "ON_RESTOREEX_SPEND";
const std::string DO_RESTORE_SPEND = "DO_RESTORE_SPEND";
const std::string UNTAR_SPEND = "UNTAR_SPEND";
const std::string BIG_FILE_SPEND = "BIG_FILE_SPEND";

const std::string GET_BUNDLE_INFO_SPEND = "GET_BUNDLE_INFO_SPEND";
const std::string TOTAL_SPEND = "TOTAL_SPEND";
const std::string SUCC_BUNDLE_CNT = "SUCC_BUNDLE_CNT";
const std::string FAIL_BUNDLE_CNT = "FAIL_BUNDLE_CNT";

} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_RADAR_CONST_INNER_H
