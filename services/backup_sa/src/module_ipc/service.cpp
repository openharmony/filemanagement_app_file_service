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

/*
 * 注意：
 *     - 注意点1：本文件原则上只处理与IPC无关的业务逻辑
 *     - 注意点2：This document, in principle, captures all exceptions.
 *               Prevent exceptions from spreading to insecure modules.
 */
#include "module_ipc/service.h"

#include <algorithm>
#include <chrono>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <regex>

#include <fcntl.h>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>

#include <directory_ex.h>

#include "ability_manager_client.h"
#include "accesstoken_kit.h"
#include "b_anony/b_anony.h"
#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_file_info.h"
#include "b_hiaudit/hi_audit.h"
#include "b_json/b_json_cached_entity.h"
#include "b_jsonutil/b_jsonutil.h"
#include "b_ohos/startup/backup_para.h"
#include "b_process/b_multiuser.h"
#include "b_radar/b_radar.h"
#include "b_resources/b_constants.h"
#include "b_sa/b_sa_utils.h"
#include "b_utils/b_time.h"
#include "bundle_mgr_client.h"
#include "filemgmt_libhilog.h"
#include "hisysevent.h"
#include "hitrace_meter.h"
#include "ipc_skeleton.h"
#include "access_token.h"
#include "tokenid_kit.h"
#include "module_app_gallery/app_gallery_dispose_proxy.h"
#include "module_external/bms_adapter.h"
#include "module_external/sms_adapter.h"
#include "module_ipc/svc_backup_connection.h"
#include "module_ipc/svc_restore_deps_manager.h"
#include "module_notify/notify_work_service.h"
#include "parameter.h"
#include "parameters.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

REGISTER_SYSTEM_ABILITY_BY_ID(Service, FILEMANAGEMENT_BACKUP_SERVICE_SA_ID, false);

namespace {
constexpr int32_t DEBUG_ID = 100;
constexpr int32_t INDEX = 3;
constexpr int32_t MS_1000 = 1000;
const static string BROADCAST_TYPE = "broadcast";
const std::string FILE_BACKUP_EVENTS = "FILE_BACKUP_EVENTS";
const static string UNICAST_TYPE = "unicast";
const int32_t CONNECT_WAIT_TIME_S = 15;
const std::string BACKUPSERVICE_WORK_STATUS_KEY = "persist.backupservice.workstatus";
const std::string BACKUPSERVICE_WORK_STATUS_ON = "true";
const std::string BACKUPSERVICE_WORK_STATUS_OFF = "false";
const std::string BACKUP_PERMISSION = "ohos.permission.BACKUP";
const int32_t MAX_TRY_CLEAR_DISPOSE_NUM = 3;
} // namespace

/* Shell/Xts user id equal to 0/1, we need set default 100 */
int32_t Service::GetUserIdDefault()
{
    auto [isDebug, debugId] = BackupPara().GetBackupDebugOverrideAccount();
    if (isDebug && debugId > DEBUG_ID) {
        return debugId;
    }
    auto multiuser = BMultiuser::ParseUid(IPCSkeleton::GetCallingUid());
    HILOGI("GetUserIdDefault userId=%{public}d.", multiuser.userId);
    if ((multiuser.userId == BConstants::SYSTEM_UID) || (multiuser.userId == BConstants::XTS_UID)) {
        return BConstants::DEFAULT_USER_ID;
    }
    return multiuser.userId;
}

void Service::OnStartResRadarReport(const std::vector<std::string> &bundleNameList, int32_t stage)
{
    std::stringstream ss;
    ss << "failedBundles:{";
    for (const auto &bundleName : bundleNameList) {
        ss << bundleName << ", ";
    }
    ss << "}";
    AppRadar::Info info("", "", ss.str());
    AppRadar::GetInstance().RecordDefaultFuncRes(info, "Service::OnStart", GetUserIdDefault(),
        static_cast<BizStageBackup>(stage), ERR_OK);
}

void Service::BundleBeginRadarReport(const std::string &bundleName, const ErrCode errCode,
    const IServiceReverseType::Scenario scenario)
{
    if (errCode == ERR_OK || errCode == BError(BError::Codes::SA_BOOT_EXT_FAIL).GetCode()) {
        return;
    }
    if (!IsReportBundleExecFail(bundleName)) {
        return;
    }
    UpdateBundleRadarReport(bundleName);
    BundleTaskInfo taskInfo;
    taskInfo.reportTime = TimeUtils::GetCurrentTime();
    taskInfo.errCode = errCode;
    UpdateFailedBundles(bundleName, taskInfo);
    AppRadar::Info info(bundleName, "", "");
    if (scenario == IServiceReverseType::Scenario::RESTORE) {
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::BundleBeginRadarReport",
            GetUserIdDefault(), BizStageRestore::BIZ_STAGE_APPEND_BUNDLES_FAIL, errCode);
    } else if (scenario == IServiceReverseType::Scenario::BACKUP) {
        AppRadar::GetInstance().RecordBackupFuncRes(info, "Service::BundleBeginRadarReport",
            GetUserIdDefault(), BizStageBackup::BIZ_STAGE_APPEND_BUNDLES_FAIL, errCode);
    }
}

void Service::BundleEndRadarReport(const std::string &bundleName, ErrCode errCode,
    const IServiceReverseType::Scenario scenario)
{
    if (errCode == ERR_OK) {
        successBundlesNum_.fetch_add(1);
        return;
    }
    if (!IsReportBundleExecFail(bundleName)) {
        return;
    }
    if (session_->GetTimeoutValue(bundleName) == 0) {
        errCode = BError::BackupErrorCode::E_FORCE_TIMEOUT;
    }
    UpdateBundleRadarReport(bundleName);
    BundleTaskInfo taskInfo;
    taskInfo.reportTime = TimeUtils::GetCurrentTime();
    taskInfo.errCode = errCode;
    UpdateFailedBundles(bundleName, taskInfo);
    AppRadar::Info info(bundleName, "", "");
    if (scenario == IServiceReverseType::Scenario::RESTORE) {
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::BundleEndRadarReport",
            GetUserIdDefault(), BizStageRestore::BIZ_STAGE_EXECU_FAIL, errCode);
    } else if (scenario == IServiceReverseType::Scenario::BACKUP) {
        AppRadar::GetInstance().RecordBackupFuncRes(info, "Service::BundleEndRadarReport",
            GetUserIdDefault(), BizStageBackup::BIZ_STAGE_EXECU_FAIL, errCode);
    }
}

void Service::FileReadyRadarReport(const std::string &bundleName, const std::string &fileName, const ErrCode errCode,
    const IServiceReverseType::Scenario scenario)
{
    if (errCode == ERR_OK) {
        return;
    }
    if (!IsReportFileReadyFail(bundleName)) {
        return;
    }
    std::string fileNameReport = std::string("fileName:\"") + GetAnonyPath(fileName) + "\"";
    AppRadar::Info info(bundleName, "", fileNameReport);
    if (scenario == IServiceReverseType::Scenario::RESTORE) {
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::FileReadyRadarReport",
            GetUserIdDefault(), BizStageRestore::BIZ_STAGE_GET_FILE_HANDLE_FAIL, errCode);
    } else if (scenario == IServiceReverseType::Scenario::BACKUP) {
        AppRadar::GetInstance().RecordBackupFuncRes(info, "Service::FileReadyRadarReport",
            GetUserIdDefault(), BizStageBackup::BIZ_STAGE_DO_BACKUP, errCode);
    }
}

void Service::ExtensionConnectFailRadarReport(const std::string &bundleName, const ErrCode errCode,
    const IServiceReverseType::Scenario scenario)
{
    if (!IsReportBundleExecFail(bundleName)) {
        return;
    }
    UpdateBundleRadarReport(bundleName);
    std::stringstream ss;
    ss << "errCode:" << errCode;
    AppRadar::Info info(bundleName, "", ss.str());
    if (scenario == IServiceReverseType::Scenario::RESTORE) {
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::ExtensionConnectFailRadarReport",
            GetUserIdDefault(), BizStageRestore::BIZ_STAGE_CONNECT_EXTENSION_FAIL,
            BError(BError::Codes::SA_BOOT_EXT_FAIL).GetCode());
    } else if (scenario == IServiceReverseType::Scenario::BACKUP) {
        AppRadar::GetInstance().RecordBackupFuncRes(info, "Service::ExtensionConnectFailRadarReport",
            GetUserIdDefault(), BizStageBackup::BIZ_STAGE_CONNECT_EXTENSION_FAIL,
            BError(BError::Codes::SA_BOOT_EXT_FAIL).GetCode());
    }
}

void Service::OnStart()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("SA OnStart Begin.");
    std::vector<std::string> bundleNameList;
    if (disposal_ != nullptr) {
        bundleNameList = disposal_->GetBundleNameFromConfigFile();
    }
    std::vector<std::string> residualBundleNameList;
    if (clearRecorder_ != nullptr) {
        residualBundleNameList = clearRecorder_->GetAllClearBundleRecords();
    }
    if (!bundleNameList.empty() || !residualBundleNameList.empty()) {
        if (!bundleNameList.empty()) {
            OnStartResRadarReport(bundleNameList, static_cast<int32_t>(BizStageBackup::BIZ_STAGE_ONSTART_DISPOSE));
        }
        if (!residualBundleNameList.empty()) {
            OnStartResRadarReport(residualBundleNameList,
                static_cast<int32_t>(BizStageBackup::BIZ_STAGE_ONSTART_RESIDUAL));
        }
        SetOccupySession(true);
        session_->Active(
            {
                .clientToken = IPCSkeleton::GetCallingTokenID(),
                .scenario = IServiceReverseType::Scenario::CLEAN,
                .clientProxy = nullptr,
                .userId = GetUserIdDefault(),
                .callerName = "BackupSA",
                .activeTime = TimeUtils::GetCurrentTime(),
            },
            isOccupyingSession_.load());
        HILOGE("SA OnStart, cleaning up backup data");
    }
    bool res = SystemAbility::Publish(sptr(this));
    if (sched_ != nullptr) {
        sched_->StartTimer();
    }
    ClearDisposalOnSaStart();
    auto ret = AppendBundlesClearSession(residualBundleNameList);
    if (isOccupyingSession_.load() && ret) {
        SetOccupySession(false);
        StopAll(nullptr, true);
    }
    HILOGI("SA OnStart End, res = %{public}d", res);
}

void Service::SetOccupySession(bool isOccupyingSession)
{
    isOccupyingSession_.store(isOccupyingSession);
}

