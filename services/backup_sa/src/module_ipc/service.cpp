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
static inline int32_t GetUserIdDefault()
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

void OnStartResRadarReport(const std::vector<std::string> &bundleNameList, int32_t stage)
{
    std::stringstream ss;
    ss << "failedBundles:{";
    for (auto &bundleName : bundleNameList) {
        ss << bundleName << ", ";
    }
    ss << "}";
    AppRadar::Info info("", "", ss.str());
    AppRadar::GetInstance().RecordDefaultFuncRes(info, "Service::OnStart", GetUserIdDefault(),
        static_cast<BizStageBackup>(stage), ERR_OK);
}

void Service::ClearFailedBundles()
{
    std::lock_guard<std::mutex> lock(failedBundlesLock_);
    failedBundles_.clear();
}

void Service::UpdateFailedBundles(const std::string &bundleName, BundleTaskInfo taskInfo)
{
    std::lock_guard<std::mutex> lock(failedBundlesLock_);
    failedBundles_[bundleName] = taskInfo;
}

void Service::BundleBeginRadarReport(const std::string &bundleName, const ErrCode errCode,
    const IServiceReverse::Scenario scenario)
{
    if (errCode == ERR_OK || errCode == BError(BError::Codes::SA_BOOT_EXT_FAIL).GetCode()) {
        return;
    }
    BundleTaskInfo taskInfo;
    taskInfo.reportTime = TimeUtils::GetCurrentTime();
    taskInfo.errCode = errCode;
    UpdateFailedBundles(bundleName, taskInfo);
    AppRadar::Info info(bundleName, "", "");
    if (scenario == IServiceReverse::Scenario::RESTORE) {
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::BundleBeginRadarReport",
            GetUserIdDefault(), BizStageRestore::BIZ_STAGE_APPEND_BUNDLES_FAIL, errCode);
    } else if (scenario == IServiceReverse::Scenario::BACKUP) {
        AppRadar::GetInstance().RecordBackupFuncRes(info, "Service::BundleBeginRadarReport",
            GetUserIdDefault(), BizStageBackup::BIZ_STAGE_APPEND_BUNDLES_FAIL, errCode);
    }
}

void Service::BundleEndRadarReport(const std::string &bundleName, const ErrCode errCode,
    const IServiceReverse::Scenario scenario)
{
    if (errCode == ERR_OK) {
        successBundlesNum_++;
        return;
    }
    BundleTaskInfo taskInfo;
    taskInfo.reportTime = TimeUtils::GetCurrentTime();
    taskInfo.errCode = errCode;
    UpdateFailedBundles(bundleName, taskInfo);
    AppRadar::Info info(bundleName, "", "");
    if (scenario == IServiceReverse::Scenario::RESTORE) {
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::BundleEndRadarReport",
            GetUserIdDefault(), BizStageRestore::BIZ_STAGE_EXECU_FAIL, errCode);
    } else if (scenario == IServiceReverse::Scenario::BACKUP) {
        AppRadar::GetInstance().RecordBackupFuncRes(info, "Service::BundleEndRadarReport",
            GetUserIdDefault(), BizStageBackup::BIZ_STAGE_EXECU_FAIL, errCode);
    }
}

void Service::FileReadyRadarReport(const std::string &bundleName, const std::string &fileName, const ErrCode errCode,
    const IServiceReverse::Scenario scenario)
{
    if (errCode == ERR_OK) {
        return;
    }
    std::string fileNameReport = std::string("fileName:\"") + GetAnonyPath(fileName) + "\"";
    AppRadar::Info info(bundleName, "", fileNameReport);
    if (scenario == IServiceReverse::Scenario::RESTORE) {
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::FileReadyRadarReport",
            GetUserIdDefault(), BizStageRestore::BIZ_STAGE_GET_FILE_HANDLE_FAIL, errCode);
    } else if (scenario == IServiceReverse::Scenario::BACKUP) {
        AppRadar::GetInstance().RecordBackupFuncRes(info, "Service::FileReadyRadarReport",
            GetUserIdDefault(), BizStageBackup::BIZ_STAGE_DO_BACKUP, errCode);
    }
}

void Service::ExtensionConnectFailRadarReport(const std::string &bundleName, const ErrCode errCode,
    const IServiceReverse::Scenario scenario)
{
    std::stringstream ss;
    ss << "errCode:" << errCode;
    AppRadar::Info info(bundleName, "", ss.str());
    if (scenario == IServiceReverse::Scenario::RESTORE) {
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::ExtensionConnectFailRadarReport",
            GetUserIdDefault(), BizStageRestore::BIZ_STAGE_CONNECT_EXTENSION_FAIL,
            BError(BError::Codes::SA_BOOT_EXT_FAIL).GetCode());
    } else if (scenario == IServiceReverse::Scenario::BACKUP) {
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
            OnStartResRadarReport(bundleNameList,
                static_cast<int32_t>(BizStageBackup::BIZ_STAGE_ONSTART_DISPOSE));
        }
        if (!residualBundleNameList.empty()) {
            OnStartResRadarReport(residualBundleNameList,
                static_cast<int32_t>(BizStageBackup::BIZ_STAGE_ONSTART_RESIDUAL));
        }
        SetOccupySession(true);
        session_->Active(
            {
                .clientToken = IPCSkeleton::GetCallingTokenID(),
                .scenario = IServiceReverse::Scenario::CLEAN,
                .clientProxy = nullptr,
                .userId = GetUserIdDefault(),
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
        VerifyCaller();
        string path = BConstants::GetSaBundleBackupRootDir(GetUserIdDefault());
        BExcepUltils::VerifyPath(path, false);
        CreateDirIfNotExist(path);
        UniqueFd fd(open(path.data(), O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR));
        if (fd < 0) {
            HILOGE("Failed to open config file = %{private}s, err = %{public}d", path.c_str(), errno);
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return UniqueFd(-EPERM);
        }
        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(std::move(fd));

        auto cache = cachedEntity.Structuralize();

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
    } catch (const exception &e) {
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return UniqueFd(-EPERM);
    } catch (...) {
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("Unexpected exception");
        return UniqueFd(-EPERM);
    }
}

void Service::StopAll(const wptr<IRemoteObject> &obj, bool force)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
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
        for (auto &failBundle : failedBundles_) {
            ss << "\"" << failBundle.first << "\":" << "{errCode:" << failBundle.second.errCode << ","
                << "reportTime:" << failBundle.second.reportTime << "},";
        }
        ss << "}";
        string resultInfo = ss.str();
        AppRadar::StatInfo statInfo("", resultInfo);
        IServiceReverse::Scenario scenario = session_->GetScenario();
        AppRadar::GetInstance().RecordStatisticRes(statInfo, GetUserIdDefault(), scenario,
                                                   successBundlesNum_.load(), fail_cnt, result);
    }
    failedBundles_.clear();
    successBundlesNum_ = 0;
    session_->Deactive(obj, force);
}

