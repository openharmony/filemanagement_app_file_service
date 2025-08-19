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

#include "incr_backup_session_transfer.h"

#include <array>
#include <bit>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/arkts_esvalue.h"
#include "filemgmt_libhilog.h"
#include "incremental_backup_session.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "session_incremental_backup_n_exporter.h"

namespace OHOS::FileManagement::Backup {
namespace {
const char *INCR_BACKUP_SESSION_TRANSFER_CLASS_NAME = "L@ohos/backup/transfer/backup/IncrBackupSessionTransfer;";
const char *INCR_BACKUP_SESSION_CLASS_NAME = "L@ohos/backup/transfer/backup/IncrementalBackupSession;";
}

void IncreBackupSessionTransfer::Init(ani_env *aniEnv)
{
    HILOGD("Init transfer native method begin");
    if (aniEnv == nullptr) {
        HILOGE("aniEnv is null");
        return;
    }

    ani_class cls = nullptr;
    auto status = aniEnv->FindClass(INCR_BACKUP_SESSION_TRANSFER_CLASS_NAME, &cls);
    if (status != ANI_OK) {
        HILOGE("FindClass failed status: %{public}d", status);
        return;
    }

    std::array nativeFuncs = {
        ani_native_function { "transferStaticSession", nullptr,
            reinterpret_cast<void*>(IncreBackupSessionTransfer::TransferStaticSession)},
        ani_native_function { "transferDynamicSession", nullptr,
            reinterpret_cast<void*>(IncreBackupSessionTransfer::TransferDynamicSession)},
    };
    status = aniEnv->Class_BindStaticNativeMethods(cls, nativeFuncs.data(), nativeFuncs.size());
    if (status != ANI_OK) {
        HILOGE("Class_BindNativeMethods failed status: %{public}d", status);
        return;
    }
    HILOGD("Init transfer native method end");
}

ani_object IncreBackupSessionTransfer::TransferStaticSession(ani_env *aniEnv, ani_class aniCls, ani_object input)
{
    HILOGD("Transfer incrBackupSession Static begin");
    if (aniEnv == nullptr) {
        HILOGE("aniEnv is null");
        return nullptr;
    }

    void *unwrapResult = nullptr;
    bool unwrapRet = arkts_esvalue_unwrap(aniEnv, input, &unwrapResult);
    if (!unwrapRet) {
        HILOGE("Failed to unwrap");
        return nullptr;
    }
    if (unwrapResult == nullptr) {
        HILOGE("UnwrapResult is nullptr");
        return nullptr;
    }
    // 1.1->1.2
    IncrementalBackupSession* entity = reinterpret_cast<IncrementalBackupSession*>(unwrapResult);
    ani_class cls = nullptr;
    ani_status ret = ANI_ERROR;
    if ((ret = aniEnv->FindClass(INCR_BACKUP_SESSION_CLASS_NAME, &cls)) != ANI_OK) {
        HILOGE("Call FindClass failed, ret = %{public}d", ret);
        return nullptr;
    }
    ani_method constructFunc;
    if ((ret = aniEnv->Class_FindMethod(cls, "<ctor>", nullptr, &constructFunc)) != ANI_OK) {
        HILOGE("Call Class_FindMethod failed, ret = %{public}d", ret);
        return nullptr;
    }
    ani_object outObj;
    ret = aniEnv->Object_New(cls, constructFunc, &outObj, entity->session.release(), entity->callbacks.get());
    if (ret != ANI_OK) {
        HILOGE("Call Object_New failed, ret = %{public}d", ret);
        return nullptr;
    }
    HILOGD("Transfer incrBackupSession Static end");
    return outObj;
}

ani_ref IncreBackupSessionTransfer::TransferDynamicSession(ani_env *aniEnv, ani_class aniCls, ani_object input)
{
    HILOGD("TransferDynamicSession start");
    if (aniEnv == nullptr) {
        HILOGE("aniEnv is null");
        return nullptr;
    }
    // 1.2->1.1
    ani_status ret = ANI_ERROR;
    ani_long session {};
    if ((ret = aniEnv->Object_GetPropertyByName_Long(input, "session", &session)) != ANI_OK) {
        HILOGE("get field session failed, ret:%{public}d", ret);
        return nullptr;
    }
    ani_long callbacks {};
    if ((ret = aniEnv->Object_GetPropertyByName_Long(input, "callbacks", &callbacks)) != ANI_OK) {
        HILOGE("get field callbacks failed, ret:%{public}d", ret);
        return nullptr;
    }
    IncrBackupEntity* entity = new IncrBackupEntity();
    std::unique_ptr<BIncrementalBackupSession> sessionPtr(reinterpret_cast<BIncrementalBackupSession*>(session));
    entity->session = move(sessionPtr);
    std::shared_ptr<GeneralCallbacks> callbackPtr(reinterpret_cast<GeneralCallbacks*>(callbacks));
    entity->callbacks = callbackPtr;
    napi_env jsEnv;
    if (!arkts_napi_scope_open(aniEnv, &jsEnv)) {
        HILOGE("Failed to arkts_napi_scope_open");
        delete entity;
        return nullptr;
    }
    napi_value napiEntity = SessionIncrementalBackupNExporter::CreateByEntity(jsEnv, entity);
    if (napiEntity == nullptr) {
        HILOGD("Failed to create napi obj");
        delete entity;
        return nullptr;
    }
    ani_ref outObj;
    if (!arkts_napi_scope_close_n(jsEnv, 1, &napiEntity, &outObj)) {
        HILOGE("Failed to arkts_napi_scope_close_n");
        delete entity;
        return nullptr;
    }
    HILOGD("TransferDynamicSession end");
    delete entity;
    return outObj;
}
}
