/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos.file.backup.proj.hpp"
#include "ohos.file.backup.impl.hpp"
#include "general_callbacks_taihe.h"
#include "stdexcept"
#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "b_sa/b_sa_utils.h"
#include "b_session_backup.h"
#include "b_incremental_restore_session.h"
#include "b_incremental_backup_session.h"
#include "filemgmt_libhilog.h"
#include "filemgmt_libn.h"
#include "general_callbacks_taihe.h"
#include "service_client.h"
#include "unique_fd.h"

namespace {
const int INVALID_FD = -1;
using namespace std;
using BSessionBackup = OHOS::FileManagement::Backup::BSessionBackup;
using TaiheGeneralCallbacks = OHOS::FileManagement::Backup::TAIHE::TaiheGeneralCallbacks;
using BError = OHOS::FileManagement::Backup::BError;
using ErrCode = OHOS::FileManagement::Backup::ErrCode;
using SAUtils = OHOS::FileManagement::Backup::SAUtils;
using UniqueFd = OHOS::UniqueFd;
using BIncrementalData = OHOS::FileManagement::Backup::BIncrementalData;
using BIncrementalRestoreSession = OHOS::FileManagement::Backup::BIncrementalRestoreSession;
using BIncrementalBackupSession = OHOS::FileManagement::Backup::BIncrementalBackupSession;
using ServiceClient = OHOS::FileManagement::Backup::ServiceClient;
using BackupErrorCode = OHOS::FileManagement::Backup::BError::BackupErrorCode;
using FileData = ::ohos::file::backup::FileData;
using IncrementalBackupTime = ::ohos::file::backup::IncrementalBackupTime;

#define BACKUP_PERMISSION 201
#define SYSTEM_PERMISSION 202

class SessionBackupImpl {
public:
    SessionBackupImpl() {
        // Don't forget to implement the constructor.
    }

    explicit SessionBackupImpl(uintptr_t callbacks) {
        ani_env *env = taihe::get_env();
        ani_object callbackObj = reinterpret_cast<ani_object>(callbacks);
        if (env == nullptr) {
            HILOGE("ani_env is nullptr ");
            return;
        }
        ani_vm *vm = nullptr;
        env->GetVM(&vm);
        if (!vm) {
            HILOGE("vm  is nullptr");
            return;
        }
        ani_status status = ANI_OK;
        status = env->GlobalReference_Create(callbackObj, &callbackRef_);
        if (ANI_OK != status)
        {
            callbackRef_ = nullptr;
            HILOGE(" GlobalReference_Create failed");
            return;
        }
        taiheGeneralCallbacks = std::make_shared<TaiheGeneralCallbacks>(getGeneCallBack(), vm);
        ErrCode errCode;
        std::string errMsg;
        bSessionBackup = BSessionBackup::Init(BSessionBackup::Callbacks {
            .onFileReady = bind(&TaiheGeneralCallbacks::onFileReady, taiheGeneralCallbacks,
                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
            .onBundleStarted = bind(&TaiheGeneralCallbacks::onBundleStarted, taiheGeneralCallbacks,
                                    std::placeholders::_1, std::placeholders::_2),
            .onBundleFinished = bind(&TaiheGeneralCallbacks::onBundleFinished, taiheGeneralCallbacks,
                std::placeholders::_1, std::placeholders::_2),
            .onAllBundlesFinished = bind(&TaiheGeneralCallbacks::onAllBundlesFinished, taiheGeneralCallbacks,
                std::placeholders::_1),
            .onResultReport = bind(&TaiheGeneralCallbacks::onResultReport, taiheGeneralCallbacks,
                std::placeholders::_1, std::placeholders::_2),
            .onBackupServiceDied = bind(&TaiheGeneralCallbacks::onBackupServiceDied, taiheGeneralCallbacks),
            .onProcess = bind(&TaiheGeneralCallbacks::onProcess, taiheGeneralCallbacks, std::placeholders::_1,
                std::placeholders::_2),
            .onBackupSizeReport = bind(&TaiheGeneralCallbacks::onBackupSizeReport, taiheGeneralCallbacks,
                std::placeholders::_1)
        }, errMsg, errCode);
    }

    ~SessionBackupImpl() {
        if (callbackRef_)
        {
            ani_env *env = taihe::get_env();
            ani_status status = ANI_OK;
            status = env->GlobalReference_Delete(callbackRef_);
            if (ANI_OK != status)
            {
                HILOGE(" GlobalReference_Delete failed");
            }
            callbackRef_ = nullptr;
        }
    }

    ::ohos::file::backup::FileData GetLocalCapabilitiesSync() {
        ::ohos::file::backup::FileData data;
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return data;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return data;
        }
        auto fd = std::make_shared<UniqueFd>();
        *fd = bSessionBackup->GetLocalCapabilities();
        data.fd = fd->Release();
        return data;
    }

