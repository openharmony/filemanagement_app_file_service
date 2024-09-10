/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "b_ohos/startup/backup_para.h"
#include "backup_para_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

bool BackupPara::GetBackupDebugOverrideExtensionConfig()
{
    return BBackupPara::backupPara->GetBackupDebugOverrideExtensionConfig();
}

bool BackupPara::GetBackupOverrideBackupSARelease()
{
    return BBackupPara::backupPara->GetBackupOverrideBackupSARelease();
}

bool BackupPara::GetBackupOverrideIncrementalRestore()
{
    return BBackupPara::backupPara->GetBackupOverrideIncrementalRestore();
}

tuple<bool, int32_t> BackupPara::GetBackupDebugOverrideAccount()
{
    return BBackupPara::backupPara->GetBackupDebugOverrideAccount();
}
} // namespace OHOS::FileManagement::Backup