static inline void PermissionCheckFailRadar(const std::string &info, const std::string &func)
{
    std::string funcPos = "Service::";
    AppRadar::Info resInfo("", "", info);
    AppRadar::GetInstance().RecordDefaultFuncRes(resInfo, funcPos.append(func),
                                                 GetUserIdDefault(), BizStageBackup::BIZ_STAGE_PERMISSION_CHECK_FAIL,
                                                 BError(BError::Codes::SA_REFUSED_ACT).GetCode());
}

string Service::VerifyCallerAndGetCallerName()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        Security::AccessToken::HapTokenInfo hapTokenInfo;
        if (Security::AccessToken::AccessTokenKit::GetHapTokenInfo(tokenCaller, hapTokenInfo) != 0) {
            PermissionCheckFailRadar("Get hap token info failed", "VerifyCallerAndGetCallerName");
            throw BError(BError::Codes::SA_INVAL_ARG, "Get hap token info failed");
        }
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(hapTokenInfo.bundleName,
            hapTokenInfo.instIndex);
        session_->VerifyBundleName(bundleNameIndexInfo);
        return bundleNameIndexInfo;
    } else {
        string str = to_string(tokenCaller);
        HILOGE("tokenID = %{private}s", GetAnonyString(str).c_str());
        std::string info = string("Invalid token type").append(to_string(tokenType)).append(string("\"}"));
        PermissionCheckFailRadar(info, "VerifyCallerAndGetCallerName");
        throw BError(BError::Codes::SA_INVAL_ARG, string("Invalid token type ").append(to_string(tokenType)));
    }
}

void Service::VerifyCaller()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
    switch (tokenType) {
        case Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE: { /* Update Service */
            if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, BACKUP_PERMISSION) !=
                Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
                std::string info = "Permission denied, token type is " + to_string(tokenType);
                PermissionCheckFailRadar(info, "VerifyCaller");
                throw BError(BError::Codes::SA_REFUSED_ACT,
                    string("Permission denied, token type is ").append(to_string(tokenType)));
            }
            break;
        }
        case Security::AccessToken::ATokenTypeEnum::TOKEN_HAP: {
            if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, BACKUP_PERMISSION) !=
                Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
                std::string info = "Permission denied, token type is " + to_string(tokenType);
                PermissionCheckFailRadar(info, "VerifyCaller");
                throw BError(BError::Codes::SA_REFUSED_ACT,
                    string("Permission denied, token type is ").append(to_string(tokenType)));
            }
            uint64_t fullTokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
            if (!Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(fullTokenId)) {
                std::string info = "Permission denied, token type is " + to_string(tokenType);
                PermissionCheckFailRadar(info, "VerifyCaller");
                throw BError(BError::Codes::SA_REFUSED_ACT,
                    string("Permission denied, token type is ").append(to_string(tokenType)));
            }
            break;
        }
        case Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL:
            if (IPCSkeleton::GetCallingUid() != BConstants::SYSTEM_UID) {
                std::string info = "invalid calling uid";
                PermissionCheckFailRadar(info, "VerifyCaller");
                throw BError(BError::Codes::SA_REFUSED_ACT, "Calling uid is invalid");
            }
            break;
        default:
            std::string info = "Permission denied, token type is " + to_string(tokenType);
            PermissionCheckFailRadar(info, "VerifyCaller");
            throw BError(BError::Codes::SA_REFUSED_ACT, string("Invalid token type ").append(to_string(tokenType)));
            break;
    }
}

void Service::VerifyCaller(IServiceReverse::Scenario scenario)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    session_->VerifyCallerAndScenario(IPCSkeleton::GetCallingTokenID(), scenario);
    VerifyCaller();
}

ErrCode Service::InitRestoreSession(sptr<IServiceReverse> remote)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        VerifyCaller();
        ErrCode errCode = session_->Active({
            .clientToken = IPCSkeleton::GetCallingTokenID(),
            .scenario = IServiceReverse::Scenario::RESTORE,
            .clientProxy = remote,
            .userId = GetUserIdDefault(),
        });
        if (errCode == 0) {
            ClearFailedBundles();
            successBundlesNum_ = 0;
        }
        return errCode;
    } catch (const BError &e) {
        StopAll(nullptr, true);
        return e.GetCode();
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::InitBackupSession(sptr<IServiceReverse> remote)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        VerifyCaller();
        int32_t oldSize = StorageMgrAdapter::UpdateMemPara(BConstants::BACKUP_VFS_CACHE_PRESSURE);
        HILOGE("InitBackupSession oldSize %{public}d", oldSize);
        session_->SetMemParaCurSize(oldSize);
        ErrCode errCode = session_->Active({
            .clientToken = IPCSkeleton::GetCallingTokenID(),
            .scenario = IServiceReverse::Scenario::BACKUP,
            .clientProxy = remote,
            .userId = GetUserIdDefault(),
        });
        if (errCode == 0) {
            ClearFailedBundles();
            successBundlesNum_ = 0;
        }
        return errCode;
    } catch (const BError &e) {
        StopAll(nullptr, true);
        return e.GetCode();
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::Start()
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        VerifyCaller(session_->GetScenario());
        session_->Start();
        OnStartSched();
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("Failde to Start");
        return e.GetCode();
    }
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
    IServiceReverse::Scenario scenario = session->GetScenario();
    if (scenario == IServiceReverse::Scenario::RESTORE && BackupPara().GetBackupOverrideIncrementalRestore() &&
        session->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
        session->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(error, bundleName);
    } else if (scenario == IServiceReverse::Scenario::RESTORE) {
        session->GetServiceReverseProxy()->RestoreOnBundleStarted(error, bundleName);
    }
    BundleBeginRadarReport(bundleName, error.GetCode(), scenario);
}

