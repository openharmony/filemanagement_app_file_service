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

#ifndef OHOS_FILEMGMT_BACKUP_B_GUARD_CWD_H
#define OHOS_FILEMGMT_BACKUP_B_GUARD_CWD_H

/**
 * @file b_cwd_guard.h
 * @brief 异常安全的临时变更目录方法
 *
 */

#include <string_view>

#include "nocopyable.h"

namespace OHOS::FileManagement::Backup {
class BGuardCwd final : protected NoCopyable {
public:
    /**
     * @brief 构造器，其中会把当前目录变更为目标目录
     *
     * @param tgtDir 目标目录
     */
    explicit BGuardCwd(std::string_view tgtDir);

    /**
     * @brief 析构器，其中会把目标目录恢复为当前目录
     *
     */
    ~BGuardCwd() final;

private:
    BGuardCwd() = delete;
    char *pwd_ {nullptr};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_GUARD_CWD_H