void Service::OnStop()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("SA OnStop Begin.");
    int32_t oldMemoryParaSize = BConstants::DEFAULT_VFS_CACHE_PRESSURE;
    if (session_ != nullptr) {
        oldMemoryParaSize = session_->GetMemParaCurSize();
    }
    StorageMgrAdapter::UpdateMemPara(oldMemoryParaSize);
    HILOGI("SA OnStop End.");
}

ErrCode Service::GetLocalCapabilities(int& fd)
{
    UniqueFd fdResult(GetLocalCapabilities());
    fd = fdResult.Release();
    return BError(BError::Codes::OK);
}

UniqueFd Service::GetLocalCapabilities()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        /*
         Only called by restore app before InitBackupSession,
           so there must be set init userId.
        */
        HILOGI("Begin");
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("GetLocalCapabilities error, session is empty.");
            return UniqueFd(-EPERM);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        ErrCode errCode = VerifyCaller();
        if (errCode != ERR_OK) {
            HILOGE("Get local abilities failed, Verify caller failed, errCode:%{public}d", errCode);
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return UniqueFd(-EPERM);
        }
        string path = BConstants::GetSaBundleBackupRootDir(GetUserIdDefault());
        BExcepUltils::VerifyPath(path, false);
        CreateDirIfNotExist(path);
        UniqueFd fd(open(path.data(), O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR));
        if (fd < 0) {
            HILOGE("Failed to open config file = %{public}s, err = %{public}d", GetAnonyPath(path).c_str(), errno);
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return UniqueFd(-EPERM);
        }
        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(std::move(fd));

        auto cache = cachedEntity.Structuralize();
        std::string backupVersion = BJsonUtil::ParseBackupVersion();
        cache.SetBackupVersion(backupVersion);
        cache.SetSystemFullName(GetOSFullName());
        cache.SetDeviceType(GetDeviceType());
        auto bundleInfos = BundleMgrAdapter::GetFullBundleInfos(GetUserIdDefault());
        cache.SetBundleInfos(bundleInfos);
        cachedEntity.Persist();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("End");
        return move(cachedEntity.GetFd());
    } catch (const BError &e) {
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGE("GetLocalCapabilities failed, errCode = %{public}d", e.GetCode());
        return UniqueFd(-e.GetCode());
    } catch (...) {
        HILOGE("Unexpected exception");
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return UniqueFd(-EPERM);
    }
}

void Service::StopAll(const wptr<IRemoteObject> &obj, bool force)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Begin Stop session");
    std::lock_guard<std::mutex> lock(failedBundlesLock_);
    uint32_t fail_cnt = failedBundles_.size();
    uint32_t totalBundles = fail_cnt + successBundlesNum_.load();
    if (totalBundles != 0) {
        int32_t result = 0;
        if (fail_cnt != 0) {
            result = BError::BackupErrorCode::E_TASKFAIL;
        }
        std::stringstream ss;
        ss << "successBundleNum:" << successBundlesNum_ << "," << "failedBundleNum:" <<
            fail_cnt << "," << "failedBundles:{";
        for (const auto &failBundle : failedBundles_) {
            ss << "\"" << failBundle.first << "\":" << "{errCode:" << failBundle.second.errCode << ","
                << "reportTime:" << failBundle.second.reportTime << "},";
        }
        ss << "}";
        string resultInfo = ss.str();
        AppRadar::StatInfo statInfo("", resultInfo);
        IServiceReverseType::Scenario scenario = session_->GetScenario();
        AppRadar::GetInstance().RecordStatisticRes(statInfo, GetUserIdDefault(), scenario,
                                                   successBundlesNum_.load(), fail_cnt, result);
    }
    failedBundles_.clear();
    successBundlesNum_ = 0;
    session_->Deactive(obj, force);
}

void Service::PermissionCheckFailRadar(const std::string &info, const std::string &func)
{
    std::string funcPos = "Service::";
    AppRadar::Info resInfo("", "", info);
    AppRadar::GetInstance().RecordDefaultFuncRes(resInfo, funcPos.append(func),
                                                 GetUserIdDefault(), BizStageBackup::BIZ_STAGE_PERMISSION_CHECK_FAIL,
                                                 BError(BError::Codes::SA_REFUSED_ACT).GetCode());
}

ErrCode Service::VerifyCallerAndGetCallerName(std::string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        Security::AccessToken::HapTokenInfo hapTokenInfo;
        if (Security::AccessToken::AccessTokenKit::GetHapTokenInfo(tokenCaller, hapTokenInfo) != 0) {
            PermissionCheckFailRadar("Get hap token info failed", "VerifyCallerAndGetCallerName");
            HILOGE("Verify and get caller name failed, Get hap token info failed");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        std::string bundleNameIndexInfo =
            BJsonUtil::BuildBundleNameIndexInfo(hapTokenInfo.bundleName, hapTokenInfo.instIndex);
        ErrCode ret = session_->VerifyBundleName(bundleNameIndexInfo);
        if (ret != ERR_OK) {
            HILOGE("Verify bundle name failed, bundleNameIndexInfo:%{public}s", bundleNameIndexInfo.c_str());
            return ret;
        }
        bundleName = bundleNameIndexInfo;
        return BError(BError::Codes::OK);
    } else {
        string str = to_string(tokenCaller);
        HILOGE("tokenID = %{private}s", GetAnonyString(str).c_str());
        std::string info = string("Invalid token type").append(to_string(tokenType)).append(string("\"}"));
        PermissionCheckFailRadar(info, "VerifyCallerAndGetCallerName");
        HILOGE("Verify and get caller name failed, Invalid token type");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
}

ErrCode Service::VerifyCaller()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
    ErrCode ret = BError(BError::Codes::OK);
    switch (tokenType) {
        case Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE: { /* Update Service */
            if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, BACKUP_PERMISSION) !=
                Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
                HILOGE("Permission denied, token type is token native");
                std::string info = "Permission denied, token type is " + to_string(tokenType);
                PermissionCheckFailRadar(info, "VerifyCaller");
                ret = BError(BError::Codes::SA_REFUSED_ACT);
            }
            break;
        }
        case Security::AccessToken::ATokenTypeEnum::TOKEN_HAP: {
            if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, BACKUP_PERMISSION) !=
                Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
                HILOGE("Permission denied, token type is token hap");
                std::string info = "Permission denied, token type is " + to_string(tokenType);
                PermissionCheckFailRadar(info, "VerifyCaller");
                ret = BError(BError::Codes::SA_REFUSED_ACT);
            }
            uint64_t fullTokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
            if (!Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(fullTokenId)) {
                HILOGE("Permission denied, token type is token hap, full tokenId is error");
                std::string info = "Permission denied, token type is " + to_string(tokenType);
                PermissionCheckFailRadar(info, "VerifyCaller");
                ret = BError(BError::Codes::SA_REFUSED_ACT);
            }
            break;
        }
        case Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL:
            if (IPCSkeleton::GetCallingUid() != BConstants::SYSTEM_UID) {
                HILOGE("Permission denied, token type is token shell");
                std::string info = "invalid calling uid";
                PermissionCheckFailRadar(info, "VerifyCaller");
                ret = BError(BError::Codes::SA_REFUSED_ACT);
            }
            break;
        default:
            std::string info = "Permission denied, token type is " + to_string(tokenType);
            PermissionCheckFailRadar(info, "VerifyCaller");
            HILOGE("Permission denied, token type is default");
            ret = BError(BError::Codes::SA_REFUSED_ACT);
            break;
    }
    return ret;
}

ErrCode Service::VerifyCaller(IServiceReverseType::Scenario scenario)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    ErrCode ret = session_->VerifyCallerAndScenario(IPCSkeleton::GetCallingTokenID(), scenario);
    if (ret != ERR_OK) {
        HILOGE("Verify bundle by scenario failed, ret:%{public}d", ret);
        return ret;
    }
    return VerifyCaller();
}

ErrCode Service::InitRestoreSession(const sptr<IServiceReverse> &remote)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    totalStatistic_ = std::make_shared<RadarTotalStatistic>(BizScene::RESTORE, GetCallerName());
    ErrCode ret = VerifyCaller();
    if (ret != ERR_OK) {
        HILOGE("Init restore session failed, verify caller failed");
        totalStatistic_->Report("InitRestoreSession", ret, MODULE_INIT);
        return ret;
    }
    ret = session_->Active({
        .clientToken = IPCSkeleton::GetCallingTokenID(),
        .scenario = IServiceReverseType::Scenario::RESTORE,
        .clientProxy = remote,
        .userId = GetUserIdDefault(),
        .callerName = GetCallerName(),
        .activeTime = TimeUtils::GetCurrentTime(),
    });
    if (ret == ERR_OK) {
        ClearFailedBundles();
        successBundlesNum_ = 0;
        ClearBundleRadarReport();
        ClearFileReadyRadarReport();
        return ret;
    }
    totalStatistic_->Report("InitRestoreSession", ret, MODULE_INIT);
    if (ret == BError(BError::Codes::SA_SESSION_CONFLICT)) {
        HILOGE("Active restore session error, Already have a session");
        return ret;
    }
    HILOGE("Active restore session error");
    StopAll(nullptr, true);
    return ret;
}

ErrCode Service::InitBackupSession(const sptr<IServiceReverse> &remote)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    totalStatistic_ = std::make_shared<RadarTotalStatistic>(BizScene::BACKUP, GetCallerName());
    ErrCode ret = VerifyCaller();
    if (ret != ERR_OK) {
        HILOGE("Init full backup session fail, verify caller failed");
        totalStatistic_->Report("InitBackupSession", ret, MODULE_INIT);
        return ret;
    }
    int32_t oldSize = StorageMgrAdapter::UpdateMemPara(BConstants::BACKUP_VFS_CACHE_PRESSURE);
    HILOGI("InitBackupSession oldSize %{public}d", oldSize);
    session_->SetMemParaCurSize(oldSize);
    ret = session_->Active({
        .clientToken = IPCSkeleton::GetCallingTokenID(),
        .scenario = IServiceReverseType::Scenario::BACKUP,
        .clientProxy = remote,
        .userId = GetUserIdDefault(),
        .callerName = GetCallerName(),
        .activeTime = TimeUtils::GetCurrentTime(),
    });
    if (ret == ERR_OK) {
        ClearFailedBundles();
        successBundlesNum_ = 0;
        ClearBundleRadarReport();
        ClearFileReadyRadarReport();
        return ret;
    }
    totalStatistic_->Report("InitBackupSession", ret, MODULE_INIT);
    if (ret == BError(BError::Codes::SA_SESSION_CONFLICT)) {
        HILOGE("Active backup session error, Already have a session");
        return ret;
    }
    HILOGE("Active backup session error");
    StopAll(nullptr, true);
    return ret;
}

