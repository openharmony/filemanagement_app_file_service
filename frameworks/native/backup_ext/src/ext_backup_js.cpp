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

#include "ext_backup_js.h"

#include <cstdio>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_extension_config.h"
#include "b_resources/b_constants.h"
#include "bundle_mgr_client.h"
#include "ext_extension.h"
#include "filemgmt_libhilog.h"
#include "js_runtime_utils.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void ExtBackupJs::OnStart(const AAFwk::Want &want)
{
    HILOGI("BackupExtensionAbility(JS) was started");
    Extension::OnStart(want);
}

static string GetSrcPath(const AppExecFwk::AbilityInfo &info)
{
    using AbilityRuntime::Extension;
    stringstream ss;

    // API9(stage model) 中通过 $(module)$(name)/$(srcEntrance/(.*$)/(.abc)) 获取自定义插件路径
    if (!info.srcEntrance.empty()) {
        ss << info.moduleName << '/' << string(info.srcEntrance, 0, info.srcEntrance.rfind(".")) << ".abc";
        return ss.str();
    }
    return "";
}

void ExtBackupJs::Init(const shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
                       const shared_ptr<AppExecFwk::OHOSApplication> &application,
                       shared_ptr<AppExecFwk::AbilityHandler> &handler,
                       const sptr<IRemoteObject> &token)
{
    HILOGI("Init the BackupExtensionAbility(JS)");
    try {
        ExtBackup::Init(record, application, handler, token);
        BExcepUltils::BAssert(abilityInfo_, BError::Codes::EXT_BROKEN_FRAMEWORK, "Invalid abilityInfo_");
        // 获取应用扩展的 BackupExtensionAbility 的路径
        const AppExecFwk::AbilityInfo &info = *abilityInfo_;
        string bundleName = info.bundleName;
        string moduleName(info.moduleName + "::" + info.name);
        string modulePath = GetSrcPath(info);
        int moduleType = static_cast<int>(info.type);
        HILOGI("Try to load %{public}s's %{public}s(type %{public}d) from %{public}s", bundleName.c_str(),
               moduleName.c_str(), moduleType, modulePath.c_str());

        // 加载用户扩展 BackupExtensionAbility 到 JS 引擎，并将之暂存在 jsObj_ 中。注意，允许加载失败，往后执行默认逻辑
        AbilityRuntime::HandleScope handleScope(jsRuntime_);
        jsObj_ = jsRuntime_.LoadModule(moduleName, modulePath, info.hapPath);
        if (jsObj_) {
            HILOGI("Wow! Here's a custsom BackupExtensionAbility");
        } else {
            HILOGW("Oops! There's no custom BackupExtensionAbility");
        }
    } catch (const BError &e) {
        HILOGE("%{public}s", e.what());
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
    }
}

[[maybe_unused]] tuple<ErrCode, NativeValue *> ExtBackupJs::CallObjectMethod(string_view name,
                                                                             const vector<NativeValue *> &argv)
{
    HILOGI("Call %{public}s", name.data());

    if (!jsObj_) {
        return {BError(BError::Codes::EXT_BROKEN_FRAMEWORK, "Invalid jsObj_").GetCode(), nullptr};
    }

    AbilityRuntime::HandleScope handleScope(jsRuntime_);

    NativeValue *value = jsObj_->Get();
    NativeObject *obj = AbilityRuntime::ConvertNativeValueTo<NativeObject>(value);
    if (!obj) {
        return {BError(BError::Codes::EXT_INVAL_ARG, "The custom BackupAbilityExtension is required to be an object")
                    .GetCode(),
                nullptr};
    }

    NativeValue *method = obj->GetProperty(name.data());
    if (!method || method->TypeOf() != NATIVE_FUNCTION) {
        return {BError(BError::Codes::EXT_INVAL_ARG, string(name).append(" is required to be a function")).GetCode(),
                nullptr};
    }

    auto ret = jsRuntime_.GetNativeEngine().CallFunction(value, method, argv.data(), argv.size());
    if (!ret) {
        return {BError(BError::Codes::EXT_INVAL_ARG, string(name).append(" raised an exception")).GetCode(), nullptr};
    }
    return {BError(BError::Codes::OK).GetCode(), ret};
}

