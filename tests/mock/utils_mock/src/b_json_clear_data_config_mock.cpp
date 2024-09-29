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

#include "b_json_clear_data_config_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BJsonClearDataConfig::BJsonClearDataConfig() {}

bool BJsonClearDataConfig::HasClearBundleRecord()
{
    return BBJsonClearDataConfig::config->HasClearBundleRecord();
}

bool BJsonClearDataConfig::FindClearBundleRecord(const string& bundleName)
{
    return BBJsonClearDataConfig::config->FindClearBundleRecord(bundleName);
}

bool BJsonClearDataConfig::InsertClearBundleRecord(const string& bundleName)
{
    return BBJsonClearDataConfig::config->InsertClearBundleRecord(bundleName);
}

bool BJsonClearDataConfig::DeleteClearBundleRecord(const string& bundleName)
{
    return BBJsonClearDataConfig::config->DeleteClearBundleRecord(bundleName);
}

vector<string> BJsonClearDataConfig::GetAllClearBundleRecords()
{
    return BBJsonClearDataConfig::config->GetAllClearBundleRecords();
}

bool BJsonClearDataConfig::DeleteConfigFile()
{
    return BBJsonClearDataConfig::config->DeleteConfigFile();
}
} // namespace OHOS::FileManagement::Backup