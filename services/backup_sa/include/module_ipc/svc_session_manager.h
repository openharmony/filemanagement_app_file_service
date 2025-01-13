/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "b_incremental_data.h"
#include "b_resources/b_constants.h"
#include "i_service.h"
#include "i_service_reverse.h"
#include "module_ipc/svc_backup_connection.h"
#include "module_ipc/sa_backup_connection.h"
#include "svc_death_recipient.h"
#include "timer.h"

namespace OHOS::FileManagement::Backup {
struct BackupExtInfo {
    bool receExtManageJson {false};
    bool receExtAppDone {false};
    bool isBundleFinished {false};
    std::string backupExtName;
    sptr<SvcBackupConnection> backUpConnection;
    std::shared_ptr<SABackupConnection> saBackupConnection;
    std::set<std::string> fileNameInfo;
    BConstants::ServiceSchedAction schedAction {BConstants::ServiceSchedAction::WAIT};
    /* [RESTORE] Record whether data backup is required during the app exec restore proceess. */
    RestoreTypeEnum restoreType;
    /* Clone App: old device app versionCode */
    int64_t versionCode;
    /* Clone App: old device app versionCode */
    std::string versionName;
    /* Ext Ability APP process time */
    uint32_t timerId;
    /* Timer Status: true is start & false is stop */
    bool extTimerStatus {false};
    bool fwkTimerStatus {false};
    uint32_t timeout;
    uint32_t startTime;
    int64_t dataSize;
    int64_t lastIncrementalTime;
    int32_t manifestFd;
    std::string backupParameters;
    int32_t backupPriority;
    std::string extInfo;
    int32_t userId {100};
    int32_t appendNum {1};
    bool isClearData {true};
    bool isInPublishFile {false};
    bool isReadyLaunch {false};
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
        RestoreTypeEnum restoreDataType {RESTORE_DATA_WAIT_SEND};
        bool isIncrementalBackup {false};
        std::string oldBackupVersion {""};
        std::string callerName {};
        std::string activeTime {};
    };

public:
    /**
     * @brief 检验调用者是否是会话所有者，且当前命令是否与当前场景相匹配
     *
     * @param clientToken 调用者TOKEN
     * @param scenario 给定场景
     *
     * @return ErrCode 错误码
     */
    ErrCode VerifyCallerAndScenario(uint32_t clientToken, IServiceReverse::Scenario scenario) const;

    /**
     * @brief 激活会话
     *
     * @param impl 客户端信息
     * @param isOccupyingSession 框架是否自占用session
     *
     * @return ErrCode 错误码
     */
    ErrCode Active(Impl newImpl, bool isOccupyingSession = false);

    /**
     * @brief 关闭会话
     *
     * @param remoteInAction 尝试关闭会话的客户端代理。只有激活会话的客户端代理有权关闭会话
     * @param force 强制关闭
     *
     * @return ErrCode 错误码
     */
    ErrCode Deactive(const wptr<IRemoteObject> &remoteInAction, bool force = false);

    /**
     * @brief 检验调用者给定的bundleName是否是有效的
     *
     * @param bundleName 调用者名称
     * @return ErrCode 调用者不是会话所有者
     */
    ErrCode VerifyBundleName(std::string &bundleName);

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
     * @brief 获取当前处理事务会话对应的callerName
     *
     * @return string
     */
    std::string GetSessionCallerName();

    /**
     * @brief 获取当前处理事务会话对应的激活时间
     *
     * @return string
     */
    std::string GetSessionActiveTime();

    /**
     * @brief 更新backupExtNameMap并判断是否完成分发
     *
     * @param bundleName 客户端信息
     * @param fileName 文件名称
     * @throw BError::Codes::SA_INVAL_ARG 获取异常
     * @return true 分发已完成
     * @return false 分发未完成
     */
    bool OnBundleFileReady(const std::string &bundleName, const std::string &fileName = "");

    /**
     * @brief 设置backup manage.json 信息
     *
     * @param bundleName 客户端信息
     * @param fd manage.json 文件描述符
     * @return UniqueFd 返回manage.json 文件描述符
     * @throw BError::Codes::SA_INVAL_ARG 获取异常
     */
    UniqueFd OnBundleExtManageInfo(const std::string &bundleName, UniqueFd fd);

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
     * @brief get sa extension connection
     *
     * @param bundleName
     * @return std::weak_ptr<SABackupConnection>
     */
    std::weak_ptr<SABackupConnection> GetSAExtConnection(const BundleName &bundleName);

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
     * @brief Set the Backup Ext Name object
     *
     * @param bundleName 应用名称
     * @param backupExtName extension name
     */
    void SetBackupExtName(const std::string &bundleName, const std::string &backupExtName);

    /**
     * @brief 获取backup extension name
     *
     * @param bundleName 应用名称
     * @return std::string extension name
     */
    std::string GetBackupExtName(const std::string &bundleName);

    /**
     * @brief 暂存ext info
     *
     * @param bundleName 应用名称
     * @param extInfo ext info
     */
    void SetBackupExtInfo(const std::string &bundleName, const std::string &extInfo);

    /**
     * @brief 获取ext info
     *
     * @param bundleName 应用名称
     * @return std::string ext info
     */
    std::string GetBackupExtInfo(const std::string &bundleName);

    /**
    * @brief 暂存应用用户id
    *
    * @param bundleName 应用名称
    * @param userId 用户id
    */
    void SetBundleUserId(const std::string &bundleName, const int32_t userId);

    /**
     * @brief 获取应用用户id
     *
     * @param bundleName 应用名称
     * @return int32_t userId
     */
    int32_t GetBundleUserId(const std::string &bundleName);

