/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "fileshare_n_exporter.h"
#include "file_permission.h"
#include "grant_permissions.h"
#include "grant_uri_permission.h"
#include "log.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileShare {
using namespace FileManagement;
using namespace FileManagement::LibN;

/***********************************************
 * Module export and register
 ***********************************************/
napi_value FileShareExport(napi_env env, napi_value exports)
{
    InitOperationMode(env, exports);
    InitPolicyFlag(env, exports);
    InitPolicyInfo(env, exports);
    InitPolicyErrorCode(env, exports);
    InitPolicyErrorResult(env, exports);
    static napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("grantUriPermission", GrantUriPermission::Async),
        DECLARE_NAPI_FUNCTION("grantPermission", GrantPermission),
        DECLARE_NAPI_FUNCTION("persistPermission", PersistPermission),
        DECLARE_NAPI_FUNCTION("revokePermission", RevokePermission),
        DECLARE_NAPI_FUNCTION("activatePermission", ActivatePermission),
        DECLARE_NAPI_FUNCTION("deactivatePermission", DeactivatePermission),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}

static napi_value InitPolicyConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value args[1] = {0};
    napi_value res = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, args, &res, &data);
    if (status != napi_ok) {
        HILOGE("InitPolicyConstructor, status is not napi_ok");
        return nullptr;
    }
    return res;
}

void InitOperationMode(napi_env env, napi_value exports)
{
    char propertyName[] = "OperationMode";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("READ_MODE",
                                     NVal::CreateUint32(env, static_cast<uint32_t>(OperationMode::READ_MODE)).val_),
        DECLARE_NAPI_STATIC_PROPERTY("WRITE_MODE",
                                     NVal::CreateUint32(env, static_cast<uint32_t>(OperationMode::WRITE_MODE)).val_),
    };
    napi_value obj = nullptr;
    napi_status status = napi_create_object(env, &obj);
    if (status != napi_ok) {
        HILOGE("Failed to create object at initializing OperationMode");
        return;
    }
    status = napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    if (status != napi_ok) {
        HILOGE("Failed to set properties of character at initializing OperationMode");
        return;
    }
    status = napi_set_named_property(env, exports, propertyName, obj);
    if (status != napi_ok) {
        HILOGE("Failed to set direction property at initializing OperationMode");
        return;
    }
}

void InitPolicyFlag(napi_env env, napi_value exports)
{
    char propertyName[] = "PolicyFlag";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "ALLOW_PERSISTENCE", NVal::CreateUint32(env, static_cast<uint32_t>(PolicyFlag::ALLOW_PERSISTENCE)).val_),
        DECLARE_NAPI_STATIC_PROPERTY(
            "FORBID_PERSISTENCE", NVal::CreateUint32(env, static_cast<uint32_t>(PolicyFlag::FORBID_PERSISTENCE)).val_),
    };
    napi_value obj = nullptr;
    napi_status status = napi_create_object(env, &obj);
    if (status != napi_ok) {
        HILOGE("Failed to create object at initializing PolicyFlag");
        return;
    }
    status = napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    if (status != napi_ok) {
        HILOGE("Failed to set properties of character at initializing PolicyFlag");
        return;
    }
    status = napi_set_named_property(env, exports, propertyName, obj);
    if (status != napi_ok) {
        HILOGE("Failed to set direction property at initializing PolicyFlag");
        return;
    }
}

void InitPolicyErrorCode(napi_env env, napi_value exports)
{
    char propertyName[] = "PolicyErrorCode";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "PERSISTENCE_FORBIDDEN",
            NVal::CreateInt32(env, static_cast<int32_t>(PolicyErrorCode::PERSISTENCE_FORBIDDEN)).val_),
        DECLARE_NAPI_STATIC_PROPERTY("INVALID_MODE",
                                     NVal::CreateInt32(env, static_cast<int32_t>(PolicyErrorCode::INVALID_MODE)).val_),
        DECLARE_NAPI_STATIC_PROPERTY("INVALID_PATH",
                                     NVal::CreateInt32(env, static_cast<int32_t>(PolicyErrorCode::INVALID_PATH)).val_),
    };
    napi_value obj = nullptr;
    napi_status status = napi_create_object(env, &obj);
    if (status != napi_ok) {
        HILOGE("Failed to create object at initializing InitPolicyErrorCode");
        return;
    }
    status = napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    if (status != napi_ok) {
        HILOGE("Failed to set properties of character at initializing InitPolicyErrorCode");
        return;
    }
    status = napi_set_named_property(env, exports, propertyName, obj);
    if (status != napi_ok) {
        HILOGE("Failed to set direction property at initializing InitPolicyErrorCode");
        return;
    }
}

void InitPolicyErrorResult(napi_env env, napi_value exports)
{
    char className[] = "PolicyErrorResult";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("uri", NVal::CreateUTF8String(env, "uri").val_),
        DECLARE_NAPI_STATIC_PROPERTY(
            "code", NVal::CreateInt32(env, static_cast<int32_t>(PolicyErrorCode::PERSISTENCE_FORBIDDEN)).val_),
        DECLARE_NAPI_STATIC_PROPERTY("message", NVal::CreateUTF8String(env, "message").val_),
    };
    napi_value obj = nullptr;
    napi_status status = napi_define_class(env, className, NAPI_AUTO_LENGTH, InitPolicyConstructor, nullptr,
                                           sizeof(desc) / sizeof(desc[0]), desc, &obj);
    napi_create_object(env, &obj);
    if (status != napi_ok) {
        HILOGE("Failed to define class at initializing PolicyErrorResult");
        return;
    }
    status = napi_set_named_property(env, exports, className, obj);
    if (status != napi_ok) {
        HILOGE("Failed to set direction property at initializing PolicyErrorResult");
        return;
    }
}

void InitPolicyInfo(napi_env env, napi_value exports)
{
    char className[] = "PolicyInfo";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("uri", NVal::CreateUTF8String(env, "uri").val_),
        DECLARE_NAPI_STATIC_PROPERTY("operationMode",
                                     NVal::CreateUint32(env, static_cast<uint32_t>(OperationMode::READ_MODE)).val_),
    };
    napi_value obj = nullptr;
    napi_status status = napi_define_class(env, className, NAPI_AUTO_LENGTH, InitPolicyConstructor, nullptr,
                                           sizeof(desc) / sizeof(desc[0]), desc, &obj);
    napi_create_object(env, &obj);
    if (status != napi_ok) {
        HILOGE("Failed to define class at initializing PolicyFlag");
        return;
    }
    status = napi_set_named_property(env, exports, className, obj);
    if (status != napi_ok) {
        HILOGE("Failed to set direction property at initializing PolicyFlag");
        return;
    }
}

NAPI_MODULE(fileshare, FileShareExport)
} // namespace ModuleFileShare
} // namespace AppFileService
} // namespace OHOS
