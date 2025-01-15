/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#ifndef INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_GENERAL_CALLBACKS_H
#define INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_GENERAL_CALLBACKS_H

#include <memory>
#include <node_api.h>
#include <string>

#include "filemgmt_libn.h"

namespace OHOS::FileManagement::Backup {
using InputArgsParser = std::function<bool(napi_env, std::vector<napi_value> &)>;

class BackupRestoreCallback {
public:
    BackupRestoreCallback(napi_env env, LibN::NVal thisPtr, LibN::NVal cb);
    ~BackupRestoreCallback();
    void CallJsMethod(InputArgsParser argParser);
    explicit operator bool() const;
    void CleanRef();
private:
    napi_env env_;
    LibN::NAsyncContextCallback *ctx_ = nullptr;
};

struct WorkArgs {
    std::mutex callbackMutex;
    std::condition_variable callbackCondition;
    std::atomic<bool> isReady {false};
    BackupRestoreCallback *ptr = nullptr;
    InputArgsParser argParser;
};

class GeneralCallbacks {
public:
    GeneralCallbacks(const napi_env &env, const LibN::NVal &thisPtr, const LibN::NVal &jsCallbacks)
        : onFileReady(env, thisPtr, jsCallbacks.GetProp("onFileReady")),
          onBundleBegin(env, thisPtr, jsCallbacks.GetProp("onBundleBegin")),
          onBundleEnd(env, thisPtr, jsCallbacks.GetProp("onBundleEnd")),
          onAllBundlesEnd(env, thisPtr, jsCallbacks.GetProp("onAllBundlesEnd")),
          onBackupServiceDied(env, thisPtr, jsCallbacks.GetProp("onBackupServiceDied")),
          onResultReport(env, thisPtr, jsCallbacks.GetProp("onResultReport")),
          onProcess(env, thisPtr, jsCallbacks.GetProp("onProcess")),
          onBackupSizeReport(env, thisPtr, jsCallbacks.GetProp("onBackupSizeReport")) {};
public:
    void RemoveCallbackRef();
public:
    LibN::NAsyncWorkCallback onFileReady;
    LibN::NAsyncWorkCallback onBundleBegin;
    LibN::NAsyncWorkCallback onBundleEnd;
    LibN::NAsyncWorkCallback onAllBundlesEnd;
    BackupRestoreCallback onBackupServiceDied;
    BackupRestoreCallback onResultReport;
    BackupRestoreCallback onProcess;
    BackupRestoreCallback onBackupSizeReport;
};
} // namespace OHOS::FileManagement::Backup
#endif // INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_GENERAL_CALLBACKS_H