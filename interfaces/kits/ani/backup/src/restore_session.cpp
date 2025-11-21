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

#include "restore_session.h"
#include <array>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>
#include "ani_utils.h"
#include "b_sa/b_sa_utils.h"
#include "filemgmt_libhilog.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"

namespace OHOS::FileManagement::Backup {
using namespace LibN;

namespace {
const char *RESTORE_SESSION_CLEANER_CLASS_NAME = "@ohos.backup.transfer.backup.RestoreSessionCleaner";
}

void RestoreSession::Init(ani_env *aniEnv)
{
    HILOGD("Init RestoreSession begin");
    if (aniEnv == nullptr) {
        HILOGE("aniEnv is null");
        return;
    }
    ani_class clsCleaner = AniUtils::GetAniClsByName(aniEnv, RESTORE_SESSION_CLEANER_CLASS_NAME);
    if (clsCleaner == nullptr) {
        return;
    }
    std::array cleanNativeFuncs = {
        ani_native_function {"clean", ":", reinterpret_cast<void*>(RestoreSessionCleaner::Clean)},
    };
    auto status = aniEnv->Class_BindNativeMethods(clsCleaner, cleanNativeFuncs.data(), cleanNativeFuncs.size());
    if (status != ANI_OK) {
        HILOGE("Class_BindNativeMethods failed status: %{public}d", status);
        return;
    }
    HILOGD("Init RestoreSession end");
}

void RestoreSessionCleaner::Clean(ani_env *aniEnv, ani_object object)
{
    if (aniEnv == nullptr) {
        HILOGE("aniEnv is null");
        return;
    }
    ani_status ret = ANI_ERROR;
    ani_long session {};
    if ((ret = aniEnv->Object_GetPropertyByName_Long(object, "session", &session)) != ANI_OK) {
        HILOGE("get field session failed, ret:%{public}d", ret);
    } else {
        BSessionRestore* sessionPtr = reinterpret_cast<BSessionRestore*>(session);
        if (sessionPtr != nullptr) {
            delete sessionPtr;
        }
    }
    ani_long incrSession {};
    if ((ret = aniEnv->Object_GetPropertyByName_Long(object, "incrSession", &incrSession)) != ANI_OK) {
        HILOGE("get field incrSession failed, ret:%{public}d", ret);
    } else {
        BIncrementalRestoreSession* incrSessionPtr = reinterpret_cast<BIncrementalRestoreSession*>(incrSession);
        if (incrSessionPtr != nullptr) {
            delete incrSessionPtr;
        }
    }
    ani_long callbacks {};
    if ((ret = aniEnv->Object_GetPropertyByName_Long(object, "callbacks", &callbacks)) != ANI_OK) {
        HILOGE("get field callbacks failed, ret:%{public}d", ret);
    } else {
        GeneralCallbacks* callbackPtr = reinterpret_cast<GeneralCallbacks*>(callbacks);
        if (callbackPtr != nullptr) {
            delete callbackPtr;
        }
    }
}
}