void ExtBackupJs::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    HILOGI("BackupExtensionAbility(JS) was invoked. restart=%{public}d, startId=%{public}d", restart, startId);

    // REM: 处理返回结果 ret
    // REM: 通过杀死进程实现 Stop
}

ExtBackupJs *ExtBackupJs::Create(const unique_ptr<AbilityRuntime::Runtime> &runtime)
{
    HILOGI("Create as an BackupExtensionAbility(JS)");
    return new ExtBackupJs(static_cast<AbilityRuntime::JsRuntime &>(*runtime));
}

string ExtBackupJs::GetUsrConfig() const
{
    vector<string> config;
    AppExecFwk::BundleMgrClient client;
    BExcepUltils::BAssert(abilityInfo_, BError::Codes::EXT_BROKEN_FRAMEWORK, "Invalid abilityInfo_");
    const AppExecFwk::AbilityInfo &info = *abilityInfo_;
    if (!client.GetProfileFromAbility(info, "ohos.extension.backup", config)) {
        throw BError(BError::Codes::EXT_INVAL_ARG, "Failed to invoke the GetProfileFromAbility method.");
    }

    return config.empty() ? "" : config[0];
}

bool ExtBackupJs::AllowToBackupRestore() const
{
    string usrConfig = GetUsrConfig();
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(usrConfig);
    auto cache = cachedEntity.Structuralize();
    if (cache.GetAllowToBackupRestore()) {
        return true;
    }
    return false;
}

BConstants::ExtensionAction ExtBackupJs::GetExtensionAction() const
{
    return extAction_;
}

static BConstants::ExtensionAction VerifyAndGetAction(const AAFwk::Want &want,
                                                      std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo)
{
    string pendingMsg = "Received an empty ability. You must missed the init proc";
    BExcepUltils::BAssert(abilityInfo, BError::Codes::EXT_INVAL_ARG, pendingMsg);
    using namespace BConstants;
    ExtensionAction extAction {want.GetIntParam(EXTENSION_ACTION_PARA, static_cast<int>(ExtensionAction::INVALID))};
    if (extAction == ExtensionAction::INVALID) {
        int extActionInt = static_cast<int>(extAction);
        pendingMsg = string("Want must specify a valid action instead of ").append(to_string(extActionInt));
        throw BError(BError::Codes::EXT_INVAL_ARG, pendingMsg);
    }

    return extAction;
}

sptr<IRemoteObject> ExtBackupJs::OnConnect(const AAFwk::Want &want)
{
    try {
        HILOGI("begin");
        BExcepUltils::BAssert(abilityInfo_, BError::Codes::EXT_BROKEN_FRAMEWORK, "Invalid abilityInfo_");
        // 发起者必须是备份服务
        auto extAction = VerifyAndGetAction(want, abilityInfo_);
        if (extAction_ != BConstants::ExtensionAction::INVALID && extAction == BConstants::ExtensionAction::INVALID &&
            extAction_ != extAction) {
            HILOGI("Verification failed.");
            return nullptr;
        }
        // 应用必须配置支持备份恢复
        if (!AllowToBackupRestore()) {
            HILOGI("The application does not allow to backup and restore.");
            return nullptr;
        }
        extAction_ = extAction;

        Extension::OnConnect(want);

        auto remoteObject =
            sptr<BackupExtExtension>(new BackupExtExtension(std::static_pointer_cast<ExtBackupJs>(shared_from_this())));
        HILOGI("end");
        return remoteObject->AsObject();
    } catch (const BError &e) {
        return nullptr;
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
        return nullptr;
    } catch (...) {
        HILOGE("");
        return nullptr;
    }
}

void ExtBackupJs::OnDisconnect(const AAFwk::Want &want)
{
    try {
        HILOGI("begin");
        Extension::OnDisconnect(want);
        extAction_ = BConstants::ExtensionAction::INVALID;
        HILOGI("end");
    } catch (const BError &e) {
        return;
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
        return;
    } catch (...) {
        HILOGE("");
        return;
    }
}
} // namespace OHOS::FileManagement::Backup