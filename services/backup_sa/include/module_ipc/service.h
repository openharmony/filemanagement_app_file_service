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

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_H

#include <cstdint>
#include <mutex>

#include "i_service_reverse.h"
#include "iremote_stub.h"
#include "module_sched/sched_scheduler.h"
#include "service_stub.h"
#include "svc_session_manager.h"
#include "system_ability.h"

namespace OHOS::FileManagement::Backup {
class Service : public SystemAbility, public ServiceStub, protected NoCopyable {
    DECLARE_SYSTEM_ABILITY(Service);

    // 以下都是IPC接口
public:
    ErrCode InitRestoreSession(sptr<IServiceReverse> remote) override;
    ErrCode InitBackupSession(sptr<IServiceReverse> remote) override;
    ErrCode Start() override;
    UniqueFd GetLocalCapabilities() override;
    ErrCode PublishFile(const BFileInfo &fileInfo) override;
    ErrCode AppFileReady(const std::string &fileName, UniqueFd fd) override;
    ErrCode AppDone(ErrCode errCode) override;
    ErrCode ServiceResultReport(const std::string &restoreRetInfo) override;
    ErrCode GetFileHandle(const std::string &bundleName, const std::string &fileName) override;
    ErrCode AppendBundlesRestoreSession(UniqueFd fd,
                                        const std::vector<BundleName> &bundleNames,
                                        RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND,
                                        int32_t userId = DEFAULT_INVAL_VALUE) override;
    ErrCode AppendBundlesBackupSession(const std::vector<BundleName> &bundleNames) override;
    ErrCode Finish() override;
    ErrCode Release() override;

    UniqueFd GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames) override;
    ErrCode InitIncrementalBackupSession(sptr<IServiceReverse> remote) override;
    ErrCode AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup) override;

    ErrCode PublishIncrementalFile(const BFileInfo &fileInfo) override;
    ErrCode AppIncrementalFileReady(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd) override;
    ErrCode AppIncrementalDone(ErrCode errCode) override;
    ErrCode GetIncrementalFileHandle(const std::string &bundleName, const std::string &fileName) override;
    ErrCode GetBackupInfo(BundleName &bundleName, std::string &result) override;

    // 以下都是非IPC接口
public:
    void OnStart() override;
    void OnStop() override;
    void StopAll(const wptr<IRemoteObject> &obj, bool force = false);
    int Dump(int fd, const std::vector<std::u16string> &args) override;

    /**
     * @brief 执行启动 backup extension
     *
     * @param bundleName
     * @return ErrCode
     */
    virtual ErrCode LaunchBackupExtension(const BundleName &bundleName);

    /**
     * @brief backup extension died
     *
     * @param bundleName 应用名称
     */
    void OnBackupExtensionDied(const std::string &&bundleName);

    /**
     * @brief extension启动连接成功
     *
     * @param bundleName 应用名称
     */
    void ExtConnectDone(std::string bundleName);

    /**
     * @brief extension启动连接失败
     *
     * @param bundleName 应用名称
     */
    void ExtConnectFailed(const std::string &bundleName, ErrCode ret);

    /**
     * @brief 执行backup extension 备份恢复流程
     *
     * @param bundleName 应用名称
     */
    virtual void ExtStart(const std::string &bundleName);

    /**
     * @brief 发送备份恢复通知到应用市场
     *
     * @param bundleName 应用名称
     *
     */
    void SendAppGalleryNotify(const std::string &bundleName);

    /**
     * @brief 结束会话删除session，卸载服务
     *
     */
    void SessionDeactive();
    /**
     * @brief 构造拉起应用所需的want
     *
     * @param bundleName 应用名称
     *
     */
    AAFwk::Want CreateConnectWant (BundleName &bundleName);

public:
    explicit Service(int32_t saID, bool runOnCreate = false) : SystemAbility(saID, runOnCreate)
    {
        session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(this)));
    };
    ~Service() override = default;

private:
    /**
     * @brief 验证调用者
     *
     */
    void VerifyCaller();

    /**
     * @brief 验证调用者
     *
     * @param scenario Scenario状态
     */
    void VerifyCaller(IServiceReverse::Scenario scenario);

    /**
     * @brief 验证调用者并返回名称
     *
     * @return std::string
     */
    std::string VerifyCallerAndGetCallerName();

    /**
     * @brief 清除Session Sched相关资源
     *
     * @param bundleName 应用名称
     */
    void ClearSessionAndSchedInfo(const std::string &bundleName);

    /**
     * @brief 整个备份恢复流程结束
     *
     * @param errCode 错误码
     */
    void OnAllBundlesFinished(ErrCode errCode);

    /**
     * @brief 执行调度器
     *
     */
    void OnStartSched();

    /**
     * @brief 通知客户端程序扩展能力处理结果
     *
     * @param bundleName 应用名称
     *
     */
    void NoticeClientFinish(const std::string &bundleName, ErrCode errCode);

    /**
     * @brief 处理有依赖的应用恢复
     *
     * @param bundleName 应用名称
     *
     */
    void HandleRestoreDepsBundle(const std::string &bundleName);

    /**
     * @brief 增量备份恢复逻辑处理
     *
     * @param bundleName
     * @return true
     * @return false
     */
    bool IncrementalBackup(const std::string &bundleName);

    /**
     * @brief extension连接断开
     *
     * @param bundleName
     */
    void ExtConnectDied(const std::string &bundleName);

private:
    static sptr<Service> instance_;
    static std::mutex instanceLock_;
    std::mutex getBackupInfoMutx_;
    std::condition_variable getBackupInfoCondition_;
    static inline std::atomic<uint32_t> seed {1};

    sptr<SvcSessionManager> session_;
    sptr<SchedScheduler> sched_;

    friend class ServiceTest;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_H