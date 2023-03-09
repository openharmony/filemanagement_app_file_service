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
    uint64_t GetFreeDiskSpace()
    {
        if (!obj_ || !obj_.isMember("freeDiskSpace") || !obj_["freeDiskSpace"].isUInt64()) {
            HILOGE("Failed to init field FreeDiskSpace");
            return 0;
        }

        return obj_["freeDiskSpace"].asUInt64();
    }

    void SetFreeDiskSpace(uint64_t freeDiskSpace)
    {
        obj_["freeDiskSpace"] = freeDiskSpace;
    }

    void SetOSFullName(std::string osFullName)
    {
        obj_["OSFullName"] = osFullName;
    }

    void SetDeviceType(std::string deviceType)
    {
        obj_["deviceType"] = deviceType;
    }

    std::string GetOSFullName()
    {
        if (!obj_ || !obj_.isMember("OSFullName") || !obj_["OSFullName"].isString()) {
            HILOGE("Failed to get field OSFullName");
            return "";
        }

        return obj_["OSFullName"].asString();
    }

    std::string GetDeviceType()
    {
        if (!obj_ || !obj_.isMember("deviceType") || !obj_["deviceType"].isString()) {
            HILOGE("Failed to get field deviceType");
            return "";
        }

        return obj_["deviceType"].asString();
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
        SetFreeDiskSpace(GetFreeDiskSpace());
    }

    BJsonEntityCaps() = delete;
    ~BJsonEntityCaps() override = default;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_CAPS_H