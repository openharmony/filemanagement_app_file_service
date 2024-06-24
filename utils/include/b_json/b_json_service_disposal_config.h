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

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_SERVICE_DISPOSAL_CONFIG_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_SERVICE_DISPOSAL_CONFIG_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include  "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
class BJsonDisposalConfig {
public:
    /**
     * @brief 判断配置文件中是否存在bundlename
     *
     * @param bundlename
     */
    bool IfBundleNameInDisposalConfigFile(const std::string& bundleName);
    
    /**
     * @brief 配置文件中写入bundlename
     *
     * @param bundlename
     */
    bool AppendIntoDisposalConfigFile(const std::string& bundleName);

    /**
     * @brief 配置文件中删除bundlename
     *
     * @param bundlename
     */
    bool DeleteFromDisposalConfigFile(const std::string& bundleName);

    /**
     * @brief 配置文件中获取bundlename
     *
     *
     */
    std::vector<std::string> GetBundleNameFromConfigFile();

    /**
     * @brief 删除配置文件
     *
     *
     */
    bool DeleteConfigFile();
public:
    /**
     * @brief 构造方法
     *
     *
     */
    BJsonDisposalConfig();

private:
    std::mutex fileMutex_;
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_B_JSON_SERVICE_DISPOSAL_CONFIG_H