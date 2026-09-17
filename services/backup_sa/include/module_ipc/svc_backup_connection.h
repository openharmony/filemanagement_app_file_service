/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_SVC_BACKUP_CONNECTION_H
#define OHOS_FILEMGMT_BACKUP_SVC_BACKUP_CONNECTION_H

#include "ability_connect_callback_stub.h"
#include "b_radar/radar_const.h"
#include "iextension.h"
#include "svc_death_recipient.h"

namespace OHOS::FileManagement::Backup {

/**
 * @brief 管理与备份扩展能力（backup extension ability）的单次连接生命周期。
 *
 * 本类为一次性对象：一旦调用过 ConnectBackupExtAbility，实例不可复用。若扩展死亡后需要重连，
 * 调用方应创建新的 SvcBackupConnection（参见 Service::ConnectBackupExtension，它通过
 * SvcSessionManager::ReplaceExtConnection 替换旧连接）。
 *
 * 线程安全：所有公开方法可从不同线程调用。内部使用单一 mutex_ 保护连接状态、回调和远程对象。
 * error_ 字段由独立的共享互斥锁保护，支持通过 GetError() 无锁读取。
 */
class SvcBackupConnection : public AAFwk::AbilityConnectionStub {
public:
    /**
     * @brief 构造一次性备份扩展连接。
     *
     * @param onDiedCb 扩展死亡或连接失败时的回调。
     * @param onConnectedCb 扩展连接成功时的回调。
     * @param bundleNameIndexInfo 包名（可带索引后缀），用于日志和框架回调中的包名校验。
     */
    SvcBackupConnection(std::function<void(const std::string &, bool)> onDiedCb,
                        std::function<void(const std::string &)> onConnectedCb,
                        std::string bundleNameIndexInfo)
        : onDiedCb_(onDiedCb), onConnectedCb_(onConnectedCb), bundleNameIndexInfo_(bundleNameIndexInfo)
    {
    }
    ~SvcBackupConnection() override {};

    /**
     * @brief 框架回调：ConnectAbility 成功时调用。
     *
     * 保存远程代理（iface_cast 为 IExtension 后缓存于 backupProxy_），注册死亡监听，
     * 状态转为 CONNECTED，并调用 on-connected 回调。回调抛出异常时捕获并记录。
     *
     * @param element 已连接的扩展能力信息。
     * @param remoteObject 扩展能力的远程代理对象。
     * @param resultCode 连接结果码（0 = 成功，非 0 = 失败）。
     */
    void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
                              const sptr<IRemoteObject> &remoteObject,
                              int resultCode) override;

    /**
     * @brief 框架回调：扩展断开连接（崩溃、被杀或主动 DisconnectAbility）时调用。
     *
     * 状态转为 DISCONNECTED（此前为 CONNECTED）或 FAILED（从未连接成功），移除死亡监听，
     * 快照并清空 on-died 回调，然后通过 InvokeOnDiedCb 携带框架结果码调用回调。
     * 通知 disconnectCond_，使竞争的 OnExtensionDied 跳过重复处理。
     *
     * @param element 已断开的扩展能力信息。
     * @param resultCode 断开结果码（0 = 成功，非 0 = 失败）。
     */
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;

    /**
     * @brief 通过 Ability Manager 连接备份扩展能力。
     *
     * 一次性保护：若连接已被使用（状态非 IDLE），返回 SA_REFUSED_ACT。
     * 成功时状态转为 CONNECTING，并记录 clean 模式供后续错误上报使用。
     *
     * @param want 描述目标扩展能力的 Want。
     * @param userId 传给 ConnectAbility 的用户 ID。
     * @param isCleanMode 是否为清理数据的重连。
     * @return 成功返回 ERR_OK，否则返回错误码。
     */
    ErrCode ConnectBackupExtAbility(AAFwk::Want &want, int32_t userId, bool isCleanMode);

