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

ErrCode Service::CreateAncoBackupTask(const sptr<IAncoBackupCallback> &callback)
{
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName, false);
        if (ret != ERR_OK) {
            HILOGE("CreateAncoBackupTask error, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("CreateAncoBackupTask, enhance service is not loaded");
            return BError(BError::Codes::OK);
        }
        return enhanceService->CreateAncoBackupTask(callerName, callback);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("CreateAncoBackupTask, Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::DestroyAncoBackupTask()
{
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName, false);
        if (ret != ERR_OK) {
            HILOGE("DestroyAncoBackupTask error, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("DestroyAncoBackupTask, enhance service is not loaded");
            return BError(BError::Codes::OK);
        }
        return enhanceService->DestroyAncoBackupTask(callerName);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("DestroyAncoBackupTask, Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::FilterAndSaveBackupPaths(
    std::set<std::string> &includes, std::set<std::string> &compatIncludes, const std::vector<std::string> &excludes)
{
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName, false);
        if (ret != ERR_OK) {
            HILOGE("FilterAndSaveBackupPaths error, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("FilterAndSaveBackupPaths, enhance service is not loaded");
            return BError(BError::Codes::OK);
        }
        return enhanceService->FilterAndSaveBackupPaths(callerName, includes, compatIncludes, excludes);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("FilterAndSaveBackupPaths, Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::StartAncoScanAllDirs(AncoScanResult &scanResult)
{
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName, false);
        if (ret != ERR_OK) {
            HILOGE("StartAncoScanAllDirs error, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("StartAncoScanAllDirs, enhance service is not loaded");
            return BError(BError::Codes::OK);
        }
        return enhanceService->StartAncoScanAllDirs(callerName, scanResult);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("StartAncoScanAllDirs, Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::StartAncoPacket(uint64_t &smallFileCount)
{
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName, false);
        if (ret != ERR_OK) {
            HILOGE("StartAncoPacket error, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("StartAncoPacket, enhance service is not loaded");
            return BError(BError::Codes::OK);
        }
        return enhanceService->StartAncoPacket(callerName, smallFileCount);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("StartAncoPacket, Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::CreateAncoRestoreTask()
{
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName, false);
        if (ret != ERR_OK) {
            HILOGE("CreateAncoRestoreTask error, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("CreateAncoRestoreTask, enhance service is not loaded");
            return BError(BError::Codes::OK);
        }
        return enhanceService->CreateAncoRestoreTask(callerName);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("CreateAncoRestoreTask, Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::DestroyAncoRestoreTask()
{
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName, false);
        if (ret != ERR_OK) {
            HILOGE("DestroyAncoRestoreTask error, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("DestroyAncoRestoreTask, enhance service is not loaded");
            return BError(BError::Codes::OK);
        }
        return enhanceService->DestroyAncoRestoreTask(callerName);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("DestroyAncoRestoreTask, Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::StartAncoUnPacket(const std::vector<string> &tarFiles, const std::vector<int64_t> &tarFileSizes,
    const std::vector<string> &tarFileNames, const std::string &rootPath)
{
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName, false);
        if (ret != ERR_OK) {
            HILOGE("StartAncoUnPacket error, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("StartAncoUnPacket, enhance service is not loaded");
            return BError(BError::Codes::OK);
        }
        return enhanceService->StartAncoUnPacket(callerName, tarFiles, tarFileSizes, tarFileNames, rootPath);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("StartAncoUnPacket, Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::AddAncoMovePaths(const std::vector<std::string> &ancoSourcePath,
    const std::vector<std::string> &ancoTargetPath, const std::vector<StatInfo> &ancoStats)
{
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName, false);
        if (ret != ERR_OK) {
            HILOGE("AddAncoMovePaths error, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("AddAncoMovePaths, enhance service is not loaded");
            return BError(BError::Codes::OK);
        }
        return enhanceService->AddAncoMovePaths(callerName, ancoSourcePath, ancoTargetPath, ancoStats);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("AddAncoMovePaths, Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::StartAncoMove(AncoRestoreResult &ancoRestoreRes)
{
    try {
        string callerName;
        ErrCode ret = VerifyCallerAndGetCallerName(callerName, false);
        if (ret != ERR_OK) {
            HILOGE("StartAncoMove error, Get bundle name failed, ret:%{public}d", ret);
            return ret;
        }
        auto enhanceService = EnhanceServiceManager::GetInstance().GetServiceInstance();
        if (!enhanceService) {
            HILOGW("StartAncoMove, enhance service is not loaded");
            return BError(BError::Codes::OK);
        }
        return enhanceService->StartAncoMove(callerName, ancoRestoreRes);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (...) {
        HILOGE("StartAncoMove, Unexpected exception");
        return EPERM;
    }
}
}  // namespace OHOS::FileManagement::Backup