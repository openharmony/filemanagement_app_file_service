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
#include "b_json/b_json_clear_data_config.h"
#include "b_json/b_json_entity_caps.h"
#include "b_json/b_json_service_disposal_config.h"
#include "i_service_reverse.h"
#include "iremote_stub.h"
#include "module_sched/sched_scheduler.h"
#include "service_stub.h"
#include "svc_session_manager.h"
#include "system_ability.h"
#include "thread_pool.h"

namespace OHOS::FileManagement::Backup {
struct ExtensionMutexInfo {
    std::string bundleName;
    std::mutex callbackMutex;
    ExtensionMutexInfo(std::string bundleName_) : bundleName(bundleName_) {};
};

struct BundleTaskInfo {
    std::string reportTime;
    ErrCode errCode;
};

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
    ErrCode ServiceResultReport(const std::string restoreRetInfo,
        BackupRestoreScenario sennario, ErrCode errCode) override;
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
    ErrCode Cancel(std::string bundleName, int32_t &result) override;

    UniqueFd GetLocalCapabilitiesIncremental(const std::vector<BIncrementalData> &bundleNames) override;
    ErrCode GetAppLocalListAndDoIncrementalBackup() override;
    ErrCode InitIncrementalBackupSession(sptr<IServiceReverse> remote) override;
    ErrCode AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup) override;
    ErrCode AppendBundlesIncrementalBackupSession(const std::vector<BIncrementalData> &bundlesToBackup,
        const std::vector<std::string> &infos) override;

    ErrCode PublishIncrementalFile(const BFileInfo &fileInfo) override;
    ErrCode PublishSAIncrementalFile(const BFileInfo &fileInfo, UniqueFd fd) override;
    ErrCode AppIncrementalFileReady(const std::string &fileName, UniqueFd fd, UniqueFd manifestFd,
        int32_t errCode) override;
    ErrCode AppIncrementalDone(ErrCode errCode) override;
    ErrCode GetIncrementalFileHandle(const std::string &bundleName, const std::string &fileName) override;
    ErrCode GetBackupInfo(BundleName &bundleName, std::string &result) override;
    ErrCode UpdateTimer(BundleName &bundleName, uint32_t timeout, bool &result) override;
    ErrCode UpdateSendRate(std::string &bundleName, int32_t sendRate, bool &result) override;
    ErrCode ReportAppProcessInfo(const std::string processInfo, const BackupRestoreScenario sennario) override;
    ErrCode StartExtTimer(bool &isExtStart) override;
    ErrCode StartFwkTimer(bool &isFwkStart) override;

    ErrCode SAResultReport(const std::string bundleName, const std::string resultInfo,
                           const ErrCode errCode, const BackupRestoreScenario sennario);
    void StartGetFdTask(std::string bundleName, wptr<Service> ptr);

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
    void OnBackupExtensionDied(const std::string &&bundleName, bool isCleanCalled = false);

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
     * @brief 备份恢复开始，设置处置位
     *
     * @param bundleName 应用名称
     *
     */
    void SendStartAppGalleryNotify(const std::string &bundleName);

    /**
     * @brief 备份恢复结束，清理处置位
     *
     * @param bundleName 应用名称
     *
     */
    void SendEndAppGalleryNotify(const std::string &bundleName);

    /**
     * @brief 备份恢复异常结束，清理处置位
     *
     */
    void SendErrAppGalleryNotify();

    /**
     * @brief SA开始时，清理配置文件中的处置位
     *
     */
    void ClearDisposalOnSaStart();

    /**
     * @brief 备份恢复全部结束，删除配置文件
     *
     *
     */
    void DeleteDisConfigFile();

    /**
     * @brief 尝试清理处置
     *
     * @param bundleName 应用名称
     *
     */
    void TryToClearDispose(const BundleName &bundleName);

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

    /**
     * @brief GetBackupInfo extension成功回调
     *
     * @param obj 当前对象
     * @param bundleName 应用名称
     *
     */
    std::function<void(const std::string &&)> GetBackupInfoConnectDone(wptr<Service> obj, std::string &bundleName);

    /**
     * @brief GetBackupInfo extension死亡回调
     *
     * @param obj 当前对象
     * @param bundleName 应用名称
     *
     */
    std::function<void(const std::string &&, bool)> GetBackupInfoConnectDied(
        wptr<Service> obj, std::string &bundleName);

    /**
     * @brief timeout callback
     *
     * @param ptr 当前对象
     * @param bundleName 应用名称
     */
    std::function<void()> TimeOutCallback(wptr<Service> ptr, std::string bundleName);

    /**
     * @brief do timeout
     *
     * @param ptr 当前对象
     * @param bundleName 应用名称
     */
    void DoTimeout(wptr<Service> ptr, std::string bundleName);

    /**
     * @brief 清理残留数据
     *
     * @param bundleName 应用名称
     *
     */
    ErrCode ClearResidualBundleData(const std::string &bundleName);

    /**
     * @brief 添加清理记录
     *
     * @param bundleName 应用名称
     *
     */
    void AddClearBundleRecord(const std::string &bundleName);

    /**
     * @brief 删除清理记录
     *
     * @param bundleName 应用名称
     *
     */
    void DelClearBundleRecord(const std::vector<std::string> &bundleNames);

    /**
     * @brief 获取extension锁
     *
     * @param bundleName 应用名称
     *
     */
    std::shared_ptr<ExtensionMutexInfo> GetExtensionMutex(const BundleName &bundleName);

    /**
     * @brief 清理extension锁
     *
     * @param bundleName 应用名称
     *
     */
    void RemoveExtensionMutex(const BundleName &bundleName);
    void StartRunningTimer(const std::string &bundleName);
