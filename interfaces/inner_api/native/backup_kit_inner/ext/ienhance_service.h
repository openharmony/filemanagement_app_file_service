/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#ifndef OHOS_FILEMGMT_BACKUP_IENHANCE_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_IENHANCE_SERVICE_H
 
#include <string>
#include <vector>
#include <set>
#include "ianco_backup_callback.h"
#include "anco_scan_result.h"
#include "anco_restore_result.h"
#include "b_error/b_error.h"
 
namespace OHOS::FileManagement::Backup {
class IEnhanceService {
public:
    virtual ~IEnhanceService() = default;
 
    virtual ErrCode CreateAncoBackupTask(const std::string &bundleName, const sptr<IAncoBackupCallback> &callback) = 0;
    virtual ErrCode DestroyAncoBackupTask(const std::string &bundleName) = 0;
    virtual ErrCode FilterAndSaveBackupPaths(const std::string &bundleName, std::set<std::string> &includes,
        std::set<std::string> &compatIncludes, const std::vector<std::string> &excludes) = 0;
    virtual ErrCode StartAncoScanAllDirs(const std::string &bundleName, AncoScanResult &scanResult) = 0;
    virtual ErrCode StartAncoPacket(const std::string &bundleName, uint64_t &smallFileCount) = 0;
    virtual ErrCode CreateAncoRestoreTask(const std::string &bundleName) = 0;
    virtual ErrCode DestroyAncoRestoreTask(const std::string &bundleName) = 0;
    virtual ErrCode StartAncoUnPacket(const std::string &bundleName, const std::vector<std::string> &tarFiles,
        const std::vector<int64_t> &tarFileSizes, const std::vector<std::string> &tarFileNames,
        const std::string &rootPath) = 0;
    virtual ErrCode StartAncoMove(const std::string &bundleName, const std::vector<std::string> &ancoSourcePath,
        const std::vector<std::string> &ancoTargetPath, const std::vector<StatInfo> &ancoStats,
        AncoRestoreResult &ancoRestoreRes) = 0;
};
}  // namespace OHOS::FileManagement::Backup
#endif