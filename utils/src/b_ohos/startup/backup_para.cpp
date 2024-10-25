/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "b_ohos/startup/backup_para.h"

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "parameter.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
const char* BACKUP_DEBUG_STATE = "sys.backup.check.enable";
/**
 * @brief 获取配置参数的值
 *
 * @param key 配置参数的参数名
 * @param len 配置参数值的最大长度
 * @return 成功获取配置参数的值则返回true，失败则返回false；以及表示配置参数值的字符串
 */
static tuple<bool, string> GetConfigParameterValue(const string &key, uint32_t len)
{
    char configParam[] = "false";
    int length = GetParameter(key.c_str(), "", configParam, len + 1);
    if (length <= 0) {
        HILOGE("Fail to GetParameter name = %{public}s, length = %{public}d", key.c_str(), length);
        return {false, ""};
    }
    return {true, configParam};
}

bool BackupPara::GetBackupDebugOverrideExtensionConfig()
{
    auto [getCfgParaValSucc, value] = GetConfigParameterValue(BConstants::BACKUP_DEBUG_OVERRIDE_EXTENSION_CONFIG_KEY,
                                                              BConstants::BACKUP_PARA_VALUE_MAX);
    if (!getCfgParaValSucc) {
        HILOGE("Fail to get configuration parameter value of backup.para, return default value");
        return BConstants::BACKUP_DEBUG_OVERRIDE_EXTENSION_CONFIG_DEFAULT_VALUE;
    }
    return value == "true";
}

bool BackupPara::GetBackupOverrideBackupSARelease()
{
    auto [getCfgParaValSucc, value] =
        GetConfigParameterValue(BConstants::BACKUP_OVERRIDE_BACKUP_SA_RELEASE_KEY, BConstants::BACKUP_PARA_VALUE_MAX);
    if (!getCfgParaValSucc) {
        HILOGE("Fail to get configuration parameter value of backup.para, return default value");
        return BConstants::BACKUP_DEBUG_OVERRIDE_BACKUP_SA_RELEASE_DEFAULT_VALUE;
    }
    return value == "true";
}

bool BackupPara::GetBackupOverrideIncrementalRestore()
{
    auto [getCfgParaValSucc, value] =
        GetConfigParameterValue(BConstants::BACKUP_OVERRIDE_INCREMENTAL_KEY, BConstants::BACKUP_PARA_VALUE_MAX);
    if (!getCfgParaValSucc) {
        HILOGE("Fail to get configuration parameter value of backup.para, return default value");
        return BConstants::BACKUP_DEBUG_OVERRIDE_INCREMENTAL_DEFAULT_VALUE;
    }
    HILOGI("Get Parse IncrementalRestore result, value: %{public}s", value.c_str());
    return value == "true";
}

tuple<bool, int32_t> BackupPara::GetBackupDebugOverrideAccount()
{
    auto [getCfgParaValSucc, value] = GetConfigParameterValue(BConstants::BACKUP_DEBUG_OVERRIDE_ACCOUNT_CONFIG_KEY,
                                                              BConstants::BACKUP_PARA_VALUE_MAX);
    if (!getCfgParaValSucc) {
        return {false, 0};
    }
    if (value == "true") {
        auto [getCfgParaValSucc, value] = GetConfigParameterValue(BConstants::BACKUP_DEBUG_OVERRIDE_ACCOUNT_NUMBER_KEY,
                                                                  BConstants::BACKUP_PARA_VALUE_MAX);
        if (!getCfgParaValSucc) {
            return {false, 0};
        }
        return {true, atoi(value.c_str())};
    }
    return {false, 0};
}

bool BackupPara::GetBackupDebugState()
{
    char paraValue[30] = {0}; // 30: for system paramter
    auto res = GetParameter(BACKUP_DEBUG_STATE, "-1", paraValue, sizeof(paraValue));
    if (res <= 0) {
        HILOGE("GetParameter fail, key:%{public}s res:%{public}d", BACKUP_DEBUG_STATE, res);
        return false;
    }
    std::string result(paraValue);
    return result == "true";
}
} // namespace OHOS::FileManagement::Backup