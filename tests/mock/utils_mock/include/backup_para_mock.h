/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_PARA_MOCK_H
#define OHOS_FILEMGMT_BACKUP_PARA_MOCK_H

#include <gmock/gmock.h>

namespace OHOS::FileManagement::Backup {
class BBackupPara {
public:
    virtual bool GetBackupDebugOverrideExtensionConfig() = 0;
    virtual bool GetBackupOverrideBackupSARelease() = 0;
    virtual bool GetBackupOverrideIncrementalRestore() = 0;
    virtual std::tuple<bool, int32_t> GetBackupDebugOverrideAccount() = 0;
public:
    BBackupPara() = default;
    virtual ~BBackupPara() = default;
public:
    static inline std::shared_ptr<BBackupPara> backupPara = nullptr;
};

class BackupParaMock : public BBackupPara {
public:
    MOCK_METHOD(bool, GetBackupDebugOverrideExtensionConfig, ());
    MOCK_METHOD(bool, GetBackupOverrideBackupSARelease, ());
    MOCK_METHOD(bool, GetBackupOverrideIncrementalRestore, ());
    MOCK_METHOD((std::tuple<bool, int32_t>), GetBackupDebugOverrideAccount, ());
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_PARA_MOCK_H