static vector<BJsonEntityCaps::BundleInfo> GetRestoreBundleNames(UniqueFd fd,
                                                                 sptr<SvcSessionManager> session,
                                                                 const vector<BundleName> &bundleNames)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    // BundleMgrAdapter::GetBundleInfos可能耗时
    auto restoreInfos = BundleMgrAdapter::GetBundleInfos(bundleNames, session->GetSessionUserId());
    BJsonCachedEntity<BJsonEntityCaps> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto bundleInfos = cache.GetBundleInfos();
    if (!bundleInfos.size()) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Json entity caps is empty");
    }
    HILOGI("restoreInfos size is:%{public}zu", restoreInfos.size());
    vector<BJsonEntityCaps::BundleInfo> restoreBundleInfos {};
    for (auto &restoreInfo : restoreInfos) {
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
    const vector<BundleName> &appendBundleNames, const vector<BundleName> &restoreBundleNames)
{
    if (appendBundleNames.size() != restoreBundleNames.size()) {
        HILOGE("AppendBundleNames not equal restoreBundleNames, appendBundleNames size:%{public}zu,"
            "restoreBundleNames size:%{public}zu", appendBundleNames.size(), restoreBundleNames.size());
        for (auto bundleName : appendBundleNames) {
            auto it = find_if(restoreBundleNames.begin(), restoreBundleNames.end(),
                [&bundleName](const auto &obj) { return obj == bundleName; });
            if (it == restoreBundleNames.end()) {
                HILOGE("AppendBundles failed, bundleName = %{public}s.", bundleName.c_str());
                OnBundleStarted(BError(BError::Codes::SA_BUNDLE_INFO_EMPTY), session, bundleName);
            }
        }
    }
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd, const vector<BundleName> &bundleNames,
    const std::vector<std::string> &bundleInfos, RestoreTypeEnum restoreType, int32_t userId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("Begin");
    try {
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("AppendBundles restore session with infos error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        session_->SetImplRestoreType(restoreType);
        if (userId != DEFAULT_INVAL_VALUE) { /* multi user scenario */
            session_->SetSessionUserId(userId);
        } else {
            session_->SetSessionUserId(GetUserIdDefault());
        }
        VerifyCaller(IServiceReverse::Scenario::RESTORE);
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, bool> isClearDataFlags;
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap =
            BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly,
                                        session_->GetSessionUserId(), isClearDataFlags);
        auto restoreInfos = GetRestoreBundleNames(move(fd), session_, bundleNames);
        auto restoreBundleNames = SvcRestoreDepsManager::GetInstance().GetRestoreBundleNames(restoreInfos, restoreType);
        HandleExceptionOnAppendBundles(session_, bundleNames, restoreBundleNames);
        if (restoreBundleNames.empty()) {
            HILOGE("AppendBundlesRestoreSession failed, restoreBundleNames is empty.");
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return BError(BError::Codes::OK);
        }
        session_->AppendBundles(restoreBundleNames);
        SetCurrentSessProperties(restoreInfos, restoreBundleNames, bundleNameDetailMap,
            isClearDataFlags, restoreType);
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("End");
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

void Service::SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo> &restoreBundleInfos,
    std::vector<std::string> &restoreBundleNames, RestoreTypeEnum restoreType)
{
    HILOGI("Start");
    for (auto restoreInfo : restoreBundleInfos) {
        auto it = find_if(restoreBundleNames.begin(), restoreBundleNames.end(), [&restoreInfo](const auto &bundleName) {
            std::string bundleNameIndex = BJsonUtil::BuildBundleNameIndexInfo(restoreInfo.name, restoreInfo.appIndex);
            return bundleName == bundleNameIndex;
        });
        if (it == restoreBundleNames.end()) {
            throw BError(BError::Codes::SA_BUNDLE_INFO_EMPTY, "Can't find bundle name");
        }
        HILOGI("bundleName: %{public}s, extensionName: %{public}s", restoreInfo.name.c_str(),
            restoreInfo.extensionName.c_str());
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(restoreInfo.name, restoreInfo.appIndex);
        if ((!restoreInfo.allToBackup && !SpecialVersion(restoreInfo.versionName)) ||
            (restoreInfo.extensionName.empty() && !SAUtils::IsSABundleName(restoreInfo.name))) {
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
        session_->SetBackupExtName(bundleNameIndexInfo, restoreInfo.extensionName);
        session_->SetIsReadyLaunch(bundleNameIndexInfo);
    }
    HILOGI("End");
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd,
                                             const vector<BundleName> &bundleNames,
                                             RestoreTypeEnum restoreType,
                                             int32_t userId)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("AppendBundles restore session error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        session_->SetImplRestoreType(restoreType);
        if (userId != DEFAULT_INVAL_VALUE) { /* multi user scenario */
            session_->SetSessionUserId(userId);
        } else {
            session_->SetSessionUserId(GetUserIdDefault());
        }
        VerifyCaller(IServiceReverse::Scenario::RESTORE);
        auto restoreInfos = GetRestoreBundleNames(move(fd), session_, bundleNames);
        auto restoreBundleNames = SvcRestoreDepsManager::GetInstance().GetRestoreBundleNames(restoreInfos, restoreType);
        HandleExceptionOnAppendBundles(session_, bundleNames, restoreBundleNames);
        if (restoreBundleNames.empty()) {
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            HILOGW("RestoreBundleNames is empty.");
            return BError(BError::Codes::OK);
        }
        session_->AppendBundles(restoreBundleNames);
        SetCurrentSessProperties(restoreInfos, restoreBundleNames, restoreType);
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

void Service::SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo> &restoreBundleInfos,
    std::vector<std::string> &restoreBundleNames,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> &bundleNameDetailMap,
    std::map<std::string, bool> &isClearDataFlags, RestoreTypeEnum restoreType)
{
    HILOGI("Start");
    for (auto restoreInfo : restoreBundleInfos) {
        auto it = find_if(restoreBundleNames.begin(), restoreBundleNames.end(),
            [&restoreInfo](const auto &bundleName) {
            std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(restoreInfo.name,
                restoreInfo.appIndex);
            return bundleName == bundleNameIndexInfo;
        });
        if (it == restoreBundleNames.end()) {
            throw BError(BError::Codes::SA_BUNDLE_INFO_EMPTY, "Can't find bundle name");
        }
        HILOGD("bundleName: %{public}s, extensionName: %{public}s", restoreInfo.name.c_str(),
            restoreInfo.extensionName.c_str());
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(restoreInfo.name, restoreInfo.appIndex);
        if ((!restoreInfo.allToBackup && !SpecialVersion(restoreInfo.versionName)) ||
            (restoreInfo.extensionName.empty() && !SAUtils::IsSABundleName(restoreInfo.name))) {
            OnBundleStarted(BError(BError::Codes::SA_FORBID_BACKUP_RESTORE), session_, bundleNameIndexInfo);
            session_->RemoveExtInfo(bundleNameIndexInfo);
            continue;
        }
        session_->SetBundleRestoreType(bundleNameIndexInfo, restoreType);
        session_->SetBundleVersionCode(bundleNameIndexInfo, restoreInfo.versionCode);
        session_->SetBundleVersionName(bundleNameIndexInfo, restoreInfo.versionName);
        session_->SetBundleDataSize(bundleNameIndexInfo, restoreInfo.spaceOccupied);
        auto iter = isClearDataFlags.find(bundleNameIndexInfo);
        if (iter != isClearDataFlags.end()) {
            session_->SetClearDataFlag(bundleNameIndexInfo, iter->second);
        }
        BJsonUtil::BundleDetailInfo broadCastInfo;
        BJsonUtil::BundleDetailInfo uniCastInfo;
        bool broadCastRet = BJsonUtil::FindBundleInfoByName(bundleNameDetailMap, bundleNameIndexInfo, BROADCAST_TYPE,
            broadCastInfo);
        if (broadCastRet) {
            bool notifyRet =
                    DelayedSingleton<NotifyWorkService>::GetInstance()->NotifyBundleDetail(broadCastInfo);
            HILOGI("Publish event end, notify result is:%{public}d", notifyRet);
        }
        bool uniCastRet = BJsonUtil::FindBundleInfoByName(bundleNameDetailMap, bundleNameIndexInfo, UNICAST_TYPE,
            uniCastInfo);
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
    try {
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("AppendBundles backup session error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__); // BundleMgrAdapter::GetBundleInfos可能耗时
        VerifyCaller(IServiceReverse::Scenario::BACKUP);
        auto bundleDetails = MakeDetailList(bundleNames);
        auto backupInfos = BundleMgrAdapter::GetBundleInfosForAppend(bundleDetails, session_->GetSessionUserId());
        std::vector<std::string> supportBackupNames = GetSupportBackupBundleNames(backupInfos, false, bundleNames);
        session_->AppendBundles(supportBackupNames);
        SetCurrentBackupSessProperties(supportBackupNames, session_->GetSessionUserId(), backupInfos, false);
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HILOGE("Failed, errCode = %{public}d", e.GetCode());
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return EPERM;
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
    try {
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("AppendBundles backup session with infos error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__); // BundleMgrAdapter::GetBundleInfos可能耗时
        VerifyCaller(IServiceReverse::Scenario::BACKUP);
        std::vector<std::string> bundleNamesOnly;
        std::map<std::string, bool> isClearDataFlags;
        std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap =
            BJsonUtil::BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly,
            session_->GetSessionUserId(), isClearDataFlags);
        auto bundleDetails = MakeDetailList(bundleNames);
        auto backupInfos = BundleMgrAdapter::GetBundleInfosForAppend(bundleDetails, session_->GetSessionUserId());
        std::vector<std::string> supportBackupNames = GetSupportBackupBundleNames(backupInfos, false, bundleNames);
        session_->AppendBundles(supportBackupNames);
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

void Service::HandleCurGroupBackupInfos(std::vector<BJsonEntityCaps::BundleInfo> &backupInfos,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> &bundleNameDetailMap,
    std::map<std::string, bool> &isClearDataFlags)
{
    for (auto &info : backupInfos) {
        HILOGI("Current backupInfo bundleName:%{public}s, extName:%{public}s, appIndex:%{public}d",
            info.name.c_str(), info.extensionName.c_str(), info.appIndex);
        std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(info.name, info.appIndex);
        SetCurrentSessProperties(info, isClearDataFlags, bundleNameIndexInfo);
        BJsonUtil::BundleDetailInfo uniCastInfo;
        if (BJsonUtil::FindBundleInfoByName(bundleNameDetailMap, bundleNameIndexInfo, UNICAST_TYPE, uniCastInfo)) {
            HILOGI("current bundle:%{public}s, unicast info:%{public}s, unicast info size:%{public}zu",
                bundleNameIndexInfo.c_str(), GetAnonyString(uniCastInfo.detail).c_str(), uniCastInfo.detail.size());
            session_->SetBackupExtInfo(bundleNameIndexInfo, uniCastInfo.detail);
        }
        session_->SetBackupExtName(bundleNameIndexInfo, info.extensionName);
        session_->SetIsReadyLaunch(bundleNameIndexInfo);
    }
}

ErrCode Service::ServiceResultReport(const std::string restoreRetInfo,
    BackupRestoreScenario sennario, ErrCode errCode)
{
    string callerName = "";
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        callerName = VerifyCallerAndGetCallerName();
        SendEndAppGalleryNotify(callerName);
        if (sennario == BackupRestoreScenario::FULL_RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnResultReport(restoreRetInfo, callerName, errCode);
            NotifyCloneBundleFinish(callerName, sennario);
        } else if (sennario == BackupRestoreScenario::INCREMENTAL_RESTORE) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnResultReport(restoreRetInfo, callerName, errCode);
            NotifyCloneBundleFinish(callerName, sennario);
        } else if (sennario == BackupRestoreScenario::FULL_BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnResultReport(restoreRetInfo, callerName);
        } else if (sennario == BackupRestoreScenario::INCREMENTAL_BACKUP) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnResultReport(restoreRetInfo, callerName);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        NotifyCloneBundleFinish(callerName, sennario);
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (const exception &e) {
        NotifyCloneBundleFinish(callerName, sennario);
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        NotifyCloneBundleFinish(callerName, sennario);
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::SAResultReport(const std::string bundleName, const std::string restoreRetInfo,
    const ErrCode errCode, const BackupRestoreScenario sennario)
{
    if (sennario == BackupRestoreScenario::FULL_RESTORE) {
        session_->GetServiceReverseProxy()->RestoreOnResultReport(restoreRetInfo, bundleName);
    } else if (sennario == BackupRestoreScenario::INCREMENTAL_RESTORE) {
        session_->GetServiceReverseProxy()->IncrementalRestoreOnResultReport(restoreRetInfo, bundleName);
    } else if (sennario == BackupRestoreScenario::FULL_BACKUP) {
        session_->GetServiceReverseProxy()->BackupOnResultReport(restoreRetInfo, bundleName);
        session_->GetServiceReverseProxy()->BackupOnBundleFinished(errCode, bundleName);
    } else if (sennario == BackupRestoreScenario::INCREMENTAL_BACKUP) {
        session_->GetServiceReverseProxy()->IncrementalBackupOnResultReport(restoreRetInfo, bundleName);
    }
    if (sennario == BackupRestoreScenario::FULL_RESTORE || sennario == BackupRestoreScenario::INCREMENTAL_RESTORE) {
        BundleEndRadarReport(bundleName, errCode, IServiceReverse::Scenario::RESTORE);
    } else if (sennario == BackupRestoreScenario::FULL_BACKUP ||
        sennario == BackupRestoreScenario::INCREMENTAL_BACKUP) {
        BundleEndRadarReport(bundleName, errCode, IServiceReverse::Scenario::BACKUP);
    }
    return SADone(errCode, bundleName);
}

void Service::NotifyCloneBundleFinish(std::string bundleName, const BackupRestoreScenario sennario)
{
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
                throw BError(BError::Codes::SA_INVAL_ARG, "backUpConnection is empty");
            }
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            proxy->HandleClear();
            session_->StopFwkTimer(bundleName);
            session_->StopExtTimer(bundleName);
            backUpConnection->DisconnectBackupExtAbility();
            ClearSessionAndSchedInfo(bundleName);
        }
        RemoveExtensionMutex(bundleName);
        OnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        HILOGI("Unexpected exception");
        ReleaseOnException();
    }
}

ErrCode Service::LaunchBackupSAExtension(const BundleName &bundleName)
{
    string extInfo = session_->GetBackupExtInfo(bundleName);
    IServiceReverse::Scenario scenario = session_->GetScenario();
    if (SAUtils::IsSABundleName(bundleName)) {
        auto saBackUpConnection = session_->GetSAExtConnection(bundleName);
        std::shared_ptr<SABackupConnection> saConnection = saBackUpConnection.lock();
        if (saConnection == nullptr) {
            HILOGE("lock sa connection ptr is nullptr");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            return saConnection->ConnectBackupSAExt(bundleName, BConstants::EXTENSION_BACKUP, extInfo);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            return saConnection->ConnectBackupSAExt(bundleName, BConstants::EXTENSION_RESTORE, extInfo);
        }
    }
    return BError(BError::Codes::OK);
}

ErrCode Service::GetFileHandle(const string &bundleName, const string &fileName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        if (session_ == nullptr) {
            HILOGE("GetFileHandle error, session is empty");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        VerifyCaller(IServiceReverse::Scenario::RESTORE);

        bool updateRes = SvcRestoreDepsManager::GetInstance().UpdateToRestoreBundleMap(bundleName, fileName);
        if (updateRes) {
            return BError(BError::Codes::OK);
        }
        auto action = session_->GetServiceSchedAction(bundleName);
        if (action == BConstants::ServiceSchedAction::RUNNING) {
            auto backUpConnection = session_->GetExtConnection(bundleName);
            if (backUpConnection == nullptr) {
                HILOGE("GetFileHandle error, backUpConnection is empty");
                return BError(BError::Codes::SA_INVAL_ARG);
            }
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                HILOGE("GetFileHandle error, Extension backup Proxy is empty");
                return BError(BError::Codes::SA_INVAL_ARG);
            }
            int32_t errCode = 0;
            UniqueFd fd = proxy->GetFileHandle(fileName, errCode);
            if (errCode != ERR_OK) {
                AppRadar::Info info (bundleName, "", "");
                AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::GetFileHandle", GetUserIdDefault(),
                    BizStageRestore::BIZ_STAGE_GET_FILE_HANDLE_FAIL, errCode);
            }
            session_->GetServiceReverseProxy()->RestoreOnFileReady(bundleName, fileName, move(fd), errCode);
            FileReadyRadarReport(bundleName, fileName, errCode, IServiceReverse::Scenario::RESTORE);
        } else {
            session_->SetExtFileNameRequest(bundleName, fileName);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

void Service::OnBackupExtensionDied(const string &&bundleName, bool isCleanCalled)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (isCleanCalled) {
        HILOGE("Backup <%{public}s> Extension Process second Died", bundleName.c_str());
        ClearSessionAndSchedInfo(bundleName);
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return;
    }
    try {
        string callName = move(bundleName);
        HILOGE("Backup <%{public}s> Extension Process Died", callName.c_str());
        session_->VerifyBundleName(callName);
        // 重新连接清理缓存
        HILOGI("Clear backup extension data, bundleName: %{public}s", callName.c_str());
        ExtConnectDied(callName);
    } catch (...) {
        HILOGE("Unexpected exception, bundleName: %{public}s", bundleName.c_str());
        ExtConnectDied(bundleName);
        return;
    }
}

void Service::ExtConnectDied(const string &callName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGI("Begin, bundleName: %{public}s", callName.c_str());
        std::shared_ptr<ExtensionMutexInfo> mutexPtr = GetExtensionMutex(callName);
        if (mutexPtr == nullptr) {
            HILOGE("extension mutex ptr is nullptr");
            return;
        }
        std::lock_guard<std::mutex> lock(mutexPtr->callbackMutex);
        /* Clear Timer */
        session_->StopFwkTimer(callName);
        session_->StopExtTimer(callName);
        auto backUpConnection = session_->GetExtConnection(callName);
        if (backUpConnection != nullptr && backUpConnection->IsExtAbilityConnected()) {
            backUpConnection->DisconnectBackupExtAbility();
        }
        session_->SetServiceSchedAction(callName, BConstants::ServiceSchedAction::CLEAN);
        auto ret = LaunchBackupExtension(callName);
        if (ret) {
            /* Clear Session before notice client finish event */
            ClearSessionAndSchedInfo(callName);
        }
        /* Notice Client Ext Ability Process Died */
        NoticeClientFinish(callName, BError(BError::Codes::EXT_ABILITY_DIED));
    } catch (...) {
        HILOGE("Unexpected exception, bundleName: %{public}s", callName.c_str());
        ClearSessionAndSchedInfo(callName);
        NoticeClientFinish(callName, BError(BError::Codes::EXT_ABILITY_DIED));
    }
    RemoveExtensionMutex(callName);
}

void Service::ExtStart(const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGE("begin ExtStart, bundle name:%{public}s", bundleName.data());
        if (SAUtils::IsSABundleName(bundleName)) {
            BackupSA(bundleName);
            return;
        }
        if (IncrementalBackup(bundleName)) {
            return;
        }
        IServiceReverse::Scenario scenario = session_->GetScenario();
        auto backUpConnection = session_->GetExtConnection(bundleName);
        if (backUpConnection == nullptr) {
            throw BError(BError::Codes::SA_INVAL_ARG, "ExtStart bundle task error, backUpConnection is empty");
        }
        auto proxy = backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            throw BError(BError::Codes::SA_INVAL_ARG, "ExtStart bundle task error, Extension backup Proxy is empty");
        }
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            auto ret = proxy->HandleBackup(session_->GetClearDataFlag(bundleName));
            session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
            BundleBeginRadarReport(bundleName, ret, scenario);
            if (ret) {
                ClearSessionAndSchedInfo(bundleName);
                NoticeClientFinish(bundleName, BError(BError::Codes::SA_INVAL_ARG));
            }
            return;
        }
        if (scenario != IServiceReverse::Scenario::RESTORE) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Failed to scenario");
        }
        auto ret = proxy->HandleRestore(session_->GetClearDataFlag(bundleName));
        session_->GetServiceReverseProxy()->RestoreOnBundleStarted(ret, bundleName);
        BundleBeginRadarReport(bundleName, ret, scenario);
        auto fileNameVec = session_->GetExtFileNameRequest(bundleName);
        for (auto &fileName : fileNameVec) {
            int32_t errCode = 0;
            UniqueFd fd = proxy->GetFileHandle(fileName, errCode);
            session_->GetServiceReverseProxy()->RestoreOnFileReady(bundleName, fileName, move(fd), errCode);
            FileReadyRadarReport(bundleName, fileName, errCode, scenario);
        }
    } catch (...) {
        HILOGI("Unexpected exception, bundleName: %{public}s", bundleName.c_str());
        ClearSessionAndSchedInfo(bundleName);
        NoticeClientFinish(bundleName, BError(BError::Codes::SA_INVAL_ARG));
    }
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

void Service::ReportOnExtConnectFailed(const IServiceReverse::Scenario scenario,
    const std::string &bundleName, const ErrCode ret)
{
    try {
        if (session_ == nullptr) {
            HILOGE("Report extConnectfailed error, session info is empty");
            return;
        }
        if (scenario == IServiceReverse::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnBundleStarted(ret, bundleName);
            BundleBeginRadarReport(bundleName, ret, scenario);
        } else if (scenario == IServiceReverse::Scenario::RESTORE &&
                   BackupPara().GetBackupOverrideIncrementalRestore() &&
                   session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(ret, bundleName);
            BundleBeginRadarReport(bundleName, ret, scenario);
            DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->EndRestore(bundleName);
            HILOGI("ExtConnectFailed EndRestore, code=%{public}d, bundleName=%{public}s", disposeErr,
                   bundleName.c_str());
        } else if (scenario == IServiceReverse::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
            BundleBeginRadarReport(bundleName, ret, scenario);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnBundleStarted(ret, bundleName);
            BundleBeginRadarReport(bundleName, ret, scenario);
            DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->EndRestore(bundleName);
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
    IServiceReverse::Scenario scenario = IServiceReverse::Scenario::UNDEFINED;
    try {
        HILOGE("begin %{public}s", bundleName.data());
        scenario = session_->GetScenario();
        ReportOnExtConnectFailed(scenario, bundleName, ret);
        ClearSessionAndSchedInfo(bundleName);
        NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_DIED));
        return;
    } catch (const BError &e) {
        return;
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return;
    } catch (...) {
        HILOGI("Unexpected exception");
        return;
    }
}

void Service::NoticeClientFinish(const string &bundleName, ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("begin %{public}s", bundleName.c_str());
    try {
        SendEndAppGalleryNotify(bundleName);
        auto scenario = session_->GetScenario();
        if (scenario == IServiceReverse::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnBundleFinished(errCode, bundleName);
        } else if (scenario == IServiceReverse::Scenario::RESTORE &&
                BackupPara().GetBackupOverrideIncrementalRestore() &&
                session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleFinished(errCode, bundleName);
        } else if (scenario == IServiceReverse::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnBundleFinished(errCode, bundleName);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnBundleFinished(errCode, bundleName);
        };
        BundleEndRadarReport(bundleName, errCode, scenario);
        /* If all bundle ext process finish, notice client. */
        OnAllBundlesFinished(BError(BError::Codes::OK));
    } catch(const BError &e) {
        ReleaseOnException();
    } catch (...) {
        ReleaseOnException();
        HILOGI("Unexpected exception");
        return;
    }
}

void Service::StartRunningTimer(const std::string &bundleName)
{
    auto timeoutCallback = TimeOutCallback(wptr<Service>(this), bundleName);
    auto scenario = session_->GetScenario();
    if (scenario == IServiceReverse::Scenario::BACKUP) {
        session_->StartExtTimer(bundleName, timeoutCallback);
    } else if (scenario == IServiceReverse::Scenario::RESTORE) {
        session_->StartFwkTimer(bundleName, timeoutCallback);
    }
}

void Service::ExtConnectDone(string bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        HILOGE("begin %{public}s", bundleName.data());

        BConstants::ServiceSchedAction curSchedAction = session_->GetServiceSchedAction(bundleName);
        if (curSchedAction == BConstants::ServiceSchedAction::CLEAN) {
            sched_->Sched(bundleName);
            return;
        }
        if (curSchedAction == BConstants::ServiceSchedAction::START &&
            clearRecorder_->FindClearBundleRecord(bundleName)) {
            session_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::CLEAN);
        } else {
            session_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::RUNNING);
            AddClearBundleRecord(bundleName);
        }
        sched_->Sched(bundleName);
    } catch (...) {
        HILOGE("Unexpected exception, bundleName: %{public}s", bundleName.c_str());
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
        return;
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
        return;
    } catch (...) {
        HILOGI("Unexpected exception");
        return;
    }
}

