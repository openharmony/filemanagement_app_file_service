/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "module_ipc/service.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/enhance_service_manager.h"

#include <string>

namespace OHOS::FileManagement::Backup {
using namespace std;

ErrCode Service::ExecuteEnhanceServiceOperationWithAuth(
    std::function<ErrCode(IEnhanceService *, const std::string &)> func)
{
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName, false);
        if (ret != ERR_OK) {
            HILOGE("get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGE("enhance service is unavailable");
            return BError(BError::Codes::OK);
        }
        return func(enhanceService, callerName);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("unexpected exception");
        return EINVAL;
    }
}

ErrCode Service::CreateAncoBackupTask(const sptr<IAncoBackupCallback> &callback)
{
    auto func = [&](IEnhanceService *enhanceService, const std::string &callerName) {
        return enhanceService->CreateAncoBackupTask(callerName, callback);
    };
    auto ret = ExecuteEnhanceServiceOperationWithAuth(func);
    if (ret != ERR_OK) {
        HILOGE("CreateAncoBackupTask error, ret:%{public}d", ret);
    }
    return ret;
}

ErrCode Service::DestroyAncoBackupTask()
{
    auto func = [](IEnhanceService *enhanceService, const std::string &callerName) {
        return enhanceService->DestroyAncoBackupTask(callerName);
    };
    auto ret = ExecuteEnhanceServiceOperationWithAuth(func);
    if (ret != ERR_OK) {
        HILOGE("DestroyAncoBackupTask error, ret:%{public}d", ret);
    }
    return ret;
}

ErrCode Service::FilterAndSaveBackupPaths(
    std::set<std::string> &includes, std::set<std::string> &compatIncludes, const std::vector<std::string> &excludes)
{
    auto func = [&](IEnhanceService *enhanceService, const std::string &callerName) {
        return enhanceService->FilterAndSaveBackupPaths(callerName, includes, compatIncludes, excludes);
    };
    auto ret = ExecuteEnhanceServiceOperationWithAuth(func);
    if (ret != ERR_OK) {
        HILOGE("FilterAndSaveBackupPaths error, ret:%{public}d", ret);
    }
    return ret;
}

ErrCode Service::StartAncoScanAllDirs(AncoScanResult &scanResult)
{
    auto func = [&](IEnhanceService *enhanceService, const std::string &callerName) {
        return enhanceService->StartAncoScanAllDirs(callerName, scanResult);
    };
    auto ret = ExecuteEnhanceServiceOperationWithAuth(func);
    if (ret != ERR_OK) {
        HILOGE("StartAncoScanAllDirs error, ret:%{public}d", ret);
    }
    return ret;
}

ErrCode Service::StartAncoPacket(uint64_t &smallFileCount)
{
    auto func = [&](IEnhanceService *enhanceService, const std::string &callerName) {
        return enhanceService->StartAncoPacket(callerName, smallFileCount);
    };
    auto ret = ExecuteEnhanceServiceOperationWithAuth(func);
    if (ret != ERR_OK) {
        HILOGE("StartAncoPacket error, ret:%{public}d", ret);
    }
    return ret;
}

ErrCode Service::CreateAncoRestoreTask(const sptr<IAncoRestoreCallback> &callback)
{
    auto func = [&](IEnhanceService *enhanceService, const std::string &callerName) {
        return enhanceService->CreateAncoRestoreTask(callerName, callback);
    };
    auto ret = ExecuteEnhanceServiceOperationWithAuth(func);
    if (ret != ERR_OK) {
        HILOGE("CreateAncoRestoreTask error, ret:%{public}d", ret);
    }
    return ret;
}

ErrCode Service::DestroyAncoRestoreTask()
{
    auto func = [&](IEnhanceService *enhanceService, const std::string &callerName) {
        return enhanceService->DestroyAncoRestoreTask(callerName);
    };
    auto ret = ExecuteEnhanceServiceOperationWithAuth(func);
    if (ret != ERR_OK) {
        HILOGE("DestroyAncoRestoreTask error, ret:%{public}d", ret);
    }
    return ret;
}

ErrCode Service::AddAncoTars(const std::vector<string> &tarFiles, const std::vector<int64_t> &tarFileSizes,
    const std::vector<string> &tarFileNames)
{
    auto func = [&](IEnhanceService *enhanceService, const std::string &callerName) {
        return enhanceService->AddAncoTars(callerName, tarFiles, tarFileSizes, tarFileNames);
    };
    auto ret = ExecuteEnhanceServiceOperationWithAuth(func);
    if (ret != ERR_OK) {
        HILOGE("AddAncoTars error, ret:%{public}d", ret);
    }
    return ret;
}

ErrCode Service::StartAncoUnPacket(const std::string &rootPath)
{
    auto func = [&](IEnhanceService *enhanceService, const std::string &callerName) {
        return enhanceService->StartAncoUnPacket(callerName, rootPath);
    };
    auto ret = ExecuteEnhanceServiceOperationWithAuth(func);
    if (ret != ERR_OK) {
        HILOGE("StartAncoUnPacket error, ret:%{public}d", ret);
    }
    return ret;
}

ErrCode Service::AddAncoMovePaths(const std::vector<std::string> &ancoSourcePath,
    const std::vector<std::string> &ancoTargetPath, const std::vector<StatInfo> &ancoStats)
{
    auto func = [&](IEnhanceService *enhanceService, const std::string &callerName) {
        return enhanceService->AddAncoMovePaths(callerName, ancoSourcePath, ancoTargetPath, ancoStats);
    };
    auto ret = ExecuteEnhanceServiceOperationWithAuth(func);
    if (ret != ERR_OK) {
        HILOGE("AddAncoMovePaths error, ret:%{public}d", ret);
    }
    return ret;
}

ErrCode Service::StartAncoMove(int &fd, AncoRestoreResult &ancoRestoreRes)
{
    auto func = [&](IEnhanceService *enhanceService, const std::string &callerName) {
        return enhanceService->StartAncoMove(callerName, fd, ancoRestoreRes);
    };
    auto ret = ExecuteEnhanceServiceOperationWithAuth(func);
    if (ret != ERR_OK) {
        HILOGE("StartAncoMove error, ret:%{public}d", ret);
    }
    return ret;
}
}  // namespace OHOS::FileManagement::Backup
