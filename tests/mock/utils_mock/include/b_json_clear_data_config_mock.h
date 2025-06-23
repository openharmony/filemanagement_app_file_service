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

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_CLEAR_DATA_CONFIG_MOCK_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_CLEAR_DATA_CONFIG_MOCK_H

#include <gmock/gmock.h>
#include "b_json/b_json_clear_data_config.h"

namespace OHOS::FileManagement::Backup {
class BBJsonClearDataConfig {
public:
    virtual bool HasClearBundleRecord() = 0;
    virtual bool FindClearBundleRecord(const std::string&) = 0;
    virtual bool InsertClearBundleRecord(const std::string&) = 0;
    virtual bool DeleteClearBundleRecord(const std::string&) = 0;
    virtual std::vector<std::string> GetAllClearBundleRecords() = 0;
    virtual bool DeleteConfigFile() = 0;
public:
    BBJsonClearDataConfig() = default;
    virtual ~BBJsonClearDataConfig() = default;
public:
    static inline std::shared_ptr<BBJsonClearDataConfig> config = nullptr;
};

class BJsonClearDataConfigMock : public BBJsonClearDataConfig {
public:
    MOCK_METHOD(bool, HasClearBundleRecord, ());
    MOCK_METHOD(bool, FindClearBundleRecord, (const std::string&));
    MOCK_METHOD(bool, InsertClearBundleRecord, (const std::string&));
    MOCK_METHOD(bool, DeleteClearBundleRecord, (const std::string&));
    MOCK_METHOD((std::vector<std::string>), GetAllClearBundleRecords, ());
    MOCK_METHOD(bool, DeleteConfigFile, ());
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_B_JSON_CLEAR_DATA_CONFIG_MOCK_H