    /**
     * @brief 断开与扩展能力的连接。
     *
     * 清空 on-died 回调（将断开视为主动行为而非崩溃），调用 DisconnectAbility，
     * 最多等待 WAIT_TIME 秒使状态离开 CONNECTED。
     *
     * @return 成功返回 ERR_OK，否则返回 DisconnectAbility 的错误码。
     */
    ErrCode DisconnectBackupExtAbility();

    /**
     * @brief 检查扩展当前是否处于 CONNECTED 状态。
     *
     * @return CONNECTED 返回 true，否则返回 false。
     */
    bool IsExtAbilityConnected();

    /**
     * @brief 检查扩展是否曾成功连接过（CONNECTED 或 DISCONNECTED）。
     *
     * 调用方据此判断是否需要创建新连接进行重连。
     *
     * @return 曾到达 CONNECTED 返回 true，仍为 IDLE/CONNECTING/FAILED 返回 false。
     */
    bool GetWasEverConnected();

    /**
     * @brief 获取已连接扩展能力的 IExtension 代理。
     *
     * 返回连接时缓存于 backupProxy_ 的代理。若非 CONNECTED 或转换失败则返回 nullptr。
     * 连接时仅做一次 iface_cast，避免每次调用重复转换并确保调用方拿到的代理与连接生命周期一致。
     *
     * @return IExtension 代理，或 nullptr。
     */
    sptr<IExtension> GetBackupExtProxy();

    /**
     * @brief 等待连接离开 CONNECTED 状态（最多 WAIT_TIME 秒）。
     *
     * @return 在超时内状态离开 CONNECTED 返回 true，超时返回 false。
     */
    bool WaitDisconnectDone();

    /**
     * @brief 获取上次连接尝试的耗时（毫秒）。
     */
    uint32_t GetConnectSpan()
    {
        return connectSpend_.GetSpan();
    }

    /**
     * @brief 获取扩展死亡或连接失败时生成的错误。
     *
     * 通过共享互斥锁实现线程安全，可从任意线程调用。
     *
     * @return 内部 BError 的引用，在对象存活期间有效。
     */
    const BError &GetError()
    {
        std::shared_lock<std::shared_mutex> lock(errMutex_);
        return error_;
    }

    /**
     * @brief 设置扩展连接成功时的回调。
     *
     * @param onConnectedCb 回调参数为包名索引信息字符串。
     */
    void SetOnConnectedCb(std::function<void(const std::string &)> onConnectedCb);

    /**
     * @brief 设置扩展死亡或意外断开时的回调。
     *
     * 回调在锁内快照并清空后于锁外调用，保证每次连接最多触发一次。
     *
     * @param onDiedCb 回调参数为包名索引信息和 clean 模式标志。
     */
    void SetOnDiedCb(std::function<void(const std::string &, bool)> onDiedCb);

private:
    /**
     * @brief 死亡监听回调：扩展进程异常死亡时调用。
     *
     * 最多等待 WAIT_DEATH_TIME_MS，优先由 OnAbilityDisconnectDone 处理（它携带框架结果码）。
     * 若超时则快照回调、转换状态，以 errCode 0 直接调用。
     *
     * @param remoteObject 已死亡远程对象的弱引用。
     */
    void OnExtensionDied(const wptr<IRemoteObject> &remoteObject);

    /**
     * @brief 快照 on-died 回调并将状态转为 DISCONNECTED/FAILED。调用前须持有 mutex_。
     *
     * 被 OnAbilityDisconnectDone 和 OnExtensionDied 共用。快照并清空 onDiedCb_，计算
     * wasEverConnected，转换状态，清空 backupProxy_。若从未连接成功则结束计时器，
     * 并根据 errCode/wasEverConnected/isCleanMode 生成错误。可选移除死亡监听（仅
     * OnAbilityDisconnectDone 需要，因为 OnExtensionDied 由死亡监听本身触发）。
     *
     * @param[out] onDiedCb 接收快照后的回调。
     * @param errCode 框架提供的错误码（死亡监听路径为 0）。
     * @param removeDeathRecipient 是否在清空 backupProxy_ 前移除死亡监听。
     */
    void SnapshotOnDiedCbLocked(std::function<void(const std::string &, bool)> &onDiedCb,
                                int errCode,
                                bool removeDeathRecipient);

