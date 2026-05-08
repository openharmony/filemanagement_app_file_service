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

#include "clone_file_info_backup_rdbstore.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

CloneFileInfoBackupRdbstore* CloneFileInfoBackupRdbstore::GetInstance(const std::string &dbPath)
{
    static CloneFileInfoBackupRdbstore instance(dbPath);
    return &instance;
}

CloneFileInfoBackupRdbstore::CloneFileInfoBackupRdbstore(const std::string &dbPath)
{
    HILOGI("Init rdb");
    if (rdbStore_ != nullptr) {
        HILOGI("RdbStore_ already exists");
        return;
    }
    if (dbPath == "" || TryGetRdbStore(dbPath) != NativeRdb::E_OK) {
        HILOGE("Failed to init CloneFileInfoRestore rdb");
        return;
    }
    HILOGI("Success to init CloneFileInfoRestore rdb");
}

int32_t CloneFileInfoBackupRdbstore::TryGetRdbStore(const std::string &dbPath)
{
    std::unique_lock<std::mutex> lock(mutex_);

    NativeRdb::RdbStoreConfig config(dbPath);
    int errCode = 0;
    CloneFileInfoBackupCallBack cloneFileInfoBackupCallBack;
    rdbStore_ = NativeRdb::RdbHelper::GetRdbStore(config, 1, cloneFileInfoBackupCallBack, errCode);
    if (rdbStore_ == nullptr || errCode != NativeRdb::E_OK) {
        HILOGE("Get RdbStore failed, errCode: %{public}d", errCode);
        rdbStore_ = nullptr;
        return errCode;
    }
    return NativeRdb::E_OK;
}

std::vector<std::string> CloneFileInfoBackupRdbstore::QueryAncoMediaFile()
{
    HILOGI("QueryAncoMediaFile");
    NativeRdb::RdbPredicates predicates("anco_file_info");
    std::string path("path");
    vector<string> columns = {path};
    vector<std::string> retPaths;
    if (rdbStore_ == nullptr) {
        HILOGE("rdbStore_ nullptr");
        return retPaths;
    }
    auto resultSet = rdbStore_->Query(predicates, columns);
    if (resultSet == nullptr) {
        HILOGE("Query resultSet nullptr");
        return retPaths;
    }
    while (resultSet->GoToNextRow() == NativeRdb::E_OK) {
        int columnIndex = 0;
        string ancoMediaFilePath;
        if (resultSet->GetColumnIndex("path", columnIndex) != NativeRdb::E_OK ||
            resultSet->GetString(columnIndex, ancoMediaFilePath) != NativeRdb::E_OK) {
            HILOGE("Fail to get path");
            resultSet->Close();
            return retPaths;
        }
        HILOGI("AddAncoFile ancoMediaFilePath %{public}s", GetAnonyString(ancoMediaFilePath).c_str());
        retPaths.push_back(ancoMediaFilePath);
    }
    resultSet->Close();
    HILOGI("QueryAncoMediaFile end");
    return retPaths;
}

int32_t CloneFileInfoBackupCallBack::OnCreate(NativeRdb::RdbStore& rdbStore)
{
    return 0;
}

int32_t CloneFileInfoBackupCallBack::OnUpgrade(NativeRdb::RdbStore& rdbStore, int32_t oldVersion, int32_t newVersion)
{
    return 0;
}
} // namespace OHOS::FileManagement::Backup