ErrCode Service::Start()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    ErrCode ret = VerifyCaller(session_->GetScenario());
    if (ret != ERR_OK) {
        HILOGE("Service start failed, Verify caller failed, ret:%{public}d", ret);
        return ret;
    }
    ret = session_->Start();
    if (ret != ERR_OK) {
        HILOGE("Service start failed, session is invalid, ret:%{public}d", ret);
        return ret;
    }
    OnStartSched();
    return BError(BError::Codes::OK);
}

static bool SpecialVersion(const string &versionName)
{
    string versionNameFlag = versionName.substr(0, versionName.find_first_of(BConstants::VERSION_NAME_SEPARATOR_CHAR));
    auto iter = find_if(BConstants::DEFAULT_VERSION_NAMES_VEC.begin(), BConstants::DEFAULT_VERSION_NAMES_VEC.end(),
                        [&versionNameFlag](const auto &version) { return version == versionNameFlag; });
    if (iter != BConstants::DEFAULT_VERSION_NAMES_VEC.end()) {
        return true;
    }
    return false;
}

void Service::OnBundleStarted(BError error, sptr<SvcSessionManager> session, const BundleName &bundleName)
{
    IServiceReverseType::Scenario scenario = session->GetScenario();
    if (scenario == IServiceReverseType::Scenario::RESTORE && BackupPara().GetBackupOverrideIncrementalRestore() &&
        session->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
        session->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(error, bundleName);
    } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
        session->GetServiceReverseProxy()->RestoreOnBundleStarted(error, bundleName);
    }
    BundleBeginRadarReport(bundleName, error.GetCode(), scenario);
}

vector<BJsonEntityCaps::BundleInfo> Service::GetRestoreBundleNames(UniqueFd fd, sptr<SvcSessionManager> session,
    const vector<BundleName> &bundleNames, std::string &oldBackupVersion)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    // BundleMgrAdapter::GetBundleInfos可能耗时
    GetBundleInfoStart();
    auto restoreInfos = BundleMgrAdapter::GetBundleInfos(bundleNames, session->GetSessionUserId());
    GetBundleInfoEnd();
    BJsonCachedEntity<BJsonEntityCaps> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    oldBackupVersion = cache.GetBackupVersion();
    if (oldBackupVersion.empty()) {
        HILOGE("backupVersion of old device is empty");
    }
    auto bundleInfos = cache.GetBundleInfos();
    if (!bundleInfos.size()) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Json entity caps is empty");
    }
    HILOGI("restoreInfos size is:%{public}zu", restoreInfos.size());
    vector<BJsonEntityCaps::BundleInfo> restoreBundleInfos {};
    for (const auto &restoreInfo : restoreInfos) {
        if (SAUtils::IsSABundleName(restoreInfo.name)) {
            BJsonEntityCaps::BundleInfo info = {.name = restoreInfo.name,
                                                .appIndex = restoreInfo.appIndex,
                                                .versionCode = restoreInfo.versionCode,
                                                .versionName = restoreInfo.versionName,
                                                .spaceOccupied = restoreInfo.spaceOccupied,
                                                .allToBackup = restoreInfo.allToBackup,
                                                .fullBackupOnly = restoreInfo.fullBackupOnly,
                                                .extensionName = restoreInfo.extensionName,
                                                .restoreDeps = restoreInfo.restoreDeps};
            restoreBundleInfos.emplace_back(info);
            continue;
        }
        auto it = find_if(bundleInfos.begin(), bundleInfos.end(), [&restoreInfo](const auto &obj) {
            return obj.name == restoreInfo.name && obj.appIndex == restoreInfo.appIndex;
        });
        if (it == bundleInfos.end()) {
            HILOGE("Bundle not need restore, bundleName is %{public}s.", restoreInfo.name.c_str());
            continue;
        }
        BJsonEntityCaps::BundleInfo info = {.name = (*it).name,
                                            .appIndex = (*it).appIndex,
                                            .versionCode = (*it).versionCode,
                                            .versionName = (*it).versionName,
                                            .spaceOccupied = (*it).spaceOccupied,
                                            .allToBackup = (*it).allToBackup,
                                            .fullBackupOnly = (*it).fullBackupOnly,
                                            .extensionName = restoreInfo.extensionName,
                                            .restoreDeps = restoreInfo.restoreDeps};
        restoreBundleInfos.emplace_back(info);
    }
    HILOGI("restoreBundleInfos size is:%{public}zu", restoreInfos.size());
    return restoreBundleInfos;
}

void Service::HandleExceptionOnAppendBundles(sptr<SvcSessionManager> session,
                                             const vector<BundleName> &appendBundleNames,
                                             const vector<BundleName> &restoreBundleNames)
{
    if (appendBundleNames.size() != restoreBundleNames.size()) {
        HILOGE("AppendBundleNames not equal restoreBundleNames, appendBundleNames size:%{public}zu,"
            "restoreBundleNames size:%{public}zu", appendBundleNames.size(), restoreBundleNames.size());
        for (const auto &bundleName : appendBundleNames) {
            auto it = find_if(restoreBundleNames.begin(), restoreBundleNames.end(),
                              [&bundleName](const auto &obj) { return obj == bundleName; });
            if (it == restoreBundleNames.end()) {
                HILOGE("AppendBundles failed, bundleName = %{public}s.", bundleName.c_str());
                AppStatReportErr(bundleName, "HandleExceptionOnAppendBundles",
                    RadarError(MODULE_BMS, BError(BError::Codes::SA_BUNDLE_INFO_EMPTY)));
                OnBundleStarted(BError(BError::Codes::SA_BUNDLE_INFO_EMPTY), session, bundleName);
            }
        }
    }
}

ErrCode Service::AppendBundlesRestoreSessionDataByDetail(int fd, const std::vector<std::string> &bundleNames,
                                                         const std::vector<std::string> &detailInfos,
                                                         int32_t restoreType, int32_t userId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Begin fd = %{public}d,restoreType = %{public}d,userId=%{public}d", fd, restoreType, userId);
    UniqueFd fdUnique(fd);
    RestoreTypeEnum restoreTypeEnum = static_cast<RestoreTypeEnum>(restoreType);
    return AppendBundlesRestoreSession(std::move(fdUnique), bundleNames, detailInfos, restoreTypeEnum, userId);
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd,
                                             const vector<BundleName> &bundleNames,
                                             const std::vector<std::string> &bundleInfos,
                                             RestoreTypeEnum restoreType,
                                             int32_t userId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    TotalStart();
    HILOGI("Begin");
    try {
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("AppendBundles restore session with infos error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        CounterHelper counterHelper(session_, __PRETTY_FUNCTION__);
        SetUserIdAndRestoreType(restoreType, userId);
        ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::RESTORE);
        if (ret != ERR_OK) {
            HILOGE("AppendBundles restore session with infos error, verify caller failed, ret:%{public}d", ret);
            HandleExceptionOnAppendBundles(session_, bundleNames, {});
            return ret;
        }
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, bool> isClearDataFlags;
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap =
            BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly, session_->GetSessionUserId(),
            isClearDataFlags);
        std::string oldBackupVersion;
        auto restoreInfos = GetRestoreBundleNames(move(fd), session_, bundleNames, oldBackupVersion);
        auto restoreBundleNames = SvcRestoreDepsManager::GetInstance().GetRestoreBundleNames(restoreInfos, restoreType);
        HandleExceptionOnAppendBundles(session_, bundleNames, restoreBundleNames);
        if (restoreBundleNames.empty()) {
            HILOGE("AppendBundlesRestoreSession failed, restoreBundleNames is empty.");
            return BError(BError::Codes::OK);
        }
        AppendBundles(restoreBundleNames);
        SetCurrentSessProperties(restoreInfos, restoreBundleNames, bundleNameDetailMap, isClearDataFlags, restoreType,
            oldBackupVersion);
        OnStartSched();
        HILOGI("End");
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("Catch exception");
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        return e.GetCode();
    } catch (...) {
        HILOGE("Unexpected exception");
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        return EPERM;
    }
}

