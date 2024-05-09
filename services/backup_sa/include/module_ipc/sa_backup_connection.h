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

#ifndef OHOS_SA_BACKUP_CONNECTION_H
#define OHOS_SA_BACKUP_CONNECTION_H

#include "if_local_ability_manager.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_load_callback_stub.h"
#include <memory>
#include "unique_fd.h"
#include "thread_pool.h"
#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
class SABackupConnection : public SystemAbilityExtensionPara {
public:
    bool InputParaSet(MessageParcel& data) override;
    bool OutputParaGet(MessageParcel& reply) override;

    /**
     * @brief connect remote BackupSAExt.
     *
     * @return ErrCode OK if connect, otherwise connect failed.
     */
    ErrCode ConnectBackupSAExt(std::string bundleName, std::string extension, std::string extInfo);

    /**
     * @brief disconnect remote BackupSAExt.
     *
     * @return ErrCode OK if disconnect, otherwise disconnect failed.
     */
    ErrCode DisconnectBackupSAExt();

    /**
     * @brief check whether connected to remote sa extension.
     *
     * @return bool true if connected, otherwise false.
     */
    bool IsSAExtConnected();

    /**
     * @brief get the proxy of BackupSAExt.
     *
     * @return the proxy of BackupSAExt.
     */
    sptr<ILocalAbilityManager> GetBackupSAExtProxy();

    /**
     * @brief load remote BackupSAExt.
     *
     * @return ErrCode OK if load, otherwise load failed.
     */
    ErrCode LoadBackupSAExt();

        /**
     * @brief load remote BackupSAExt by samgr.
     *
     * @return ErrCode OK if load, otherwise load failed.
     */
    ErrCode LoadBackupSAExtInner();

        /**
     * @brief call backup BackupSAExt.
     *
     * @return ErrCode OK if backup, otherwise backupErr.
     */
    ErrCode CallBackupSA();

        /**
     * @brief call restore BackupSAExt.
     *
     * @param fd restore文件描述符
     * @return ErrCode OK if restore, otherwise restoreErr.
     */
    ErrCode CallRestoreSA(UniqueFd fd);

public:
    SABackupConnection(
        std::function<void(const std::string &&)> callDied,
        std::function<void(const std::string &&)> callConnect,
        std::function<void(const std::string &&, const int &&, const std::string &&, const ErrCode &&)> callBackup,
        std::function<void(const std::string &&, const std::string &&, const ErrCode &&)> callRestore)
        : callDied_(callDied), callConnected_(callConnect), callBackup_(callBackup), callRestore_(callRestore)
    {
        threadPool_.Start(BConstants::EXTENSION_THREAD_POOL_COUNT);
    }
    virtual ~SABackupConnection()
    {
        threadPool_.Stop();
    };

public:
    class SALoadCallback : public SystemAbilityLoadCallbackStub {
    public:
        void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject) override;
        void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;

    public:
        std::condition_variable proxyConVar_;
        std::atomic<bool> isLoadSuccess_ = {false};
    };

private:
    std::function<void(const std::string &&)> callDied_;
    std::function<void(const std::string &&)> callConnected_;
    std::function<void(const std::string &&, const int &&, const std::string &&, const ErrCode &&)> callBackup_;
    std::function<void(const std::string &&, const std::string &&, const ErrCode &&)> callRestore_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> isConnected_ = {false};
    std::atomic<bool> isLoaded_ = {false};
    sptr<ILocalAbilityManager> proxy_;
    OHOS::ThreadPool threadPool_;
    std::atomic<int> reloadNum_ = 0;
    std::string extension_;
    std::string bundleName_;
    int32_t saId_ = BConstants::BACKUP_DEFAULT_SA_ID;
    MessageParcel parcel_;
    std::string extInfo_;
    UniqueFd fd_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_SA_BACKUP_CONNECTION_H