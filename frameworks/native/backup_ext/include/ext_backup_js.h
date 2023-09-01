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
using InputArgsParser = std::function<bool(NativeEngine &, std::vector<NativeValue *> &)>;
using ResultValueParser = std::function<bool(NativeEngine &, NativeValue *)>;

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

    CallBackInfo(std::function<void()> callbackIn) : callback(callbackIn) {}
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
     * @param callback The callback.
     */
    ErrCode OnRestore(std::function<void()> callback) override;

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
    std::tuple<ErrCode, NativeValue *> CallObjectMethod(std::string_view name,
                                                        const std::vector<NativeValue *> &argv = {});

    void ExportJsContext(void);

    AbilityRuntime::JsRuntime &jsRuntime_;
    std::unique_ptr<NativeReference> jsObj_;
    std::shared_ptr<CallBackInfo> callbackInfo_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_EXT_BACKUP_JS_H
