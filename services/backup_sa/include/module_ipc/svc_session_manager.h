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

/*
 * 说明：
 * 备份服务同一时间只接受一个唯一的会话。在会话期间，服务只与一个备份应用通信
 * 且只响应备份或恢复接口中的一种。
 */

#ifndef OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_H
#define OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_H

#include <map>
#include <memory>
#include <shared_mutex>
#include <vector>

#include <refbase.h>

#include "b_file_info.h"
#include "b_resources/b_constants.h"
#include "i_service_reverse.h"
#include "i_service.h"
#include "module_ipc/svc_backup_connection.h"
#include "svc_death_recipient.h"
#include "timer.h"

namespace OHOS::FileManagement::Backup {
struct BackupExtInfo {
    bool receExtManageJson {false};
    bool receExtAppDone {false};
    bool isBundleFinished {false};
    std::string backupExtName;
    sptr<SvcBackupConnection> backUpConnection;
    std::set<std::string> fileNameInfo;
    BConstants::ServiceSchedAction schedAction {BConstants::ServiceSchedAction::WAIT};
    bool bNeedToInstall {false};
    std::string installState;
    /* [RESTORE] Record whether data backup is required during the app exec restore proceess. */
    RestoreTypeEnum restoreType;
    /* Clone App: old device app versionCode */
    uint32_t versionCode;
    /* Clone App: old device app versionCode */
    std::string versionName;
    /* Ext Ability APP process time */
    uint32_t extTimerId;
    /* Timer Status: true is start & false is stop */
    bool timerStatus {false};
    int64_t dataSize;
};

class Service;
class SvcSessionManager : public virtual RefBase {
public:
    struct Impl {
        uint32_t clientToken {0};
        IServiceReverse::Scenario scenario {IServiceReverse::Scenario::UNDEFINED};
        std::map<BundleName, BackupExtInfo> backupExtNameMap;
        sptr<IServiceReverse> clientProxy;
        bool isBackupStart {false};
        bool isAppendFinish {false};
        /* Note: Multi user scenario: <System Update Upgrade>
            Caller must complete all processes before next user tigger.<Session>
            [RESTORE] Support for multiple users, incoming during restore process.
        */
        int32_t userId {100};
    };

public:
    /**
     * @brief 检验调用者是否是会话所有者，且当前命令是否与当前场景相匹配
     *
     * @param clientToken 调用者TOKEN
     * @param scenario 给定场景
     * @throw BError::Codes::SA_REFUSED_ACT 调用者不是会话所有者
     * @throw BError::Codes::SDK_MIXED_SCENARIO 调用者在备份/恢复场景使用了不匹配的函数
     */
    void VerifyCallerAndScenario(uint32_t clientToken, IServiceReverse::Scenario scenario) const;

    /**
     * @brief 激活会话
     *
     * @param impl 客户端信息
     */
    void Active(Impl newImpl);

    /**
     * @brief 关闭会话
     *
     * @param remoteInAction 尝试关闭会话的客户端代理。只有激活会话的客户端代理有权关闭会话
     * @param force 强制关闭
     */
    void Deactive(const wptr<IRemoteObject> &remoteInAction, bool force = false);

    /**
     * @brief 检验调用者给定的bundleName是否是有效的
     *
     * @param bundleName 调用者名称
     * @throw BError::Codes::SA_REFUSED_ACT 调用者不是会话所有者
     */
    void VerifyBundleName(std::string &bundleName);

    /**
     * @brief 获取IServiceReverse
     *
     * @return sptr<IServiceReverse> 返回clientProxy
     * @throw BError::Codes::SA_REFUSED_ACT 调用者不是会话所有者
     */
    sptr<IServiceReverse> GetServiceReverseProxy();

    /**
     * @brief 获取Scenario
     *
     * @return IServiceReverse::Scenario 返回scenario
     * @throw BError::Codes::SA_INVAL_ARG 获取异常
     */
    IServiceReverse::Scenario GetScenario();

