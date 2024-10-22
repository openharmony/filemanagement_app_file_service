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
#include <chrono>
#include <iomanip>
#include <sstream>
#include "cJSON.h"

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "b_utils/b_time.h"

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
    std::string indexStr = "";
    if (to_string(bundleNameStr.back()) != BUNDLE_INDEX_SPLICE) {
        indexStr = bundleNameStr.substr(hasPos + 1);
        int index = std::stoi(indexStr);
        bundleDetailInfo.bundleIndex = index;
    } else {
        bundleDetailInfo.bundleIndex =  BUNDLE_INDEX_DEFAULT_VAL;
    }
    bundleDetailInfo.bundleName = bundleName;
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
            std::string indexSplit = "";
            if (to_string(bundleName.back()) != BUNDLE_INDEX_SPLICE) {
                indexSplit = bundleName.substr(pos + 1);
                int index = std::stoi(indexSplit);
                bundleIndex = index;
            } else {
                bundleIndex = BUNDLE_INDEX_DEFAULT_VAL;
            }
            bundleNameOnly = bundleNameSplit;
            bundleNamesOnly.emplace_back(bundleNameSplit);
        }
        std::string bundleInfo = bundleInfos[i];
        bool isClearData = true;
        BJsonUtil::BundleDetailInfo bundleDetailInfo;
        bundleDetailInfo.bundleName = bundleNameOnly;
        bundleDetailInfo.bundleIndex = bundleIndex;
        bundleDetailInfo.userId = userId;
        ParseBundleInfoJson(bundleInfo, bundleDetailInfos, bundleDetailInfo, isClearData, userId);
        isClearDataFlags[bundleName] = isClearData;
        bundleNameDetailMap[bundleName] = bundleDetailInfos;
    }
    HILOGI("End BuildBundleInfos");
    return bundleNameDetailMap;
}

// 传递的bundleinfo不包含unicast字段时 需要拼接unicast字段
static bool AddUnicastInfo(std::string &bundleInfo)
{
    cJSON *root = cJSON_Parse(bundleInfo.c_str());
    if (root == nullptr) {
        HILOGE("Parse json error,root is null");
        return false;
    }
    cJSON *info = cJSON_CreateObject();
    if (info == nullptr) {
        cJSON_Delete(root);
        return false;
    }
    cJSON_AddStringToObject(info, "type", "unicast");
    cJSON *details = cJSON_CreateArray();
    if (details == nullptr) {
        cJSON_Delete(root);
        cJSON_Delete(info);
        return false;
    }
    cJSON_AddItemToArray(details, {});
    cJSON_AddItemToObject(info, "details", details);
    cJSON *infos = cJSON_GetObjectItem(root, "infos");
    if (infos == nullptr || !cJSON_IsArray(infos)) {
        cJSON_Delete(root);
        cJSON_Delete(info);
        return false;
    }
    cJSON_AddItemToArray(infos, info);
    char *jsonStr = cJSON_Print(root);
    if (jsonStr == nullptr) {
        cJSON_Delete(root);
        return false;
    }
    bundleInfo = string(jsonStr);
    cJSON_Delete(root);
    free(jsonStr);
    return true;
}

bool BJsonUtil::HasUnicastInfo(std::string &bundleInfo)
{
    cJSON *root = cJSON_Parse(bundleInfo.c_str());
    if (root == nullptr) {
        HILOGE("Parse json error,root is null");
        return false;
    }
    cJSON *infos = cJSON_GetObjectItem(root, "infos");
    if (infos == nullptr || !cJSON_IsArray(infos) || cJSON_GetArraySize(infos) == 0) {
        HILOGE("Parse json error, infos is not array");
        cJSON_Delete(root);
        return false;
    }
    int infosCount = cJSON_GetArraySize(infos);
    for (int i = 0; i < infosCount; i++) {
        cJSON *infoItem = cJSON_GetArrayItem(infos, i);
        if (!cJSON_IsObject(infoItem)) {
            HILOGE("Parse json error, info item is not an object");
            continue;
        }
        cJSON *type = cJSON_GetObjectItem(infoItem, "type");
        if (type == nullptr || !cJSON_IsString(type) || (type->valuestring == nullptr)) {
            HILOGE("Parse json type element error");
            continue;
        }
        if (string(type->valuestring).compare(BConstants::UNICAST_TYPE) == 0) {
            cJSON_Delete(root);
            return true;
        }
    }
    cJSON_Delete(root);
    return false;
}

static void InsertBundleDetailInfo(cJSON *infos, int infosCount,
                                   std::vector<BJsonUtil::BundleDetailInfo> &bundleDetails,
                                   BJsonUtil::BundleDetailInfo bundleDetailInfo,
                                   int32_t userId)
{
    for (int i = 0; i < infosCount; i++) {
        cJSON *infoItem = cJSON_GetArrayItem(infos, i);
        if (!cJSON_IsObject(infoItem)) {
            HILOGE("Parse json error, info item is not an object");
            return;
        }
        cJSON *type = cJSON_GetObjectItem(infoItem, "type");
        if (type == nullptr || !cJSON_IsString(type) || (type->valuestring == nullptr)) {
            HILOGE("Parse json type element error");
            return;
        }
        bundleDetailInfo.type = type->valuestring;
        cJSON *details = cJSON_GetObjectItem(infoItem, "details");
        if (details == nullptr || !cJSON_IsArray(details)) {
            HILOGE("Parse json details element error");
            return;
        }
        if (bundleDetailInfo.type.compare(BConstants::UNICAST_TYPE) == 0) {
            cJSON *detail = cJSON_CreateObject();
            if (detail == nullptr) {
                HILOGE("creat json error");
                return;
            }
            string userIdstr = to_string(userId);
            const char *const zeroUserId = userIdstr.c_str();
            cJSON_AddStringToObject(detail, "type", "userId");
            cJSON_AddStringToObject(detail, "detail", zeroUserId);
            cJSON_AddItemToArray(details, detail);
        }
        char *detailInfos = cJSON_Print(details);
        bundleDetailInfo.detail = std::string(detailInfos);
        bundleDetails.emplace_back(bundleDetailInfo);
        cJSON_free(detailInfos);
    }
}

