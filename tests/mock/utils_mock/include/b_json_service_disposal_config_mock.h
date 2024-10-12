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

#ifndef OHOS_FILEMGMT_BACKUP_B_B_JSON_SERVICE_DISPOSAL_CONFIG_MOCK_H
#define OHOS_FILEMGMT_BACKUP_B_B_JSON_SERVICE_DISPOSAL_CONFIG_MOCK_H

#include <gmock/gmock.h>

#include "b_json/b_json_service_disposal_config.h"

namespace OHOS::FileManagement::Backup {
class BBJsonDisposalConfig {
public:
    virtual bool IfBundleNameInDisposalConfigFile(const std::string&) = 0;
    virtual bool AppendIntoDisposalConfigFile(const std::string&) = 0;
    virtual bool DeleteFromDisposalConfigFile(const std::string&) = 0;
    virtual std::vector<std::string> GetBundleNameFromConfigFile() = 0;
    virtual bool DeleteConfigFile();
public:
    BBJsonDisposalConfig() = default;
    virtual ~BBJsonDisposalConfig() = default;
public:
    static inline std::shared_ptr<BBJsonDisposalConfig> config = nullptr;
};

class BJsonDisposalConfigMock : public BBJsonDisposalConfig {
public:
    MOCK_METHOD(bool, IfBundleNameInDisposalConfigFile, (const std::string&));
    MOCK_METHOD(bool, AppendIntoDisposalConfigFile, (const std::string&));
    MOCK_METHOD(bool, DeleteFromDisposalConfigFile, (const std::string&));
    MOCK_METHOD((std::vector<std::string>), GetBundleNameFromConfigFile, ());
    MOCK_METHOD(bool, DeleteConfigFile, ());
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_B_B_JSON_SERVICE_DISPOSAL_CONFIG_MOCK_H
