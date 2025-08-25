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

#include "incremental_backup_session.h"
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
const char *BACKUP_SESSION_CLASS_NAME = "L@ohos/backup/transfer/backup/IncrementalBackupSession;";
const char *B_INCREMENTAL_DATA_CLASS_NAME = "L@ohos/backup/transfer/backup/BIncrementalData;";
const char *INCR_BACKUP_SESSION_CLEANER_CLASS_NAME = "L@ohos/backup/transfer/backup/IncrBackupSessionCleaner;";
constexpr int32_t E_OK = 0;
}

void IncrementalBackupSession::Init(ani_env *aniEnv)
{
    HILOGD("Init IncrementalBackupSession begin");
    if (aniEnv == nullptr) {
        HILOGE("aniEnv is null");
        return;
    }

    ani_class clsCleaner = AniUtils::GetAniClsByName(aniEnv, INCR_BACKUP_SESSION_CLEANER_CLASS_NAME);
    if (clsCleaner == nullptr) {
        return;
    }
    std::array cleanNativeFuncs = {
        ani_native_function {"clean", ":V", reinterpret_cast<void*>(IncrBackupSessionCleaner::Clean)},
    };
    auto status = aniEnv->Class_BindNativeMethods(clsCleaner, cleanNativeFuncs.data(), cleanNativeFuncs.size());
    if (status != ANI_OK) {
        HILOGE("Class_BindNativeMethods failed status: %{public}d", status);
        return;
    }

    ani_class cls = AniUtils::GetAniClsByName(aniEnv, BACKUP_SESSION_CLASS_NAME);
    if (cls == nullptr) {
        return;
    }

    std::array nativeFuncs = {
        ani_native_function {"getLocalCapabilities", ":i",
            reinterpret_cast<void*>(IncrementalBackupSession::GetLocalCapabilities)},
        ani_native_function {"getBackupDataSize", "zC{escompat.Array}:i",
            reinterpret_cast<void*>(IncrementalBackupSession::GetBackupDataSize)},
        ani_native_function {"appendBundles", "C{escompat.Array}:i",
            reinterpret_cast<void*>(IncrementalBackupSession::AppendBundles)},
        ani_native_function {"release", ":i",
            reinterpret_cast<void*>(IncrementalBackupSession::Release)},
        ani_native_function {"cancel", "C{std.core.String}:i",
            reinterpret_cast<void*>(IncrementalBackupSession::Cancel)},
        ani_native_function {"cleanBundleTempDir", "C{std.core.String}:i",
            reinterpret_cast<void*>(IncrementalBackupSession::CleanBundleTempDir)},
    };
    status = aniEnv->Class_BindNativeMethods(cls, nativeFuncs.data(), nativeFuncs.size());
    if (status != ANI_OK) {
        HILOGE("Class_BindNativeMethods failed status: %{public}d", status);
        return;
    }
    HILOGD("Init IncrementalBackupSession end");
}

