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
    const static string COMMON_EVENT_TYPE = "broadcast";
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

std::vector<BJsonUtil::BundleDetailInfo> BJsonUtil::ConvertBundleDetailInfos(
    const std::vector<std::string> &bundleNameIndexStrs,
    const std::vector<std::string> &details, const std::string &patternInfo, std::vector<std::string> &realBundleNames)
{
    std::vector<BundleDetailInfo> bundleDetailInfoList;
    for (size_t pos = 0; pos < bundleNameIndexStrs.size(); pos++)
    {
        std::string bundleNameIndexStr = bundleNameIndexStrs[pos];
        BundleDetailInfo bundleDetailInfo;
        size_t hasPos = bundleNameIndexStr.find(patternInfo);
        if (hasPos == std::string::npos) {
            bundleDetailInfo.bundleName = bundleNameIndexStr;
            bundleDetailInfo.bundleIndex = BUNDLE_INDEX_DEFAULT_VAL;
            realBundleNames.push_back(bundleNameIndexStr);
        } else {
            std::string bundleName = bundleNameIndexStr.substr(0, hasPos);
            std::string indexStr = bundleNameIndexStr.substr(hasPos + 1);
            int index = std::stoi(indexStr);
            bundleDetailInfo.bundleName = bundleName;
            bundleDetailInfo.bundleIndex = index;
            realBundleNames.push_back(bundleNameIndexStr);
        }
        if (pos < details.size()) {
            std::string bundleDetailStr = details[pos];
            ParseBundleDetailInfo(bundleDetailStr, bundleDetailInfo);
        }
        bundleDetailInfoList.push_back(bundleDetailInfo);
    }
    return bundleDetailInfoList;
}

void BJsonUtil::ParseBundleDetailInfo(const std::string &bundleDetailInfo, BundleDetailInfo &bundleDetail)
{
    cJSON *root = cJSON_Parse(bundleDetailInfo.c_str());
    if (root == nullptr) {
        HILOGE("Parse json error,root is null");
        return;
    }
    cJSON *infos = cJSON_GetObjectItem(root, "infos");
    if (!cJSON_IsArray(infos)) {
        HILOGE("Parse json error, infos is not array");
        cJSON_Delete(root);
        return;
    }
    int infosCount = cJSON_GetArraySize(infos);
    for (int i = 0; i < infosCount; i++)
    {
        cJSON *infoItem = cJSON_GetArrayItem(infos, i);
        if (!cJSON_IsObject(infoItem)) {
            HILOGE("Parse json error, info item is not an object");
            cJSON_Delete(root);
            return;
        }
        cJSON *type = cJSON_GetObjectItem(infoItem, "type");
        if (type && cJSON_IsString(type)) {
            if (type->valuestring == COMMON_EVENT_TYPE) {
                bundleDetail.type = type->valuestring;
            }
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

void BJsonUtil::RecordBundleDetailRelation(std::map<std::string,BundleDetailInfo> &bundleNameDetailMap,
    std::vector<BundleDetailInfo> &bundleDetailInfos)
{
    for (auto &bundleDetail : bundleDetailInfos)
    {
        std::string bundleName = bundleDetail.bundleName;
        bundleNameDetailMap[bundleName] = bundleDetail;
    }
}
}



