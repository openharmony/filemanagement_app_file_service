/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_JS_RUNTIME_MOCK_H
#define OHOS_FILEMGMT_BACKUP_JS_RUNTIME_MOCK_H

#include <gmock/gmock.h>

#include "runtime.h"

namespace OHOS::FileManagement::Backup {
class JsRuntimeMock : public AbilityRuntime::JsRuntime {
public:
    MOCK_METHOD(Language, GetLanguage, (), (const));
    MOCK_METHOD(void, StartDebugMode, (const DebugOption debugOption));
    MOCK_METHOD(void, DumpHeapSnapshot, (bool isPrivate));
    MOCK_METHOD(void, DumpCpuProfile, ());
    MOCK_METHOD(void, DestroyHeapProfiler, ());
    MOCK_METHOD(void, ForceFullGC, ());
    MOCK_METHOD(void, ForceFullGC, (uint32_t tid));
    MOCK_METHOD(void, DumpHeapSnapshot, (uint32_t tid, bool isFullGC, bool isBinary));
    MOCK_METHOD(void, AllowCrossThreadExecution, ());
    MOCK_METHOD(void, GetHeapPrepare, ());
    MOCK_METHOD(void, NotifyApplicationState, (bool isBackground));
    MOCK_METHOD(bool, SuspendVM, (uint32_t tid));
    MOCK_METHOD(void, ResumeVM, (uint32_t tid));
    MOCK_METHOD(void, PreloadSystemModule, (const std::string& moduleName));
    MOCK_METHOD(void, FinishPreload, ());
    MOCK_METHOD(bool, LoadRepairPatch, (const std::string& patchFile, const std::string& baseFile));
    MOCK_METHOD(bool, NotifyHotReloadPage, ());
    MOCK_METHOD(bool, UnLoadRepairPatch, (const std::string& patchFile));
    MOCK_METHOD(void, RegisterQuickFixQueryFunc, ((const std::map<std::string, std::string>&) moduleAndPath));
    MOCK_METHOD(void, StartProfiler, (const DebugOption debugOption));
    MOCK_METHOD(void, DoCleanWorkAfterStageCleaned, ());
    MOCK_METHOD(void, SetModuleLoadChecker, (const std::shared_ptr<ModuleCheckerDelegate>), (const));
    MOCK_METHOD(void, SetDeviceDisconnectCallback, (const std::function<bool()> &cb));
    MOCK_METHOD(void, UpdatePkgContextInfoJson, (std::string moduleName, std::string hapPath, std::string packageName));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_JS_RUNTIME_MOCK_H
