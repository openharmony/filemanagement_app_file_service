/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "b_json/b_json_entity_onbackupex_ret.h"

#include <any>
#include <fcntl.h>
#include <string_view>
#include <sys/types.h>
#include <unistd.h>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

unordered_set<string> BJsonEntityOnBackupExRet::GetCompatibleDirs() const
{
    if (!HasArray(BConstants::COMPATIBLE_DIR_MAPPING)) {
        return {};
    }

    unordered_set<string> dirs;
    for (auto &&item : obj_[BConstants::COMPATIBLE_DIR_MAPPING]) {
        if (!item.isString()) {
            HILOGE("Each item of array 'compatibleDirMapping' must be of the type string");
            continue;
        }
        auto dir = item.asString();
        if (dir.empty()) {
            HILOGE("Item is empty string");
            continue;
        }
        dirs.emplace(dir);
        if (dirs.size() > BConstants::MAX_COMPAT_DIR_COUNT) {
            HILOGE("mapping dirs count is over max size!");
            return {};
        }
    }
    return dirs;
}
} // namespace OHOS::FileManagement::Backup
