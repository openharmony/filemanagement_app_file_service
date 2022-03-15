/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "securec.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#define HMDFS_IOC 0xf2
#define HMDFS_IOC_SET_SHARE_PATH    _IOW(HMDFS_IOC, 1, \
                        struct hmdfs_share_control)

namespace OHOS {
namespace RemoteFileShare {
const int HMDFS_CID_SIZE = 64;
const int DEFAULT_PROMISE_ARGC = 2;
const int DEFAULT_ASYNC_ARGC = 3;
const int DEFAULT_AYSNC_CALLBACK_ARGC = 2;
const int ERR_BUF_SIZE = 256;

enum NUM {
    ZERO = 0,
    ONE = 1,
    TWO = 2,
};

struct hmdfs_share_control {
    int32_t src_fd;
    char cid[HMDFS_CID_SIZE];
};

struct AddonData {
    napi_async_work work;
    napi_deferred deferred;
    napi_ref callbackRef;
    int err;
    int status;
    int32_t fd;
    char *cid;
};

void ExecuteWork(napi_env env, void *data)
{
    struct AddonData *addonData = (struct AddonData *)data;
    struct hmdfs_share_control sc;
    int32_t err = 0;
    int32_t dirFd;
    std::string sharePath = "/data/storage/el2/distributedfiles/.share";

    if (access(sharePath.c_str(), F_OK) != 0) {
        err = mkdir(sharePath.c_str(), S_IRWXU | S_IRWXG | S_IXOTH);
        if (err < 0) {
            addonData->status = 0;
            addonData->err = errno;
            return;
        }
    }

    dirFd = open(sharePath.c_str(), O_RDONLY);
    if (dirFd < 0) {
        addonData->status = 0;
        addonData->err = errno;
        return;
    }

    sc.src_fd = addonData->fd;
    memcpy_s(sc.cid, HMDFS_CID_SIZE, addonData->cid, HMDFS_CID_SIZE);

    err = ioctl(dirFd, HMDFS_IOC_SET_SHARE_PATH, &sc);
    if (err < 0) {
        addonData->status = 0;
        addonData->err = errno;
    }

    close(dirFd);
}

void WorkComplete(napi_env env, napi_status status, void *data)
{
    struct AddonData *addonData = (struct AddonData *)data;
    napi_value path, callback, global;
    napi_value argv[DEFAULT_AYSNC_CALLBACK_ARGC], result[DEFAULT_AYSNC_CALLBACK_ARGC];
    char errBuf[ERR_BUF_SIZE] = {0};

    if (status != napi_ok) {
        return;
    }

    if (addonData->callbackRef != NULL) {
        napi_get_reference_value(env, addonData->callbackRef, &callback);
        napi_get_global(env, &global);
        if (addonData->status) {
            napi_get_null(env, &argv[NUM::ZERO]);
            napi_create_string_utf8(env, "/data/storage/el2/distributedfiles/.share",
                NAPI_AUTO_LENGTH, &argv[NUM::ONE]);
            napi_call_function(env, global, callback, NUM::TWO, argv, result);
        } else {
            strerror_r(addonData->err, errBuf, ERR_BUF_SIZE);
            napi_create_string_utf8(env, errBuf, NAPI_AUTO_LENGTH, &argv[NUM::ZERO]);
            napi_get_null(env, &argv[NUM::ONE]);
            napi_call_function(env, global, callback, NUM::TWO, argv, result);
        }
        napi_delete_reference(env, addonData->callbackRef);
    } else {
        if (addonData->status) {
            napi_create_string_utf8(env, "/data/storage/el2/distributedfiles/.share",
                            NAPI_AUTO_LENGTH, &path);
            napi_resolve_deferred(env, addonData->deferred, path);
        } else {
            strerror_r(addonData->err, errBuf, ERR_BUF_SIZE);
            napi_create_string_utf8(env, errBuf, NAPI_AUTO_LENGTH, &path);
            napi_reject_deferred(env, addonData->deferred, path);
        }
    }

    napi_delete_async_work(env, addonData->work);
    free(addonData->cid);
    delete addonData;
}

static napi_value CreateSharePath(napi_env env, napi_callback_info info)
{
    napi_value result, workName;
    struct AddonData *addonData = new AddonData();
    size_t argc = DEFAULT_ASYNC_ARGC;
    napi_value args[DEFAULT_ASYNC_ARGC];
    size_t copysize;
    napi_valuetype type = napi_undefined;

    addonData->status = 1;
    addonData->err = 0;
    addonData->callbackRef = NULL;
    napi_get_cb_info(env, info, &argc, args, NULL, NULL);

    if (argc != DEFAULT_ASYNC_ARGC && argc != DEFAULT_PROMISE_ARGC) {
        napi_throw_error(env, NULL, "number of arguments mismatch");
        delete addonData;
        return NULL;
    }

    napi_typeof(env, args[NUM::ZERO], &type);
    if (type != napi_number) {
        addonData->err = EINVAL;
        addonData->status = 0;
    }
    napi_get_value_int32(env, args[NUM::ZERO], &(addonData->fd));

    napi_typeof(env, args[NUM::ONE], &type);
    if (type != napi_string) {
        addonData->err = EINVAL;
        addonData->status = 0;
    }
    napi_get_value_string_utf8(env, args[NUM::ONE], NULL, 0, &copysize);
    if (copysize != HMDFS_CID_SIZE) {
        addonData->err = EINVAL;
        addonData->status = 0;
    }
    addonData->cid = (char *)malloc(sizeof(char) * (copysize + 1));
    if (addonData->cid == nullptr) {
        napi_throw_error(env, NULL, "malloc failed");
        delete addonData;
        return NULL;
    }
    addonData->cid[copysize] = '\0';
    napi_get_value_string_utf8(env, args[NUM::ONE], addonData->cid, copysize + 1, &copysize);

    if (argc == DEFAULT_ASYNC_ARGC) {
        napi_typeof(env, args[NUM::TWO], &type);
        if (type != napi_function) {
            addonData->err = EINVAL;
            addonData->status = 0;
        } else {
            napi_create_reference(env, args[NUM::TWO], NUM::ONE, &addonData->callbackRef);
        }
    }

    if (addonData->callbackRef == NULL) {
        napi_create_promise(env, &addonData->deferred, &result);
    } else {
        napi_get_undefined(env, &result);
    }

    napi_create_string_utf8(env, "Async Work", NAPI_AUTO_LENGTH, &workName);
    napi_create_async_work(env, NULL, workName, ExecuteWork, WorkComplete, addonData, &(addonData->work));
    napi_queue_async_work(env, addonData->work);

    return result;
}

#define DECLARE_NAPI_METHOD(name, func)         \
    {                                           \
        name, 0, func, 0, 0, 0, napi_default, 0 \
    }

static napi_value Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_property_descriptor desc = DECLARE_NAPI_METHOD("createSharePath", CreateSharePath);
    status = napi_define_properties(env, exports, 1, &desc);
    return exports;
}
} // namespace RemoteFileShare
} // namespace OHOS

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = OHOS::RemoteFileShare::Init,
    .nm_modname = "remotefileshare",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&demoModule);
}