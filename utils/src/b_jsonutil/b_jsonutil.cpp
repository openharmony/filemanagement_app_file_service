/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "b_jsonutil/b_jsonutil.h"

#include <cstring>
#include "cJSON.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
namespace {
    const static int BUNDLE_INDEX_DEFAULT_VAL = 0;
    const static std::string BUNDLE_INDEX_SPLICE = ":";
}

BJsonUtil::BundleDetailInfo BJsonUtil::ParseBundleNameIndexStr(const std::string &bundleNameStr,
    const std::string &patternInfo)
{
    HILOGI("Start parse bundle name and index");
    size_t hasPos = bundleNameStr.find(patternInfo);
    BundleDetailInfo bundleDetailInfo;
    if (hasPos == std::string::npos) {
        bundleDetailInfo.bundleName = bundleNameStr;
        bundleDetailInfo.bundleIndex = BUNDLE_INDEX_DEFAULT_VAL;
        return bundleDetailInfo;
    }
    std::string bundleName = bundleNameStr.substr(0, hasPos);
    std::string indexStr = bundleNameStr.substr(hasPos + 1);
    int index = std::stoi(indexStr);
    bundleDetailInfo.bundleName = bundleName;
    bundleDetailInfo.bundleIndex = index;
    HILOGI("End parse bundle name and index");
    return bundleDetailInfo;
}

std::map<std::string, BJsonUtil::BundleDetailInfo> BJsonUtil::BuildBundleInfos(
    const std::vector<std::string> &bundleNames, const std::vector<std::string> &bundleInfos,
    std::vector<std::string> &bundleNamesOnly, int32_t userId)
{
    std::vector<BJsonUtil::BundleDetailInfo> bundleDetailInfos;
    std::map<std::string, BJsonUtil::BundleDetailInfo> bundleNameDetailMap;
    if (bundleNames.size() != bundleInfos.size()) {
        HILOGE("bundleNames count is not equals bundleInfos count");
        return bundleNameDetailMap;
    }
    HILOGI("Start BuildBundleInfos");
    for (size_t i = 0; i < bundleNames.size(); i++) {
        std::string bundleName = bundleNames[i];
        BJsonUtil::BundleDetailInfo bundleDetailInfo;
        bundleDetailInfo.userId = userId;
        size_t pos = bundleName.find(BUNDLE_INDEX_SPLICE);
        if (pos == 0 || pos == (bundleName.size() - 1)) {
            HILOGE("Current bundle name is wrong");
            continue;
        }
        if (pos == std::string::npos) {
            bundleDetailInfo.bundleName = bundleName;
            bundleDetailInfo.bundleIndex = BUNDLE_INDEX_DEFAULT_VAL;
            bundleNamesOnly.emplace_back(bundleName);
        } else {
            std::string bundleNameSplit = bundleName.substr(0, pos);
            std::string indexSplit = bundleName.substr(pos, bundleName.size() - 1);
            int index = std::stoi(indexSplit);
            bundleDetailInfo.bundleName = bundleNameSplit;
            bundleDetailInfo.bundleIndex = index;
            bundleNamesOnly.emplace_back(bundleNameSplit);
        }
        std::string bundleInfo = bundleInfos[i];
        ParseBundleInfoJson(bundleInfo, bundleDetailInfo);
        bundleDetailInfos.emplace_back(bundleDetailInfo);
        bundleNameDetailMap[bundleDetailInfo.bundleName] = bundleDetailInfo;
    }
    HILOGI("End BuildBundleInfos");
    return bundleNameDetailMap;
}

void BJsonUtil::ParseBundleInfoJson(const std::string &bundleInfo, BundleDetailInfo &bundleDetail)
{
    cJSON *root = cJSON_Parse(bundleInfo.c_str());
    if (root == nullptr) {
        HILOGE("Parse json error,root is null");
        return;
    }
    cJSON *infos = cJSON_GetObjectItem(root, "infos");
    if (infos == nullptr || !cJSON_IsArray(infos) || cJSON_GetArraySize(infos) == 0) {
        HILOGE("Parse json error, infos is not array");
        cJSON_Delete(root);
        return;
    }
    int infosCount = cJSON_GetArraySize(infos);
    for (int i = 0; i < infosCount; i++) {
        cJSON *infoItem = cJSON_GetArrayItem(infos, i);
        if (!cJSON_IsObject(infoItem)) {
            HILOGE("Parse json error, info item is not an object");
            cJSON_Delete(root);
            return;
        }
        cJSON *type = cJSON_GetObjectItem(infoItem, "type");
        if (type && cJSON_IsString(type)) {
            bundleDetail.type = type->valuestring;
        }
        cJSON *details = cJSON_GetObjectItem(infoItem, "details");
        if (details && cJSON_IsArray(details)) {
            char *detailInfos = cJSON_Print(details);
            bundleDetail.detail = std::string(detailInfos);
            free(detailInfos);
        }
    }
    cJSON_Delete(root);
}
}