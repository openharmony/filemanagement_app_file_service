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

#include "restore_session_transfer.h"

#include <array>
#include <bit>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/arkts_esvalue.h"
#include "filemgmt_libhilog.h"
#include "ani_utils.h"
#include "restore_session.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "session_restore_n_exporter.h"

namespace OHOS::FileManagement::Backup {
namespace {
const char *RESTORE_SESSION_TRANSFER_CLASS_NAME = "@ohos.backup.transfer.backup.RestoreSessionTransfer";
const char *RESTORE_SESSION_CLASS_NAME = "@ohos.backup.transfer.backup.RestoreSession";
}

void RestoreSessionTransfer::Init(ani_env *aniEnv)
{
    HILOGD("Init transfer native method begin");
    if (aniEnv == nullptr) {
        HILOGE("aniEnv is null");
        return;
    }
    ani_class cls = AniUtils::GetAniClsByName(aniEnv, RESTORE_SESSION_TRANSFER_CLASS_NAME);
    if (cls == nullptr) {
        return;
    }
    std::array nativeFuncs = {
        ani_native_function { "transferStaticSession", nullptr,
            reinterpret_cast<void*>(RestoreSessionTransfer::TransferStaticSession)},
        ani_native_function { "transferDynamicSession", nullptr,
            reinterpret_cast<void*>(RestoreSessionTransfer::TransferDynamicSession)},
    };
    int32_t status = aniEnv->Class_BindStaticNativeMethods(cls, nativeFuncs.data(), nativeFuncs.size());
    if (status != ANI_OK) {
        HILOGE("Class_BindNativeMethods failed status: %{public}d", status);
        return;
    }
    HILOGD("Init transfer native method end");
}

ani_object RestoreSessionTransfer::TransferStaticSession(ani_env *aniEnv, ani_class aniCls, ani_object input)
{
    HILOGD("Transfer RestoreSession Static begin");
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
    RestoreSession* entity = reinterpret_cast<RestoreSession*>(unwrapResult);
    ani_class cls = AniUtils::GetAniClsByName(aniEnv, RESTORE_SESSION_CLASS_NAME);
    if (cls == nullptr) {
        HILOGE("Call FindClass failed");
        return nullptr;
    }
    ani_method constructFunc;
    ani_status ret = ANI_ERROR;
    if ((ret = aniEnv->Class_FindMethod(cls, "<ctor>", nullptr, &constructFunc)) != ANI_OK) {
        HILOGE("Call Class_FindMethod failed, ret = %{public}d", ret);
        return nullptr;
    }
    ani_object outObj;
    ret = aniEnv->Object_New(cls, constructFunc, &outObj, entity->session.release(),
        entity->incrSession.release(), entity->callbacks.get());
    if (ret != ANI_OK) {
        HILOGE("Call Object_New failed, ret = %{public}d", ret);
        return nullptr;
    }
    HILOGD("Transfer RestoreSession Static end");
    return outObj;
}

ani_ref RestoreSessionTransfer::TransferDynamicSession(ani_env *aniEnv, ani_class aniCls, ani_object input)
{
    HILOGD("TransferDynamicSession start");
    if (aniEnv == nullptr) {
        HILOGE("aniEnv is null");
        return nullptr;
    }
    ani_status ret = ANI_ERROR;
    ani_long session {};
    if ((ret = aniEnv->Object_GetPropertyByName_Long(input, "session", &session)) != ANI_OK) {
        HILOGE("get field session failed, ret:%{public}d", ret);
        return nullptr;
    }
    ani_long incrSession {};
    if ((ret = aniEnv->Object_GetPropertyByName_Long(input, "incrSession", &incrSession)) != ANI_OK) {
        HILOGE("get field incrSession failed, ret:%{public}d", ret);
        return nullptr;
    }
    ani_long callbacks {};
    if ((ret = aniEnv->Object_GetPropertyByName_Long(input, "callbacks", &callbacks)) != ANI_OK) {
        HILOGE("get field callbacks failed, ret:%{public}d", ret);
        return nullptr;
    }
    std::unique_ptr<RestoreEntity> entity = std::make_unique<RestoreEntity>();
    std::unique_ptr<BSessionRestore> sessionPtr(reinterpret_cast<BSessionRestore*>(session));
    entity->sessionWhole = move(sessionPtr);
    std::unique_ptr<BIncrementalRestoreSession>
        incrSessionPtr(reinterpret_cast<BIncrementalRestoreSession*>(incrSession));
    entity->sessionSheet = move(incrSessionPtr);
    std::shared_ptr<GeneralCallbacks> callbackPtr(reinterpret_cast<GeneralCallbacks*>(callbacks));
    entity->callbacks = callbackPtr;
    if ((entity->sessionWhole == nullptr && entity->sessionSheet == nullptr) || callbackPtr == nullptr) {
        HILOGE("session or callbacks is invalid ptr");
        return nullptr;
    }
    napi_env jsEnv;
    if (!arkts_napi_scope_open(aniEnv, &jsEnv)) {
        HILOGE("Failed to arkts_napi_scope_open");
        return nullptr;
    }
    napi_value napiEntity = SessionRestoreNExporter::CreateByEntity(jsEnv, move(entity));
    if (napiEntity == nullptr) {
        HILOGE("Failed to create napi obj");
        return nullptr;
    }
    ani_ref outObj;
    if (!arkts_napi_scope_close_n(jsEnv, 1, &napiEntity, &outObj)) {
        HILOGE("Failed to arkts_napi_scope_close_n");
        return nullptr;
    }
    HILOGD("TransferDynamicSession end");
    return outObj;
}
}