void Service::SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo> &restoreBundleInfos,
                                       std::vector<std::string> &restoreBundleNames,
                                       RestoreTypeEnum restoreType, std::string &backupVersion)
{
    HILOGI("Start");
    session_->SetOldBackupVersion(backupVersion);
    for (const auto &restoreInfo : restoreBundleInfos) {
        auto it = find_if(restoreBundleNames.begin(), restoreBundleNames.end(), [&restoreInfo](const auto &bundleName) {
            std::string bundleNameIndex = BJsonUtil::BuildBundleNameIndexInfo(restoreInfo.name, restoreInfo.appIndex);
            return bundleName == bundleNameIndex;
        });
        if (it == restoreBundleNames.end()) {
            HILOGE("Can not find current bundle, bundleName:%{public}s, appIndex:%{public}d", restoreInfo.name.c_str(),
                restoreInfo.appIndex);
            continue;
        }
        HILOGI("bundleName: %{public}s, extensionName: %{public}s", restoreInfo.name.c_str(),
            restoreInfo.extensionName.c_str());
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(restoreInfo.name, restoreInfo.appIndex);
        if ((!restoreInfo.allToBackup && !SpecialVersion(restoreInfo.versionName)) ||
            (restoreInfo.extensionName.empty() && !SAUtils::IsSABundleName(restoreInfo.name))) {
            AppStatReportErr(restoreInfo.name, "SetCurrentSessProperties",
                RadarError(MODULE_BMS, BError(BError::Codes::SA_FORBID_BACKUP_RESTORE)));
            OnBundleStarted(BError(BError::Codes::SA_FORBID_BACKUP_RESTORE), session_, bundleNameIndexInfo);
            session_->RemoveExtInfo(bundleNameIndexInfo);
            continue;
        }
        session_->SetBundleRestoreType(bundleNameIndexInfo, restoreType);
        session_->SetBundleVersionCode(bundleNameIndexInfo, restoreInfo.versionCode);
        session_->SetBundleVersionName(bundleNameIndexInfo, restoreInfo.versionName);
        session_->SetBundleDataSize(bundleNameIndexInfo, restoreInfo.spaceOccupied);
        if (BundleMgrAdapter::IsUser0BundleName(bundleNameIndexInfo, session_->GetSessionUserId())) {
            SendUserIdToApp(bundleNameIndexInfo, session_->GetSessionUserId());
        }
        session_->SetBundleUserId(bundleNameIndexInfo, session_->GetSessionUserId());
        session_->SetBackupExtName(bundleNameIndexInfo, restoreInfo.extensionName);
        session_->SetIsReadyLaunch(bundleNameIndexInfo);
    }
    HILOGI("End");
}
ErrCode Service::AppendBundlesRestoreSessionData(int fd, const std::vector<std::string> &bundleNames,
                                                 int32_t restoreType, int32_t userId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Begin fd = %{public}d,restoreType = %{public}d,userId=%{public}d", fd, restoreType, userId);
    UniqueFd fdUnique(fd);
    RestoreTypeEnum restoreTypeEnum = static_cast<RestoreTypeEnum>(restoreType);
    return AppendBundlesRestoreSession(std::move(fdUnique), bundleNames, restoreTypeEnum, userId);
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd,
                                             const vector<BundleName> &bundleNames,
                                             RestoreTypeEnum restoreType,
                                             int32_t userId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    TotalStart();
    try {
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("AppendBundles restore session error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        SetUserIdAndRestoreType(restoreType, userId);
        ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::RESTORE);
        if (ret != ERR_OK) {
            HILOGE("AppendBundles restore session with infos error, verify caller failed, ret:%{public}d", ret);
            HandleExceptionOnAppendBundles(session_, bundleNames, {});
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return ret;
        }
        std::string oldBackupVersion;
        auto restoreInfos = GetRestoreBundleNames(move(fd), session_, bundleNames, oldBackupVersion);
        auto restoreBundleNames = SvcRestoreDepsManager::GetInstance().GetRestoreBundleNames(restoreInfos, restoreType);
        HandleExceptionOnAppendBundles(session_, bundleNames, restoreBundleNames);
        if (restoreBundleNames.empty()) {
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            HILOGW("RestoreBundleNames is empty.");
            return BError(BError::Codes::OK);
        }
        AppendBundles(restoreBundleNames);
        SetCurrentSessProperties(restoreInfos, restoreBundleNames, restoreType, oldBackupVersion);
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("Catch exception");
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return e.GetCode();
    } catch (...) {
        HILOGE("Unexpected exception");
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

void Service::SetCurrentSessProperties(
    std::vector<BJsonEntityCaps::BundleInfo> &restoreBundleInfos,
    std::vector<std::string> &restoreBundleNames,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> &bundleNameDetailMap,
    std::map<std::string, bool> &isClearDataFlags, RestoreTypeEnum restoreType, std::string &backupVersion)
{
    HILOGI("Start");
    session_->SetOldBackupVersion(backupVersion);
    for (const auto &restoreInfo : restoreBundleInfos) {
        auto it = find_if(restoreBundleNames.begin(), restoreBundleNames.end(),
            [&restoreInfo](const auto &bundleName) {
            std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(restoreInfo.name,
                restoreInfo.appIndex);
            return bundleName == bundleNameIndexInfo;
        });
        if (it == restoreBundleNames.end()) {
            HILOGE("Can not find current bundle, bundleName:%{public}s, appIndex:%{public}d", restoreInfo.name.c_str(),
                   restoreInfo.appIndex);
            continue;
        }
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(restoreInfo.name, restoreInfo.appIndex);
        if ((!restoreInfo.allToBackup && !SpecialVersion(restoreInfo.versionName)) ||
            (restoreInfo.extensionName.empty() && !SAUtils::IsSABundleName(restoreInfo.name))) {
            AppStatReportErr(restoreInfo.name, "SetCurrentSessProperties",
                RadarError(MODULE_BMS, BError(BError::Codes::SA_FORBID_BACKUP_RESTORE)));
            OnBundleStarted(BError(BError::Codes::SA_FORBID_BACKUP_RESTORE), session_, bundleNameIndexInfo);
            session_->RemoveExtInfo(bundleNameIndexInfo);
            continue;
        }
        session_->SetBundleRestoreType(bundleNameIndexInfo, restoreType);
        session_->SetBundleVersionCode(bundleNameIndexInfo, restoreInfo.versionCode);
        session_->SetBundleVersionName(bundleNameIndexInfo, restoreInfo.versionName);
        session_->SetBundleDataSize(bundleNameIndexInfo, restoreInfo.spaceOccupied);
        session_->SetBundleUserId(bundleNameIndexInfo, session_->GetSessionUserId());
        auto iter = isClearDataFlags.find(bundleNameIndexInfo);
        if (iter != isClearDataFlags.end()) {
            session_->SetClearDataFlag(bundleNameIndexInfo, iter->second);
        }
        BJsonUtil::BundleDetailInfo broadCastInfo;
        BJsonUtil::BundleDetailInfo uniCastInfo;
        bool broadCastRet =
            BJsonUtil::FindBundleInfoByName(bundleNameDetailMap, bundleNameIndexInfo, BROADCAST_TYPE, broadCastInfo);
        if (broadCastRet) {
            bool notifyRet = DelayedSingleton<NotifyWorkService>::GetInstance()->NotifyBundleDetail(broadCastInfo);
            HILOGI("Publish event end, notify result is:%{public}d", notifyRet);
        }
        bool uniCastRet =
            BJsonUtil::FindBundleInfoByName(bundleNameDetailMap, bundleNameIndexInfo, UNICAST_TYPE, uniCastInfo);
        if (uniCastRet) {
            HILOGI("current bundle, unicast info:%{public}s", GetAnonyString(uniCastInfo.detail).c_str());
            session_->SetBackupExtInfo(bundleNameIndexInfo, uniCastInfo.detail);
        }
        session_->SetBackupExtName(bundleNameIndexInfo, restoreInfo.extensionName);
        session_->SetIsReadyLaunch(bundleNameIndexInfo);
    }
    HILOGI("End");
}

ErrCode Service::AppendBundlesBackupSession(const vector<BundleName> &bundleNames)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    TotalStart();
    try {
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("AppendBundles backup session error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__); // BundleMgrAdapter::GetBundleInfos可能耗时
        ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::BACKUP);
        if (ret != ERR_OK) {
            HILOGE("AppendBundles backup session error, verify caller failed, ret:%{public}d", ret);
            HandleExceptionOnAppendBundles(session_, bundleNames, {});
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return ret;
        }
        auto bundleDetails = MakeDetailList(bundleNames);
        GetBundleInfoStart();
        auto backupInfos = BundleMgrAdapter::GetBundleInfosForAppendBundles(bundleDetails,
            session_->GetSessionUserId());
        GetBundleInfoEnd();
        std::vector<std::string> supportBackupNames = GetSupportBackupBundleNames(backupInfos, false, bundleNames);
        AppendBundles(supportBackupNames);
        SetCurrentBackupSessProperties(supportBackupNames, session_->GetSessionUserId(), backupInfos, false);
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("Failed, errCode = %{public}d", e.GetCode());
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return e.GetCode();
    } catch (...) {
        HILOGE("Unexpected exception");
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

ErrCode Service::AppendBundlesDetailsBackupSession(const vector<BundleName> &bundleNames,
                                                   const vector<std::string> &bundleInfos)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    TotalStart();
    try {
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("AppendBundles backup session with infos error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__); // BundleMgrAdapter::GetBundleInfos可能耗时
        ErrCode ret = VerifyCaller(IServiceReverseType::Scenario::BACKUP);
        if (ret != ERR_OK) {
            HILOGE("AppendBundles backup session with infos error, verify caller failed, ret:%{public}d", ret);
            HandleExceptionOnAppendBundles(session_, bundleNames, {});
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return ret;
        }
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, bool> isClearDataFlags;
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap =
            BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly,
            session_->GetSessionUserId(), isClearDataFlags);
        auto bundleDetails = MakeDetailList(bundleNames);
        GetBundleInfoStart();
        auto backupInfos = BundleMgrAdapter::GetBundleInfosForAppendBundles(bundleDetails,
            session_->GetSessionUserId());
        GetBundleInfoEnd();
        std::vector<std::string> supportBackupNames = GetSupportBackupBundleNames(backupInfos, false, bundleNames);
        AppendBundles(supportBackupNames);
        HandleCurGroupBackupInfos(backupInfos, bundleNameDetailMap, isClearDataFlags);
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("Failed, errCode = %{public}d", e.GetCode());
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return e.GetCode();
    } catch(...) {
        HILOGE("Unexpected exception");
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

void Service::HandleCurGroupBackupInfos(
    std::vector<BJsonEntityCaps::BundleInfo> &backupInfos,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> &bundleNameDetailMap,
    std::map<std::string, bool> &isClearDataFlags)
{
    for (auto &info : backupInfos) {
        HILOGI("Current backupInfo bundleName:%{public}s, extName:%{public}s, appIndex:%{public}d", info.name.c_str(),
               info.extensionName.c_str(), info.appIndex);
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(info.name, info.appIndex);
        SetCurrentSessProperties(info, isClearDataFlags, bundleNameIndexInfo);
        BJsonUtil::BundleDetailInfo uniCastInfo;
        if (BJsonUtil::FindBundleInfoByName(bundleNameDetailMap, bundleNameIndexInfo, UNICAST_TYPE, uniCastInfo)) {
            HILOGI("current bundle:%{public}s, unicast info:%{public}s, unicast info size:%{public}zu",
                   bundleNameIndexInfo.c_str(), GetAnonyString(uniCastInfo.detail).c_str(), uniCastInfo.detail.size());
            session_->SetBackupExtInfo(bundleNameIndexInfo, uniCastInfo.detail);
        }
        session_->SetBundleUserId(bundleNameIndexInfo, session_->GetSessionUserId());
        session_->SetBackupExtName(bundleNameIndexInfo, info.extensionName);
        session_->SetIsReadyLaunch(bundleNameIndexInfo);
    }
}

ErrCode Service::ServiceResultReport(const std::string& restoreRetInfo, BackupRestoreScenario sennario, ErrCode errCode)
{
    string callerName;
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        ErrCode ret = VerifyCallerAndGetCallerName(callerName);
        if (ret != ERR_OK) {
            HILOGE("Result report fail, bundleName:%{public}s, ret:%{public}d", callerName.c_str(), ret);
            HandleCurBundleEndWork(callerName, sennario);
            CallOnBundleEndByScenario(callerName, sennario, ret);
            OnAllBundlesFinished(BError(BError::Codes::OK));
            return ret;
        }
        SendEndAppGalleryNotify(callerName);
        if (sennario == BackupRestoreScenario::FULL_RESTORE) {
            HandleCurBundleEndWork(callerName, sennario);
            session_->GetServiceReverseProxy()->RestoreOnResultReport(restoreRetInfo, callerName, errCode);
            OnAllBundlesFinished(BError(BError::Codes::OK));
        } else if (sennario == BackupRestoreScenario::INCREMENTAL_RESTORE) {
            HandleCurBundleEndWork(callerName, sennario);
            session_->GetServiceReverseProxy()->IncrementalRestoreOnResultReport(restoreRetInfo, callerName, errCode);
            OnAllBundlesFinished(BError(BError::Codes::OK));
        } else if (sennario == BackupRestoreScenario::FULL_BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnResultReport(restoreRetInfo, callerName);
        } else if (sennario == BackupRestoreScenario::INCREMENTAL_BACKUP) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnResultReport(restoreRetInfo, callerName);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("Service result report error");
        HandleCurBundleEndWork(callerName, sennario);
        CallOnBundleEndByScenario(callerName, sennario, e.GetCode());
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return e.GetCode(); // 任意异常产生，终止监听该任务
    }
}

ErrCode Service::SAResultReport(const std::string bundleName, const std::string restoreRetInfo,
                                const ErrCode errCode, const BackupRestoreScenario sennario)
{
    SADone(errCode, bundleName);
    if (sennario == BackupRestoreScenario::FULL_RESTORE) {
        session_->GetServiceReverseProxy()->RestoreOnResultReport(restoreRetInfo, bundleName, ERR_OK);
    } else if (sennario == BackupRestoreScenario::INCREMENTAL_RESTORE) {
        session_->GetServiceReverseProxy()->IncrementalRestoreOnResultReport(restoreRetInfo, bundleName, ERR_OK);
    } else if (sennario == BackupRestoreScenario::FULL_BACKUP) {
        session_->GetServiceReverseProxy()->BackupOnResultReport(restoreRetInfo, bundleName);
        session_->GetServiceReverseProxy()->BackupOnBundleFinished(errCode, bundleName);
    } else if (sennario == BackupRestoreScenario::INCREMENTAL_BACKUP) {
        session_->GetServiceReverseProxy()->IncrementalBackupOnResultReport(restoreRetInfo, bundleName);
        session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(errCode, bundleName);
    }
    OnAllBundlesFinished(BError(BError::Codes::OK));
    if (sennario == BackupRestoreScenario::FULL_RESTORE || sennario == BackupRestoreScenario::INCREMENTAL_RESTORE) {
        BundleEndRadarReport(bundleName, errCode, IServiceReverseType::Scenario::RESTORE);
    } else if (sennario == BackupRestoreScenario::FULL_BACKUP ||
               sennario == BackupRestoreScenario::INCREMENTAL_BACKUP) {
        BundleEndRadarReport(bundleName, errCode, IServiceReverseType::Scenario::BACKUP);
    }
    return BError(BError::Codes::OK);
}

void Service::HandleCurBundleEndWork(std::string bundleName, const BackupRestoreScenario sennario)
{
    HILOGI("Begin");
    try {
        if (sennario != BackupRestoreScenario::FULL_RESTORE &&
            sennario != BackupRestoreScenario::INCREMENTAL_RESTORE) {
            return;
        }
        if (session_->OnBundleFileReady(bundleName)) {
            std::shared_ptr<ExtensionMutexInfo> mutexPtr = GetExtensionMutex(bundleName);
            if (mutexPtr == nullptr) {
                HILOGE("extension mutex ptr is nullptr");
                return;
            }
            std::lock_guard<std::mutex> lock(mutexPtr->callbackMutex);
            auto backUpConnection = session_->GetExtConnection(bundleName);
            if (backUpConnection == nullptr) {
                HILOGE("backUpConnection is empty, bundle:%{public}s", bundleName.c_str());
                return;
            }
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                HILOGE("Extension backup Proxy is empty, bundle:%{public}s", bundleName.c_str());
                return;
            }
            proxy->HandleClear();
            session_->StopFwkTimer(bundleName);
            session_->StopExtTimer(bundleName);
            backUpConnection->DisconnectBackupExtAbility();
            ClearSessionAndSchedInfo(bundleName);
        }
        RemoveExtensionMutex(bundleName);
    } catch (...) {
        HILOGE("Unexpected exception");
    }
}

ErrCode Service::LaunchBackupSAExtension(const BundleName &bundleName)
{
    string extInfo = session_->GetBackupExtInfo(bundleName);
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    if (SAUtils::IsSABundleName(bundleName)) {
        auto saBackUpConnection = session_->GetSAExtConnection(bundleName);
        std::shared_ptr<SABackupConnection> saConnection = saBackUpConnection.lock();
        if (saConnection == nullptr) {
            HILOGE("lock sa connection ptr is nullptr");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        if (scenario == IServiceReverseType::Scenario::BACKUP) {
            return saConnection->ConnectBackupSAExt(bundleName, BConstants::EXTENSION_BACKUP, extInfo);
        } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
            return saConnection->ConnectBackupSAExt(bundleName, BConstants::EXTENSION_RESTORE, extInfo);
        }
    }
    return BError(BError::Codes::OK);
}

void Service::ExtStart(const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGE("begin ExtStart, bundle name:%{public}s", bundleName.data());
        if (SAUtils::IsSABundleName(bundleName)) {
            if (session_->GetIsIncrementalBackup()) {
                IncrementalBackupSA(bundleName);
            } else {
                BackupSA(bundleName);
            }
            return;
        }
        if (IncrementalBackup(bundleName)) {
            return;
        }
        StartCurBundleBackupOrRestore(bundleName);
    } catch (...) {
        HILOGE("Unexpected exception, bundleName: %{public}s", bundleName.c_str());
        SendEndAppGalleryNotify(bundleName);
        ClearSessionAndSchedInfo(bundleName);
        NoticeClientFinish(bundleName, BError(BError::Codes::SA_INVAL_ARG));
    }
}

void Service::StartCurBundleBackupOrRestore(const std::string &bundleName)
{
    HILOGI("Begin handle current bundle full backup or full restore, bundleName:%{public}s", bundleName.c_str());
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    auto backUpConnection = session_->GetExtConnection(bundleName);
    if (backUpConnection == nullptr) {
        HILOGE("Error, backUpConnection is empty, bundle:%{public}s", bundleName.c_str());
        ReportOnBundleStarted(scenario, bundleName);
        return;
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (proxy == nullptr) {
        HILOGE("Error, Extension backup Proxy is empty, bundle:%{public}s", bundleName.c_str());
        ReportOnBundleStarted(scenario, bundleName);
        return;
    }
    if (scenario == IServiceReverseType::Scenario::BACKUP) {
        auto ret = proxy->HandleBackup(session_->GetClearDataFlag(bundleName));
        session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
        BundleBeginRadarReport(bundleName, ret, scenario);
        if (ret) {
            SendEndAppGalleryNotify(bundleName);
            ClearSessionAndSchedInfo(bundleName);
            NoticeClientFinish(bundleName, BError(BError::Codes::SA_INVAL_ARG));
        }
    } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
        auto ret = proxy->HandleRestore(session_->GetClearDataFlag(bundleName));
        session_->GetServiceReverseProxy()->RestoreOnBundleStarted(ret, bundleName);
        GetOldDeviceBackupVersion();
        BundleBeginRadarReport(bundleName, ret, scenario);
        auto fileNameVec = session_->GetExtFileNameRequest(bundleName);
        for (const auto &fileName : fileNameVec) {
            int32_t errCode = 0;
            int fdCode = 0;
            proxy->GetFileHandleWithUniqueFd(fileName, errCode, fdCode);
            UniqueFd fd(fdCode);
            session_->GetServiceReverseProxy()->RestoreOnFileReady(bundleName, fileName, move(fd), errCode);
            FileReadyRadarReport(bundleName, fileName, errCode, scenario);
        }
    }
    HILOGI("End handle current bundle full backup or full restore, bundleName:%{public}s", bundleName.c_str());
}

int Service::Dump(int fd, const vector<u16string> &args)
{
    if (fd < 0) {
        HILOGI("HiDumper handle invalid");
        return -1;
    }

    session_->DumpInfo(fd, args);
    return 0;
}

void Service::ReportOnExtConnectFailed(const IServiceReverseType::Scenario scenario,
                                       const std::string &bundleName, const ErrCode ret)
{
    try {
        if (session_ == nullptr) {
            HILOGE("Report extConnectfailed error, session info is empty");
            return;
        }
        if (scenario == IServiceReverseType::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnBundleStarted(ret, bundleName);
            BundleBeginRadarReport(bundleName, ret, scenario);
        } else if (scenario == IServiceReverseType::Scenario::RESTORE &&
                   BackupPara().GetBackupOverrideIncrementalRestore() &&
                   session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(ret, bundleName);
            BundleBeginRadarReport(bundleName, ret, scenario);
            DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->EndRestore(bundleName,
                session_->GetBundleUserId(bundleName));
            HILOGI("ExtConnectFailed EndRestore, code=%{public}d, bundleName=%{public}s", disposeErr,
                   bundleName.c_str());
        } else if (scenario == IServiceReverseType::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
            BundleBeginRadarReport(bundleName, ret, scenario);
        } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnBundleStarted(ret, bundleName);
            BundleBeginRadarReport(bundleName, ret, scenario);
            DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->EndRestore(bundleName,
                session_->GetBundleUserId(bundleName));
            HILOGI("ExtConnectFailed EndRestore, code=%{public}d, bundleName=%{public}s", disposeErr,
                   bundleName.c_str());
        }
    } catch (...) {
        HILOGE("Report extConnectfailed error");
    }
}

