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

#include <fcntl.h>
#include <set>
#include <string>
#include <vector>
#include <set>
#include "ianco_backup_callback.h"
#include "ianco_restore_callback.h"
#include "anco_scan_result.h"
#include "anco_restore_result.h"
#include "b_error/b_error.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
static constexpr int32_t MIGRATE_LEN_FILE_PATH = 4096;
static constexpr int32_t MIGRATE_PERM_LEN = 16;
static constexpr int32_t MIGRATE_ID_LEN = 36;
struct FileBackupParam {
    char srcFilePath[MIGRATE_LEN_FILE_PATH];
    char dstFilePath[MIGRATE_LEN_FILE_PATH];
    char uid[MIGRATE_ID_LEN] = {0};
    char gid[MIGRATE_ID_LEN] = {0};
    char perm[MIGRATE_PERM_LEN] = {0};
    struct stat sta;
    bool isBigFile = false;
    int32_t recurseOpt = 0;
};

struct ResultParam {
    char filePath[MIGRATE_LEN_FILE_PATH];
    char fileName[MIGRATE_LEN_FILE_PATH];
    uint64_t size;
    struct stat sta;
    int32_t errCode;
    int32_t fd;
};

struct FileBackupResultMsg {
    int32_t cmdId;
    int32_t result;
    int32_t errorCode;
    int64_t totalBigFileSize;
    int64_t totalSmallFileSize;
    std::vector<ResultParam> resInfo;
};

class IEnhanceService {
public:
    virtual ~IEnhanceService() = default;

    virtual ErrCode CreateAncoBackupTask(const std::string &bundleName, const sptr<IAncoBackupCallback> &callback) = 0;
    virtual ErrCode DestroyAncoBackupTask(const std::string &bundleName) = 0;
    virtual ErrCode FilterAndSaveBackupPaths(const std::string &bundleName, std::set<std::string> &includes,
        std::set<std::string> &compatIncludes, const std::vector<std::string> &excludes) = 0;
    virtual ErrCode StartAncoScanAllDirs(const std::string &bundleName, AncoScanResult &scanResult) = 0;
    virtual ErrCode StartAncoPacket(const std::string &bundleName, uint64_t &smallFileCount) = 0;
    virtual ErrCode CreateAncoRestoreTask(const std::string &bundleName,
        const sptr<IAncoRestoreCallback> &callback) = 0;
    virtual ErrCode DestroyAncoRestoreTask(const std::string &bundleName) = 0;
    virtual ErrCode AddAncoTars(const std::string &bundleName, const std::vector<std::string> &tarFiles,
        const std::vector<int64_t> &tarFileSizes, const std::vector<std::string> &tarFileNames) = 0;
    virtual ErrCode StartAncoUnPacket(const std::string &bundleName, const std::string &rootPath) = 0;
    virtual ErrCode AddAncoMovePaths(const std::string &bundleName, const std::vector<std::string> &ancoSourcePath,
        const std::vector<std::string> &ancoTargetPath, const std::vector<StatInfo> &ancoStats) = 0;
    virtual ErrCode StartAncoMove(const std::string &bundleName, int &fd, AncoRestoreResult &ancoRestoreRes) = 0;
    virtual int OpenAncoFileReadOnly(const std::string &sandboxPath) = 0;
    virtual bool RemoveAncoFile(const std::string &sandboxPath) = 0;
    virtual void GetIncrementalAncoFileHandle(const std::string &bundleName, const std::string &fileName, int &fdVal,
        int &reportFdVal, int &errCode) = 0;
    virtual int32_t MoveFiles(const std::vector<FileBackupParam> &fileInfo, FileBackupResultMsg &resultMsg) = 0;
    virtual int32_t OpenFiles(const std::vector<FileBackupParam> &fileInfo, FileBackupResultMsg &resultMsg) = 0;
// default clone
    virtual ErrCode CreateDefaultTask(const std::string &bundleName, int32_t userId) = 0;
    virtual ErrCode DefaultAppFileHandle(const std::string &bundleName,
        const std::string &fileName, UniqueFd &fd, UniqueFd &reportFd) = 0;
    virtual ErrCode StartDefaultAppUnPack(const std::string &bundleName, AncoRestoreResult &ancoRestoreRes) = 0;
    virtual void StartDefaultAppClear(const std::string &bundleName) = 0;
    virtual ErrCode DefaultAppRestoreBigFiles(const std::string &bundleName, bool appendTargetPath,
        AncoRestoreResult &ancoRestoreRes) = 0;
    virtual ErrCode GetIndexFile(const std::string &bundleName, const std::string &path,
        int &size, UniqueFd &fd) = 0;
    virtual void StartDefaultPacket(const std::string &bundleName) = 0;
    virtual ErrCode StartDefaultScanAllDirs(const std::string &bundleName, std::set<std::string> &expandIncludes,
        std::vector<std::string>& excludes, AncoRestoreResult &ancoRestoreRes) = 0;
};
}  // namespace OHOS::FileManagement::Backup
#endif