void Service::HandleRestoreDepsBundle(const string &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (session_->GetScenario() != IServiceReverse::Scenario::RESTORE) {
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
    session_->AppendBundles(restoreBundleNames);
    for (const auto &bundle : restoreBundleMap) {
        for (auto &bundleInfo : SvcRestoreDepsManager::GetInstance().GetAllBundles()) {
            if (bundle.first != bundleInfo.name) {
                continue;
            }
            SvcRestoreDepsManager::RestoreInfo info = bundle.second;
            session_->SetBundleRestoreType(bundleInfo.name, info.restoreType_);
            session_->SetBundleVersionCode(bundleInfo.name, bundleInfo.versionCode);
            session_->SetBundleVersionName(bundleInfo.name, bundleInfo.versionName);
            session_->SetBundleDataSize(bundleInfo.name, bundleInfo.spaceOccupied);
            for (auto &fileName : info.fileNames_) {
                session_->SetExtFileNameRequest(bundleInfo.name, fileName);
            }
            session_->SetBackupExtName(bundleInfo.name, bundleInfo.extensionName);
            session_->SetIsReadyLaunch(bundleInfo.name);
        }
    }
    HILOGI("End");
}

void Service::OnAllBundlesFinished(ErrCode errCode)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    HILOGI("called begin.");
    if (session_->IsOnAllBundlesFinished()) {
        IServiceReverse::Scenario scenario = session_->GetScenario();
        if (isInRelease_.load() && (scenario == IServiceReverse::Scenario::RESTORE)) {
            SessionDeactive();
        }
        if (scenario == IServiceReverse::Scenario::BACKUP && session_->GetIsIncrementalBackup()) {
            session_->GetServiceReverseProxy()->IncrementalBackupOnAllBundlesFinished(errCode);
        } else if (scenario == IServiceReverse::Scenario::RESTORE &&
                   BackupPara().GetBackupOverrideIncrementalRestore() &&
                   session_->ValidRestoreDataType(RestoreTypeEnum::RESTORE_DATA_WAIT_SEND)) {
            session_->GetServiceReverseProxy()->IncrementalRestoreOnAllBundlesFinished(errCode);
        } else if (scenario == IServiceReverse::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnAllBundlesFinished(errCode);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnAllBundlesFinished(errCode);
        }
        if (!BackupPara().GetBackupOverrideBackupSARelease()) {
            sched_->TryUnloadServiceTimer(true);
        }
    }
    HILOGI("called end.");
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
    IServiceReverse::Scenario scenario = session_->GetScenario();
    if (scenario != IServiceReverse::Scenario::RESTORE) {
        return;
    }
    if (!disposal_->IfBundleNameInDisposalConfigFile(bundleName)) {
        HILOGE("WriteDisposalConfigFile Failed");
        return;
    }
    HILOGI("AppendIntoDisposalConfigFile OK, bundleName=%{public}s", bundleName.c_str());
    DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->StartRestore(bundleName);
    HILOGI("StartRestore, code=%{public}d, bundleName=%{public}s", disposeErr,
        bundleName.c_str());
}

