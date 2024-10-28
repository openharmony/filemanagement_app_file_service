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
        int appIndex;
        int64_t versionCode;
        std::string versionName;
        int64_t spaceOccupied;
        int64_t increSpaceOccupied;
        bool allToBackup;
        bool fullBackupOnly;
        std::string extensionName;
        std::string restoreDeps;
        std::string supportScene;
        Json::Value extraInfo;
    };
    struct BundleBackupConfigPara {
        bool allToBackup;
        bool fullBackupOnly;
        std::string extensionName;
        std::string restoreDeps;
        std::string supportScene;
        std::vector<std::string> includes;
        std::vector<std::string> excludes;
        Json::Value extraInfo;
    };
public:
    void SetSystemFullName(const char *systemFullName)
    {
        if (systemFullName == nullptr) {
            HILOGE("systemFullName is nullptr, use default.");
            obj_["systemFullName"] = "default";
            return;
        }
        obj_["systemFullName"] = systemFullName;
    }

    void SetDeviceType(const char * deviceType)
    {
        if (deviceType == nullptr) {
            HILOGE("deviceType is nullptr, use default.");
            obj_["deviceType"] = "default";
            return;
        }
        obj_["deviceType"] = deviceType;
    }

    void SetBundleInfos(std::vector<BundleInfo> bundleInfos, bool incre = false)
    {
        if (obj_.isMember("bundleInfos")) {
            obj_["bundleInfos"].clear();
        }
        for (const auto &item : bundleInfos) {
            Json::Value arrObj;
            arrObj["name"] = item.name;
            arrObj["appIndex"] = item.appIndex;
            arrObj["versionCode"] = item.versionCode;
            arrObj["versionName"] = item.versionName;
            arrObj["spaceOccupied"] = item.spaceOccupied;
            if (incre) {
                arrObj["increSpaceOccupied"] = item.increSpaceOccupied;
            }
            arrObj["allToBackup"] = item.allToBackup;
            arrObj["fullBackupOnly"] = item.fullBackupOnly;
            arrObj["extensionName"] = item.extensionName;
            arrObj["restoreDeps"] = item.restoreDeps;
            arrObj["supportScene"] = item.supportScene;
            Json::Value extraInfo;
            if (item.extraInfo.empty()) {
                Json::Value senceArray(Json::arrayValue);
                extraInfo["supportScene"] = senceArray;
            } else {
                extraInfo = item.extraInfo;
            }
            arrObj["extraInfo"] = extraInfo;
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

    std::string GetSupportScene()
    {
        if (!obj_ || !obj_.isMember("supportScene") || !obj_["supportScene"].isString()) {
            HILOGI("Failed to get field supportScene");
            return "";
        }

        return obj_["supportScene"].asString();
    }

    Json::Value GetExtraInfo()
    {
        if (!obj_ || !obj_.isMember("extraInfo") || !obj_["extraInfo"].isObject()) {
            HILOGE("Failed to get field extraInfo");
            return Json::Value();
        }
        return obj_["extraInfo"];
    }

    bool CheckBundlePropertiesValid(const Json::Value &bundleInfo)
    {
        if (!bundleInfo) {
            HILOGE("Failed Check bundleInfo");
            return false;
        }
        if (!bundleInfo.isMember("name") || !bundleInfo["name"].isString()) {
            HILOGE("Failed Check bundleInfo name property");
            return false;
        }
        if (!bundleInfo.isMember("versionCode") || !bundleInfo["versionCode"].isInt64()) {
            HILOGE("Failed Check bundleInfo versionCode property");
            return false;
        }
        if (!bundleInfo.isMember("versionName") || !bundleInfo["versionName"].isString()) {
            HILOGE("Failed Check bundleInfo versionName property");
            return false;
        }
        if (!bundleInfo.isMember("spaceOccupied") || !bundleInfo["spaceOccupied"].isInt64()) {
            HILOGE("Failed Check bundleInfo spaceOccupied property");
            return false;
        }
        if (!bundleInfo.isMember("allToBackup") || !bundleInfo["allToBackup"].isBool()) {
            HILOGE("Failed Check bundleInfo allToBackup property");
            return false;
        }
        if (!bundleInfo.isMember("extensionName") || !bundleInfo["extensionName"].isString()) {
            HILOGE("Failed Check bundleInfo extensionName property");
            return false;
        }
        return true;
    }

    std::vector<BundleInfo> GetBundleInfos()
    {
        if (!obj_ || !obj_.isMember("bundleInfos") || !obj_["bundleInfos"].isArray()) {
            HILOGI("Failed to get field get bundleInfos");
            return {};
        }
        std::vector<BundleInfo> bundleInfos;
        for (const auto &item : obj_["bundleInfos"]) {
            if (!CheckBundlePropertiesValid(item)) {
                return {};
            }
            string restoreDeps("");
            if (item.isMember("restoreDeps") && item["restoreDeps"].isString()) {
                restoreDeps = item["restoreDeps"].asString();
            }
            string supportScene("");
            if (item.isMember("supportScene") && item["supportScene"].isString()) {
                restoreDeps = item["supportScene"].asString();
            }
            Json::Value extraInfo;
            if (item.isMember("extraInfo") && item["extraInfo"].isObject()) {
                extraInfo = item["extraInfo"];
            }
            bool fullBackupOnly = false;
            if (item.isMember("fullBackupOnly") && item["fullBackupOnly"].isBool()) {
                fullBackupOnly = item["fullBackupOnly"].asBool();
            }
            int appIndex = 0;
            if (item.isMember("appIndex") && item["appIndex"].isInt()) {
                appIndex = item["appIndex"].asInt();
            }
            int64_t increSpaceOccupied = 0;
            if (item.isMember("increSpaceOccupied") && item["increSpaceOccupied"].isInt64()) {
                increSpaceOccupied = item["increSpaceOccupied"].asInt64();
            }
            bundleInfos.emplace_back(BundleInfo {item["name"].asString(), appIndex, item["versionCode"].asInt64(),
                                                 item["versionName"].asString(), item["spaceOccupied"].asInt64(),
                                                 increSpaceOccupied, item["allToBackup"].asBool(), fullBackupOnly,
                                                 item["extensionName"].asString(),
                                                 restoreDeps, supportScene, extraInfo});
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