    void GetBackupDataSizeSync(bool isPreciseScan, ::taihe::array_view<IncrementalBackupTime> dataList) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }
        std::vector<BIncrementalData> bundleNames;
        ::taihe::array<::ohos::file::backup::IncrementalBackupTime> dataListTemp(dataList);
        for (auto it = dataListTemp.begin(); it != dataListTemp.end(); ++it)
        {
            BIncrementalData data;
            data.bundleName = std::string(it->bundleName);
            data.lastIncrementalTime = it->lastIncrementalTime;
            bundleNames.push_back(data);
        }
        bSessionBackup->GetBackupDataSize(isPreciseScan, bundleNames);
    }

    void AppendBundlesSync1(::taihe::array_view<::taihe::string> bundlesToBackup,
                            ::taihe::optional_view<::taihe::array<::taihe::string>> infos) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }
        std::vector<std::string> bundleNames;
        std::vector<std::string> bundleInfos;
        ::taihe::array<::taihe::string> bundles(bundlesToBackup);
        for (auto it = bundles.begin(); it != bundles.end(); ++it)
        {
            bundleNames.push_back(std::string(*it));
        }

        if (infos.has_value())
        {
            ::taihe::array<::taihe::string> infosTemp = infos.value();
            for (auto it = infosTemp.begin(); it != infosTemp.end(); ++it)
            {
                bundleInfos.push_back(std::string(*it));
            }
        }

        bSessionBackup->AppendBundles(bundleNames, bundleInfos);
    }

    void AppendBundlesSync2(::taihe::array_view<::taihe::string> bundlesToBackup) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }

        std::vector<std::string> bundleNames;
        ::taihe::array<::taihe::string> bundles(bundlesToBackup);
        for (auto it = bundles.begin(); it != bundles.end(); ++it)
        {
            bundleNames.push_back(std::string(*it));
        }

        bSessionBackup->AppendBundles(bundleNames);
    }

    void ReleaseSync() {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }
        bSessionBackup->Release();
    }

    int32_t cancel(::taihe::string_view bundleName) {
        int result = 0;
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return result;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return result;
        }
        result = bSessionBackup->Cancel(std::string(bundleName));
        return result;
    }

    bool CleanBundleTempDirSync(::taihe::string_view bundleName) {
        bool result = false;
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return result;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return result;
        }
        ErrCode code = bSessionBackup->CleanBundleTempDir(std::string(bundleName));
        if (BError(BError::Codes::OK) == code)
        {
            result = true;
        }else {
            ::taihe::set_business_error((int32_t)code, "CleanBundleTempDirSync error");
        }
        return result;
    }

    ::taihe::string GetCompatibilityInfoSync(::taihe::string_view bundleName, ::taihe::string_view extInfo) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return ::taihe::string("");
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return ::taihe::string("");
        }
        std::string bundleNameTemp(bundleName);
        std::string exInfoTemp(extInfo);
        auto compatInfo = std::make_shared<std::string>();
        bSessionBackup->GetCompatibilityInfo(bundleNameTemp, exInfoTemp, *compatInfo);
        std::string result = *compatInfo;
        return ::taihe::string(result.c_str());
    }

private:
    ani_ref* getGeneCallBack()
    {
        return &callbackRef_;
    }
    ani_ref callbackRef_ = nullptr;
    std::unique_ptr<BSessionBackup> bSessionBackup = nullptr;
    std::shared_ptr<TaiheGeneralCallbacks> taiheGeneralCallbacks = nullptr;
};

class SessionRestoreImpl {
public:
    SessionRestoreImpl() {
        // Don't forget to implement the constructor.
    }

