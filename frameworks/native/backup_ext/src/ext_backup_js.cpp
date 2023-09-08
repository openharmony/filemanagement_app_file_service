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
#include <memory>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

#include "bundle_mgr_client.h"
#include "js_extension_context.h"
#include "js_runtime.h"
#include "js_runtime_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_common_util.h"
#include "napi_common_want.h"
#include "napi_remote_object.h"
#include "unique_fd.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_extension_config.h"
#include "b_resources/b_constants.h"
#include "ext_extension.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

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

static NativeValue *PromiseCallback(NativeEngine *engine, NativeCallbackInfo *info)
{
    if (info == nullptr || info->functionInfo == nullptr || info->functionInfo->data == nullptr) {
        HILOGI("PromiseCallback, Invalid input info.");
        return nullptr;
    }
    void *data = info->functionInfo->data;
    auto *callbackInfo = static_cast<CallBackInfo *>(data);
    callbackInfo->callback();
    info->functionInfo->data = nullptr;
    return nullptr;
}

static bool CheckPromise(NativeValue *result)
{
    if (result == nullptr) {
        HILOGE("CheckPromise, result is null, no need to call promise.");
        return false;
    }
    if (!result->IsPromise()) {
        HILOGE("CheckPromise, result is not promise, no need to call promise.");
        return false;
    }
    return true;
}

static bool CallPromise(AbilityRuntime::JsRuntime &jsRuntime, NativeValue *result, CallBackInfo *callbackInfo)
{
    auto *retObj = AbilityRuntime::ConvertNativeValueTo<NativeObject>(result);
    if (retObj == nullptr) {
        HILOGI("CallPromise, Failed to convert native value to NativeObject.");
        return false;
    }
    NativeValue *then = retObj->GetProperty("then");
    if (then == nullptr) {
        HILOGI("CallPromise, Failed to get property: then.");
        return false;
    }
    if (!then->IsCallable()) {
        HILOGI("CallPromise, property then is not callable.");
        return false;
    }
    AbilityRuntime::HandleScope handleScope(jsRuntime);
    auto &nativeEngine = jsRuntime.GetNativeEngine();
    auto promiseCallback =
        nativeEngine.CreateFunction("promiseCallback", strlen("promiseCallback"), PromiseCallback, callbackInfo);
    NativeValue *argv[1] = {promiseCallback};
    nativeEngine.CallFunction(result, then, argv, 1);
    return true;
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
        jsObj_ = jsRuntime_.LoadModule(moduleName, modulePath, info.hapPath,
                                       abilityInfo_->compileMode == AbilityRuntime::CompileMode::ES_MODULE);
        if (jsObj_ == nullptr) {
            HILOGW("Oops! There's no custom BackupExtensionAbility");
            return;
        }
        HILOGI("Wow! Here's a custsom BackupExtensionAbility");
        ExportJsContext();
    } catch (const BError &e) {
        HILOGE("%{public}s", e.what());
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
    }
}

