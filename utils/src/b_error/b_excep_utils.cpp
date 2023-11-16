/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "b_error/b_excep_utils.h"

#include <string_view>
#include <cstdlib>
#include <climits>

#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void BExcepUltils::VerifyPath(const string_view &path, bool isExtension)
{
    string absPath = BExcepUltils::Canonicalize(path);
    if (isExtension &&
        absPath.find(string(BConstants::PATH_BUNDLE_BACKUP_HOME)
        .append(BConstants::SA_BUNDLE_BACKUP_RESTORE)) == string::npos) {
        throw BError(BError::Codes::EXT_INVAL_ARG, "Invalid path, not in backup restore path");
    }
}

string BExcepUltils::Canonicalize(const string_view &path)
{
    char full_path[PATH_MAX] = {0};
    //1.转换绝对路径到dir
#ifdef _WIN32
    _fullpath(full_path, path.data(), PATH_MAX);
#else
    realpath(path.data(), full_path);
#endif
    return string(full_path);
}
} // namespace OHOS::FileManagement::Backup