    explicit SessionRestoreImpl(uintptr_t callbacks) {
        ani_env *env = taihe::get_env();
        ani_object callbackObj = reinterpret_cast<ani_object>(callbacks);
        if (env == nullptr) {
            HILOGE("ani_env is nullptr ");
            return;
        }
        ani_vm *vm = nullptr;
        env->GetVM(&vm);
        if (!vm) {
            HILOGE("vm is nullptr");
            return;
        }
        ani_status status = ANI_OK;
        status = env->GlobalReference_Create(callbackObj, &callbackRef_);
        if (ANI_OK != status)
        {
            callbackRef_ = nullptr;
            HILOGE(" GlobalReference_Create failed");
            return;
        }
        taiheGeneralCallbacks = std::make_shared<TaiheGeneralCallbacks>(getGeneCallBack(), vm);
        ErrCode errCode;
        std::string errMsg;
        bIncrementalRestoreSession = BIncrementalRestoreSession::Init(BIncrementalRestoreSession::Callbacks {
            .onFileReady = bind(&TaiheGeneralCallbacks::onFileReady, taiheGeneralCallbacks,
                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
            .onBundleStarted = bind(&TaiheGeneralCallbacks::onBundleStarted, taiheGeneralCallbacks,
                                    std::placeholders::_1, std::placeholders::_2),
            .onBundleFinished = bind(&TaiheGeneralCallbacks::onBundleFinished, taiheGeneralCallbacks,
                std::placeholders::_1, std::placeholders::_2),
            .onAllBundlesFinished = bind(&TaiheGeneralCallbacks::onAllBundlesFinished, taiheGeneralCallbacks,
                std::placeholders::_1),
            .onResultReport = bind(&TaiheGeneralCallbacks::onResultReport, taiheGeneralCallbacks,
                std::placeholders::_1, std::placeholders::_2),
            .onBackupServiceDied = bind(&TaiheGeneralCallbacks::onBackupServiceDied, taiheGeneralCallbacks),
            .onProcess = bind(&TaiheGeneralCallbacks::onProcess, taiheGeneralCallbacks,
                std::placeholders::_1, std::placeholders::_2)
        }, errMsg, errCode);
    }

    ~SessionRestoreImpl() {
        if (callbackRef_)
        {
            ani_env *env = taihe::get_env();
            ani_status status = ANI_OK;
            status = env->GlobalReference_Delete(callbackRef_);
            if (ANI_OK != status)
            {
                HILOGE(" GlobalReference_Delete failed");
            }
            callbackRef_ = nullptr;
        }
    }

    ::ohos::file::backup::FileData GetLocalCapabilitiesSync() {
        ::ohos::file::backup::FileData data;
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return data;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return data;
        }
        auto fd = make_shared<UniqueFd>();
        *fd = bIncrementalRestoreSession->GetLocalCapabilities();
        data.fd = fd->Release();
        return data;
    }