void Service::SendEndAppGalleryNotify(const BundleName &bundleName)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    if (SAUtils::IsSABundleName(bundleName)) {
        HILOGI("SA does not need to EndRestore");
        return;
    }
    IServiceReverse::Scenario scenario = session_->GetScenario();
    if (scenario != IServiceReverse::Scenario::RESTORE) {
        return;
    }
    DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->EndRestore(bundleName);
    HILOGI("EndRestore, code=%{public}d, bundleName=%{public}s", disposeErr,
        bundleName.c_str());
    if (disposeErr != DisposeErr::OK) {
        HILOGE("Error code=%{public}d, disposal will be clear in the end", disposeErr);
        return;
    }
    if (!disposal_->DeleteFromDisposalConfigFile(bundleName)) {
        HILOGE("DeleteFromDisposalConfigFile Failed, bundleName=%{public}s", bundleName.c_str());
        return;
    }
    HILOGI("DeleteFromDisposalConfigFile OK, bundleName=%{public}s", bundleName.c_str());
}

void Service::TryToClearDispose(const BundleName &bundleName)
{
    int32_t maxAtt = MAX_TRY_CLEAR_DISPOSE_NUM;
    int32_t att = 0;
    while (att < maxAtt) {
        DisposeErr disposeErr = AppGalleryDisposeProxy::GetInstance()->EndRestore(bundleName);
        HILOGI("EndRestore, code=%{public}d, bundleName=%{public}s", disposeErr, bundleName.c_str());
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
    IServiceReverse::Scenario scenario = session_->GetScenario();
    if (scenario != IServiceReverse::Scenario::RESTORE) {
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
    IServiceReverse::Scenario scenario = session_->GetScenario();
    if (scenario != IServiceReverse::Scenario::RESTORE) {
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
        if (session_->GetScenario() == IServiceReverse::Scenario::RESTORE &&
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

std::function<void(const std::string &&)> Service::GetBackupInfoConnectDone(wptr<Service> obj, std::string &bundleName)
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

std::function<void(const std::string &&, bool)> Service::GetBackupInfoConnectDied(
    wptr<Service> obj, std::string &bundleName)
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
        throw BError(BError::Codes::SA_INVAL_ARG, "backUpConnection is empty");
    }
    auto proxy = backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
    }
    // 通知ext清理
    ErrCode res = proxy->HandleClear();
    if (backUpConnection->IsExtAbilityConnected()) {
        backUpConnection->DisconnectBackupExtAbility();
    }
    ClearSessionAndSchedInfo(bundleName);
    // 非清理任务，需要上报
    if (session_->GetScenario() != IServiceReverse::Scenario::CLEAN) {
        OnAllBundlesFinished(BError(BError::Codes::OK));
    }
    return res;
}

ErrCode Service::GetBackupInfoCmdHandle(BundleName &bundleName, std::string &result)
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

ErrCode Service::GetBackupInfo(BundleName &bundleName, std::string &result)
{
    try {
        std::lock_guard<std::mutex> lock(getBackupInfoProcLock_);
        HILOGI("Service::GetBackupInfo begin.");
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("Get BackupInfo error, session is empty.");
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        if (session_->GetImpl().clientToken) {
            return BError(BError::Codes::SA_REFUSED_ACT, "Already have an active session");
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        auto ret = GetBackupInfoCmdHandle(bundleName, result);
        HILOGI("Service::GetBackupInfo end. result: %{public}s", result.c_str());
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return ret;
    } catch (...) {
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::StartExtTimer(bool &isExtStart)
{
    try {
        HILOGI("Service::StartExtTimer begin.");
        if (session_ == nullptr) {
            HILOGE("StartExtTimer error, session_ is nullptr.");
            isExtStart = false;
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        string bundleName = VerifyCallerAndGetCallerName();
        auto timeoutCallback = TimeOutCallback(wptr<Service>(this), bundleName);
        session_->StopFwkTimer(bundleName);
        isExtStart = session_->StartExtTimer(bundleName, timeoutCallback);
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (...) {
        isExtStart = false;
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::StartFwkTimer(bool &isFwkStart)
{
    try {
        HILOGI("Service::StartFwkTimer begin.");
        if (session_ == nullptr) {
            HILOGE("StartFwkTimer error, session_ is nullptr.");
            isFwkStart = false;
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        string bundleName = VerifyCallerAndGetCallerName();
        auto timeoutCallback = TimeOutCallback(wptr<Service>(this), bundleName);
        session_->StopExtTimer(bundleName);
        isFwkStart = session_->StartFwkTimer(bundleName, timeoutCallback);
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (...) {
        isFwkStart = false;
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("Unexpected exception");
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
        for (auto info : backupInfos) {
            std::string bundleNameIndexStr = BJsonUtil::BuildBundleNameIndexInfo(info.name, info.appIndex);
            supportBundleNames.emplace_back(bundleNameIndexStr);
        }
        session_->AppendBundles(supportBundleNames);
        for (auto info : backupInfos) {
            std::string bundleNameIndexInfo = BJsonUtil::BuildBundleNameIndexInfo(info.name, info.appIndex);
            session_->SetBackupExtName(bundleNameIndexInfo, info.extensionName);
            session_->SetIsReadyLaunch(bundleNameIndexInfo);
        }
        OnStartSched();
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGE("Failed, errCode = %{public}d", e.GetCode());
        return e.GetCode();
    } catch (const exception &e) {
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HandleExceptionOnAppendBundles(session_, bundleNames, {});
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::UpdateTimer(BundleName &bundleName, uint32_t timeout, bool &result)
{
    try {
        HILOGI("Service::UpdateTimer begin.");
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("Update Timer error, session is empty.");
            result = false;
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        VerifyCaller();
        auto timeoutCallback = TimeOutCallback(wptr<Service>(this), bundleName);
        result = session_->UpdateTimer(bundleName, timeout, timeoutCallback);
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (...) {
        result = false;
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::UpdateSendRate(std::string &bundleName, int32_t sendRate, bool &result)
{
    try {
        HILOGI("Begin, bundle name:%{public}s, sendRate is:%{public}d", bundleName.c_str(), sendRate);
        if (session_ == nullptr || isOccupyingSession_.load()) {
            HILOGE("Update Send Rate error, session is empty.");
            result = false;
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        session_->IncreaseSessionCnt(__PRETTY_FUNCTION__);
        std::shared_ptr<ExtensionMutexInfo> mutexPtr = GetExtensionMutex(bundleName);
        if (mutexPtr == nullptr) {
            HILOGE("extension mutex ptr is nullptr");
            return BError(BError::Codes::SA_INVAL_ARG, "Extension mutex ptr is null.");
        }
        std::lock_guard<std::mutex> lock(mutexPtr->callbackMutex);
        VerifyCaller();
        IServiceReverse::Scenario scenario = session_ -> GetScenario();
        if (scenario != IServiceReverse::Scenario::BACKUP) {
            HILOGE("This method is applicable to the backup scenario");
            result = false;
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return BError(BError::Codes::SA_INVAL_ARG);
        }
        auto tempBackupConnection = session_->GetExtConnection(bundleName);
        auto backupConnection = tempBackupConnection.promote();
        auto proxy = backupConnection->GetBackupExtProxy();
        if (!proxy) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
        }
        auto ret = proxy->UpdateFdSendRate(bundleName, sendRate);
        if (ret != NO_ERROR) {
            result = false;
            session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
            return BError(BError::Codes::EXT_BROKEN_IPC);
        }
        result = true;
        RemoveExtensionMutex(bundleName);
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        return BError(BError::Codes::OK);
    } catch (...) {
        result = false;
        RemoveExtensionMutex(bundleName);
        session_->DecreaseSessionCnt(__PRETTY_FUNCTION__);
        HILOGI("Unexpected exception");
        return EPERM;
    }
}

AAFwk::Want Service::CreateConnectWant (BundleName &bundleName)
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
    IServiceReverse::Scenario scenario = session_->GetScenario();
    auto backUpConnection = session_->GetSAExtConnection(bundleName);
    std::shared_ptr<SABackupConnection> saConnection = backUpConnection.lock();
    if (saConnection == nullptr) {
        HILOGE("lock sa connection ptr is nullptr");
        return BError(BError::Codes::SA_INVAL_ARG);
    }
    if (scenario == IServiceReverse::Scenario::BACKUP) {
        auto ret = saConnection->CallBackupSA();
        session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
        BundleBeginRadarReport(bundleName, ret, scenario);
        if (ret) {
            HILOGI("BackupSA ret is %{public}d", ret);
            ClearSessionAndSchedInfo(bundleName);
            NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_DIED));
            return BError(ret);
        }
    } else if (scenario == IServiceReverse::Scenario::RESTORE) {
        session_->GetServiceReverseProxy()->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), bundleName);
    }
    return BError(BError::Codes::OK);
}

void Service::OnSABackup(const std::string &bundleName, const int &fd, const std::string &result,
    const ErrCode &errCode)
{
    auto task = [bundleName, fd, result, errCode, this]() {
        HILOGI("OnSABackup bundleName: %{public}s, fd: %{public}d, result: %{public}s, err: %{public}d",
            bundleName.c_str(), fd, result.c_str(), errCode);
        session_->GetServiceReverseProxy()->BackupOnFileReady(bundleName, "", move(fd), errCode);
        FileReadyRadarReport(bundleName, "", errCode, IServiceReverse::Scenario::BACKUP);
        SAResultReport(bundleName, result, errCode, BackupRestoreScenario::FULL_BACKUP);
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
    auto task = [bundleName, result, errCode, this]() {
        HILOGI("OnSARestore bundleName: %{public}s, result: %{public}s, err: %{public}d",
            bundleName.c_str(), result.c_str(), errCode);
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
        OnAllBundlesFinished(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        ReleaseOnException();
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (const exception &e) {
        ReleaseOnException();
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch(...) {
        ReleaseOnException();
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

void Service::NotifyCallerCurAppDone(ErrCode errCode, const std::string &callerName)
{
    IServiceReverse::Scenario scenario = session_->GetScenario();
    if (scenario == IServiceReverse::Scenario::BACKUP) {
        HILOGI("will notify clone data, scenario is Backup");
        session_->GetServiceReverseProxy()->BackupOnBundleFinished(errCode, callerName);
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        std::stringstream strTime;
        strTime << (std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S:")) << (std::setfill('0'))
            << (std::setw(INDEX)) << (ms.count() % MS_1000);
        HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
            FILE_BACKUP_EVENTS,
            OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "PROC_NAME", "ohos.appfileservice",
            "BUNDLENAME", callerName,
            "PID", getpid(),
            "TIME", strTime.str()
        );
    } else if (scenario == IServiceReverse::Scenario::RESTORE) {
        HILOGI("will notify clone data, scenario is Restore");
        SendEndAppGalleryNotify(callerName);
        session_->GetServiceReverseProxy()->RestoreOnBundleFinished(errCode, callerName);
    }
    BundleEndRadarReport(callerName, errCode, scenario);
}

ErrCode Service::ReportAppProcessInfo(const std::string processInfo, BackupRestoreScenario sennario)
{
    HITRACE_METER_NAME(HITRACE_TAG_FILEMANAGEMENT, __PRETTY_FUNCTION__);
    try {
        string bundleName = VerifyCallerAndGetCallerName();
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
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (const exception &e) {
        HILOGI("Catched an unexpected low-level exception %{public}s", e.what());
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
            thisPtr->ClearSessionAndSchedInfo(bundleName);
            thisPtr->NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_TIMEOUT));
        }
    };
}

void Service::TimeoutRadarReport(IServiceReverse::Scenario scenario, std::string &bundleName)
{
    int32_t errCode = BError(BError::Codes::EXT_ABILITY_TIMEOUT).GetCode();
    if (scenario == IServiceReverse::Scenario::BACKUP) {
        AppRadar::Info info(bundleName, "", "on backup timeout");
        AppRadar::GetInstance().RecordBackupFuncRes(info, "Service::TimeOutCallback", GetUserIdDefault(),
            BizStageBackup::BIZ_STAGE_ON_BACKUP, errCode);
    } else if (scenario == IServiceReverse::Scenario::RESTORE) {
        AppRadar::Info info(bundleName, "", "on restore timeout");
        AppRadar::GetInstance().RecordRestoreFuncRes(info, "Service::TimeOutCallback", GetUserIdDefault(),
            BizStageRestore::BIZ_STAGE_ON_RESTORE, errCode);
    }
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
    IServiceReverse::Scenario scenario = sessionPtr->GetScenario();
    TimeoutRadarReport(scenario, bundleName);
    try {
        std::shared_ptr<ExtensionMutexInfo> mutexPtr = GetExtensionMutex(bundleName);
        if (mutexPtr == nullptr) {
            HILOGE("extension mutex ptr is nullptr");
            return;
        }
        std::lock_guard<std::mutex> lock(mutexPtr->callbackMutex);
        if (SAUtils::IsSABundleName(bundleName)) {
            auto sessionConnection = sessionPtr->GetSAExtConnection(bundleName);
            shared_ptr<SABackupConnection> saConnection = sessionConnection.lock();
            if (saConnection == nullptr) {
                HILOGE("lock sa connection ptr is nullptr");
                return;
            }
            saConnection->DisconnectBackupSAExt();
        } else {
            auto sessionConnection = sessionPtr->GetExtConnection(bundleName);
            sessionConnection->DisconnectBackupExtAbility();
        }
        sessionPtr->StopFwkTimer(bundleName);
        sessionPtr->StopExtTimer(bundleName);
        thisPtr->ClearSessionAndSchedInfo(bundleName);
        thisPtr->NoticeClientFinish(bundleName, BError(BError::Codes::EXT_ABILITY_TIMEOUT));
    } catch (...) {
        HILOGE("Unexpected exception, bundleName: %{public}s", bundleName.c_str());
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
    try {
        if (session_->IsOnAllBundlesFinished()) {
            IServiceReverse::Scenario scenario = session_->GetScenario();
            if (isInRelease_.load() && (scenario == IServiceReverse::Scenario::RESTORE)) {
                SessionDeactive();
            }
        }
    } catch (...) {
        HILOGE("Unexpected exception");
    }
}
} // namespace OHOS::FileManagement::Backup
