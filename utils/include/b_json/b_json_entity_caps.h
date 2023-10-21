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

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_CAPS_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_CAPS_H

#include "b_json/b_json_cached_entity.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
class BJsonEntityCaps : public BJsonEntity {
public:
    struct BundleInfo {
        std::string name;
        uint32_t versionCode;
        std::string versionName;
        int64_t spaceOccupied;
        bool allToBackup;
        std::string extensionName;
        bool needToInstall {false};
        std::string restoreDeps;
    };

public:
    void SetSystemFullName(std::string systemFullName)
    {
        obj_["systemFullName"] = systemFullName;
    }

    void SetDeviceType(std::string deviceType)
    {
        obj_["deviceType"] = deviceType;
    }

    void SetBundleInfos(std::vector<BundleInfo> bundleInfos)
    {
        if (obj_.isMember("bundleInfos")) {
            obj_["bundleInfos"].clear();
        }
        for (const auto &item : bundleInfos) {
            Json::Value arrObj;
            arrObj["name"] = item.name;
            arrObj["versionCode"] = item.versionCode;
            arrObj["versionName"] = item.versionName;
            arrObj["spaceOccupied"] = item.spaceOccupied;
            arrObj["allToBackup"] = item.allToBackup;
            arrObj["extensionName"] = item.extensionName;
            arrObj["needToInstall"] = item.needToInstall;
            arrObj["restoreDeps"] = item.restoreDeps;
            obj_["bundleInfos"].append(arrObj);
        }
    }

    std::string GetSystemFullName()
    {
        if (!obj_ || !obj_.isMember("systemFullName") || !obj_["systemFullName"].isString()) {
            HILOGI("Failed to get field systemFullName");
            return "";
        }

        return obj_["systemFullName"].asString();
    }

    std::string GetDeviceType()
    {
        if (!obj_ || !obj_.isMember("deviceType") || !obj_["deviceType"].isString()) {
            HILOGI("Failed to get field deviceType");
            return "";
        }

        return obj_["deviceType"].asString();
    }

    std::string GetRestoreDeps()
    {
        if (!obj_ || !obj_.isMember("restoreDeps") || !obj_["restoreDeps"].isString()) {
            HILOGI("Failed to get field restoreDeps");
            return "";
        }

        return obj_["restoreDeps"].asString();
    }

    std::vector<BundleInfo> GetBundleInfos()
    {
        if (!obj_ || !obj_.isMember("bundleInfos") || !obj_["bundleInfos"].isArray()) {
            HILOGI("Failed to get field get bundleInfos");
            return {};
        }
        std::vector<BundleInfo> bundleInfos;
        for (const auto &item : obj_["bundleInfos"]) {
            if (!item || !item["name"].isString() || !item["versionCode"].isUInt() || !item["versionName"].isString() ||
                !item["spaceOccupied"].isInt64() || !item["allToBackup"].isBool() ||
                !item["extensionName"].isString() || !item["needToInstall"].isBool() || !item["restoreDeps"].isString()) {
                HILOGI("Failed to get field bundleInfos, type error");
                return {};
            }
            bundleInfos.emplace_back(BundleInfo {item["name"].asString(), item["versionCode"].asUInt(),
                                                 item["versionName"].asString(), item["spaceOccupied"].asInt64(),
                                                 item["allToBackup"].asBool(), item["extensionName"].asString(),
                                                 item["needToInstall"].asBool(), item["restoreDeps"].asString()});
        }
        return bundleInfos;
    }

public:
    /**
     * @brief 构造方法，具备T(Json::Value&, std::any)能力的构造函数
     *
     * @param obj Json对象引用
     * @param option 任意类型对象
     */
    explicit BJsonEntityCaps(Json::Value &obj, std::any option = std::any()) : BJsonEntity(obj, option)
    {
        SetBundleInfos(GetBundleInfos());
    }

    BJsonEntityCaps() = delete;
    ~BJsonEntityCaps() override = default;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_CAPS_H