    void AppendBundlesSync1(int32_t remoteCapabilitiesFd, ::taihe::array_view<::taihe::string> bundlesToBackup,
                            ::taihe::optional_view<::taihe::array<::taihe::string>> infos) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }

        std::vector<std::string> bundleNames;
        std::vector<std::string> bundleInfos;
        ::taihe::array<::taihe::string> bundles(bundlesToBackup);
        for (auto it = bundles.begin(); it != bundles.end(); ++it)
        {
            bundleNames.push_back(std::string(*it));
        }

        if (infos.has_value())
        {
            ::taihe::array<::taihe::string> infosTemp = infos.value();
            for (auto it = infosTemp.begin(); it != infosTemp.end(); ++it)
            {
                bundleInfos.push_back(std::string(*it));
            }
            bIncrementalRestoreSession->AppendBundles(UniqueFd(remoteCapabilitiesFd), bundleNames, bundleInfos);
        } else {
            bIncrementalRestoreSession->AppendBundles(UniqueFd(remoteCapabilitiesFd), bundleNames);
        }
    }

    void AppendBundlesSync2(int32_t remoteCapabilitiesFd, ::taihe::array_view<::taihe::string> bundlesToBackup) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }

        std::vector<std::string> bundleNames;
        ::taihe::array<::taihe::string> bundles(bundlesToBackup);
        for (auto it = bundles.begin(); it != bundles.end(); ++it)
        {
            bundleNames.push_back(std::string(*it));
        }

        bIncrementalRestoreSession->AppendBundles(UniqueFd(remoteCapabilitiesFd), bundleNames);
    }

    void PublishFileSync(::ohos::file::backup::FileMeta const& fileMeta) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }

        std::string bundleName(fileMeta.bundleName);
        std::string fileName(fileMeta.uri);

        BFileInfo fileInfo(bundleName, fileName, 0);

        bIncrementalRestoreSession->PublishSAFile(fileInfo, UniqueFd(std::atoi(fileName.c_str())));
    }

    void GetFileHandleSync(::ohos::file::backup::FileMeta const& fileMeta) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }

        std::string bundleName(fileMeta.bundleName);
        std::string fileName(fileMeta.uri);
        bIncrementalRestoreSession->GetFileHandle(bundleName, fileName);
    }

    void ReleaseSync() {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }
        bIncrementalRestoreSession->Release();
    }

    int32_t cancel(::taihe::string_view bundleName) {
        int result = 0;
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return 0;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return 0;
        }
        result = bIncrementalRestoreSession->Cancel(std::string(bundleName));
        return result;
    }

    bool CleanBundleTempDirSync(::taihe::string_view bundleName) {
        bool result = false;
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return result;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return result;
        }
        ErrCode code = bIncrementalRestoreSession->CleanBundleTempDir(std::string(bundleName));
        if (BError(BError::Codes::OK) == code)
        {
            result = true;
        }else {
            ::taihe::set_business_error((int32_t)code, "CleanBundleTempDirSync error");
        }
        return result;
    }

    ::taihe::string GetCompatibilityInfoSync(::taihe::string_view bundleName, ::taihe::string_view extInfo) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return ::taihe::string("");
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return ::taihe::string("");
        }
        std::string bundleNameTemp(bundleName);
        std::string exInfoTemp(extInfo);
        auto compatInfo = std::make_shared<std::string>();
        bIncrementalRestoreSession->GetCompatibilityInfo(bundleNameTemp, exInfoTemp, *compatInfo);
        std::string result = *compatInfo;
        return ::taihe::string(result.c_str());
    }

private:
    ani_ref* getGeneCallBack()
    {
        return &callbackRef_;
    }
    ani_ref callbackRef_ = nullptr;
    std::unique_ptr<BIncrementalRestoreSession> bIncrementalRestoreSession = nullptr;
    std::shared_ptr<TaiheGeneralCallbacks> taiheGeneralCallbacks = nullptr;
};

class IncrementalBackupSessionImpl {
public:
    IncrementalBackupSessionImpl() {
        // Don't forget to implement the constructor.
    }

    explicit IncrementalBackupSessionImpl(uintptr_t callbacks) {
        ani_env *env = taihe::get_env();
        ani_object callbackObj = reinterpret_cast<ani_object>(callbacks);
        if (env == nullptr) {
            HILOGE("ani_env is nullptr");
            return;
        }
        ani_vm *vm = nullptr;
        env->GetVM(&vm);
        if (!vm) {
            HILOGE("vm is nullptr ");
            return;
        }
        ani_status status = ANI_OK;
        status = env->GlobalReference_Create(callbackObj, &callbackRef_);
        if (ANI_OK != status)
        {
            callbackRef_ = nullptr;
            HILOGE(" GlobalReference_Create failed");
            return;
        }
        taiheGeneralCallbacks = std::make_shared<TaiheGeneralCallbacks>(getGeneCallBack(), vm);
        ErrCode errCode;
        std::string errMsg;
        bIncrementalBackupSession = BIncrementalBackupSession::Init(BIncrementalBackupSession::Callbacks {
            .onFileReady = bind(&TaiheGeneralCallbacks::onFileReady, taiheGeneralCallbacks,
                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
            .onBundleStarted = bind(&TaiheGeneralCallbacks::onBundleStarted, taiheGeneralCallbacks,
                                    std::placeholders::_1, std::placeholders::_2),
            .onBundleFinished = bind(&TaiheGeneralCallbacks::onBundleFinished, taiheGeneralCallbacks,
                std::placeholders::_1, std::placeholders::_2),
            .onAllBundlesFinished = bind(&TaiheGeneralCallbacks::onAllBundlesFinished, taiheGeneralCallbacks,
                std::placeholders::_1),
            .onResultReport = bind(&TaiheGeneralCallbacks::onResultReport, taiheGeneralCallbacks,
                std::placeholders::_1, std::placeholders::_2),
            .onBackupServiceDied = bind(&TaiheGeneralCallbacks::onBackupServiceDied, taiheGeneralCallbacks),
            .onProcess = bind(&TaiheGeneralCallbacks::onProcess, taiheGeneralCallbacks,
                std::placeholders::_1, std::placeholders::_2),
            .onBackupSizeReport = bind(&TaiheGeneralCallbacks::onBackupSizeReport, taiheGeneralCallbacks,
                std::placeholders::_1)
        }, errMsg, errCode);
    }

    ::ohos::file::backup::FileData GetLocalCapabilitiesSync() {
        ::ohos::file::backup::FileData data;
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return data;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return data;
        }
        auto fd = make_shared<UniqueFd>();
        *fd = bIncrementalBackupSession->GetLocalCapabilities();
        HILOGE("UniqueFd.fd = %{public}d", fd->Get());
        data.fd = fd->Release();
        HILOGE("data.fd = %{public}d", data.fd);
        return data;
    }

