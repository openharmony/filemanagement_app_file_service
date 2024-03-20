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

#ifndef OHOS_FILEMGMT_BACKUP_EXT_BACKUP_JS_H
#define OHOS_FILEMGMT_BACKUP_EXT_BACKUP_JS_H

#include <memory>
#include <string_view>
#include <tuple>
#include <vector>

#include "b_resources/b_constants.h"
#include "ext_backup.h"
#include "js_runtime.h"
#include "native_reference.h"
#include "native_value.h"
#include "unique_fd.h"
#include "want.h"

namespace OHOS::FileManagement::Backup {
using InputArgsParser = std::function<bool(napi_env, std::vector<napi_value> &)>;
using ResultValueParser = std::function<bool(napi_env, napi_value)>;

struct CallJsParam {
    std::mutex backupOperateMutex;
    std::condition_variable backupOperateCondition;
    bool isReady = false;
    std::string funcName;
    AbilityRuntime::JsRuntime *jsRuntime;
    NativeReference *jsObj;
    InputArgsParser argParser;
    ResultValueParser retParser;

    CallJsParam(const std::string &funcNameIn,
                AbilityRuntime::JsRuntime *jsRuntimeIn,
                NativeReference *jsObjIn,
                InputArgsParser &argParserIn,
                ResultValueParser &retParserIn)
        : funcName(funcNameIn), jsRuntime(jsRuntimeIn), jsObj(jsObjIn), argParser(argParserIn), retParser(retParserIn)
    {
    }
};

struct CallBackInfo {
    std::function<void()> callback;
    std::function<void(const std::string &restoreRetInfo)> callbackEx;

    CallBackInfo(std::function<void()> callbackIn) : callback(callbackIn) {}
    CallBackInfo( std::function<void(const std::string &restoreRetInfo)> callbackEx) : callbackEx(callbackEx) {}
};

class ExtBackupJs : public ExtBackup {
public:
    /**
     * @brief Init the extension.
     *
     * @param record the extension record.
     * @param application the application info.
     * @param handler the extension handler.
     * @param token the remote token.
     */
    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
              const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
              std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
              const sptr<IRemoteObject> &token) override;

public:
    /**
     * @brief Create ExtBackupJs.
     *
     * @param runtime The runtime.
     * @return The ExtBackupJs instance.
     */
    static ExtBackupJs *Create(const std::unique_ptr<AbilityRuntime::Runtime> &runtime);

    /**
     * @brief Call the app's OnBackup.
     *
     * @param callback The callback.
     */
    ErrCode OnBackup(std::function<void()> callback) override;

    /**
     * @brief Call the app's OnRestore.
     *
     * @param callbackEx The callbackEx.
     * @param callback The callBack.
     */
    ErrCode OnRestore(std::function<void(const std::string &restoreRetInfo)> callbackEx,
        std::function<void()> callback) override;

public:
    explicit ExtBackupJs(AbilityRuntime::JsRuntime &jsRuntime) : jsRuntime_(jsRuntime) {}
    ~ExtBackupJs()
    {
        jsRuntime_.FreeNativeReference(std::move(jsObj_));
    }

private:
    int CallJsMethod(const std::string &funcName,
                     AbilityRuntime::JsRuntime &jsRuntime,
                     NativeReference *jsObj,
                     InputArgsParser argParser,
                     ResultValueParser retParser);
    std::tuple<ErrCode, napi_value> CallObjectMethod(std::string_view name, const std::vector<napi_value> &argv = {});
    std::function<bool(napi_env env, std::vector<napi_value> &argv)> ParseRestoreExInfo();
    std::function<bool(napi_env env, std::vector<napi_value> &argv)> ParseRestoreInfo();

    void ExportJsContext(void);

    AbilityRuntime::JsRuntime &jsRuntime_;
    std::unique_ptr<NativeReference> jsObj_;
    std::shared_ptr<CallBackInfo> callbackInfoEx_;
    std::shared_ptr<CallBackInfo> callbackInfo_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_EXT_BACKUP_JS_H
