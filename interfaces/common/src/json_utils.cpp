/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "json_utils.h"

#include <fstream>
#include <sstream>

#include "log.h"

namespace OHOS {
namespace AppFileService {

int32_t JsonUtils::GetJsonObjFromPath(nlohmann::json& jsonObj, const std::string& jsonPath)
{
    std::ifstream jsonFileStream;
    jsonFileStream.open(jsonPath.c_str(), std::ios::in);
    if (!jsonFileStream.is_open()) {
        LOGE("Open json file path %{public}s failed with error %{public}d", jsonPath.c_str(), errno);
        return -errno;
    }

    std::ostringstream buf;
    char ch;
    while (buf && jsonFileStream.get(ch)) {
        buf.put(ch);
    }
    jsonFileStream.close();

    jsonObj = nlohmann::json::parse(buf.str(), nullptr, false);
    if (jsonObj.is_discarded()) {
        LOGE("Parse json file path %{public}s failed", jsonPath.c_str());
    }
    return 0;
}

int32_t JsonUtils::GetKVFromJson(const nlohmann::json &json, const std::string &key,
                                 std::string &value)
{
    if (!json.is_object()) {
        LOGE("Invalid json object");
        return -EINVAL;
    }

    bool ret = json.find(key) != json.end() && json.at(key).is_string();
    if (ret) {
        value = json.at(key).get<std::string>();
        return 0;
    } else {
        return -EINVAL;
    }
}

} // AppFileService
} // OHOS