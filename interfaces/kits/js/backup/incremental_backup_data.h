/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#ifndef INTERFACES_KITS_JS_SRC_MOD_BACKUP_INCREMENTAL_BACKUP_DATA_H
#define INTERFACES_KITS_JS_SRC_MOD_BACKUP_INCREMENTAL_BACKUP_DATA_H

#include <memory>
#include <node_api.h>
#include <string>
#include <tuple>

#include "filemgmt_libn.h"

namespace OHOS::FileManagement::Backup {
struct IncrementalBackupTime {
    explicit IncrementalBackupTime(const LibN::NVal &data)
    {
        LibN::NVal name = data.GetProp("bundleName");
        if (name.val_ != nullptr) {
            auto [succ, str, ignore] = name.ToUTF8String();
            if (succ) {
                bundleName = std::string(str.get());
            }
        }

        LibN::NVal time = data.GetProp("lastIncrementalTime");
        if (time.val_ != nullptr) {
            auto [succ, tm] = time.ToInt64();
            if (succ) {
                lastIncrementalTime = tm;
            }
        }
    }
    std::string bundleName = "";
    int64_t lastIncrementalTime = 0;
};

struct FileManifestData {
    explicit FileManifestData(const LibN::NVal &data)
    {
        LibN::NVal fd = data.GetProp("manifestFd");
        if (fd.val_ != nullptr) {
            auto [succ, tmp] = fd.ToInt32();
            if (succ) {
                manifestFd = tmp;
            }
        }
    }
    int32_t manifestFd = -1;
};

struct BackupParams {
    explicit BackupParams(const LibN::NVal &data)
    {
        LibN::NVal para = data.GetProp("parameters");
        if (para.val_ != nullptr) {
            auto [succ, str, ignore] = para.ToUTF8String();
            if (succ) {
                parameters = std::string(str.get());
            }
        }
    }
    std::string parameters = "";
};

struct BackupPriority {
    explicit BackupPriority(const LibN::NVal &data)
    {
        LibN::NVal pr = data.GetProp("priority");
        if (pr.val_ != nullptr) {
            auto [succ, tmp] = pr.ToInt32();
            if (succ) {
                priority = tmp;
            }
        }
    }
    int32_t priority = -1;
};

struct IncrementalBackupData
    : public IncrementalBackupTime, public FileManifestData, public BackupParams, public BackupPriority {
    explicit IncrementalBackupData(const LibN::NVal &data)
        : IncrementalBackupTime(data),
          FileManifestData(data),
          BackupParams(data),
          BackupPriority(data) {};
};
} // namespace OHOS::FileManagement::Backup
#endif // INTERFACES_KITS_JS_SRC_MOD_BACKUP_INCREMENTAL_BACKUP_DATA_H