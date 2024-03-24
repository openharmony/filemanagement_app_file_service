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

#ifndef OHOS_FILEMGMT_BACKUP_JSON_UTIL_H
#define OHOS_FILEMGMT_BACKUP_JSON_UTIL_H

#include <string>
#include <map>

namespace OHOS::FileManagement::Backup {
class BJsonUtil {
public:
    typedef struct BundleDetailInfo {
        std::string bundleName;
        std::string type;
        std::string detail;
        int bundleIndex;
    }BundleDetailInfo;
    /**
     * @brief 带有拼接字符的bundleName按照拼接字符进行分割
     *
     * @param bundleNameStr bundleName拼接index的字符串
     * @param patternInfo 拼接字符串
     *
     * @return 分割好的结果赋值给结构体
     */
    static BundleDetailInfo ParseBundleNameIndexStr(const std::string &bundleNameStr, const std::string &patternInfo);

    /**
     * @brief 将传进来的bundleNames的集合进行按照拼接字符分割处理
     *
     * @param bundleNames bundleName拼接index的字符串集合
     * @param details infos的集合
     * @param patternInfo 拼接的字符
     * @param realBundleNames 分割后真正的bundleNames
     *
     * @return 结构体集合
     *
     */
    static std::vector<BundleDetailInfo> ConvertBundleDetailInfos(const std::vector<std::string> &bundleNames,
        const std::vector<std::string> &details, const std::string &patternInfo,
        std::vector<std::string> &realBundleNames);

    /**
     * @brief 解析单个bundle对应的json串
     *
     * @param bundleDetailInfo json串
     * @param bundleDetail 结构体对象
     *
     */
    static void ParseBundleDetailInfo(const std::string &bundleDetailInfo, BundleDetailInfo &bundleDetail);


    /**
     * @brief 根据bundleName获取对应的detailInfo
     *
     * @param bundleNameDetailMap bundleName和detail的对应关系
     * @param bundleDetailInfo 结构体对象
     *
    */
    static void RecordBundleDetailRelation(std::map<std::string, BundleDetailInfo> &bundleNameDetailMap,
        std::vector<BundleDetailInfo> &bundleDetailInfo);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_BACKUP_PARA_H