    /**
     * @brief 获取当前处理事务会话对应的userId
     *
     * @return int32_t
     */
    int32_t GetSessionUserId();

    /**
     * @brief 设置当前处理事务会话对应的userId
     *
     * @param userid 相关会话对应的userId
     */
    void SetSessionUserId(int32_t userId);

    /**
     * @brief 更新backupExtNameMap并判断是否完成分发
     *
     * @param bundleName 客户端信息
     * @param fileName 文件名称
     * @throw BError::Codes::SA_INVAL_ARG 获取异常
     * @return true 分发已完成
     * @return false 分发未完成
     */
    bool OnBunleFileReady(const std::string &bundleName, const std::string &fileName = "");

    /**
     * @brief 设置backup manage.json 信息
     *
     * @param bundleName 客户端信息
     * @param fd manage.json 文件描述符
     * @return UniqueFd 返回manage.json 文件描述符
     * @throw BError::Codes::SA_INVAL_ARG 获取异常
     */
    UniqueFd OnBunleExtManageInfo(const std::string &bundleName, UniqueFd fd);

    /**
     * @brief Remove backup extension info
     *
     * @param bundleName 应用名称
     */
    void RemoveExtInfo(const std::string &bundleName);

    /**
     * @brief get extension connection info
     *
     * @param bundleName
     * @return wptr<SvcBackupConnection>
     */
    wptr<SvcBackupConnection> GetExtConnection(const BundleName &bundleName);

    /**
     * @brief HiDumper dump info
     *
     * @param fd 对端dump句柄
     * @param args 服务参数
     */
    void DumpInfo(const int fd, const std::vector<std::u16string> &args);

    /**
     * @brief 暂存restore流程真实文件请求
     *
     * @param bundleName 应用名称
     * @param fileName  文件名称
     */
    void SetExtFileNameRequest(const std::string &bundleName, const std::string &fileName);

    /**
     * @brief 获取restore流程真实文件请求
     *
     * @param bundleName 应用名称
     * @return std::set<std::string> 返回真实文件vec
     */
    std::set<std::string> GetExtFileNameRequest(const std::string &bundleName);

    /**
     * @brief 获取ServiceSchedAction状态
     *
     * @param bundleName 应用名称
     * @return BConstants::ServiceSchedAction
     */
    BConstants::ServiceSchedAction GetServiceSchedAction(const std::string &bundleName);

    /**
     * @brief 设置ServiceSchedAction状态
     *
     * @param bundleName 应用名称
     * @param action 状态
     */
    void SetServiceSchedAction(const std::string &bundleName, BConstants::ServiceSchedAction action);

    /**
     * @brief 获取所需要的调度信息
     *
     * @return std::string 返回bundleName
     */
    bool GetSchedBundleName(std::string &bundleName);

    /**
     * @brief 获取backup extension name
     *
     * @param bundleName 应用名称
     * @return std::string
     * @return std::string extension name
     */
    std::string GetBackupExtName(const std::string &bundleName);

    /**
     * @brief 追加应用
     *
     * @param bundleNames 应用名称
     */
    void AppendBundles(const std::vector<std::string> &bundleNames);

    /**
     * @brief 开始备份
     *
     * @return ErrCode
     */
    void Start();

    /**
     * @brief 结束追加应用
     *
     * @return ErrCode
     */
    void Finish();

    /**
     * @brief 整个备份恢复流程是否结束
     *
     * @return true 备份恢复流程结束
     * @return false 备份恢复流程未结束
     */
    bool IsOnAllBundlesFinished();

    /**
     * @brief 是否启动调度器
     *
     * @return true 启动调度器
     * @return false 不启动调度器
     */
    bool IsOnOnStartSched();

    /**
     * @brief Set the Install State object
     *
     * @param bundleName
     * @param state
     */
    void SetInstallState(const std::string &bundleName, const std::string &state);

    /**
     * @brief Get the Install State object
     *
     * @param bundleName
     * @return std::string
     */
    std::string GetInstallState(const std::string &bundleName);