void ExtBackupJs::ExportJsContext(void)
{
    auto &engine = jsRuntime_.GetNativeEngine();
    NativeObject *obj = AbilityRuntime::ConvertNativeValueTo<NativeObject>(jsObj_->Get());
    if (obj == nullptr) {
        HILOGE("Failed to get BackupExtAbility object");
        return;
    }

    auto context = GetContext();
    if (context == nullptr) {
        HILOGE("Failed to get context");
        return;
    }

    HILOGI("CreateBackupExtAbilityContext");
    NativeValue *contextObj = CreateJsExtensionContext(engine, context);
    auto contextRef = jsRuntime_.LoadSystemModule("application.ExtensionContext", &contextObj, 1);
    contextObj = contextRef->Get();
    HILOGI("Bind context");
    context->Bind(jsRuntime_, contextRef.release());
    obj->SetProperty("context", contextObj);

    auto nativeObj = AbilityRuntime::ConvertNativeValueTo<NativeObject>(contextObj);
    if (nativeObj == nullptr) {
        HILOGE("Failed to get backup extension ability native object");
        return;
    }

    HILOGI("Set backup extension ability context pointer is nullptr: %{public}d", context.get() == nullptr);
    auto releaseContext = [](NativeEngine *, void *data, void *) {
        HILOGI("Finalizer for weak_ptr backup extension ability context is called");
        delete static_cast<std::weak_ptr<AbilityRuntime::Context> *>(data);
    };
    nativeObj->SetNativePointer(new std::weak_ptr<AbilityRuntime::Context>(context), releaseContext, nullptr);
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

ExtBackupJs *ExtBackupJs::Create(const unique_ptr<AbilityRuntime::Runtime> &runtime)
{
    HILOGI("Create as an BackupExtensionAbility(JS)");
    return new ExtBackupJs(static_cast<AbilityRuntime::JsRuntime &>(*runtime));
}

ErrCode ExtBackupJs::OnBackup(function<void()> callback)
{
    HILOGI("BackupExtensionAbility(JS) OnBackup.");
    BExcepUltils::BAssert(jsObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
                          "The app does not provide the onRestore interface.");
    callbackInfo_ = std::make_shared<CallBackInfo>(callback);
    auto retParser = [jsRuntime {&jsRuntime_}, callbackInfo {callbackInfo_}](NativeEngine &engine,
                                                                                       NativeValue *result) -> bool {
        if (!CheckPromise(result)) {
            callbackInfo->callback();
            return true;
        }
        HILOGI("CheckPromise(JS) OnBackup ok.");
        return CallPromise(*jsRuntime, result, callbackInfo.get());
    };

    auto errCode = CallJsMethod("onBackup", jsRuntime_, jsObj_.get(), {}, retParser);
    if (errCode != ERR_OK) {
        HILOGE("CallJsMethod error, code:%{public}d.", errCode);
    }
    return errCode;
}

ErrCode ExtBackupJs::OnRestore(function<void()> callback)
{
    HILOGI("BackupExtensionAbility(JS) OnRestore.");
    BExcepUltils::BAssert(jsObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
                          "The app does not provide the onRestore interface.");

    auto argParser = [appVersionCode(appVersionCode_),
                      appVersionStr(appVersionStr_)](NativeEngine &engine, vector<NativeValue *> &argv) -> bool {
        NativeValue *verCode = engine.CreateNumber(appVersionCode);
        NativeValue *verStr = engine.CreateString(appVersionStr.c_str(), appVersionStr.length());
        NativeValue *param = engine.CreateObject();
        auto paramObj = reinterpret_cast<NativeObject *>(param->GetInterface(NativeObject::INTERFACE_ID));
        paramObj->SetProperty("code", verCode);
        paramObj->SetProperty("name", verStr);
        argv.push_back(param);
        return true;
    };
    callbackInfo_ = std::make_shared<CallBackInfo>(callback);
    auto retParser = [jsRuntime {&jsRuntime_}, callbackInfo {callbackInfo_}](NativeEngine &engine,
                                                                                       NativeValue *result) -> bool {
        if (!CheckPromise(result)) {
            callbackInfo->callback();
            return true;
        }
        HILOGI("CheckPromise(JS) OnRestore ok.");
        return CallPromise(*jsRuntime, result, callbackInfo.get());
    };

    auto errCode = CallJsMethod("onRestore", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOGE("CallJsMethod error, code:%{public}d.", errCode);
    }
    return errCode;
}

static int DoCallJsMethod(CallJsParam *param)
{
    AbilityRuntime::JsRuntime *jsRuntime = param->jsRuntime;
    if (jsRuntime == nullptr) {
        HILOGE("failed to get jsRuntime.");
        return EINVAL;
    }
    AbilityRuntime::HandleEscape handleEscape(*jsRuntime);
    auto &nativeEngine = jsRuntime->GetNativeEngine();
    vector<NativeValue *> argv = {};
    if (param->argParser != nullptr) {
        if (!param->argParser(nativeEngine, argv)) {
            HILOGE("failed to get params.");
            return EINVAL;
        }
    }
    NativeValue *value = param->jsObj->Get();
    if (value == nullptr) {
        HILOGE("failed to get native value object.");
        return EINVAL;
    }
    NativeObject *obj = AbilityRuntime::ConvertNativeValueTo<NativeObject>(value);
    if (obj == nullptr) {
        HILOGE("failed to get BackupExtAbility object.");
        return EINVAL;
    }
    NativeValue *method = obj->GetProperty(param->funcName.c_str());
    if (method == nullptr) {
        HILOGE("failed to get %{public}s from BackupExtAbility object.", param->funcName.c_str());
        return EINVAL;
    }
    if (param->retParser == nullptr) {
        HILOGE("ResultValueParser must not null.");
        return EINVAL;
    }
    if (!param->retParser(nativeEngine,
                          handleEscape.Escape(nativeEngine.CallFunction(value, method, argv.data(), argv.size())))) {
        HILOGI("Parser js result fail.");
        return EINVAL;
    }
    return ERR_OK;
}

int ExtBackupJs::CallJsMethod(const std::string &funcName,
                              AbilityRuntime::JsRuntime &jsRuntime,
                              NativeReference *jsObj,
                              InputArgsParser argParser,
                              ResultValueParser retParser)
{
    uv_loop_s *loop = nullptr;
    napi_status status = napi_get_uv_event_loop(reinterpret_cast<napi_env>(&jsRuntime.GetNativeEngine()), &loop);
    if (status != napi_ok) {
        HILOGE("failed to get uv event loop.");
        return EINVAL;
    }
    auto param = std::make_shared<CallJsParam>(funcName, &jsRuntime, jsObj, argParser, retParser);
    BExcepUltils::BAssert(param, BError::Codes::EXT_BROKEN_FRAMEWORK, "failed to new param.");

    auto work = std::make_shared<uv_work_t>();
    BExcepUltils::BAssert(work, BError::Codes::EXT_BROKEN_FRAMEWORK, "failed to new uv_work_t.");

    work->data = reinterpret_cast<void *>(param.get());
    int ret = uv_queue_work(
        loop, work.get(), [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            CallJsParam *param = reinterpret_cast<CallJsParam *>(work->data);
            do {
                if (param == nullptr) {
                    HILOGE("failed to get CallJsParam.");
                    break;
                }
                if (DoCallJsMethod(param) != ERR_OK) {
                    HILOGE("failed to call DoCallJsMethod.");
                }
            } while (false);
            std::unique_lock<std::mutex> lock(param->backupOperateMutex);
            param->isReady = true;
            param->backupOperateCondition.notify_one();
        });
    if (ret != 0) {
        HILOGE("failed to exec uv_queue_work.");
        return EINVAL;
    }
    std::unique_lock<std::mutex> lock(param->backupOperateMutex);
    param->backupOperateCondition.wait(lock, [param]() { return param->isReady; });

    return ERR_OK;
}
} // namespace OHOS::FileManagement::Backup