void Service::ExtConnectFailed(const string &bundleName, ErrCode ret)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    IServiceReverseType::Scenario scenario = IServiceReverseType::Scenario::UNDEFINED;
    try {
        HILOGE("begin %{public}s", bundleName.data());
        scenario = session_->GetScenario();
        ReportOnExtConnectFailed(scenario, bundleName, ret);
        SendEndAppGalleryNotify(bundleName);
        ClearSessionAndSchedInfo(bundleName);
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return;
    } catch (const BError &e) {
        HILOGE("ExtConnectFailed exception, bundleName:%{public}s", bundleName.c_str());
        return;
    } catch (...) {
        HILOGE("Unexpected exception");
        return;
    }
}

void Service::StartRunningTimer(const std::string &bundleName)
{
    auto timeoutCallback = TimeOutCallback(wptr<Service>(this), bundleName);
    auto scenario = session_->GetScenario();
    if (scenario == IServiceReverseType::Scenario::BACKUP) {
        session_->StartExtTimer(bundleName, timeoutCallback);
    } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
        session_->StartFwkTimer(bundleName, timeoutCallback);
    }
}

void Service::ExtConnectDone(string bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGE("begin %{public}s", bundleName.data());
        BConstants::ServiceSchedAction curSchedAction = session_->GetServiceSchedAction(bundleName);
        if (curSchedAction == BConstants::ServiceSchedAction::UNKNOWN) {
            HILOGE("Can not find bundle from this session, bundleName:%{public}s", bundleName.c_str());
            SendEndAppGalleryNotify(bundleName);
            ClearSessionAndSchedInfo(bundleName);
            NoticeClientFinish(bundleName, BError(BError::Codes::SA_REFUSED_ACT));
            return;
        }
        if (curSchedAction == BConstants::ServiceSchedAction::CLEAN) {
            HILOGI("Current bundle will execute clean task, bundleName:%{public}s", bundleName.c_str());
            sched_->Sched(bundleName);
            return;
        }
        if (curSchedAction == BConstants::ServiceSchedAction::START &&
            clearRecorder_->FindClearBundleRecord(bundleName)) {
            session_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::CLEAN);
        } else {
            session_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::RUNNING);
            if (totalStatistic_ != nullptr) {
                session_->UpdateDfxInfo(bundleName, totalStatistic_->GetUniqId());
            }
            bool needCleanData = session_->GetClearDataFlag(bundleName);
            if (needCleanData) {
                HILOGI("Current bundle need clean data, bundleName:%{public}s", bundleName.c_str());
                AddClearBundleRecord(bundleName);
            }
        }
        sched_->Sched(bundleName);
    } catch (...) {
        HILOGE("Unexpected exception, bundleName: %{public}s", bundleName.c_str());
        SendEndAppGalleryNotify(bundleName);
        ClearSessionAndSchedInfo(bundleName);
        NoticeClientFinish(bundleName, BError(BError::Codes::SDK_INVAL_ARG));
        return;
    }
}

