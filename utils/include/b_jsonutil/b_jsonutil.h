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
        int32_t userId;
    }BundleDetailInfo;

    /**
     * @brief 带有拼接字符的bundleName按照拼接字符进行分割
     *
     * @param bundleNameStr bundleName拼接index的字符串
     * @param patternInfo 拼接字符串
     *
     * @return 分割好的结果赋值给结构体
     */
    static BundleDetailInfo ParseBundleNameIndexStr (const std::string &bundleNameStr, const std::string &patternInfo);

    /**
     * @brief 将传进来的bundleNames的集合进行按照拼接字符分割处理
     *
     * @param bundleNames bundleName拼接index的字符串集合
     * @param details infos的集合
     * @param patternInfo 拼接的字符
     * @param realBundleNames 分割后真正的bundleNames
     * @param userId userId
     *
     * @return 包名和解析结果的对应关系集合
     *
     */
    static std::map<std::string, std::vector<BundleDetailInfo>> BuildBundleInfos(
        const std::vector<std::string> &bundleNames, const std::vector<std::string> &details,
        std::vector<std::string> &realBundleNames, int32_t userId);

    /**
     * @brief 解析单个bundle对应的json串
     *
     * @param bundleDetailInfo json串
     * @param bundleDetail 结构体对象
     * @param bundleNameOnly bundle名称
     * @param bundleIndex bundle对应的索引
     * @param userId userId
     *
     */
    static void ParseBundleInfoJson(const std::string &bundleInfo, std::vector<BundleDetailInfo> &bundleDetails,
        std::string &bundleNameOnly, int bundleIndex, int32_t userId);

    /**
     * @brief 根据业务类型和bundleName确定唯一的bundleInfo
     *
     * @param bundleNameDetailsMap 包名和当前包扩展信息解析结果的集合
     * @param bundleName 包名
     * @param jobType 业务类型broadcast或者unicast
     * @param bundleDetail 确定下来的bundleInfo
     *
     * @return 是否获取到
     *
     */
    static bool FindBundleInfoByName(std::map<std::string, std::vector<BundleDetailInfo>> &bundleNameDetailsMap,
        std::string &bundleName, const std::string &jobType, BundleDetailInfo &bundleDetail);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_JSON_UTIL_H