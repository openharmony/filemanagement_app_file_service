/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_B_JSONUTIL_MOCK_MOCK_H
#define OHOS_FILEMGMT_BACKUP_B_JSONUTIL_MOCK_MOCK_H

#include <gmock/gmock.h>

#include "b_jsonutil/b_jsonutil.h"

namespace OHOS::FileManagement::Backup {
class BBJsonUtil {
public:
    virtual BJsonUtil::BundleDetailInfo ParseBundleNameIndexStr (const std::string&) = 0;
    virtual std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> BuildBundleInfos(
        const std::vector<std::string>&, const std::vector<std::string>&, std::vector<std::string>&, int32_t,
        std::map<std::string, bool>&) = 0;
    virtual void ParseBundleInfoJson(const std::string&, std::vector<BJsonUtil::BundleDetailInfo>&,
        BJsonUtil::BundleDetailInfo, bool&, int32_t) = 0;
    virtual bool FindBundleInfoByName(std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>>&, std::string&,
        const std::string&, BJsonUtil::BundleDetailInfo&) = 0;
    virtual bool BuildExtensionErrInfo(std::string&, int, std::string) = 0;
    virtual std::string BuildBundleNameIndexInfo(const std::string&, int) = 0;
    virtual bool BuildExtensionErrInfo(std::string&, std::map<std::string, std::vector<int>>) = 0;
    virtual bool BuildOnProcessRetInfo(std::string&, std::string) = 0;
    virtual bool BuildOnProcessErrInfo(std::string&, std::string, int) = 0;
    virtual bool BuildBundleInfoJson(int32_t, std::string&) = 0;
    virtual bool HasUnicastInfo(std::string&) = 0;
    virtual std::string BuildInitSessionErrInfo(int32_t, std::string, std::string) = 0;
    virtual bool WriteToStr(std::vector<BJsonUtil::BundleDataSize>&, size_t, std::string, std::string&) = 0;
public:
    BBJsonUtil() = default;
    virtual ~BBJsonUtil() = default;
public:
    static inline std::shared_ptr<BBJsonUtil> jsonUtil = nullptr;
};

class BJsonUtilMock : public BBJsonUtil {
public:
    MOCK_METHOD(BJsonUtil::BundleDetailInfo, ParseBundleNameIndexStr, (const std::string&));
    MOCK_METHOD((std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>>), BuildBundleInfos,
        ((const std::vector<std::string>&), (const std::vector<std::string>&), (std::vector<std::string>&), int32_t,
        (std::map<std::string, bool>&)));
    MOCK_METHOD(void, ParseBundleInfoJson, (const std::string&, (std::vector<BJsonUtil::BundleDetailInfo>&),
        BJsonUtil::BundleDetailInfo, bool&, int32_t));
    MOCK_METHOD(bool, FindBundleInfoByName, ((std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>>&),
        std::string&, const std::string&, BJsonUtil::BundleDetailInfo&));
    MOCK_METHOD(bool, BuildExtensionErrInfo, (std::string&, int, std::string));
    MOCK_METHOD(std::string, BuildBundleNameIndexInfo, (const std::string&, int));
    MOCK_METHOD(bool, BuildExtensionErrInfo, (std::string&, (std::map<std::string, std::vector<int>>)));
    MOCK_METHOD(bool, BuildOnProcessRetInfo, (std::string&, std::string));
    MOCK_METHOD(bool, BuildOnProcessErrInfo, (std::string&, std::string, int));
    MOCK_METHOD(bool, BuildBundleInfoJson, (int32_t, std::string&));
    MOCK_METHOD(bool, HasUnicastInfo, (std::string&));
    MOCK_METHOD(std::string, BuildInitSessionErrInfo, (int32_t, std::string, std::string));
    MOCK_METHOD(bool, WriteToStr, (std::vector<BJsonUtil::BundleDataSize>&, size_t, std::string, std::string&));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_B_JSONUTIL_MOCK_MOCK_H