    /**
     * @brief 锁外公共辅助：调用 on-died 回调（异常安全）。
     *
     * 被 OnAbilityDisconnectDone 和 OnExtensionDied 在锁内快照回调并更新状态后调用。
     * 错误生成与计时器结束已在 SnapshotOnDiedCbLocked 中完成，此处仅打印日志并调用回调，
     * 回调抛出异常时捕获并记录，避免异常逃逸到框架线程。
     *
     * @param onDiedCb 快照后的回调（为空则空操作）。
     */
    void InvokeOnDiedCb(std::function<void(const std::string &, bool)> &onDiedCb);

    /**
     * @brief 根据连接结果生成 BError，供后续 GetError() 获取。
     *
     * 利用连接耗时和超时阈值区分超时（SA_BOOT_EXT_TIMEOUT）和其他死亡原因
     *（EXT_ABILITY_DIED）。内部获取 errMutex_，可持有 mutex_ 时调用（锁序 mutex_ → errMutex_）。
     *
     * @param errCode 框架提供的错误码（死亡监听路径为 0）。
     * @param wasEverConnected 扩展是否曾连接成功。
     * @param isCleanMode 是否为清理数据的重连。
     */
    void GenErrorByStatus(int errCode, bool wasEverConnected, bool isCleanMode);

    /**
     * @brief 在锁内准备死亡监听：移除已有监听并创建新的 deathRecipient_。调用前须持有 mutex_。
     *        不调用 AddDeathRecipient，该 IPC 框架调用须在释放 mutex_ 后由调用方执行，
     *        避免持有 mutex_ 期间进入 binder 框架内部锁/事务。
     */
    void PrepareDeathRecipientLocked();

    /**
     * @brief 从 backupProxy_->AsObject() 移除已注册的死亡监听。若代理或监听为空、或对象已死亡则为空操作。
     *        调用前须持有 mutex_。
     */
    void RemoveDeathRecipientLocked();

    /** 连接状态机（单调递进，从不重置）。 */
    enum class ConnState {
        IDLE,         ///< 初始状态，ConnectBackupExtAbility 之前。
        CONNECTING,   ///< 已调用 ConnectAbility，等待 OnAbilityConnectDone。
        CONNECTED,    ///< OnAbilityConnectDone 成功。
        DISCONNECTED, ///< 曾为 CONNECTED，后断开（崩溃或主动）。
        FAILED,       ///< 从未到达 CONNECTED（连接失败或连接前断开）。
    };

    std::mutex mutex_;                       ///< 保护 connState_、backupProxy_、回调。
    std::condition_variable disconnectCond_; ///< 由 OnAbilityDisconnectDone 通知，OnExtensionDied 等待。
    std::condition_variable connCond_;       ///< 状态变化时通知，Disconnect/WaitDisconnect 等待。
    ConnState connState_ {ConnState::IDLE};  ///< 当前连接状态。
    bool isCleanMode_ {false};               ///< 由 ConnectBackupExtAbility 设置一次，供错误上报使用。
    sptr<IExtension> backupProxy_;          ///< 连接时缓存的 IExtension 代理，非 CONNECTED 时为 nullptr。
    sptr<IRemoteObject::DeathRecipient> deathRecipient_; ///< 注册在 backupProxy_->AsObject() 上的死亡监听。

    std::function<void(const std::string &, bool)> onDiedCb_; ///< 一次性 on-died 回调。
    std::function<void(const std::string &)> onConnectedCb_;  ///< 一次性 on-connected 回调。
    std::string bundleNameIndexInfo_;                         ///< 包名（可带索引），用于日志和校验。
    Duration connectSpend_;                                   ///< 计时：从 Connect 开始到 Connect/Die。
    std::shared_mutex errMutex_;                              ///< error_ 的读写锁。
    BError error_; ///< 最近一次错误，扩展死亡或连接失败时设置。
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SVC_BACKUP_CONNECTION_H
