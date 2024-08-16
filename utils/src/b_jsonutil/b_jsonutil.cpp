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

BJsonUtil::BundleDetailInfo BJsonUtil::ParseBundleNameIndexStr(const std::string &bundleNameStr)
{
    HILOGI("Start parse bundle name and index");
    size_t hasPos = bundleNameStr.find(BUNDLE_INDEX_SPLICE);
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

std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> BJsonUtil::BuildBundleInfos(
    const std::vector<std::string> &bundleNames, const std::vector<std::string> &bundleInfos,
    std::vector<std::string> &bundleNamesOnly, int32_t userId,
    std::map<std::string, bool> &isClearDataFlags)
{
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> bundleNameDetailMap;
    if (bundleNames.size() != bundleInfos.size()) {
        HILOGE("bundleNames count is not equals bundleInfos count");
        return bundleNameDetailMap;
    }
    HILOGI("Start BuildBundleInfos");
    for (size_t i = 0; i < bundleNames.size(); i++) {
        std::string bundleName = bundleNames[i];
        std::vector<BJsonUtil::BundleDetailInfo> bundleDetailInfos;
        size_t pos = bundleName.find(BUNDLE_INDEX_SPLICE);
        if (pos == 0 || pos == (bundleName.size() - 1)) {
            HILOGE("Current bundle name is wrong");
            continue;
        }
        std::string bundleNameOnly;
        int bundleIndex;
        if (pos == std::string::npos) {
            bundleNameOnly = bundleName;
            bundleIndex = BUNDLE_INDEX_DEFAULT_VAL;
            bundleNamesOnly.emplace_back(bundleName);
        } else {
            std::string bundleNameSplit = bundleName.substr(0, pos);
            std::string indexSplit = bundleName.substr(pos + 1);
            int index = std::stoi(indexSplit);
            bundleNameOnly = bundleNameSplit;
            bundleIndex = index;
            bundleNamesOnly.emplace_back(bundleNameSplit);
        }
        std::string bundleInfo = bundleInfos[i];
        bool isClearData = true;
        BJsonUtil::BundleDetailInfo bundleDetailInfo;
        bundleDetailInfo.bundleName = bundleNameOnly;
        bundleDetailInfo.bundleIndex = bundleIndex;
        bundleDetailInfo.userId = userId;
        ParseBundleInfoJson(bundleInfo, bundleDetailInfos, bundleDetailInfo, isClearData);
        isClearDataFlags[bundleName] = isClearData;
        bundleNameDetailMap[bundleName] = bundleDetailInfos;
    }
    HILOGI("End BuildBundleInfos");
    return bundleNameDetailMap;
}

void BJsonUtil::ParseBundleInfoJson(const std::string &bundleInfo, std::vector<BundleDetailInfo> &bundleDetails,
    BJsonUtil::BundleDetailInfo bundleDetailInfo, bool &isClearData)
{
    cJSON *root = cJSON_Parse(bundleInfo.c_str());
    if (root == nullptr) {
        HILOGE("Parse json error,root is null");
        return;
    }
    cJSON *clearBackupData = cJSON_GetObjectItem(root, "clearBackupData");
    if (clearBackupData == nullptr || !cJSON_IsString(clearBackupData) || (clearBackupData->valuestring == nullptr)) {
        HILOGE("Parse json error.");
    } else {
        std::string value = clearBackupData->valuestring;
        isClearData = value.compare("false") != 0;
        HILOGI("bundleName:%{public}s clear data falg:%{public}d", bundleDetailInfo.bundleName.c_str(), isClearData);
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
        if (type == nullptr || !cJSON_IsString(type) || (type->valuestring == nullptr)) {
            HILOGE("Parse json type element error");
            cJSON_Delete(root);
            return;
        }
        bundleDetailInfo.type = type->valuestring;
        cJSON *details = cJSON_GetObjectItem(infoItem, "details");
        if (details == nullptr || !cJSON_IsArray(details)) {
            HILOGE("Parse json details element error");
            cJSON_Delete(root);
            return;
        }
        char *detailInfos = cJSON_Print(details);
        bundleDetailInfo.detail = std::string(detailInfos);
        bundleDetails.emplace_back(bundleDetailInfo);
        cJSON_free(detailInfos);
    }
    cJSON_Delete(root);
}

bool BJsonUtil::FindBundleInfoByName(std::map<std::string, std::vector<BundleDetailInfo>> &bundleNameDetailsMap,
    std::string &bundleName, const std::string &jobType, BundleDetailInfo &bundleDetail)
{
    auto iter = bundleNameDetailsMap.find(bundleName);
    if (iter == bundleNameDetailsMap.end()) {
        return false;
    }
    std::vector<BJsonUtil::BundleDetailInfo> bundleDetailInfos = iter->second;
    for (auto &bundleDetailInfo : bundleDetailInfos) {
        if (bundleDetailInfo.type == jobType) {
            bundleDetail = bundleDetailInfo;
            return true;
        }
    }
    return false;
}

bool BJsonUtil::BuildRestoreErrInfo(std::string &jsonStr, int errCode, std::string errMsg)
{
    cJSON *info = cJSON_CreateObject();
    if (info == nullptr) {
        return false;
    }

    cJSON *errInfo = cJSON_CreateObject();
    if (errInfo == nullptr) {
        cJSON_Delete(info);
        return false;
    }
    cJSON_AddNumberToObject(errInfo, "errorCode", errCode);
    cJSON_AddStringToObject(errInfo, "errorInfo", errMsg.c_str());
    cJSON_AddStringToObject(errInfo, "type", "ErrorInfo");

    cJSON_AddItemToObject(info, "resultInfo", errInfo);

    char *data = cJSON_Print(info);
    if (data == nullptr) {
        cJSON_Delete(info);
        return false;
    }
    jsonStr = std::string(data);
    cJSON_Delete(info);
    cJSON_free(data);
    return true;
}

std::string BJsonUtil::BuildBundleNameIndexInfo(const std::string &bundleName, int appIndex)
{
    std::string result = bundleName;
    if (appIndex == BUNDLE_INDEX_DEFAULT_VAL) {
        return result;
    }
    result += BUNDLE_INDEX_SPLICE;
    result += std::to_string(appIndex);
    return result;
}

bool BJsonUtil::BuildRestoreErrInfo(std::string &jsonStr, std::map<std::string, std::vector<int>> errFileInfo)
{
    cJSON *errJson = cJSON_CreateObject();
    if (errJson == nullptr) {
        HILOGE("Creat json failed");
        return false;
    }
    cJSON *arrJson = cJSON_CreateArray();
    if (arrJson == nullptr) {
        cJSON_Delete(errJson);
        return false;
    }
    for (const auto &it : errFileInfo) {
        for (const auto &codeIt : it.second) {
            cJSON *eleJson = cJSON_CreateObject();
            if (eleJson == nullptr) {
                HILOGE("Creat eleJson failed");
                continue;
            }
            cJSON_AddStringToObject(eleJson, "type", "ErrorInfo");
            cJSON_AddStringToObject(eleJson, "errorInfo", it.first.c_str());
            cJSON_AddNumberToObject(eleJson, "errorCode", codeIt);
            cJSON_AddItemToArray(arrJson, eleJson);
        }
    }
    cJSON_AddItemToObject(errJson, "resultInfo", arrJson);
    char *data = cJSON_Print(errJson);
    if (data == nullptr) {
        cJSON_Delete(errJson);
        return false;
    }
    jsonStr = std::string(data);
    cJSON_Delete(errJson);
    cJSON_free(data);
    return true;
}
}