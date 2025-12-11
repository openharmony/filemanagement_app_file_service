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

#include "b_jsonutil_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BJsonUtil::BundleDetailInfo BJsonUtil::ParseBundleNameIndexStr(const std::string &bundleNameStr)
{
    return BBJsonUtil::jsonUtil->ParseBundleNameIndexStr(bundleNameStr);
}

std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> BJsonUtil::BuildBundleInfos(
    const std::vector<std::string> &bundleNames, const std::vector<std::string> &bundleInfos,
    std::vector<std::string> &bundleNamesOnly, int32_t userId,
    std::map<std::string, bool> &isClearDataFlags)
{
    return BBJsonUtil::jsonUtil->BuildBundleInfos(bundleNames, bundleInfos, bundleNamesOnly, userId, isClearDataFlags);
}

void BJsonUtil::ParseBundleInfoJson(const std::string &bundleInfo, std::vector<BundleDetailInfo> &bundleDetails,
    BJsonUtil::BundleDetailInfo bundleDetailInfo, bool &isClearData, int32_t userId)
{
    BBJsonUtil::jsonUtil->ParseBundleInfoJson(bundleInfo, bundleDetails, bundleDetailInfo, isClearData, userId);
}

bool BJsonUtil::FindBundleInfoByName(std::map<std::string, std::vector<BundleDetailInfo>> &bundleNameDetailsMap,
    std::string &bundleName, const std::string &jobType, BundleDetailInfo &bundleDetail)
{
    return BBJsonUtil::jsonUtil->FindBundleInfoByName(bundleNameDetailsMap, bundleName, jobType, bundleDetail);
}

bool BJsonUtil::BuildExtensionErrInfo(std::string &jsonStr, int errCode, std::string errMsg)
{
    return BBJsonUtil::jsonUtil->BuildExtensionErrInfo(jsonStr, errCode, errMsg);
}

std::string BJsonUtil::BuildBundleNameIndexInfo(const std::string &bundleName, int appIndex)
{
    return BBJsonUtil::jsonUtil->BuildBundleNameIndexInfo(bundleName, appIndex);
}

bool BJsonUtil::BuildExtensionErrInfo(std::string &jsonStr, std::map<std::string, std::vector<int>> errFileInfo)
{
    return BBJsonUtil::jsonUtil->BuildExtensionErrInfo(jsonStr, errFileInfo);
}

bool BJsonUtil::BuildOnProcessRetInfo(std::string &jsonStr, std::string onProcessRet)
{
    return BBJsonUtil::jsonUtil->BuildOnProcessRetInfo(jsonStr, onProcessRet);
}

bool BJsonUtil::BuildOnProcessErrInfo(std::string &reportInfo, std::string path, int err)
{
    return BBJsonUtil::jsonUtil->BuildOnProcessErrInfo(reportInfo, path, err);
}

bool BJsonUtil::BuildBundleInfoJson(int32_t userId, string &detailInfo)
{
    return BBJsonUtil::jsonUtil->BuildBundleInfoJson(userId, detailInfo);
}

std::string BJsonUtil::BuildInitSessionErrInfo(int32_t userId, string callerName, string activeTime, string scenario)
{
    return BBJsonUtil::jsonUtil->BuildInitSessionErrInfo(userId, callerName, activeTime, scenario);
}

bool BJsonUtil::WriteToStr(std::vector<BundleDataSize> &bundleDataList, size_t listSize, std::string scanning,
    std::string &jsonStr)
{
    return BBJsonUtil::jsonUtil->WriteToStr(bundleDataList, listSize, scanning, jsonStr);
}
}