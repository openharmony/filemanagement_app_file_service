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

#include "b_jsonutil/b_jsonutil.h"
#include "b_json/b_json_entity_caps.h"
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
    ErrCode AppFileReady(const std::string &fileName, UniqueFd fd, int32_t errCode) override;
    ErrCode AppDone(ErrCode errCode) override;
    ErrCode ServiceResultReport(const std::string restoreRetInfo, BackupRestoreScenario sennario) override;
    ErrCode GetFileHandle(const std::string &bundleName, const std::string &fileName) override;
    ErrCode AppendBundlesRestoreSession(UniqueFd fd,
                                        const std::vector<BundleName> &bundleNames,
                                        const std::vector<std::string> &bundleInfos,
                                        RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND,
                                        int32_t userId = DEFAULT_INVAL_VALUE) override;
    ErrCode AppendBundlesRestoreSession(UniqueFd fd,
                                        const std::vector<BundleName> &bundleNames,
                                        RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND,
                                        int32_t userId = DEFAULT_INVAL_VALUE) override;
    ErrCode AppendBundlesBackupSession(const std::vector<BundleName> &bundleNames) override;
    ErrCode AppendBundlesDetailsBackupSession(const std::vector<BundleName> &bundleNames,
                                              const std::vector<std::string> &bundleInfos) override;
    ErrCode Finish() override;
    ErrCode Release() override;

    UniqueFd GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames) override;
    ErrCode GetAppLocalListAndDoIncrementalBackup() override;
    ErrCode InitIncrementalBackupSession(sptr<IServiceReverse> remote) override;
    ErrCode AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup) override;

    ErrCode PublishIncrementalFile(const BFileInfo &fileInfo) override;
    ErrCode PublishSAIncrementalFile(const BFileInfo &fileInfo, UniqueFd fd) override;
    ErrCode AppIncrementalFileReady(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd,
        int32_t errCode) override;
    ErrCode AppIncrementalDone(ErrCode errCode) override;
    ErrCode GetIncrementalFileHandle(const std::string &bundleName, const std::string &fileName) override;
    ErrCode GetBackupInfo(BundleName &bundleName, std::string &result) override;
    ErrCode UpdateTimer(BundleName &bundleName, uint32_t timeOut, bool &result) override;

    ErrCode SAResultReport(const std::string bundleName, const std::string resultInfo,
                           const ErrCode errCode, const BackupRestoreScenario sennario);

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
     * @brief 执行启动 backup sa extension
     *
     * @param bundleName
     * @return ErrCode
     */
    ErrCode LaunchBackupSAExtension(const BundleName &bundleName);

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

    /**
     * @brief SA backup回调
     *
     * @param bundleName 应用名称
     * @param fd 备份数据
     * @param result SA备份的结果（异常）
     * @param errCode backup的错误
     *
     */
    void OnSABackup(const std::string &bundleName, const int &fd, const std::string &result, const ErrCode &errCode);

    /**
     * @brief SA restore回调
     *
     * @param bundleName 应用名称
     * @param result SA恢复的结果（异常）
     * @param errCode restore的错误
     *
     */
    void OnSARestore(const std::string &bundleName, const std::string &result, const ErrCode &errCode);

public:
    explicit Service(int32_t saID, bool runOnCreate = false) : SystemAbility(saID, runOnCreate)
    {
        session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(this)));
    };
    ~Service() override
    {

    };

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

    /**
     * @brief 设置当前session的关键信息
     *
     * @param restoreBundleInfos 待恢复的应用
     * @param restoreBundleNames 待恢复的应用包信息
     * @param bundleNameDetailMap bundle和detail的对应关系
     * @param restoreType 任务类型
     */
    void SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo> &restoreBundleInfos,
        std::vector<std::string> &restoreBundleNames,
        std::map<std::string, BJsonUtil::BundleDetailInfo> &bundleNameDetailMap, RestoreTypeEnum restoreType);

    /**
     * @brief 通知权限模块
     *
     * @param bundleName 包名称
     *
    */
    void NotifyCloneBundleFinish(std::string bundleName);

    /**
     * @brief SA 备份恢复结束
     *
     * @param bundleName SAID
     *
     * @return ErrCode OK if saDone, otherwise saDone failed.
    */
    ErrCode SADone(const ErrCode errCode, std::string bundleName);

    /**
     * @brief SA备份恢复入口
     *
     * @param bundleName SAID
     *
     * @return ErrCode OK if backup sa, otherwise backup sa failed.
    */
    ErrCode BackupSA(std::string bundleName);

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