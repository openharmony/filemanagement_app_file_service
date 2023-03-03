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

#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "b_json/b_json_entity_ext_manage.h"
#include "filemgmt_libhilog.h"
#include "json/value.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

Json::Value Stat2JsonValue(struct stat sta)
{
    Json::Value value;

    value["st_size"] = static_cast<uint64_t>(sta.st_size);
    value["st_atim"]["tv_sec"] = static_cast<uint64_t>(sta.st_atim.tv_sec);
    value["st_atim"]["tv_nsec"] = static_cast<uint64_t>(sta.st_atim.tv_nsec);
    value["st_mtim"]["tv_sec"] = static_cast<uint64_t>(sta.st_mtim.tv_sec);
    value["st_mtim"]["tv_nsec"] = static_cast<uint64_t>(sta.st_mtim.tv_nsec);

    return value;
}

struct stat JsonValue2Stat(const Json::Value &value)
{
    struct stat sta = {};

    sta.st_size = value.isMember("st_size") ? value["st_size"].asInt64() : 0;
    if (value.isMember("st_atim")) {
        sta.st_atim.tv_sec = value["st_atim"].isMember("tv_sec") ? value["st_atim"]["tv_sec"].asInt64() : 0;
        sta.st_atim.tv_nsec = value["st_atim"].isMember("tv_nsec") ? value["st_atim"]["tv_nsec"].asInt64() : 0;
    }
    if (value.isMember("st_mtim")) {
        sta.st_mtim.tv_sec = value["st_mtim"].isMember("tv_sec") ? value["st_mtim"]["tv_sec"].asInt64() : 0;
        sta.st_mtim.tv_nsec = value["st_mtim"].isMember("tv_nsec") ? value["st_mtim"]["tv_nsec"].asInt64() : 0;
    }

    return sta;
}

void BJsonEntityExtManage::SetExtManage(const map<string, pair<string, struct stat>> &info) const
{
    obj_.clear();

    vector<bool> vec(info.size(), false);

    auto FindLinks = [&vec](map<string, pair<string, struct stat>>::const_iterator it,
                            unsigned long index) -> set<string> {
        if (it->second.second.st_dev == 0 || it->second.second.st_ino == 0) {
            return {};
        }

        set<string> lks;
        auto item = it;
        item++;

        for (auto i = index + 1; i < vec.size(); ++i, ++item) {
            if (it->second.second.st_dev == item->second.second.st_dev &&
                it->second.second.st_ino == item->second.second.st_ino) {
                vec[i] = true;
                lks.insert(item->second.first);
            }
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
        value["information"]["path"] = item->second.first;
        value["information"]["stat"] = Stat2JsonValue(item->second.second);
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
        string fileName = item.isMember("fileName") ? item["fileName"].asString() : "";
        info.emplace(fileName);
    }
    return info;
}

map<string, pair<string, struct stat>> BJsonEntityExtManage::GetExtManageInfo() const
{
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return {};
    }
    if (!obj_.isArray()) {
        HILOGE("json object isn't an array");
        return {};
    }

    map<string, pair<string, struct stat>> info;
    for (const Json::Value &item : obj_) {
        string fileName = item.isMember("fileName") ? item["fileName"].asString() : "";

        if (!item.isMember("information")) {
            continue;
        }

        struct stat sta = {};
        string path = item["information"].isMember("path") ? item["information"]["path"].asString() : "";
        if (item["information"].isMember("stat")) {
            sta = JsonValue2Stat(item["information"]["stat"]);
        }

        if (!fileName.empty() && !path.empty()) {
            info.emplace(fileName, make_pair(path, sta));
        }
    }

    return info;
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
        string fileName = item.isMember("fileName") ? item["fileName"].asString() : "";
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
        string fileName = item.isMember("fileName") ? item["fileName"].asString() : "";
        if (origin == fileName) {
            if (!(item.isMember("hardlinks") && item["hardlinks"].isArray())) {
                break;
            }
            for (const auto &lk : item["hardlinks"]) {
                hardlinks.emplace(lk.asString());
            }
        }
    }

    return hardlinks;
}
} // namespace OHOS::FileManagement::Backup