    void GetBackupDataSizeSync(bool isPreciseScan, ::taihe::array_view<IncrementalBackupTime> dataList) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }
        std::vector<BIncrementalData> bundleNames;
        ::taihe::array<::ohos::file::backup::IncrementalBackupTime> dataListTemp(dataList);
        for (auto it = dataListTemp.begin(); it != dataListTemp.end(); ++it)
        {
            BIncrementalData data;
            data.bundleName = std::string(it->bundleName);
            data.lastIncrementalTime = it->lastIncrementalTime;
            bundleNames.push_back(data);
        }
        bIncrementalBackupSession->GetBackupDataSize(isPreciseScan, bundleNames);
    }

    void AppendBundlesSync(::taihe::array_view<::ohos::file::backup::IncrementalBackupData> bundlesToBackup) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }
        std::vector<BIncrementalData> backupBundles;
        std::vector<std::string> bundleInfos;
        ::taihe::array<::ohos::file::backup::IncrementalBackupData> bundles(bundlesToBackup);
        for (auto it = bundles.begin(); it != bundles.end(); ++it)
        {
            BIncrementalData data;
            data.bundleName = std::string(it->incrementalBackupTime.bundleName);
            data.lastIncrementalTime = it->incrementalBackupTime.lastIncrementalTime;
            data.manifestFd = it->fileManifestData.manifestFd;
            if (it->backupParams.parameters.has_value())
            {
                data.backupParameters = std::string(it->backupParams.parameters.value());
            }

            if (it->backupPriority.priority.has_value())
            {
                data.backupPriority = it->backupPriority.priority.value();
            }
            backupBundles.push_back(data);
        }
        bIncrementalBackupSession->AppendBundles(backupBundles);
    }

    void AppendBundlesSync2(::taihe::array_view<::ohos::file::backup::IncrementalBackupData> bundlesToBackup,
                            ::taihe::array_view<::taihe::string> infos) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }
        std::vector<BIncrementalData> backupBundles;
        std::vector<std::string> bundleInfos;
        ::taihe::array<::ohos::file::backup::IncrementalBackupData> bundles(bundlesToBackup);
        for (auto it = bundles.begin(); it != bundles.end(); ++it)
        {
            BIncrementalData data;
            data.bundleName = std::string(it->incrementalBackupTime.bundleName);
            data.lastIncrementalTime = it->incrementalBackupTime.lastIncrementalTime;
            data.manifestFd = it->fileManifestData.manifestFd;
            if (it->backupParams.parameters.has_value())
            {
                data.backupParameters = std::string(it->backupParams.parameters.value());
            }

            if (it->backupPriority.priority.has_value())
            {
                data.backupPriority = it->backupPriority.priority.value();
            }
            backupBundles.push_back(data);
        }

        ::taihe::array<::taihe::string> infosTemp(infos);
        for (auto it = infosTemp.begin(); it != infosTemp.end(); ++it)
        {
            bundleInfos.push_back(std::string(*it));
        }

        bIncrementalBackupSession->AppendBundles(backupBundles, bundleInfos);
    }

    void ReleaseSync() {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return;
        }
        bIncrementalBackupSession->Release();
    }

    int32_t cancel(::taihe::string_view bundleName) {
        int result = 0;
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return BACKUP_PERMISSION;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return SYSTEM_PERMISSION;
        }
        result = bIncrementalBackupSession->Cancel(std::string(bundleName));
        return result;
    }

    bool CleanBundleTempDirSync(::taihe::string_view bundleName) {
        bool result = false;
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return result;
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return result;
        }
        ErrCode code = bIncrementalBackupSession->CleanBundleTempDir(std::string(bundleName));
        if (BError(BError::Codes::OK) == code)
        {
            result = true;
        }else {
            ::taihe::set_business_error((int32_t)code, "CleanBundleTempDirSync error");
        }
        return result;
    }

    ::taihe::string GetCompatibilityInfoSync(::taihe::string_view bundleName, ::taihe::string_view extInfo) {
        if (!SAUtils::CheckBackupPermission()) {
            HILOGE("Has not permission!");
            ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
            return ::taihe::string("");
        }
        if (!SAUtils::IsSystemApp()) {
            HILOGE("System App check fail!");
            ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
            return ::taihe::string("");
        }
        std::string bundleNameTemp(bundleName);
        std::string exInfoTemp(extInfo);
        auto compatInfo = std::make_shared<std::string>();
        bIncrementalBackupSession->GetCompatibilityInfo(bundleNameTemp, exInfoTemp, *compatInfo);
        std::string result = *compatInfo;
        return ::taihe::string(result.c_str());
    }

