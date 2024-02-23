/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "remotefileshare_n_exporter.h"

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>

#include "securec.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleRemoteFileShare {
using namespace FileManagement::LibN;
using namespace std;
namespace {
    constexpr int HMDFS_CID_SIZE = 64;
    constexpr unsigned HMDFS_IOC = 0xf2;
    const std::string g_sharePath = "/data/storage/el2/distributedfiles/.share";
}

#define HMDFS_IOC_SET_SHARE_PATH    _IOW(HMDFS_IOC, 1, struct HmdfsShareControl)

struct HmdfsShareControl {
    int srcFd;
    char cid[HMDFS_CID_SIZE];
};

static NError CreateSharePath(const int srcFd, const std::string &cid)
{
    struct HmdfsShareControl sc;
    int32_t ret = 0;
    int32_t dirFd;

    if (access(g_sharePath.c_str(), F_OK) != 0) {
        ret = mkdir(g_sharePath.c_str(), S_IRWXU | S_IRWXG | S_IXOTH);
        if (ret < 0) {
            return NError(errno);
        }
    }

    char realPath[PATH_MAX] = {0};
    if (!realpath(g_sharePath.c_str(), realPath)) {
        return NError(errno);
    }
    dirFd = open(realPath, O_RDONLY);
    if (dirFd < 0) {
        return NError(errno);
    }

    sc.srcFd = srcFd;
    if (memcpy_s(sc.cid, HMDFS_CID_SIZE, cid.c_str(), cid.size()) != 0) {
        close(dirFd);
        return NError(ENOMEM);
    }

    ret = ioctl(dirFd, HMDFS_IOC_SET_SHARE_PATH, &sc);
    if (ret < 0) {
        close(dirFd);
        return NError(errno);
    }

    close(dirFd);
    return NError(ERRNO_NOERR);
}

napi_value CreateSharePath(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(static_cast<size_t>(NARG_CNT::TWO), static_cast<size_t>(NARG_CNT::THREE))) {
        NError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    bool succ = false;
    int srcFd;
    std::unique_ptr<char []> cid;
    size_t cidLen;
    tie(succ, srcFd) = NVal(env, funcArg[static_cast<size_t>(NARG_POS::FIRST)]).ToInt32();
    if (!succ) {
        NError(EINVAL).ThrowErr(env, "Invalid fd");
        return nullptr;
    }
    tie(succ, cid, cidLen) = NVal(env, funcArg[static_cast<size_t>(NARG_POS::SECOND)]).ToUTF8String();
    if (!succ || cidLen != HMDFS_CID_SIZE) {
        NError(EINVAL).ThrowErr(env, "Invalid cid");
        return nullptr;
    }

    std::string cidString(cid.get());
    auto cbExec = [srcFd, cidString]() -> NError {
        return CreateSharePath(srcFd, cidString);
    };
    auto cbComplete = [](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        } else {
            return NVal::CreateUTF8String(env, g_sharePath);
        }
    };
    std::string procedureName = "CreateSharePath";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == static_cast<size_t>(NARG_CNT::TWO)) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    } else {
        NVal cb(env, funcArg[static_cast<size_t>(NARG_POS::THIRD)]);
        if (cb.TypeIs(napi_function)) {
            return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
        } else {
            NError(EINVAL).ThrowErr(env, "Callback function error");
            return nullptr;
        }
    }
    return NVal::CreateUndefined(env).val_;
}
} // namespace ModuleRemoteFileShare
} // namespace AppFileService
} // namespace OHOS