void Service::ClearSessionAndSchedInfo(const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("begin %{public}s", bundleName.c_str());
        session_->RemoveExtInfo(bundleName);
        sched_->RemoveExtConn(bundleName);
        HandleRestoreDepsBundle(bundleName);
        DelClearBundleRecord({bundleName});
        if (isOccupyingSession_.load() && session_->IsOnAllBundlesFinished()) {
            HILOGI("Cleaning up backup data end.");
            SetOccupySession(false);
            StopAll(nullptr, true);
            return;
        }
        for (int num = 0; num < BConstants::EXT_CONNECT_MAX_COUNT; num++) {
            sched_->Sched();
        }
    } catch (const BError &e) {
        HILOGE("ClearSessionAndSchedInfo exception");
        return;
    } catch (...) {
        HILOGE("Unexpected exception");
        return;
    }
}

void Service::HandleRestoreDepsBundle(const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (session_->GetScenario() != IServiceReverseType::Scenario::RESTORE) {
        return;
    }
    HILOGI("Begin, bundleName: %{public}s", bundleName.c_str());
    SvcRestoreDepsManager::GetInstance().AddRestoredBundles(bundleName);
    // 该应用恢复完成，判断依赖hap的前置hap是否全部恢复完成，如果成了，追加该依赖hap
    auto restoreBundleMap = SvcRestoreDepsManager::GetInstance().GetRestoreBundleMap();
    if (restoreBundleMap.empty()) {
        HILOGI("restoreBundleMap is empty.");
        return;
    }
    // 启动恢复会话
    vector<string> restoreBundleNames {};
    for (const auto &bundle : restoreBundleMap) {
        HILOGI("Start restore session, bundle: %{public}s", bundle.first.c_str());
        restoreBundleNames.emplace_back(bundle.first);
    }
    AppendBundles(restoreBundleNames);
    for (const auto &bundle : restoreBundleMap) {
        for (const auto &bundleInfo : SvcRestoreDepsManager::GetInstance().GetAllBundles()) {
            if (bundle.first != bundleInfo.name) {
                continue;
            }
            SvcRestoreDepsManager::RestoreInfo info = bundle.second;
            session_->SetBundleRestoreType(bundleInfo.name, info.restoreType_);
            session_->SetBundleVersionCode(bundleInfo.name, bundleInfo.versionCode);
            session_->SetBundleVersionName(bundleInfo.name, bundleInfo.versionName);
            session_->SetBundleDataSize(bundleInfo.name, bundleInfo.spaceOccupied);
            for (const auto &fileName : info.fileNames_) {
                session_->SetExtFileNameRequest(bundleInfo.name, fileName);
            }
            session_->SetBackupExtName(bundleInfo.name, bundleInfo.extensionName);
            session_->SetIsReadyLaunch(bundleInfo.name);
        }
    }
    HILOGI("End");
}

void Service::OnStartSched()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (session_->IsOnOnStartSched()) {
        for (int num = 0; num < BConstants::EXT_CONNECT_MAX_COUNT; num++) {
            sched_->Sched();
        }
    }
}

void Service::SendStartAppGalleryNotify(const BundleName &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (SAUtils::IsSABundleName(bundleName)) {
        HILOGI("SA does not need to StartRestore");
        return;
    }
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    if (scenario != IServiceReverseType::Scenario::RESTORE) {
        return;
    }
    int32_t userId = session_->GetBundleUserId(bundleName);
    std::string bundleNameWithUserId = BundleNameWithUserId(bundleName, userId);
    if (!disposal_->IfBundleNameInDisposalConfigFile(bundleNameWithUserId)) {
        HILOGE("WriteDisposalConfigFile Failed");
        return;
    }
    HILOGI("AppendIntoDisposalConfigFile OK, bundleName=%{public}s", bundleNameWithUserId.c_str());
    DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->StartRestore(bundleName, userId);
    HILOGI("StartRestore, code=%{public}d, bundleName=%{public}s, userId=%{public}d", disposeErr, bundleName.c_str(),
        userId);
}

void Service::SendEndAppGalleryNotify(const BundleName &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (SAUtils::IsSABundleName(bundleName)) {
        HILOGI("SA does not need to EndRestore");
        return;
    }
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    if (scenario != IServiceReverseType::Scenario::RESTORE) {
        return;
    }
    int32_t userId = session_->GetBundleUserId(bundleName);
    DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->EndRestore(bundleName, userId);
    HILOGI("EndRestore, code=%{public}d, bundleName=%{public}s, userId=%{public}d", disposeErr, bundleName.c_str(),
        userId);
    if (disposeErr != DisposeErr::OK) {
        HILOGE("Error code=%{public}d, disposal will be clear in the end", disposeErr);
        return;
    }
    std::string bundleNameWithUserId = BundleNameWithUserId(bundleName, userId);
    if (!disposal_->DeleteFromDisposalConfigFile(bundleNameWithUserId)) {
        HILOGE("DeleteFromDisposalConfigFile Failed, bundleName=%{public}s", bundleNameWithUserId.c_str());
        return;
    }
    HILOGI("DeleteFromDisposalConfigFile OK, bundleName=%{public}s", bundleNameWithUserId.c_str());
}

void Service::TryToClearDispose(const BundleName &bundleName)
{
    auto [bundle, userId] = SplitBundleName(bundleName);
    if (bundle.empty() || userId == -1) {
        HILOGE("BundleName from disposal config is invalid, bundleName = %{public}s", bundleName.c_str());
        if (!disposal_->DeleteFromDisposalConfigFile(bundleName)) {
            HILOGE("DeleteFromDisposalConfigFile Failed, bundleName=%{public}s", bundleName.c_str());
        }
        return;
    }
    int32_t maxAtt = MAX_TRY_CLEAR_DISPOSE_NUM;
    int32_t att = 0;
    while (att < maxAtt) {
        DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->EndRestore(bundle, userId);
        HILOGI("EndRestore, code=%{public}d, bundleName=%{public}s, userId=%{public}d", disposeErr, bundle.c_str(),
            userId);
        if (disposeErr == DisposeErr::OK) {
            break;
        }
        ++att;
        HILOGI("Try to clear dispose, num = %{public}d", att);
    }
    if (!disposal_->DeleteFromDisposalConfigFile(bundleName)) {
        HILOGE("DeleteFromDisposalConfigFile Failed, bundleName=%{public}s", bundleName.c_str());
    }
}

void Service::SendErrAppGalleryNotify()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    if (scenario != IServiceReverseType::Scenario::RESTORE) {
        return;
    }
    vector<string> bundleNameList = disposal_->GetBundleNameFromConfigFile();
    if (bundleNameList.empty()) {
        HILOGI("End, All disposal pasitions have been cleared");
        return;
    }
    for (vector<string>::iterator it = bundleNameList.begin(); it != bundleNameList.end(); ++it) {
        string bundleName = *it;
        TryToClearDispose(bundleName);
    }
}

void Service::ClearDisposalOnSaStart()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    vector<string> bundleNameList = disposal_->GetBundleNameFromConfigFile();
    if (!bundleNameList.empty()) {
        for (vector<string>::iterator it = bundleNameList.begin(); it != bundleNameList.end(); ++it) {
            string bundleName = *it;
            HILOGE("dispose has residual, clear now, bundelName =%{public}s", bundleName.c_str());
            TryToClearDispose(bundleName);
        }
    }
    HILOGI("SA start, All Errdisposal pasitions have been cleared");
}

void Service::DeleteDisConfigFile()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    if (scenario != IServiceReverseType::Scenario::RESTORE) {
        return;
    }
    vector<string> bundleNameList = disposal_->GetBundleNameFromConfigFile();
    if (!bundleNameList.empty()) {
        HILOGE("DisposalConfigFile is not empty");
        return;
    }
    if (!disposal_->DeleteConfigFile()) {
        HILOGE("DeleteConfigFile failed");
    }
}