public:
    explicit Service(int32_t saID, bool runOnCreate = false) : SystemAbility(saID, runOnCreate)
    {
        threadPool_.Start(BConstants::EXTENSION_THREAD_POOL_COUNT);
        session_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(this)));
        disposal_ = make_shared<BJsonDisposalConfig>();
        clearRecorder_ = make_shared<BJsonClearDataConfig>();
        sched_ = sptr(new SchedScheduler(wptr(this), wptr(session_)));
    };
    ~Service() override
    {
        threadPool_.Stop();
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
     * @param isClearDataFlags 清理数据标志集合
     * @param restoreType 任务类型
     */
    void SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo> &restoreBundleInfos,
        std::vector<std::string> &restoreBundleNames,
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> &bundleNameDetailMap,
        std::map<std::string, bool> &isClearDataFlags, RestoreTypeEnum restoreType);

    /**
     * @brief set session info
     *
     * @param restoreBundleInfos: bundles to be restored
     * @param restoreBundleNames: bundles info to be restored
     * @param restoreType: retore type
     */
    void SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo> &restoreBundleInfos,
        std::vector<std::string> &restoreBundleNames, RestoreTypeEnum restoreType);

    void SetCurrentSessProperties(BJsonEntityCaps::BundleInfo &info, std::map<std::string, bool> &isClearDataFlags,
        const std::string &bundleNameIndexInfo);

    /**
     * @brief add useridinfo to  current backup session
     *
     * @param bundleNames: bundleNames list
     * @param userId: userId
     * @param backupBundleInfos: backupBundleInfos
     * @param isIncBackup: isIncBackup
     *
     */
    void SetCurrentBackupSessProperties(const std::vector<std::string> &bundleNames, int32_t userId,
        std::vector<BJsonEntityCaps::BundleInfo> &backupBundleInfos, bool isIncBackup);

    /**
     * @brief send userid to app
     *
     * @param bundleName: bundleName
     * @param userId: userId
     *
     */
    void SendUserIdToApp(std::string &bundleName, int32_t userId);

    /**
     * @brief 通知权限模块
     *
     * @param bundleName 包名称
     *
    */
    void NotifyCloneBundleFinish(std::string bundleName, const BackupRestoreScenario sennario);

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

    /**
     * @brief 执行通知调用方
     *
     * @param errCode 错误码
     * @param callerName 业务调用方
     *
     */
    void NotifyCallerCurAppDone(ErrCode errCode, const std::string &callerName);

    /**
     * @brief 执行通知调用方
     *
     * @param errCode 错误码
     * @param callerName 业务调用方
     *
     */
    void NotifyCallerCurAppIncrementDone(ErrCode errCode, const std::string &callerName);

    void SetWant(AAFwk::Want &want, const BundleName &bundleName, const BConstants::ExtensionAction &action);

    /**
     * @brief GetBackupInfo 任务执行
     *
     * @param bundleName 应用名称
     * @param result 业务结果出参
     *
     */
    ErrCode GetBackupInfoCmdHandle(BundleName &bundleName, std::string &result);

    /**
     * @brief 添加需要清理的Session
     *
     * @param bundleNames 需要清理的应用包信息
     *
     */
    ErrCode AppendBundlesClearSession(const std::vector<BundleName> &bundleNames);

    /**
     * @brief 设置是否自占用Session
     *
     * @param isOccupyingSession 框架是否自占用session
     *
     */
    void SetOccupySession(bool isOccupyingSession);

    void ReportOnExtConnectFailed(const IServiceReverse::Scenario scenario,
        const std::string &bundleName, const ErrCode ret);

    void ReleaseOnException();

    vector<BIncrementalData> MakeDetailList(const vector<BundleName> &bundleNames);

    vector<string> GetBundleNameByDetails(const std::vector<BIncrementalData> &bundlesToBackup);

    void HandleCurGroupBackupInfos(vector<BJsonEntityCaps::BundleInfo> &bundleInfos,
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> &bundleNameDetailMap,
        std::map<std::string, bool> &isClearDataFlags);

    void HandleCurGroupIncBackupInfos(vector<BJsonEntityCaps::BundleInfo> &bundleInfos,
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> &bundleNameDetailMap,
        std::map<std::string, bool> &isClearDataFlags);

    void TimeoutRadarReport(IServiceReverse::Scenario scenario, std::string &bundleName);

    void OnBundleStarted(BError error, sptr<SvcSessionManager> session, const BundleName &bundleName);

    void HandleExceptionOnAppendBundles(sptr<SvcSessionManager> session, const vector<BundleName> &appendBundleNames,
        const vector<BundleName> &restoreBundleNames);

    void BundleBeginRadarReport(const std::string &bundleName, const ErrCode errCode,
        const IServiceReverse::Scenario scenario);

    void BundleEndRadarReport(const std::string &bundleName, const ErrCode errCode,
        const IServiceReverse::Scenario scenario);

    void FileReadyRadarReport(const std::string &bundleName, const std::string &fileName, const ErrCode errCode,
        const IServiceReverse::Scenario scenario);

    void ExtensionConnectFailRadarReport(const std::string &bundleName, const ErrCode errCode,
        const IServiceReverse::Scenario scenario);

    void UpdateFailedBundles(const std::string &bundleName, BundleTaskInfo taskInfo);

    void ClearFailedBundles();
    void CreateDirIfNotExist(const std::string &path);

    std::vector<std::string> GetSupportBackupBundleNames(vector<BJsonEntityCaps::BundleInfo> &bundleInfos,
        bool isIncBackup, const vector<std::string> &srcBundleNames);

    void RefreshBundleDataSize(const vector<BJsonEntityCaps::BundleInfo> &newBundleInfos,
        std::string bundleName, wptr<Service> ptr);

    void HandleNotSupportBundleNames(const std::vector<std::string> &srcBundleNames,
        std::vector<std::string> &supportBundleNames, bool isIncBackup);

    void SetBundleIncDataInfo(const std::vector<BIncrementalData> &bundlesToBackup,
        std::vector<std::string> &supportBundleNames);
    
    void CancelTask(std::string bundleName, wptr<Service> ptr);
private:
    static sptr<Service> instance_;
    static std::mutex instanceLock_;
    std::mutex getBackupInfoProcLock_;
    std::mutex getBackupInfoSyncLock_;
    std::condition_variable getBackupInfoCondition_;
    static inline std::atomic<uint32_t> seed {1};
    std::atomic<bool> isConnectDied_ {false};
    std::atomic<bool> isOccupyingSession_ {false};

    sptr<SvcSessionManager> session_;
    sptr<SchedScheduler> sched_;
    std::shared_ptr<BJsonDisposalConfig> disposal_;
    std::shared_ptr<BJsonClearDataConfig> clearRecorder_;
    std::atomic<bool> isInRelease_ {false};
    std::atomic<bool> isRmConfigFile_ {true};
    friend class ServiceTest;

    OHOS::ThreadPool threadPool_;
    std::mutex extensionMutexLock_;
    std::mutex failedBundlesLock_;
public:
    std::map<BundleName, std::shared_ptr<ExtensionMutexInfo>> backupExtMutexMap_;
    std::map<BundleName, BundleTaskInfo> failedBundles_;
    std::atomic<uint32_t> successBundlesNum_ {0};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_H