private:
    ani_ref* getGeneCallBack()
    {
        return &callbackRef_;
    }
    ani_ref callbackRef_ = nullptr;
    std::unique_ptr<BIncrementalBackupSession> bIncrementalBackupSession = nullptr;
    std::shared_ptr<TaiheGeneralCallbacks> taiheGeneralCallbacks = nullptr;
};

::taihe::string getBackupVersion() {
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
        return ::taihe::string("");
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
        return ::taihe::string("");
    }
    std::string result = OHOS::FileManagement::Backup::BConstants::BACKUP_VERSION;
    return ::taihe::string(result);
}

::ohos::file::backup::FileData GetLocalCapabilitiesSync1() {
    ::ohos::file::backup::FileData data;
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
        return data;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
        return data;
    }
    auto fd = make_shared<UniqueFd>();
    ServiceClient::InvaildInstance();
    auto proxy = ServiceClient::GetInstance();
    if (!proxy) {
        HILOGI("called LocalCapabilities::AsyncCallback cbExec, failed to get proxy");
        ::taihe::set_business_error((int32_t)BackupErrorCode::E_IPCSS, "failed to get proxy");
        return data;
    }

    int fdNum = OHOS::INVALID_FD;
    proxy->GetLocalCapabilities(fdNum);
    UniqueFd fdData(fdNum);
    *fd = std::move(fdData);
    data.fd = fd->Release();
    return data;
}

FileData GetLocalCapabilitiesSync2(::taihe::array_view<::ohos::file::backup::IncrementalBackupTime> dataList) {
    ::ohos::file::backup::FileData data;
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
        return data;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
        return data;
    }
    std::vector<BIncrementalData> bundleNames;
    ::taihe::array<::ohos::file::backup::IncrementalBackupTime> dataListTemp(dataList);
    for (auto it = dataListTemp.begin(); it != dataListTemp.end(); ++it)
    {
        BIncrementalData data;
        data.bundleName = std::string(it->bundleName);
        data.lastIncrementalTime = it->lastIncrementalTime;
        bundleNames.push_back(data);
    }
    auto fd = make_shared<UniqueFd>();
    ServiceClient::InvaildInstance();
    auto proxy = ServiceClient::GetInstance();
    if (!proxy) {
        HILOGI("called LocalCapabilities::AsyncDataList cbExec, failed to get proxy");
        ::taihe::set_business_error((int32_t)BackupErrorCode::E_IPCSS, "failed to get proxy");
        return data;
    }
    int fdValue = INVALID_FD;
    proxy->GetLocalCapabilitiesIncremental(bundleNames, fdValue);
    UniqueFd fdData(fdValue);
    *fd = std::move(fdData);
    data.fd = fd->Release();
    return data;
}