void Service::SessionDeactive()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin");
        isInRelease_.store(true);
        //清理处置状态
        if (session_ == nullptr) {
            HILOGE("Session deactive error, session is empty");
            return;
        }
        ErrCode ret = BError(BError::Codes::OK);
        std::vector<std::string> bundleNameList;
        if (session_->GetScenario() == IServiceReverseType::Scenario::RESTORE &&
            session_->CleanAndCheckIfNeedWait(ret, bundleNameList)) {
            if (ret != ERR_OK) {
                isRmConfigFile_.store(false);
            }
            if (!bundleNameList.empty()) {
                DelClearBundleRecord(bundleNameList);
            }
            return;
        }
        isInRelease_.store(false);
        if (!bundleNameList.empty()) {
            DelClearBundleRecord(bundleNameList);
        }
        SendErrAppGalleryNotify();
        DeleteDisConfigFile();
        // 结束定时器
        if (sched_ == nullptr) {
            HILOGE("Session deactive error, sched is empty");
            return;
        }
        sched_->ClearSchedulerData();
        // 清除缓存数据
        if (session_ == nullptr) {
            HILOGE("Session deactive error, session is empty");
            return;
        }
        ret = session_->ClearSessionData();
        if (clearRecorder_ != nullptr && !ret && isRmConfigFile_.load()) {
            clearRecorder_->DeleteConfigFile();
        }
        // close session
        StopAll(nullptr, true);
        if (session_->GetSessionCnt() <= 0) {
            HILOGI("do unload Service.");
            sched_->TryUnloadService();
        }
    } catch (...) {
        HILOGE("Unexpected exception");
        return;
    }
}

std::function<void(const std::string &&)> Service::GetBackupInfoConnectDone(wptr<Service> obj,
                                                                            const std::string &bundleName)
{
    return [obj](const string &&bundleName) {
        HILOGI("GetBackupInfoConnectDone, bundleName: %{public}s", bundleName.c_str());
        auto thisPtr = obj.promote();
        if (!thisPtr) {
            HILOGW("this pointer is null.");
            return;
        }
        thisPtr->getBackupInfoCondition_.notify_one();
    };
}

std::function<void(const std::string &&, bool)> Service::GetBackupInfoConnectDied(wptr<Service> obj,
                                                                                  const std::string &bundleName)
{
    return [obj](const string &&bundleName, bool isCleanCalled) {
        HILOGI("GetBackupInfoConnectDied, bundleName: %{public}s", bundleName.c_str());
        auto thisPtr = obj.promote();
        if (!thisPtr) {
            HILOGW("this pointer is null.");
            return;
        }
        thisPtr->isConnectDied_.store(true);
        thisPtr->getBackupInfoCondition_.notify_one();
    };
}