bool IncrementalBackupSession::ParseBIncrementalData(ani_env* env, ani_object dataObj, BIncrementalData& data)
{
    if (env == nullptr) {
        HILOGE("aniEnv is null");
        return false;
    }
    ani_ref aniBundleName {};
    int32_t status = 0;
    status = env->Object_GetFieldByName_Ref(dataObj, "bundleName", &aniBundleName);
    if (status != ANI_OK) {
        HILOGE("get field bundleName fail, ret=%{public}d", status);
        return false;
    }
    if (!AniUtils::AniObjectToStdString(env, static_cast<ani_object>(aniBundleName), data.bundleName)) {
        HILOGE("parse bundleName fail");
        return false;
    }
    ani_long lastIncrementalTime = 0;
    status = env->Object_GetFieldByName_Long(dataObj, "lastIncrementalTime", &lastIncrementalTime);
    if (status != ANI_OK) {
        HILOGE("get field lastIncrementalTime fail, ret=%{public}d", status);
        return false;
    }
    ani_int manifestFd = 0;
    status = env->Object_GetFieldByName_Int(dataObj, "manifestFd", &manifestFd);
    if (status != ANI_OK) {
        HILOGE("get field manifestFd fail, ret=%{public}d", status);
        return false;
    }
    ani_ref aniBackupParameters {};
    status = env->Object_GetFieldByName_Ref(dataObj, "backupParameters", &aniBackupParameters);
    if (status != ANI_OK) {
        HILOGE("get field backupParameters fail, ret=%{public}d", status);
        return false;
    }
    if (!AniUtils::AniObjectToStdString(env, static_cast<ani_object>(aniBackupParameters), data.backupParameters)) {
        HILOGE("parse backupParameters fail");
        return false;
    }
    ani_int backupPriority = 0;
    status = env->Object_GetFieldByName_Int(dataObj, "backupPriority", &backupPriority);
    if (status != ANI_OK) {
        HILOGE("get field backupPriority fail, ret=%{public}d", status);
        return false;
    }
    data.lastIncrementalTime = lastIncrementalTime;
    data.manifestFd = manifestFd;
    data.backupPriority = backupPriority;
    return true;
}

bool IncrementalBackupSession::ParseIncrDataFromAniArray(ani_env *aniEnv, ani_array bundles,
    std::vector<BIncrementalData>& result)
{
    if (aniEnv == nullptr) {
        HILOGE("aniEnv is null");
        return false;
    }
    ani_size arrSize = 0;
    if (ANI_OK != aniEnv->Array_GetLength(bundles, &arrSize)) {
        HILOGE("Array_GetLength fail");
        return false;
    }
    if (arrSize == 0) {
        return false;
    }
    for (ani_size idx = 0; idx < arrSize; idx++) {
        ani_ref item;
        if (ANI_OK != aniEnv->Array_Get(bundles, idx, &item)) {
            HILOGE("Array_Get fail, idx=%{public}zu", idx);
            return false;
        }
        ani_class incrementalDataCls = AniUtils::GetAniClsByName(aniEnv, B_INCREMENTAL_DATA_CLASS_NAME);
        if (incrementalDataCls == nullptr) {
            return false;
        }
        ani_boolean isIncrementalData = ANI_FALSE;
        ani_object dataObj = static_cast<ani_object>(item);
        aniEnv->Object_InstanceOf(dataObj, incrementalDataCls, &isIncrementalData);
        if (!isIncrementalData) {
            HILOGE("arr element is not BIncrementalData");
            return false;
        }
        BIncrementalData incrData;
        if (!ParseBIncrementalData(aniEnv, dataObj, incrData)) {
            HILOGE("BIncrementalData fail");
            return false;
        }
        result.push_back(incrData);
    }
    return true;
}

int32_t IncrementalBackupSession::checkPermission(ani_env *aniEnv)
{
    if (aniEnv == nullptr) {
        HILOGE("aniEnv is null");
        return -E_PARAMS;
    }
    if (!SAUtils::CheckBackupPermission()) {
        HILOGE("Has not permission!");
        return -E_PERMISSION;
    }
    if (!SAUtils::IsSystemApp()) {
        HILOGE("System App check fail!");
        return -E_PERMISSION_SYS;
    }
    return E_OK;
}

ani_int IncrementalBackupSession::GetLocalCapabilities(ani_env *aniEnv, ani_object object)
{
    int32_t checkRet;
    if ((checkRet = checkPermission(aniEnv)) != E_OK) {
        return checkRet;
    }
    IncrementalBackupSession* entity = reinterpret_cast<IncrementalBackupSession*>(object);
    if (entity == nullptr || entity->session == nullptr) {
        HILOGE("object is invalid");
        return -E_PARAMS;
    }
    UniqueFd fd = entity->session->GetLocalCapabilities();
    return fd.Release();
}

