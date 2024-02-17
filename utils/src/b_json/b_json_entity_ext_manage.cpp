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

#include <directory_ex.h>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "json/value.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
namespace {
const int32_t DEFAULT_MODE = 0100660; // 0660
}

static bool CheckBigFile(struct stat sta)
{
    if (sta.st_size > BConstants::BIG_FILE_BOUNDARY) {
        return true;
    }
    return false;
}

/**
 * @brief 判断是否属于自身打包的文件
 *
 * @param fileName
 */
static bool CheckOwnPackTar(const string &fileName)
{
    if (ExtractFileExt(fileName) != "tar") {
        return false;
    }
    // 如果不是在默认路径下的文件包，不属于自身打包的tar文件
    string defaultBackupPath = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BACKUP);
    string absPath;
    try {
        absPath = IncludeTrailingPathDelimiter(BExcepUltils::Canonicalize(ExtractFilePath(fileName)));
    } catch (const BError &e) {
        HILOGE("file is not backup path");
        return false;
    } catch (...) {
        return false;
    }

    string::size_type pathPos = absPath.find(defaultBackupPath);
    if (pathPos == string::npos || pathPos != 0) {
        return false;
    }
    // 获取tar包名称
    string tarFile = ExtractFileName(fileName);
    string::size_type pos = tarFile.find(".");
    if (pos == string::npos) {
        return false;
    }

    string firstName = string(tarFile).substr(0, pos);
    // 判断文件名是否包含part (兼容增量)
    string::size_type partPos = firstName.find("part");
    if (partPos == string::npos) {
        return false;
    }

    return true;
}

static bool CheckUserTar(const string &fileName, struct stat sta)
{
    if (access(fileName.c_str(), F_OK) != 0) {
        HILOGI("file does not exists");
        return false;
    }
    return (ExtractFileExt(fileName) == "tar") && CheckBigFile(sta);
}

Json::Value Stat2JsonValue(struct stat sta)
{
    Json::Value value;
    value["st_size"] = static_cast<int64_t>(sta.st_size);
    value["st_mode"] = static_cast<int32_t>(sta.st_mode);
    value["st_atim"]["tv_sec"] = static_cast<int64_t>(sta.st_atim.tv_sec);
    value["st_atim"]["tv_nsec"] = static_cast<int64_t>(sta.st_atim.tv_nsec);
    value["st_mtim"]["tv_sec"] = static_cast<int64_t>(sta.st_mtim.tv_sec);
    value["st_mtim"]["tv_nsec"] = static_cast<int64_t>(sta.st_mtim.tv_nsec);
    return value;
}

struct stat JsonValue2Stat(const Json::Value &value)
{
    struct stat sta = {};

    if (!value.isObject()) {
        return sta;
    }

    sta.st_size = value.isMember("st_size") && value["st_size"].isInt64() ? value["st_size"].asInt64() : 0;
    sta.st_mode = value.isMember("st_mode") && value["st_mode"].isInt() ? value["st_mode"].asInt() : DEFAULT_MODE;
    if (value.isMember("st_atim")) {
        sta.st_atim.tv_sec = value["st_atim"].isMember("tv_sec") && value["st_atim"]["tv_sec"].isInt64()
                                 ? value["st_atim"]["tv_sec"].asInt64()
                                 : 0;
        sta.st_atim.tv_nsec = value["st_atim"].isMember("tv_nsec") && value["st_atim"]["tv_nsec"].isInt64()
                                  ? value["st_atim"]["tv_nsec"].asInt64()
                                  : 0;
    }
    if (value.isMember("st_mtim")) {
        sta.st_mtim.tv_sec = value["st_mtim"].isMember("tv_sec") && value["st_mtim"]["tv_sec"].isInt64()
                                 ? value["st_mtim"]["tv_sec"].asInt64()
                                 : 0;
        sta.st_mtim.tv_nsec = value["st_mtim"].isMember("tv_nsec") && value["st_mtim"]["tv_nsec"].isInt64()
                                  ? value["st_mtim"]["tv_nsec"].asInt64()
                                  : 0;
    }

    return sta;
}

void BJsonEntityExtManage::SetExtManageForClone(const map<string, tuple<string, struct stat, bool, bool>> &info) const
{
    obj_.clear();

    unsigned long index = 0;
    for (auto item = info.begin(); item != info.end(); ++item, ++index) {
        Json::Value value;
        value["fileName"] = item->first;
        auto [path, sta, isBigFile, isUserTar] = item->second;
        value["information"]["path"] = path;
        value["information"]["stat"] = Stat2JsonValue(sta);
        value["isUserTar"] = isUserTar;
        value["isBigFile"] = isBigFile;

        obj_.append(value);
    }
}

void BJsonEntityExtManage::SetExtManage(const map<string, tuple<string, struct stat, bool>> &info) const
{
    obj_.clear();

    for (auto item = info.begin(); item != info.end(); ++item) {
        Json::Value value;
        value["fileName"] = item->first;
        auto [path, sta, isBeforeTar] = item->second;
        value["information"]["path"] = path;
        value["information"]["stat"] = Stat2JsonValue(sta);
        value["isUserTar"] = isBeforeTar && CheckUserTar(path, sta);
        value["isBigFile"] = !CheckOwnPackTar(path) && CheckBigFile(sta);

        obj_.append(value);
    }
}

set<string> BJsonEntityExtManage::GetExtManage() const
{
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return {};
    }
    if (!obj_.isArray()) {
        HILOGE("json object isn't an array");
        return {};
    }

    set<string> info;
    for (Json::Value &item : obj_) {
        string fileName = item.isObject() && item.isMember("fileName") && item["fileName"].isString()
                              ? item["fileName"].asString()
                              : "";
        info.emplace(fileName);
    }
    return info;
}

std::vector<ExtManageInfo> BJsonEntityExtManage::GetExtManageInfo() const
{
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return {};
    }
    if (!obj_.isArray()) {
        HILOGE("json object isn't an array");
        return {};
    }

    std::vector<ExtManageInfo> infos {};
    for (const Json::Value &item : obj_) {
        if (!(item.isObject() && item.isMember("information"))) {
            continue;
        }

        struct stat sta = {};
        string path = item["information"].isMember("path") && item["information"]["path"].isString()
                          ? item["information"]["path"].asString()
                          : "";
        if (item["information"].isMember("stat")) {
            sta = JsonValue2Stat(item["information"]["stat"]);
        }
        string fileName = item.isMember("fileName") && item["fileName"].isString() ? item["fileName"].asString() : "";
        bool isUserTar = item.isMember("isUserTar") && item["isUserTar"].isBool() ? item["isUserTar"].asBool() : false;
        bool isBigFile = item.isMember("isBigFile") && item["isBigFile"].isBool() ? item["isBigFile"].asBool() : false;
        // 兼容旧版本没有isBigFile属性时,增加判断是否为bigFile
        if (!item.isMember("isBigFile") && fileName != "" && ExtractFileExt(fileName) != "tar") {
            isBigFile = true;
        }
        HILOGI("GetExtManageInfo, fileName:%{public}s, isUserTar:%{public}d, isBigFile:%{public}d", fileName.data(),
               isUserTar, isBigFile);
        if (!fileName.empty()) {
            ExtManageInfo info = {
                .hashName = fileName, .fileName = path, .sta = sta, .isUserTar = isUserTar, .isBigFile = isBigFile};
            infos.emplace_back(info);
        }
    }

    return infos;
}
} // namespace OHOS::FileManagement::Backup