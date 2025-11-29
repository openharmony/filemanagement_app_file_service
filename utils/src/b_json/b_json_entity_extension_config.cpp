/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "b_json/b_json_entity_extension_config.h"

#include <any>
#include <fcntl.h>
#include <string_view>
#include <sys/types.h>
#include <unistd.h>

#include "b_error/b_error.h"
#include "b_filesystem/b_file.h"
#include "b_ohos/startup/backup_para.h"
#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

vector<string> BJsonEntityExtensionConfig::GetIncludes() const
{
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return {BConstants::PATHES_TO_BACKUP.begin(), BConstants::PATHES_TO_BACKUP.end()};
    }
    if (!obj_.isMember("includes")) {
        HILOGE("'includes' field not found");
        return {BConstants::PATHES_TO_BACKUP.begin(), BConstants::PATHES_TO_BACKUP.end()};
    }
    if (!obj_["includes"].isArray()) {
        HILOGE("'includes' field must be an array");
        return {BConstants::PATHES_TO_BACKUP.begin(), BConstants::PATHES_TO_BACKUP.end()};
    }

    vector<string> dirs;
    for (auto &&item : obj_["includes"]) {
        if (!item.isString() || item.empty()) {
            HILOGE("Each item of array 'includes' must be of the type string");
            continue;
        }
        dirs.emplace_back(item.asString());
    }

    if (dirs.empty()) {
        dirs.emplace_back("");
    }
    return dirs;
}

vector<string> BJsonEntityExtensionConfig::GetExcludes() const
{
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return {};
    }
    if (!obj_.isMember("excludes")) {
        HILOGD("'excludes' field not found");
        return {};
    }
    if (!obj_["excludes"].isArray()) {
        HILOGE("'excludes' field must be an array");
        return {};
    }

    vector<string> dirs;
    for (auto &&item : obj_["excludes"]) {
        if (!item.isString() || item.empty()) {
            HILOGE("Each item of array 'excludes' must be of the type string");
            continue;
        }
        dirs.emplace_back(item.asString());
    }
    return dirs;
}

bool BJsonEntityExtensionConfig::GetAllowToBackupRestore() const
{
    if (!obj_ || !obj_.isMember("allowToBackupRestore") || !obj_["allowToBackupRestore"].isBool()) {
        HILOGD("Failed to get field allowToBackupRestore");
        return false;
    }

    return obj_["allowToBackupRestore"].asBool();
}

bool BJsonEntityExtensionConfig::GetFullBackupOnly() const
{
    if (!obj_ || !obj_.isMember("fullBackupOnly") || !obj_["fullBackupOnly"].isBool()) {
        HILOGD("Failed to get field fullBackupOnly");
        return false;
    }

    return obj_["fullBackupOnly"].asBool();
}

string BJsonEntityExtensionConfig::GetSupportScene() const
{
    if (!obj_ || !obj_.isMember("supportScene") || !obj_["supportScene"].isString()) {
        HILOGD("Failed to get field supportScene");
        return "";
    }

    return obj_["supportScene"].asString();
}

Json::Value BJsonEntityExtensionConfig::GetExtraInfo() const
{
    if (!obj_ || !obj_.isMember("extraInfo") || !obj_["extraInfo"].isObject()) {
        HILOGD("Failed to get field extraInfo");
        return Json::Value();
    }
    
    return obj_["extraInfo"];
}

string BJsonEntityExtensionConfig::GetJSonSource(string_view jsonFromRealWorld, any option)
{
    if (!BackupPara().GetBackupDebugOverrideExtensionConfig()) {
        return string(jsonFromRealWorld);
    }

    /* Server for test case, the user function does not execute the following code. */
    if (!option.has_value()) {
        if (getuid() == static_cast<uid_t>(BConstants::BACKUP_UID)) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Current process is not extension process");
        }
        string jsonFilePath = string(BConstants::BACKUP_CONFIG_EXTENSION_PATH).append(BConstants::BACKUP_CONFIG_JSON);
        UniqueFd fd(open(jsonFilePath.c_str(), O_RDONLY));
        if (fd < 0) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, "open json file failed");
        }
        return BFile::ReadFile(std::move(fd)).get();
    }

    if (getuid() != static_cast<uid_t>(BConstants::BACKUP_UID)) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Current process is not service process");
    }
    string bundleName;
    try {
        bundleName = any_cast<string>(option);
    } catch (const bad_any_cast &e) {
        throw BError(BError::Codes::SA_INVAL_ARG, e.what());
    }
    string jsonFilePath = string(BConstants::GetSaBundleBackupRootDir(BConstants::DEFAULT_USER_ID)).
                                append(bundleName).
                                append("/").
                                append(BConstants::BACKUP_CONFIG_JSON);
    if (access(jsonFilePath.c_str(), F_OK) != 0) {
        HILOGI("Failed to access jsonFilePath : %{public}s", jsonFilePath.c_str());
        return string(jsonFromRealWorld);
    }
    UniqueFd fd(open(jsonFilePath.c_str(), O_RDONLY));
    if (fd < 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, "open json file failed");
    }
    return BFile::ReadFile(std::move(fd)).get();
}

