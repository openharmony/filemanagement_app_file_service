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

#ifndef OHOS_FILEMGMT_BACKUP_B_PROCESS_H
#define OHOS_FILEMGMT_BACKUP_B_PROCESS_H

#include <functional>
#include <string_view>
#include <tuple>
#include <vector>

#include "errors.h"
#include "nocopyable.h"

namespace OHOS::FileManagement::Backup {
class BProcess final : protected NoCopyable {
public:
    /**
     * @brief 执行一个命令并同步等待执行结果
     *
     * @param argv 命令参数表
     * Parameter one is the command name represented by the absolute path.
     * After that, parameter indicates the corresponding command parameter.
     * Finally,nullptr does not need to be appended.
     *
     * @param DetectFatalLog 回调函数，用来对命令的stderr输出错误信息做进一步处理，检测是否发生了严重错误等。
     *
     * @return 回调函数返回值，命令执行结果
     *
     * @throw BError(UTILS_INVAL_PROCESS_ARG) 系统调用异常(子进程启动失败、waitpid调用失败)
     *
     * @throw BError(UTILS_INTERRUPTED_PROCESS) 系统调用异常(pipe调用失败、dup2调用失败、子进程被信号终止)
     */
    static std::tuple<bool, ErrCode> ExecuteCmd(std::vector<std::string_view> argv,
                                                std::function<bool(std::string_view)> DetectFatalLog = nullptr);

private:
    BProcess() = delete;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_PROCESS_H