::taihe::string getBackupInfo(::taihe::string_view bundleToBackup) {
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
        return ::taihe::string("");
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
        return ::taihe::string("");
    }

    std::string result;
    ServiceClient::InvaildInstance();
    auto proxy = ServiceClient::GetInstance();
    if (!proxy) {
        HILOGE("called DoGetBackupInfo,failed to get proxy");
        ::taihe::set_business_error((int32_t)BackupErrorCode::E_IPCSS, "failed to get proxy");
        return ::taihe::string("");
    }

    std::string bundleName(bundleToBackup);
    ErrCode errcode = proxy->GetBackupInfo(bundleName, result);
    if (errcode != 0) {
        HILOGE("proxy->GetBackupInfo faild.");
        ::taihe::set_business_error(errcode, "GetBackupInfo error");
        return ::taihe::string("");
    }

    return ::taihe::string(result);
}

bool updateTimer(::taihe::string_view bundleName, int32_t timeout) {
    bool result = false;
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
        return result;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
        return result;
    }

    std::string bundleName_(bundleName);
    uint32_t timeout_ = static_cast<uint32_t>(timeout);

    ServiceClient::InvaildInstance();
    auto proxy = ServiceClient::GetInstance();
    if (!proxy) {
        HILOGE("called DoUpdateTimer,failed to get proxy");
        ::taihe::set_business_error((int32_t)BackupErrorCode::E_IPCSS, "failed to get proxy");
        return result;
    }
    ErrCode errcode = proxy->UpdateTimer(bundleName_, timeout_, result);
    if (errcode != 0) {
        HILOGE("proxy->UpdateTimer faild.");
        ::taihe::set_business_error(errcode, "updateTimer error");
        return result;
    }
    return true;
}

bool updateSendRate(::taihe::string_view bundleName, int32_t sendRate) {
    bool result = false;
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        ::taihe::set_business_error(BACKUP_PERMISSION, "CheckBackupPermission error");
        return result;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        ::taihe::set_business_error(SYSTEM_PERMISSION, "IsSystemApp error");
        return result;
    }

    std::string bundleName_(bundleName);
    uint32_t sendRate_ = static_cast<uint32_t>(sendRate);

    ServiceClient::InvaildInstance();
    auto proxy = ServiceClient::GetInstance();
    if (!proxy) {
        HILOGE("called UpdateSendRate,failed to get proxy");
        ::taihe::set_business_error((int32_t)BackupErrorCode::E_IPCSS, "failed to get proxy");
        return result;
    }
    ErrCode errCode = proxy->UpdateSendRate(bundleName_, sendRate_, result);
    if (errCode != 0) {
        HILOGE("Proxy execute UpdateSendRate failed. errCode:%{public}d", errCode);
        ::taihe::set_business_error(errCode, "GetBackupInfo error");
        return result;
    }
    return true;
}

::ohos::file::backup::SessionBackup CreateSessionBackup(uintptr_t callbacks) {
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return taihe::make_holder<SessionBackupImpl, ::ohos::file::backup::SessionBackup>(callbacks);
}

::ohos::file::backup::SessionRestore CreateSessionRestore(uintptr_t callbacks) {
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return taihe::make_holder<SessionRestoreImpl, ::ohos::file::backup::SessionRestore>(callbacks);
}

::ohos::file::backup::IncrementalBackupSession CreateIncrementalBackupSession(uintptr_t callbacks) {
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return taihe::make_holder<IncrementalBackupSessionImpl, ::ohos::file::backup::IncrementalBackupSession>(callbacks);
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_getBackupVersion(getBackupVersion);
TH_EXPORT_CPP_API_GetLocalCapabilitiesSync1(GetLocalCapabilitiesSync1);
TH_EXPORT_CPP_API_GetLocalCapabilitiesSync2(GetLocalCapabilitiesSync2);
TH_EXPORT_CPP_API_getBackupInfo(getBackupInfo);
TH_EXPORT_CPP_API_updateTimer(updateTimer);
TH_EXPORT_CPP_API_updateSendRate(updateSendRate);
TH_EXPORT_CPP_API_CreateSessionBackup(CreateSessionBackup);
TH_EXPORT_CPP_API_CreateSessionRestore(CreateSessionRestore);
TH_EXPORT_CPP_API_CreateIncrementalBackupSession(CreateIncrementalBackupSession);
// NOLINTEND
