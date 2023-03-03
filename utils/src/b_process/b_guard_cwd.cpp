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

#include "b_process/b_guard_cwd.h"
#include "b_error/b_error.h"

#include <cstdlib>
#include <unistd.h>

namespace OHOS::FileManagement::Backup {
using namespace std;

BGuardCwd::BGuardCwd(std::string_view tgtDir)
{
    pwd_ = getcwd(nullptr, 0);
    if (!pwd_) {
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, "Out of memory");
    }
    if (chdir(tgtDir.data())) {
        std::free(pwd_);
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, std::generic_category().message(errno));
    }
}

BGuardCwd::~BGuardCwd()
{
    chdir(pwd_);
    free(pwd_);
}
} // namespace OHOS::FileManagement::Backup
