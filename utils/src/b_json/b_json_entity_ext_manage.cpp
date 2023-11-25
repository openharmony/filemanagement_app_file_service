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
#include "b_json/b_json_entity_ext_manage.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "json/value.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static bool CheckBigFile(const string &tarFile)
{
    HILOGI("CheckBigFile tarFile:%{public}s", tarFile.data());
    struct stat sta;
    int ret = stat(tarFile.c_str(), &sta);
    if (ret != 0) {
        HILOGE("stat file failed, file:%{public}s", tarFile.c_str());
        return false;
    }
    if (sta.st_size > BConstants::BIG_FILE_BOUNDARY) {
        return true;
    }
    return false;
}

static bool CheckUserTar(const string &fileName)
{
    if (access(fileName.c_str(), F_OK) != 0) {
        HILOGI("file does not exists");
        return false;
    }

    return (ExtractFileExt(fileName) == "tar") && CheckBigFile(fileName);
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
    sta.st_mode = value.isMember("st_mode") && value["st_mode"].isInt() ? value["st_mode"].asInt() : 0;
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

void BJsonEntityExtManage::SetExtManage(const map<string, tuple<string, struct stat, bool>> &info) const
{
    obj_.clear();

    vector<bool> vec(info.size(), false);

    auto FindLinks = [&vec](map<string, tuple<string, struct stat, bool>>::const_iterator it,
                            unsigned long index) -> set<string> {
        if (std::get<1>(it->second).st_dev == 0 || std::get<1>(it->second).st_ino == 0) {
            return {};
        }

        set<string> lks;
        auto item = it;
        item++;

        for (auto i = index + 1; i < vec.size(); ++i, ++item) {
            if (std::get<1>(it->second).st_dev == std::get<1>(item->second).st_dev &&
                std::get<1>(it->second).st_ino == std::get<1>(item->second).st_ino) {
                vec[i] = true;
                lks.insert(std::get<0>(item->second));
                HILOGI("lks insert %{public}s", std::get<0>(item->second).c_str());
            }
            HILOGI("lks doesn't insert %{public}s", std::get<0>(item->second).c_str());
        }
        return lks;
    };

    unsigned long index = 0;
    for (auto item = info.begin(); item != info.end(); ++item, ++index) {
        if (vec[index]) {
            HILOGI("skipped file is %{public}s", item->first.c_str());
            continue;
        }
        HILOGI("file name is %{public}s", item->first.c_str());

        Json::Value value;
        value["fileName"] = item->first;
        if (std::get<0>(item->second) == BConstants::RESTORE_INSTALL_PATH) {
            throw BError(BError::Codes::UTILS_INVAL_JSON_ENTITY, "Failed to set ext manage, invalid path");
        }
        auto [path, sta, isBeforeTar] = item->second;
        value["information"]["path"] = path;
        value["information"]["stat"] = Stat2JsonValue(sta);
        value["isUserTar"] = isBeforeTar && CheckUserTar(path);
        value["isBigFile"] = CheckBigFile(path);
        set<string> lks = FindLinks(item, index);
        for (const auto &lk : lks) {
            value["hardlinks"].append(lk);
        }

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
        if (path == BConstants::RESTORE_INSTALL_PATH) {
            throw BError(BError::Codes::UTILS_INVAL_JSON_ENTITY, "Failed to get ext manage info, invalid path");
        }
        if (item["information"].isMember("stat")) {
            sta = JsonValue2Stat(item["information"]["stat"]);
        }
        string fileName = item.isMember("fileName") && item["fileName"].isString() ? item["fileName"].asString() : "";
        bool isUserTar = item.isMember("isUserTar") && item["isUserTar"].isBool() ? item["isUserTar"].asBool() : false;
        bool isBigFile = item.isMember("isBigFile") && item["isBigFile"].isBool() ? item["isBigFile"].asBool() : false;
        HILOGI("GetExtManageInfo, fileName:%{public}s, isUserTar:%{public}d, isBigFile:%{public}d", fileName.data(),
               isUserTar, isBigFile);
        if (!fileName.empty() && !path.empty()) {
            ExtManageInfo info = {
                .hashName = fileName, .fileName = path, .sta = sta, .isUserTar = isUserTar, .isBigFile = isBigFile};
            infos.emplace_back(info);
        }
    }

    return infos;
}

bool BJsonEntityExtManage::SetHardLinkInfo(const string origin, const set<string> hardLinks)
{
    if (origin.empty()) {
        HILOGE("origin file name can not empty");
        return false;
    }
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return false;
    }
    if (!obj_.isArray()) {
        HILOGE("json object isn't an array");
        return false;
    }

    for (Json::Value &item : obj_) {
        string fileName = item.isObject() && item.isMember("fileName") && item["fileName"].isString()
                              ? item["fileName"].asString()
                              : "";
        if (origin == fileName) {
            for (const auto &lk : hardLinks) {
                item["hardlinks"].append(lk);
            }
            return true;
        }
    }

    return false;
}

const set<string> BJsonEntityExtManage::GetHardLinkInfo(const string origin)
{
    if (origin.empty()) {
        HILOGE("origin file name can not empty");
        return {};
    }
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return {};
    }
    if (!obj_.isArray()) {
        HILOGE("json object isn't an array");
        return {};
    }

    set<string> hardlinks;
    for (const Json::Value &item : obj_) {
        if (!item.isObject()) {
            continue;
        }
        string fileName = item.isMember("fileName") && item["fileName"].isString() ? item["fileName"].asString() : "";
        if (origin != fileName) {
            continue;
        }
        if (!(item.isMember("hardlinks") && item["hardlinks"].isArray())) {
            break;
        }
        for (const auto &lk : item["hardlinks"]) {
            if (lk.isString()) {
                hardlinks.emplace(lk.asString());
            }
        }
    }

    return hardlinks;
}
} // namespace OHOS::FileManagement::Backup