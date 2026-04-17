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

#include "anco_restore_result.h"

#include "cJSON.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include <cstdint>
#include <memory>

namespace OHOS::FileManagement::Backup {
using namespace std;

struct CJsonDeleter {
    void operator()(cJSON* ptr) const {
        if (ptr != nullptr) {
            cJSON_Delete(ptr);
        }
    }
};

void AncoRestoreResult::Serialize()
{
    unique_ptr<cJSON, CJsonDeleter> root(cJSON_CreateObject());
    if (root == nullptr) {
        HILOGE("fail to create root");
        return;
    }

    cJSON_AddNumberToObject(root.get(), "successCount", successCount);
    cJSON_AddNumberToObject(root.get(), "duplicateCount", duplicateCount);
    cJSON_AddNumberToObject(root.get(), "failedCount", failedCount);
    AddEndFileInfos(root.get());
    AddErrFileInfos(root.get());

    const char *jsonString = cJSON_Print(root.get());
    if (jsonString) {
        size = strlen(jsonString);
        data = jsonString;
    }
}

void AncoRestoreResult::AddEndFileInfos(cJSON* root)
{
    unique_ptr<cJSON, CJsonDeleter> endFileInfosJson(cJSON_CreateObject());
    if (endFileInfosJson == nullptr) {
        HILOGE("fail to serialize end file info");
        return;
    }
    for (const auto &pair : endFileInfos) {
        cJSON_AddNumberToObject(endFileInfosJson.get(), pair.first.c_str(), pair.second);
    }
    cJSON_AddItemToObject(root, "endFileInfos", endFileInfosJson.release());
}

void AncoRestoreResult::AddErrFileInfos(cJSON* root)
{
    unique_ptr<cJSON, CJsonDeleter> errFileInfosJson(cJSON_CreateObject());
    if (errFileInfosJson == nullptr) {
        HILOGE("fail to serialize err file info");
        return;
    }
    for (const auto &pair : errFileInfos) {
        unique_ptr<cJSON, CJsonDeleter> errCodesJson(cJSON_CreateArray());
        if (errCodesJson == nullptr) {
            HILOGE("fail to serialize err file info");
            return;
        }
        for (int errCode : pair.second) {
            cJSON_AddItemToArray(errCodesJson.get(), cJSON_CreateNumber(errCode));
        }
        cJSON_AddItemToObject(errFileInfosJson.get(), pair.first.c_str(), errCodesJson.release());
    }
    cJSON_AddItemToObject(root, "errFileInfos", errFileInfosJson.release());
}

int32_t AncoRestoreResult::RawDataCpy(const void* readdata) {
    if (!readdata) {
        HILOGE("raw data is empty");
        return -1;
    }

    const char* jsonString = static_cast<const char*>(readdata);
    unique_ptr<cJSON, CJsonDeleter> root(cJSON_Parse(jsonString));
    if (!root) {
        HILOGE("fail to parse");
        return -1;
    }

    if (!ParseBasicCounts(root.get())) {
        HILOGE("fail to deserialize basic counts");
        return -1;
    }
    if (!ParseEndFileInfos(root.get())) {
        HILOGE("fail to deserialize end file info");
        return -1;
    }
    if (!ParseErrFileInfos(root.get())) {
        HILOGE("fail to deserialize err file info");
        return -1;
    }

    return 0;
}

bool AncoRestoreResult::ParseBasicCounts(cJSON* root) {
    cJSON *successCountItem = cJSON_GetObjectItem(root, "successCount");
    if (successCountItem == nullptr) {
        return false;
    }
    successCount = successCountItem->valueint;

    cJSON *duplicateCountItem = cJSON_GetObjectItem(root, "duplicateCount");
    if (duplicateCountItem == nullptr) {
        return false;
    }
    duplicateCount = duplicateCountItem->valueint;

    cJSON *failedCountItem = cJSON_GetObjectItem(root, "failedCount");
    if (failedCountItem == nullptr) {
        return false;
    }
    failedCount = failedCountItem->valueint;

    return true;
}

bool AncoRestoreResult::ParseEndFileInfos(cJSON* root) {
    cJSON *endFileInfosJson = cJSON_GetObjectItem(root, "endFileInfos");
    if (endFileInfosJson == nullptr) {
        return false;
    }
    cJSON *endItem = nullptr;
    cJSON_ArrayForEach(endItem, endFileInfosJson) {
        std::string key = endItem->string;
        int64_t value = endItem->valueint;
        endFileInfos[key] = value;
    }
    return true;
}

bool AncoRestoreResult::ParseErrFileInfos(cJSON* root) {
    cJSON *errFileInfosJson = cJSON_GetObjectItem(root, "errFileInfos");
    if (errFileInfosJson == nullptr) {
        return false;
    }
    cJSON *errItem = nullptr;
    cJSON_ArrayForEach(errItem, errFileInfosJson) {
        std::string key = errItem->string;
        std::vector<int> errCodes;
        cJSON *errCodesJson = errItem;
        cJSON *errCodeItem = nullptr;
        cJSON_ArrayForEach(errCodeItem, errCodesJson) {
            errCodes.push_back(errCodeItem->valueint);
        }
        errFileInfos[key] = errCodes;
    }
    return true;
}
}  // namespace OHOS::FileManagement::Backup