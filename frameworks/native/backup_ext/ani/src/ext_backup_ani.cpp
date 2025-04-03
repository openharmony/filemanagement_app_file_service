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

#include "ext_backup_ani.h"
#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ExtBackupAni::ExtBackupAni(AbilityRuntime::Runtime &runtime)
    : stsRuntime_(static_cast<AbilityRuntime::STSRuntime &>(runtime))
{
    HILOGI("Create as an BackupExtensionAbility");
    env_ = stsRuntime_.GetAniEnv();
}

ExtBackupAni::~ExtBackupAni()
{
    HILOGI("ExtBackupAni::~ExtBackupAni.");
}

void ExtBackupAni::Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
                        const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
                        std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
                        const sptr<IRemoteObject> &token)
{
    HILOGI("Init the BackupExtensionAbility");
    try {
        if (record == nullptr) {
            HILOGE("record null");
            return;
        }
        Extension::Init(record, application, handler, token);
        if (Extension::abilityInfo_ == nullptr || Extension::abilityInfo_->srcEntrance.empty()) {
            HILOGE("BackupExtensionAbility Init abilityInfo error");
            return;
        }
        std::string srcPath(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        auto pos = srcPath.rfind(".");
        if (pos != std::string::npos) {
            srcPath.erase(pos);
            srcPath.append(".abc");
        }
        std::string moduleName(Extension::abilityInfo_->moduleName);
        moduleName.append("::").append(abilityInfo_->name);
        etsObj_ = stsRuntime_.LoadModule(moduleName, srcPath, abilityInfo_->hapPath,
                                         abilityInfo_->compileMode == AppExecFwk::CompileMode::ES_MODULE, false,
                                         abilityInfo_->srcEntrance);
        if (etsObj_ == nullptr) {
            HILOGE("Failed to get etsObj");
            return;
        }
    } catch (const BError &e) {
        HILOGE("%{public}s", e.what());
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
    }
}

ErrCode ExtBackupAni::CallEtsOnBackup()
{
    if (ANI_OK != env_->Object_CallMethodByName_Void(etsObj_->aniObj, "onBackup", nullptr)) {
        HILOGE("Failed to call the method: onBackup");
        return EINVAL;
    }
    return ERR_OK;
}

ErrCode ExtBackupAni::CallEtsOnRestore()
{
    ani_object bundleVersionObj = AniObjectUtils::Create(env_, "LBundleVersionInner;");
    if (nullptr == bundleVersionObj) {
        HILOGE("Failed to Create the BundleVersionInner");
        return EINVAL;
    }
    if (ANI_OK != env_->Object_CallMethodByName_Void(etsObj_->aniObj, "onRestore", nullptr, bundleVersionObj)) {
        HILOGE("Failed to call the method: onRestore");
        return EINVAL;
    }
    return ERR_OK;
}

ExtBackupAni *ExtBackupAni::Create(const std::unique_ptr<AbilityRuntime::Runtime> &runtime)
{
    HILOGI("Create as an BackupExtensionAbility");
    return new ExtBackupAni(*runtime);
}

ErrCode ExtBackupAni::OnBackup(function<void(ErrCode, std::string)> callback,
                               std::function<void(ErrCode, const std::string)> callbackEx)
{
    HILOGI("ExtBackupAni OnBackup");
    BExcepUltils::BAssert(etsObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
                          "The app does not provide the onBackup interface.");
    return CallEtsOnBackup();
}

ErrCode ExtBackupAni::OnRestore(std::function<void(ErrCode, std::string)> callback,
                                std::function<void(ErrCode, const std::string)> callbackEx)
{
    HILOGI("ExtBackupAni OnRestore");
    BExcepUltils::BAssert(etsObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
                          "The app does not provide the OnRestore interface.");
    return CallEtsOnRestore();
}

} // namespace OHOS::FileManagement::Backup