void BJsonUtil::ParseBundleInfoJson(const std::string &bundleInfo, std::vector<BundleDetailInfo> &bundleDetails,
    BJsonUtil::BundleDetailInfo bundleDetailInfo, bool &isClearData, int32_t userId)
{
    string bundleInfoCopy = move(bundleInfo);
    if (!HasUnicastInfo(bundleInfoCopy)) {
        if (!AddUnicastInfo(bundleInfoCopy)) {
            HILOGE("AddUnicastInfo failed");
            return;
        }
    }
    cJSON *root = cJSON_Parse(bundleInfoCopy.c_str());
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
    InsertBundleDetailInfo(infos, infosCount, bundleDetails, bundleDetailInfo, userId);
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

bool BJsonUtil::BuildExtensionErrInfo(std::string &jsonStr, int errCode, std::string errMsg)
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

bool BJsonUtil::BuildExtensionErrInfo(std::string &jsonStr, std::map<std::string, std::vector<int>> errFileInfo)
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

bool OHOS::FileManagement::Backup::BJsonUtil::BuildOnProcessRetInfo(std::string &jsonStr, std::string onProcessRet)
{
    cJSON *info = cJSON_CreateObject();
    if (info == nullptr) {
        return false;
    }
    cJSON *processInfo = cJSON_CreateObject();
    if (processInfo == nullptr) {
        cJSON_Delete(info);
        return false;
    }
    std::string timeInfo = std::to_string(TimeUtils::GetTimeS());
    cJSON_AddStringToObject(processInfo, "timeInfo", timeInfo.c_str());
    cJSON_AddStringToObject(processInfo, "resultInfo", onProcessRet.c_str());
    cJSON_AddItemToObject(info, "processResult", processInfo);
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

bool OHOS::FileManagement::Backup::BJsonUtil::BuildOnProcessErrInfo(std::string &reportInfo, std::string path, int err)
{
    cJSON *info = cJSON_CreateObject();
    if (info == nullptr) {
        return false;
    }
    cJSON *item = cJSON_CreateObject();
    if (item == nullptr) {
        cJSON_Delete(info);
        return false;
    }
    cJSON *errInfoJs = cJSON_CreateObject();
    if (errInfoJs == nullptr) {
        cJSON_Delete(info);
        cJSON_Delete(item);
        return false;
    }
    std::string errStr = std::to_string(err);
    std::string timeInfo = std::to_string(TimeUtils::GetTimeS());
    cJSON_AddStringToObject(errInfoJs, "errorCode", errStr.c_str());
    cJSON_AddStringToObject(errInfoJs, "errorMessage", path.c_str());
    cJSON_AddStringToObject(item, "timeInfo", timeInfo.c_str());
    cJSON_AddItemToObject(item, "errorInfo", errInfoJs);
    cJSON_AddItemToObject(info, "processResult", item);
    char *data = cJSON_Print(info);
    if (data == nullptr) {
        cJSON_Delete(info);
        return false;
    }
    reportInfo = std::string(data);
    cJSON_Delete(info);
    cJSON_free(data);
    return true;
}

bool OHOS::FileManagement::Backup::BJsonUtil::BuildBundleInfoJson(int32_t userId, string &detailInfo)
{
    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        return false;
    }
    cJSON *infos = cJSON_CreateArray();
    if (infos == nullptr) {
        cJSON_Delete(root);
        return false;
    }
    cJSON_AddItemToObject(root, "infos", infos);
    cJSON *info = cJSON_CreateObject();
    if (info == nullptr) {
        cJSON_Delete(root);
        return false;
    }
    cJSON_AddStringToObject(info, "type", "unicast");
    cJSON_AddItemToArray(infos, info);
    cJSON *details = cJSON_CreateArray();
    if (details == nullptr) {
        cJSON_Delete(root);
        return false;
    }
    cJSON_AddItemToObject(info, "details", details);
    cJSON *detail = cJSON_CreateObject();
    if (detail == nullptr) {
        cJSON_Delete(root);
        return false;
    }
    string userIdstr = to_string(userId);
    const char *const zeroUserId = userIdstr.c_str();
    cJSON_AddStringToObject(detail, "type", "userId");
    cJSON_AddStringToObject(detail, "detail", zeroUserId);
    cJSON_AddItemToArray(details, detail);
    char *jsonStr = cJSON_Print(root);
    if (jsonStr == nullptr) {
        cJSON_Delete(root);
        return false;
    }
    detailInfo = string(jsonStr);
    cJSON_Delete(root);
    free(jsonStr);
    return true;
}
}