    /**
     * @brief Set the Need To Install object
     *
     * @param bundleName
     * @param needToInstall
     */
    void SetNeedToInstall(const std::string &bundleName, bool needToInstall);

    /**
     * @brief Get the Need To Install object
     *
     * @param bundleName
     * @return true
     * @return false
     */
    bool GetNeedToInstall(const std::string &bundleName);

    /**
     * @brief Whether to unload the service
     *
     * @return true
     * @return false
     */
    bool NeedToUnloadService();

/**
     * @brief Set the bundle restore type object
     *
     * @param bundleName
     * @param restoreType
     */
    void SetBundleRestoreType(const std::string &bundleName, RestoreTypeEnum restoreType);

    /**
     * @brief Get the bundle restore type object
     *
     * @param  bundleName
     * @return restoreType
     */
    RestoreTypeEnum GetBundleRestoreType(const std::string &bundleName);

    /**
     * @brief Set the bundle version code object
     *
     * @param bundleName
     * @param versionCode
     */
    void SetBundleVersionCode(const std::string &bundleName, uint32_t versionCode);

    /**
     * @brief Get the bundle version code object
     *
     * @param  bundleName
     * @return versionCode
     */
    uint32_t GetBundleVersionCode(const std::string &bundleName);

    /**
     * @brief Set the bundle version name object
     *
     * @param bundleName
     * @param versionName
     */
    void SetBundleVersionName(const std::string &bundleName, std::string versionName);

    /**
     * @brief Get the bundle version name object
     *
     * @param  bundleName
     * @return versionName
     */
    std::string GetBundleVersionName(const std::string &bundleName);

    /**
     * @brief Set the bundle data size object
     *
     * @param bundleName
     * @param dataSize
     */
    void SetBundleDataSize(const std::string &bundleName, int64_t dataSize);

    /**
     * @brief Get the bundle data size object
     *
     * @param  bundleName
     * @return dataSize
     */
    int64_t GetBundleDataSize(const std::string &bundleName);

    /**
     * @brief 启动应用扩展能力定时器
     *
     * @param bundleName 应用名称
     * @return
     */
    void BundleExtTimerStart(const std::string &bundleName, const Utils::Timer::TimerCallback& callback);

    /**
     * @brief 取消/暂停应用扩展能力定时器
     *
     * @param bundleName 应用名称
     * @return
     */
    void BundleExtTimerStop(const std::string &bundleName);

private:
    /**
     * @brief 获取backup extension ability
     *
     * @param bundleName 应用名称
     */
    sptr<SvcBackupConnection> GetBackupExtAbility(const std::string &bundleName);

    /**
     * @brief 初始化 clientProxy
     *
     * @param newImpl
     */
    void InitClient(Impl &newImpl);

    /**
     * @brief 获取BackupExtNameMap iterator
     *
     * @param bundleName 应用名称
     * @return std::map<BundleName, BackupExtInfo>::iterator
     */
    std::map<BundleName, BackupExtInfo>::iterator GetBackupExtNameMap(const std::string &bundleName);

    /**
     * @brief 计算出应用程序处理数据可能使用的时间
     *
     * @param bundleName 应用名称
     * @return
     */
    uint32_t CalAppProcessTime(const std::string &bundleName);

public:
    /**
     * @brief Construct a new Svc Session object
     *
     * @param reversePtr 指向Service的反向指针，使用wptr避免循环引用
     */
    explicit SvcSessionManager(wptr<Service> reversePtr) : reversePtr_(reversePtr)
    {
        extBundleTimer.Setup();
    }
    ~SvcSessionManager() override
    {
        extBundleTimer.Shutdown();
    }

private:
    mutable std::shared_mutex lock_;
    wptr<Service> reversePtr_;
    sptr<SvcDeathRecipient> deathRecipient_;
    Impl impl_;
    uint32_t extConnectNum_ {0};
    Utils::Timer extBundleTimer {"backupBundleExtTimer"};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_H
