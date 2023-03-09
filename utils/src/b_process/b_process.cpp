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

#include "b_process/b_process.h"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <memory>
#include <regex>
#include <string_view>
#include <sys/wait.h>
#include <tuple>
#include <unistd.h>

#include "b_error/b_error.h"
#include "b_process/b_guard_signal.h"
#include "errors.h"
#include "filemgmt_libhilog.h"
#include "securec.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static tuple<bool, ErrCode> WaitForChild(pid_t pid,
                                         unique_ptr<FILE, function<void(FILE *)>> pipeStream,
                                         function<bool(string_view)> DetectFatalLog)
{
    const int BUF_LEN = 1024;
    auto buf = make_unique<char[]>(BUF_LEN);
    int status = 0;

    do {
        regex reg("^\\W*$");
        while ((void)memset_s(buf.get(), BUF_LEN, 0, BUF_LEN),
               fgets(buf.get(), BUF_LEN - 1, pipeStream.get()) != nullptr) {
            if (regex_match(buf.get(), reg)) {
                continue;
            }
            HILOGE("child process output error: %{public}s", buf.get());
            if (DetectFatalLog && DetectFatalLog(string_view(buf.get()))) {
                return {true, EPERM};
            }
        }

        if (waitpid(pid, &status, 0) == -1) {
            throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, generic_category().message(errno));
        } else if (WIFEXITED(status)) {
            return {false, WEXITSTATUS(status)};
        } else if (WIFSIGNALED(status)) {
            // 因某种信号中断获取状态
            // 异常机制存在问题，导致应用在正常的错误下Crash。为确保测试顺利展开，此处暂时屏蔽崩溃错误。
            HILOGE("some fatal errors occurred, child process is killed by a signal.");
            return {true, EPERM};
        }
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    HILOGE("If you look at me, there are some fatal errors occurred!!!");
    return {true, EPERM};
}

tuple<bool, ErrCode> BProcess::ExecuteCmd(vector<string_view> argvSv, function<bool(string_view)> DetectFatalLog)
{
    vector<const char *> argv;
    auto getStringViewData = [](const auto &arg) { return arg.data(); };
    transform(argvSv.begin(), argvSv.end(), back_inserter(argv), getStringViewData);
    argv.push_back(nullptr);

    // 临时将SIGCHLD恢复成默认值，从而能够从作为僵尸进程的子进程中获得返回值
    BGuardSignal guard(SIGCHLD);

    int pipeFd[2];
    if (pipe(pipeFd) < 0) {
        throw BError(BError::Codes::UTILS_INTERRUPTED_PROCESS, generic_category().message(errno));
    }

    pid_t pid = 0;
    if ((pid = fork()) == 0) {
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(pipeFd[0]);
        UniqueFd fd(pipeFd[1]);
        if (dup2(pipeFd[1], STDERR_FILENO) == -1) {
            throw BError(BError::Codes::UTILS_INTERRUPTED_PROCESS, generic_category().message(errno));
        }
        exit((execvp(argv[0], const_cast<char **>(argv.data())) == -1) ? errno : 0);
    }

    UniqueFd fd(pipeFd[0]);
    close(pipeFd[1]);
    if (pid == -1) {
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, generic_category().message(errno));
    }
    unique_ptr<FILE, function<void(FILE *)>> pipeStream {fdopen(pipeFd[0], "r"), fclose};
    if (!pipeStream) {
        throw BError(errno);
    }
    fd.Release();

    return WaitForChild(pid, move(pipeStream), DetectFatalLog);
}
} // namespace OHOS::FileManagement::Backup