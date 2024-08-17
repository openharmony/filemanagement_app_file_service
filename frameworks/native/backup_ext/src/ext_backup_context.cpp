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

#include "ext_backup_context.h"

#include "b_filesystem/b_file.h"
#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
const std::string ExtBackupContext::GetBackupDir()
{
    int area = GetArea();
    if (area > 1) { // 1 : el2 area
        return "";
    }

    std::string path = std::string(BConstants::BACKUP_DIR_PRE) + std::string(BConstants::CONTEXT_ELS[area]) +
        std::string(BConstants::BACKUP_DIR_END);
    std::string bundleName = GetBundleName();
    if (BFile::EndsWith(bundleName, BConstants::BUNDLE_FILE_MANAGER) && bundleName.size() == BConstants::FM_LEN) {
        path = std::string(BConstants::PATH_FILEMANAGE_BACKUP_HOME) + BConstants::FILE_SEPARATOR_CHAR;
    } else if (bundleName == BConstants::BUNDLE_MEDIAL_DATA) {
        path = std::string(BConstants::PATH_MEDIALDATA_BACKUP_HOME) + BConstants::FILE_SEPARATOR_CHAR;
    }

    return path;
}
} // namespace OHOS::FileManagement::Backup