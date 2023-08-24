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

#ifndef OHOS_FILEMGMT_BACKUP_BACKUP_PARA_H
#define OHOS_FILEMGMT_BACKUP_BACKUP_PARA_H

#include <tuple>

namespace OHOS::FileManagement::Backup {
class BackupPara {
public:
    /**
     * @brief 获取backup.para配置项backup.debug.overrideExtensionConfig的值
     *
     * @return 获取的配置项backup.debug.overrideExtensionConfig值为true时则返回true，否则返回false
     */
    bool GetBackupDebugOverrideExtensionConfig();

    /**
     * @brief 获取backup.para配置项backup.debug.overrideAccountConfig
     *
     * @return bool值为配置项backup.debug.overrideAccountConfig值
     * @return int32_t值为配置项backup.debug.overrideAccountNumber值
     */
    std::tuple<bool, int32_t> GetBackupDebugOverrideAccount();
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_BACKUP_PARA_H