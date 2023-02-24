/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "b_error/b_error.h"

#include <sstream>
#include <sys/syscall.h>

#include "dfx_dump_catcher.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

string BError::WrapMessageWithExtraInfos(const char *fileName,
                                         int lineNo,
                                         const char *functionName,
                                         Codes code,
                                         const vector<string_view> &msgs) const
{
    stringstream ss;
    ss << '[' << fileName << ':' << lineNo << " -> " << functionName << ']' << ' ';
    for (size_t i = 0; i < msgs.size(); ++i) {
        ss << msgs[i];
        if (i != msgs.size() - 1) {
            ss << ". ";
        }
    }

    if (code != Codes::OK) {
        string msg;
        HiviewDFX::DfxDumpCatcher().DumpCatch(getpid(), syscall(SYS_gettid), msg);
        ss << endl << msg;
    }

    string res = ss.str();
    HiviewDFX::HiLog::Error(FILEMGMT_LOG_LABEL, "%{public}s", res.c_str());
    return res;
}

ErrCode BError::ExceptionCatcherLocked(std::function<ErrCode(void)> callBack)
{
    try {
        return callBack();
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGE("Unexpected exception");
        return EPERM;
    }
}
} // namespace OHOS::FileManagement::Backup