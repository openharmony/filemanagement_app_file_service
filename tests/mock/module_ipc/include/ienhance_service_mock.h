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
 
#ifndef OHOS_FILEMGMT_BACKUP_MOCK_IENHANCE_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_MOCK_IENHANCE_SERVICE_H
 
#include <gmock/gmock.h>
#include "ienhance_service.h"
 
namespace OHOS::FileManagement::Backup {
class MockIEnhanceService : public IEnhanceService {
public:
    MOCK_METHOD(ErrCode, CreateAncoBackupTask, (const std::string &, const sptr<IAncoBackupCallback> &), (override));
    MOCK_METHOD(ErrCode, DestroyAncoBackupTask, (const std::string &), (override));
    MOCK_METHOD(ErrCode, FilterAndSaveBackupPaths, (const std::string &, std::set<std::string> &,
        std::set<std::string> &, const std::vector<std::string> &), (override));
    MOCK_METHOD(ErrCode, StartAncoScanAllDirs, (const std::string &, AncoScanResult &), (override));
    MOCK_METHOD(ErrCode, StartAncoPacket, (const std::string &, uint64_t &), (override));
    MOCK_METHOD(ErrCode, CreateAncoRestoreTask, (const std::string &), (override));
    MOCK_METHOD(ErrCode, DestroyAncoRestoreTask, (const std::string &), (override));
    MOCK_METHOD(ErrCode, StartAncoUnPacket, (const std::string &, const std::vector<std::string> &,
        const std::vector<int64_t> &, const std::vector<std::string> &, const std::string &), (override));
    MOCK_METHOD(ErrCode, StartAncoMove, (const std::string &, const std::vector<std::string> &,
        const std::vector<std::string> &, const std::vector<StatInfo> &, AncoRestoreResult &), (override));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_MOCK_IENHANCE_SERVICE_H