    /**
     * @brief 追加应用
     *
     * @param bundleNames 应用名称
     * @param failedBundles 返回失败应用名称
     */
    void AppendBundles(const std::vector<std::string> &bundleNames, std::vector<std::string> &failedBundles);

    /**
     * @brief 添加指定应用
     *
     * @param bundleName 应用名称
     */
    sptr<SvcBackupConnection> CreateBackupConnection(BundleName &bundleName);

    /**
     * @brief 开始备份
     *
     * @return ErrCode
     */
    ErrCode Start();

    /**
     * @brief 结束追加应用
     *
     * @return ErrCode
     */
    ErrCode Finish();

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
    void SetBundleVersionCode(const std::string &bundleName, int64_t versionCode);

    /**
     * @brief Get the bundle version code object
     *
     * @param  bundleName
     * @return versionCode
     */
    int64_t GetBundleVersionCode(const std::string &bundleName);

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
     * @brief 启动框架定时器
     *
     * @param bundleName 应用名称
     * @param callback 超时回调
     * @return bool
     */
    bool StartFwkTimer(const std::string &bundleName, const Utils::Timer::TimerCallback &callback);

    /**
     * @brief 停止框架定时器
     *
     * @param bundleName 应用名称
     * @param callback 超时回调
     * @return bool
     */
    bool StopFwkTimer(const std::string &bundleName);

    /**
     * @brief 启动extension定时器
     *
     * @param bundleName 应用名称
     * @param callback 超时回调
     * @return bool
     */
    bool StartExtTimer(const std::string &bundleName, const Utils::Timer::TimerCallback &callback);

    /**
     * @brief 停止extension定时器
     *
     * @param bundleName 应用名称
     * @return bool
     */
    bool StopExtTimer(const std::string &bundleName);

    /**
     * @brief 重新设置定时器
     *
     * @param bundleName 应用名称
     * @param timeout 超时时间
     * @return true
     * @return false
     */
    bool UpdateTimer(const std::string &bundleName, uint32_t timeout,
        const Utils::Timer::TimerCallback &callback);

    /**
     * @brief sessionCnt加计数
     *
     * @param sessionCnt
     */
    void IncreaseSessionCnt(const std::string funcName);

    /**
     * @brief sessionCnt加计数
     *
     * @param sessionCnt
     */
    void DecreaseSessionCnt(const std::string funcName);

    /**
     * @brief clear session data
     *
     */
    ErrCode ClearSessionData();

    /**
     * @brief Get the Is Incremental Backup object
     *
     * @return true
     * @return false
     */
    bool GetIsIncrementalBackup();

    /**
     * @brief Set the Incremental Data object
     *
     * @param incrementalData
     */
    void SetIncrementalData(const BIncrementalData &incrementalData);

    /**
     * @brief Get the Manifest Fd object
     *
     * @param bundleName 应用名称
     * @return int32_t
     */
    int32_t GetIncrementalManifestFd(const std::string &bundleName);

    /**
     * @brief Get the Last Incremental Time object
     *
     * @param bundleName
     * @return int64_t
     */
    int64_t GetLastIncrementalTime(const std::string &bundleName);

    /**
     * @brief 获取备份前内存参数
     *
     */
    int32_t GetMemParaCurSize();

    /**
     * @brief 记录备份前内存参数
     *
     * @param size
     */
    void SetMemParaCurSize(int32_t size);

    /**
     * @brief Set the old device backup version object
     *
     * @param backupVersion
     */
    void SetOldBackupVersion(const std::string &backupVersion);

    /**
     * @brief Get the old device backup version object
     */
    std::string GetOldBackupVersion();

    bool ValidRestoreDataType(RestoreTypeEnum restoreType);

    Impl GetImpl();
    int GetSessionCnt();

    void SetClearDataFlag(const std::string &bundleName, bool isClearData);
    bool GetClearDataFlag(const std::string &bundleName);

    bool CleanAndCheckIfNeedWait(ErrCode &ret, std::vector<std::string> &bundleNameList);

    void SetPublishFlag(const std::string &bundleName);

    void SetImplRestoreType(const RestoreTypeEnum restoreType);

    void SetIsReadyLaunch(const std::string &bundleName);

private:
    /**
     * @brief 获取backup extension ability
     *
     * @param bundleName 应用名称
     *
     * @return sptr<SvcBackupConnection>
     */
    sptr<SvcBackupConnection> GetBackupAbilityExt(const std::string &bundleName);

    /**
     * @brief 获取backup sa extension
     *
     * @param bundleName 应用名称
     *
     * @return std::shared_ptr<SABackupConnection>
     */
    std::shared_ptr<SABackupConnection> GetBackupSAExt(const std::string &bundleName);

    /**
     * @brief 初始化 clientProxy
     *
     * @param newImpl
     */
    ErrCode InitClient(Impl &newImpl);

    /**
     * @brief 获取BackupExtNameMap iterator
     *
     * @param bundleName 应用名称
     * @return std::map<BundleName, BackupExtInfo>::iterator
     */
    std::tuple<bool, std::map<BundleName, BackupExtInfo>::iterator> GetBackupExtNameMap(const std::string &bundleName);

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
        timer_.Setup();
    }
    ~SvcSessionManager() override
    {
        timer_.Shutdown();
    }

private:
    mutable std::shared_mutex lock_;
    wptr<Service> reversePtr_;
    sptr<SvcDeathRecipient> deathRecipient_;
    Impl impl_;
    uint32_t extConnectNum_ {0};
    Utils::Timer timer_ {"backupTimer"};
    std::atomic<int> sessionCnt_ {0};
    int32_t memoryParaCurSize_ {BConstants::DEFAULT_VFS_CACHE_PRESSURE};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_H