ani_int IncrementalBackupSession::GetBackupDataSize(ani_env *aniEnv, ani_object object, ani_boolean isPreciseScan,
    ani_array bundles)
{
    int32_t checkRet;
    if ((checkRet = checkPermission(aniEnv)) != E_OK) {
        return checkRet;
    }
    std::vector<BIncrementalData> bundleInfos;
    bool parseRet = ParseIncrDataFromAniArray(aniEnv, bundles, bundleInfos);
    if (!parseRet || bundleInfos.size() == 0) {
        HILOGE("parse fail or bundles is empty");
        return -E_PARAMS;
    }
    IncrementalBackupSession* entity = reinterpret_cast<IncrementalBackupSession*>(object);
    if (entity == nullptr || entity->session == nullptr) {
        HILOGE("object is invalid");
        return -E_PARAMS;
    }
    return entity->session->GetBackupDataSize(isPreciseScan, bundleInfos);
}

ani_int IncrementalBackupSession::AppendBundles(ani_env *aniEnv, ani_object object, ani_array bundles)
{
    int32_t checkRet;
    if ((checkRet = checkPermission(aniEnv)) != E_OK) {
        return checkRet;
    }
    IncrementalBackupSession* entity = reinterpret_cast<IncrementalBackupSession*>(object);
    if (entity == nullptr || entity->session == nullptr) {
        HILOGE("object is invalid");
        return -E_PARAMS;
    }
    std::vector<BIncrementalData> bundleInfos;
    bool parseResult = ParseIncrDataFromAniArray(aniEnv, bundles, bundleInfos);
    if (!parseResult || bundleInfos.size() == 0) {
        HILOGE("parse fail or bundles is empty");
        return -E_PARAMS;
    }
    std::vector<std::string> bundleNames;
    for (const BIncrementalData& item: bundleInfos) {
        bundleNames.push_back(item.bundleName);
    }
    return entity->session->AppendBundles(bundleInfos, bundleNames);
}

ani_int IncrementalBackupSession::Release(ani_env *aniEnv, ani_object object)
{
    int32_t checkRet;
    if ((checkRet = checkPermission(aniEnv)) != E_OK) {
        return checkRet;
    }
    IncrementalBackupSession* entity = reinterpret_cast<IncrementalBackupSession*>(object);
    if (entity == nullptr || entity->session == nullptr) {
        HILOGE("object is invalid");
        return -E_PARAMS;
    }
    return entity->session->Release();
}

ani_int IncrementalBackupSession::Cancel(ani_env *aniEnv, ani_object object, ani_string bundleName)
{
    int32_t checkRet;
    if ((checkRet = checkPermission(aniEnv)) != E_OK) {
        return checkRet;
    }
    IncrementalBackupSession* entity = reinterpret_cast<IncrementalBackupSession*>(object);
    if (entity == nullptr || entity->session == nullptr) {
        HILOGE("object is invalid");
        return -E_PARAMS;
    }
    std::string bundle = AniUtils::AniStringToStdString(aniEnv, bundleName);
    return entity->session->Cancel(bundle);
}

ani_int IncrementalBackupSession::CleanBundleTempDir(ani_env *aniEnv, ani_object object, ani_string bundleName)
{
    int32_t checkRet;
    if ((checkRet = checkPermission(aniEnv)) != E_OK) {
        return checkRet;
    }
    IncrementalBackupSession* entity = reinterpret_cast<IncrementalBackupSession*>(object);
    if (entity == nullptr || entity->session == nullptr) {
        HILOGE("object is invalid");
        return -E_PARAMS;
    }
    std::string bundle = AniUtils::AniStringToStdString(aniEnv, bundleName);
    return entity->session->CleanBundleTempDir(bundle);
}

void IncrBackupSessionCleaner::Clean(ani_env *aniEnv, ani_object object)
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
        BIncrementalBackupSession* sessionPtr = reinterpret_cast<BIncrementalBackupSession*>(session);
        if (sessionPtr != nullptr) {
            delete sessionPtr;
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
