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

#ifndef OHOS_FILEMGMT_BACKUP_B_INCREMENTAL_BACKUP_BACKUP_H
#define OHOS_FILEMGMT_BACKUP_B_INCREMENTAL_BACKUP_BACKUP_H

#include <functional>
#include <memory>
#include <vector>

#include "b_file_info.h"
#include "b_incremental_data.h"
#include "errors.h"
#include "svc_death_recipient.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BIncrementalBackupSession {
public:
    struct Callbacks {
        std::function<void(const BFileInfo &, UniqueFd, UniqueFd, ErrCode)> onFileReady; // 当备份服务有文件待发送时执行的回调
        std::function<void(ErrCode, const BundleName)> onBundleStarted; // 当启动某个应用的备份流程结束时执行的回调函数
        std::function<void(ErrCode, const BundleName)>
            onBundleFinished; // 当某个应用的备份流程结束或意外中止时执行的回调函数
        std::function<void(ErrCode)> onAllBundlesFinished; // 当整个备份流程结束或意外中止时执行的回调函数
        std::function<void(const std::string)> onResultReport; // 某个应用备份流程中自定义错误信息的上报的回调函数
        std::function<void()> onBackupServiceDied;         // 当备份服务意外死亡时执行的回调函数
    };

public:
    /**
     * @brief 获取一个用于控制备份流程的会话
     *
     * @param callbacks 注册回调
     * @return std::unique_ptr<BIncrementalBackupSession> 指向会话的智能指针。失败时为空指针
     */
    static std::unique_ptr<BIncrementalBackupSession> Init(Callbacks callbacks);

    /**
     * @brief 用于追加应用，现阶段仅支持在Start之前调用
     *
     * @param bundlesToBackup 待备份的应用清单
     * @return ErrCode 规范错误码
     */
    ErrCode AppendBundles(std::vector<BIncrementalData> bundlesToBackup);

    /**
     * @brief 用于结束服务
     *
     * @return ErrCode 规范错误码
     */
    ErrCode Release();

    /**
     * @brief 注册备份服务意外死亡时执行的回调函数
     *
     * @param functor 回调函数
     */
    void RegisterBackupServiceDied(std::function<void()> functor);

public:
    ~BIncrementalBackupSession();

private:
    sptr<SvcDeathRecipient> deathRecipient_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_INCREMENTAL_BACKUP_BACKUP_H