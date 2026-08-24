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

#ifndef OHOS_FILEMGMT_BACKUP_EXT_CLONE_FILE_INFO_BACKUP_RDBSTORE_H
#define OHOS_FILEMGMT_BACKUP_EXT_CLONE_FILE_INFO_BACKUP_RDBSTORE_H

#include "b_anony/b_anony.h"
#include "filemgmt_libhilog.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"
#include "rdb_sql_utils.h"
#include "rdb_store.h"
#include "rdb_store_config.h"
#include "rdb_types.h"

#include <mutex>
#include <string>

namespace OHOS::FileManagement::Backup {
class CloneFileInfoBackupRdbstore {
public:
    ~CloneFileInfoBackupRdbstore() = default;
    static CloneFileInfoBackupRdbstore* GetInstance(const std::string &dbPath);
    std::vector<std::string> QueryAncoMediaFile();

private:
    CloneFileInfoBackupRdbstore(const std::string &dbPath);
    int32_t TryGetRdbStore(const std::string &dbPath);
    std::shared_ptr<NativeRdb::RdbStore> rdbStore_ {nullptr};
    std::mutex mutex_;
};

class CloneFileInfoBackupCallBack : public NativeRdb::RdbOpenCallback {
public:
    int32_t OnCreate(NativeRdb::RdbStore& rdbStore) override;
    int32_t OnUpgrade(NativeRdb::RdbStore& rdbStore, int32_t oldVersion, int32_t newVersion) override;
};
} // namespace OHOS::FileManagement::Backup

#endif