ErrCode Service::ClearResidualBundleData(const std::string &bundleName)
{
    if (session_ == nullptr) {
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto backUpConnection = session_->GetExtConnection(bundleName);
    if (backUpConnection == nullptr) {
        HILOGE("BackUpConnection is empty, bundle:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("Extension backup Proxy is empty, bundle:%{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    // 通知ext清理
    HILOGI("Current bundle will clean extension data, bundleName:%{public}s", bundleName.c_str());
    ErrCode res = proxy->HandleClear();
    if (backUpConnection->IsExtAbilityConnected()) {
        backUpConnection->DisconnectBackupExtAbility();
    }
    ClearSessionAndSchedInfo(bundleName);
    // 非清理任务，需要上报
    if (session_->GetScenario() != IServiceReverseType::Scenario::CLEAN) {
        OnAllBundlesFinished(BError(BError::Codes::OK));
    }
    return res;
}

ErrCode Service::GetBackupInfoCmdHandle(const BundleName &bundleName, std::string &result)
{
    if (session_ == nullptr) {
        HILOGE("Get BackupInfo error, session is empty.");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto backupConnection = session_->CreateBackupConnection(bundleName);
    if (backupConnection == nullptr) {
        HILOGE("backupConnection is null. bundleName: %{public}s", bundleName.c_str());
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    auto callConnected = GetBackupInfoConnectDone(wptr(this), bundleName);
    auto callDied = GetBackupInfoConnectDied(wptr(this), bundleName);
    backupConnection->SetCallback(callConnected);
    backupConnection->SetCallDied(callDied);
    AAFwk::Want want = CreateConnectWant(bundleName);
    auto ret = backupConnection->ConnectBackupExtAbility(want, GetUserIdDefault(), false);
    if (ret) {
        HILOGE("ConnectBackupExtAbility faild, bundleName:%{public}s, ret:%{public}d", bundleName.c_str(), ret);
        return BError(BError::Codes::SA_BOOT_EXT_FAIL);
    }
    std::unique_lock<std::mutex> lock(getBackupInfoSyncLock_);
    getBackupInfoCondition_.wait_for(lock, std::chrono::seconds(CONNECT_WAIT_TIME_S));
    if (isConnectDied_.load()) {
        HILOGE("GetBackupInfoConnectDied, please check bundleName: %{public}s", bundleName.c_str());
        isConnectDied_.store(false);
        return BError(BError::Codes::EXT_ABILITY_DIED);
    }
    auto proxy = backupConnection->GetBackupExtProxy();
    if (!proxy) {
        HILOGE("Extension backup Proxy is empty.");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    ret = proxy->GetBackupInfo(result);
    backupConnection->DisconnectBackupExtAbility();
    if (ret != ERR_OK) {
        HILOGE("Call Ext GetBackupInfo faild.");
        AppRadar::Info info(bundleName, "", "Call Ext GetBackupInfo faild");
        Backup::AppRadar::GetInstance().RecordBackupFuncRes(info, "Service::GetBackupInfoCmdHandle", GetUserIdDefault(),
                                                            BizStageBackup::BIZ_STAGE_GET_BACKUP_INFO_FAIL, ret);
        return BError(BError::Codes::SA_INVAL_ARG);
    }

    return BError(BError::Codes::OK);
}

ErrCode Service::GetBackupInfo(const BundleName &bundleName, std::string &result)
{
    ErrCode errCode = VerifyCaller();
    if (errCode != ERR_OK) {
        HILOGE("GetBackupInfo failed, verify caller failed, bundleName:%{public}s, errCode:%{public}d",
            bundleName.c_str(), errCode);
        return errCode;
    }
    try {
        std::lock_guard<std::mutex> lock(getBackupInfoProcLock_);
        HILOGI("Service::GetBackupInfo begin bundleName: %{public}s", bundleName.c_str());
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("Get BackupInfo error, session is empty.");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        if (session_->GetImpl().clientToken) {
            HILOGI("Already have an active session, bundleName:%{public}s", bundleName.c_str());
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        auto ret = GetBackupInfoCmdHandle(bundleName, result);
        HILOGI("Service::GetBackupInfo end. result: %{public}s", result.c_str());
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return ret;
    } catch (...) {
        HILOGE("Unexpected exception");
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

ErrCode Service::AppendBundlesClearSession(const std::vector<BundleName> &bundleNames)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (bundleNames.empty() || session_ == nullptr) {
            HILOGE("AppendBundles clear session error, session is empty");
            return EPERM;
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__); // BundleMgrAdapter::GetBundleInfos可能耗时
        auto backupInfos = BundleMgrAdapter::GetBundleInfos(bundleNames, session_->GetSessionUserId());
        if (backupInfos.empty()) {
            if (clearRecorder_ != nullptr) {
                clearRecorder_->DeleteConfigFile();
            }
            HILOGE("AppendBundles clear session error, backupInfos is empty");
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return EPERM;
        }
        std::vector<std::string> supportBundleNames;
        for (const auto &info : backupInfos) {
            std::string bundleNameIndexStr = BJsonUtil::BuildBundleNameIndexInfo(info.name, info.appIndex);
            supportBundleNames.emplace_back(bundleNameIndexStr);
        }
        AppendBundles(supportBundleNames);
        for (const auto &info : backupInfos) {
            std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(info.name, info.appIndex);
            session_->SetBackupExtName(bundleNameIndexInfo, info.extensionName);
            session_->SetIsReadyLaunch(bundleNameIndexInfo);
        }
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("Failed, errCode = %{public}d", e.GetCode());
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return e.GetCode();
    } catch (...) {
        HILOGE("Unexpected exception");
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

ErrCode Service::UpdateTimer(const BundleName &bundleName, uint32_t timeout, bool &result)
{
    try {
        HILOGI("Service::UpdateTimer begin.");
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("Update Timer error, session is empty.");
            result = false;
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        ErrCode ret = VerifyCaller();
        if (ret != ERR_OK) {
            HILOGE("Update timer failed, verify caller failed, bundleName:%{public}s", bundleName.c_str());
            result = false;
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return ret;
        }
        auto timeoutCallback = TimeOutCallback(wptr<Service>(this), bundleName);
        result = session_->UpdateTimer(bundleName, timeout, timeoutCallback);
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (...) {
        HILOGE("Unexpected exception");
        result = false;
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
    }
}

ErrCode Service::UpdateSendRate(const std::string &bundleName, int32_t sendRate, bool &result)
{
    if (session_ == nullptr || isOccupyingSession_.load()) {
        HILOGE("Update Send Rate error, session is empty.");
        result = false;
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
    ErrCode ret = VerifySendRateParam();
    if (ret != ERR_OK) {
        HILOGE("Verify sendRate param failed, bundleName:%{public}s", bundleName.c_str());
        result = false;
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return ret;
    }
    std::shared_ptr<ExtensionMutexInfo> mutexPtr = GetExtensionMutex(bundleName);
    if (mutexPtr == nullptr) {
        result = false;
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::SA_INVAL_ARG, "Extension mutex ptr is null.");
    }
    do {
        std::lock_guard<std::mutex> lock(mutexPtr->callbackMutex);
        result = true;
        auto backUpConnection = session_->GetExtConnection(bundleName);
        if (backUpConnection == nullptr) {
            HILOGE("backUpConnection is empty, bundle:%{public}s", bundleName.c_str());
            result = false;
            ret = BError(BError::Codes::SA_INVAL_ARG);
            break;
        }
        auto proxy = backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            HILOGE("Update send rate fail, extension proxy is empty");
            result = false;
            ret = BError(BError::Codes::SA_INVAL_ARG);
            break;
        }
        std::string extBundleName = static_cast<std::string>(bundleName);
        ret = proxy->UpdateFdSendRate(extBundleName, sendRate);
        if (ret != NO_ERROR) {
            HILOGE("Extension do updateSendRate failed, ret:%{public}d", ret);
            result = false;
            ret = BError(BError::Codes::EXT_BROKEN_IPC);
            break;
        }
    } while (0);
    session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
    RemoveExtensionMutex(bundleName);
    return ret;
}

AAFwk::Want Service::CreateConnectWant(const BundleName &bundleName)
{
    BConstants::ExtensionAction action = BConstants::ExtensionAction::BACKUP;
    AAFwk::Want want;
    string backupExtName = BundleMgrAdapter::GetExtName(bundleName, session_->GetSessionUserId());
    want.SetElementName(bundleName, backupExtName);
    want.SetParam(BConstants::EXTENSION_ACTION_PARA, static_cast<int>(action));
    return want;
}

ErrCode Service::BackupSA(std::string bundleName)
{
    HILOGI("BackupSA begin %{public}s", bundleName.c_str());
    if (totalStatistic_ != nullptr) {
        saStatistic_ = std::make_shared<RadarAppStatistic>(bundleName, totalStatistic_->GetUniqId(),
            totalStatistic_->GetBizScene());
        saStatistic_->doBackupSpend_.Start();
    }
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    auto backUpConnection = session_->GetSAExtConnection(bundleName);
    std::shared_ptr<SABackupConnection> saConnection = backUpConnection.lock();
    if (saConnection == nullptr) {
        HILOGE("lock sa connection ptr is nullptr");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    if (scenario == IServiceReverseType::Scenario::BACKUP) {
        auto ret = saConnection->CallBackupSA();
        session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
        BundleBeginRadarReport(bundleName, ret, scenario);
        if (ret) {
            HILOGI("BackupSA ret is %{public}d", ret);
            ClearSessionAndSchedInfo(bundleName);
            NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_DIED));
            return BError(ret);
        }
    } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
        session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), bundleName);
    }
    return BError(BError::Codes::OK);
}

void Service::OnSABackup(const std::string &bundleName, const int &fd, const std::string &result,
    const ErrCode &errCode)
{
    SaStatReport(bundleName, "OnSABackup", RadarError(MODULE_BACKUP, errCode));
    auto task = [bundleName, fd, result, errCode, this]() {
        HILOGI("OnSABackup bundleName: %{public}s, fd: %{public}d, result: %{public}s, err: %{public}d",
            bundleName.c_str(), fd, result.c_str(), errCode);
        BackupRestoreScenario scenario = BackupRestoreScenario::FULL_BACKUP;
        if (session_->GetIsIncrementalBackup()) {
            scenario = BackupRestoreScenario::INCREMENTAL_BACKUP;
            session_->GetServiceReverseProxy()->IncrementalSaBackupOnFileReady(bundleName, "", move(fd), errCode);
        } else {
            scenario = BackupRestoreScenario::FULL_BACKUP;
            session_->GetServiceReverseProxy()->BackupOnFileReady(bundleName, "", move(fd), errCode);
        }
        FileReadyRadarReport(bundleName, "", errCode, IServiceReverseType::Scenario::BACKUP);
        SAResultReport(bundleName, result, errCode, scenario);
    };
    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

void Service::OnSARestore(const std::string &bundleName, const std::string &result, const ErrCode &errCode)
{
    SaStatReport(bundleName, "OnSARestore", RadarError(MODULE_BACKUP, errCode));
    auto task = [bundleName, result, errCode, this]() {
        HILOGI("OnSARestore bundleName: %{public}s, result: %{public}s, err: %{public}d", bundleName.c_str(),
               result.c_str(), errCode);
        SAResultReport(bundleName, result, errCode, BackupRestoreScenario::INCREMENTAL_RESTORE);
    };
    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (...) {
            HILOGE("Failed to add task to thread pool");
        }
    });
}

ErrCode Service::SADone(ErrCode errCode, std::string bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_->OnBundleFileReady(bundleName)) {
            auto backupConnection = session_->GetSAExtConnection(bundleName);
            std::shared_ptr<SABackupConnection> saConnection = backupConnection.lock();
            if (saConnection == nullptr) {
                HILOGE("lock sa connection ptr is nullptr");
                return BError(BError::Codes::SA_INVAL_ARG);
            }
            session_->StopFwkTimer(bundleName);
            session_->StopExtTimer(bundleName);
            saConnection->DisconnectBackupSAExt();
            ClearSessionAndSchedInfo(bundleName);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        ReleaseOnException();
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch(...) {
        HILOGE("Unexpected exception");
        ReleaseOnException();
        return EPERM;
    }
}

void Service::NotifyCallerCurAppDone(ErrCode errCode, const std::string &callerName)
{
    IServiceReverseType::Scenario scenario = session_->GetScenario();
    if (scenario == IServiceReverseType::Scenario::BACKUP) {
        HILOGI("will notify clone data, scenario is Backup");
        session_->GetServiceReverseProxy()->BackupOnBundleFinished(errCode, callerName);
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        std::stringstream strTime;
        strTime << (std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S:")) << (std::setfill('0'))
                << (std::setw(INDEX)) << (ms.count() % MS_1000);
        HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT, FILE_BACKUP_EVENTS,
                        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "PROC_NAME", "ohos.appfileservice",
                        "BUNDLENAME", callerName, "PID", getpid(), "TIME", strTime.str());
    } else if (scenario == IServiceReverseType::Scenario::RESTORE) {
        HILOGI("will notify clone data, scenario is Restore");
        session_->GetServiceReverseProxy()->RestoreOnBundleFinished(errCode, callerName);
    }
    BundleEndRadarReport(callerName, errCode, scenario);
}

ErrCode Service::ReportAppProcessInfo(const std::string &processInfo, BackupRestoreScenario sennario)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        string bundleName;
        ErrCode ret = VerifyCallerAndGetCallerName(bundleName);
        if (ret != ERR_OK) {
            HILOGE("Report app process info failed, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        if (sennario == BackupRestoreScenario::FULL_RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnProcessInfo(bundleName, processInfo);
        } else if (sennario == BackupRestoreScenario::INCREMENTAL_RESTORE) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnProcessInfo(bundleName, processInfo);
        } else if (sennario == BackupRestoreScenario::FULL_BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnProcessInfo(bundleName, processInfo);
        } else if (sennario == BackupRestoreScenario::INCREMENTAL_BACKUP) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnProcessInfo(bundleName, processInfo);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("ReportAppProcessInfo exception");
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    }
}

std::function<void()> Service::TimeOutCallback(wptr<Service> ptr, std::string bundleName)
{
    return [ptr, bundleName, this]() {
        HILOGI("begin timeoutCallback bundleName = %{public}s", bundleName.c_str());
        auto thisPtr = ptr.promote();
        if (!thisPtr) {
            HILOGE("ServicePtr is nullptr.");
            return;
        }
        try {
            DoTimeout(thisPtr, bundleName);
        } catch (...) {
            HILOGE("Unexpected exception, bundleName: %{public}s", bundleName.c_str());
            SendEndAppGalleryNotify(bundleName);
            thisPtr->ClearSessionAndSchedInfo(bundleName);
            thisPtr->NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_TIMEOUT));
        }
    };
}

void Service::DoTimeout(wptr<Service> ptr, std::string bundleName)
{
    auto thisPtr = ptr.promote();
    if (!thisPtr) {
        HILOGE("ServicePtr is nullptr.");
        return;
    }
    auto sessionPtr = thisPtr->session_;
    if (sessionPtr == nullptr) {
        HILOGE("SessionPtr is nullptr.");
        return;
    }
    IServiceReverseType::Scenario scenario = sessionPtr->GetScenario();
    try {
        std::shared_ptr<ExtensionMutexInfo> mutexPtr = GetExtensionMutex(bundleName);
        if (mutexPtr == nullptr) {
            HILOGE("extension mutex ptr is nullptr, bundleName:%{public}s", bundleName.c_str());
            return;
        }
        std::lock_guard<std::mutex> lock(mutexPtr->callbackMutex);
        if (SAUtils::IsSABundleName(bundleName)) {
            auto sessionConnection = sessionPtr->GetSAExtConnection(bundleName);
            shared_ptr<SABackupConnection> saConnection = sessionConnection.lock();
            if (saConnection == nullptr) {
                HILOGE("Error, saConnection is empty, bundleName:%{public}s", bundleName.c_str());
                return;
            }
            saConnection->DisconnectBackupSAExt();
        } else {
            auto sessionConnection = sessionPtr->GetExtConnection(bundleName);
            if (sessionConnection == nullptr) {
                HILOGE("Error, sessionConnection is empty, bundleName:%{public}s", bundleName.c_str());
                return;
            }
            sessionConnection->DisconnectBackupExtAbility();
        }
        TimeoutRadarReport(scenario, bundleName);
        sessionPtr->StopFwkTimer(bundleName);
        sessionPtr->StopExtTimer(bundleName);
        SendEndAppGalleryNotify(bundleName);
        thisPtr->ClearSessionAndSchedInfo(bundleName);
        thisPtr->NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_TIMEOUT));
    } catch (...) {
        HILOGE("Unexpected exception, bundleName: %{public}s", bundleName.c_str());
        SendEndAppGalleryNotify(bundleName);
        thisPtr->ClearSessionAndSchedInfo(bundleName);
        thisPtr->NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_TIMEOUT));
    }
    RemoveExtensionMutex(bundleName);
}

void Service::AddClearBundleRecord(const std::string &bundleName)
{
    // 添加清理记录
    if (!clearRecorder_->InsertClearBundleRecord(bundleName)) {
        HILOGE("Failed to add clear bundle record, bundleName=%{public}s", bundleName.c_str());
        return;
    }
    HILOGI("Add clear bundle record OK, bundleName=%{public}s", bundleName.c_str());
}

void Service::DelClearBundleRecord(const std::vector<std::string> &bundleNames)
{
    // 删除清理记录
    for (const auto &it : bundleNames) {
        if (!clearRecorder_->DeleteClearBundleRecord(it)) {
            HILOGE("Failed to delete clear bundle record, bundleName=%{public}s", it.c_str());
            continue;
        }
        HILOGI("Delete clear bundle record OK, bundleName=%{public}s", it.c_str());
    }
}

void Service::ReleaseOnException()
{
    if (session_->IsOnAllBundlesFinished()) {
        IServiceReverseType::Scenario scenario = session_->GetScenario();
        if (isInRelease_.load() && (scenario == IServiceReverseType::Scenario::RESTORE)) {
            HILOGI("Will execute destory session info");
            SessionDeactive();
        }
    }
}

void Service::SetUserIdAndRestoreType(RestoreTypeEnum restoreType, int32_t userId)
{
    session_->SetImplRestoreType(restoreType);
    if (userId != DEFAULT_INVAL_VALUE) { /* multi user scenario */
        session_->SetSessionUserId(userId);
    } else {
        session_->SetSessionUserId(GetUserIdDefault());
    }
}
} // namespace OHOS::FileManagement::Backup