string BJsonEntityExtensionConfig::GetRestoreDeps() const
{
    if (!obj_ || !obj_.isMember("restoreDeps") || !obj_["restoreDeps"].isString()) {
        HILOGD("Failed to get field restoreDeps");
        return "";
    }

    return obj_["restoreDeps"].asString();
}

bool BJsonEntityExtensionConfig::GetRequireCompatibility() const
{
    if (!obj_ || !obj_.isMember("requireCompatibility") || !obj_["requireCompatibility"].isBool()) {
        HILOGD("Failed to get field requireCompatibility");
        return false;
    }

    return obj_["requireCompatibility"].asBool();
}

string BJsonEntityExtensionConfig::GetBackupScene() const
{
    if (!obj_ || !obj_.isMember("backupScene") || !obj_["backupScene"].isString()) {
        HILOGD("Failed to get field backupScene");
        return "";
    }

    return obj_["backupScene"].asString();
}

bool BJsonEntityExtensionConfig::HasOptionDir() const
{
    if (!obj_ || !obj_.isMember("optionDir") || !obj_["optionDir"].isArray()) {
        HILOGE("OptionDir is not exist");
        return false;
    }
    return true;
}

vector<string> BJsonEntityExtensionConfig::GetOptionDir(const string& sceneId, const string& fileType) const
{
    if (!HasOptionDir()) {
        return {};
    }
    for (auto &&item : obj_["optionDir"]) {
        if (item.empty()) {
            HILOGE("Each item of array 'optionDir' must be not empty");
            continue;
        }
        if (item.isMember("sceneId") && item["sceneId"] == sceneId && item.isMember(fileType)) {
            return GetDirList(item[fileType]);
        }
    }
    return {};
}

vector<string> BJsonEntityExtensionConfig::GetDirList(Json::Value& jsonItem) const
{
    vector<string> dirs;
    if (!jsonItem.isArray()) {
        HILOGE("item must be not array");
        return dirs;
    }
    for (auto &&item : jsonItem) {
        if (!item.isString() || item.empty()) {
            HILOGE("Each item of array must be of the type string");
            continue;
        }
        dirs.emplace_back(item.asString());
    }

    if (dirs.empty()) {
        dirs.emplace_back("");
    }
    return dirs;
}

std::unordered_map<std::string, std::string> BJsonEntityExtensionConfig::GetCompatibleDirMapping() const
{
    if (!HasArray(BConstants::COMPATIBLE_DIR_MAPPING)) {
        return {};
    }
    std::unordered_map<std::string, std::string> mapping;
    for (auto &&item : obj_[BConstants::COMPATIBLE_DIR_MAPPING]) {
        if (!item.isObject() || item.empty()) {
            HILOGE("item is not object or item empty");
            continue;
        }
        if (!item.isMember(BConstants::BACKUP_DIR) || !item[BConstants::BACKUP_DIR].isString()
            || item[BConstants::BACKUP_DIR].empty()) {
            HILOGE("item's backupDir is invalid");
            continue;
        }
        if (!item.isMember(BConstants::RESTORE_DIR) || !item[BConstants::RESTORE_DIR].isString()
            || item[BConstants::RESTORE_DIR].empty()) {
            HILOGE("item's restoreDir is invalid");
            continue;
        }
        std::string restoreDir = item[BConstants::RESTORE_DIR].asString();
        std::string backupDir = item[BConstants::BACKUP_DIR].asString();
        if (restoreDir.find(BConstants::BACKUP_RESTORE_DIR_SEPARATOR) != std::string::npos ||
            restoreDir.find(BConstants::DIR_WILDCARD) != std::string::npos) {
            HILOGE("config compatDirMapping.restoreDir contain invalid char!");
            continue;
        }
        if (backupDir.find(BConstants::BACKUP_RESTORE_DIR_SEPARATOR) != std::string::npos ||
            backupDir.find(BConstants::DIR_WILDCARD) != std::string::npos) {
            HILOGE("config compatDirMapping.backupDir contain invalid char!");
            continue;
        }
        if (mapping.count(restoreDir) > 0) {
            HILOGE("restoreDir repeat, retoreDir:%{public}s", GetAnonyPath(restoreDir).c_str());
            continue;
        }
        mapping.emplace(restoreDir, backupDir);
        if (mapping.size() > BConstants::MAX_COMPAT_DIR_COUNT) {
            HILOGE("compat dir mapping size is too big!");
            return {};
        }
    }
    return mapping;
}
} // namespace OHOS::FileManagement::Backup
