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

#ifndef OHOS_FILEMGMT_BACKUP_B_INCREMENTAL_SESSION_RESTORE_ASYNC_H
#define OHOS_FILEMGMT_BACKUP_B_INCREMENTAL_SESSION_RESTORE_ASYNC_H

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "b_file_info.h"
#include "errors.h"
#include "i_service.h"
#include "svc_death_recipient.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BIncrementalSessionRestoreAsync : public std::enable_shared_from_this<BIncrementalSessionRestoreAsync> {
public:
    struct Callbacks {
        std::function<void(const BFileInfo &, UniqueFd, UniqueFd, ErrCode)> onFileReady; // 当备份服务有文件待发送时执行的回调
        std::function<void(ErrCode, const BundleName)> onBundleStarted; // 当启动某个应用的恢复流程结束时执行的回调函数
        std::function<void(ErrCode, const BundleName)>
            onBundleFinished; // 当某个应用的恢复流程结束或意外中止时执行的回调函数
        std::function<void(ErrCode)> onAllBundlesFinished; // 当整个恢复流程结束或意外中止时执行的回调函数
        std::function<void(const std::string)> onResultReport; // 某个应用恢复流程中自定义错误信息的上报的回调函数
        std::function<void()> onBackupServiceDied;         // 当备份服务意外死亡时执行的回调函数
    };

    struct AppendBundleInfo {
        UniqueFd remoteCap;                       // 已打开的保存远端设备能力的Json文件
        std::vector<BundleName> bundlesToRestore; // 需要恢复的应用名称列表
        RestoreTypeEnum restoreType; // 待恢复类型(例如升级服务数据迁移就绪无需进行数据传输)
        int32_t userId;              // 用户ID
    };

public:
    /**
     * @brief 获取一个用于控制恢复流程的会话
     *
     * @param callbacks 注册的回调函数
     * @return std::unique_ptr<BRestoreSession> 指向BRestoreSession的智能指针。失败时为空指针
     */
    static std::shared_ptr<BIncrementalSessionRestoreAsync> Init(Callbacks callbacks);

    /**
     * @brief 通知备份服务文件内容已就绪
     *
     * @param fileInfo 文件描述信息
     * @return ErrCode 规范错误码
     * @see GetFileHandle
     */
    ErrCode PublishFile(BFileInfo fileInfo);

    /**
     * @brief 请求恢复流程所需的真实文件
     *
     * @param bundleName 应用名称
     * @param fileName   文件名称
     */
    ErrCode GetFileHandle(const std::string &bundleName, const std::string &fileName);

    /**
     * @brief 用于追加待恢复应用
     *
     * @param remoteCap 已打开的保存远端设备能力的Json文件。可使用GetLocalCapabilities方法获取
     * @param bundlesToRestore 待恢复的应用清单
     * @param userId 用户ID
     * @return ErrCode 规范错误码
    */
    ErrCode AppendBundles(UniqueFd remoteCap, std::vector<BundleName> bundlesToRestore,
        std::vector<std::string> detailInfos, RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND,
        int32_t userId = DEFAULT_INVAL_VALUE);

    /**
     * @brief 用于追加待恢复应用
     *
     * @param remoteCap 已打开的保存远端设备能力的Json文件。可使用GetLocalCapabilities方法获取
     * @param bundlesToRestore 待恢复的应用清单
     * @param userId 用户ID
     * @return ErrCode 规范错误码
     */
    ErrCode AppendBundles(UniqueFd remoteCap,
                          std::vector<BundleName> bundlesToRestore,
                          RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND,
                          int32_t userId = DEFAULT_INVAL_VALUE);

    /**
     * @brief 用于结束服务
     *
     * @return ErrCode 规范错误码
     */
    ErrCode Release();

public:
    explicit BIncrementalSessionRestoreAsync(Callbacks callbacks) : callbacks_(callbacks) {};
    ~BIncrementalSessionRestoreAsync();

private:
    /** @brief 注册备份服务意外死亡时执行的回调函数 */
    void OnBackupServiceDied();

    /**
     * @brief 注册备份服务意外死亡时执行的回调函数
     *
     * @param functor 回调函数
     */
    void RegisterBackupServiceDied(std::function<void()> functor);

private:
    sptr<SvcDeathRecipient> deathRecipient_;
    Callbacks callbacks_;
    std::atomic<bool> isAppend_ {false};
    std::mutex mutex_;
    std::queue<AppendBundleInfo> workList_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_INCREMENTAL_SESSION_RESTORE_ASYNC_H