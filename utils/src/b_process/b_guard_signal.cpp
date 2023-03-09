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

#include "b_process/b_guard_signal.h"

#include <sstream>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
BGuardSignal::BGuardSignal(int sig) : sig_(sig)
{
    prevHandler_ = signal(sig, SIG_DFL);
    if (prevHandler_ == SIG_ERR) {
        stringstream ss;
        ss << "Invalid sigal " << sig << ", received error " << system_category().message(errno);
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, ss.str());
    }
}

BGuardSignal::~BGuardSignal()
{
    if (signal(sig_, prevHandler_) == SIG_ERR) {
        HILOGE("Failed to reset sig %{public}d", sig_);
    }
}
} // namespace OHOS::FileManagement::Backup