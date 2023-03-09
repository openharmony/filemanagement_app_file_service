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

#ifndef OHOS_FILEMGMT_BACKUP_B_GUARD_SIGNAL_H
#define OHOS_FILEMGMT_BACKUP_B_GUARD_SIGNAL_H

/**
 * @file b_guard_signal.h
 * @brief 异常安全的临时调整信号处理程序方法
 *
 */

#include <csignal>

#include "nocopyable.h"

namespace OHOS::FileManagement::Backup {
class BGuardSignal final : protected NoCopyable {
public:
    /**
     * @brief 构造器，其中会把给定信号的处理程序重置为默认值
     *
     * @param sig 给定信号
     */
    explicit BGuardSignal(int sig);

    /**
     * @brief 析构器，其中会还原给定信号的处理程序
     *
     */
    ~BGuardSignal() final;

private:
    BGuardSignal() = delete;
    sighandler_t prevHandler_ {nullptr};
    int sig_ {-1};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_GUARD_SIGNAL_H